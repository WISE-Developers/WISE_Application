/**
 * WISE_Application: mqtt.cpp
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

#include "CWFGMProject.h"

#ifdef _MSC_VER
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include "Thread.h"
#include "mqtt.h"
#include "mqtt/client.h"

#include <string>
#include <sstream>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>
#include <atomic>
#include <chrono>
#include <bitset>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include "file_upload.pb.h"
#include "cwfgmProject.pb.h"
#include "hss_vers.h"
#include "doubleBuilder.h"
#include <google/protobuf/util/json_util.h>

#undef strtoull
#undef strtoll
#include "json.hpp"

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

using namespace std::chrono_literals;
using namespace std::string_literals;

#include "filesystem.hpp"

using json = nlohmann::json;

mqtt::async_client *_asyncClient = nullptr;
std::mutex m_lock;

#define MAX_RECONNECT_ATTEMPTS 10


namespace JSON
{
	struct Status
	{
		int status;
		std::string time;
		std::string message;
		std::vector<std::tuple<std::string, Service::StatisticType>> statistics;
	};

	struct Checkin
	{
		std::string nodeId;
		std::string topicString;
		std::uint64_t memoryUsage;
		double cpuUsage;
	};

	std::string to_json(Status const& val)
	{
		json j = { {"status"s, val.status},{"message"s, val.message},{"time", val.time} };
		if (val.statistics.size() > 0)
		{
			j["stats"s] = json::object();
			for (auto& stat : val.statistics)
			{
				Service::StatisticType obj = std::get<1>(stat);
				if (std::holds_alternative<double>(obj))
					j["stats"s][std::get<0>(stat)] = std::get<double>(obj);
				else if (std::holds_alternative<std::uint32_t>(obj))
					j["stats"s][std::get<0>(stat)] = std::get<std::uint32_t>(obj);
				else if (std::holds_alternative<std::uint64_t>(obj))
					j["stats"s][std::get<0>(stat)] = std::get<std::uint64_t>(obj);
				else if (std::holds_alternative<std::string>(obj))
					j["stats"s][std::get<0>(stat)] = std::get<std::string>(obj);
			}
		}
		return j.dump();
	}

	std::string to_json(Checkin const& val)
	{
		json j = { { "node_id"s, val.nodeId }, { "version"s, VER_PROMETHEUS_VERSION_FULL_STRING },
			{ "status"s, 0 }, { "node_type"s, 5 }, { "topic_string"s, val.topicString },
			{ "wise_details"s, { { "memory_usage"s, val.memoryUsage }, { "cpu_usage"s, val.cpuUsage } } } };
		return j.dump();
	}

	UpdateMessage jsonToUpdate(json j)
	{
		JSON::UpdateMessage message;
		message.name = j["name"].get<std::string>();
		message.status = j["status"];
		return message;
	}
}


class ReceiveWorker
{
public:
	ReceiveWorker() = default;

	void initialize(MqttClient* client)
	{
		_client = client;
		m_thread = std::make_unique<std::thread>(std::bind(&ReceiveWorker::Entry, this));
	}

	void shutdown()
	{
		if (!m_exit && m_thread)
		{
			{
				std::lock_guard<std::mutex> lock(m_locker);
				m_exit = true;
				m_poke.notify_one();
			}
			m_thread->join();
		}
	}

	void push_back(mqtt::const_message_ptr message)
	{
		std::unique_lock<std::mutex> lock(m_locker);
		if (!m_exit)
		{
      		std::tuple<std::string, std::string> tuple = std::make_tuple(message->get_topic(), message->get_payload_str());
      		m_queue.push(tuple);
      		m_poke.notify_one();
      	}
	}

private:
	void Entry()
	{
		//lock this thread to the event loop processor
		CWorkerThreadPool::SetThreadAffinityToMask(CWorkerThreadPool::GetCurrentThread(), CWorkerThreadPool::EVENT_LOOP_MASK);

		while (true)
		{
			std::unique_lock<std::mutex> tlock(m_locker);
			m_poke.wait(tlock, [&]() { return m_exit || m_queue.size() > 0; });

			if (m_exit && m_queue.size() == 0)
				return;

			std::tuple<std::string, std::string> msg = m_queue.front();
			m_queue.pop();

			_client->processMessage(std::get<0>(msg), std::get<1>(msg));
		}
	}

private:
	std::queue<std::tuple<std::string, std::string>> m_queue;
	std::unique_ptr<std::thread> m_thread;
	std::condition_variable m_poke;
	std::mutex m_locker;
	MqttClient *_client;
	bool m_exit;
};
ReceiveWorker worker;


class user_callback : public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
public:
	user_callback(MqttClient *client, mqtt::connect_options& opts)
		: _retries(0)
	{
		_client = client;
		_opts = opts;
	}

	// Callback for when the connection is lost.
	void connection_lost(const std::string& cause) override
	{
		std::cout << "Connection Lost: "s << cause << std::endl;
		_client->m_mqtt_connected = false;
		_retries = 0;
		reconnect();
	}

	void delivery_complete(mqtt::delivery_token_ptr tok) override
	{
	}

	//re-connect failure
	void on_failure(const mqtt::token& tok) override
	{
		if (!_client->m_shutdown)
		{
			std::cout << "Failure: "s << tok.get_message_id() << std::endl;
			_retries++;
			if (_retries < MAX_RECONNECT_ATTEMPTS)
				reconnect();
		}
	}

	void connected(const std::string& cause) override
	{
		_client->m_mqtt_connected = true;
		std::ostringstream stream;
		stream << _client->m_mqtt_topic << "/"s;
		stream << "+/"s;
		stream << _client->m_mqtt_job << "/"s;
		stream << "shutdown"s;
		_asyncClient->subscribe(stream.str(), _client->m_mqtt_qos, nullptr, *this);

		stream.str("");
		stream.clear();
		stream << _client->m_mqtt_topic << "/"s;
		stream << "+/"s;
		stream << _client->m_mqtt_job << "/"s;
		stream << "updatejob"s;
		_asyncClient->subscribe(stream.str(), _client->m_mqtt_qos, nullptr, *this);

		//listen for load balance responses
		stream.str("");
		stream.clear();
		stream << _client->m_mqtt_topic << "/"s;
		stream << "+/"s;
		stream << _client->m_mqtt_id << "/"s;
		stream << "balance"s;
		_asyncClient->subscribe(stream.str(), _client->m_mqtt_qos, nullptr, *this);

		//listen for requests to report that the job is running
		stream.str("");
		stream.clear();
		stream << _client->m_mqtt_topic << "/"s;
		stream << "+/broadcast/reportin"s;
		_asyncClient->subscribe(stream.str(), _client->m_mqtt_qos, nullptr, *this);

		stream.str("");
		stream.clear();
		stream << _client->m_mqtt_topic << "/"s;
		stream << "+/wise/reportin"s;
		_asyncClient->subscribe(stream.str(), _client->m_mqtt_qos, nullptr, *this);
	}

	//re-connect success
	void on_success(const mqtt::token& tok) override
	{
	}

	// Callback for when a message arrives.
	void message_arrived(mqtt::const_message_ptr msg) override
	{
		worker.push_back(msg);
	}

private:
	void reconnect()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(2500));
		_retries++;
		try
		{
			_asyncClient->connect(_opts, nullptr, *this);
		}
		catch (const mqtt::exception& e)
		{
			std::cerr << "ERROR: Unable to reconnect to MQTT server ("s << _client->connectionUrl() << "): '"s << e.what() << "'"s << std::endl;
			delete _client;
			_client = nullptr;
		}
	}

private:
	MqttClient *_client;
	mqtt::connect_options _opts;
	std::int32_t _retries;
};
user_callback *cb;


class MqttClientPrivate
{
public:
	std::atomic_int m_scenarioRequestCounter;
	std::atomic_int m_scenarioWaitingCounter;
	std::vector<int> m_scenarioIndices;
	std::mutex m_scenarioIndexLock;
	std::condition_variable m_scenarioIndexPoke;
};


std::string MqttClient::connectionUrl()
{
	std::ostringstream stream;
	if (!boost::istarts_with(m_mqtt_host, "tcp://"s))
		stream << "tcp://"s;
	stream << m_mqtt_host;
	if (m_mqtt_port > 0)
		stream << ":"s << m_mqtt_port;
	return stream.str();
}


void MqttClient::processMessage(const std::string& topic, const std::string& body) noexcept
{
	if (m_callback)
	{
		std::vector<std::string> parts;
		boost::split(parts, topic, boost::is_any_of("/"), boost::token_compress_on);

		if (boost::ends_with(topic, "/shutdown"s))
		{
			JSON::ShutdownMessage message;
			json j;
			try {
				j = json::parse(body);
				message.priority = j["priority"s];
				m_callback->Shutdown(parts, message);
			}
			catch (...) {
			}
		}
		else if (boost::ends_with(topic, "/updatejob"s))
		{
			std::vector<JSON::UpdateMessage> messages;
			json j;
			try {
				j = json::parse(body);
			}
			catch (...) {
			}
			if (j.is_array())
			{
				for (auto& element : j)
				{
					messages.push_back(JSON::jsonToUpdate(element));
				}
			}
			else
			{
				messages.push_back(JSON::jsonToUpdate(j));
			}
			m_callback->UpdateJob(parts, messages);
		}
		else if (boost::ends_with(topic, "/balance"s))
		{
			{
				std::unique_lock<std::mutex> tlock(m_private->m_scenarioIndexLock);
				try {
					json j = json::parse(body);
					m_private->m_scenarioIndices.push_back(j["index"s]);
					m_private->m_scenarioRequestCounter--;
				}
				catch (...) {
				}
			}
			m_private->m_scenarioIndexPoke.notify_one();
		}
		else if (boost::ends_with(topic, "/reportin"s))
		{
			std::lock_guard<std::mutex> lock(m_lock);
			//quit if there is no MQTT communication going to take place
			if (!_asyncClient)
				return;

			std::vector<std::string> parts;
			boost::split(parts, topic, boost::is_any_of("/"s));

			std::ostringstream stream;
			stream << m_mqtt_topic << "/"s;
			stream << m_mqtt_job << "/"s;
			stream << parts[1] << "/"s;
			stream << "checkin"s;

			JSON::Checkin checkin{ m_mqtt_id, m_mqtt_topic, CWorkerThreadPool::GetCurrentMemoryUsage(), CWorkerThreadPool::GetCurrentCpuUsage() };

			if (_asyncClient->is_connected())
			{
				std::string body = JSON::to_json(checkin);
				mqtt::message_ptr msg = mqtt::make_message(stream.str(), body);
				msg->set_qos(m_mqtt_qos);
				_asyncClient->publish(msg, nullptr, *cb);
			}
		}
	}
}


std::string randomHost(std::uint16_t length)
{
	std::string retval;
	static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < length; i++)
		retval.push_back(alphanum[rand() % (sizeof(alphanum) - 1)]);
	return retval;
}


bool MqttClient::initMQTT()
{
	std::lock_guard<std::mutex> lock(m_lock);
	//quit if there is no MQTT communication going to take place
	if (m_mqtt_lv == 0)
		return true;

	char hostname[256];
#ifdef _MSC_VER
	int pid = _getpid();
#else
    int pid = getpid();
#endif
	hostname[0] = '\0';
	gethostname(hostname, 256);
	hostname[255] = '\0';
	std::string temp = hostname;
	if (temp.length() == 0)
		temp = randomHost(10);
	boost::replace_all(temp, "#"s, ""s);
	boost::replace_all(temp, "*"s, ""s);
	boost::replace_all(temp, "/"s, ""s);

	std::ostringstream stream;
	stream << "wise_"s << pid << "-"s << temp;
	m_mqtt_id = stream.str();

	_asyncClient = new mqtt::async_client(connectionUrl(), m_mqtt_id);

	if (!_asyncClient)
		return false;

	mqtt::connect_options connOpts;
	connOpts.set_automatic_reconnect(true);
	connOpts.set_connect_timeout(std::chrono::seconds(10));
	if (m_mqtt_user.length() > 0 && m_mqtt_pwd.length() > 0)
	{
		connOpts.set_user_name(m_mqtt_user);
		connOpts.set_password(m_mqtt_pwd);
	}
	connOpts.set_clean_session(true);
	if (m_mqtt_max_inflight > 0)
		connOpts.set_max_inflight(m_mqtt_max_inflight);

	cb = new user_callback(this, connOpts);

	_asyncClient->set_callback(*cb);

	worker.initialize(this);

	try
	{
		_asyncClient->connect(connOpts, nullptr, *cb)->wait();
		m_shutdown = false;
	}
	catch (const mqtt::exception&)
	{
		delete _asyncClient;
		_asyncClient = nullptr;
		std::cerr << "ERROR: Unable to connect to MQTT server: '"s << connectionUrl() << "'"s << std::endl;
		return false;
	}

	return true;
}


bool MqttClient::deinitMQTT()
{
	std::lock_guard<std::mutex> lock(m_lock);
	m_shutdown = true;

	worker.shutdown();

	bool retval;
	if (!_asyncClient)
		retval = true;
	else
	{
		try
		{
			//wait for all publishes to complete
			for (const auto& token : _asyncClient->get_pending_delivery_tokens())
			{
				token->wait();
			}
			_asyncClient->disconnect()->wait();
			retval = true;
		}
		catch (const mqtt::exception& exc)
		{
			std::cerr << exc.what() << std::endl;
			retval = false;
		}
		delete _asyncClient;
		_asyncClient = nullptr;

		delete cb;
		cb = nullptr;
	}

	return retval;
}


void MqttClient::updateMQTT(std::uint16_t level, MQTTStatus status, const std::string &message, short qos)
{
	std::lock_guard<std::mutex> lock(m_lock);
	//quit if there is no MQTT communication going to take place
	if (!_asyncClient)
		return;

	std::ostringstream stream;
	stream << m_mqtt_topic << "/"s;
	stream << m_mqtt_id << "/"s;
	stream << m_mqtt_job << "/"s;
	stream << "status"s;

	HSS_Time::WTime time = HSS_Time::WTime::Now(&m_timeManager, 0);
	JSON::Status stats{ status, time.ToString(WTIME_FORMAT_STRING_ISO8601), message };

	if (_asyncClient->is_connected())
	{
		std::string body = JSON::to_json(stats);
		mqtt::message_ptr msg = mqtt::make_message(stream.str(), body);
		if (qos < 0 || qos > 2)
			msg->set_qos(m_mqtt_qos);
		else
			msg->set_qos(qos);
		_asyncClient->publish(msg, nullptr, *cb);
	}
}


void MqttClient::debugMQTT(std::uint16_t level, MQTTStatus status, const std::string &message)
{
	std::lock_guard<std::mutex> lock(m_lock);
	//quit if there is no MQTT communication going to take place
	if (!_asyncClient)
		return;

	std::ostringstream stream;
	stream << m_mqtt_topic << "/"s;
	stream << m_mqtt_id << "/"s;
	stream << m_mqtt_job << "/"s;
	stream << "debug"s;

	HSS_Time::WTime time = HSS_Time::WTime::Now(&m_timeManager, 0);
	JSON::Status stats{ status, time.ToString(WTIME_FORMAT_STRING_ISO8601), message };

	if (_asyncClient->is_connected())
	{
		std::string body = JSON::to_json(stats);
		mqtt::message_ptr msg = mqtt::make_message(stream.str(), body);
		msg->set_qos(m_mqtt_qos);
		_asyncClient->publish(msg, nullptr, *cb);
	}
}

void MqttClient::requestScenarioIndex(std::uint8_t count)
{
	std::lock_guard<std::mutex> lock(m_lock);

	sendRequestScenarioIndex(count);
}

void MqttClient::sendRequestScenarioIndex(std::uint8_t count)
{
	//quit if there is no MQTT communication going to take place
	if (!_asyncClient)
		return;

	std::ostringstream stream;
	stream << m_mqtt_topic << "/"s;
	stream << m_mqtt_id << "/delegator/balance"s;

	if (_asyncClient->is_connected())
	{
		auto body = (boost::format("{\"job\": \"%s\"}") % m_mqtt_job).str();
		for (int i = 0; i < count; i++)
		{
			mqtt::message_ptr msg = mqtt::make_message(stream.str(), body);
			msg->set_qos(m_mqtt_qos);
			_asyncClient->publish(msg, nullptr, *cb);
			m_private->m_scenarioRequestCounter++;
		}
	}
}

std::int32_t MqttClient::nextScenarioIndex(std::int32_t defaultValue)
{
	std::unique_lock<std::mutex> tlock(m_private->m_scenarioIndexLock);
	m_private->m_scenarioWaitingCounter++;

	{
		std::lock_guard<std::mutex> lock(m_lock);
		//there are currently no scenario indices available, request a new one
		if ((m_private->m_scenarioRequestCounter + m_private->m_scenarioIndices.size()) < m_private->m_scenarioWaitingCounter)
			sendRequestScenarioIndex(1);
	}

	int index;
	//wait for a scenario index to become available
	if (m_private->m_scenarioIndexPoke.wait_for(tlock, 10s, [&]() { return m_private->m_scenarioIndices.size() > 0; }))
	{

		index = m_private->m_scenarioIndices.front();
		m_private->m_scenarioIndices.erase(m_private->m_scenarioIndices.begin());
		m_private->m_scenarioWaitingCounter--;
	}
	else
		index = defaultValue;

	return index;
}

void MqttClient::updateMQTT(std::uint16_t level, MQTTStatus status, const std::string &message, const std::vector<std::tuple<std::string, Service::StatisticType>> &statistics)
{
	std::lock_guard<std::mutex> lock(m_lock);
	//quit if there is no MQTT communication going to take place
	if (!_asyncClient)
		return;

	std::ostringstream stream;
	stream << m_mqtt_topic << "/"s;
	stream << m_mqtt_id << "/"s;
	stream << m_mqtt_job << "/"s;
	stream << "status"s;

	HSS_Time::WTime time = HSS_Time::WTime::Now(&m_timeManager, 0);
	JSON::Status stats{ status, time.ToString(WTIME_FORMAT_STRING_ISO8601), message, statistics };

	if (_asyncClient->is_connected())
	{
		std::string body = JSON::to_json(stats);
		mqtt::message_ptr msg = mqtt::make_message(stream.str(), body);
		msg->set_qos(m_mqtt_qos);
		_asyncClient->publish(msg, nullptr, *cb);
	}
}

bool MqttClient::streamOutputFile(const std::string& filename, const std::string& coverageName,
	std::uint8_t type, bool exists, const std::string& managerId, google::protobuf::Message* message)
{
	std::lock_guard<std::mutex> lock(m_lock);
	bool retval = true;
	//quit if there is no MQTT communication going to take place
	if (_asyncClient)
	{
		if (managerId.length() > 0)
		{
			auto proto = dynamic_cast<WISE::ProjectProto::PrometheusData*>(message);

			if (proto && proto->outputlocation_size())
			{
				WISE::Comms::ServerProto::FileUpload builder;
				builder.set_filename(filename);
				builder.set_job(m_mqtt_job);
                builder.set_filetype((WISE::Comms::ServerProto::FileUpload_FileType)type);
				builder.set_exists(exists);

				for (auto& out : proto->outputlocation())
				{
					//streaming over MQTT is requested
					if (out.has_mqtt())
					{
						auto mqtt = builder.add_upload_type()->mutable_mqtt();
						if (m_mqtt_user.length() > 0)
							mqtt->set_username(m_mqtt_user);
						if (m_mqtt_pwd.length() > 0)
							mqtt->set_password(m_mqtt_pwd);
					}
					//uploading to geoserver is requested, don't stream summary files to geoserver
					else if (out.has_geoserver() && type != 1)
					{
						auto input = out.geoserver();
						auto geo = builder.add_upload_type()->mutable_geoserver();
						geo->set_password(input.password());
						geo->set_srs(input.srs());
						geo->set_url(input.url());
						geo->set_username(input.username());
						geo->set_workspace(input.workspace());
						geo->set_coverage(input.coverage());
						if (coverageName.size())
						{
							geo->set_allocated_raster_coverage(createProtobufObject(coverageName));
							geo->set_allocated_vector_coverage(createProtobufObject(coverageName));
						}
						else
						{
							if (input.has_raster_coverage())
								geo->set_allocated_raster_coverage(createProtobufObject(input.raster_coverage().value()));
							if (input.has_vector_coverage())
								geo->set_allocated_vector_coverage(createProtobufObject(input.vector_coverage().value()));
						}
					}
				}

				//if any valid upload types were found
				if (builder.upload_type_size())
				{
					//send the message to the manager that started WISE
					std::ostringstream stream;
					stream << m_mqtt_topic << "/"s;
					stream << m_mqtt_id << "/"s;
					stream << managerId << "/"s;
					stream << "file"s;

					std::string json;
					google::protobuf::util::JsonOptions options;
					options.add_whitespace = false; //always minimize
					options.always_print_primitive_fields = true;
					options.preserve_proto_field_names = true;

					google::protobuf::util::MessageToJsonString(builder, &json, options);

					if (_asyncClient->is_connected())
					{
						mqtt::message_ptr msg = mqtt::make_message(stream.str(), json);
						msg->set_qos(m_mqtt_qos);
						_asyncClient->publish(msg, nullptr, *cb);
					}
				}
			}
		}
	}

	return retval;
}

MqttClient::MqttClient()
	: m_mqtt_port(0),
	  m_mqtt_connected(false),
	  m_mqtt_topic("wise"),
	  m_callback(nullptr),
	  m_private(new MqttClientPrivate()),
	  m_worldLocation(0.0, 0.0, false),
	  m_timeManager(HSS_Time::WTimeManager::GetSystemTimeManager(m_worldLocation))
{
}

MqttClient::~MqttClient()
{
	deinitMQTT();
	delete m_private;
}
