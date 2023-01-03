/**
 * WISE_Project: zip_helper.cpp
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

#include "zip_helper.h"

#include <filesystem>
#include <random>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <minizip/unzip.h>
#include <minizip/zip.h>
#include <ogrsf_frmts.h>
#include <iostream>
#include <chrono>


constexpr std::size_t BUFFER_SIZE = 4096;
namespace fs = std::filesystem;


zip::file::file(const std::string& name)
	: filename(name)
{
	fs::path p(name);
	//check to see if the LIBKML driver is supported
	bool libkmlExists = kmz_supported();
	//hack for kmz, was going to do all zip files but I think I'll stick to kmz for now
	if (fs::exists(p) && boost::iequals(p.extension().string(), ".kmz") && !libkmlExists)
	{
		//create a unique name for a temporary directory
		auto tempDir = temporary_directory();

		//store the temporary directory, should already be absolute but make sure
		tempPath = fs::absolute(tempDir).string();

		//extract the archive
		if (zip::extract(name, tempPath, true))
			//set the main file to the required kmz filename if the archive was successfully extracted
			mainFile = "doc.kml";
	}
}


zip::file::~file()
{
	try
	{
		if (tempPath.length() && fs::exists(tempPath))
		{
			fs::remove_all(tempPath);
		}
	}
	catch (fs::filesystem_error&) {}
}


zip::file::operator std::string() const
{
	return to_string();
}

const std::string zip::file::to_string() const
{
	if (mainFile.length())
	{
		return (fs::path(tempPath) / mainFile).string();
	}
	return filename;
}


bool zip::extract(const std::string& archive, const std::string& outPath, bool overwrite)
{
	fs::path input(archive);
	if (!fs::exists(input))
		return false;

	fs::path output(outPath);
	fs::create_directories(output);

	//buffers for extraction
	char currentFilename[512];
	void* buffer = operator new(BUFFER_SIZE);
	unz_file_info info;

	//open the archive
	auto mcontext_ = unzOpen(input.string().c_str());
	//open the first file in the archive
	std::int32_t error = unzGoToFirstFile(mcontext_);

	while (error == UNZ_OK)
	{
		//get the name of the current file
		unzGetCurrentFileInfo(mcontext_,
			&info, currentFilename, sizeof(currentFilename), 0, 0, 0, 0);

		//create the output filename
		std::string name = std::string(currentFilename);
		auto outPath = output / name;

		if (overwrite || !fs::exists(outPath))
		{
			//make sure to create any directories that might exist
			fs::create_directories(outPath.parent_path());

			//extract the file
			std::ofstream fout;
			fout.open(outPath, std::ios::binary | std::ios::out);
			unzOpenCurrentFile(mcontext_);
			std::int32_t readCount = 0;
			while ((readCount = unzReadCurrentFile(mcontext_, buffer, sizeof(buffer))) > 0)
			{
				fout.write((char*)buffer, readCount);
			}
			fout.close();
			unzCloseCurrentFile(mcontext_);
		}

		//go to the next file in the archive
		error = unzGoToNextFile(mcontext_);
	}

	unzClose(mcontext_);
	operator delete(buffer);

	return true;
}


static int isLargeFile(const fs::path& p)
{
	auto size = fs::file_size(p);
	return size > 0xffffffff;
}


bool zip::archive(const std::string& fileToArchive, const std::string& archiveName, const std::string& archivePath)
{
	fs::path input(fileToArchive);
	if (!fs::exists(input))
		return false;

	fs::path output(archivePath);
	//no overwrite
	if (fs::exists(output))
		return false;
	//make sure the directory that the archive will be stored in exists
	fs::create_directories(output.parent_path());

	//buffers for archiving
	char currentFilename[512];
	void* buffer = operator new(BUFFER_SIZE);

	//open the archive for writing
	auto mcontext_ = zipOpen64(output.string().c_str(), APPEND_STATUS_CREATE);

	//set the last modified time
	zip_fileinfo zi = { 0 };
	auto modifiedTime = fs::last_write_time(input);
	auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(modifiedTime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
	auto tt = std::chrono::system_clock::to_time_t(sctp);
	//should maybe be localtime
	auto gmt = std::gmtime(&tt);
	zi.tmz_date.tm_sec = gmt->tm_sec;
	zi.tmz_date.tm_min = gmt->tm_min;
	zi.tmz_date.tm_hour = gmt->tm_hour;
	zi.tmz_date.tm_mday = gmt->tm_mday;
	zi.tmz_date.tm_mon = gmt->tm_mon;
	zi.tmz_date.tm_year = gmt->tm_year;

#ifdef Z_DEFLATED
	auto err = zipOpenNewFileInZip64(mcontext_, archiveName.c_str(),
		&zi, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, 9, isLargeFile(input));
#else
	auto err = zipOpenNewFileInZip(mcontext_, archiveName.c_str(),
		&zi, nullptr, 0, nullptr, 0, nullptr, Z_BZIP2ED, 9);
#endif

	if (err == ZIP_OK)
	{
		//open the file to be archived
		std::ifstream in(input, std::ios::binary);

		size_t sizeRead = 0;
		do
		{
			err = ZIP_OK;
			in.read((char*)buffer, BUFFER_SIZE);
			sizeRead = in.gcount();

			//some error checking on the input stream
			if (sizeRead < BUFFER_SIZE && !in.eof() && !in.good())
				err = ZIP_ERRNO;

			if (sizeRead > 0)
				err = zipWriteInFileInZip(mcontext_, buffer, (unsigned int)sizeRead);
		} while ((err == ZIP_OK) && (sizeRead > 0));

		//close the input and output files
		in.close();
		zipCloseFileInZip(mcontext_);
	}

	//close the archive
	zipClose(mcontext_, nullptr);
	//delete the buffer
	operator delete(buffer);

	return err = ZIP_OK;
}


const std::string zip::temporary_directory()
{
	auto tempDir = fs::temp_directory_path();
	std::random_device dev;
	std::mt19937 prng(dev());
	std::uniform_int_distribution<std::uint64_t> rand(0);
	std::stringstream ss;
	ss << std::hex << rand(prng);
	tempDir /= ss.str();

	return tempDir.string();
}


bool zip::kmz_supported()
{
	auto count = OGRGetDriverCount();
	bool libkmlExists = false;
	for (int i = 0; i < count; i++)
	{
		auto driver = OGRGetDriver(i);
		std::string driverName = OGR_Dr_GetName(driver);
		if (boost::iequals(driverName, "LIBKML"))
		{
			libkmlExists = true;
			break;
		}
	}

	return libkmlExists;
}
