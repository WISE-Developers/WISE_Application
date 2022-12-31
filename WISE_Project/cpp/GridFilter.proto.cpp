/**
 * WISE_Project: GridFilter.proto.cpp
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
#include "AttributeGridFilter.h"
#include "FuelGridFilter.h"
#include "GreenupGridFilter.h"
#include "PercentConiferGridFilter.h"
#include "AngleGridFilter.h"
#include "ReplaceGridFilter.h"
#include "vectorcollection.h"
#include "CWFGM_Asset.h"
#include "GridFilterCollection.h"
#include "CWFGMProject.h"
#include "WindDirectionGrid.h"
#include "WindSpeedGrid.h"

#include "CWFGM_AttributeFilter.h"
#include "CWFGM_ReplaceGridFilter.h"
#include "CWFGM_PolyReplaceGridFilter.h"
#include "CWFGM_VectorFilter.h"
#include "CWFGM_Target.h"
#include "CWFGM_FuelMap.h"
#include "CWFGM_Grid.h"
#include "doubleBuilder.h"
#include "geo_poly.h"
#include "str_printf.h"
#include "filesystem.hpp"


std::int32_t Project::AttributeGridFilter::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message*Project::AttributeGridFilter::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
	auto filter = new WISE::ProjectProto::ProjectAttributeGridFilter();
		filter->set_version(serialVersionUid(options));

	filter->set_name(m_name);
	filter->set_comments(m_comments);

	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
		if (gf) {
		filter->set_allocated_filter(gf->serialize(options));
	}

	return filter;
	}
	else {
		auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
		if (gf) {	// ******** add this check for this cast in other similar places
			auto filter = gf->serialize(options);

			filter->set_name(m_name);
			if (m_comments.length())
				filter->set_comments(m_comments);

			return filter;
		}
	}
	return nullptr;
}

auto Project::AttributeGridFilter::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> AttributeGridFilter*
{
	auto filter = dynamic_cast<const WISE::GridProto::CwfgmAttributeFilter*>(&proto);

	if (!filter)
	{
		auto filter = dynamic_cast<const WISE::ProjectProto::ProjectAttributeGridFilter*>(&proto);

		if (!filter)
		{
			if (valid)
				/// <summary>
				/// The object passed as a grid filter is invalid. An incorrect object type was passed to the parser.
				/// </summary>
				/// <type>internal</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectAttributeGridFilter", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
			weak_assert(0);
			throw ISerializeProto::DeserializeError("AttributeGridFilter: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
		}
		if (filter->version() != 1)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The grid filter is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectAttributeGridFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("AttributeGridFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for attribute filters.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectAttributeGridFilter", name);
		auto v = vt.lock();

		m_name = filter->name();
		if (!filter->name().length()) {
			if (v)
				/// <summary>
				/// The filter's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
			throw ISerializeProto::DeserializeError("WISE.ProjectProto.ProjectAttributeGridFilter: Invalid or missing name.");
		}
		m_comments = filter->comments();

		if (filter->has_filter())
		{
			auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
			if (!gf->deserialize(filter->filter(), v, "filter"))
			{
				weak_assert(0);
				return nullptr;
			}
		}
		else if (valid) {
			/// <summary>
			/// The object doesn't contain all ignition data
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.GridProto.CwfgmAttributeFilter", "filter", validation::error_level::SEVERE, validation::id::object_incomplete, "");
		}
	}
	else {
		if (filter->version() != 2)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The grid filter is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.GridProto.CwfgmAttributeFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("AttributeGridFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for attribute filters.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.GridProto.CwfgmAttributeFilter", name);
		auto v = vt.lock();

		if (filter->has_name()) {
			m_name = filter->name();
			if (!filter->name().length()) {
				if (v)
					/// <summary>
					/// The filter's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
				throw ISerializeProto::DeserializeError("WISE.GridProto.CwfgmAttributeFilter: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.GridProto.CwfgmAttributeFilter: Invalid or missing name.");
		}
		if (filter->has_comments())
			m_comments = filter->comments();

		//	if (filter->has_filter())
		//	{
		auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
		//this looks like a memory leak
		//if (!gf)
		//	gf = new CCWFGM_AttributeFilter();
		if (!gf->deserialize(*filter, v, "filter"))
		{
			weak_assert(0);
			return nullptr;
		}
	}
	return this;
}


std::int32_t Project::ReplaceGridFilter::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message*Project::ReplaceGridFilter::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
	auto filter = new WISE::ProjectProto::ProjectReplaceGridFilter();
		filter->set_version(serialVersionUid(options));

	filter->set_name(m_name);
	filter->set_comments(m_comments);

		filter->set_allocated_filter(dynamic_cast_assert<WISE::ProjectProto::ProjectReplaceGridFilterBase*>(ReplaceGridBase::serialize(options)));
	auto replace = boost::dynamic_pointer_cast<CCWFGM_ReplaceGridFilter>(m_filter);
	if (replace)
	{
		filter->set_allocated_area(replace->serialize(options));
	}

	return filter;
	}
	else {
		auto replace = boost::dynamic_pointer_cast<CCWFGM_ReplaceGridFilter>(m_filter);
		if (replace) {
			auto filter = replace->serialize(options);

			filter->set_name(m_name);
			filter->set_comments(m_comments);

			filter->set_allocated_filter(dynamic_cast_assert<WISE::GridProto::CwfgmReplaceGridFilterBase*>(ReplaceGridBase::serialize(options)));
			return filter;
		}
	}
	return nullptr;
}

auto Project::ReplaceGridFilter::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> ReplaceGridFilter*
{
	throw std::logic_error("Invalid serialization usage");
}

auto Project::ReplaceGridFilter::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name, ISerializationData* data) -> ReplaceGridFilter*
{
	auto filter = dynamic_cast<const WISE::GridProto::CwfgmReplaceGridFilter*>(&proto);

	if (!filter)
	{
	auto filter = dynamic_cast<const WISE::ProjectProto::ProjectReplaceGridFilter*>(&proto);

	if (!filter)
	{
		if (valid)
			/// <summary>
			/// The object passed as a grid filter is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectReplaceGridFilter", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("ReplaceGridFilter: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if (filter->version() != 1)
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The grid filter is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectReplaceGridFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("ReplaceGridFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validation for grid filters.
	/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectReplaceGridFilter", name);
	auto v = vt.lock();

	m_name = filter->name();
	if (!filter->name().length()) {
		if (v)
			/// <summary>
			/// The filter's name (to identify the output) is missing.
			/// </summary>
			/// <type>user</type>
			v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
			throw ISerializeProto::DeserializeError("WISE.ProjectProto.ProjectReplaceGridFilter: Invalid or missing name.");
	}
	m_comments = filter->comments();

	if (!ReplaceGridBase::deserialize(filter->filter(), v, "filter", data))
	{
		weak_assert(0);
		return nullptr;
	}

	if (filter->has_area())
	{
		auto replace = dynamic_cast<CCWFGM_ReplaceGridFilter*>(m_filter.get());
		if (replace)
		{
			if (!replace->deserialize(filter->area(), v, "area")) {
				weak_assert(0);
				delete replace;
				return nullptr;
			}
		}
	}

	} else {

		if (filter->version() != 2)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The grid filter is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectReplaceGridFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("ReplaceGridFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for grid filters.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectReplaceGridFilter", name);
		auto v = vt.lock();

		if (filter->has_name()) {
			m_name = filter->name();
			if (!filter->name().length()) {
				if (v)
					/// <summary>
					/// The filter's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
				throw ISerializeProto::DeserializeError("WISE.GridProto.CwfgmReplaceGridFilter: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.GridProto.CwfgmReplaceGridFilter: Invalid or missing name.");
		}
		if (filter->has_comments())
			m_comments = filter->comments();

		if (!ReplaceGridBase::deserialize(filter->filter(), v, "filter", data))
		{
			weak_assert(0);
			return nullptr;
		}

		auto replace = dynamic_cast<CCWFGM_ReplaceGridFilter*>(m_filter.get());
		if (replace)
		{
			if (!replace->deserialize(*filter, v, "area")) {
				weak_assert(0);
				delete replace;
				return nullptr;
			}
		}
	}
	return this;
}


std::int32_t Project::PolyReplaceGridFilter::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message*Project::PolyReplaceGridFilter::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
	auto filter = new WISE::ProjectProto::ProjectPolyReplaceGridFilter();
		filter->set_version(serialVersionUid(options));

	filter->set_name(m_name);
	filter->set_comments(m_comments);
		filter->set_allocated_color(createProtobufObject((std::uint32_t)BoundaryColor()));
		filter->set_allocated_size(createProtobufObject((std::uint64_t)GetLineWidth()));

	auto replace = dynamic_cast<CCWFGM_PolyReplaceGridFilter*>(m_filter.get());
	if (replace)
		filter->set_allocated_polygon(replace->serialize(options));
		filter->set_allocated_filter(dynamic_cast_assert<WISE::ProjectProto::ProjectReplaceGridFilterBase*>(ReplaceGridBase::serialize(options)));

	return filter;
	}
	else {
		auto replace = dynamic_cast<CCWFGM_PolyReplaceGridFilter*>(m_filter.get());
		if (replace) {
			WISE::GridProto::CwfgmPolyReplaceGridFilter* filter = replace->serialize(options);
			filter->set_allocated_filter(dynamic_cast_assert<WISE::GridProto::CwfgmReplaceGridFilterBase*>(ReplaceGridBase::serialize(options)));

			filter->set_name(m_name);
			if (m_comments.length())
				filter->set_comments(m_comments);
			if (m_color.has_value())
				filter->set_color(m_color.value());
			if (m_lineWidth.has_value())
				filter->set_size(m_lineWidth.value());
			return filter;
		}
	}
	return nullptr;
}

auto Project::PolyReplaceGridFilter::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> PolyReplaceGridFilter*
{
	throw std::logic_error("Invalid serialization usage");
}

auto Project::PolyReplaceGridFilter::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name, ISerializationData* data) -> PolyReplaceGridFilter*
{
	auto filter = dynamic_cast<const WISE::GridProto::CwfgmPolyReplaceGridFilter*>(&proto);

	if (!filter)
	{
	auto filter = dynamic_cast<const WISE::ProjectProto::ProjectPolyReplaceGridFilter*>(&proto);

	if (!filter)
	{
		if (valid)
			/// <summary>
			/// The object passed as a grid filter is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectPolyReplaceGridFilter", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("PolyReplaceGridFilter: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if (filter->version() != 1)
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The grid filter is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectPolyReplaceGridFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("PolyReplaceGridFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validation for grid filters.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectPolyReplaceGridFilter", name);
	auto v = vt.lock();

	m_name = filter->name();
	if (!filter->name().length()) {
		if (v)
			/// <summary>
			/// The filter's name (to identify the output) is missing.
			/// </summary>
			/// <type>user</type>
			v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
			throw ISerializeProto::DeserializeError("WISE.GridProto.ProjectPolyReplaceGridFilter: Invalid or missing name.");
	}
	m_comments = filter->comments();

	if (filter->has_color())
		m_color = filter->color().value();
	if (filter->has_size())
		m_lineWidth = filter->size().value();

	if (filter->shape_case() == WISE::ProjectProto::ProjectPolyReplaceGridFilter::kPolygon)
	{
		auto replace = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
		if (!replace->deserialize(filter->polygon(), v, "polygon"))
		{
			weak_assert(0);
			return nullptr;
		}
	}
	else if (filter->shape_case() == WISE::ProjectProto::ProjectPolyReplaceGridFilter::kFilename)
	{
#ifdef __GNUC__
        if (fs::exists(fs::current_path() / filter->filename().value()))
#else
		if (fs::exists(fs::relative(filter->filename().value())))
#endif
		{
			HRESULT hr;
			if (FAILED(hr = ImportPolygons(filter->filename().value(), &CWFGMProject::m_permissibleVectorReadDriversSA))) {
				if (v) {
					switch (hr) {
					case E_POINTER:						v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::e_pointer, filter->filename().value()); break;
					case E_INVALIDARG:					v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::e_invalidarg, filter->filename().value()); break;
					case E_OUTOFMEMORY:					v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::out_of_memory, filter->filename().value()); break;
					case ERROR_GRID_UNINITIALIZED:		v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::initialization_incomplete, "grid_engine"); break;
					case ERROR_FILE_NOT_FOUND:			v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::file_not_found, filter->filename().value()); break;
					case ERROR_TOO_MANY_OPEN_FILES:		v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::too_many_open_files, filter->filename().value()); break;
					case ERROR_ACCESS_DENIED:			v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::access_denied, filter->filename().value()); break;
					case ERROR_INVALID_HANDLE:			v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::invalid_handle, filter->filename().value()); break;
					case ERROR_HANDLE_DISK_FULL:		v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::disk_full, filter->filename().value()); break;
					case ERROR_FILE_EXISTS:				v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::file_exists, filter->filename().value()); break;
					case ERROR_SEVERITY_WARNING:
					case ERROR_FIREBREAK_NOT_FOUND:
					case S_FALSE:
					default:							v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::unspecified, filter->filename().value()); break;
					}
				}
				return nullptr;
			}
		}
		else {
			if (v)
				v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::file_not_found, filter->filename().value());
		}
	}

	if (!ReplaceGridBase::deserialize(filter->filter(), v, "filter", data))
	{
		weak_assert(0);
		return nullptr;
	}
	}
	else {
		if (filter->version() != 1)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The grid filter is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.GridProto.PolyCwfgmReplaceGridFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(filter->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("PolyReplaceGridFilter: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for grid filters.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.GridProto.PolyCwfgmReplaceGridFilter", name);
		auto v = vt.lock();

		if (filter->has_name()) {
			m_name = filter->name();
			if (!filter->name().length()) {
				if (v)
					/// <summary>
					/// The filter's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, filter->name());
				throw ISerializeProto::DeserializeError("WISE.GridProto.PolyCwfgmReplaceGridFilter: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.GridProto.PolyCwfgmReplaceGridFilter: Invalid or missing name.");
		}
		if (filter->has_comments())
			m_comments = filter->comments();

		if (filter->has_color())
			m_color = filter->color();
		if (filter->has_size())
			m_lineWidth = filter->size();

		if (filter->shape_case() == WISE::GridProto::CwfgmPolyReplaceGridFilter::kPolygons)
		{
			auto replace = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
			if (!replace->deserialize(*filter, v, "polygon"))
			{
				weak_assert(0);
				return nullptr;
			}
		}
		else if (filter->shape_case() == WISE::GridProto::CwfgmPolyReplaceGridFilter::kFilename)
		{
#ifdef __GNUC__
			if (fs::exists(fs::current_path() / filter->filename()))
#else
			if (fs::exists(fs::relative(filter->filename())))
#endif
			{
				HRESULT hr;
				if (FAILED(hr = ImportPolygons(filter->filename(), &CWFGMProject::m_permissibleVectorReadDriversSA))) {
					if (v) {
						switch (hr) {
						case E_POINTER:						v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::e_pointer, filter->filename()); break;
						case E_INVALIDARG:					v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::e_invalidarg, filter->filename()); break;
						case E_OUTOFMEMORY:					v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::out_of_memory, filter->filename()); break;
						case ERROR_GRID_UNINITIALIZED:		v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::initialization_incomplete, "grid_engine"); break;
						case ERROR_FILE_NOT_FOUND:			v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::file_not_found, filter->filename()); break;
						case ERROR_TOO_MANY_OPEN_FILES:		v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::too_many_open_files, filter->filename()); break;
						case ERROR_ACCESS_DENIED:			v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::access_denied, filter->filename()); break;
						case ERROR_INVALID_HANDLE:			v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::invalid_handle, filter->filename()); break;
						case ERROR_HANDLE_DISK_FULL:		v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::disk_full, filter->filename()); break;
						case ERROR_FILE_EXISTS:				v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::file_exists, filter->filename()); break;
						case ERROR_SEVERITY_WARNING:
						case ERROR_FIREBREAK_NOT_FOUND:
						case S_FALSE:
						default:							v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::unspecified, filter->filename()); break;
						}
					}
					return nullptr;
				}
			}
			else {
				if (v)
					v->add_child_validation("string", "shape.filename", validation::error_level::SEVERE, validation::id::file_not_found, filter->filename());
			}
		}

		if (!ReplaceGridBase::deserialize(filter->filter(), v, "filter", data))
		{
			weak_assert(0);
			return nullptr;
		}
	}
	return this;
}


std::int32_t Project::StaticVector::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::StaticVector::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
	auto vector = new WISE::ProjectProto::ProjectVectorFilter();
		vector->set_version(serialVersionUid(options));

	std::string name;
	if (ln_Name)
		name = ln_Name;
	vector->set_name(name);
	vector->set_comments(m_comments);
		vector->set_color(GetColor());
		vector->set_fillcolor(GetFillColor());
		vector->set_symbol(GetSymbol());
		vector->set_width(GetLineWidth());
		vector->set_imported(IsImported());

	auto filter = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	if (filter)
		vector->set_allocated_filter(filter->serialize(options));

	return vector;
	}
	else {
		auto filter = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
		if (filter) {
			auto vector = filter->serialize(options);

			std::string name;
			if (ln_Name)
				name = ln_Name;
			vector->set_name(name);
			if (m_comments.length())
				vector->set_comments(m_comments);
			if (m_col.has_value())
				vector->set_color(m_col.value());
			if (m_fillColor.has_value())
				vector->set_fillcolor(m_fillColor.value());
			if (m_symbol.has_value())
				vector->set_symbol(m_symbol.value());
			if (m_LineWidth.has_value())
				vector->set_width(m_LineWidth.value());
			if (m_imported.has_value())
				vector->set_imported(m_imported.value());
			return vector;
		}
	}
	return nullptr;
}


auto Project::StaticVector::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> StaticVector* {
	auto vector = dynamic_cast<const WISE::GridProto::CwfgmVectorFilter*>(&proto);

	if (!vector)
	{
	auto vector = dynamic_cast<const WISE::ProjectProto::ProjectVectorFilter*>(&proto);

	if (!vector)
	{
		if (valid)
			/// <summary>
			/// The object passed as a vector is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectVectorFilter", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("StaticVector: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if (vector->version() != 1)
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The vector is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectVectorFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(vector->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("StaticVector: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validation for vectors.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectVectorFilter", name);
	auto v = vt.lock();

	Name(vector->name().c_str());
	if ((!ln_Name) || (!strlen(ln_Name))) {
		if (v)
			/// <summary>
			/// The filter's name (to identify the output) is missing.
			/// </summary>
			/// <type>user</type>
			v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, vector->name());
	}
	m_comments = vector->comments();
	m_col = vector->color();
	m_fillColor = vector->fillcolor();
	m_symbol = vector->symbol();
	m_LineWidth = vector->width();
	m_imported = vector->imported();

	HRESULT hr;
	auto filter = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	if (filter)
	{
		if (vector->data_case() == WISE::ProjectProto::ProjectVectorFilter::kFilter)
		{
			if (!filter->deserialize(vector->filter(), v, "filter")) {
				weak_assert(0);
				return nullptr;
			}
		}
		else if (vector->data_case() == WISE::ProjectProto::ProjectVectorFilter::kFilename)
		{
#ifdef __GNUC__
				HSS_PRAGMA_WARNING_PUSH
					HSS_PRAGMA_GCC(GCC diagnostic ignored "-Wdeprecated-declarations")
            if (fs::exists(fs::current_path() / vector->filename()))
#else
				HSS_PRAGMA_SUPPRESS_MSVC_WARNING(4996)
			if (fs::exists(fs::relative(vector->filename())))
#endif
			{
				if (FAILED(hr = ImportPolylines(vector->file().filename(), &CWFGMProject::m_permissibleVectorReadDriversSA))) {
					if (v) {
						switch (hr) {
						case E_POINTER:						v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::e_pointer, vector->file().filename()); break;
						case E_INVALIDARG:					v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::e_invalidarg, vector->file().filename()); break;
						case E_OUTOFMEMORY:					v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::out_of_memory, vector->file().filename()); break;
						case ERROR_GRID_UNINITIALIZED:		v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::initialization_incomplete, "grid_engine"); break;
						case ERROR_FILE_NOT_FOUND:			v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_not_found, vector->file().filename()); break;
						case ERROR_TOO_MANY_OPEN_FILES:		v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::too_many_open_files, vector->file().filename()); break;
						case ERROR_ACCESS_DENIED:			v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::access_denied, vector->file().filename()); break;
						case ERROR_INVALID_HANDLE:			v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::invalid_handle, vector->file().filename()); break;
						case ERROR_HANDLE_DISK_FULL:		v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::disk_full, vector->file().filename()); break;
						case ERROR_FILE_EXISTS:				v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_exists, vector->file().filename()); break;
						case ERROR_SEVERITY_WARNING:
						case ERROR_FIREBREAK_NOT_FOUND:
						case S_FALSE:
						default:							v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::unspecified, vector->file().filename()); break;
						}
					}
					else
						return nullptr;
				}
			}
			else {
				if (v)
					v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_not_found, vector->file().filename());
			}
#ifdef __GNUC__
				HSS_PRAGMA_WARNING_POP
#endif
		}
		else if (vector->data_case() == WISE::ProjectProto::ProjectVectorFilter::kFile)
		{
			auto vt2 = validation::conditional_make_object(v, "WISE.ProjectProto.ProjectVectorFilter.VectorFile", "data.file");
			auto v2 = vt2.lock();

			auto file = vector->file();
#ifdef __GNUC__
			if (fs::exists(fs::current_path() / file.filename()))
#else
			if (fs::exists(fs::relative(file.filename())))
#endif
			{
				if (FAILED(hr = ImportPolylines(file.filename(), &CWFGMProject::m_permissibleVectorReadDriversSA))) {
					if (v2) {
						switch (hr) {
						case E_POINTER:						v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::e_pointer, vector->file().filename()); break;
						case E_INVALIDARG:					v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::e_invalidarg, vector->file().filename()); break;
						case E_OUTOFMEMORY:					v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::out_of_memory, vector->file().filename()); break;
						case ERROR_GRID_UNINITIALIZED:		v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::initialization_incomplete, "grid_engine"); break;
						case ERROR_FILE_NOT_FOUND:			v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::file_not_found, vector->file().filename()); break;
						case ERROR_TOO_MANY_OPEN_FILES:		v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::too_many_open_files, vector->file().filename()); break;
						case ERROR_ACCESS_DENIED:			v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::access_denied, vector->file().filename()); break;
						case ERROR_INVALID_HANDLE:			v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::invalid_handle, vector->file().filename()); break;
						case ERROR_HANDLE_DISK_FULL:		v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::disk_full, vector->file().filename()); break;
						case ERROR_FILE_EXISTS:				v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::file_exists, vector->file().filename()); break;
						case ERROR_SEVERITY_WARNING:
						case ERROR_FIREBREAK_NOT_FOUND:
						case S_FALSE:
						default:							v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::unspecified, vector->file().filename()); break;
						}
					}
					else
						return nullptr;
				}
			}
			else {
				if (v)
					v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::file_not_found, vector->file().filename());
			}

			if (file.has_firebreakwidth())
			{
				double dValue = DoubleBuilder().withProtobuf(file.firebreakwidth(), v, "fireBreakWidth").getValue();
				//ignore if not value
				if ((dValue > 0.0) && (dValue < 250.0))
				{
					//store the scaled fire break width
					filter->put_Width(dValue);
				}
				else if (v)
				{
					/// <summary>
					/// The firebreak width is not within the acceptable range (0, 250).
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("WISE.ProjectProto.ProjectVectorFilter", "fireBreakWidth", validation::error_level::WARNING, validation::id::value_invalid, std::to_string(file.firebreakwidth().value()));
				}
			}
		}
	}
	}
	else {
		if (vector->version() != 2)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The vector is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectVectorFilter", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(vector->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("StaticVector: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for vectors.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectVectorFilter", name);
		auto v = vt.lock();

		if (vector->has_name()) {
			Name(vector->name().c_str());
			if ((!ln_Name) || (!strlen(ln_Name))) {
				if (v)
					/// <summary>
					/// The filter's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, vector->name());
				throw ISerializeProto::DeserializeError("WISE.GridProto.CwfgmVectorFilter: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.GridProto.CwfgmVectorFilter: Invalid or missing name.");
		}
		if (vector->has_comments())
			m_comments = vector->comments();
		if (vector->has_color())
			m_col = vector->color();
		if (vector->has_fillcolor())
			m_fillColor = vector->fillcolor();
		if (vector->has_symbol())
			m_symbol = vector->symbol();
		if (vector->has_width())
			m_LineWidth = vector->width();
		if (vector->has_imported())
			m_imported = vector->imported();

		HRESULT hr;
		auto filter = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
		if (filter)
		{
			if (vector->data_case() == WISE::GridProto::CwfgmVectorFilter::kPolygons)
			{
				if (!filter->deserialize(*vector, v, "filter")) {
					weak_assert(0);
					return nullptr;
				}
			}
			else if (vector->data_case() == WISE::GridProto::CwfgmVectorFilter::kFilename)
			{
				auto vt2 = validation::conditional_make_object(v, "WISE.ProjectProto.ProjectVectorFilter.VectorFile", "data.file");
				auto v2 = vt2.lock();

#ifdef __GNUC__
				if (fs::exists(fs::current_path() / vector->filename()))
#else
				if (fs::exists(fs::relative(vector->filename())))
#endif
				{
					if (FAILED(hr = ImportPolylines(vector->filename(), &CWFGMProject::m_permissibleVectorReadDriversSA))) {
						if (v2) {
							switch (hr) {
							case E_POINTER:						v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::e_pointer, vector->filename()); break;
							case E_INVALIDARG:					v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::e_invalidarg, vector->filename()); break;
							case E_OUTOFMEMORY:					v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::out_of_memory, vector->filename()); break;
							case ERROR_GRID_UNINITIALIZED:		v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::initialization_incomplete, "grid_engine"); break;
							case ERROR_FILE_NOT_FOUND:			v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::file_not_found, vector->filename()); break;
							case ERROR_TOO_MANY_OPEN_FILES:		v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::too_many_open_files, vector->filename()); break;
							case ERROR_ACCESS_DENIED:			v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::access_denied, vector->filename()); break;
							case ERROR_INVALID_HANDLE:			v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::invalid_handle, vector->filename()); break;
							case ERROR_HANDLE_DISK_FULL:		v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::disk_full, vector->filename()); break;
							case ERROR_FILE_EXISTS:				v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::file_exists, vector->filename()); break;
							case ERROR_SEVERITY_WARNING:
							case ERROR_FIREBREAK_NOT_FOUND:
							case S_FALSE:
							default:							v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::unspecified, vector->filename()); break;
							}
						}
						else
							return nullptr;
					}
				}
				else {
					if (v)
						v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::file_not_found, vector->filename());
				}

				if (vector->has_firebreakwidth())
				{
					double dValue = DoubleBuilder().withProtobuf(vector->firebreakwidth(), v, "fireBreakWidth").getValue();
					//ignore if not value
					if ((dValue > 0.0) && (dValue < 250.0))
					{
						//store the scaled fire break width
						filter->put_Width(dValue);
					}
					else if (v)
					{
						/// <summary>
						/// The firebreak width is not within the acceptable range (0, 250).
						/// </summary>
						/// <type>user</type>
						v->add_child_validation("WISE.ProjectProto.ProjectVectorFilter", "fireBreakWidth", validation::error_level::WARNING, validation::id::value_invalid, std::to_string(vector->firebreakwidth().value()));
					}
				}
			}
		}
	}
	return this;
}


std::int32_t Project::Asset::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::Asset::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
	auto asset = new WISE::ProjectProto::ProjectAsset();
		asset->set_version(serialVersionUid(options));

	std::string name;
	if (ln_Name.length())
		name = ln_Name;
	asset->set_name(name);
	asset->set_comments(m_comments);
		asset->set_color(GetColor());
		asset->set_fillcolor(GetFillColor());
		asset->set_symbol(GetSymbol());
		asset->set_width(GetLineWidth());
		asset->set_imported(IsImported());

	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	asset->set_allocated_filter(vf->serialize(options));

	return asset;
	}
	else {
		auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
		if (vf) {
			auto asset = vf->serialize(options);

			std::string name;
			if (ln_Name.length())
				name = ln_Name;
			asset->set_name(name);
			if (m_comments.length())
				asset->set_comments(m_comments);
			if (m_col.has_value())
				asset->set_color(m_col.value());
			if (m_fillColor.has_value())
				asset->set_fillcolor(m_fillColor.value());
			if (m_symbol.has_value())
				asset->set_symbol(m_symbol.value());
			if (m_LineWidth.has_value())
				asset->set_width(m_LineWidth.value());
			if (m_imported.has_value())
				asset->set_imported(m_imported.value());
			return asset;
		}
	}
	return nullptr;
}


auto Project::Asset::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> Asset*
{
	auto asset = dynamic_cast<const WISE::GridProto::CwfgmAsset*>(&proto);

	if (!asset)
	{
	auto asset = dynamic_cast<const WISE::ProjectProto::ProjectAsset*>(&proto);

	if (!asset)
	{
		if (valid)
			/// <summary>
			/// The object passed as an asset is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectAsset", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("Asset: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if (asset->version() != 1)
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The asset is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectAsset", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(asset->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("Asset: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validation for assets.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectAsset", name);
	auto v = vt.lock();

	Name(asset->name().c_str());
		if (!asset->name().length()) {
		if (v)
			/// <summary>
			/// The filter's name (to identify the output) is missing.
			/// </summary>
			/// <type>user</type>
			v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, asset->name());
			throw ISerializeProto::DeserializeError("WISE.ProjectProto.ProjectAsset: Invalid or missing name.");
	}
	m_comments = asset->comments();
	m_col = asset->color();
	m_fillColor = asset->fillcolor();
	m_symbol = asset->symbol();
	m_LineWidth = asset->width();
	m_imported = asset->imported();

	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	if (vf)
	{
		if (asset->data_case() == WISE::ProjectProto::ProjectAsset::kFilter)
		{
			if (!vf->deserialize(asset->filter(), v, "filter"))
			{
				weak_assert(0);
				return nullptr;
			}
		}
		else if (asset->data_case() == WISE::ProjectProto::ProjectAsset::kFile)
		{
			auto file = asset->file();
#ifdef __GNUC__
			if (fs::exists(fs::current_path() / file.filename()))
#else
			if (fs::exists(fs::relative(file.filename())))
#endif
			{
				HRESULT hr;
				if (FAILED(hr = ImportPolylines(file.filename(), CWFGMProject::m_permissibleVectorReadDriversSA))) {
					switch (hr) {
					case E_POINTER:							if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::e_pointer, file.filename()); break;
					case E_INVALIDARG:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::e_invalidarg, file.filename()); break;
					case ERROR_VECTOR_UNINITIALIZED:		if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::object_invalid, file.filename()); break;
					case ERROR_SCENARIO_SIMULATION_RUNNING: if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::simulation_running, file.filename()); break;
					case E_OUTOFMEMORY:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::out_of_memory, file.filename()); break;
					case ERROR_FILE_NOT_FOUND:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_not_found, file.filename()); break;
					case ERROR_TOO_MANY_OPEN_FILES:			if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::too_many_open_files, file.filename()); break;
					case ERROR_ACCESS_DENIED:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::access_denied, file.filename()); break;
					case ERROR_INVALID_HANDLE:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::invalid_handle, file.filename()); break;
					case ERROR_HANDLE_DISK_FULL:			if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::disk_full, file.filename()); break;
					case ERROR_FILE_EXISTS:					if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_exists, file.filename()); break;
					case E_UNEXPECTED:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::grid_type_invalid, file.filename()); break;
					case S_FALSE:
					case E_FAIL:
					case ERROR_SEVERITY_WARNING:
					default:
															if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::unspecified, file.filename()); break;
					}
				}
			}
			else {
				if (v)
					v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_not_found, file.filename());
			}

			if (file.has_assetboundary())
			{
				double dValue = DoubleBuilder().withProtobuf(file.assetboundary(), v, "assetBoundary").getValue();
				//ignore if not value
				if (dValue > 0.0 && dValue < 250.0)
				{
					//store the scaled fire break width
					vf->put_Width(dValue);
				}
			}
			else
			{
				/// <summary>
				/// The asset boundary size is not within the acceptable range (0, 250).
				/// </summary>
				/// <type>user</type>
				if (v)
					v->add_child_validation("WISE.ProjectProto.ProjectAsset", "assetBoundary", validation::error_level::WARNING, "Deserialize.Parse:Invalid", std::to_string(file.assetboundary().value()));
			}
		}
	}

	} else {

		if (asset->version() != 2)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The asset is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectAsset", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(asset->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("Asset: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for assets.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectAsset", name);
		auto v = vt.lock();

		if (asset->has_name()) {
			Name(asset->name().c_str());
			if (!asset->name().length()) {
				if (v)
					/// <summary>
					/// The filter's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, asset->name());
				throw ISerializeProto::DeserializeError("WISE.ProjectProto.CwfgmAsset: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.GridProto.CwfgmAsset: Invalid or missing name.");
		}
		if (asset->has_comments())
			m_comments = asset->comments();
		if (asset->has_color())
			m_col = asset->color();
		if (asset->has_fillcolor())
			m_fillColor = asset->fillcolor();
		if (asset->has_symbol())
			m_symbol = asset->symbol();
		if (asset->has_width())
			m_LineWidth = asset->width();
		if (asset->has_imported())
			m_imported = asset->imported();

		auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
		if (vf)
		{
			if (asset->data_case() == WISE::GridProto::CwfgmAsset::kAssets)
			{
				if (!vf->deserialize(*asset, v, "filter"))
				{
					weak_assert(false);
					return nullptr;
				}
			}
			else if (asset->data_case() == WISE::GridProto::CwfgmAsset::kFilename)
			{
#ifdef __GNUC__
				if (fs::exists(fs::current_path() / asset->filename()))
#else
				if (fs::exists(fs::relative(asset->filename())))
#endif
				{
					HRESULT hr;
					if (FAILED(hr = ImportPolylines(asset->filename(), CWFGMProject::m_permissibleVectorReadDriversSA))) {
						switch (hr) {
						case E_POINTER:							if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::e_pointer, asset->filename()); break;
						case E_INVALIDARG:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::e_invalidarg, asset->filename()); break;
						case ERROR_VECTOR_UNINITIALIZED:		if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::object_invalid, asset->filename()); break;
						case ERROR_SCENARIO_SIMULATION_RUNNING: if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::simulation_running, asset->filename()); break;
						case E_OUTOFMEMORY:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::out_of_memory, asset->filename()); break;
						case ERROR_FILE_NOT_FOUND:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_not_found, asset->filename()); break;
						case ERROR_TOO_MANY_OPEN_FILES:			if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::too_many_open_files, asset->filename()); break;
						case ERROR_ACCESS_DENIED:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::access_denied, asset->filename()); break;
						case ERROR_INVALID_HANDLE:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::invalid_handle, asset->filename()); break;
						case ERROR_HANDLE_DISK_FULL:			if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::disk_full, asset->filename()); break;
						case ERROR_FILE_EXISTS:					if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_exists, asset->filename()); break;
						case E_UNEXPECTED:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::grid_type_invalid, asset->filename()); break;
						case S_FALSE:
						case E_FAIL:
						case ERROR_SEVERITY_WARNING:
						default:
							if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::unspecified, asset->filename()); break;
						}
					}
				}
				else {
					if (v)
						v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_not_found, asset->filename());
				}

				if (asset->has_assetboundary())
				{
					double dValue = DoubleBuilder().withProtobuf(asset->assetboundary(), v, "assetBoundary").getValue();
					//ignore if not value
					if (dValue > 0.0 && dValue < 250.0)
					{
						//store the scaled fire break width
						vf->put_Width(dValue);
					}
				}
				else
				{
					/// <summary>
					/// The asset boundary size is not within the acceptable range (0, 250).
					/// </summary>
					/// <type>user</type>
					if (v)
						v->add_child_validation("WISE.ProjectProto.ProjectAsset", "assetBoundary", validation::error_level::WARNING, "Deserialize.Parse:Invalid", std::to_string(asset->assetboundary().value()));
				}
			}
		}
	}
	return this;
}


std::int32_t Project::Target::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::Target::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
	HRESULT hr;
	auto target = new WISE::ProjectProto::ProjectTarget();
		target->set_version(serialVersionUid(options));

	std::string name;
	if (!ln_Name.empty())
		name = ln_Name;
	target->set_name(name);
	target->set_comments(m_comments);
		target->set_color(GetColor());
		target->set_displaysize(GetDisplaySize());
		target->set_symbol(GetSymbol());
		target->set_imported(IsImported());

	auto protos = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	auto tar = protos->serialize(options);
		target->set_allocated_filter(dynamic_cast_assert<WISE::GridProto::CwfgmTarget*>(tar));

	return target;
	}
	else {
		auto protos = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
		HRESULT hr;
		auto target = protos->serialize(options);

		std::string name;
		if (!ln_Name.empty())
			name = ln_Name;
		target->set_name(name);
		if (m_comments.length())
			target->set_comments(m_comments);
		if (m_col.has_value())
			target->set_color(m_col.value());
		if (m_displaySize.has_value())
			target->set_displaysize(m_displaySize.value());
		if (m_symbol.has_value())
			target->set_symbol(m_symbol.value());
		if (m_imported.has_value())
			target->set_imported(m_imported.value());
		return target;
	}
}


Project::Target* Project::Target::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name)
{
	auto target = dynamic_cast<const WISE::GridProto::CwfgmTarget*>(&proto);

	if (!target)
	{
	auto target = dynamic_cast<const WISE::ProjectProto::ProjectTarget*>(&proto);

	if (!target)
	{
		if (valid)
			/// <summary>
			/// The object passed as a target is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectTarget", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("Target: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if (target->version() != 1)
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The target is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectTarget", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(target->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("Target: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validation for targets.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectTarget", name);
	auto v = vt.lock();

	Name(target->name().c_str());
	if (ln_Name.empty()) {
		if (v)
			/// <summary>
			/// The filter's name (to identify the output) is missing.
			/// </summary>
			/// <type>user</type>
			v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, target->name());
	}
	m_comments = target->comments();
	m_col = target->color();
	m_symbol = target->symbol();
	m_displaySize = target->displaysize();
	m_imported = target->imported();

	if (m_target)
	{
		if (target->data_case() == WISE::ProjectProto::ProjectTarget::kFilter) {
			auto protos = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
			if (!(protos->deserialize(target->filter(), v, "target"))) {
				weak_assert(0);
				return nullptr;
			}
		}
		else if (target->data_case() == WISE::ProjectProto::ProjectTarget::kFile)
		{
			auto file = target->file();
			if (std::filesystem::exists(std::filesystem::relative(file.filename())))
			{
				HRESULT hr;
				if (FAILED(hr = ImportPointSet(file.filename(), CWFGMProject::m_permissibleVectorReadDriversSA))) {
					switch (hr) {
					case E_POINTER:							if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::e_pointer, file.filename()); break;
					case E_INVALIDARG:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::e_invalidarg, file.filename()); break;
					case ERROR_VECTOR_UNINITIALIZED:		if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::object_invalid, file.filename()); break;
					case ERROR_SCENARIO_SIMULATION_RUNNING: if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::simulation_running, file.filename()); break;
					case E_OUTOFMEMORY:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::out_of_memory, file.filename()); break;
					case ERROR_FILE_NOT_FOUND:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_not_found, file.filename()); break;
					case ERROR_TOO_MANY_OPEN_FILES:			if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::too_many_open_files, file.filename()); break;
					case ERROR_ACCESS_DENIED:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::access_denied, file.filename()); break;
					case ERROR_INVALID_HANDLE:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::invalid_handle, file.filename()); break;
					case ERROR_HANDLE_DISK_FULL:			if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::disk_full, file.filename()); break;
					case ERROR_FILE_EXISTS:					if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_exists, file.filename()); break;
					case E_UNEXPECTED:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::grid_type_invalid, file.filename()); break;
					case S_FALSE:
					case E_FAIL:
					case ERROR_SEVERITY_WARNING:
					default:
						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::unspecified, file.filename()); break;
					}
				}
			}
			else {
				if (v)
					v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_not_found, file.filename());
			}
		}
	}
	}
	else {

		if (target->version() != 2)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The target is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectTarget", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(target->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("Target: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for targets.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectTarget", name);
		auto v = vt.lock();

		if (target->has_name()) {
			Name(target->name().c_str());
			if (ln_Name.empty()) {
				if (v)
					/// <summary>
					/// The target's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, target->name());
				throw ISerializeProto::DeserializeError("WISE.GridProto.CwfgmTarget: Invalid or missing name.");
			}
		}
		else {
			if (v)
				/// <summary>
				/// The target's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, "");
			throw ISerializeProto::DeserializeError("WISE.GridProto.CwfgmTarget: Invalid or missing name.");
		}
		if (target->has_comments())
			m_comments = target->comments();
		if (target->has_color())
			m_col = target->color();
		if (target->has_symbol())
			m_symbol = target->symbol();
		if (target->has_displaysize())
			m_displaySize = target->displaysize();
		if (target->has_imported())
			m_imported = target->imported();

		if (m_target)
		{
			if (target->data_case() == WISE::GridProto::CwfgmTarget::kTargets) {
				auto protos = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
				if (!(protos->deserialize(*target, v, "target"))) {
					weak_assert(0);
					return nullptr;
				}
			}
			else if (target->data_case() == WISE::GridProto::CwfgmTarget::kFilename)
			{
				//			auto file = target->filename();
				if (std::filesystem::exists(std::filesystem::relative(target->filename())))
				{
					HRESULT hr;
					if (FAILED(hr = ImportPointSet(target->filename(), CWFGMProject::m_permissibleVectorReadDriversSA))) {
						switch (hr) {
						case E_POINTER:							if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::e_pointer, target->filename()); break;
						case E_INVALIDARG:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::e_invalidarg, target->filename()); break;
						case ERROR_VECTOR_UNINITIALIZED:		if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::object_invalid, target->filename()); break;
						case ERROR_SCENARIO_SIMULATION_RUNNING: if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::simulation_running, target->filename()); break;
						case E_OUTOFMEMORY:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::out_of_memory, target->filename()); break;
						case ERROR_FILE_NOT_FOUND:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_not_found, target->filename()); break;
						case ERROR_TOO_MANY_OPEN_FILES:			if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::too_many_open_files, target->filename()); break;
						case ERROR_ACCESS_DENIED:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::access_denied, target->filename()); break;
						case ERROR_INVALID_HANDLE:				if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::invalid_handle, target->filename()); break;
						case ERROR_HANDLE_DISK_FULL:			if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::disk_full, target->filename()); break;
						case ERROR_FILE_EXISTS:					if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_exists, target->filename()); break;
						case E_UNEXPECTED:						if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::grid_type_invalid, target->filename()); break;
						case S_FALSE:
						case E_FAIL:
						case ERROR_SEVERITY_WARNING:
						default:
							if (v) v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::unspecified, target->filename()); break;
						}
					}
				}
				else {
					if (v)
						v->add_child_validation("string", "data.filename", validation::error_level::SEVERE, validation::id::file_not_found, target->filename());
				}
			}
		}
	}
	return this;
}


std::int32_t Project::GridFilterCollection::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::GridFilterCollection::serialize(const SerializeProtoOptions& options) {
	auto collection = new WISE::ProjectProto::GridCollection();
	collection->set_version(serialVersionUid(options));

	if (options.fileVersion() == 1) {
		GridFilter* gf = FirstFilter();
	while (gf->LN_Succ())
	{
		auto replace = dynamic_cast<ReplaceGridFilter*>(gf);
		auto poly = dynamic_cast<PolyReplaceGridFilter*>(gf);
		auto attr = dynamic_cast<AttributeGridFilter*>(gf);
		auto direction = dynamic_cast<CWindDirectionGrid*>(gf);
		auto speed = dynamic_cast<CWindSpeedGrid*>(gf);
		auto weather = dynamic_cast<CWeatherGridFilter*>(gf);
		auto polyWeather = dynamic_cast<CWeatherPolyFilter*>(gf);

		if (replace)
		{
			auto filter = collection->add_filters();
			auto grid = replace->serialize(options);
				filter->set_allocated_replace(dynamic_cast_assert<WISE::ProjectProto::ProjectReplaceGridFilter*>(grid));
		}
		else if (poly)
		{
			auto filter = collection->add_filters();
			auto grid = poly->serialize(options);
				filter->set_allocated_polyreplace(dynamic_cast_assert<WISE::ProjectProto::ProjectPolyReplaceGridFilter*>(grid));
		}
		else if (attr)
		{
			auto filter = collection->add_filters();
			auto grid = attr->serialize(options);
			auto a = new WISE::ProjectProto::GridCollection_Filter_AttributeFilter();
				a->set_allocated_filter((WISE::ProjectProto::ProjectAttributeGridFilter*)grid);
			if (typeid(*gf) == typeid(FuelGridFilter))
				a->set_type(WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_FUEL_GRID);
			else if (typeid(*gf) == typeid(PercentCureGrassGridFilter))
				a->set_type(WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_DEGREE_CURING);
			else if (typeid(*gf) == typeid(GreenupGridFilter))
				a->set_type(WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_GREENUP);
			else if (typeid(*gf) == typeid(PercentConiferGridFilter))
				a->set_type(WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_PERCENT_CONIFER);
			else if (typeid(*gf) == typeid(PercentDeadFirGridFilter))
				a->set_type(WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_PERCENT_DEAD_FIR);
			else if (typeid(*gf) == typeid(CBHGridFilter))
				a->set_type(WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_CROWN_BASE_HEIGHT);
			else if (typeid(*gf) == typeid(TreeHeightGridFilter))
				a->set_type(WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_TREE_HEIGHT);
			else if (typeid(*gf) == typeid(FuelLoadGridFilter))
				a->set_type(WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_FUEL_LOAD);
			else if (typeid(*gf) == typeid(OVDGridFilter))
				a->set_type(WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_FBP_VECTOR);
			filter->set_allocated_attribute(a);
		}
		else if (direction)
		{
			auto filter = collection->add_filters();
			auto grid = direction->serialize(options);
				filter->set_allocated_wind(dynamic_cast_assert<WISE::ProjectProto::ProjectWindGrid*>(grid));
		}
		else if (speed)
		{
			auto filter = collection->add_filters();
			auto grid = speed->serialize(options);
				filter->set_allocated_wind(dynamic_cast_assert<WISE::ProjectProto::ProjectWindGrid*>(grid));
		}
		else if (weather)
		{
			auto filter = collection->add_filters();
			auto grid = weather->serialize(options);
				filter->set_allocated_weather(dynamic_cast_assert<WISE::ProjectProto::ProjectWeatherGridFilter*>(grid));
		}
		else if (polyWeather)
		{
			auto filter = collection->add_filters();
			auto grid = polyWeather->serialize(options);
				filter->set_allocated_polyweather(dynamic_cast_assert<WISE::ProjectProto::ProjectPolyWeatherGridFilter*>(grid));
		}
		else
		{
			comment_assert(false, "Missing grid filter export");
		}

		gf = gf->LN_Succ();
	}

	} else {

		GridFilter* gf = FirstFilter();
		while (gf->LN_Succ())
		{
			auto replace = dynamic_cast<ReplaceGridFilter*>(gf);
			auto poly = dynamic_cast<PolyReplaceGridFilter*>(gf);
			auto attr = dynamic_cast<AttributeGridFilter*>(gf);
			auto direction = dynamic_cast<CWindDirectionGrid*>(gf);
			auto speed = dynamic_cast<CWindSpeedGrid*>(gf);
			auto weather = dynamic_cast<CWeatherGridFilter*>(gf);
			auto polyWeather = dynamic_cast<CWeatherPolyFilter*>(gf);

			if (replace)
			{
				auto filter = collection->add_filters();
				auto grid = replace->serialize(options);
				filter->set_allocated_replacefilter(dynamic_cast_assert<WISE::GridProto::CwfgmReplaceGridFilter*>(grid));
			}
			else if (poly)
			{
				auto filter = collection->add_filters();
				auto grid = poly->serialize(options);
				filter->set_allocated_polyreplacefilter(dynamic_cast_assert<WISE::GridProto::CwfgmPolyReplaceGridFilter*>(grid));
			}
			else if (attr)
			{
				auto filter = collection->add_filters();
				auto grid = attr->serialize(options);
				filter->set_allocated_attributefilter(dynamic_cast_assert<WISE::GridProto::CwfgmAttributeFilter*>(grid));
			}
			else if (direction)
			{
				auto filter = collection->add_filters();
				auto grid = direction->serialize(options);
				filter->set_allocated_windgrid(dynamic_cast_assert<WISE::WeatherProto::WindGrid*>(grid));
			}
			else if (speed)
			{
				auto filter = collection->add_filters();
				auto grid = speed->serialize(options);
				filter->set_allocated_windgrid(dynamic_cast_assert<WISE::WeatherProto::WindGrid*>(grid));
			}
			else if (weather)
			{
				auto filter = collection->add_filters();
				auto grid = weather->serialize(options);
				filter->set_allocated_weathergridfilter(dynamic_cast_assert<WISE::WeatherProto::WeatherGridFilter*>(grid));
			}
			else if (polyWeather)
			{
				auto filter = collection->add_filters();
				auto grid = polyWeather->serialize(options);
				filter->set_allocated_weathergridfilter(dynamic_cast_assert<WISE::WeatherProto::WeatherGridFilter*>(grid));
			}
			else
			{
				comment_assert(false, "Missing grid filter export");
			}

			gf = gf->LN_Succ();
		}
	}
	return collection;
}

auto Project::GridFilterCollection::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> GridFilterCollection*
{
	auto collection = dynamic_cast<const WISE::ProjectProto::GridCollection*>(&proto);

	if (!collection)
	{
		if (valid)
			/// <summary>
			/// The object passed as a grid filter collection is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.GridCollection", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("GridFilterCollection: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if ((collection->version() != 1) && (collection->version() != 2))
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The grid filter collection is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.GridCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(collection->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("GridFilterCollection: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validation for grid filter collections.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.GridCollection", name);
	auto v = vt.lock();

	ICWFGM_CommonData* data;
	m_project->grid()->GetCommonData(nullptr, &data);

	if (collection->version() == 1) {
	for (int i = 0; i < collection->filters_size(); i++)
	{
		auto filter = collection->filters(i);
		if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kReplace)
		{
			ReplaceGridFilter* gf = new ReplaceGridFilter();
			gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
			gf->m_filter->PutCommonData(nullptr, data);
			gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
			gf->SetArea(gsl::narrow_cast<USHORT>(-1), gsl::narrow_cast<USHORT>(-1), gsl::narrow_cast<USHORT>(-1), gsl::narrow_cast<USHORT>(-1));
			gf->SetFuelMap(m_project->fuelMap());

			ReplaceGridBaseData d;
			d.m_project = m_project;
			if (!gf->deserialize(filter.replace(), v, strprintf("filters[%d]", i), &d))
			{
				weak_assert(0);
				delete gf;
				return nullptr;
			}
			AddFilter(gf);
		}
		else if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kPolyReplace)
		{
				PolyReplaceGridFilter* gf = new PolyReplaceGridFilter();
			gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
			gf->m_filter->PutCommonData(nullptr, data);
			gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
			gf->SetFuelMap(m_project->fuelMap());

			ReplaceGridBaseData d;
			d.m_project = m_project;
			if (!gf->deserialize(filter.polyreplace(), v, strprintf("filters[%d]", i), &d))
			{
				weak_assert(0);
				delete gf;
				return nullptr;
			}
			AddFilter(gf);
		}
		else if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kAttribute)
		{
			AttributeGridFilter* gf = nullptr;
			switch (filter.attribute().type())
			{
			case WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_FUEL_GRID:
				gf = new FuelGridFilter();
				break;
			case WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_DEGREE_CURING:
				gf = new PercentCureGrassGridFilter();
				break;
			case WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_GREENUP:
				gf = new GreenupGridFilter();
				break;
			case WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_PERCENT_CONIFER:
				gf = new PercentConiferGridFilter();
				break;
			case WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_PERCENT_DEAD_FIR:
				gf = new PercentDeadFirGridFilter();
				break;
			case WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_CROWN_BASE_HEIGHT:
				gf = new CBHGridFilter();
				break;
			case WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_TREE_HEIGHT:
				gf = new TreeHeightGridFilter();
				break;
			case WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_FUEL_LOAD:
				gf = new FuelLoadGridFilter();
				break;
			case WISE::ProjectProto::GridCollection_Filter_AttributeFilter_AttributeType_FBP_VECTOR:
				gf = new OVDGridFilter();
				break;
				default:
					if (v)
						v->add_child_validation("kAttribute", "filter", validation::error_level::SEVERE, validation::id::unspecified, "unknown filter type"); break;
			}

			if (gf)
			{
				gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
				gf->m_filter->PutCommonData(nullptr, data);
				gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
				gf->SetFuelMap(m_project->fuelMap());
				gf->Reset();
				if (!gf->deserialize(filter.attribute().filter(), v, strprintf("filters[%d]", i)))
				{
					weak_assert(0);
					delete gf;
					return nullptr;
				}

				if (filter.attribute().filter().has_filter() &&
					filter.attribute().filter().filter().data_case() == WISE::GridProto::CwfgmAttributeFilter::kFile)
				{
					HRESULT hr;
					if (FAILED(hr = gf->Import(m_project, filter.attribute().filter().filter().file().projection(), filter.attribute().filter().filter().file().filename()))) {
						switch (hr) {
						case E_POINTER:							if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::e_pointer, filter.attribute().filter().filter().file().filename()); break;
						case E_INVALIDARG:						if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::e_invalidarg, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_GRID_UNINITIALIZED:			if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::object_invalid, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_SCENARIO_SIMULATION_RUNNING: if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::simulation_running, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_FIRE_IGNITION_TYPE_UNKNOWN:	if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::enum_invalid, filter.attribute().filter().filter().file().filename()); break;
						case E_OUTOFMEMORY:						if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::out_of_memory, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_FILE_NOT_FOUND:				if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::file_not_found, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_TOO_MANY_OPEN_FILES:			if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::too_many_open_files, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_ACCESS_DENIED:				if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::access_denied, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_INVALID_HANDLE:				if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::invalid_handle, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_HANDLE_DISK_FULL:			if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::disk_full, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_FILE_EXISTS:					if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::file_exists, filter.attribute().filter().filter().file().filename()); break;
						case E_UNEXPECTED:						if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::grid_type_invalid, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_GRID_SIZE_INCORRECT:			if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::grid_size_mismatch, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_GRID_UNSUPPORTED_RESOLUTION:	if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::grid_resolution_mismatch, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_GRID_LOCATION_OUT_OF_RANGE:	if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::grid_projection_mismatch, filter.attribute().filter().filter().file().filename()); break;
						case ERROR_FUELS_FUEL_UNKNOWN:			if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::unknown_fuel, filter.attribute().filter().filter().file().filename()); break;
						case S_FALSE:
						case E_FAIL:
						case ERROR_SEVERITY_WARNING:
						default:
							if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::unspecified, filter.attribute().filter().filter().file().filename()); break;
						}
					}
				}

				AddFilter(gf);
			}
		}
		else if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kWind)
		{
			if (filter.wind().type() == WISE::WeatherProto::WindGrid_GridType_WindDirection)
			{
					CWindDirectionGrid* gf = new CWindDirectionGrid();
				gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
				gf->m_filter->PutCommonData(nullptr, data);
				gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
				if (!gf->deserialize(filter.wind(), v, strprintf("filters[%d]", i)))
				{
					weak_assert(0);
					delete gf;
					return nullptr;
				}
				AddFilter(gf);
			}
			else
			{
					CWindSpeedGrid* gf = new CWindSpeedGrid();
				gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
				gf->m_filter->PutCommonData(nullptr, data);
				gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
				if (!gf->deserialize(filter.wind(), v, strprintf("filters[%d]", i)))
				{
					weak_assert(0);
					delete gf;
					return nullptr;
				}
				AddFilter(gf);
			}
		}
			else if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kWeather) 
		{
			auto gf = new CWeatherGridFilter();
			gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
			gf->m_filter->PutCommonData(nullptr, data);
			gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
			if (!gf->deserialize(filter.weather(), v, strprintf("filters[%d]", i)))
			{
				weak_assert(0);
				delete gf;
				return nullptr;
			}
			AddFilter(gf);
		}
			else if ((filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kPolyWeather) &&
				(filter.polyweather().landscape() == true))
			{
				auto gf = new CWeatherGridFilter();
				gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
				gf->m_filter->PutCommonData(nullptr, data);
				gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
				if (!gf->deserialize(filter.polyweather(), v, strprintf("filters[%d]", i)))
				{
					weak_assert(0);
					delete gf;
					return nullptr;
				}
				AddFilter(gf);
			}
		else if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kPolyWeather)
		{
			auto gf = new CWeatherPolyFilter();
			gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
			gf->m_filter->PutCommonData(nullptr, data);
			gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
			if (!gf->deserialize(filter.polyweather(), v, strprintf("filters[%d]", i)))
			{
				weak_assert(0);
				delete gf;
				return nullptr;
			}
			AddFilter(gf);
		}
	}

	} else {

		for (int i = 0; i < collection->filters_size(); i++)
		{
			auto filter = collection->filters(i);
			if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kReplaceFilter)
			{
				ReplaceGridFilter* gf = new ReplaceGridFilter();
				gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
				gf->m_filter->PutCommonData(nullptr, data);
				gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
				gf->SetArea(gsl::narrow_cast<USHORT>(-1), gsl::narrow_cast<USHORT>(-1), gsl::narrow_cast<USHORT>(-1), gsl::narrow_cast<USHORT>(-1));
				gf->SetFuelMap(m_project->fuelMap());

				ReplaceGridBaseData d;
				d.m_project = m_project;
				if (!gf->deserialize(filter.replacefilter(), v, strprintf("filters[%d]", i), &d))
				{
					weak_assert(0);
					delete gf;
					return nullptr;
				}
				AddFilter(gf);
			}
			else if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kPolyReplaceFilter)
			{
				PolyReplaceGridFilter* gf = new PolyReplaceGridFilter();
				gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
				gf->m_filter->PutCommonData(nullptr, data);
				gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
				gf->SetFuelMap(m_project->fuelMap());

				ReplaceGridBaseData d;
				d.m_project = m_project;
				if (!gf->deserialize(filter.polyreplacefilter(), v, strprintf("filters[%d]", i), &d))
				{
					weak_assert(0);
					delete gf;
					return nullptr;
				}
				AddFilter(gf);
			}
			else if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kAttributeFilter)
			{
				auto af = filter.attributefilter();
				AttributeGridFilter* gf = nullptr;

				std::uint16_t optionKey;
				if (af.has_binary())
					optionKey = (std::uint16_t)af.binary().datakey();
				else
					optionKey = (std::uint16_t)af.file().datakey();

				if ((optionKey >= 15506) && (optionKey <= 15511))
					optionKey += 100;								// fixes some ancient testing files
				WISE::GridProto::CwfgmAttributeFilter_DataKey oKey = (WISE::GridProto::CwfgmAttributeFilter_DataKey)optionKey;

				switch (oKey)
				{
				case WISE::GridProto::CwfgmAttributeFilter_DataKey::CwfgmAttributeFilter_DataKey_FUEL:
					gf = new FuelGridFilter();
					break;
				case WISE::GridProto::CwfgmAttributeFilter_DataKey::CwfgmAttributeFilter_DataKey_CURINGDEGREE:
					gf = new PercentCureGrassGridFilter();
					break;
				case WISE::GridProto::CwfgmAttributeFilter_DataKey::CwfgmAttributeFilter_DataKey_GREENUP:
					gf = new GreenupGridFilter();
					break;
				case WISE::GridProto::CwfgmAttributeFilter_DataKey::CwfgmAttributeFilter_DataKey_PC:
					gf = new PercentConiferGridFilter();
					break;
				case WISE::GridProto::CwfgmAttributeFilter_DataKey::CwfgmAttributeFilter_DataKey_PDF:
					gf = new PercentDeadFirGridFilter();
					break;
				case WISE::GridProto::CwfgmAttributeFilter_DataKey::CwfgmAttributeFilter_DataKey_CBH:
					gf = new CBHGridFilter();
					break;
				case WISE::GridProto::CwfgmAttributeFilter_DataKey::CwfgmAttributeFilter_DataKey_TREEHEIGHT:
					gf = new TreeHeightGridFilter();
					break;
				case WISE::GridProto::CwfgmAttributeFilter_DataKey::CwfgmAttributeFilter_DataKey_FUELLOAD:
					gf = new FuelLoadGridFilter();
					break;
				case WISE::GridProto::CwfgmAttributeFilter_DataKey::CwfgmAttributeFilter_DataKey_FBP_OVD:
					gf = new OVDGridFilter();
					break;
				default:
					if (v)
						v->add_child_validation("DataKey", "datakey", validation::error_level::SEVERE, validation::id::unspecified, "unknown filter type"); break;
					weak_assert(0);
				}

				if (gf)
				{
					gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
					gf->m_filter->PutCommonData(nullptr, data);
					gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
					gf->SetFuelMap(m_project->fuelMap());
					gf->Reset();
					if (!gf->deserialize(af, v, strprintf("filters[%d]", i)))
					{
						weak_assert(0);
						delete gf;
						return nullptr;
					}


					if (af.data_case() == WISE::GridProto::CwfgmAttributeFilter::kFile)
					{
						HRESULT hr;
						if (FAILED(hr = gf->Import(m_project, af.file().projection(), af.file().filename()))) {
							switch (hr) {
							case E_POINTER:							if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::e_pointer, af.file().filename()); break;
							case E_INVALIDARG:						if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::e_invalidarg, af.file().filename()); break;
							case ERROR_GRID_UNINITIALIZED:			if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::object_invalid, af.file().filename()); break;
							case ERROR_SCENARIO_SIMULATION_RUNNING: if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::simulation_running, af.file().filename()); break;
							case ERROR_FIRE_IGNITION_TYPE_UNKNOWN:	if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::enum_invalid, af.file().filename()); break;
							case E_OUTOFMEMORY:						if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::out_of_memory, af.file().filename()); break;
							case ERROR_FILE_NOT_FOUND:				if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::file_not_found, af.file().filename()); break;
							case ERROR_TOO_MANY_OPEN_FILES:			if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::too_many_open_files, af.file().filename()); break;
							case ERROR_ACCESS_DENIED:				if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::access_denied, af.file().filename()); break;
							case ERROR_INVALID_HANDLE:				if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::invalid_handle, af.file().filename()); break;
							case ERROR_HANDLE_DISK_FULL:			if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::disk_full, af.file().filename()); break;
							case ERROR_FILE_EXISTS:					if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::file_exists, af.file().filename()); break;
							case E_UNEXPECTED:						if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::grid_type_invalid, af.file().filename()); break;
							case ERROR_GRID_SIZE_INCORRECT:			if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::grid_size_mismatch, af.file().filename()); break;
							case ERROR_GRID_UNSUPPORTED_RESOLUTION:	if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::grid_resolution_mismatch, af.file().filename()); break;
							case ERROR_GRID_LOCATION_OUT_OF_RANGE:	if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::grid_projection_mismatch, af.file().filename()); break;
							case ERROR_FUELS_FUEL_UNKNOWN:			if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::unknown_fuel, af.file().filename()); break;
							case S_FALSE:
							case E_FAIL:
							case ERROR_SEVERITY_WARNING:
							default:
								if (v) v->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::unspecified, af.file().filename()); break;
							}
						}
					}

					AddFilter(gf);
				}
				else {
					weak_assert(0);
				}
			}
			else if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kWindGrid)
			{
				if (filter.windgrid().type() == WISE::WeatherProto::WindGrid_GridType_WindDirection)
				{
					CWindDirectionGrid* gf = new CWindDirectionGrid();
					gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
					gf->m_filter->PutCommonData(nullptr, data);
					gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
					if (!gf->deserialize(filter.windgrid(), v, strprintf("filters[%d]", i)))
					{
						weak_assert(0);
						delete gf;
						return nullptr;
					}
					AddFilter(gf);
				}
				else
				{
					CWindSpeedGrid* gf = new CWindSpeedGrid();
					gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
					gf->m_filter->PutCommonData(nullptr, data);
					gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
					if (!gf->deserialize(filter.windgrid(), v, strprintf("filters[%d]", i)))
					{
						weak_assert(0);
						delete gf;
						return nullptr;
					}
					AddFilter(gf);
				}
			}
			else if (filter.filter_case() == WISE::ProjectProto::GridCollection_Filter::kWeatherGridFilter)
			{
				auto wf = filter.weathergridfilter();
				if (wf.shape_case() == WISE::WeatherProto::WeatherGridFilter::kLandscape) {
					auto gf = new CWeatherGridFilter();
					gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
					gf->m_filter->PutCommonData(nullptr, data);
					gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
					if (!gf->deserialize(filter.weathergridfilter(), v, strprintf("filters[%d]", i)))
					{
						weak_assert(0);
						delete gf;
						return nullptr;
					}
					AddFilter(gf);
				}
				else
				{
					auto gf = new CWeatherPolyFilter();
					gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
					gf->m_filter->PutCommonData(nullptr, data);
					gf->m_filter->PutGridEngine(nullptr, m_project->gridEngine());
					if (!gf->deserialize(filter.weathergridfilter(), v, strprintf("filters[%d]", i)))
					{
						weak_assert(0);
						delete gf;
						return nullptr;
					}
					AddFilter(gf);
				}
			}
			else {
				weak_assert(0);
			}
		}
	}
	return this;
}

google::protobuf::Message* Project::ReplaceGridBase::serialize(const SerializeProtoOptions& options)
{
	if (options.fileVersion() == 1) {
	auto grid = new WISE::ProjectProto::ProjectReplaceGridFilterBase();
	grid->set_version(ReplaceGridBase::serialVersionUid);

	std::uint8_t idx2;
		ICWFGM_Fuel* from_fuel, * to_fuel;
	HRESULT hr = GetRelationship(&from_fuel, &idx2, &to_fuel);
	if (SUCCEEDED(hr))
	{
		if (!from_fuel)
			grid->set_fromfuelrule(WISE::ProjectProto::ProjectReplaceGridFilterBase_FromFuelRule_ALL_FUELS);
		else if (from_fuel == (ICWFGM_Fuel*)(-2))
			grid->set_fromfuelrule(WISE::ProjectProto::ProjectReplaceGridFilterBase_FromFuelRule_NODATA);
		else if (from_fuel == (ICWFGM_Fuel*)~0)
			grid->set_fromfuelrule(WISE::ProjectProto::ProjectReplaceGridFilterBase_FromFuelRule_ALL_COMBUSTIBLE_FUELS);
		else
			grid->set_fromfuelname(Fuel::FromCOM(from_fuel)->Name());

		if (to_fuel)
			grid->set_tofuelname(Fuel::FromCOM(to_fuel)->Name());
	}
	return grid;
	}
	else {
		auto grid = new WISE::GridProto::CwfgmReplaceGridFilterBase();
		grid->set_version(ReplaceGridBase::serialVersionUid);

		std::uint8_t idx2;
		ICWFGM_Fuel* from_fuel, * to_fuel;
		HRESULT hr = GetRelationship(&from_fuel, &idx2, &to_fuel);
		if (SUCCEEDED(hr))
		{
			if (!from_fuel)
				grid->set_fromfuelrule(WISE::GridProto::CwfgmReplaceGridFilterBase_FromFuelRule_ALL_FUELS);
			else if (from_fuel == (ICWFGM_Fuel*)(-2))
				grid->set_fromfuelrule(WISE::GridProto::CwfgmReplaceGridFilterBase_FromFuelRule_NODATA);
			else if (from_fuel == (ICWFGM_Fuel*)~0)
				grid->set_fromfuelrule(WISE::GridProto::CwfgmReplaceGridFilterBase_FromFuelRule_ALL_COMBUSTIBLE_FUELS);
			else
				grid->set_fromfuelname(Fuel::FromCOM(from_fuel)->Name());

			if (to_fuel)
				grid->set_tofuelname(Fuel::FromCOM(to_fuel)->Name());
		}
		return grid;
	}
}

auto Project::ReplaceGridBase::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> ReplaceGridBase*
{
	throw std::logic_error("Invalid deserialization usage");
}

auto Project::ReplaceGridBase::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name, ISerializationData* data) -> ReplaceGridBase*
{
	ReplaceGridBaseData *dproject = dynamic_cast<ReplaceGridBaseData *>(data);
	if (!dproject)
	{
		weak_assert(0);
		throw std::invalid_argument("ReplaceGridBase: Parameter invalid");
	}
	const CWFGMProject* project = dproject->m_project;;

	auto grid = dynamic_cast<const WISE::GridProto::CwfgmReplaceGridFilterBase*>(&proto);

	if (!grid)
	{
	auto grid = dynamic_cast<const WISE::ProjectProto::ProjectReplaceGridFilterBase*>(&proto);

	if (!grid)
	{
		if (valid)
			/// <summary>
			/// The object passed as a grid filter is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectReplaceGridFilterBase", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("ReplaceGridBase: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}

	if (grid->version() != 1)
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The grid filter is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.ProjectReplaceGridFilterBase", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(grid->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("ReplaceGridBase: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	bool from = true;
		ICWFGM_Fuel* fromFuel = nullptr, * toFuel = nullptr;
	std::uint8_t fromIndex = (std::uint8_t)-1;
	long export_idx;
	if (grid->fromFuel_case() == WISE::ProjectProto::ProjectReplaceGridFilterBase::kFromFuelName)
	{
			Fuel* ff = project->m_fuelCollection.FindName(grid->fromfuelname().c_str());
		if (ff)
			fromFuel = ff->m_fuel.get();
		else
			from = false;
	}
	else if (grid->fromFuel_case() == WISE::ProjectProto::ProjectReplaceGridFilterBase::kFromFuelRule)
	{
		if (grid->fromfuelrule() == WISE::ProjectProto::ProjectReplaceGridFilterBase_FromFuelRule_ALL_COMBUSTIBLE_FUELS)
				fromFuel = (ICWFGM_Fuel*)~0;
		else if (grid->fromfuelrule() == WISE::ProjectProto::ProjectReplaceGridFilterBase_FromFuelRule_ALL_FUELS)
			fromFuel = NULL;
		else
				fromFuel = (ICWFGM_Fuel*)(-2);
	}
	else
	{
		long exportFileIndex;
		if (FAILED(project->fuelMap()->FuelAtFileIndex(grid->fromfuelindex(), &fromIndex, &exportFileIndex, &fromFuel)))
			from = false;
	}

	if (from)
	{
		if (grid->toFuel_case() == WISE::ProjectProto::ProjectReplaceGridFilterBase::kToFuelName)
		{
				Fuel* ff = project->m_fuelCollection.FindName(grid->tofuelname().c_str());
			if (ff)
			{
				toFuel = ff->m_fuel.get();
				SetRelationship(fromFuel, fromIndex, toFuel);
			}
		}
		else if (grid->toFuel_case() == WISE::ProjectProto::ProjectReplaceGridFilterBase::kToFuelIndex)
		{
			std::uint8_t toIndex;
			project->fuelMap()->FuelAtFileIndex(grid->tofuelindex(), &toIndex, &export_idx, &toFuel);
			if (toFuel)
				SetRelationship(fromFuel, fromIndex, toFuel);
		}
	}
	}
	else {
		if (grid->version() != 1)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The grid filter is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.ProjectReplaceGridFilterBase", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(grid->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("ReplaceGridBase: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		bool from = true;
		ICWFGM_Fuel* fromFuel = nullptr, * toFuel = nullptr;
		std::uint8_t fromIndex = (std::uint8_t)-1;
		long export_idx;
		if (grid->fromFuel_case() == WISE::GridProto::CwfgmReplaceGridFilterBase::kFromFuelName)
		{
			Fuel* ff = project->m_fuelCollection.FindName(grid->fromfuelname().c_str());
			if (ff)
				fromFuel = ff->m_fuel.get();
			else
				from = false;
		}
		else if (grid->fromFuel_case() == WISE::GridProto::CwfgmReplaceGridFilterBase::kFromFuelRule)
		{
			if (grid->fromfuelrule() == WISE::GridProto::CwfgmReplaceGridFilterBase_FromFuelRule_ALL_COMBUSTIBLE_FUELS)
				fromFuel = (ICWFGM_Fuel*)~0;
			else if (grid->fromfuelrule() == WISE::GridProto::CwfgmReplaceGridFilterBase_FromFuelRule_ALL_FUELS)
				fromFuel = NULL;
			else
				fromFuel = (ICWFGM_Fuel*)(-2);
		}
		else
		{
			long exportFileIndex;
			if (FAILED(project->fuelMap()->FuelAtFileIndex(grid->fromfuelindex(), &fromIndex, &exportFileIndex, &fromFuel)))
				from = false;
		}

		if (from)
		{
			if (grid->toFuel_case() == WISE::GridProto::CwfgmReplaceGridFilterBase::kToFuelName)
			{
				Fuel* ff = project->m_fuelCollection.FindName(grid->tofuelname().c_str());
				if (ff)
				{
					toFuel = ff->m_fuel.get();
					SetRelationship(fromFuel, fromIndex, toFuel);
				}
			}
			else if (grid->toFuel_case() == WISE::GridProto::CwfgmReplaceGridFilterBase::kToFuelIndex)
			{
				std::uint8_t toIndex;
				project->fuelMap()->FuelAtFileIndex(grid->tofuelindex(), &toIndex, &export_idx, &toFuel);
				if (toFuel)
					SetRelationship(fromFuel, fromIndex, toFuel);
			}
		}
	}
	return this;
}


std::int32_t Project::VectorCollection::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::VectorCollection::serialize(const SerializeProtoOptions& options) {
	WISE::ProjectProto::VectorCollection *collection = new WISE::ProjectProto::VectorCollection();
	collection->set_version(serialVersionUid(options));

	Vector* vec = FirstVector();
	while (vec->LN_Succ())
	{
		auto vector = dynamic_cast<StaticVector*>(vec);
		if (vector)
		{
			if (options.fileVersion() == 1) {
			auto filter = collection->add_vectors();
				filter->set_allocated_filter(dynamic_cast_assert<WISE::ProjectProto::ProjectVectorFilter *>(vector->serialize(options)));
		}
			else {
				auto filter = collection->add_vectordata();
				auto ser = vector->serialize(options);
				filter->Swap(dynamic_cast_assert<WISE::GridProto::CwfgmVectorFilter*>(ser));
				delete ser;
			}
		}
		vec = vec->LN_Succ();
	}

	return collection;
}


auto Project::VectorCollection::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> VectorCollection*
{
	auto collection = dynamic_cast<const WISE::ProjectProto::VectorCollection*>(&proto);

	if (!collection)
	{
		if (valid)
			/// <summary>
			/// The object passed as a vector collection is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.VectorCollection", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("VectorCollection: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if ((collection->version() != 1) && (collection->version() != 2))
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The vector collection is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.VectorCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(collection->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("VectorCollection: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validation for vector collections.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.VectorCollection", name);
	auto v2 = vt.lock();

	ICWFGM_CommonData* data;
	m_project->grid()->GetCommonData(nullptr, &data);

	if (collection->version() == 1) {
		for (int i = 0; i < collection->vectors_size(); i++)
		{
			auto filter = collection->vectors(i);
			if (filter.has_filter())
			{
				auto v = new StaticVector();
				AddVector(v);
				v->m_filter->put_CommonData(data);
				v->gridEngine(m_gridEngine.get());
				if (!v->deserialize(filter.filter(), v2, strprintf("vectors[%d]", i)))
				{
					weak_assert(0);
					return nullptr;
				}

				PolymorphicAttribute attr;
				if (SUCCEEDED(m_gridEngine->GetAttribute(0, CWFGM_GRID_ATTRIBUTE_PLOTRESOLUTION, &attr)))
				{
					double resolution;
					if (SUCCEEDED(VariantToDouble_(attr, &resolution)))
					{
						double width = v->GetPolyLineWidth();
						if ((width * resolution) > 10000.0)
						{
							width = 10000.0 / resolution;
							v->SetPolyLineWidth(width);
							m_loadWarning += "Geographic data '";
							m_loadWarning += v->Name();
							m_loadWarning += "' width is too large. It has been reset to 10000.0m.";
						}
					}
				}
			}
		}
	}
	else {
		for (int i = 0; i < collection->vectordata().size(); i++)
		{
			auto filter = collection->vectordata(i);
			{
				auto v = new StaticVector();
				AddVector(v);
				v->m_filter->put_CommonData(data);
				v->gridEngine(m_gridEngine.get());
				if (!v->deserialize(filter, v2, strprintf("vectors[%d]", i)))
				{
					weak_assert(0);
					return nullptr;
				}

				PolymorphicAttribute attr;
				if (SUCCEEDED(m_gridEngine->GetAttribute(0, CWFGM_GRID_ATTRIBUTE_PLOTRESOLUTION, &attr)))
				{
					double resolution;
					if (SUCCEEDED(VariantToDouble_(attr, &resolution)))
					{
						double width = v->GetPolyLineWidth();
						if ((width * resolution) > 10000.0)
						{
							width = 10000.0 / resolution;
							v->SetPolyLineWidth(width);
							m_loadWarning += "Geographic data '";
							m_loadWarning += v->Name();
							m_loadWarning += "' width is too large. It has been reset to 10000.0m.";
						}
					}
				}
			}
		}
	}
	return this;
}


std::int32_t Project::AssetCollection::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::AssetCollection::serialize(const SerializeProtoOptions& options) {
	auto collection = new WISE::ProjectProto::AssetCollection();
	collection->set_version(serialVersionUid(options));

	Asset* vec = FirstAsset();
	while (vec->LN_Succ())
	{
		if (options.fileVersion() == 1) {
		auto filter = collection->add_assets();
		auto ser = vec->serialize(options);
			filter->Swap(dynamic_cast_assert<WISE::ProjectProto::ProjectAsset*>(ser));
		delete ser;
		}
		else {
			auto filter = collection->add_valuesatrisk();
			auto ser = vec->serialize(options);
			filter->Swap(dynamic_cast_assert<WISE::GridProto::CwfgmAsset*>(ser));
			delete ser;
		}
		vec = vec->LN_Succ();
	}

	return collection;
}


auto Project::AssetCollection::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> AssetCollection*
{
	auto collection = dynamic_cast<const WISE::ProjectProto::AssetCollection*>(&proto);

	if (!collection)
	{
		if (valid)
			/// <summary>
			/// The object passed as an asset collection is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.AssetCollection", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("AssetCollection: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if ((collection->version() != 1) && (collection->version() != 2))
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The asset collection is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.AssetCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(collection->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("AssetCollection: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validation for asset collections.
	/// </summary>
	auto vt2 = validation::conditional_make_object(valid, "WISE.ProjectProto.AssetCollection", name);
	auto v2 = vt2.lock();

	if (collection->version() == 1) {
	for (int i = 0; i < collection->assets_size(); i++)
	{
		auto filter = collection->assets(i);
		Asset* v;
		try {
			v = new Asset();
		}
		catch (std::exception& e) {
			/// <summary>
			/// The COM object could not be instantiated.
			/// </summary>
			/// <type>internal</type>
			if (valid)
				valid->add_child_validation("WISE.ProjectProto.AssetCollection", strprintf("assets[%d]", i), validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_Asset");
			return nullptr;
		}
		AddAsset(v);
		v->gridEngine(m_project->gridEngine());
		if (!v->deserialize(filter, v2, strprintf("assets[%d]", i))) {
			weak_assert(0);
			return nullptr;
		}

		PolymorphicAttribute attr;
		if (SUCCEEDED(m_project->gridEngine()->GetAttribute(0, CWFGM_GRID_ATTRIBUTE_PLOTRESOLUTION, &attr)))
		{
			double resolution;
			if (SUCCEEDED(VariantToDouble_(attr, &resolution)))
			{
				double width = v->GetPolyLineWidth();
				if ((width * resolution) > 10000.0)
				{
					width = 10000.0 / resolution;
					v->SetPolyLineWidth(width);
					m_loadWarning += "Geographic data '";
					m_loadWarning += v->Name();
					m_loadWarning += "' width is too large. It has been reset to 10000.0m.";
				}
			}
			else {
				if (valid)
					/// <summary>
					/// The plot resolution is not readable but should be by this time in deserialization.
					/// </summary>
					/// <type>internal</type>
					valid->add_child_validation("WISE.ProjectProto.AssetCollection", name, validation::error_level::SEVERE,
						validation::id::initialization_incomplete, "resolution");
			}
		}
		else {
			if (valid)
				/// <summary>
				/// The plot resolution is not readable but should be by this time in deserialization.
				/// </summary>
				/// <type>internal</type>
				valid->add_child_validation("WISE.ProjectProto.AssetCollection", name, validation::error_level::SEVERE,
					validation::id::initialization_incomplete, "resolution");
			weak_assert(0);
			m_loadWarning = "Error: WISE.ProjectProto.AssetCollection: Incomplete initialization";
			return nullptr;
		}
	}
	}
	else {
		for (int i = 0; i < collection->valuesatrisk_size(); i++)
		{
			auto filter = collection->valuesatrisk(i);
			Asset* v;
			try {
				v = new Asset();
			}
			catch (std::exception& e) {
				/// <summary>
				/// The COM object could not be instantiated.
				/// </summary>
				/// <type>internal</type>
				if (valid)
					valid->add_child_validation("WISE.ProjectProto.AssetCollection", strprintf("assets[%d]", i), validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_Asset");
				return nullptr;
			}
			AddAsset(v);
			v->gridEngine(m_project->gridEngine());
			if (!v->deserialize(filter, v2, strprintf("assets[%d]", i))) {
				weak_assert(0);
				return nullptr;
			}

			PolymorphicAttribute attr;
			if (SUCCEEDED(m_project->gridEngine()->GetAttribute(0, CWFGM_GRID_ATTRIBUTE_PLOTRESOLUTION, &attr)))
			{
				double resolution;
				if (SUCCEEDED(VariantToDouble_(attr, &resolution)))
				{
					double width = v->GetPolyLineWidth();
					if ((width * resolution) > 10000.0)
					{
						width = 10000.0 / resolution;
						v->SetPolyLineWidth(width);
						m_loadWarning += "Geographic data '";
						m_loadWarning += v->Name();
						m_loadWarning += "' width is too large. It has been reset to 10000.0m.";
					}
				}
				else {
					if (valid)
						/// <summary>
						/// The plot resolution is not readable but should be by this time in deserialization.
						/// </summary>
						/// <type>internal</type>
						valid->add_child_validation("WISE.ProjectProto.AssetCollection", name, validation::error_level::SEVERE,
							validation::id::initialization_incomplete, "resolution");
				}
			}
			else {
				if (valid)
					/// <summary>
					/// The plot resolution is not readable but should be by this time in deserialization.
					/// </summary>
					/// <type>internal</type>
					valid->add_child_validation("WISE.ProjectProto.AssetCollection", name, validation::error_level::SEVERE,
						validation::id::initialization_incomplete, "resolution");
				weak_assert(0);
				m_loadWarning = "Error: WISE.ProjectProto.AssetCollection: Incomplete initialization";
				return nullptr;
			}
		}
	}
	return this;
}


std::int32_t Project::TargetCollection::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::TargetCollection::serialize(const SerializeProtoOptions& options)
{
	auto collection = new WISE::ProjectProto::TargetCollection();
	collection->set_version(serialVersionUid(options));

	Target* t = FirstTarget();
	while (t->LN_Succ())
	{
		if (options.fileVersion() == 1) {
		auto filter = collection->add_targets();
		auto ser = t->serialize(options);
			filter->Swap(dynamic_cast_assert<WISE::ProjectProto::ProjectTarget*>(ser));
		delete ser;
		}
		else {
			auto filter = collection->add_targetdata();
			auto ser = t->serialize(options);
			filter->Swap(dynamic_cast_assert<WISE::GridProto::CwfgmTarget*>(ser));
			delete ser;
		}
		t = t->LN_Succ();
	}

	return collection;
}


Project::TargetCollection* Project::TargetCollection::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name)
{
	auto collection = dynamic_cast<const WISE::ProjectProto::TargetCollection*>(&proto);

	if (!collection)
	{
		if (valid)
			/// <summary>
			/// The object passed as a target collection is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.TargetCollection", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("TargetCollection: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if ((collection->version() != 1) && (collection->version() != 2))
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The target collection is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.TargetCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(collection->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("TargetCollection: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validation for target collections.
	/// </summary>
	auto vt2 = validation::conditional_make_object(valid, "WISE.ProjectProto.TargetCollection", name);
	auto v2 = vt2.lock();

	if (collection->version() == 1) {
	for (int i = 0; i < collection->targets_size(); i++)
	{
		auto filter = collection->targets(i);
		Target* v;
		try
        {
			v = new Target();
		}
        catch (std::exception& e)
        {
			/// <summary>
			/// The COM object could not be instantiated.
			/// </summary>
			/// <type>internal</type>
			if (valid)
				valid->add_child_validation("WISE.ProjectProto.TargetCollection", strprintf("targets[%d]", i), validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_Target");
			return nullptr;
		}

			AddTarget(v);
		v->gridEngine(m_project->gridEngine());
		if (!v->deserialize(filter, v2, strprintf("targets[%d]", i))) {
			weak_assert(0);
			return nullptr;
		}
	}
	}
	else {
		for (int i = 0; i < collection->targetdata_size(); i++)
		{
			auto filter = collection->targetdata(i);
			Target* v;
			try
			{
				v = new Target();
			}
			catch (std::exception& e)
			{
				/// <summary>
				/// The COM object could not be instantiated.
				/// </summary>
				/// <type>internal</type>
				if (valid)
					valid->add_child_validation("WISE.ProjectProto.TargetCollection", strprintf("targets[%d]", i), validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_Target");
				return nullptr;
			}

			AddTarget(v);
			v->gridEngine(m_project->gridEngine());
			if (!v->deserialize(filter, v2, strprintf("targets[%d]", i))) {
				weak_assert(0);
				return nullptr;
			}
		}
	}
	return this;
}
