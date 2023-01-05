/**
 * WISE_Application: misc_native.cpp
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

#include <thread>
#include <mutex>

#if _DLL
#include "stdafx.h"
#else //_DLL
#include "types.h"
#endif //_DLL

#include "misc_native.h"
#ifdef _MSC_VER
#include <winerror.h>
#else
#include <pwd.h>
#include "kmllib.h"
#endif
#include "types.h"
#include "mqtt.h"
#include "ProtoStatus.h"
#include "status.pb.h"
#include "wise_config.pb.h"
#include "cwfgmProject.pb.h"
#include <mqtt/async_client.h>
#include "Dlgcnvt.h"
#include "interprocess_comms.h"

#include <google/protobuf/util/json_util.h>


std::string getProtobufVersion()
{
	return google::protobuf::internal::VersionString(GOOGLE_PROTOBUF_VERSION);
}

std::string getPahoVersion()
{
	return mqtt::VERSION_STR;
}

ProtoStatus::StatusUpdater	*s_protoStatus = nullptr;

void createProtoStatus(const std::string& path)
{
	s_protoStatus = new ProtoStatus::StatusUpdater(path);
}

void cleanupProtoStatus()
{
	if (s_protoStatus)
		delete s_protoStatus;
}

void cleanupProtobuf()
{
	google::protobuf::ShutdownProtobufLibrary();
}

void UpdateProtoStatus(int sts, const std::string& msg)
{
	if (s_protoStatus)
	{
		WISE::StatusProto::jobStatus_statusType psts;
		switch (sts)
		{
		case 0:
			psts = WISE::StatusProto::jobStatus_statusType_COMPLETE;
			break;
		case 1:
			psts = WISE::StatusProto::jobStatus_statusType_ERROR;
			break;
		case 2:
			psts = WISE::StatusProto::jobStatus_statusType_FAILED;
			break;
		case 3:
			psts = WISE::StatusProto::jobStatus_statusType_INFORMATION;
			break;
		case 4:
			psts = WISE::StatusProto::jobStatus_statusType_SCENARIO_COMPLETED;
			break;
		case 5:
			psts = WISE::StatusProto::jobStatus_statusType_SCENARIO_FAILED;
			break;
		case 6:
			psts = WISE::StatusProto::jobStatus_statusType_SCENARIO_STARTED;
			break;
		case 7:
			psts = WISE::StatusProto::jobStatus_statusType_STARTED;
			break;
		case 8:
			psts = WISE::StatusProto::jobStatus_statusType_SUBMITTED;
			break;
		case 9:
			psts = WISE::StatusProto::jobStatus_statusType_SHUTDOWN_REQUESTED;
			break;
		default:
			return;
		}
		s_protoStatus->updateStatus(psts, msg);
	}
}

void addProtobufSettings(google::protobuf::Message* output, google::protobuf::Message* known)
{
	if (output && known)
	{
		auto proto = dynamic_cast<WISE::ProjectProto::PrometheusData*>(output);
		auto protoKnown = dynamic_cast<WISE::ProjectProto::PrometheusData*>(known);

		if (proto && protoKnown)
		{
			proto->set_allocated_settings(new WISE::ProjectProto::PrometheusData_SimulationSettings(protoKnown->settings()));
		}
	}
}

int extractCoreCount(google::protobuf::Message* message)
{
	if (message)
	{
		auto proto = dynamic_cast<WISE::ProjectProto::PrometheusData*>(message);
		if (proto && proto->has_settings() && proto->settings().has_hardware())
		{
			return proto->settings().hardware().cores();
		}
	}
	return 6;
}

std::string extractJobName(google::protobuf::Message* message)
{
	if (message)
	{
		auto proto = dynamic_cast<WISE::ProjectProto::PrometheusData*>(message);
		if (proto && proto->has_name())
		{
			return proto->name().value();
		}
	}
	return "";
}

void parseLogData(google::protobuf::Message* message, std::uint8_t* verbosity, std::string *mqttHost,
	std::uint16_t *mqttPort, std::string *mqttUsername, std::string *mqttPassword, std::string *mqttTopic,
	std::int32_t *mqttQos, std::uint8_t *mqttVerbosity)
{
	weak_assert(verbosity);
	weak_assert(mqttHost);
	weak_assert(mqttPort);
	weak_assert(mqttUsername);
	weak_assert(mqttPassword);
	weak_assert(mqttTopic);
	weak_assert(mqttQos);
	weak_assert(mqttVerbosity);

	if (message)
	{
		auto proto = dynamic_cast<WISE::ProjectProto::PrometheusData*>(message);
		if (proto)
		{
			if (proto->has_settings())
			{
				if (proto->settings().has_logfile())
				{
					*verbosity = proto->settings().logfile().verbosity();
				}
				if (proto->settings().has_mqtt())
				{
					*mqttHost = proto->settings().mqtt().hostname();
					if (proto->settings().mqtt().has_port())
						*mqttPort = (std::uint16_t)proto->settings().mqtt().port().value();
					if (proto->settings().mqtt().has_username())
						*mqttUsername = proto->settings().mqtt().username().value();
					if (proto->settings().mqtt().has_password())
						*mqttPassword = proto->settings().mqtt().password().value();
					*mqttTopic = proto->settings().mqtt().topic();
					if (proto->settings().mqtt().has_qos())
						*mqttQos = proto->settings().mqtt().qos().value();
					*mqttVerbosity = proto->settings().mqtt().verbosity();
				}
			}
		}
	}
}


int getExportOverride(google::protobuf::Message* message)
{
	if (message)
	{
		auto proto = dynamic_cast<WISE::ProjectProto::PrometheusData*>(message);
		if ((proto) && (proto->has_settings()) && (proto->settings().export_format_oneof_case() == WISE::ProjectProto::PrometheusData_SimulationSettings::kExportFormat))
		{
			if (proto->settings().export_format() == WISE::ProjectProto::PrometheusData_SimulationSettings::BINARY)
				return 2;
			else if (proto->settings().export_format() == WISE::ProjectProto::PrometheusData_SimulationSettings::MINIMAL_JSON)
				return 1;
			else if (proto->settings().export_format() == WISE::ProjectProto::PrometheusData_SimulationSettings::PRETTY_JSON)
				return 3;
		}
	}
	return -1;
}


void initializeTemporaryOutputs(google::protobuf::Message* message, gsl::not_null<std::int32_t*> temporaryInterval, gsl::not_null<std::int32_t*> keepOld)
{
	*temporaryInterval = 100;
	*keepOld = 2;

	if (message)
	{
		auto proto = dynamic_cast<WISE::ProjectProto::PrometheusData*>(message);
		if ((proto) && (proto->has_sub_scenario_options()))
		{
			if (proto->sub_scenario_options().has_temp_output_interval())
				*temporaryInterval = proto->sub_scenario_options().temp_output_interval().value();
			if (proto->sub_scenario_options().has_temp_output_count())
				*keepOld = proto->sub_scenario_options().temp_output_count().value();
		}
	}
}


void initializeUnits(google::protobuf::Message* message, UnitConversion* unitConversion)
{
	if (message)
	{
		auto proto = dynamic_cast<WISE::ProjectProto::PrometheusData*>(message);
		if ((proto) && (proto->has_unitsettings()))
		{
			if (proto->unitsettings().smallmeasureoutput())
				unitConversion->SmallMeasureDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().smallmeasureoutput());

			if (proto->unitsettings().smalldistanceoutput())
				unitConversion->SmallDistanceDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().smalldistanceoutput());

			if (proto->unitsettings().distanceoutput())
				unitConversion->DistanceDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().distanceoutput());

			if (proto->unitsettings().alternatedistanceoutput())
				unitConversion->AltDistanceDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().alternatedistanceoutput());

			if (proto->unitsettings().areaoutput())
				unitConversion->AreaDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().areaoutput());

			if (proto->unitsettings().volumeoutput())
				unitConversion->VolumeDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().volumeoutput());

			if (proto->unitsettings().temperatureoutput())
				unitConversion->TempDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().temperatureoutput());

			if (proto->unitsettings().massoutput())
				unitConversion->MassDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().massoutput());

			if ((proto->unitsettings().massareaoutput().mass()) || (proto->unitsettings().massareaoutput().area()))
				unitConversion->MassAreaDisplay(
					((((UnitConvert::STORAGE_UNIT)proto->unitsettings().massareaoutput().mass()) << 0x20) |
					  ((UnitConvert::STORAGE_UNIT)proto->unitsettings().massareaoutput().area()))
					);

			if (proto->unitsettings().energyoutput())
				unitConversion->EnergyDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().energyoutput());

			if (proto->unitsettings().angleoutput())
				unitConversion->AngleDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().angleoutput());

			if ((proto->unitsettings().velocityoutput().distance()) || (proto->unitsettings().velocityoutput().time()))
				unitConversion->VelocityDisplay(
					((UnitConvert::STORAGE_UNIT)proto->unitsettings().velocityoutput().distance()) |
					((UnitConvert::STORAGE_UNIT)proto->unitsettings().velocityoutput().time())
					);

			if ((proto->unitsettings().alternatevelocityoutput().distance()) || (proto->unitsettings().alternatevelocityoutput().time()))
				unitConversion->AltVelocityDisplay(
					((UnitConvert::STORAGE_UNIT)proto->unitsettings().alternatevelocityoutput().distance()) |
					((UnitConvert::STORAGE_UNIT)proto->unitsettings().alternatevelocityoutput().time())
					);

			if (proto->unitsettings().coordinateoutput())
				unitConversion->CoordinateDisplay((UnitConvert::STORAGE_UNIT)proto->unitsettings().coordinateoutput());

			if ((proto->unitsettings().intensityoutput().energy()) || (proto->unitsettings().intensityoutput().distance()))
				unitConversion->IntensityDisplay(
					((((UnitConvert::STORAGE_UNIT)proto->unitsettings().intensityoutput().energy()) << 0x20) |
	 				  ((UnitConvert::STORAGE_UNIT)proto->unitsettings().intensityoutput().distance()))
					);
		}
	}
}


std::string get_job_directory()
{
	std::string retval;
#ifdef _MSC_VER
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\WISE_Manager", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		CHAR szBuffer[512];
		DWORD dwBufferSize = sizeof(szBuffer);
		if (RegQueryValueEx(hKey, "JobDirectory", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize) == ERROR_SUCCESS)
		{
			retval = szBuffer;
		}
	}
#else
	const char* homedir = getenv("HOME");
	//if the environment variable isn't set try getting the directory from the user account details
	if (homedir == nullptr)
	{
		struct passwd* pw = getpwuid(getuid());
		homedir = pw->pw_dir;
	}

	if (homedir)
	{
		fs::path path(homedir);
		path /= ".java/.userPrefs/manager/prefs.xml";
		//the preference file exists
		if (fs::exists(path))
			Java::read_job_directory(path, retval);
	}
#endif

	return retval;
}

void get_core_settings(bool& numa_lock, std::uint32_t& job_count, std::uint32_t& skip_processors)
{
	std::string directory = get_job_directory();
	bool info_set = false;
	if (!directory.empty())
	{
		fs::path path(directory);
		path /= "config.json";
		if (fs::exists(path))
		{
			std::ifstream in(path);
			if (in.is_open())
			{
				google::protobuf::util::JsonParseOptions options;
				options.ignore_unknown_fields = true;

				std::string json((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
				auto data = new wise::confic::ServerConfiguration();
				try
				{
					if (google::protobuf::util::JsonStringToMessage(json, data, options).ok())
					{
						if (data->has_manager_settings())
						{
							job_count = data->manager_settings().max_concurrent();
							if (data->manager_settings().has_numa_lock())
								numa_lock = data->manager_settings().numa_lock().value();
							else
								numa_lock = true;
							if (data->manager_settings().has_skip_cores())
								skip_processors = data->manager_settings().skip_cores().value();
							else
								skip_processors = 0;
							info_set = true;
						}
					}
				}
				catch (std::exception& e) {}

				delete data;
				data = nullptr;
			}
		}
	}

	//apply some defaults
	if (!info_set)
	{
		numa_lock = true;
		job_count = CWorkerThreadPool::NumberProcessors(0) - 1;
		if (job_count < interprocess_comms::DEFAULT_CPU_PER_JOB)
			job_count = 1;
		else
			job_count = static_cast<std::uint32_t>(floor(job_count / ((double)interprocess_comms::DEFAULT_CPU_PER_JOB)));
		skip_processors = 1;
	}
}
