/**
 * WISE_Project: Scenario.proto.cpp
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
#include <winerror.h>
#endif
#include "types.h"
#include "projectScenario.pb.h"
#include "WTime.h"
#include "ScenarioCollection.h"
#include "CWFGMProject.h"
#include "FuelCom_ext.h"
#include "str_printf.h"
#include "doubleBuilder.h"


std::int32_t Project::Scenario::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion() + 1;
}


google::protobuf::Message* Project::Scenario::serialize(const SerializeProtoOptions& options) {
	throw std::logic_error("Incorrect serialization method");
}


google::protobuf::Message* Project::Scenario::serialize(const SerializeProtoOptions& options, ISerializationData* userData)
{
	ScenarioSerializationData *data = dynamic_cast<ScenarioSerializationData*>(userData);

	if (!data)
	{
		weak_assert(false);
		throw std::invalid_argument("Scenario: Parameter invalid");
	}

	if (options.fileVersion() == 1) {
		auto scenario = new WISE::ProjectProto::ProjectScenario();
		scenario->set_version(serialVersionUid(options));

		scenario->set_name(m_name);
		scenario->set_comments(m_comments);
		scenario->set_allocated_scenario(m_scenario->serialize(options));

		scenario->set_allocated_temporalconditions(m_temporalFilter->serialize(options));

		IgnitionOptions* io = m_fireList.LH_Head();
		while (io->LN_Succ())
		{
			auto reference = scenario->add_fireindex();
			auto fire = io->LN_Ptr();
			reference->set_name(fire->m_name);
			io = io->LN_Succ();
		}

		auto primaryStream = GetPrimaryStream();
		auto stream = m_streamList.LH_Head();
		while (stream->LN_Succ())
		{
			auto weather = scenario->add_weatherindex();
			weather->mutable_streamindex()->set_name(stream->LN_Ptr()->m_name);
			weather->mutable_stationindex()->set_name(stream->LN_Ptr()->m_weatherStation()->m_name);
			if (primaryStream == stream->LN_Ptr())
				weather->set_allocated_isprimary(createProtobufObject(true));
			//if the weather stream has sub-scenario options, serialize them
			if (stream->m_valid)
			{
				auto options1 = weather->mutable_weatheroptions();
				options1->set_subname(stream->m_name);
				options1->set_allocated_starttime(HSS_Time::Serialization::TimeSerializer::serializeTime(stream->m_startTime, options.fileVersion()));
				options1->set_allocated_endtime(HSS_Time::Serialization::TimeSerializer::serializeTime(stream->m_endTime, options.fileVersion()));
				if (stream->m_ignitionTime.GetTotalMicroSeconds())
					options1->set_allocated_ignitiontime(HSS_Time::Serialization::TimeSerializer::serializeTime(stream->m_ignitionTime, options.fileVersion()));
			}
			stream = stream->LN_Succ();
		}

		if (m_filterList.GetCount() > 0)
		{
			GFNode* pn = m_filterList.LH_Head();
			while (pn->LN_Succ())
			{
				auto filter = scenario->add_filterindex();
				filter->set_name(pn->LN_Ptr()->m_name);

				pn = pn->LN_Succ();
			}
		}

		RefNode<Vector>* pn = m_vFilterList.LH_Head();
		while (pn->LN_Succ())
		{
			auto vector = scenario->add_vectorindex();
			vector->set_name(pn->LN_Ptr()->Name());

			pn = pn->LN_Succ();
		}

		RefNode<Asset>* an = m_assetList.LH_Head();
		while (an->LN_Succ())
		{
			auto asset = scenario->add_assetindex();
			asset->set_name(an->LN_Ptr()->Name());

			if (m_scenarioCollection->m_project->m_assetCollection.GetCount()) {
				auto ao = new WISE::ProjectProto::ProjectScenario::AssetOptions();

				std::uint32_t mode;
				m_scenario->GetAssetOperation(an->LN_Ptr()->m_filter.get(), &mode);
				if (mode == ((std::uint32_t)-1))
					ao->set_operation(WISE::ProjectProto::ProjectScenario::STOP_AFTER_ALL);
				else if (mode == 1)
					ao->set_operation(WISE::ProjectProto::ProjectScenario::STOP_IMMEDIATE);
				else if (mode != 0) {
					ao->set_operation(WISE::ProjectProto::ProjectScenario::STOP_AFTER_X);
					ao->set_allocated_collisioncount(createProtobufObject((std::int32_t)mode));
				}
				else
					// this is the default operation - stop when all asset geometries are reached
					ao->set_operation(WISE::ProjectProto::ProjectScenario::NO_EFFECT);

				asset->set_allocated_assetoptions(ao);
			}

			an = an->LN_Succ();
		}

		ICWFGM_Target* target = nullptr;
		unsigned long target_idx, target_sub_idx;
		if (SUCCEEDED(m_scenario->GetWindTarget(&target, &target_idx, &target_sub_idx)))
			if (target) {
				auto t = new ::WISE::ProjectProto::ProjectScenario_TargetReference();
				t->set_index(data->targetCollection.IndexOf(target));
				t->set_geometryindex(target_idx);
				t->set_pointindex(target_sub_idx);
				scenario->set_allocated_windtarget(t);
			}
		target = nullptr;
		if (SUCCEEDED(m_scenario->GetVectorTarget(&target, &target_idx, &target_sub_idx)))
			if (target) {
				auto t = new ::WISE::ProjectProto::ProjectScenario_TargetReference();
				t->set_index(data->targetCollection.IndexOf(target));
				t->set_geometryindex(target_idx);
				t->set_pointindex(target_sub_idx);
				scenario->set_allocated_vectortarget(t);
			}

		if (m_scenarioCollection->m_project->m_assetCollection.GetCount()) {
			std::uint32_t mode;
			m_scenario->GetAssetOperation(nullptr, &mode);
			if (mode == ((std::uint32_t)-1))
				scenario->set_globalassetoperation(WISE::ProjectProto::ProjectScenario::STOP_AFTER_ALL);
			else if (mode == 1)
				scenario->set_globalassetoperation(WISE::ProjectProto::ProjectScenario::STOP_IMMEDIATE);
			else if (mode != 0) {
				scenario->set_globalassetoperation(WISE::ProjectProto::ProjectScenario::STOP_AFTER_X);
				scenario->set_allocated_globalcollisioncount(createProtobufObject((std::int32_t)mode));
			}
			else
				scenario->set_globalassetoperation(WISE::ProjectProto::ProjectScenario::NO_EFFECT);
		}

		auto idw = new WISE::ProjectProto::ProjectScenario_IdwExponent();
		PolymorphicAttribute attr;
		double d;
		m_weatherGrid->GetAttribute(CWFGM_WEATHER_OPTION_ADIABATIC_IDW_EXPONENT_TEMP, &attr);
		if (SUCCEEDED(VariantToDouble_(attr, &d)))
			idw->set_allocated_temperature(DoubleBuilder().withValue(d).forProtobuf(options.useVerboseFloats()));

		m_weatherGrid->GetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_WS, &attr);
		if (SUCCEEDED(VariantToDouble_(attr, &d)))
			idw->set_allocated_windspeed(DoubleBuilder().withValue(d).forProtobuf(options.useVerboseFloats()));

		m_weatherGrid->GetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_PRECIP, &attr);
		if (SUCCEEDED(VariantToDouble_(attr, &d)))
			idw->set_allocated_precipitation(DoubleBuilder().withValue(d).forProtobuf(options.useVerboseFloats()));

		m_weatherGrid->GetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_FWI, &attr);
		if (SUCCEEDED(VariantToDouble_(attr, &d)))
			idw->set_allocated_fwi(DoubleBuilder().withValue(d).forProtobuf(options.useVerboseFloats()));
		scenario->set_allocated_idw(idw);

		return scenario;

	}
	else {

		auto scenario = m_scenario->serialize(options);

		scenario->set_name(m_name);
		scenario->set_comments(m_comments);

		scenario->set_allocated_temporalconditions(m_temporalFilter->serialize(options));

		auto components = new WISE::FireEngineProto::CwfgmScenario::Components();
		scenario->set_allocated_components(components);

		IgnitionOptions* io = m_fireList.LH_Head();
		while (io->LN_Succ())
		{
			auto reference = components->add_fireindex();
			auto fire = io->LN_Ptr();
			reference->set_name(fire->m_name);
			io = io->LN_Succ();
		}

		auto primaryStream = GetPrimaryStream();
		auto stream = m_streamList.LH_Head();
		while (stream->LN_Succ())
		{
			auto weather = components->add_weatherindex();
			weather->mutable_streamindex()->set_name(stream->LN_Ptr()->m_name);
			weather->mutable_stationindex()->set_name(stream->LN_Ptr()->m_weatherStation()->m_name);
			if (primaryStream == stream->LN_Ptr())
				weather->set_isprimary(createProtobufObject(true));
			//if the weather stream has sub-scenario options, serialize them
			if (stream->m_valid)
			{
				auto options1 = weather->mutable_weatheroptions();
				options1->set_subname(stream->m_name);
				options1->set_allocated_starttime(HSS_Time::Serialization::TimeSerializer::serializeTime(stream->m_startTime, options.fileVersion()));
				options1->set_allocated_endtime(HSS_Time::Serialization::TimeSerializer::serializeTime(stream->m_endTime, options.fileVersion()));
				if (stream->m_ignitionTime.GetTotalMicroSeconds())
					options1->set_allocated_ignitiontime(HSS_Time::Serialization::TimeSerializer::serializeTime(stream->m_ignitionTime, options.fileVersion()));
			}
			stream = stream->LN_Succ();
		}

		if (m_filterList.GetCount() > 0)
		{
			GFNode* pn = m_filterList.LH_Head();
			while (pn->LN_Succ())
			{
				auto filter = components->add_filterindex();
				filter->set_name(pn->LN_Ptr()->m_name);

				pn = pn->LN_Succ();
			}
		}

		RefNode<Vector>* pn = m_vFilterList.LH_Head();
		while (pn->LN_Succ())
		{
			auto vector = components->add_vectorindex();
			vector->set_name(pn->LN_Ptr()->Name());

			pn = pn->LN_Succ();
		}

		RefNode<Asset>* an = m_assetList.LH_Head();
		while (an->LN_Succ())
		{
			auto asset = components->add_assetindex();
			asset->set_name(an->LN_Ptr()->Name());

			if (m_scenarioCollection->m_project->m_assetCollection.GetCount()) {
				auto ao = new WISE::FireEngineProto::CwfgmScenario::AssetOptions();

				std::uint32_t mode;
				m_scenario->GetAssetOperation(an->LN_Ptr()->m_filter.get(), &mode);
				if (mode == ((std::uint32_t)-1))
					ao->set_operation(WISE::FireEngineProto::CwfgmScenario::STOP_AFTER_ALL);
				else if (mode == 1)
					ao->set_operation(WISE::FireEngineProto::CwfgmScenario::STOP_IMMEDIATE);
				else if (mode != 0) {
					ao->set_operation(WISE::FireEngineProto::CwfgmScenario::STOP_AFTER_X);
					ao->set_collisioncount(mode);
				}
				else
					// this is the default operation - stop when all asset geometries are reached
					ao->set_operation(WISE::FireEngineProto::CwfgmScenario::NO_EFFECT);

				asset->set_allocated_assetoptions(ao);
			}

			an = an->LN_Succ();
		}

		ICWFGM_Target* target = nullptr;
		unsigned long target_idx, target_sub_idx;
		if (SUCCEEDED(m_scenario->GetWindTarget(&target, &target_idx, &target_sub_idx)))
			if (target) {
				auto t = new ::WISE::FireEngineProto::CwfgmScenario_TargetReference();
				t->set_index(data->targetCollection.IndexOf(target));
				t->set_geometryindex(target_idx);
				t->set_pointindex(target_sub_idx);
				components->set_allocated_windtarget(t);
			}
		target = nullptr;
		if (SUCCEEDED(m_scenario->GetVectorTarget(&target, &target_idx, &target_sub_idx)))
			if (target) {
				auto t = new ::WISE::FireEngineProto::CwfgmScenario_TargetReference();
				t->set_index(data->targetCollection.IndexOf(target));
				t->set_geometryindex(target_idx);
				t->set_pointindex(target_sub_idx);
				components->set_allocated_vectortarget(t);
			}

		if (m_scenarioCollection->m_project->m_assetCollection.GetCount()) {
			std::uint32_t mode;
			m_scenario->GetAssetOperation(nullptr, &mode);
			if (mode == ((std::uint32_t)-1))
				scenario->set_globalassetoperation(WISE::FireEngineProto::CwfgmScenario::STOP_AFTER_ALL);
			else if (mode == 1)
				scenario->set_globalassetoperation(WISE::FireEngineProto::CwfgmScenario::STOP_IMMEDIATE);
			else if (mode != 0) {
				scenario->set_globalassetoperation(WISE::FireEngineProto::CwfgmScenario::STOP_AFTER_X);
				scenario->set_globalcollisioncount(mode);
			}
			else
				scenario->set_globalassetoperation(WISE::FireEngineProto::CwfgmScenario::NO_EFFECT);
		}

		auto idw = new WISE::FireEngineProto::CwfgmScenario_IdwExponent();
		PolymorphicAttribute attr;
		double d;
		m_weatherGrid->GetAttribute(CWFGM_WEATHER_OPTION_ADIABATIC_IDW_EXPONENT_TEMP, &attr);
		if (SUCCEEDED(VariantToDouble_(attr, &d)))
			idw->set_allocated_temperature(DoubleBuilder().withValue(d).forProtobuf(options.useVerboseFloats()));

		m_weatherGrid->GetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_WS, &attr);
		if (SUCCEEDED(VariantToDouble_(attr, &d)))
			idw->set_allocated_windspeed(DoubleBuilder().withValue(d).forProtobuf(options.useVerboseFloats()));

		m_weatherGrid->GetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_PRECIP, &attr);
		if (SUCCEEDED(VariantToDouble_(attr, &d)))
			idw->set_allocated_precipitation(DoubleBuilder().withValue(d).forProtobuf(options.useVerboseFloats()));

		m_weatherGrid->GetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_FWI, &attr);
		if (SUCCEEDED(VariantToDouble_(attr, &d)))
			idw->set_allocated_fwi(DoubleBuilder().withValue(d).forProtobuf(options.useVerboseFloats()));
		scenario->set_allocated_idw(idw);

		return scenario;
	}
}


Project::Scenario *Project::Scenario::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) {
	throw std::logic_error("Incorrect serialization method");
}


Project::Scenario *Project::Scenario::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name, ISerializationData* userData) {
	ScenarioSerializationData *data = dynamic_cast<ScenarioSerializationData*>(userData);

	ICWFGM_CommonData* _data;
	this->m_scenarioCollection->m_project->grid()->GetCommonData(nullptr, &_data);

	if (!data)
	{
		weak_assert(false);
		throw std::invalid_argument("Scenario: Parameter invalid");
	}

	auto scenario = dynamic_cast<const WISE::FireEngineProto::CwfgmScenario*>(&proto);

	if (!scenario)
	{
		ScenarioSerializationData* data = dynamic_cast<ScenarioSerializationData*>(userData);

		if (!data)
		{
			weak_assert(false);
			throw std::invalid_argument("Scenario: Parameter invalid");
		}

		auto scenario = dynamic_cast<const WISE::ProjectProto::ProjectScenario*>(&proto);

		if (!scenario)
		{
			if (valid)
				/// <summary>
				/// The object passed as a scenario is invalid. An incorrect object type was passed to the parser.
				/// </summary>
				/// <type>internal</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectScenario", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
			weak_assert(false);
			throw ISerializeProto::DeserializeError("Scenario: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
		}
		if ((scenario->version() != 1) && (scenario->version() != 2))
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The scenario is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectScenario", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(scenario->version()));
			weak_assert(false);
			throw ISerializeProto::DeserializeError("Scenario: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validations for scenarios.
		/// </summary>
		m_validation = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectScenario", name);
		auto myValid = m_validation.lock();

		m_name = scenario->name();
		if (!scenario->name().length()) {
			if (myValid)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				myValid->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, scenario->name());
		}
		m_comments = scenario->comments();

		m_scenario->PutCommonData(nullptr, _data);
		m_scenario->PutGridEngine(layerThread(), m_temporalFilter.get());

		if ((!scenario->has_copyname()) || (scenario->has_scenario()))
		{
			// if there is a copyname() then this is optional
			ISerializeProto* sp = m_scenario.get();
			if (!sp->deserialize(scenario->scenario(), myValid, "scenario")) {
				weak_assert(false);
				return nullptr;
			}
		}
		m_scenario->put_UserData(this);

		if (scenario->has_temporalconditions())
		{
			if (!m_temporalFilter->deserialize(scenario->temporalconditions(), myValid, "temporalConditions"))
			{
				weak_assert(false);
				return nullptr;
			}
		}

		for (int i = 0; i < scenario->fireindex_size(); i++)
		{
			Fire* fire = nullptr;
			std::string value;
			if (scenario->fireindex(i).reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kIndex) {
				fire = data->fireCollection.FireAtIndex(scenario->fireindex(i).index());
				value = std::to_string(scenario->fireindex(i).index());
			}
			else if (scenario->fireindex(i).reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kName) {
				fire = data->fireCollection.FindName(scenario->fireindex(i).name().c_str());
				value = scenario->fireindex(i).name();
			}

			if (fire)
			{
				if (scenario->fireindex(i).has_remove() && scenario->fireindex(i).remove().value())
				{
					if ((!RemoveFire(fire)) && (myValid))
					{
						/// <summary>
						/// The ignition index/name cannot be removed.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("fireIndex[%d]", i), validation::error_level::SEVERE, validation::id::remove_element_failed, value);
					}
				}
				else
				{
					IgnitionOptions* io = AddFire(fire);

					if ((!io) && (myValid))
					{
						/// <summary>
						/// The ignition index/name cannot be added.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("fireIndex[%d]", i), validation::error_level::SEVERE, validation::id::add_element_failed, value);
					}
				}
			}
			else
			{
				if (myValid)
				{
					/// <summary>
					/// The ignition index/name is unknown.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("fireIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, value);
				}
			}
		}

		for (int i = 0; i < scenario->weatherindex_size(); i++)
		{
			/// <summary>
			/// Child validations for adding a weather station/stream to a scenario.
			/// </summary>
			auto vt2 = validation::conditional_make_object(myValid, "WISE.ProjectProto.ProjectScenario.WeatherIndex", strprintf("weatherindex[%d]", i));
			auto weatherValid = vt2.lock();

			auto weather = scenario->weatherindex(i);
			WeatherStation* station = nullptr;
			std::string station_value;
			if (weather.stationindex().reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kIndex) {
				station = data->weatherCollection.StationAtIndex(weather.stationindex().index());
				station_value = std::to_string(weather.stationindex().index());
			}
			else if (weather.stationindex().reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kName) {
				station = data->weatherCollection.FindName(weather.stationindex().name().c_str());
				station_value = weather.stationindex().name();
			}

			if (station)
			{
				WeatherStream* stream = nullptr;
				std::string stream_value;
				if (weather.streamindex().reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kIndex) {
					stream = station->StreamAtIndex(weather.streamindex().index());
					stream_value = weather.streamindex().index();
				}
				else if (weather.streamindex().reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kName) {
					stream = station->FindName(weather.streamindex().name().c_str());
					stream_value = weather.streamindex().name();
				}

				if (stream)
				{
					if (weather.streamindex().has_remove() && weather.streamindex().remove().value()) {
						if ((!RemoveStream(stream)) && (weatherValid)) {
							/// <summary>
							/// The weather stream index/name cannot be removed.
							/// </summary>
							/// <type>user</type>
							weatherValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("weatherIndex[%d]", i), validation::error_level::SEVERE, validation::id::remove_element_failed, strprintf("stationIndex[%s].streamIndex[%s]", station_value.c_str(), stream_value.c_str()));
						}
					}
					else {
						auto node = AddStream(stream, true);

						if ((!node) && (weatherValid)) {
							/// <summary>
							/// The weather stream index/name cannot be added.
							/// </summary>
							/// <type>user</type>
							weatherValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("weatherIndex[%d]", i), validation::error_level::SEVERE, validation::id::add_element_failed, strprintf("stationIndex[%s].streamIndex[%s]", station_value.c_str(), stream_value.c_str()));
						}

						if (weather.has_isprimary() && weather.isprimary().value()) {
							HRESULT hr1 = SetPrimaryStream(stream);
							if (FAILED(hr1) && (weatherValid)) {
								/// <summary>
								/// The weather stream index/name cannot be set as the primary stream.
								/// </summary>
								/// <type>user</type>
								weatherValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("weatherIndex[%d]", i), validation::error_level::SEVERE, validation::id::primary_stream_failed, strprintf("stationIndex[%s].streamIndex[%s]", station_value.c_str(), stream_value.c_str()));
							}
						}

						if (node && weather.has_weatheroptions())
						{
							auto vt3 = validation::conditional_make_object(weatherValid, "WISE.ProjectProto.ProjectScenario.WeatherIndex.WeatherIndexOptions", "weatherOptions");
							auto weatherValidOptions = vt3.lock();

							node->m_name = weather.weatheroptions().subname();
							node->m_valid = true;
							if (weather.weatheroptions().has_starttime())
							{
								auto time = HSS_Time::Serialization::TimeSerializer::deserializeTime(weather.weatheroptions().starttime(), m_timeManager, weatherValid, "startTime");
								node->m_startTime = WTime(*time);
								delete time;

								if ((node->m_startTime < WTime::GlobalMin(m_timeManager)) || (node->m_startTime > WTime::GlobalMax(m_timeManager)))
								{
									m_loadWarning = "Error: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: Invalid scenario start time";
									if (weatherValidOptions)
										/// <summary>
										/// The scenario start time is out of range or invalid.
										/// </summary>
										/// <type>user</type>
										weatherValidOptions->add_child_validation("HSS.Times.WTime", "startTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_startTime.ToString(WTIME_FORMAT_STRING_ISO8601), { true, WTime::GlobalMin().ToString(WTIME_FORMAT_STRING_ISO8601) }, { true, WTime::GlobalMax().ToString(WTIME_FORMAT_STRING_ISO8601) });
								}
								if (node->m_startTime.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
									m_loadWarning += "Warning: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: fractions of seconds on the start time will be purged to the start of the minute.";
									if (weatherValidOptions)
										/// <summary>
										/// The scenario start time contains fractions of seconds.
										/// </summary>
										/// <type>user</type>
										weatherValidOptions->add_child_validation("HSS.Times.WTime", "startTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_startTime.ToString(WTIME_FORMAT_STRING_ISO8601), "Fractions of seconds will be purged.");
									node->m_startTime.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
								}
							}
							if (weather.weatheroptions().has_endtime())
							{
								auto time = HSS_Time::Serialization::TimeSerializer::deserializeTime(weather.weatheroptions().endtime(), m_timeManager, weatherValid, "endTime");
								node->m_endTime = WTime(*time);
								delete time;

								if ((node->m_endTime < WTime::GlobalMin(m_timeManager)) || (node->m_endTime > WTime::GlobalMax(m_timeManager)))
								{
									m_loadWarning = "Error: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: Invalid scenario end time";
									if (weatherValidOptions)
										/// <summary>
										/// The scenario end time is out of range or invalid.
										/// </summary>
										/// <type>user</type>
										weatherValidOptions->add_child_validation("HSS.Times.WTime", "endTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_endTime.ToString(WTIME_FORMAT_STRING_ISO8601), { true, WTime::GlobalMin().ToString(WTIME_FORMAT_STRING_ISO8601) }, { true, WTime::GlobalMax().ToString(WTIME_FORMAT_STRING_ISO8601) });
								}
								if (node->m_endTime.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
									m_loadWarning += "Warning: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: fractions of seconds on the end time will be purged to the end of the minute.";
									if (weatherValidOptions)
										/// <summary>
										/// The scenario end time contains fractions of seconds.
										/// </summary>
										/// <type>user</type>
										weatherValidOptions->add_child_validation("HSS.Times.WTime", "endTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_endTime.ToString(WTIME_FORMAT_STRING_ISO8601), "Fractions of seconds will be purged.");
									node->m_endTime.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
								}
							}
							if (weather.weatheroptions().has_starttime() && weather.weatheroptions().has_endtime()) {
								if ((node->m_startTime > node->m_endTime) || ((node->m_startTime + WTimeSpan(365, 0, 0, 0)) < node->m_endTime))
								{
									m_loadWarning = "Error: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: Invalid scenario times";
									if (weatherValidOptions) {
										if (node->m_startTime > node->m_endTime) {
											/// <summary>
											/// The scenario start time occurs after the end time.
											/// </summary>
											/// <type>user</type>
											weatherValidOptions->add_child_validation("HSS.Times.WTime", { "startTime", "endTime" }, validation::error_level::WARNING, validation::id::time_invalid, { node->m_startTime.ToString(WTIME_FORMAT_STRING_ISO8601), node->m_endTime.ToString(WTIME_FORMAT_STRING_ISO8601) });
										}
										else /*if (m_startTime + WTimeSpan(365, 0, 0, 0)) < m_endTime)*/ {
											/// <summary>
											/// The duration of the scenario exceeds one year.
											/// </summary>
											/// <type>user</type>
											weatherValidOptions->add_child_validation("HSS.Times.WTime", { "endTime", "startTime" }, validation::error_level::WARNING, validation::id::time_range_invalid, { node->m_endTime.ToString(WTIME_FORMAT_STRING_ISO8601), (node->m_startTime + WTimeSpan(365, 0, 0, 0)).ToString(WTIME_FORMAT_STRING_ISO8601) });
										}
									}
								}
							}
							if (weather.weatheroptions().has_ignitiontime())
							{
								auto time = HSS_Time::Serialization::TimeSerializer::deserializeTime(weather.weatheroptions().ignitiontime(), m_timeManager, weatherValid, "WeatherOptions.IgnitionTime");
								node->m_ignitionTime = WTime(*time);
								delete time;

								if ((node->m_ignitionTime < WTime::GlobalMin(m_timeManager)) || (node->m_ignitionTime > WTime::GlobalMax(m_timeManager)))
								{
									m_loadWarning = "Error: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: Invalid ignition start time";
									if (weatherValidOptions)
										/// <summary>
										/// The scenario start time is out of range or invalid.
										/// </summary>
										/// <type>user</type>
										weatherValidOptions->add_child_validation("HSS.Times.WTime", "startTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_ignitionTime.ToString(WTIME_FORMAT_STRING_ISO8601), { true, WTime::GlobalMin().ToString(WTIME_FORMAT_STRING_ISO8601) }, { true, WTime::GlobalMax().ToString(WTIME_FORMAT_STRING_ISO8601) });
								}
								if (node->m_ignitionTime.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
									m_loadWarning += "Warning: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: fractions of seconds on the ignition start time will be purged to the start of the minute.";
									if (weatherValidOptions)
										/// <summary>
										/// The scenario start time contains fractions of seconds.
										/// </summary>
										/// <type>user</type>
										weatherValidOptions->add_child_validation("HSS.Times.WTime", "startTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_ignitionTime.ToString(WTIME_FORMAT_STRING_ISO8601), "Fractions of seconds will be purged.");
									node->m_ignitionTime.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
								}
							}
							if (weather.weatheroptions().has_winddirection())
							{
								node->m_windDirection = DoubleBuilder().withProtobuf(weather.weatheroptions().winddirection()).getValue();
								if (((node->m_windDirection < -360.0) || (node->m_windDirection > 360.0)) && (weatherValidOptions))
									/// <summary>
									/// The scenario's override wind direction must be <= +-360.0, inclusive.
									/// </summary>
									/// <type>user</type>
									weatherValidOptions->add_child_validation("Math.Double", "windDirection", validation::error_level::WARNING, validation::id::value_invalid, std::to_string(node->m_windDirection.value()), { true, -360.0 }, { true, 360.0 }, "degrees");
							}
							if (weather.weatheroptions().has_deltawinddirection())
							{
								node->m_deltaWindDirection = DoubleBuilder().withProtobuf(weather.weatheroptions().deltawinddirection()).getValue();
								if (((node->m_deltaWindDirection < -360.0) || (node->m_deltaWindDirection > 360.0)) && (weatherValidOptions))
									/// <summary>
									/// The scenario's delta wind direction must be <= +-360.0, inclusive.
									/// </summary>
									/// <type>user</type>
									weatherValidOptions->add_child_validation("Math.Double", "deltaWindDirection", validation::error_level::WARNING, validation::id::value_invalid, std::to_string(node->m_deltaWindDirection.value()), { true, -360.0 }, { true, 360.0 }, "degrees");
							}
						}
					}
				}
				else {
					if (myValid) {
						/// <summary>
						/// The weather station/stream index/name is unknown.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.WeatherIndex", strprintf("weatherIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, strprintf("stationIndex[%s].streamIndex[%s]", station_value.c_str(), stream_value.c_str()));
					}
				}
			}
			else {
				if (myValid) {
					/// <summary>
					/// The weather station index/name is unknown.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.WeatherIndex", strprintf("weatherIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, hss::strprintf("stationIndex[%s]", station_value));
				}
			}
		}

		for (int i = 0; i < scenario->filterindex_size(); i++)
		{
			GridFilter* filter = nullptr;
			std::string value;
			if (scenario->filterindex(i).reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kIndex) {
				filter = data->gridFilterCollection.FilterAtIndex(scenario->filterindex(i).index());
				value = std::to_string(scenario->filterindex(i).index());
			}
			else if (scenario->filterindex(i).reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kName) {
				filter = data->gridFilterCollection.FindName(scenario->filterindex(i).name().c_str());
				value = scenario->filterindex(i).name();
			}

			if (filter)
			{
				if (scenario->filterindex(i).has_remove() && scenario->filterindex(i).remove().value()) {
					if ((!RemoveFilter(filter)) && (myValid)) {
						/// <summary>
						/// The filter index/name cannot be removed.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("filterIndex[%d]", i), validation::error_level::SEVERE, validation::id::remove_element_failed, value);
					}
				}
				else
				{
					auto node = InsertFilter(filter, i, true);

					if ((!node) && (myValid)) {
						/// <summary>
						/// The ignition index/name cannot be added.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("filterIndex[%d]", i), validation::error_level::SEVERE, validation::id::add_element_failed, value);
					}

					if (node)
					{
						if (scenario->filterindex(i).has_filteroptions())
						{
							//only apply the filter to certain sub-scenarios
							for (auto _name : scenario->filterindex(i).filteroptions().subname())
							{
								//remove names that are empty
								if (_name.length())
									node->m_subNames.push_back(_name);
							}
						}
					}
				}
			}
			else {
				if (myValid) {
					/// <summary>
					/// The ignition index/name is unknown.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("filterIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, value);
				}
			}
		}

		for (int i = 0; i < scenario->vectorindex_size(); i++)
		{
			Vector* vector = nullptr;
			std::string value;
			if (scenario->vectorindex(i).reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kIndex) {
				vector = data->vectorCollection.VectorAtIndex(scenario->vectorindex(i).index());
				value = std::to_string(scenario->vectorindex(i).index());
			}
			else if (scenario->vectorindex(i).reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kName) {
				vector = data->vectorCollection.FindName(scenario->vectorindex(i).name().c_str());
				value = scenario->vectorindex(i).name();
			}

			if (vector)
			{
				if (scenario->vectorindex(i).has_remove() && scenario->vectorindex(i).remove().value()) {
					if ((!RemoveVector(vector)) && (myValid)) {
						/// <summary>
						/// The vector index/name cannot be removed.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("vectorIndex[%d]", i), validation::error_level::SEVERE, validation::id::remove_element_failed, value);
					}
				}
				else {
					if ((!AddVector(vector, true)) && (myValid)) {
						/// <summary>
						/// The vector index/name cannot be added.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("vectorIndex[%d]", i), validation::error_level::SEVERE, validation::id::add_element_failed, value);
					}
				}
			}
			else {
				if (myValid) {
					/// <summary>
					/// The vector index/name is unknown.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("vectorIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, value);
				}
			}
		}

		for (int i = 0; i < scenario->assetindex_size(); i++)
		{
			auto& reference = scenario->assetindex(i);
			Asset* asset = nullptr;
			std::string value;
			if (reference.reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kIndex) {
				asset = data->assetCollection.AssetAtIndex(scenario->assetindex(i).index());
				value = std::to_string(scenario->assetindex(i).index());
			}
			else if (reference.reference_case() == WISE::ProjectProto::ProjectScenario_Reference::kName) {
				asset = data->assetCollection.FindName(scenario->assetindex(i).name());
				value = scenario->assetindex(i).name();
			}

			if (asset)
			{
				if (reference.has_remove() && reference.remove().value()) {
					if ((!RemoveAsset(asset)) && (myValid)) {
						/// <summary>
						/// The asset index/name cannot be removed.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("assetIndex[%d]", i), validation::error_level::SEVERE, validation::id::remove_element_failed, value);
					}
				}
				else
				{
					std::uint32_t operation = 0;
					if (reference.has_assetoptions())
					{
						if (reference.assetoptions().operationNull_case() == WISE::ProjectProto::ProjectScenario_AssetOptions::kOperation)
						{
							if (reference.assetoptions().operation() == WISE::ProjectProto::ProjectScenario::NO_EFFECT)
								operation = 0;
							else if (reference.assetoptions().operation() == WISE::ProjectProto::ProjectScenario::STOP_IMMEDIATE)
								operation = 1;
							else if (reference.assetoptions().operation() == WISE::ProjectProto::ProjectScenario::STOP_AFTER_X)
							{
								if (reference.assetoptions().has_collisioncount())
									operation = reference.assetoptions().collisioncount().value();
								else
									operation = (ULONG)-1;
							}
						}
						else
							operation = (ULONG)-1;
					}
					if ((!AddAsset(asset, operation, true)) && (myValid)) {
						/// <summary>
						/// The asset index/name cannot be added.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("assetIndex[%d]", i), validation::error_level::SEVERE, validation::id::add_element_failed, value);
					}
				}
			}
			else {
				if (myValid) {
					/// <summary>
					/// The asset index/name is unknown.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("assetIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, value);
				}
			}
		}

		if (scenario->has_windtarget()) {
			auto wt = scenario->windtarget();
			if (wt.has_remove() && wt.remove().value()) {
				m_scenario->SetWindTarget(NULL, (ULONG)-1, (ULONG)-1);
			}
			else {
				Target* target = nullptr;
				std::string value;
				if (wt.reference_case() == WISE::ProjectProto::ProjectScenario_TargetReference::ReferenceCase::kIndex) {
					target = data->targetCollection.TargetAtIndex(wt.index());
					value = std::to_string(wt.index());
				}
				else if (wt.reference_case() == WISE::ProjectProto::ProjectScenario_TargetReference::ReferenceCase::kName) {
					target = data->targetCollection.FindName(wt.name().c_str());
					value = wt.name();
				}

				if (target) {
					HRESULT hr = m_scenario->SetWindTarget(target->m_target.get(), wt.geometryindex(), wt.pointindex());
					if (FAILED(hr) && (myValid)) {
						/// <summary>
						/// The target index/name cannot be added.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", "windTarget", validation::error_level::SEVERE, validation::id::add_element_failed, value);
					}
				}
				else if (myValid) {
					/// <summary>
					/// The target index/name cannot be added.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", "windTarget", validation::error_level::SEVERE, validation::id::add_element_failed, value);
				}
			}
		}

		if (scenario->has_vectortarget()) {
			auto wt = scenario->vectortarget();
			if (wt.has_remove() && wt.remove().value()) {
				m_scenario->SetVectorTarget(NULL, (ULONG)-1, (ULONG)-1);
			}
			else {
				Target* target = nullptr;
				std::string value;
				if (wt.reference_case() == WISE::ProjectProto::ProjectScenario_TargetReference::ReferenceCase::kIndex) {
					target = data->targetCollection.TargetAtIndex(wt.index());
					value = std::to_string(wt.index());
				}
				else if (wt.reference_case() == WISE::ProjectProto::ProjectScenario_TargetReference::ReferenceCase::kName) {
					target = data->targetCollection.FindName(wt.name().c_str());
					value = wt.name();
				}

				if (target) {
					HRESULT hr = m_scenario->SetVectorTarget(target->m_target.get(), wt.geometryindex(), wt.pointindex());
					if (FAILED(hr) && (myValid)) {
						/// <summary>
						/// The target index/name cannot be added.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", "vectorTarget", validation::error_level::SEVERE, validation::id::add_element_failed, value);
					}
				}
				else if (myValid) {
					/// <summary>
					/// The target index/name cannot be added.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", "vectorTarget", validation::error_level::SEVERE, validation::id::add_element_failed, value);
				}
			}
		}

		if (scenario->globalAssetOperationNull_case() == WISE::ProjectProto::ProjectScenario::kGlobalAssetOperation)
		{
			if (scenario->globalassetoperation() == WISE::ProjectProto::ProjectScenario::NO_EFFECT)
				m_scenario->SetAssetOperation(nullptr, 0);
			else if (scenario->globalassetoperation() == WISE::ProjectProto::ProjectScenario::STOP_IMMEDIATE)
				m_scenario->SetAssetOperation(nullptr, 1);
			else if (scenario->globalassetoperation() == WISE::ProjectProto::ProjectScenario::STOP_AFTER_X)
			{
				if (scenario->has_globalcollisioncount())
					m_scenario->SetAssetOperation(nullptr, scenario->globalcollisioncount().value());
				else // assumed if not set, to be stop after all
					m_scenario->SetAssetOperation(nullptr, (ULONG)-1);		// if it's not set, then use -1 to say all asset geometries, regardless of count
			}
			else // stop after all
				m_scenario->SetAssetOperation(nullptr, (ULONG)-1);
		}
		else if (scenario->version() == 1)
			m_scenario->SetAssetOperation(nullptr, (ULONG)-1);
		else
			m_scenario->SetAssetOperation(nullptr, 0);

		if (scenario->has_idw())
		{
			m_weatherGrid->SetAttribute(CWFGM_WEATHER_OPTION_ADIABATIC_IDW_EXPONENT_TEMP, DoubleBuilder().withProtobuf(scenario->idw().temperature()).getValue());
			m_weatherGrid->SetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_WS, DoubleBuilder().withProtobuf(scenario->idw().windspeed()).getValue());
			m_weatherGrid->SetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_PRECIP, DoubleBuilder().withProtobuf(scenario->idw().precipitation()).getValue());
			m_weatherGrid->SetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_FWI, DoubleBuilder().withProtobuf(scenario->idw().fwi()).getValue());
		}

		int max_thread_count;
		max_thread_count = CWorkerThreadPool::NumberIdealProcessors();

		PolymorphicAttribute v = max_thread_count;
		m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_MULTITHREADING, v);
		v = true;
		m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_FORCE_AFFINITY, v);

		{
			const CBurnPeriodOption* defaultoption = m_scenarioCollection->m_burnOptions;
			if (defaultoption)
				if (CorrectBurnConditionRange(*defaultoption, myValid, "burnConditions")) {
					m_loadWarning += "Burning conditions have been modified to apply to the scenario date range.\n";
				}
		}

	} else {

		if (scenario->version() != 6)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The scenario is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectScenario", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(scenario->version()));
			weak_assert(false);
			throw ISerializeProto::DeserializeError("Scenario: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validations for scenarios.
		/// </summary>
		m_validation = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectScenario", name);
		auto myValid = m_validation.lock();

		if (scenario->has_name()) {
			m_name = scenario->name();
			if (!scenario->name().length()) {
				if (myValid)
					/// <summary>
					/// The scenario's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, scenario->name());
				throw ISerializeProto::DeserializeError("WISE.FireEngineProto.CwfgmScenario: Invalid or missing name.");
			}
		}
		else {
			if (myValid)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				myValid->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.FireEngineProto.CwfgmScenario: Invalid or missing name.");
		}
		if (scenario->has_comments())
			m_comments = scenario->comments();

		m_scenario->PutCommonData(nullptr, _data);
		m_scenario->PutGridEngine(layerThread(), m_temporalFilter.get());

		if (!scenario->has_copyname())
		{
			// if there is a copyname() then this is optional
			ISerializeProto* sp = m_scenario.get();
			if (!sp->deserialize(*scenario, myValid, "scenario")) {
				weak_assert(false);
				return nullptr;
			}
		}
		m_scenario->put_UserData(this);

		if (scenario->has_temporalconditions())
		{
			if (!m_temporalFilter->deserialize(scenario->temporalconditions(), myValid, "temporalConditions"))
			{
				weak_assert(false);
				return nullptr;
			}
		}

		if (scenario->has_components()) {
			for (int i = 0; i < scenario->components().fireindex_size(); i++)
			{
				Fire* fire = nullptr;
				std::string value;
				if (scenario->components().fireindex(i).reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kIndex) {
					fire = data->fireCollection.FireAtIndex(scenario->components().fireindex(i).index());
					value = std::to_string(scenario->components().fireindex(i).index());
				}
				else if (scenario->components().fireindex(i).reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kName) {
					fire = data->fireCollection.FindName(scenario->components().fireindex(i).name().c_str());
					value = scenario->components().fireindex(i).name();
				}

				if (fire)
				{
					if (scenario->components().fireindex(i).has_remove() && scenario->components().fireindex(i).remove())
					{
						if ((!RemoveFire(fire)) && (myValid))
						{
							/// <summary>
							/// The ignition index/name cannot be removed.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("fireIndex[%d]", i), validation::error_level::SEVERE, validation::id::remove_element_failed, value);
						}
					}
					else
					{
						IgnitionOptions* io = AddFire(fire);

						if ((!io) && (myValid))
						{
							/// <summary>
							/// The ignition index/name cannot be added.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("fireIndex[%d]", i), validation::error_level::SEVERE, validation::id::add_element_failed, value);
						}
					}
				}
				else
				{
					if (myValid)
					{
						/// <summary>
						/// The ignition index/name is unknown.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("fireIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, value);
					}
				}
			}

			for (int i = 0; i < scenario->components().weatherindex_size(); i++)
			{
				/// <summary>
				/// Child validations for adding a weather station/stream to a scenario.
				/// </summary>
				auto vt2 = validation::conditional_make_object(myValid, "WISE.ProjectProto.ProjectScenario.WeatherIndex", strprintf("weatherindex[%d]", i));
				auto weatherValid = vt2.lock();

				auto weather = scenario->components().weatherindex(i);
				WeatherStation* station = nullptr;
				std::string station_value;
				if (weather.stationindex().reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kIndex) {
					station = data->weatherCollection.StationAtIndex(weather.stationindex().index());
					station_value = std::to_string(weather.stationindex().index());
				}
				else if (weather.stationindex().reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kName) {
					station = data->weatherCollection.FindName(weather.stationindex().name().c_str());
					station_value = weather.stationindex().name();
				}

				if (station)
				{
					WeatherStream* stream = nullptr;
					std::string stream_value;
					if (weather.streamindex().reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kIndex) {
						stream = station->StreamAtIndex(weather.streamindex().index());
						stream_value = weather.streamindex().index();
					}
					else if (weather.streamindex().reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kName) {
						stream = station->FindName(weather.streamindex().name().c_str());
						stream_value = weather.streamindex().name();
					}

					if (stream)
					{
						if (weather.streamindex().has_remove() && weather.streamindex().remove()) {
							if ((!RemoveStream(stream)) && (weatherValid)) {
								/// <summary>
								/// The weather stream index/name cannot be removed.
								/// </summary>
								/// <type>user</type>
								weatherValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("weatherIndex[%d]", i), validation::error_level::SEVERE, validation::id::remove_element_failed, strprintf("stationIndex[%s].streamIndex[%s]", station_value.c_str(), stream_value.c_str()));
							}
						}
						else {
							auto node = AddStream(stream, true);

							if ((!node) && (weatherValid)) {
								/// <summary>
								/// The weather stream index/name cannot be added.
								/// </summary>
								/// <type>user</type>
								weatherValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("weatherIndex[%d]", i), validation::error_level::SEVERE, validation::id::add_element_failed, strprintf("stationIndex[%s].streamIndex[%s]", station_value.c_str(), stream_value.c_str()));
							}

							if (weather.has_isprimary() && weather.isprimary()) {
								HRESULT hr1 = SetPrimaryStream(stream);
								if (FAILED(hr1) && (weatherValid)) {
									/// <summary>
									/// The weather stream index/name cannot be set as the primary stream.
									/// </summary>
									/// <type>user</type>
									weatherValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("weatherIndex[%d]", i), validation::error_level::SEVERE, validation::id::primary_stream_failed, strprintf("stationIndex[%s].streamIndex[%s]", station_value.c_str(), stream_value.c_str()));
								}
							}

							if (node && weather.has_weatheroptions())
							{
								auto vt3 = validation::conditional_make_object(weatherValid, "WISE.ProjectProto.ProjectScenario.WeatherIndex.WeatherIndexOptions", "weatherOptions");
								auto weatherValidOptions = vt3.lock();

								node->m_name = weather.weatheroptions().subname();
								node->m_valid = true;
								if (weather.weatheroptions().has_starttime())
								{
									auto time = HSS_Time::Serialization::TimeSerializer::deserializeTime(weather.weatheroptions().starttime(), m_timeManager, weatherValid, "startTime");
									node->m_startTime = WTime(*time);
									delete time;

									if ((node->m_startTime < WTime::GlobalMin(m_timeManager)) || (node->m_startTime > WTime::GlobalMax(m_timeManager)))
									{
										m_loadWarning = "Error: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: Invalid scenario start time";
										if (weatherValidOptions)
											/// <summary>
											/// The scenario start time is out of range or invalid.
											/// </summary>
											/// <type>user</type>
											weatherValidOptions->add_child_validation("HSS.Times.WTime", "startTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_startTime.ToString(WTIME_FORMAT_STRING_ISO8601), { true, WTime::GlobalMin().ToString(WTIME_FORMAT_STRING_ISO8601) }, { true, WTime::GlobalMax().ToString(WTIME_FORMAT_STRING_ISO8601) });
									}
									if (node->m_startTime.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
										m_loadWarning += "Warning: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: fractions of seconds on the start time will be purged to the start of the minute.";
										if (weatherValidOptions)
											/// <summary>
											/// The scenario start time contains fractions of seconds.
											/// </summary>
											/// <type>user</type>
											weatherValidOptions->add_child_validation("HSS.Times.WTime", "startTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_startTime.ToString(WTIME_FORMAT_STRING_ISO8601), "Fractions of seconds will be purged.");
										node->m_startTime.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
									}
								}
								if (weather.weatheroptions().has_endtime())
								{
									auto time = HSS_Time::Serialization::TimeSerializer::deserializeTime(weather.weatheroptions().endtime(), m_timeManager, weatherValid, "endTime");
									node->m_endTime = WTime(*time);
									delete time;

									if ((node->m_endTime < WTime::GlobalMin(m_timeManager)) || (node->m_endTime > WTime::GlobalMax(m_timeManager)))
									{
										m_loadWarning = "Error: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: Invalid scenario end time";
										if (weatherValidOptions)
											/// <summary>
											/// The scenario end time is out of range or invalid.
											/// </summary>
											/// <type>user</type>
											weatherValidOptions->add_child_validation("HSS.Times.WTime", "endTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_endTime.ToString(WTIME_FORMAT_STRING_ISO8601), { true, WTime::GlobalMin().ToString(WTIME_FORMAT_STRING_ISO8601) }, { true, WTime::GlobalMax().ToString(WTIME_FORMAT_STRING_ISO8601) });
									}
									if (node->m_endTime.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
										m_loadWarning += "Warning: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: fractions of seconds on the end time will be purged to the end of the minute.";
										if (weatherValidOptions)
											/// <summary>
											/// The scenario end time contains fractions of seconds.
											/// </summary>
											/// <type>user</type>
											weatherValidOptions->add_child_validation("HSS.Times.WTime", "endTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_endTime.ToString(WTIME_FORMAT_STRING_ISO8601), "Fractions of seconds will be purged.");
										node->m_endTime.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
									}
								}
								if (weather.weatheroptions().has_starttime() && weather.weatheroptions().has_endtime()) {
									if ((node->m_startTime > node->m_endTime) || ((node->m_startTime + WTimeSpan(365, 0, 0, 0)) < node->m_endTime))
									{
										m_loadWarning = "Error: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: Invalid scenario times";
										if (weatherValidOptions) {
											if (node->m_startTime > node->m_endTime) {
												/// <summary>
												/// The scenario start time occurs after the end time.
												/// </summary>
												/// <type>user</type>
												weatherValidOptions->add_child_validation("HSS.Times.WTime", { "startTime", "endTime" }, validation::error_level::WARNING, validation::id::time_invalid, { node->m_startTime.ToString(WTIME_FORMAT_STRING_ISO8601), node->m_endTime.ToString(WTIME_FORMAT_STRING_ISO8601) });
											}
											else /*if (m_startTime + WTimeSpan(365, 0, 0, 0)) < m_endTime)*/ {
												/// <summary>
												/// The duration of the scenario exceeds one year.
												/// </summary>
												/// <type>user</type>
												weatherValidOptions->add_child_validation("HSS.Times.WTime", { "endTime", "startTime" }, validation::error_level::WARNING, validation::id::time_range_invalid, { node->m_endTime.ToString(WTIME_FORMAT_STRING_ISO8601), (node->m_startTime + WTimeSpan(365, 0, 0, 0)).ToString(WTIME_FORMAT_STRING_ISO8601) });
											}
										}
									}
								}
								if (weather.weatheroptions().has_ignitiontime())
								{
									auto time = HSS_Time::Serialization::TimeSerializer::deserializeTime(weather.weatheroptions().ignitiontime(), m_timeManager, weatherValid, "WeatherOptions.IgnitionTime");
									node->m_ignitionTime = WTime(*time);
									delete time;

									if ((node->m_ignitionTime < WTime::GlobalMin(m_timeManager)) || (node->m_ignitionTime > WTime::GlobalMax(m_timeManager)))
									{
										m_loadWarning = "Error: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: Invalid ignition start time";
										if (weatherValidOptions)
											/// <summary>
											/// The scenario start time is out of range or invalid.
											/// </summary>
											/// <type>user</type>
											weatherValidOptions->add_child_validation("HSS.Times.WTime", "startTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_ignitionTime.ToString(WTIME_FORMAT_STRING_ISO8601), { true, WTime::GlobalMin().ToString(WTIME_FORMAT_STRING_ISO8601) }, { true, WTime::GlobalMax().ToString(WTIME_FORMAT_STRING_ISO8601) });
									}
									if (node->m_ignitionTime.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
										m_loadWarning += "Warning: WISE.ProjectProto.WeatherIndex.WeatherIndexOptions: fractions of seconds on the ignition start time will be purged to the start of the minute.";
										if (weatherValidOptions)
											/// <summary>
											/// The scenario start time contains fractions of seconds.
											/// </summary>
											/// <type>user</type>
											weatherValidOptions->add_child_validation("HSS.Times.WTime", "startTime", validation::error_level::WARNING, validation::id::time_invalid, node->m_ignitionTime.ToString(WTIME_FORMAT_STRING_ISO8601), "Fractions of seconds will be purged.");
										node->m_ignitionTime.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
									}
								}
								if (weather.weatheroptions().has_winddirection())
								{
									node->m_windDirection = DoubleBuilder().withProtobuf(weather.weatheroptions().winddirection()).getValue();
									if (((node->m_windDirection < -360.0) || (node->m_windDirection > 360.0)) && (weatherValidOptions))
										/// <summary>
										/// The scenario's override wind direction must be <= +-360.0, inclusive.
										/// </summary>
										/// <type>user</type>
										weatherValidOptions->add_child_validation("Math.Double", "windDirection", validation::error_level::WARNING, validation::id::value_invalid, std::to_string(node->m_windDirection.value()), { true, -360.0 }, { true, 360.0 }, "degrees");
								}
								if (weather.weatheroptions().has_deltawinddirection())
								{
									node->m_deltaWindDirection = DoubleBuilder().withProtobuf(weather.weatheroptions().deltawinddirection()).getValue();
									if (((node->m_deltaWindDirection < -360.0) || (node->m_deltaWindDirection > 360.0)) && (weatherValidOptions))
										/// <summary>
										/// The scenario's delta wind direction must be <= +-360.0, inclusive.
										/// </summary>
										/// <type>user</type>
										weatherValidOptions->add_child_validation("Math.Double", "deltaWindDirection", validation::error_level::WARNING, validation::id::value_invalid, std::to_string(node->m_deltaWindDirection.value()), { true, -360.0 }, { true, 360.0 }, "degrees");
								}
							}
						}
					}
					else {
						if (myValid) {
							/// <summary>
							/// The weather station/stream index/name is unknown.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.WeatherIndex", strprintf("weatherIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, strprintf("stationIndex[%s].streamIndex[%s]", station_value.c_str(), stream_value.c_str()));
						}
					}
				}
				else {
					if (myValid) {
						/// <summary>
						/// The weather station index/name is unknown.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.WeatherIndex", strprintf("weatherIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, hss::strprintf("stationIndex[%s]", station_value));
					}
				}
			}

			for (int i = 0; i < scenario->components().filterindex_size(); i++)
			{
				GridFilter* filter = nullptr;
				std::string value;
				if (scenario->components().filterindex(i).reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kIndex) {
					filter = data->gridFilterCollection.FilterAtIndex(scenario->components().filterindex(i).index());
					value = std::to_string(scenario->components().filterindex(i).index());
				}
				else if (scenario->components().filterindex(i).reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kName) {
					filter = data->gridFilterCollection.FindName(scenario->components().filterindex(i).name().c_str());
					value = scenario->components().filterindex(i).name();
				}

				if (filter)
				{
					if (scenario->components().filterindex(i).has_remove() && scenario->components().filterindex(i).remove()) {
						if ((!RemoveFilter(filter)) && (myValid)) {
							/// <summary>
							/// The filter index/name cannot be removed.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("filterIndex[%d]", i), validation::error_level::SEVERE, validation::id::remove_element_failed, value);
						}
					}
					else
					{
						auto node = InsertFilter(filter, i, true);

						if ((!node) && (myValid)) {
							/// <summary>
							/// The ignition index/name cannot be added.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("filterIndex[%d]", i), validation::error_level::SEVERE, validation::id::add_element_failed, value);
						}

						if (node)
						{
							if (scenario->components().filterindex(i).has_filteroptions())
							{
								//only apply the filter to certain sub-scenarios
								for (auto _name : scenario->components().filterindex(i).filteroptions().subname())
								{
									//remove names that are empty
									if (_name.length())
										node->m_subNames.push_back(_name);
								}
							}
						}
					}
				}
				else {
					if (myValid) {
						/// <summary>
						/// The ignition index/name is unknown.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("filterIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, value);
					}
				}
			}

			for (int i = 0; i < scenario->components().vectorindex_size(); i++)
			{
				Vector* vector = nullptr;
				std::string value;
				if (scenario->components().vectorindex(i).reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kIndex) {
					vector = data->vectorCollection.VectorAtIndex(scenario->components().vectorindex(i).index());
					value = std::to_string(scenario->components().vectorindex(i).index());
				}
				else if (scenario->components().vectorindex(i).reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kName) {
					vector = data->vectorCollection.FindName(scenario->components().vectorindex(i).name().c_str());
					value = scenario->components().vectorindex(i).name();
				}

				if (vector)
				{
					if (scenario->components().vectorindex(i).has_remove() && scenario->components().vectorindex(i).remove()) {
						if ((!RemoveVector(vector)) && (myValid)) {
							/// <summary>
							/// The vector index/name cannot be removed.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("vectorIndex[%d]", i), validation::error_level::SEVERE, validation::id::remove_element_failed, value);
						}
					}
					else {
						if ((!AddVector(vector, true)) && (myValid)) {
							/// <summary>
							/// The vector index/name cannot be added.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("vectorIndex[%d]", i), validation::error_level::SEVERE, validation::id::add_element_failed, value);
						}
					}
				}
				else {
					if (myValid) {
						/// <summary>
						/// The vector index/name is unknown.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("vectorIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, value);
					}
				}
			}
		}
		for (int i = 0; i < scenario->components().assetindex_size(); i++)
		{
			auto& reference = scenario->components().assetindex(i);
			Asset* asset = nullptr;
			std::string value;
			if (reference.reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kIndex) {
				asset = data->assetCollection.AssetAtIndex(scenario->components().assetindex(i).index());
				value = std::to_string(scenario->components().assetindex(i).index());
			}
			else if (reference.reference_case() == WISE::FireEngineProto::CwfgmScenario_Reference::kName) {
				asset = data->assetCollection.FindName(scenario->components().assetindex(i).name());
				value = scenario->components().assetindex(i).name();
			}

			if (asset)
			{
				if (reference.has_remove() && reference.remove()) {
					if ((!RemoveAsset(asset)) && (myValid)) {
						/// <summary>
						/// The asset index/name cannot be removed.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("assetIndex[%d]", i), validation::error_level::SEVERE, validation::id::remove_element_failed, value);
					}
				}
				else
				{
					std::uint32_t operation = 0;
					if (reference.has_assetoptions())
					{
						if (reference.assetoptions().operationNull_case() == WISE::FireEngineProto::CwfgmScenario_AssetOptions::kOperation)
						{
							if (reference.assetoptions().operation() == WISE::FireEngineProto::CwfgmScenario::NO_EFFECT)
								operation = 0;
							else if (reference.assetoptions().operation() == WISE::FireEngineProto::CwfgmScenario::STOP_IMMEDIATE)
								operation = 1;
							else if (reference.assetoptions().operation() == WISE::FireEngineProto::CwfgmScenario::STOP_AFTER_X)
							{
								if (reference.assetoptions().has_collisioncount())
									operation = reference.assetoptions().collisioncount();
								else
									operation = (ULONG)-1;
							}
						}
						else
							operation = (ULONG)-1;
					}
					if ((!AddAsset(asset, operation, true)) && (myValid)) {
						/// <summary>
						/// The asset index/name cannot be added.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("assetIndex[%d]", i), validation::error_level::SEVERE, validation::id::add_element_failed, value);
					}
				}
			}
			else {
				if (myValid) {
					/// <summary>
					/// The asset index/name is unknown.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", strprintf("assetIndex[%d]", i), validation::error_level::SEVERE, validation::id::index_invalid, value);
				}
			}
		}

		if (scenario->components().has_windtarget()) {
			auto wt = scenario->components().windtarget();
			if (wt.has_remove() && wt.remove()) {
				m_scenario->SetWindTarget(NULL, (ULONG)-1, (ULONG)-1);
			}
			else {
				Target* target = nullptr;
				std::string value;
				if (wt.reference_case() == WISE::FireEngineProto::CwfgmScenario_TargetReference::ReferenceCase::kIndex) {
					target = data->targetCollection.TargetAtIndex(wt.index());
					value = std::to_string(wt.index());
				}
				else if (wt.reference_case() == WISE::FireEngineProto::CwfgmScenario_TargetReference::ReferenceCase::kName) {
					target = data->targetCollection.FindName(wt.name().c_str());
					value = wt.name();
				}

				if (target) {
					HRESULT hr = m_scenario->SetWindTarget(target->m_target.get(), wt.geometryindex(), wt.pointindex());
					if (FAILED(hr) && (myValid)) {
						/// <summary>
						/// The target index/name cannot be added.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", "windTarget", validation::error_level::SEVERE, validation::id::add_element_failed, value);
					}
				}
				else if (myValid) {
					/// <summary>
					/// The target index/name cannot be added.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", "windTarget", validation::error_level::SEVERE, validation::id::add_element_failed, value);
				}
			}
		}

		if (scenario->components().has_vectortarget()) {
			auto wt = scenario->components().vectortarget();
			if (wt.has_remove() && wt.remove()) {
				m_scenario->SetVectorTarget(NULL, (ULONG)-1, (ULONG)-1);
			}
			else {
				Target* target = nullptr;
				std::string value;
				if (wt.reference_case() == WISE::FireEngineProto::CwfgmScenario_TargetReference::ReferenceCase::kIndex) {
					target = data->targetCollection.TargetAtIndex(wt.index());
					value = std::to_string(wt.index());
				}
				else if (wt.reference_case() == WISE::FireEngineProto::CwfgmScenario_TargetReference::ReferenceCase::kName) {
					target = data->targetCollection.FindName(wt.name().c_str());
					value = wt.name();
				}

				if (target) {
					HRESULT hr = m_scenario->SetVectorTarget(target->m_target.get(), wt.geometryindex(), wt.pointindex());
					if (FAILED(hr) && (myValid)) {
						/// <summary>
						/// The target index/name cannot be added.
						/// </summary>
						/// <type>user</type>
						myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", "vectorTarget", validation::error_level::SEVERE, validation::id::add_element_failed, value);
					}
				}
				else if (myValid) {
					/// <summary>
					/// The target index/name cannot be added.
					/// </summary>
					/// <type>user</type>
					myValid->add_child_validation("WISE.ProjectProto.ProjectScenario.Reference", "vectorTarget", validation::error_level::SEVERE, validation::id::add_element_failed, value);
				}
			}
		}

		if (scenario->globalAssetOperationNull_case() == WISE::FireEngineProto::CwfgmScenario::kGlobalAssetOperation)
		{
			if (scenario->globalassetoperation() == WISE::FireEngineProto::CwfgmScenario::NO_EFFECT)
				m_scenario->SetAssetOperation(nullptr, 0);
			else if (scenario->globalassetoperation() == WISE::FireEngineProto::CwfgmScenario::STOP_IMMEDIATE)
				m_scenario->SetAssetOperation(nullptr, 1);
			else if (scenario->globalassetoperation() == WISE::FireEngineProto::CwfgmScenario::STOP_AFTER_X)
			{
				if (scenario->has_globalcollisioncount())
					m_scenario->SetAssetOperation(nullptr, scenario->globalcollisioncount());
				else // assumed if not set, to be stop after all
					m_scenario->SetAssetOperation(nullptr, (ULONG)-1);		// if it's not set, then use -1 to say all asset geometries, regardless of count
			}
			else // stop after all
				m_scenario->SetAssetOperation(nullptr, (ULONG)-1);
		}
		else if (scenario->version() == 1)
			m_scenario->SetAssetOperation(nullptr, (ULONG)-1);
		else
			m_scenario->SetAssetOperation(nullptr, 0);

		if (scenario->has_idw())
		{
			m_weatherGrid->SetAttribute(CWFGM_WEATHER_OPTION_ADIABATIC_IDW_EXPONENT_TEMP, DoubleBuilder().withProtobuf(scenario->idw().temperature()).getValue());
			m_weatherGrid->SetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_WS, DoubleBuilder().withProtobuf(scenario->idw().windspeed()).getValue());
			m_weatherGrid->SetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_PRECIP, DoubleBuilder().withProtobuf(scenario->idw().precipitation()).getValue());
			m_weatherGrid->SetAttribute(CWFGM_WEATHER_OPTION_IDW_EXPONENT_FWI, DoubleBuilder().withProtobuf(scenario->idw().fwi()).getValue());
		}

		int max_thread_count;
		max_thread_count = CWorkerThreadPool::NumberIdealProcessors();

		PolymorphicAttribute v = max_thread_count;
		m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_MULTITHREADING, v);

		v = true;
		m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_FORCE_AFFINITY, v);

		{
			const CBurnPeriodOption* defaultoption = m_scenarioCollection->m_burnOptions;
			if (defaultoption)
				if (CorrectBurnConditionRange(*defaultoption, myValid, "burnConditions")) {
					m_loadWarning += "Burning conditions have been modified to apply to the scenario date range.\n";
				}
		}
	}

	return this;
}


std::int32_t Project::ScenarioCollection::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


WISE::ProjectProto::ScenarioCollection* Project::ScenarioCollection::serialize(const SerializeProtoOptions& options) {
	auto collection = new WISE::ProjectProto::ScenarioCollection();
	collection->set_version(serialVersionUid(options));

	ScenarioSerializationData data(m_project->m_gridFilterCollection,
		m_project->m_vectorCollection, m_project->m_assetCollection, m_project->m_targetCollection, m_project->m_weatherCollection, m_project->m_fireCollection);

	Project::Scenario *scenario = m_scenarioList.LH_Head();
	while (scenario->LN_Succ())
	{
		if (options.fileVersion() == 1) {
		auto scen = collection->add_scenarios();
		auto ser = scenario->serialize(options, &data);
			scen->Swap(dynamic_cast_assert<WISE::ProjectProto::ProjectScenario *>(ser));
		delete ser;
		}
		else {
			auto scen = collection->add_scenariodata();
			auto ser = scenario->serialize(options, &data);
			scen->Swap(dynamic_cast_assert<WISE::FireEngineProto::CwfgmScenario *>(ser));
			delete ser;
		}
		scenario = scenario->LN_Succ();
	}

	return collection;
}

auto Project::ScenarioCollection::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> ScenarioCollection*
{
	auto collection = dynamic_cast<const WISE::ProjectProto::ScenarioCollection*>(&proto);

	if (!collection)
	{
		if (valid)
			/// <summary>
			/// The object passed as a scenario collection is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.ScenarioCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, proto.GetDescriptor()->name());
		weak_assert(false);
		throw ISerializeProto::DeserializeError("ScenarioCollection: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if ((collection->version() != 1) && (collection->version() != 2))
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The scenario collection is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.ScenarioCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(collection->version()));
		weak_assert(false);
		throw ISerializeProto::DeserializeError("ScenarioCollection: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	auto s = FirstScenario();
	RemoveScenario(s);
	ScenarioCollection* retval = this;

	ScenarioSerializationData data(m_project->m_gridFilterCollection,
		m_project->m_vectorCollection, m_project->m_assetCollection, m_project->m_targetCollection, m_project->m_weatherCollection, m_project->m_fireCollection);

	/// <summary>
	/// Child validations for scenario collections.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ScenarioCollection", name);
	auto v = vt.lock();

	ICWFGM_CommonData* _data;
	m_project->grid()->GetCommonData(nullptr, &_data);

	if (collection->version() == 1) {
		for (int i = 0; i < collection->scenarios_size(); i++)
		{
			auto scen = collection->scenarios(i);

			//scenario copy
			if (scen.has_copyname())
			{
				Project::Scenario* toCopy = FindName(scen.copyname().value().c_str());
				if (!toCopy)
				{
					if (v)
						/// <summary>
						/// The requested scenario to copy doesn't exist in the FGM.
						/// </summary>
						/// <type>user</type>
						v->add_child_validation("WISE.ProjectProto.ScenarioCollection", "scenarioCopy", validation::error_level::SEVERE, "Scenario.Copy:Missing", scen.copyname().value());
					throw std::logic_error("Unknown scenario");
				}

				Scenario* scenario = nullptr;
				try {
					scenario = new Project::Scenario(*toCopy, m_project->m_timeManager, m_project->gridEngine(), false);
				}
				catch (std::exception& e) {
					/// <summary>
					/// The COM object could not be instantiated.
					/// </summary>
					/// <type>internal</type>
					if (valid)
						valid->add_child_validation("WISE.ProjectProto.ScenarioCollection", strprintf("scenarios[%d]", i), validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_scenario");
					retval = nullptr;
					break;
				}
				AddScenario(scenario);
				if (!scenario->deserialize(scen, v, "scenarioCopy", &data)) {
					weak_assert(false);
					delete scenario;
					retval = nullptr;
					break;
				}
			}
			//new scenario
			else
			{
				Scenario* scenario = nullptr;
				try {
					scenario = new Project::Scenario(this, m_project->m_timeManager, m_project->gridEngine());
				}
				catch (std::exception& e) {
					/// <summary>
					/// The COM object could not be instantiated.
					/// </summary>
					/// <type>internal</type>
					if (valid)
						valid->add_child_validation("WISE.ProjectProto.ScenarioCollection", strprintf("scenarios[%d]", i), validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_scenario");
					retval = nullptr;
					break;
				}
				AddScenario(scenario);
				if (!scenario->deserialize(scen, v, "scenario", &data)) {
					weak_assert(false);
					retval = nullptr;
					break;
				}
			}
		}
	}
	else {
		for (int i = 0; i < collection->scenariodata_size(); i++)
		{
			auto scen = collection->scenariodata(i);

			//scenario copy
			if (scen.has_copyname())
			{
				Project::Scenario* toCopy = FindName(scen.copyname().c_str());
				if (!toCopy)
				{
					if (v)
						/// <summary>
						/// The requested scenario to copy doesn't exist in the FGM.
						/// </summary>
						/// <type>user</type>
						v->add_child_validation("WISE.ProjectProto.ScenarioCollection", "scenarioCopy", validation::error_level::SEVERE, "Scenario.Copy:Missing", scen.copyname());
					throw std::logic_error("Unknown scenario");
				}

				Scenario* scenario = nullptr;
				try {
					scenario = new Project::Scenario(*toCopy, m_project->m_timeManager, m_project->gridEngine(), false);
				}
				catch (std::exception& e) {
					/// <summary>
					/// The COM object could not be instantiated.
					/// </summary>
					/// <type>internal</type>
					if (valid)
						valid->add_child_validation("WISE.ProjectProto.ScenarioCollection", strprintf("scenarios[%d]", i), validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_scenario");
					retval = nullptr;
					break;
				}
				AddScenario(scenario);
				if (!scenario->deserialize(scen, v, "scenarioCopy", &data)) {
					weak_assert(false);
					delete scenario;
					retval = nullptr;
					break;
				}
			}
			//new scenario
			else
			{
				Scenario* scenario = nullptr;
				try {
					scenario = new Project::Scenario(this, m_project->m_timeManager, m_project->gridEngine());
				}
				catch (std::exception& e) {
					/// <summary>
					/// The COM object could not be instantiated.
					/// </summary>
					/// <type>internal</type>
					if (valid)
						valid->add_child_validation("WISE.ProjectProto.ScenarioCollection", strprintf("scenarios[%d]", i), validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_scenario");
					retval = nullptr;
					break;
				}
				AddScenario(scenario);
				if (!scenario->deserialize(scen, v, "scenario", &data)) {
					weak_assert(false);
					retval = nullptr;
					break;
				}
			}
		}
	}

	if (!GetCount()) {
		weak_assert(false);		// likely a version issue with this object
		AddScenario(s);
	} else
		delete s;
	return retval;
}
