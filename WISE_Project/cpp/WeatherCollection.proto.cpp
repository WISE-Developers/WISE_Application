/**
 * WISE_Project: WeatherCollection.proto.cpp
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
#include "WeatherCollection.h"
#include "WindSpeedGrid.h"
#include "WindDirectionGrid.h"
#include "WeatherGridFilter.h"
#include "CWFGM_WeatherGridFilter.h"
#include "CWFGMProject.h"

#include "cwfgmWeatherStream.pb.h"
#include "str_printf.h"
#include "doubleBuilder.h"



std::int32_t Project::WeatherStation::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message *Project::WeatherStation::serialize(const SerializeProtoOptions& options) {
	if (serialVersionUid(options) == 1) {
		auto station = new WISE::ProjectProto::ProjectWeatherStation();
		station->set_version(serialVersionUid(options));
		station->set_name(m_name);
		station->set_comments(m_comments);
		station->set_allocated_color(createProtobufObject((std::uint32_t)GetColor()));
		station->set_allocated_size(createProtobufObject((std::uint64_t)GetDisplaySize()));
		station->set_allocated_display(createProtobufObject((std::uint32_t)GetSymbol()));
		auto cwfgm = m_station->serialize(options);
		cwfgm->set_allocated_skipstream(createProtobufObject(true));
		station->set_allocated_station(cwfgm);

		WeatherStream* stream = m_streamList.LH_Head();
		while (stream->LN_Succ())
		{
			auto msg = dynamic_cast_assert<WISE::WeatherProto::CwfgmWeatherStream*>(stream->serialize(options));

			auto str = cwfgm->add_streams();
			str->Swap(msg);
			delete msg;

			stream = stream->LN_Succ();
		}

		return station;
	} else {
		auto station = m_station->serialize(options);
		station->set_name(m_name);
		if (m_comments.length())
			station->set_comments(m_comments);
		if (m_color.has_value())
			station->set_color(m_color.value());
		if (m_displaySize.has_value())
			station->set_size(m_displaySize.value());
		if (m_symbol.has_value())
			station->set_symbol(m_symbol.value());
		station->set_allocated_skipstream(createProtobufObject(true));

		WeatherStream* stream = m_streamList.LH_Head();
		while (stream->LN_Succ())
		{
			auto msg = dynamic_cast_assert<WISE::WeatherProto::CwfgmWeatherStream*>(stream->serialize(options));

			auto str = station->add_streams();
			str->Swap(msg);
			delete msg;

			stream = stream->LN_Succ();
		}

		return station;
	}
}


auto Project::WeatherStation::deserialize(const google::protobuf::Message& message, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> WeatherStation*
{
	auto station = dynamic_cast<const WISE::WeatherProto::CwfgmWeatherStation*>(&message);
	
	ICWFGM_CommonData data;
	data.m_timeManager = m_collection->m_project->m_timeManager;

	if (!station)
	{
		auto station = dynamic_cast<const WISE::ProjectProto::ProjectWeatherStation*>(&message);

		if (!station)
		{
			if (valid)
				/// <summary>
				/// The object passed as a weather station is invalid. An incorrect object type was passed to the parser.
				/// </summary>
				/// <type>internal</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWeatherStation", name, validation::error_level::SEVERE, validation::id::object_invalid, message.GetDescriptor()->name());
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("WeatherStation: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
		}
		if (station->version() != 1)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The weather station is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWeatherStation", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(station->version()));
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("WeatherStation: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validations for weather stations.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectWeatherStation", name);
		auto v = vt.lock();

		m_name = station->name();
		if (!station->name().length()) {
			if (v)
				/// <summary>
				/// The weather station's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, station->name());
		}

		m_comments = station->comments();
		if (station->has_color())
			m_color = station->color().value();
		if (station->has_size())
			m_displaySize = station->size().value();
		if (station->has_display())
			m_symbol = station->display().value();

		boost::intrusive_ptr<ICWFGM_GridEngine> engine;
		if (m_station)
			m_station->get_GridEngine(&engine);
		try {
			m_station = new CCWFGM_WeatherStation();
		}
		catch (std::exception& e) {
			/// <summary>
			/// The COM object could not be instantiated.
			/// </summary>
			/// <type>internal</type>
			if (v)
				v->add_child_validation("WISE.WeatherProto.CwfgmWeatherStation", "station", validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_WeatherStation");
			return nullptr;
		}
		if (engine) {
			m_station->put_CommonData(&data);
			m_station->put_GridEngine(engine.get());
		}
		if (!m_station->deserialize(station->station(), v, "station"))
		{
			weak_assert(false);;
			return nullptr;
		}

		for (int i = 0; i < station->station().streams_size(); i++)
		{
			auto str = station->station().streams(i);
			auto stream = New();
			stream->put_CommonData(&data);
			if (!stream->deserialize(str, v, strprintf("streams[%d]", i))) {
				weak_assert(false);;
				delete stream;
				return nullptr;
			}
			AddStream(stream);
		}
	} else {
		if (station->version() != 2)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The weather station is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWeatherStation", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(station->version()));
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("WeatherStation: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validations for weather stations.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectWeatherStation", name);
		auto v = vt.lock();

		if (station->has_name()) {
			m_name = station->name();
			if (!station->name().length()) {
				if (v)
					/// <summary>
					/// The weather station's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, station->name());
				throw ISerializeProto::DeserializeError("WISE.WeatherProto.CwfgmWeatherStation: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The weather station's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.WeatherProto.CwfgmWeatherStation: Invalid or missing name.");
		}
		if (station->has_comments())
			m_comments = station->comments();
		if (station->has_color())
			m_color = station->color();
		if (station->has_size())
			m_displaySize = station->size();
		if (station->has_symbol())
			m_symbol = station->symbol();

		boost::intrusive_ptr<ICWFGM_GridEngine> engine;
		if (m_station)
			m_station->get_GridEngine(&engine);
		try {
			m_station = new CCWFGM_WeatherStation();
		}
		catch (std::exception& e) {
			/// <summary>
			/// The COM object could not be instantiated.
			/// </summary>
			/// <type>internal</type>
			if (v)
				v->add_child_validation("WISE.WeatherProto.CwfgmWeatherStation", "station", validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_WeatherStation");
			return nullptr;
		}
		if (engine) {
			m_station->put_CommonData(&data);
			m_station->put_GridEngine(engine.get());
		}
		if (!m_station->deserialize(*station, v, "station"))
		{
			weak_assert(false);;
			return nullptr;
		}

		for (int i = 0; i < station->streams_size(); i++)
		{
			auto str = station->streams(i);
			auto stream = New();
			ICWFGM_CommonData data;
			data.m_timeManager = (WTimeManager*)&m_collection->m_project->m_timeManager;
			stream->put_CommonData(&data);
			if (!stream->deserialize(str, v, strprintf("streams[%d]", i))) {
				weak_assert(false);;
				delete stream;
				return nullptr;
			}
			AddStream(stream);
		}
	}
	return this;
}


std::int32_t Project::WeatherStream::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::WeatherStream::serialize(const SerializeProtoOptions& options) {
	auto stream = m_stream->serialize(options);
	if (stream)
	{
		stream->set_allocated_name(createProtobufObject(m_name));
		stream->set_allocated_comments(createProtobufObject(m_comments));
	}
	return stream;
}


auto Project::WeatherStream::deserialize(const google::protobuf::Message& message, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> WeatherStream* {
	auto sstream = dynamic_cast<const WISE::WeatherProto::CwfgmWeatherStream*>(&message);

	if (!sstream)
	{
		if (valid)
			/// <summary>
			/// The object passed as a weather stream is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.WeatherProto.CwfgmWeatherStream", name, validation::error_level::SEVERE, validation::id::object_invalid, message.GetDescriptor()->name());
		weak_assert(false);;
		throw ISerializeProto::DeserializeError("WeatherStream: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if ((sstream->version() != 1) && (sstream->version() != 2))
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The weather stream is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.WeatherProto.CwfgmWeatherStream", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(sstream->version()));
		weak_assert(false);;
		throw ISerializeProto::DeserializeError("WeatherStream: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validations for weather streams.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.WeatherProto.CwfgmWeatherStream", name);
	auto myValid = vt.lock();

	if (sstream->has_name()) {
		m_name = sstream->name().value();
		if (!sstream->name().value().length()) {
			if (myValid)
				/// <summary>
				/// The stream's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				myValid->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, sstream->name().value());
			return nullptr;
		}
	}
	else {
		if (myValid)
			/// <summary>
			/// The stream's name (to identify the output) is missing.
			/// </summary>
			/// <type>user</type>
			myValid->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, sstream->name().value());
		return nullptr;
	}
	if (sstream->has_comments())
		m_comments = sstream->comments().value();
	if (!m_stream->deserialize(*sstream, myValid, "stream"))
	{
		weak_assert(false);;
		return nullptr;
	}

	return this;
}


std::int32_t Project::CWindSpeedGrid::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::CWindSpeedGrid::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
		auto grid = new WISE::ProjectProto::ProjectWindGrid();
		grid->set_version(serialVersionUid(options));

		grid->set_type(WISE::WeatherProto::WindGrid_GridType::WindGrid_GridType_WindSpeed);
		grid->set_name(m_name);
		grid->set_comments(m_comments);

		auto filter = dynamic_cast<CCWFGM_WindSpeedGrid*>(m_filter.get());
		if (filter)
		{
			grid->set_allocated_grid(filter->serialize(options));
		}

		return grid;
	}
	else {
		auto filter = dynamic_cast<CCWFGM_WindSpeedGrid*>(m_filter.get());
		weak_assert(filter);
		if (filter) {
			auto grid = filter->serialize(options);

			grid->set_type(WISE::WeatherProto::WindGrid_GridType::WindGrid_GridType_WindSpeed);
			grid->set_name(m_name);
			if (m_comments.length())
				grid->set_comments(m_comments);

			return grid;
		}
		return nullptr;
	}
}


auto Project::CWindSpeedGrid::deserialize(const google::protobuf::Message& message, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> CWindSpeedGrid* {
	auto grid = dynamic_cast<const WISE::WeatherProto::WindGrid*>(&message);

	if (!grid)
	{
		auto grid = dynamic_cast<const WISE::ProjectProto::ProjectWindGrid*>(&message);

		if (!grid)
		{
			if (valid)
				/// <summary>
				/// The object passed as a wind speed grid is invalid. An incorrect object type was passed to the parser.
				/// </summary>
				/// <type>internal</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWindGrid", name, validation::error_level::SEVERE, validation::id::object_invalid, message.GetDescriptor()->name());
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("CWindSpeedGrid: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
		}

		if (grid->version() != 1)
		{
			if (valid)
				/// <summary>
				/// A wind direction grid has been specified in place of a wind speed grid.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWindGrid", name, validation::error_level::SEVERE, "Deserialize.Type:Invalid", "WISE.WeatherProto.WindGrid_GridType.WindGrid_GridType_WindDirection");
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("CWindSpeedGrid: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}
		if (grid->type() != WISE::WeatherProto::WindGrid_GridType::WindGrid_GridType_WindSpeed)
		{
			weak_assert(false);;
			throw std::invalid_argument("CWindSpeedGrid: Type is invalid");
		}

		/// <summary>
		/// Child validations for wind speed grids.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectWindGrid", name);
		auto v = vt.lock();

		m_name = grid->name();
		if (!grid->name().length()) {
			if (v)
				/// <summary>
				/// The stream's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, grid->name());
			return nullptr;
		}
		m_comments = grid->comments();

		if (grid->has_grid())
		{
			if (!m_filter) {
				try {
					m_filter = new CCWFGM_WindSpeedGrid();
				}
				catch (std::exception& e) {
					/// <summary>
					/// The COM object could not be instantiated.
					/// </summary>
					/// <type>internal</type>
					if (v)
						v->add_child_validation("WISE.WeatherProto.WindGrid", "grid", validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_WindSpeedGrid");
					return nullptr;
				}
			}
			auto filter = dynamic_cast<CCWFGM_WindSpeedGrid*>(m_filter.get());
			weak_assert(filter);
			if (filter) {
				if (!filter->deserialize(grid->grid(), v, "grid")) {
					weak_assert(false);;
					return nullptr;
				}
			}
		}

	} else {

		if (grid->version() != 2)
		{
			if (valid)
				/// <summary>
				/// A wind direction grid has been specified in place of a wind speed grid.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWindGrid", name, validation::error_level::SEVERE, "Deserialize.Type:Invalid", "WISE.WeatherProto.WindGrid_GridType.WindGrid_GridType_WindDirection");
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("CWindSpeedGrid: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}
		if (grid->type() != WISE::WeatherProto::WindGrid_GridType::WindGrid_GridType_WindSpeed)
		{
			weak_assert(false);;
			throw std::invalid_argument("CWindSpeedGrid: Type is invalid");
		}

		/// <summary>
		/// Child validations for wind speed grids.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectWindGrid", name);
		auto v = vt.lock();

		if (grid->has_name()) {
			m_name = grid->name();
			if (!grid->name().length()) {
				if (v)
					/// <summary>
					/// The stream's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, grid->name());
				throw ISerializeProto::DeserializeError("WISE.WeatherProto.WindGrid: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The stream's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.WeatherProto.WindGrid: Invalid or missing name.");
		}
		if (grid->has_comments())
			m_comments = grid->comments();

		if (!m_filter) {
			try {
				m_filter = new CCWFGM_WindSpeedGrid();
			}
			catch (std::exception& e) {
				/// <summary>
				/// The COM object could not be instantiated.
				/// </summary>
				/// <type>internal</type>
				if (v)
					v->add_child_validation("WISE.WeatherProto.WindGrid", "grid", validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_WindSpeedGrid");
				return nullptr;
			}
		}
		auto filter = dynamic_cast<CCWFGM_WindSpeedGrid*>(m_filter.get());
		weak_assert(filter);
		if (filter) {
			if (!filter->deserialize(*grid, v, "grid")) {
				weak_assert(false);;
				return nullptr;
			}
		}
	}
	return this;
}


std::int32_t Project::CWindDirectionGrid::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::CWindDirectionGrid::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
		auto grid = new WISE::ProjectProto::ProjectWindGrid();
		grid->set_version(serialVersionUid(options));

		grid->set_type(WISE::WeatherProto::WindGrid_GridType::WindGrid_GridType_WindDirection);
		grid->set_name(m_name);
		grid->set_comments(m_comments);

		auto filter = dynamic_cast<CCWFGM_WindDirectionGrid*>(m_filter.get());
		if (filter)
		{
			grid->set_allocated_grid(filter->serialize(options));
		}

		return grid;
	} else {
		auto filter = dynamic_cast<CCWFGM_WindDirectionGrid*>(m_filter.get());
		weak_assert(filter);
		if (filter) {
			auto grid = filter->serialize(options);

			grid->set_type(WISE::WeatherProto::WindGrid_GridType::WindGrid_GridType_WindDirection);
			grid->set_name(m_name);
			if (m_comments.length())
				grid->set_comments(m_comments);

			return grid;
		}
		return nullptr;
	}
}

auto Project::CWindDirectionGrid::deserialize(const google::protobuf::Message& message, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> CWindDirectionGrid*
{
	auto grid = dynamic_cast<const WISE::WeatherProto::WindGrid*>(&message);

	if (!grid)
	{
		auto grid = dynamic_cast<const WISE::ProjectProto::ProjectWindGrid*>(&message);

		if (!grid)
		{
			if (valid)
				/// <summary>
				/// The object passed as a wind direction grid is invalid. An incorrect object type was passed to the parser.
				/// </summary>
				/// <type>internal</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWindGrid", name, validation::error_level::SEVERE, validation::id::object_invalid, message.GetDescriptor()->name());
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("CWindDirectionGrid: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
		}
		if (grid->version() != 1)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The wind direction grid is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWindGrid", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(grid->version()));
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("CWindDirectionGrid: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}
		if (grid->type() != WISE::WeatherProto::WindGrid_GridType::WindGrid_GridType_WindDirection)
		{
			if (valid)
				/// <summary>
				/// A wind speed grid has been specified in place of a wind direction grid.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWindGrid", name, validation::error_level::SEVERE, validation::id::grid_type_invalid, "WISE.WeatherProto.WindGrid_GridType.WindGrid_GridType_WindSpeed");
			weak_assert(false);;
			throw std::invalid_argument("CWindDirectionGrid: Type is invalid");
		}

		/// <summary>
		/// Child validations for wind direction grids.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectWindGrid", name);
		auto v = vt.lock();

		m_name = grid->name();
		if (!grid->name().length()) {
			if (v)
				/// <summary>
				/// The stream's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, grid->name());
			return nullptr;
		}
		m_comments = grid->comments();

		if (grid->has_grid())
		{
			if (!m_filter) {
				try {
					m_filter = new CCWFGM_WindDirectionGrid();
				}
				catch (std::exception& e) {
					/// <summary>
					/// The COM object could not be instantiated.
					/// </summary>
					/// <type>internal</type>
					if (v)
						v->add_child_validation("WISE.WeatherProto.WindGrid", "grid", validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_WindDirectionGrid");
					return nullptr;
				}
			}
			auto filter = dynamic_cast<CCWFGM_WindDirectionGrid*>(m_filter.get());
			weak_assert(filter);
			if (filter) {
				if (!filter->deserialize(grid->grid(), v, "grid")) {
					weak_assert(false);;
					return nullptr;
				}
			}
		}

	} else {

		if (grid->version() != 2)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The wind direction grid is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWindGrid", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(grid->version()));
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("CWindDirectionGrid: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}
		if (grid->type() != WISE::WeatherProto::WindGrid_GridType::WindGrid_GridType_WindDirection)
		{
			if (valid)
				/// <summary>
				/// A wind speed grid has been specified in place of a wind direction grid.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWindGrid", name, validation::error_level::SEVERE, validation::id::grid_type_invalid, "WISE.WeatherProto.WindGrid_GridType.WindGrid_GridType_WindSpeed");
			weak_assert(false);;
			throw std::invalid_argument("CWindDirectionGrid: Type is invalid");
		}

		/// <summary>
		/// Child validations for wind direction grids.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectWindGrid", name);
		auto v = vt.lock();

		if (grid->has_name()) {
			m_name = grid->name();
			if (!grid->name().length()) {
				if (v)
					/// <summary>
					/// The stream's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, grid->name());
				throw ISerializeProto::DeserializeError("WISE.WeatherProto.WindGrid: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The stream's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.WeatherProto.WindGrid: Invalid or missing name.");
		}
		if (grid->has_comments())
			m_comments = grid->comments();

		if (!m_filter) {
			try {
				m_filter = new CCWFGM_WindDirectionGrid();
			}
			catch (std::exception& e) {
				/// <summary>
				/// The COM object could not be instantiated.
				/// </summary>
				/// <type>internal</type>
				if (v)
					v->add_child_validation("WISE.WeatherProto.WindGrid", "grid", validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_WindDirectionGrid");
				return nullptr;
			}
		}
		auto filter = dynamic_cast<CCWFGM_WindDirectionGrid*>(m_filter.get());
		weak_assert(filter);
		if (filter) {
			if (!filter->deserialize(*grid, v, "grid")) {
				weak_assert(false);;
				return nullptr;
			}
		}
	}

	return this;
}


std::int32_t Project::CWeatherGridFilter::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::CWeatherGridFilter::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
		auto filter = new WISE::ProjectProto::ProjectWeatherGridFilter();
		filter->set_version(serialVersionUid(options));

		filter->set_name(m_name);
		filter->set_comments(m_comments);

		auto base = dynamic_cast<CCWFGM_WeatherGridFilter*>(m_filter.get());
		if (base)
		{
			filter->set_allocated_filter(base->serialize(options));
		}

		return filter;

	} else {

		auto base = dynamic_cast<CCWFGM_WeatherGridFilter*>(m_filter.get());
		weak_assert(base);
		if (base) {
			auto filter = base->serialize(options);
			weak_assert(filter);
			if (filter) {
				filter->set_name(m_name);
				if (m_comments.length())
					filter->set_comments(m_comments);

				return filter;
			}
		}
		return nullptr;
	}
}

auto Project::CWeatherGridFilter::deserialize(const google::protobuf::Message& message, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> CWeatherGridFilter*
{
	auto filter = dynamic_cast<const WISE::WeatherProto::WeatherGridFilter*>(&message);

	if (!filter)
	{
		auto filter = dynamic_cast<const WISE::ProjectProto::ProjectPolyWeatherGridFilter*>(&message);
		if (filter) {
			if (filter->version() != 1)
			{
				if (valid)
					/// <summary>
					/// The object version is not supported. The weather polygon filter is not supported by this version of Prometheus.
					/// </summary>
					/// <type>user</type>
					valid->add_child_validation("WISE.ProjectProto.ProjectPolyWeatherGridFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
				weak_assert(false);;
				throw ISerializeProto::DeserializeError("CWeatherPolyFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
			}

			/// <summary>
			/// Child validations for weather polygon filters.
			/// </summary>
			auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectPolyWeatherGridFilter", name);
			auto v = vt.lock();

			m_name = filter->name();
			if (!filter->name().length()) {
				if (v)
					/// <summary>
					/// The stream's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
				return nullptr;
			}
			m_comments = filter->comments();
			if (filter->landscape())
			{
				auto hack = const_cast<WISE::WeatherProto::WeatherGridFilter*>(&filter->filter());
				hack->set_landscape(true);
			}
			else
			{
				weak_assert(false);;
			}

			if (filter->has_filter())
			{
				auto base = dynamic_cast<CCWFGM_WeatherGridFilter*>(m_filter.get());
				if (base)
				{
					SerializeWeatherGridFilterData data;
					data.permissible_drivers = &CWFGMProject::m_permissibleVectorReadDriversSA;
					if (!base->deserialize(filter->filter(), v, "filter", &data)) {
						weak_assert(false);;
						return nullptr;
					}
				}
			}

		} else {

			auto filter = dynamic_cast<const WISE::ProjectProto::ProjectWeatherGridFilter*>(&message);

			if (!filter)
			{
				if (valid)
					/// <summary>
					/// The object passed as a weather grid filter is invalid. An incorrect object type was passed to the parser.
					/// </summary>
					/// <type>internal</type>
					valid->add_child_validation("WISE.ProjectProto.ProjectWeatherGridFilter", name, validation::error_level::SEVERE, validation::id::object_invalid, message.GetDescriptor()->name());
				weak_assert(false);;
				throw ISerializeProto::DeserializeError("CWeatherGridFilter: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
			}
			if (filter->version() != 1)
			{
				if (valid)
					/// <summary>
					/// The object version is not supported. The weather grid filter is not supported by this version of Prometheus.
					/// </summary>
					/// <type>user</type>
					valid->add_child_validation("WISE.ProjectProto.ProjectWeatherGridFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
				weak_assert(false);;
				throw ISerializeProto::DeserializeError("CWeatherGridFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
			}

			/// <summary>
			/// Child validations for wind direction grids.
			/// </summary>
			auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectWeatherGridFilter", name);
			auto v = vt.lock();

			m_name = filter->name();
			if (!filter->name().length()) {
				if (v)
					/// <summary>
					/// The stream's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
				return nullptr;
			}
			m_comments = filter->comments();

			if (filter->has_filter())
			{
				auto base = dynamic_cast<CCWFGM_WeatherGridFilter*>(m_filter.get());
				if (base)
				{
					SerializeWeatherGridFilterData data;
					data.permissible_drivers = &CWFGMProject::m_permissibleVectorReadDriversSA;
					if (!base->deserialize(filter->filter(), v, "filter", &data))
					{
						weak_assert(false);;
						return nullptr;
					}
				}
			}
		}
	}
	else
	{
		if (filter->version() != 2)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The weather grid filter is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectWeatherGridFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("CWeatherGridFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validations for wind direction grids.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectWeatherGridFilter", name);
		auto v = vt.lock();

		if (filter->has_name()) {
			m_name = filter->name();
			if (!filter->name().length()) {
				if (v)
					/// <summary>
					/// The stream's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
				throw ISerializeProto::DeserializeError("WISE.ProjectProto.ProjectWeatherGridFilter: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The stream's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.ProjectProto.ProjectWeatherGridFilter: Invalid or missing name.");
		}
		if (filter->has_comments())
			m_comments = filter->comments();

		auto base = dynamic_cast<CCWFGM_WeatherGridFilter*>(m_filter.get());
		if (base)
		{
			SerializeWeatherGridFilterData data;
			data.permissible_drivers = &CWFGMProject::m_permissibleVectorReadDriversSA;
			if (!base->deserialize(*filter, v, "filter", &data))
			{
				weak_assert(false);;
				return nullptr;
			}
		}
	}
	return this;
}


std::int32_t Project::CWeatherPolyFilter::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::CWeatherPolyFilter::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
		auto filter = new WISE::ProjectProto::ProjectPolyWeatherGridFilter();
		filter->set_version(serialVersionUid(options));

		filter->set_name(m_name);
		filter->set_comments(m_comments);
		filter->set_landscape((m_flags & 0x1));
		if (!filter->landscape())
		{
			filter->set_allocated_color(createProtobufObject((std::uint32_t)m_color));
			filter->set_allocated_size(createProtobufObject((std::uint64_t)m_lineWidth));
		}

		auto base = dynamic_cast<CCWFGM_WeatherGridFilter*>(m_filter.get());
		if (base)
		{
			filter->set_allocated_filter(base->serialize(options));
		}

		return filter;

	} else {

		auto base = dynamic_cast<CCWFGM_WeatherGridFilter*>(m_filter.get());
		weak_assert(base);
		if (base) {
			auto filter = base->serialize(options);
			weak_assert(filter);
			if (filter) {
				filter->set_name(m_name);
				if (m_comments.length())
					filter->set_comments(m_comments);
				if (filter->has_polygons())
				{
					filter->set_color(m_color);
					filter->set_size(m_lineWidth);
				}

				return filter;
			}
		}
		return nullptr;
	}
}

static XY_Point *doLoad()
{
	XY_Point *p = new XY_Point[5];
	p[0].x = -1.0;		p[0].y = -1.0;
	p[1].x = -1.0;		p[1].y = -1.0;
	p[2].x = -1.0;		p[2].y = -1.0;
	p[3].x = -1.0;		p[3].y = -1.0;
	p[4].x = -1.0;		p[4].y = -1.0;

	return p;
}

auto Project::CWeatherPolyFilter::deserialize(const google::protobuf::Message& message, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> CWeatherPolyFilter*
{
	auto filter = dynamic_cast<const WISE::WeatherProto::WeatherGridFilter*>(&message);

	if (!filter)
	{
		auto filter = dynamic_cast<const WISE::ProjectProto::ProjectPolyWeatherGridFilter*>(&message);

		if (!filter)
		{
			if (valid)
				/// <summary>
				/// The object passed as a weather polygon filter is invalid. An incorrect object type was passed to the parser.
				/// </summary>
				/// <type>internal</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectPolyWeatherGridFilter", name, validation::error_level::SEVERE, validation::id::object_invalid, message.GetDescriptor()->name());
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("CWeatherPolyFilter: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
		}
		if (filter->version() != 1)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The weather polygon filter is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectPolyWeatherGridFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("CWeatherPolyFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validations for weather polygon filters.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectPolyWeatherGridFilter", name);
		auto v = vt.lock();

		m_name = filter->name();
		if (!filter->name().length()) {
			if (v)
				/// <summary>
				/// The stream's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
			return nullptr;
		}
		m_comments = filter->comments();
		if (filter->landscape())
		{
			XY_Point* p = doLoad();
			AddPolygon(*p, 5);
			delete[] p;
			SetLandscapeWxFlag(true);

			//this is a dirty way to pass the info to the grid engine
			if (filter->has_filter() && filter->filter().shape_case() == WISE::WeatherProto::WeatherGridFilter::SHAPE_NOT_SET)
			{
				auto hack = const_cast<WISE::WeatherProto::WeatherGridFilter*>(&filter->filter());
				hack->set_landscape(true);
			}
		}
		else
		{
			if (filter->has_color())
				m_color = filter->color().value();
			if (filter->has_size())
				m_lineWidth = filter->size().value();
		}

		if (filter->has_filter())
		{
			auto base = dynamic_cast<CCWFGM_WeatherGridFilter*>(m_filter.get());
			if (base)
			{
				SerializeWeatherGridFilterData data;
				data.permissible_drivers = &CWFGMProject::m_permissibleVectorReadDriversSA;
				if (!base->deserialize(filter->filter(), v, "filter", &data)) {
					weak_assert(false);;
					return nullptr;
				}
			}
		}

	} else {

		if (filter->version() != 2)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The weather polygon filter is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectPolyWeatherGridFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("CWeatherPolyFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validations for weather polygon filters.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectPolyWeatherGridFilter", name);
		auto v = vt.lock();

		if (filter->has_name()) {
			m_name = filter->name();
			if (!filter->name().length()) {
				if (v)
					/// <summary>
					/// The stream's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
				throw ISerializeProto::DeserializeError("WISE.ProjectProto.ProjectPolyWeatherGridFilter: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The stream's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.ProjectProto.ProjectPolyWeatherGridFilter: Invalid or missing name.");
		}
		if (filter->has_comments())
			m_comments = filter->comments();
		if (filter->landscape())
		{
			XY_Point* p = doLoad();
			AddPolygon(*p, 5);
			delete[] p;
			SetLandscapeWxFlag(true);

			//this is a dirty way to pass the info to the grid engine
			if (filter->shape_case() == WISE::WeatherProto::WeatherGridFilter::SHAPE_NOT_SET)
			{
				((WISE::WeatherProto::WeatherGridFilter*)filter)->set_landscape(true);
			}
		}
		else
		{
			if (filter->has_color())
				m_color = filter->color();
			if (filter->has_size())
				m_lineWidth = filter->size();
		}

		auto base = dynamic_cast<CCWFGM_WeatherGridFilter*>(m_filter.get());
		if (base)
		{
			SerializeWeatherGridFilterData data;
			data.permissible_drivers = &CWFGMProject::m_permissibleVectorReadDriversSA;
			if (!base->deserialize(*filter, v, "filter", &data)) {
				weak_assert(false);;
				return nullptr;
			}
		}
	}
	return this;
}


std::int32_t Project::WeatherStationCollection::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


WISE::ProjectProto::StationCollection* Project::WeatherStationCollection::serialize(const SerializeProtoOptions& options) {
	auto collection = new WISE::ProjectProto::StationCollection();
	collection->set_version(serialVersionUid(options));

	WeatherStation *station = m_stationList.LH_Head();
	while (station->LN_Succ()) {
		if (options.fileVersion() == 1) {
			auto exp = collection->add_stations();
			auto ser = station->serialize(options);
			exp->Swap(dynamic_cast_assert<WISE::ProjectProto::ProjectWeatherStation*>(ser));
			delete ser;
		}
		else {
			auto exp = collection->add_wxstationdata();
			auto ser = station->serialize(options);
			exp->Swap(dynamic_cast_assert<WISE::WeatherProto::CwfgmWeatherStation*>(ser));
			delete ser;
		}
		station = station->LN_Succ();
	}

	return collection;
}


auto Project::WeatherStationCollection::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> WeatherStationCollection*
{
	auto collection = dynamic_cast<const WISE::ProjectProto::StationCollection*>(&proto);

	if (!collection)
	{
		if (valid)
			/// <summary>
			/// The object passed as a weather station collection is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.StationCollection", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(false);;
		throw ISerializeProto::DeserializeError("WeatherStationCollection: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if ((collection->version() != 1) &&
		(collection->version() != 2))
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The weather station collection is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.StationCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(collection->version()));
		weak_assert(false);;
		throw ISerializeProto::DeserializeError("WeatherStationCollection: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validations for weather station collections.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.StationCollection", name);
	auto v = vt.lock();

	ICWFGM_CommonData* data;
	m_project->grid()->GetCommonData(nullptr, &data);

	if (collection->version() == 1) {
		for (int i = 0; i < collection->stations_size(); i++)
		{
			auto station = new WeatherStation(this);
			station->m_station->put_CommonData(data);
			station->m_station->put_GridEngine(m_project->gridEngine());
			if (!station->deserialize(collection->stations(i), v, strprintf("stations[%d]", i)))
			{
				weak_assert(false);;
				delete station;
				return nullptr;
			}

			AddStation(station);
		}
	}
	else {
		for (int i = 0; i < collection->wxstationdata_size(); i++)
		{
			auto station = new WeatherStation(this);
			station->m_station->put_CommonData(data);
			station->m_station->put_GridEngine(m_project->gridEngine());
			if (!station->deserialize(collection->wxstationdata(i), v, strprintf("stations[%d]", i)))
			{
				weak_assert(false);;
				delete station;
				return nullptr;
			}

			AddStation(station);
		}
	}
	return this;
}
