/**
 * WISE_Application: misc_native.h
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

#pragma once

#include <string>
#include <gsl/gsl>

namespace google
{
	namespace protobuf
	{
		class Message;
	}
}

std::string boost_filename(const char *path);

std::string getProtobufVersion();
std::string getPahoVersion();
void createProtoStatus(const std::string& path);
void cleanupProtoStatus();
void cleanupProtobuf();

void UpdateProtoStatus(int sts, const std::string& msg);
void addProtobufSettings(google::protobuf::Message* output, google::protobuf::Message* known);

int extractCoreCount(google::protobuf::Message* message);
std::string extractJobName(google::protobuf::Message* message);

void parseLogData(google::protobuf::Message* message, std::uint8_t* verbosity, std::string *mqttHost,
	std::uint16_t *mqttPort, std::string *mqttUsername, std::string *mqttPassword, std::string *mqttTopic,
	std::int32_t *mqttQos, std::uint8_t *mqttVerbosity);

int getExportOverride(google::protobuf::Message* message);
void initializeTemporaryOutputs(google::protobuf::Message* message, gsl::not_null<std::int32_t*> temporaryInterval, gsl::not_null<std::int32_t*> keepOld);
void initializeUnits(google::protobuf::Message* message, class UnitConversion* unitConversion);
void get_core_settings(bool& numa_lock, std::uint32_t& job_count, std::uint32_t& skip_processors);
