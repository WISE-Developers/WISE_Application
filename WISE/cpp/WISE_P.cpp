/**
 * WISE_Grid_Module: WISE_P.cpp
 * Copyright (C) 2023  WISE
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#if _DLL
#include "stdafx.h"
#endif
#include "types.h"
#include "WISE_P.h"
#include "WISE.h"
#include "COMInit.h"
#if _DLL
#include "cwinapp.h"
#else
#include <pthread.h>
#endif
#include "prom_vers.h"
#include "gdalclient.h"
#include "misc_native.h"
#include "REDappWrapper.h"
#include "out_helper.h"
#include <gdal.h>
#ifdef INCLUDE_LOCAL_LIBRARIES
#include "hss_vers.h"
#endif

#include <iostream>
#include "filesystem.hpp"
#include <boost/io/ios_state.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <bitset>


// The one and only application object

#if _DLL
CWinAppExt theApp;
#endif


using namespace std;

namespace po = boost::program_options;


void printUsage(const po::options_description& options)
{
	std::cout << "WISE version " VER_PROMETHEUS_VERSION_FULL_STRING << std::endl;
	std::cout << options << std::endl;
}


void GuessProcessorGroup(std::int32_t coreCount, std::int32_t coreOffset, const std::string& coreMask)
{
	std::string maskToUse(coreMask);
	if (maskToUse.length() == 0)
    {
		if (coreOffset > 0)
			maskToUse.append(coreOffset, '0');
		maskToUse.append(coreCount, '1');
    }

	std::vector<std::int32_t> groups;
	std::map<std::int32_t, std::uint64_t> masks;
#ifdef __GNUC__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
#endif
	std::uint32_t jb = 0;
	std::uint32_t numnuma = CWorkerThreadPool::NumberNUMA();
	for (std::uint32_t i = 0; i < numnuma; i++)
	{
		std::uint32_t count = CWorkerThreadPool::NumberProcessors(i);

		for (int j = 0; j < count; j++)
		{
			if ((j + jb) < maskToUse.length())
			{
				if (maskToUse[j + jb] == '1')
				{
					if (masks.find(i) == masks.end())
                    {
						groups.push_back(i);
						std::uint64_t groupMask{ 0 };
						for (int k = 0; k < count && k < coreCount; k++)
                        {
							groupMask |= ((std::uint64_t)1) << (k + j);
#ifdef __GNUC__
                            CPU_SET(k, &cpuset);
#endif
                        }
						masks[i] = groupMask;
					}
				}
            }
        }

		jb += count;
	}

	if (groups.size() == 1)
	{
#ifdef _MSC_VER
		GROUP_AFFINITY ga;
		GetNumaNodeProcessorMaskEx(groups[0], &ga);
		GROUP_AFFINITY ga2;
		int offset = 0;
		for (int i = 0; i < groups[0]; i++) {
			GetNumaNodeProcessorMaskEx(i, &ga2);
			if (ga2.Group == ga.Group)
				offset += std::bitset<sizeof(KAFFINITY)* CHAR_BIT>(ga2.Mask).count();
		}
		//		std::cout << "NUMA node " << groups[0] << " is in group " << ga.Group << " and offset " << offset << std::endl;
		ga.Mask = masks[groups[0]] << offset;
		ga.Reserved[0] = ga.Reserved[1] = ga.Reserved[2] = 0;
		BOOL retval = SetThreadGroupAffinity(GetCurrentThread(), &ga, NULL);
		if (!retval)
		{
			std::cout << "Failed to set processor affinity with error code " << GetLastError() << std::endl;
		}
#else
        pthread_setaffinity_np(CWorkerThreadPool::GetCurrentThread(), sizeof(cpu_set_t), &cpuset);
#endif
	}
}


std::string getJavaOverridePath()
{
#ifdef _MSC_VER
	HKEY hk;
	std::string key = TEXT("Software\\CWFGM Project Steering Committee\\Prometheus\\Configuration Options");
	LSTATUS n = RegOpenKeyEx(HKEY_CURRENT_USER, key.c_str(), NULL, KEY_READ, &hk);
	if (n == 0)
	{
		DWORD dwSize = 0;
		std::string str;
		n = RegGetValue(hk, nullptr, TEXT("JAVA_HOME"), RRF_RT_REG_SZ, nullptr, nullptr, &dwSize);
		str.resize(dwSize);
		n = RegGetValue(hk, nullptr, TEXT("JAVA_HOME"), RRF_RT_REG_SZ, nullptr, &str[0], &dwSize);
		RegCloseKey(hk);
		str.erase(std::remove(str.begin(), str.end(), '\0'), str.end());
		return str;
	}
#endif

	return "";
}


#ifdef _MSC_VER
int _tmain(int argc, TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	int nRetCode = 1;

#ifdef _MSC_VER
	HMODULE hModule = ::GetModuleHandle(NULL);
#else
    bool hModule = true;
#endif

	std::recursive_mutex m_GDALsemaphore;

	std::string password;
	std::string newPassword;
	std::string format;
	std::uint32_t formatVersion = 0;
	std::string managerLoc;
	std::int32_t coreCount = -1;
	std::int32_t coreOffset = -1;
	std::string coreMask;
	std::string validationFileName;
	std::string loadBalanceIndex;
	//public command line arguments
	po::options_description desc("usage: WISE [OPTIONS]... [FILENAME]\nW.I.S.E. supports multiple file formats with different extensions:\n"
		"\tFGMJ: W.I.S.E. inputs stored in JSON format\n"
		"\tFGMB: W.I.S.E. inputs stored in binary format"
	);
	desc.add_options()
		("help,h", "print this message")
		("version,v", "print the W.I.S.E. version")
		("include-shared-memory,i", "include shared memory block recording used cores")
		("output,o", po::value(&format), "optional output for the conversion function. Must use a supported file extension. If the same file extension is specified as the input type no conversion will be carried out")
		("convert,c", "convert the input file to a different format, the job will not be run. Both the input and output files must use one of the supported file extensions")
		("convertversion,V", po::value(&formatVersion), "if converting, then this specifies the version of the FGMJ output file")
		("summary,u", "output summary files for the scenario(s) in the job file in TXT format then quit. A summary file will be created for all scenarios but none will be run")
		("jsummary,j", "output summary files for the scenario(s) in the job file in JSON format then quit. A summary file will be created for all scenarios but none will be run")
		("bsummary,b", "output summary files for the scenario(s) in the job file in binary format then quit. A summary file will be created for all scenarios but none will be run")
		("sysinfo,s", "prints system information (cores, memory, etc.) then quits")
		("password,p", po::value(&password), "an optional password for the job file")
		("new-password,n", po::value(&newPassword), "an optional password for the converted job file")
		("manager,m", po::value(&managerLoc), "an optional W.I.S.E. Manager ID for W.I.S.E. to communicate with")
		("stats,t", "output run statistics including start time and peak memory usage")
		("cores,r", po::value(&coreCount), "an optional override for the number of cores to use to run the job. Not to be used at the same time as mask. cores and offset combined will define which cores the process is locked to")
		("offset,f", po::value(&coreOffset), "an optional offset into the existing cores (disregarding NUMA nodes and processor groups) to lock the cores that will run the job. Not to be used at the same time as mask. cores and offset combined will define which cores the process is locked to")
		("mask,a", po::value(&coreMask), "an optional bit mask to specify which cores should be used to run the job. Not to be used at the same time as offset. The maximum number of bits that will be used is the same as the number of cores in the system, extra bits will be ignored")
		("validate", "validate the FGM but don't run it. The results will be written to validation.json unless a different file is specified using the `vn` option")
		("vn", po::value(&validationFileName)->default_value("validation.json"), "an optional name for the exported validation file. Defaults to validation.json")
		("balance,l", po::value(&loadBalanceIndex)->default_value(""), "select which scenario indices should be run if multiple are available. Will override any load balancing specified in the file");

	//internal and hidden command line arguments
	po::options_description intern("Internal options");
	intern.add_options()
		("input-file", "the job file to load")
		("v2", "print the W.I.S.E. version without any description")
		("mem-dump", "dump the contents of the shared memory block")
		("verbose-version", "Test the setup and quit");

	//the filename doesn't need an explicit name, just add it to the end of the command line
	po::positional_options_description positional;
	positional.add("input-file", 1);

	//combine the public and internal options
	po::options_description options;
	options.add(desc).add(intern);

	//parse the command line
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv)
		.options(options)
		.positional(positional)
		.allow_unregistered()
		.run(), vm);
	po::notify(vm);

	//if the cores to use were set on the command line try to lock to those cores
	if (coreCount > 0 || std::count(coreMask.begin(), coreMask.end(), '1') > 0)
		GuessProcessorGroup(coreCount, coreOffset, coreMask);

	hss::InitializeCOM(-1, hss::INIT_WITH_SECURITY);
#if _DLL
	theApp.InitInstance();	// ***** don't know if this is needed!
#endif
	GDALClient::GDALClient::RegisterGDAL();
	GDALClient::GDALClient::setGDALMutex(&m_GDALsemaphore);
	SPARCS_P::StaticInitialization();

	WorldLocation currentLocation;
	auto systemTimeManager = HSS_Time::WTimeManager::GetSystemTimeManager(currentLocation);
	std::string startTime = HSS_Time::WTime::Now(&systemTimeManager, 0).ToString(WTIME_FORMAT_STRING_ISO8601);

	std::string javaOverridePath = getJavaOverridePath();
	if (javaOverridePath.size())
		REDapp::REDappWrapper::SetPathOverride(javaOverridePath);

	bool useShmem = vm.count("include-shared-memory") > 0;

	//output the version number
	if (vm.count("v2"))
	{
		std::cout << VER_PROMETHEUS_VERSION_FULL_STRING << std::endl;
		nRetCode = 0;
	}
	//output the version number in a human readable format
	else if (vm.count("version"))
	{
		std::cout << "W.I.S.E. version " VER_PROMETHEUS_VERSION_FULL_STRING << std::endl;
		std::cout << "Released " VER_RELEASE_DATE << std::endl;
		nRetCode = 0;
	}
	//output the help message
	else if (vm.count("help"))
	{
		printUsage(desc);
		nRetCode = 0;
	}
	else if (vm.count("verbose-version"))
	{
		std::cout << "Testing W.I.S.E. setup" << std::endl;
		std::cout << "Version: " VER_PROMETHEUS_VERSION_FULL_STRING << std::endl;
		std::cout << "Released: " VER_RELEASE_DATE << std::endl;
		std::string m_path;
#ifdef _MSC_VER
		std::vector<TCHAR> path;
		DWORD copied = 0;
		do
		{
			path.resize(path.size() + MAX_PATH);
			copied = ::GetModuleFileName(NULL, &path.at(0), path.size());
		} while (copied >= path.size());

		m_path = fs::path(&path.at(0)).string();
#else
		std::array<char, 512> buffer{};
		size_t len = readlink("/proc/self/exe", buffer.data(), 512);
		m_path = fs::path(buffer.data()).string();
#endif
		std::cout << "Running: " << m_path << std::endl;
		std::cout << GDALVersionInfo("--version") << std::endl;
#ifdef INCLUDE_LOCAL_LIBRARIES
		std::cout << "HSS Math: " << VER_MATH_VERSION_STRING2 << std::endl;
		std::cout << "HSS Geography: " << VER_GEOG_VERSION_STRING2 << std::endl;
		std::cout << "WTime: " << VER_TIMES_VERSION_STRING2 << std::endl;
#endif
		std::cout << "Google Protocol Buffers: " << getProtobufVersion() << std::endl;
		std::cout << getPahoVersion() << std::endl;
		std::cout << "Boost: " << (BOOST_VERSION / 100000) << "." << (BOOST_VERSION / 100 % 1000) << "." << (BOOST_VERSION % 100) << std::endl;
		if (!REDapp::REDappWrapper::CanLoadJava())
			std::cout << "Unable to load Java: " << REDapp::REDappWrapper::GetErrorDescription() << std::endl << "Tested Paths:" << std::endl << REDapp::REDappWrapper::GetDetailedError() << std::endl;
        else
            std::cout << "Found Java at " << REDapp::REDappWrapper::GetJavaPath() << std::endl;
		nRetCode = 0;
	}
	else if (vm.count("sysinfo"))
	{
#ifdef _MSC_VER
		DWORD origMode = 0;
		GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &origMode);
		DWORD mode = origMode | ENABLE_ECHO_INPUT;
		SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode);
#endif
		std::cout << CWorkerThreadPool::NumberProcessors() << " logical cores in " << CWorkerThreadPool::NumberNUMA() << " NUMA nodes." << std::endl;
		if (CWorkerThreadPool::NumberNUMA() > 1)
		{
			for (int i = 0; i < CWorkerThreadPool::NumberNUMA(); i++)
			{
				std::cout << "NUMA node " << i << " has " << CWorkerThreadPool::NumberProcessors(i) << " logical cores." << std::endl;
			}
		}
		SPARCS_P sp;
		std::string memoryContents;
		sp.SetupThreads(coreCount, coreOffset, coreMask, useShmem, vm.count("mem-dump") ? &memoryContents : nullptr);

		std::cout << std::endl << out_helper::format_color(out_helper::Color::Ignore, out_helper::Color::Ignore, out_helper::Style::Underline) << "Thread allocation mask" << out_helper::format_color() << std::endl;
        for (std::uint32_t i = 0; i < CWorkerThreadPool::NumberNUMA(); i++)
		{
			std::cout << "Simulation: NUMA Node " << i << ": " << out_helper::format_color(out_helper::Color::Ignore, out_helper::Color::Black, out_helper::Style::Bold);
			auto eventLoop = CWorkerThreadPool::LoadThreadAffinityMask(CWorkerThreadPool::EVENT_LOOP_MASK, i);
			auto group = CWorkerThreadPool::LoadThreadAffinityMask(100000 + i, i);

			for (int j = CWorkerThreadPool::NumberProcessors(i) - 1; j >= 0; j--)
			{
				std::uint64_t position = 1llu << j;
				if ((eventLoop & position))
					std::cout << out_helper::format_color(out_helper::Color::Green) << "E";
				else if ((group & position))
					std::cout << out_helper::format_color(out_helper::Color::Red) << 1;
				else
					std::cout << out_helper::format_color(out_helper::Color::White) << 0;
			}
			std::cout << out_helper::format_color() << std::endl;
		}
		if (memoryContents.size())
			std::cout << memoryContents << std::endl;
		nRetCode = 0;
#ifdef _MSC_VER
		SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), origMode);
#endif
	}
	else
	{
		nRetCode = 0;
		bool isValid = true;
		bool doConvert = false;
		fs::path input;
		fs::path output;

		if (!vm.count("input-file"))
		{
			std::cout << "Missing input file" << std::endl;
			isValid = false;
		}
		else
		{
			input = vm["input-file"].as<std::string>();
			if (vm.count("convert"))
			{
				doConvert = true;

				std::string ext = input.extension().string();
				std::string oext;
				if (format.length() > 0)
				{
					oext = fs::path(format).extension().string();
					output = format;
				}
				if (input == output)
				{
					std::cout << "Cannot write to the input file" << std::endl;
					nRetCode = 1;
				}
				else if (boost::iequals(ext, ".fgmj"))
				{
					if (boost::iequals(oext, ".fgmj"))
					{
					}
					else if (format.length() == 0)
						output = fs::path(input).replace_extension(".fgmb");
				}
				else if (boost::iequals(ext, ".fgmb"))
				{
					if (boost::iequals(oext, ".fgmb"))
					{
					}
					else if (format.length() == 0)
						output = fs::path(input).replace_extension(".fgmj");
				}
				else
				{
					std::cout << "Unsupported input format" << std::endl;
					nRetCode = 1;
				}
			}
			else
			{
				std::string ext = input.extension().string();
				if (!boost::iequals(ext, ".fgmj") && !boost::iequals(ext, ".fgmb"))
				{
					std::cout << "Unsupported input format" << std::endl;
					nRetCode = 1;
				}
			}
		}

		if (nRetCode == 0)
		{
			if (!isValid)
			{
				printUsage(desc);
				nRetCode = 1;
			}
			else
			{
				if (hModule)
				{
#if _DLL
					// initialize MFC and print and error on failure
					if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
					{
						std::cout << "Fatal Error: MFC initialization failed" << std::endl;
						nRetCode = 1;
					}
					else
					{
#endif //_DLL
						SPARCS_P sp;
						fs::path path(input);
						path = fs::absolute(path).parent_path();
						sp.m_client.m_mqtt_job = path.filename().string();
						sp.m_managerId = managerLoc;
						sp.m_connectImmediately = !doConvert;
						time_t now;
						time(&now);
						char timeBuf[sizeof("2000-01-01T00:00:00Z")];
						strftime(timeBuf, sizeof(timeBuf), "%FT%TZ", gmtime(&now));
						sp.m_startTime = timeBuf;

						//see if the user just wants summary files exported
						SummaryType justSummary = SummaryType::NONE;
						if (vm.count("validate") > 0)
							justSummary = SummaryType::VALIDATE;
						else if (vm.count("summary") > 0)
							justSummary = SummaryType::TEXT;
						else if (vm.count("jsummary"))
							justSummary = SummaryType::JSON;
						else if (vm.count("bsummary"))
							justSummary = SummaryType::BINARY;

						if (FAILED(sp.Initialize_SPARCS_P()))
						{
							boost::io::ios_flags_saver ifs(std::cout);
							std::cout << std::hex << sp.last_error << ": " << sp.last_error_msg << std::endl;
							nRetCode = 1;
						}
						else
						{
							sp.SetupThreads(coreCount, coreOffset, coreMask, useShmem);
							HRESULT hr;
							std::uint32_t fileVersion;
							if (FAILED(hr = sp.LoadProtobuf(input.string().c_str(), fileVersion, [&password]() { return password; }, justSummary)))
							{
								boost::io::ios_flags_saver ifs(std::cout);
								std::cout << std::hex << sp.last_error << ": " << sp.last_error_msg << std::endl;
								nRetCode = 1;
								if (justSummary == SummaryType::VALIDATE)
									sp.SaveValidationReport(validationFileName);
							}

							//the file was loaded successfully
							if (nRetCode == 0)
							{
								//change the file format
								if (doConvert)
								{
									if ((formatVersion == 1) || (formatVersion == 2))
										fileVersion = formatVersion;
									if (FAILED(sp.SaveProtobuf(output.string().c_str(), fileVersion, [&newPassword]() { return newPassword; })))
									{
										boost::io::ios_flags_saver ifs(std::cout);
										std::cout << std::hex << sp.last_error << ": " << sp.last_error_msg << std::endl;
										nRetCode = 1;
									}
								}
								//run the job
								else
								{
									std::vector<std::int32_t> runIndices;
									//scenario indices to run were specified on the command line
									if (loadBalanceIndex.length())
									{
										std::size_t prev = 0, pos;
										while ((pos = loadBalanceIndex.find_first_of(",;:", prev)) != std::string::npos)
										{
											if (pos > prev)
												runIndices.push_back(std::stoi(loadBalanceIndex.substr(prev, pos - prev)));
											prev = pos + 1;
										}
										if (prev < loadBalanceIndex.length())
											runIndices.push_back(std::stoi(loadBalanceIndex.substr(prev, std::string::npos)));
										sp.addLoadBalancingIndices(runIndices);
									}

									std::string cmd;

#ifdef WIN32
									cmd = sp.m_project->m_preExecutionCommand_Windows;
#else
									cmd = sp.m_project->m_preExecutionCommand_Linux;
#endif
									if (!cmd.empty()) {
										std::system(cmd.c_str());
									}

									if (FAILED(sp.RunSimulations(justSummary)))
									{
										boost::io::ios_flags_saver ifs(std::cout);
										std::cout << std::hex << sp.last_error << ": " << sp.last_error_msg << std::endl;
										nRetCode = 1;
									}
									else if (justSummary == SummaryType::VALIDATE)
									{
										sp.SaveValidationReport(validationFileName);
									}
									else if (justSummary == SummaryType::NONE)
									{
										sp.SaveModifiedProtofbuf(input.string(), fileVersion, [&password]() { return password; });
									}

#ifdef WIN32
									cmd = sp.m_project->m_postExecutionCommand_Windows;
#else
									cmd = sp.m_project->m_postExecutionCommand_Linux;
#endif
									if (!cmd.empty()) {
										std::system(cmd.c_str());
									}

									if (vm.count("stats"))
									{
										std::string endTime = HSS_Time::WTime::Now(&systemTimeManager, 0).ToString(WTIME_FORMAT_STRING_ISO8601);
										auto memory = CWorkerThreadPool::GetPeakMemoryUsage();

										auto pwd = fs::current_path() / "statistics.txt";
										std::ofstream outFile(pwd);

										outFile << "{" << std::endl;
										outFile << "    \"start_time\": \"" << startTime << "\"," << std::endl;
										outFile << "    \"end_time\": \"" << endTime << "\"," << std::endl;
										outFile << "    \"peak_memory\": " << memory << std::endl;
										outFile << "}" << std::endl;

										outFile.close();
									}
								}
							}
						}
#ifdef _MSC_VER
					}
#endif //_MSC_VER
				}
			}
		}
	}

	SPARCS_P::StaticDeconstruction();
	GDALClient::GDALClient::CleanupGDAL();
#if _DLL
	theApp.ExitInstance();
#endif

	return nRetCode;
}
