/**
 * WISE_Application: WISE.h
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

#include <functional>
#if defined(__GNUC__)
#include <condition_variable>
#include <thread>
#endif
#include <fstream>
#include "types.h"
#include "CWFGMProject.h"
#include "FwiCom.h"
#include "mqtt.h"
#include "pevents.h"

#include "prom_vers.h"
#include "validation_object.h"
#include "interprocess_comms.h"


enum struct WISEStatus
{
	COMPLETE,
	WISE_ERROR,
	FAILED,
	INFORMATION,
	SCENARIO_COMPLETED,
	SCENARIO_FAILED,
	SCENARIO_STARTED,
	STARTED,
	SUBMITTED,
	SHUTDOWN_REQUESTED
};

enum struct InputType
{
	JSON,
	BINARY,
	JSON_PRETTY,
	UNKNOWN
};

enum struct SummaryType
{
	NONE,
	TEXT,
	JSON,
	BINARY,
	VALIDATE
};


class SPARCS_P : public IMqttCallback
{
protected:
	void initMe2();

public:
	SPARCS_P();
	~SPARCS_P();

	neosmart::neosmart_event_t m_event;
	CRWThreadSemaphore *m_lock;
	MinListTempl<class SimMessage> m_list;

	std::unique_ptr<ICWFGM_FWI>	m_fwi;
	std::tuple<const char*, const char*> guessDriverNameFromFilename(const char *filename);

	HRESULT Initialize_SPARCS_P();
	virtual HRESULT ConnectMQTT(bool justSummary) { return ConnectMQTTProto(justSummary); }
	[[nodiscard]]
	virtual HRESULT LoadProtobuf(const char *filename, std::uint32_t &loadedFileVersion, std::function<std::string(void)> password_callback, SummaryType justSummary);
	virtual HRESULT SaveProtobuf(const char *filename, std::uint32_t majorFileVersion, std::function < std::string(void)> password_callback);
	virtual HRESULT SaveModifiedProtofbuf(const std::string& original, std::uint32_t majorFileVersion, std::function<std::string(void)> password_callback);
	/// <summary>
	/// Save the validation report to a JSON file.
	/// </summary>
	virtual HRESULT SaveValidationReport(const std::string& path);
	HRESULT RunSimulations(SummaryType justSummary);

	std::int16_t SimulationCalculateContinue(Project::Scenario **s, std::uint32_t *s_cnt);
	HRESULT KMLHelper(const char *filename, const WTimeSpan &offset);

	void Shutdown(const std::vector<std::string>& topic, JSON::ShutdownMessage message) override;
	void UpdateJob(const std::vector<std::string>& topic, std::vector<JSON::UpdateMessage> messages) override;

	std::string			m_id,
						m_log_file;
	std::uint16_t		m_lv;
	std::uint8_t		*m_mqtt_msg;
	std::uint32_t		m_mqtt_msg_len;
	bool				m_stop_requested;
	InputType			m_inputType;
	InputType			m_outputTypeOverride;
	bool				m_connectImmediately;
	std::string			m_managerId;
	std::int32_t		m_tempOutputInterval;
	std::int32_t		m_keepTempFiles;
	std::string			m_startTime;
	std::string			m_loadWarnings;
	std::atomic<std::int32_t>	m_remainingScenarioCnt;

	google::protobuf::Message* m_proto;
	UnitConversion		m_unitConversion;
	std::shared_ptr<interprocess_comms::shared_block> m_sharedMemory;

	MqttClient			m_client;

	Project::CWFGMProject	*m_project;
	std::string				job_name;

	HRESULT					last_error;
	const char				*last_error_msg;
	
	constexpr static std::uint8_t OUTPUT_STATUS_FILE = 1;
	constexpr static std::uint8_t OUTPUT_STATUS_MQTT = 2;
	constexpr static std::uint8_t OUTPUT_STATUS_ALL = OUTPUT_STATUS_FILE | OUTPUT_STATUS_MQTT;

	bool initMQTT();
	bool deinitMQTT();
	void updateMQTT(std::uint16_t level, MQTTStatus _status, const char *status_msg, short qos);
	void debugMQTT(std::uint16_t level, MQTTStatus _status, const char *status_msg);
	void emitStatisticsMQTT(USHORT level, MQTTStatus _status, const char *status_msg, const std::vector<std::tuple<std::string, Service::StatisticType>> &statistics);

	void UpdateStatus(std::uint16_t level, WISEStatus status, const std::string& msg, short qos, std::uint8_t outputType);
	void UpdateStatus(std::uint16_t level, WISEStatus status, const std::string& msg, short qos, std::uint8_t outputType, const std::vector<std::tuple<std::string, Service::StatisticType>>& statistics);
	void DebugStatus(std::uint16_t level, WISEStatus status, const std::string& msg, std::uint8_t outputType = OUTPUT_STATUS_ALL);
	void EmitStatistics(const std::vector<std::tuple<std::string, Service::StatisticType>>& statistics, std::uint8_t outputType = OUTPUT_STATUS_ALL);

	/// <summary>
	/// Setup processor locking.
	/// </summary>
	/// <param name="coreCount">The number of cores to use to run the simulation. Use -1 to use the value from the FGM.</param>
	/// <param name="coreOffset">An offset into the cores the system has to lock the cores that will be used. Use -1 for no offset.</param>
	/// <param name="coreMask">An alternative way to coreOffset to set which cores should be used to run the job. Use an empty string to not use a mask.</param>
	/// <param name="memDump">If present the contents of the shared memory block (if available and used) will be returned.</param>
	void SetupThreads(std::int32_t coreCount, std::int32_t coreOffset, const std::string& coreMask, bool useSharedMemory, std::string* memDump = nullptr);

	void addLoadBalancingIndices(const std::vector<std::int32_t>& indices) { m_project->loadBalancing(/*Project::LoadBalancingType::LOCAL_FILE*/); m_balanceIndices.insert(m_balanceIndices.end(), indices.begin(), indices.end()); }

	static void StaticInitialization();
	static void StaticDeconstruction();

protected:
	HRESULT ConnectMQTTProto(bool justSummary);
	int protobufCoreCount();
	std::string protobufJobName();

	/// <summary>
	/// Prefetch the next scenario index to run from the load balancer,
	/// if applicable.
	/// </summary>
	void requestNextScenarioIndex();
	/// <summary>
	/// Get the next scenario index to run from the load balancer or
	/// a prefetched value if available.
	/// </summary>
	/// <param name="defaultValue">The default value to return if a scenario
	/// index couldn't be fetched from the load balancer.</param>
	std::int32_t fetchNextScenarioIndex(std::int32_t defaultValue);

	std::string FileNameToExeDirPath(const char* filename);
	std::string m_path;

	HRESULT RunSimulation(Project::Scenario *scenario, SummaryType justSummary, int balanceIndex);
	/**
	 * Initialize a scenario that should have child scenarios.
	 */
	HRESULT InitializeSubScenarios(Project::Scenario *parent);
	HRESULT CleanupSubScenarios(Project::Scenario *parent);

private:
	std::shared_ptr<validation::validation_object> m_rootValidation{ nullptr };
	std::vector<std::int32_t> m_balanceIndices;
	std::int32_t m_balanceIndicesIndex{ 0 };
};

extern const char* ErrorMsg(HRESULT hr, const char* msg);
