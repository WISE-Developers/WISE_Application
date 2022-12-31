/**
 * WISE_Application: Initialization.proto.cpp
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
#include "SPARCS.h"
#include "misc_native.h"
#include "doubleBuilder.h"

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <exception>

#include <google/protobuf/util/json_util.h>

#include <functional>
#include "filesystem.hpp"
#include "validation_report.pb.h"


const std::optional<std::string> buildDetails(const std::shared_ptr<validation::validation_object>& object)
{
	if (object->errorMessage().has_value() && object->errorMessage().value().size())
		return object->errorMessage();
	else
	{
		std::uint8_t count = 0;
		std::string retval = "Values must be ";
		if (object->minimum().has_value())
		{
			std::string temp;
			bool add = false;

			if (object->minimum().value().is_inclusive)
				temp = ">= ";
			else
				temp = "> ";

			if (std::holds_alternative<double>(object->minimum().value().value))
			{
				auto val = std::get<double>(object->minimum().value().value);
				if (val != std::numeric_limits<double>::infinity())
				{
					add = true;
					temp += (boost::format("%.1f") % val).str();
				}
			}
			else if (std::holds_alternative<std::int32_t>(object->minimum().value().value))
			{
				auto val = std::get<std::int32_t>(object->minimum().value().value);
				if (val != std::numeric_limits<std::int32_t>::infinity())
				{
					add = true;
					temp += (boost::format("%d") % val).str();
				}
			}
			else
			{
				auto val = std::get<std::string>(object->minimum().value().value);
				if (val.size())
				{
					add = true;
					temp += val;
				}
			}

			if (add)
			{
				count++;
				retval += temp;
			}
		}

		if (object->maximum().has_value())
		{
			std::string temp;
			bool add = false;

			if (object->maximum().value().is_inclusive)
				temp = "<= ";
			else
				temp = "< ";

			if (std::holds_alternative<double>(object->maximum().value().value))
			{
				auto val = std::get<double>(object->maximum().value().value);
				if (val != std::numeric_limits<double>::infinity())
				{
					add = true;
					temp += (boost::format("%.1f") % val).str();
				}
			}
			else if (std::holds_alternative<std::int32_t>(object->maximum().value().value))
			{
				auto val = std::get<std::int32_t>(object->maximum().value().value);
				if (val != std::numeric_limits<std::int32_t>::infinity())
				{
					add = true;
					temp += (boost::format("%d") % val).str();
				}
			}
			else
			{
				auto val = std::get<std::string>(object->maximum().value().value);
				if (val.size())
				{
					add = true;
					temp += val;
				}
			}

			if (add)
			{
				if (count)
					retval += " and ";
				count++;
				retval += temp;
			}
		}

		if (count)
			return std::make_optional(retval);

		return std::nullopt;
	}
}


WISE::Comms::ValidationProto::ValidationReport_ValidationNode* buildValidationNode(const std::shared_ptr<validation::validation_object>& object)
{
	if (object->max_error_level() > validation::error_level::NONE)
	{
		auto retval = new WISE::Comms::ValidationProto::ValidationReport_ValidationNode();
		retval->set_protobuf_name(object->protobufObject());
		retval->set_error_level((WISE::Comms::ValidationProto::ValidationReport_ValidationNode::ErrorLevel)object->max_error_level());
		if (object->errorLevel() > validation::error_level::NONE)
		{
			if (object->objectName().size() != object->errorValue().size()) {
				delete retval;
				throw std::runtime_error("The number of object names doesn't match the number of error values.");
			}
			//have to step through both the name and value lists
			//The must be the same length otherwise just let things crash
			auto iter = object->errorValue().begin();
			for (auto& name : object->objectName())
			{
				auto err = retval->add_error_values();
				err->set_name(name);
				err->set_allocated_value(createProtobufObject(*iter));
				iter++;
			}

			retval->set_allocated_error_id(createProtobufObject(object->errorIdentifier()));

			auto details = buildDetails(object);
			if (details.has_value())
				retval->set_allocated_details(createProtobufObject(details.value()));

			return retval;
		}
		else
		{
			for (auto& name : object->objectName())
				retval->add_error_values()->set_name(name);

			for (auto& child : object->children())
			{
				auto proto = buildValidationNode(child);
				if (proto)
				{
					retval->add_children()->Swap(proto);
					delete proto;
				}
			}

			if (retval->children_size())
				return retval;
		}
	}
	return nullptr;
}


HRESULT SPARCS_P::LoadProtobuf(const char* filename, std::uint32_t &loadedFileVersion, std::function<std::string(void)> password_callback, SummaryType justSummary)
{
	HRESULT retval = S_OK;

	fs::path file(filename);
	if (boost::iequals(file.extension().string(), ".fgmb"))
		m_inputType = InputType::BINARY;
	else
		m_inputType = InputType::JSON;
	createProtoStatus(file.parent_path().string());

	m_id = file.replace_extension("").filename().string();

	m_rootValidation.reset();
	if (justSummary == SummaryType::VALIDATE)
		m_rootValidation = std::make_shared<validation::validation_object>("WISE", filename);

	try
	{
		retval = m_project->deserialize(filename, loadedFileVersion, password_callback, &m_proto, m_rootValidation, "project");
	}
	catch (std::exception&)
	{
		if (justSummary != SummaryType::VALIDATE)
			throw;
	}


	if (SUCCEEDED(retval))
	{
		job_name = extractJobName(m_proto);
		
		auto output = getExportOverride(m_proto);
		if (output >= 0)
			m_outputTypeOverride = (InputType)output;
		else
			m_outputTypeOverride = InputType::UNKNOWN;
		initializeTemporaryOutputs(m_proto, &m_tempOutputInterval, &m_keepTempFiles);
		initializeUnits(m_proto, &m_unitConversion);

		if (m_connectImmediately)
		{
			HRESULT r = ConnectMQTTProto(justSummary != SummaryType::NONE);
			//only fail completely if load balancing is requested, allow the job to continue if the only issue will be status reporting
			if (FAILED(r) && m_project->loadBalancing() != Project::LoadBalancingType::NONE) {
				retval = r;
				last_error = retval;
				last_error_msg = ErrorMsg(retval, nullptr);
			}
		}
	} else {
		last_error = retval;
		last_error_msg = ErrorMsg(retval, nullptr);
	}

	return retval;
}


HRESULT SPARCS_P::SaveValidationReport(const std::string& path)
{
	if (!m_rootValidation)
		throw std::logic_error("Attempt to save validation object without creating it.");
	m_project->CollectLoadWarnings();
	WISE::Comms::ValidationProto::ValidationReport report;
	report.set_success(true);
	report.set_valid(m_rootValidation->max_error_level() <= validation::error_level::WARNING);
	std::string loadWarnings = m_loadWarnings;
	loadWarnings += m_project->m_loadWarnings;
	report.set_allocated_load_warnings(createProtobufObject(loadWarnings));
	auto tree = buildValidationNode(m_rootValidation);
	if (tree)
		report.set_allocated_validation_tree(tree);

	fs::path file(path);

	try
	{
		if (fs::exists(file))
			fs::remove(file);
	}
	catch (fs::filesystem_error& e) {}

	std::string pretty;
	google::protobuf::util::JsonOptions options;
	options.add_whitespace = true;
	options.always_print_primitive_fields = true;
	options.preserve_proto_field_names = true;

	google::protobuf::util::MessageToJsonString(report, &pretty, options);

	std::ofstream out(file);
	out << pretty;
	out.close();

    return S_OK;
}


HRESULT SPARCS_P::ConnectMQTTProto(bool justSummary)
{
	std::uint8_t m_log_verbosity = 0;
	std::string m_mqtt_host;
	std::uint16_t m_mqtt_port = 1883;
	std::string m_mqtt_user;
	std::string m_mqtt_pwd;
	std::string m_mqtt_topic;
	std::int32_t qos;
	std::uint8_t m_mqtt_verbosity = 0;
	parseLogData(m_proto, &m_log_verbosity, &m_mqtt_host, &m_mqtt_port, &m_mqtt_user,
		&m_mqtt_pwd, &m_mqtt_topic, &qos, &m_mqtt_verbosity);

	switch (m_log_verbosity) {
	case 3:
		m_lv = 1;
		break;
	case 2:
		m_lv = 2;
		break;
	case 1:
		m_lv = 3;
		break;
	case 4:
		m_lv = 4;
		break;
	default:
		m_lv = 0;
		break;
	}

	if (m_mqtt_host.size() > 0 && m_mqtt_verbosity > 0)
	{
		m_client.m_mqtt_host = m_mqtt_host;
		m_client.m_mqtt_port = m_mqtt_port;
		if (m_mqtt_user.size() > 0)
			m_client.m_mqtt_user = m_mqtt_user;
		if (m_mqtt_pwd.size() > 0)
			m_client.m_mqtt_pwd = m_mqtt_pwd;
		if (m_mqtt_topic.size() > 0)
			m_client.m_mqtt_topic = m_mqtt_topic;

		if ((qos > -1) && (qos < 2))
			m_client.m_mqtt_qos = qos;

		switch (m_mqtt_verbosity)
		{
		case 3:
			m_client.m_mqtt_lv = 1;
			break;
		case 2:
			m_client.m_mqtt_lv = 2;
			break;
		case 1:
			m_client.m_mqtt_lv = 3;
			break;
		case 4:
			m_client.m_mqtt_lv = 4;
			break;
		default:
			m_client.m_mqtt_lv = 0;
			break;
		}

		if (!initMQTT())
		{
			UpdateStatus(0, WISEStatus::FAILED, "Failed to connect to MQTT broker.", -1, OUTPUT_STATUS_FILE);
			return ERROR_NETWORK_UNREACHABLE | ERROR_SEVERITY_WARNING;
		}
	}
	//don't broadcast that we exist if we're just printing summary files
	if (!justSummary)
		UpdateStatus(0, WISEStatus::STARTED, "WISE.EXE operations", 1, OUTPUT_STATUS_ALL);

	return S_OK;
}

HRESULT SPARCS_P::SaveProtobuf(const char *filename, std::uint32_t majorFileVersion, std::function < std::string(void)> password_callback)
{
	HRESULT retval = S_OK;

	retval = m_project->serialize(filename, majorFileVersion, [this](google::protobuf::Message* msg)
		{
			addProtobufSettings(msg, this->m_proto);
		},
		password_callback);

	return retval;
}

HRESULT SPARCS_P::SaveModifiedProtofbuf(const std::string& original, std::uint32_t majorFileVersion, std::function<std::string(void)> password_callback)
{
	HRESULT retval = S_OK;
	auto dirty = m_project->isdirty();
	InputType type;
	if (m_outputTypeOverride != InputType::UNKNOWN)
		type = m_outputTypeOverride;
	else
		type = m_inputType;

	if (type != InputType::UNKNOWN && ((dirty.has_value() && dirty.value()) || m_project->m_outputs.job.saveJob))
	{
		std::string filename;
		if (m_project->m_outputs.job.filename.length() > 0)
		{
			fs::path path(m_project->m_outputs.job.filename);
			path = path.replace_extension("");
			std::string name = path.filename().string();
			std::string ext;
			if (type == InputType::BINARY)
				ext = ".fgmb";
			else if (type == InputType::JSON || type == InputType::JSON_PRETTY)
				ext = ".fgmj";
			path = path.replace_extension(ext);
			int index = 1;
			while (fs::exists(path))
			{
				path = path.replace_filename(name + "_" + std::to_string(++index));
				path = path.replace_extension(ext);
			}
			filename = path.string();
		}
		else
		{
			fs::path path(original);
			path = path.replace_extension("");
			std::string name = path.filename().string();
			std::string ext;
			if (type == InputType::BINARY)
				ext = ".fgmb";
			else if (type == InputType::JSON || type == InputType::JSON_PRETTY)
				ext = ".fgmj";
			path = path.replace_extension(ext);
			int index = 1;
			while (fs::exists(path))
			{
				path = path.replace_filename(name + "_" + std::to_string(++index));
				path = path.replace_extension(ext);
			}
			filename = path.string();
		}
		retval = SaveProtobuf(filename.c_str(), majorFileVersion, password_callback);
	}
	return retval;
}

int SPARCS_P::protobufCoreCount()
{
	return extractCoreCount(m_proto);
}

std::string SPARCS_P::protobufJobName()
{
	return extractJobName(m_proto);
}
