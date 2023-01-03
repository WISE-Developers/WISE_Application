/**
 * WISE_Application: mqtt.h
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
#include <cstdint>
#include <vector>

#include "WTime.h"
#include "TimestepSettings.h"
#include "CWFGMProject.h"
#include "filesystem.hpp"

#if !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
#pragma managed(push, off)
#endif

enum MQTTStatus
{
	Submitted,
	Started,
	ScenarioStarted,
	ScenarioCompleted,
	ScenarioFailed,
	Complete,
	Failed,
	Error,
	Information,
	ShutdownRequested
};

namespace JSON
{
	struct ShutdownMessage
	{
		std::int32_t priority;
	};

	struct UpdateMessage
	{
		std::string name;
		std::int32_t status;
	};
};

class IMqttCallback
{
public:
	virtual void Shutdown(const std::vector<std::string>& topic, JSON::ShutdownMessage message) = 0;
	virtual void UpdateJob(const std::vector<std::string>& topic, std::vector<JSON::UpdateMessage> messages) = 0;
};


class user_callback;

class MqttClient
{
	friend class ReceiveWorker;

public:
	MqttClient();
	~MqttClient();

public:
	bool initMQTT();
	bool deinitMQTT();
	void updateMQTT(std::uint16_t level, MQTTStatus status, const std::string &message, short qos);
	void updateMQTT(std::uint16_t level, MQTTStatus status, const std::string &message, const std::vector<std::tuple<std::string, Service::StatisticType>> &statistics);
	/**
	 * Send a status message to the debug topic.
	 */
	void debugMQTT(std::uint16_t level, MQTTStatus status, const std::string &message);
	std::string connectionUrl();

	void updateTimezone(const HSS_Time::WorldLocation& location) { m_worldLocation = HSS_Time::WorldLocation(location); }
	void updateTimezone(HSS_Time::WorldLocation&& location) { m_worldLocation = location; }

	/**
	 * If a remote location was specified notify manager that it should stream the specified file.
     * @param type 1 -> summary file, 2 -> vector file, 3 -> grid file
	 * @param exists Does the file exist or was it skipped because it was empty.
	 */
	bool streamOutputFile(const std::string& filename, const std::string& coverageName, std::uint8_t type, bool exists,
		const std::string& managerId, google::protobuf::Message* message);

	/**
	 * Request a new scenario index to run for load balancing.
	 * The request is made asynchronously.
	 * @param count The number of indices to request.
	 */
	void requestScenarioIndex(std::uint8_t count = 1);
	/**
	 * Get the next available scenario index to run. If there
	 * are no indices in the queue, a new index is requested.
	 * @param defaultValue If an index cannot be retreived from
     *                     the load balancer, this value will be returned.
	 * @returns The next scenario index to run.
	 */
	std::int32_t nextScenarioIndex(std::int32_t defaultValue);

private:
	void processMessage(const std::string& topic, const std::string& body) noexcept;
	void sendRequestScenarioIndex(std::uint8_t count);

public:
	std::string m_mqtt_host;
	std::string m_mqtt_user;
	std::string m_mqtt_pwd;
	std::string m_mqtt_id;
	std::string m_mqtt_job;
	std::string m_mqtt_topic;
	bool m_mqtt_connected;
	bool m_shutdown;
	std::int16_t m_mqtt_lv;
	std::int16_t m_mqtt_port;
	std::int16_t m_mqtt_max_inflight;
	std::int32_t m_mqtt_qos;
	IMqttCallback *m_callback;
	class MqttClientPrivate* m_private;
	HSS_Time::WorldLocation m_worldLocation;
	HSS_Time::WTimeManager m_timeManager;
};

#if !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
#pragma managed(pop)
#endif
