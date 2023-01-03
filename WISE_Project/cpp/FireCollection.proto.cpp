/**
 * WISE_Project: FireCollection.proto.cpp
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

#include "CWFGMProject.h"
#include "FireCollection.h"
#include "PermissibleVectorReadDrivers.h"
#include "str_printf.h"


std::int32_t Project::Fire::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::Fire::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
		auto fire = new WISE::ProjectProto::ProjectFire();
		fire->set_version(serialVersionUid(options));

		fire->set_name(m_name);
		fire->set_comments(m_comments);
		fire->set_color(GetColor());
		fire->set_fillcolor(GetFillColor());
		fire->set_size(GetLineWidth());
		fire->set_imported(IsImported());
		fire->set_symbol(GetSymbol());
		fire->set_allocated_ignition(m_fire->serialize(options));

		return fire;
	}
	else {
		auto fire = m_fire->serialize(options);
		if (fire) {
			fire->set_name(m_name);
			if (m_comments.length())
				fire->set_comments(m_comments);
			if (m_color.has_value())
				fire->set_color(m_color.value());
			if (m_fillColor.has_value())
				fire->set_fillcolor(m_fillColor.value());
			if (m_lineWidth.has_value())
				fire->set_size(m_lineWidth.value());
			if (m_symbol.has_value())
				fire->set_symbol(m_symbol.value());
			if (m_imported.has_value())
				fire->set_imported(m_imported.value());
		}
		return fire;
	}
}

auto Project::Fire::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> Fire*
{
	auto fire = dynamic_cast<const WISE::FireEngineProto::CwfgmIgnition*>(&proto);

	if (!fire)
	{
		auto fire = dynamic_cast<const WISE::ProjectProto::ProjectFire*>(&proto);

		if (!fire)
		{
			if (valid)
				valid->add_child_validation("WISE.ProjectProto.ProjectFire", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("Fire: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
		}
		if (fire->version() != 1)
		{
			if (valid)
				valid->add_child_validation("WISE.ProjectProto.ProjectFire", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(fire->version()));
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("Fire: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for ignitions.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectFire", name);
		auto v = vt.lock();

		m_name = fire->name();
		if (!fire->name().length()) {
			if (v)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, fire->name());
		}
		m_comments = fire->comments();
		m_color = fire->color();
		m_fillColor = fire->fillcolor();
		m_lineWidth = fire->size();
		m_imported = fire->imported();
		m_symbol = fire->symbol();

		if (fire->has_ignition())
		{
			std::vector<std::string> drivers = GDAL_READ_DRIVERS;
			SerializeIgnitionData data;
			data.permissible_drivers = &drivers;
			ISerializeProto* sp = m_fire.get();
			if (!sp->deserialize(fire->ignition(), v, "ignition", &data))
			{
				weak_assert(false);;
				m_fire = nullptr; //clear the intrusive pointer, hopefully that is what was meant
				return nullptr; //isn't really meaningful
			}
		}
		else if (valid) {
			/// <summary>
			/// The object doesn't contain all ignition data
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.FireEngineProto.CwfgmIgnition", "ignition", validation::error_level::SEVERE, validation::id::object_incomplete, "");
		}
	}
	else {
		if (fire->version() != 2)
		{
			if (valid)
				valid->add_child_validation("WISE.ProjectProto.ProjectFire", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(fire->version()));
			weak_assert(false);;
			throw ISerializeProto::DeserializeError("Fire: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for ignitions.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectFire", name);
		auto v = vt.lock();

		if (fire->has_name()) {
			m_name = fire->name();
			if (!fire->name().length()) {
				if (v)
					/// <summary>
					/// The scenario's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, fire->name());
				throw ISerializeProto::DeserializeError("WISE.FireEngineProto.CwfgmFire: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.FireEngineProto.CwfgmFire: Invalid or missing name.");
		}
		if (fire->has_comments())
			m_comments = fire->comments();
		if (fire->has_color())
			m_color = fire->color();
		if (fire->has_fillcolor())
			m_fillColor = fire->fillcolor();
		if (fire->has_size())
			m_lineWidth = fire->size();
		if (fire->has_imported())
			m_imported = fire->imported();
		if (fire->has_symbol())
			m_symbol = fire->symbol();

		std::vector<std::string> drivers = GDAL_READ_DRIVERS;
		SerializeIgnitionData data;
		data.permissible_drivers = &drivers;
		ISerializeProto* sp = m_fire.get();
		if (!sp->deserialize(*fire, v, "ignition", &data))
		{
			weak_assert(false);;
			m_fire = nullptr; //clear the intrusive pointer, hopefully that is what was meant
			return nullptr; //isn't really meaningful
		}
	}
	return this;
}


std::int32_t Project::FireCollection::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::FireCollection::serialize(const SerializeProtoOptions& options) {
	auto collection = new WISE::ProjectProto::FireCollection();
	collection->set_version(serialVersionUid(options));

	auto fire = (Fire*)m_fireList.LH_Head();
	while (fire->LN_Succ())
	{
		if (options.fileVersion() == 1) {
			auto ignition = collection->add_ignitions();
			auto ser = fire->serialize(options);
			ignition->Swap(dynamic_cast_assert<WISE::ProjectProto::ProjectFire *>(ser));
			delete ser;
		}
		else {
			auto ignition = collection->add_ignitiondata();
			auto ser = fire->serialize(options);
			ignition->Swap(dynamic_cast_assert<WISE::FireEngineProto::CwfgmIgnition*>(ser));
			delete ser;
		}
		fire = fire->LN_Succ();
	}

	return collection;
}


auto Project::FireCollection::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> FireCollection* {
	auto collection = dynamic_cast<const WISE::ProjectProto::FireCollection*>(&proto);

	if (!collection)
	{
		if (valid)
			valid->add_child_validation("WISE.ProjectProto.FireCollection", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(false);;
		throw ISerializeProto::DeserializeError("FireCollection: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}

	if ((collection->version() != 1) && (collection->version() != 2))
	{
		if (valid)
			valid->add_child_validation("WISE.ProjectProto.FireCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(collection->version()));
		weak_assert(false);;
		throw ISerializeProto::DeserializeError("FireCollection: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validations for fire collections.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.FireCollection", name);
	auto v = vt.lock();

	if (collection->version() == 1) {
		for (int i = 0; i < collection->ignitions_size(); i++)
		{
			Fire* fire;
			try {
				fire = new Fire(*this);
			}
			catch (std::exception& e) {
				/// <summary>
				/// The COM object could not be instantiated.
				/// </summary>
				/// <type>internal</type>
				if (v)
					v->add_child_validation("WISE.ProjectProto.ProjectFire", "ignition", validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_Ignition");
				weak_assert(false);;
				return nullptr;
			}

			ICWFGM_CommonData data;
			data.m_timeManager = m_project->m_timeManager;
			fire->m_fire->put_CommonData(&data);
			fire->m_fire->put_GridEngine(m_project->gridEngine());
			try {
				if (!fire->deserialize(collection->ignitions(i), v, strprintf("ignitions[%d]", i)))
				{
					weak_assert(false);;
					delete fire;
					return nullptr; //isn't really meaningful
				}
			}
			catch (...) {
				// Fire::deserialize, or CCWFGM_Ignition::deserialize could both throw exceptions if an object type or version is unknown
				return nullptr;
			}
			AddFire(fire);
		}
	}
	else {
		for (int i = 0; i < collection->ignitiondata_size(); i++)
		{
			Fire* fire;
			try {
				fire = new Fire(*this);
			}
			catch (std::exception& e) {
				/// <summary>
				/// The COM object could not be instantiated.
				/// </summary>
				/// <type>internal</type>
				if (v)
					v->add_child_validation("WISE.ProjectProto.ProjectFire", "ignition", validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_Ignition");
				weak_assert(false);;
				return nullptr;
			}
			ICWFGM_CommonData data;
			data.m_timeManager = m_project->m_timeManager;
			fire->m_fire->put_CommonData(&data);
			fire->m_fire->put_GridEngine(m_project->gridEngine());
			try {
				if (!fire->deserialize(collection->ignitiondata(i), v, strprintf("ignitions[%d]", i)))
				{
					weak_assert(false);;
					delete fire;
					return nullptr; //isn't really meaningful
				}
			}
			catch (...) {
				// Fire::deserialize, or CCWFGM_Ignition::deserialize could both throw exceptions if an object type or version is unknown
				return nullptr;
			}
			AddFire(fire);
		}
	}
	return this;
}
