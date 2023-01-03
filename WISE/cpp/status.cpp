/**
 * WISE_Grid_Module: status.cpp
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
#endif //_MSC_VER
#include "types.h"
#include "WISE.h"
#include "misc_native.h"

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif


void SPARCS_P::UpdateStatus(std::uint16_t level, WISEStatus sts, const std::string& msg, short qos, std::uint8_t outputType)
{
	if ((outputType & OUTPUT_STATUS_FILE) && m_lv > level)
		UpdateProtoStatus((int)sts, msg);

	if ((outputType & OUTPUT_STATUS_MQTT) && m_client.m_mqtt_lv > level)
	{
		MQTTStatus msts;
		switch (sts)
		{
		case WISEStatus::COMPLETE:
			msts = MQTTStatus::Complete;
			break;
		case WISEStatus::WISE_ERROR:
			msts = MQTTStatus::Error;
			break;
		case WISEStatus::FAILED:
			msts = MQTTStatus::Failed;
			break;
		case WISEStatus::INFORMATION:
			msts = MQTTStatus::Information;
			break;
		case WISEStatus::SCENARIO_COMPLETED:
			msts = MQTTStatus::ScenarioCompleted;
			break;
		case WISEStatus::SCENARIO_FAILED:
			msts = MQTTStatus::ScenarioFailed;
			break;
		case WISEStatus::SCENARIO_STARTED:
			msts = MQTTStatus::ScenarioStarted;
			break;
		case WISEStatus::STARTED:
			msts = MQTTStatus::Started;
			break;
		case WISEStatus::SUBMITTED:
			msts = MQTTStatus::Submitted;
			break;
		case WISEStatus::SHUTDOWN_REQUESTED:
			msts = MQTTStatus::ShutdownRequested;
			break;
		default:
			return;
		}

		updateMQTT(level, msts, msg.c_str(), qos);
	}
}

void SPARCS_P::UpdateStatus(std::uint16_t level, WISEStatus sts, const std::string& msg, short qos,
	std::uint8_t outputType, const std::vector<std::tuple<std::string, Service::StatisticType>>& statistics)
{
	if ((outputType & OUTPUT_STATUS_FILE) && m_lv > level)
		UpdateProtoStatus((int)sts, msg);

	if ((outputType & OUTPUT_STATUS_MQTT) && m_client.m_mqtt_lv > level)
	{
		MQTTStatus msts;
		switch (sts)
		{
		case WISEStatus::COMPLETE:
			msts = MQTTStatus::Complete;
			break;
		case WISEStatus::WISE_ERROR:
			msts = MQTTStatus::Error;
			break;
		case WISEStatus::FAILED:
			msts = MQTTStatus::Failed;
			break;
		case WISEStatus::INFORMATION:
			msts = MQTTStatus::Information;
			break;
		case WISEStatus::SCENARIO_COMPLETED:
			msts = MQTTStatus::ScenarioCompleted;
			break;
		case WISEStatus::SCENARIO_FAILED:
			msts = MQTTStatus::ScenarioFailed;
			break;
		case WISEStatus::SCENARIO_STARTED:
			msts = MQTTStatus::ScenarioStarted;
			break;
		case WISEStatus::STARTED:
			msts = MQTTStatus::Started;
			break;
		case WISEStatus::SUBMITTED:
			msts = MQTTStatus::Submitted;
			break;
		case WISEStatus::SHUTDOWN_REQUESTED:
			msts = MQTTStatus::ShutdownRequested;
			break;
		default:
			return;
		}

		emitStatisticsMQTT(level, msts, msg.c_str(), statistics);
	}
}

void SPARCS_P::DebugStatus(std::uint16_t level, WISEStatus sts, const std::string& msg, std::uint8_t outputType)
{
	if ((outputType & OUTPUT_STATUS_MQTT) && m_client.m_mqtt_lv > level)
	{
		MQTTStatus msts;
		switch (sts)
		{
		case WISEStatus::COMPLETE:
			msts = MQTTStatus::Complete;
			break;
		case WISEStatus::WISE_ERROR:
			msts = MQTTStatus::Error;
			break;
		case WISEStatus::FAILED:
			msts = MQTTStatus::Failed;
			break;
		case WISEStatus::INFORMATION:
			msts = MQTTStatus::Information;
			break;
		case WISEStatus::SCENARIO_COMPLETED:
			msts = MQTTStatus::ScenarioCompleted;
			break;
		case WISEStatus::SCENARIO_FAILED:
			msts = MQTTStatus::ScenarioFailed;
			break;
		case WISEStatus::SCENARIO_STARTED:
			msts = MQTTStatus::ScenarioStarted;
			break;
		case WISEStatus::STARTED:
			msts = MQTTStatus::Started;
			break;
		case WISEStatus::SUBMITTED:
			msts = MQTTStatus::Submitted;
			break;
		case WISEStatus::SHUTDOWN_REQUESTED:
			msts = MQTTStatus::ShutdownRequested;
			break;
		default:
			return;
		}

		debugMQTT(level, msts, msg.c_str());
	}
}


void SPARCS_P::EmitStatistics(const std::vector<std::tuple<std::string, Service::StatisticType>> &statistics, std::uint8_t outputType)
{
	std::uint16_t level = 0;//emit statistics at log level INFO
	if ((outputType & OUTPUT_STATUS_FILE) && m_lv > level)
	{
		std::string msg = "Statistics: ";
		std::string separator = "";
		for (auto& entry : statistics)
		{
			msg += separator;
			msg += std::get<0>(entry);
			msg += ": ";
			Service::StatisticType obj = std::get<1>(entry);
			if (std::holds_alternative<double>(obj))
				msg += std::to_string(std::get<double>(obj));
			else if (std::holds_alternative<std::uint32_t>(obj))
				msg += std::to_string(std::get<std::uint32_t>(obj));
			else if (std::holds_alternative<std::uint64_t>(obj))
				msg += std::to_string(std::get<std::uint64_t>(obj));
			else if (std::holds_alternative<std::string>(obj))
				msg += std::get<std::string>(obj);
			else
				msg += "null";
			separator = ", ";
		}
		UpdateProtoStatus((int)WISEStatus::INFORMATION, msg);
	}

	if ((outputType & OUTPUT_STATUS_MQTT) && m_client.m_mqtt_lv > level)
	{
		emitStatisticsMQTT(level, MQTTStatus::Information, "Statistics Outputs", statistics);
	}
}
