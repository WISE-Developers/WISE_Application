/**
 * WISE_Project: FuelCollection.proto.cpp
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

#ifndef _SERIALIZE_PROTO
#define _SERIALIZE_PROTO
#endif

#include "FuelCollection.h"

#include "points.h"
#include "doubleBuilder.h"

#include <fstream>
#include <boost/algorithm/string.hpp>

#include <google/protobuf/util/json_util.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "filesystem.hpp"
#include "str_printf.h"


std::int32_t Project::FuelCollection::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::FuelCollection::serialize(const SerializeProtoOptions& options) {
	if (options.fileVersion() == 1) {
		auto collection = new WISE::ProjectProto::FuelCollection();
		collection->set_version(serialVersionUid(options));

		collection->set_allocated_importedlut(createProtobufObject(IsImported()));
		if (m_showUnusedFuels.has_value())
			collection->set_allocated_showunused(createProtobufObject(m_showUnusedFuels.value()));
		else
			collection->set_allocated_showunused(createProtobufObject(false));
		if (m_csLUTFileName.has_value())
			collection->set_allocated_lutfilename(createProtobufObject(m_csLUTFileName.value()));

		long fi, fi2, export_i;
		std::uint8_t b, j = 0;
		ICWFGM_Fuel* f;
		Fuel* fuel;

		for (std::int16_t i = 254; i >= 0; i--)
			if (SUCCEEDED(m_fuelMap->FuelAtIndex(i, &fi, &export_i, &f)))
			{
				j = (std::uint8_t)(i + 1);
				break;
			}

		for (std::uint8_t i = 0; i < j; i++)
		{
			if (SUCCEEDED(m_fuelMap->FuelAtIndex(i, &fi, &export_i, &f)))
			{
				fuel = Fuel::FromCOM(f);
				auto ff = collection->add_fuels();
				auto ff2 = fuel->serialize(options);
				ff->Swap(dynamic_cast_assert<WISE::ProjectProto::ProjectFuel*>(ff2));
				delete ff2;
			}
			else
			{
				auto ff = collection->add_fuels();
				ff->set_version(0);
			}
		}

		return collection;

	} else {

		weak_assert(false);
		return nullptr;
	}
}

google::protobuf::Message* Project::FuelCollection::serialize(const SerializeProtoOptions& options, ISerializationData* userData) {
	if (options.fileVersion() != 1) {
		FuelCollectionSerializationData* fd = dynamic_cast_assert<FuelCollectionSerializationData*>(userData);
		if (!fd)
			return nullptr;
		auto collection = fd->fuelMap;
		collection->set_version(serialVersionUid(options));

		if (m_isImportedLUT.has_value())
			collection->set_importedlut(m_isImportedLUT.value());
		if (m_showUnusedFuels.has_value())
			collection->set_showunused(m_showUnusedFuels.value());
		if (m_csLUTFileName.has_value())
			collection->set_allocated_lutfilename(new std::string(m_csLUTFileName.value()));

		long fi, fi2, export_i;
		std::uint8_t b, j = 0;
		ICWFGM_Fuel* f;
		Fuel* fuel;

		for (std::int16_t i = 254; i >= 0; i--)
			if (SUCCEEDED(m_fuelMap->FuelAtIndex(i, &fi, &export_i, &f)))
			{
				j = (std::uint8_t)(i + 1);
				break;
			}

		for (std::uint8_t i = 0; i < j; i++)
		{
			if (SUCCEEDED(m_fuelMap->FuelAtIndex(i, &fi, &export_i, &f)))
			{
				fuel = Fuel::FromCOM(f);
				FuelSerializationData fc;
				fc.fuel = const_cast<WISE::GridProto::CwfgmFuelMap_CwfgmFuelData*>(&collection->data(i));
				auto ff2 = fuel->serialize(options, &fc);
			}
		}
		return collection;

	} else {

		weak_assert(false);
		return nullptr;
	}
}

auto Project::FuelCollection::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> FuelCollection*
{
	auto collection = dynamic_cast<const WISE::GridProto::CwfgmFuelMap*>(&proto);

	if (!collection)
	{
		auto collection = dynamic_cast<const WISE::ProjectProto::FuelCollection*>(&proto);

		if (!collection)
		{
			if (valid)
				/// <summary>
				/// The object passed as a fuel collection is invalid. An incorrect object type was passed to the parser.
				/// </summary>
				/// <type>internal</type>
				valid->add_child_validation("WISE.ProjectProto.FuelCollection", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
			weak_assert(0);
			throw ISerializeProto::DeserializeError("FuelCollection: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
		}
		if (collection->version() != 1)
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The fuel collection is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.FuelCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(collection->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("FuelCollection: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.FuelCollection", name);
		auto myValid = vt.lock();

		if (collection->has_importedlut())
			m_isImportedLUT = collection->importedlut().value();
		if (collection->has_showunused())
			m_showUnusedFuels = collection->showunused().value();
		if (collection->has_lutfilename()) {
			m_csLUTFileName = collection->lutfilename().value();
			//no fuel map defined in the proto object
			if (collection->fuels_size() == 0)
			{
#ifdef _MSC_VER
				if (fs::exists(std::filesystem::relative(m_csLUTFileName.value())))
#else
				if (fs::exists(fs::current_path() / m_csLUTFileName.value()))
#endif
				{
					USHORT cnt = ImportFuelMap(m_csLUTFileName.value().c_str(), this, NULL, NULL, NULL);
					if (cnt == (USHORT)-1) {
						if (myValid)
							/// <summary>
							/// The LUT file could not be parsed or is not valid.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.FuelCollection", "fuels", validation::error_level::SEVERE, "Fuel.LUT:Invalid", collection->lutfilename().value());
						throw std::runtime_error("Unable to load LUT file.");
					}
					else if (cnt == (USHORT)-2) {
						if (myValid)
							/// <summary>
							/// The LUT file is not in a correct format.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.FuelCollection", "fuels", validation::error_level::SEVERE, "Fuel.LUT:Format", collection->lutfilename().value());
						throw std::runtime_error("Invalid LUT file format.");
					}
					else if (!cnt) {
						if (myValid)
							/// <summary>
							/// No fuels were found in the LUT file.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.FuelCollection", "fuels", validation::error_level::SEVERE, "Fuel.LUT:Empty", collection->lutfilename().value());
						throw std::runtime_error("Error loading LUT file.");
					}
				}
			}
		}

		long fi, fi2, export_i;
		std::uint8_t b;
		ICWFGM_Fuel* f;
		Fuel* fuel;
		for (std::uint8_t i = 0; i < 255 && i < collection->fuels_size(); i++)
		{
			auto imported = collection->fuels(i);
			// using 0 for an empty entry, possibly due to a deleted fuel
			if (imported.version() != 0)
			{
				if (SUCCEEDED(m_fuelMap->FuelAtIndex(i, &fi, &export_i, &f)))
				{
					Fuel* fff = Fuel::FromCOM(f);

					if (!fff) {
						fuel = NewFuel(f, 0, "", "");
						if (!fuel->deserialize(imported, myValid, strprintf("fuels[%d]", i)))
						{
							weak_assert(0);
							return nullptr;
						}
					}
				}
			}
		}

	} else {

		if ((collection->version() != 1) && (collection->version() != 2))
		{
			if (valid)
				/// <summary>
				/// The object version is not supported. The fuel collection is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.FuelCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(collection->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("FuelCollection: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.FuelCollection", name);
		auto myValid = vt.lock();

		if (collection->has_importedlut())
			m_isImportedLUT = collection->importedlut();
		if (collection->has_showunused())
			m_showUnusedFuels = collection->showunused();
		if (collection->has_lutfilename())
		{
			m_csLUTFileName = collection->lutfilename();
			//no fuel map defined in the proto object
			if (collection->data_size() == 0)
			{
#ifdef _MSC_VER
				if (fs::exists(std::filesystem::relative(m_csLUTFileName.value())))
#else
				if (fs::exists(fs::current_path() / m_csLUTFileName.value()))
#endif
				{
					USHORT cnt = ImportFuelMap(m_csLUTFileName.value().c_str(), this, NULL, NULL, NULL);
					if (cnt == (USHORT)-1) {
						if (myValid)
							/// <summary>
							/// The LUT file could not be parsed or is not valid.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.FuelCollection", "fuels", validation::error_level::SEVERE, "Fuel.LUT:Invalid", collection->lutfilename());
						throw std::runtime_error("Unable to load LUT file.");
					}
					else if (cnt == (USHORT)-2) {
						if (myValid)
							/// <summary>
							/// The LUT file is not in a correct format.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.FuelCollection", "fuels", validation::error_level::SEVERE, "Fuel.LUT:Format", collection->lutfilename());
						throw std::runtime_error("Invalid LUT file format.");
					}
					else if (!cnt) {
						if (myValid)
							/// <summary>
							/// No fuels were found in the LUT file.
							/// </summary>
							/// <type>user</type>
							myValid->add_child_validation("WISE.ProjectProto.FuelCollection", "fuels", validation::error_level::SEVERE, "Fuel.LUT:Empty", collection->lutfilename());
						throw std::runtime_error("Error loading LUT file.");
					}
				}
			}
		}

		long fi, fi2, export_i;
		std::uint8_t b;
		ICWFGM_Fuel* f;
		Fuel* fuel;
		for (std::uint8_t i = 0; i < 255 && i < collection->data_size(); i++)
		{
			if (collection->data(i).has_fueldata()) {
				auto imported = collection->data(i).fueldata();
				// using 0 for an empty entry, possibly due to a deleted fuel
				if (imported.version() != 0)
				{
					if (SUCCEEDED(m_fuelMap->FuelAtIndex(i, &fi, &export_i, &f)))
					{
						Fuel* fff = Fuel::FromCOM(f);

						if (!fff) {
							fuel = NewFuel(f, 0, /*"",*/ "", "");
							if (!fuel->deserialize(imported, myValid, strprintf("fuels[%d]", i)))
							{
								weak_assert(0);
								return nullptr;
							}
						}
					}
				}
			}
			else {
				weak_assert(false);
			}
		}
	}

#ifdef _DEBUG
	std::uint8_t count, unique_count, i;
	long		ASCII_index, tmp, export_index;
	ICWFGM_Fuel* fuel;

	m_fuelMap->GetFuelCount(&count, &unique_count);
	for (i = 0; i < unique_count; i++) {
		if (FAILED(m_fuelMap->FuelAtIndex(i, &ASCII_index, &export_index, &fuel))) {
			unique_count++;
			continue;
		}
		Project::Fuel* f = Project::Fuel::FromCOM(fuel);
		weak_assert(f);
	}
#endif

	return this;
}


bool Project::FuelCollection::LoadProtoFuelmap(const char *filename)
{
	WISE::GridProto::CwfgmFuelMap* fmap = nullptr;
	fs::path p(filename);
	if (boost::iequals(p.extension().string(), ".fuelmapj"))
	{
		std::ifstream in(filename);
		if (in.is_open())
		{
			google::protobuf::util::JsonParseOptions options;
			options.ignore_unknown_fields = true;

			std::string json((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
			fmap = new WISE::GridProto::CwfgmFuelMap();
			if (google::protobuf::util::JsonStringToMessage(json, fmap, options).ok())
			{
				delete fmap;
				fmap = nullptr;
			}
			in.close();
		}
	}
	else if (boost::iequals(p.extension().string(), ".fuelmapb"))
	{
		std::ifstream in(filename, std::ios::binary);
		if (in.is_open())
		{
			//reset the stream
			in.clear();
			in.seekg(0, std::ios::beg);
			//try to parse the data
			fmap = new WISE::GridProto::CwfgmFuelMap();
			if (!fmap->ParseFromIstream(&in))
			{
				delete fmap;
				fmap = nullptr;
			}
			in.close();
		}
	}

	if (fmap)
	{
		if (!m_fuelMap->deserialize(*fmap, nullptr, ""))
		{
			weak_assert(0);
			delete fmap;
			return false;
		}
		if (!deserialize(*fmap, nullptr, "")) {
			weak_assert(0);
			delete fmap;
			return false;
		}
		delete fmap;
		return true;
	}
	return false;
}


bool Project::FuelCollection::SaveProtoFuelmap(const char *filename)
{
	long fi, fi2, export_i;
	std::uint8_t b;
	ICWFGM_Fuel *f;
	Project::Fuel *fuel;
	HRESULT hr;

	fs::path p(filename);
	SerializeProtoOptions options;
	options.setVerboseOutput(false);
	options.setFileVersion(2);
	//only compress arrays in JSON files
	if (boost::iequals(p.extension().string(), ".fuelmapj"))
	{
		options.setZipOutput(true);
		options.setVerboseFloat(true);
	}

	auto fmap = m_fuelMap->serialize(options);
	if (fmap)
	{
		FuelCollectionSerializationData fd;
		fd.fuelMap = fmap;

		serialize(options, &fd);

		if (boost::iequals(p.extension().string(), ".fuelmapj"))
		{
			std::string json;
			google::protobuf::util::JsonOptions joptions;
			joptions.add_whitespace = true;
			joptions.always_print_primitive_fields = true;
			joptions.preserve_proto_field_names = true;

			google::protobuf::util::MessageToJsonString(*fmap, &json, joptions);

			std::ofstream out(filename);
			if (out.is_open())
			{
				out << json;
				out.close();

				delete fmap;
				return true;
			}
		}
		else if (boost::iequals(p.extension().string(), ".fuelmapb"))
		{
			std::ofstream out(filename, std::ios::binary);
			if (out.is_open())
			{
				google::protobuf::io::OstreamOutputStream os(&out);
				fmap->SerializeToZeroCopyStream(&os);

				delete fmap;
				return true;
			}
		}
	}
	delete fmap;
	return false;
}


std::int32_t Project::Fuel::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


google::protobuf::Message* Project::Fuel::serialize(const SerializeProtoOptions& options)
{
	if (options.fileVersion() == 1) {
		auto fuel = new WISE::ProjectProto::ProjectFuel();
		fuel->set_version(serialVersionUid(options));

		fuel->set_color(Color());
		fuel->set_name(Name());
		fuel->set_comments(m_comments);
		auto flags = new WISE::ProjectProto::ProjectFuel_Flags();
		if ((m_flags & Fuel::FLAG_NONSTANDARD))
			flags->set_allocated_isnonstandard(createProtobufObject(true));
		if ((m_flags & Fuel::FLAG_MODIFIED))
			flags->set_allocated_ismodified(createProtobufObject(true));
		if ((m_flags & Fuel::FLAG_MODIFIED_NAME))
			flags->set_allocated_ismodifiedname(createProtobufObject(true));
		if ((m_flags & Fuel::FLAG_USED))
			flags->set_allocated_isused(createProtobufObject(true));
		fuel->set_allocated_flags(flags);
		fuel->set_default_(m_defaultFuelFBP);

		return fuel;
	} else {
		weak_assert(0);
		return nullptr;
	}
}

google::protobuf::Message* Project::Fuel::serialize(const SerializeProtoOptions& options, ISerializationData* userData)
{
	if (options.fileVersion() != 1) {
		FuelSerializationData* fd = dynamic_cast_assert<FuelSerializationData*>(userData);
		if (!fd)
			return nullptr;
		
		WISE::FuelProto::CcwfgmFuel* fuel = nullptr;

		auto fueldata = fd->fuel;
		if (fueldata) {
			if (fueldata->has_fueldata())
				fuel = const_cast<WISE::FuelProto::CcwfgmFuel*>(&fueldata->fueldata());

			if (fuel) {
				if (m_col.has_value())
					fuel->set_color(m_col.value());
				fuel->set_comments(m_comments);
				auto flags = new WISE::FuelProto::CcwfgmFuel_Flags();
				if ((m_flags & Fuel::FLAG_NONSTANDARD))
					flags->set_isnonstandard(true);
				if ((m_flags & Fuel::FLAG_MODIFIED))
					flags->set_ismodified(true);
				if ((m_flags & Fuel::FLAG_MODIFIED_NAME))
					flags->set_ismodifiedname(true);
				if ((m_flags & Fuel::FLAG_USED))
					flags->set_isused(true);
				fuel->set_allocated_flags(flags);
			}
		}
		return fueldata;
	} else {
		weak_assert(0);
		return nullptr;
	}
}

auto Project::Fuel::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> Fuel*
{
	auto fuel = dynamic_cast<const WISE::FuelProto::CcwfgmFuel*>(&proto);

	if (!fuel)
	{
		auto fuel = dynamic_cast<const WISE::ProjectProto::ProjectFuel*>(&proto);

		if (!fuel)
		{
			if (valid)
				valid->add_child_validation("WISE.ProjectProto.ProjectFuel", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
			weak_assert(0);
			throw ISerializeProto::DeserializeError("Fuel: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
		}
		if (fuel->version() != 1)
		{
			if (valid)
				valid->add_child_validation("WISE.ProjectProto.ProjectFuel", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(fuel->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("Fuel: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for fuels.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectFuel", name);
		auto v = vt.lock();

		m_col = fuel->color();
		if (!fuel->name().length()) {
			if (v)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v->add_child_validation("string", "name", validation::error_level::SEVERE, validation::id::missing_name, fuel->name());
		}
		Name(fuel->name());
		m_comments = fuel->comments();
		if (fuel->has_flags())
		{
			if (fuel->flags().has_isnonstandard())
				m_flags |= (fuel->flags().isnonstandard().value()) ? Fuel::FLAG_NONSTANDARD : 0;
			if (fuel->flags().has_ismodified())
				m_flags |= (fuel->flags().ismodified().value()) ? Fuel::FLAG_MODIFIED : 0;
			if (fuel->flags().has_ismodifiedname())
				m_flags |= (fuel->flags().ismodifiedname().value()) ? Fuel::FLAG_MODIFIED_NAME : 0;
			if (fuel->flags().has_isused())
				m_flags |= (fuel->flags().isused().value()) ? Fuel::FLAG_USED : 0;
		}
		m_defaultFuelFBP = fuel->default_();

	} else {

		if (fuel->version() < 2)
		{
			if (valid)
				valid->add_child_validation("WISE.ProjectProto.ProjectFuel", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(fuel->version()));
			weak_assert(0);
			throw ISerializeProto::DeserializeError("Fuel: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
		}

		/// <summary>
		/// Child validation for fuels.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.ProjectFuel", name);
		auto v = vt.lock();

		m_col = fuel->color();
		m_comments = fuel->comments();
		if (fuel->has_flags())
		{
			if (fuel->flags().has_isnonstandard())
				m_flags |= (fuel->flags().isnonstandard()) ? Fuel::FLAG_NONSTANDARD : 0;
			if (fuel->flags().has_ismodified())
				m_flags |= (fuel->flags().ismodified()) ? Fuel::FLAG_MODIFIED : 0;
			if (fuel->flags().has_ismodifiedname())
				m_flags |= (fuel->flags().ismodifiedname()) ? Fuel::FLAG_MODIFIED_NAME : 0;
			if (fuel->flags().has_isused())
				m_flags |= (fuel->flags().isused()) ? Fuel::FLAG_USED : 0;
		}
		if (fuel->has_data()) {
			if (fuel->data().has_fuel()) {
				auto a = fuel->data().fuel().defaultfueltype();
				const char* ca = ICWFGM_FBPFuel::FuelNameToChar(a);

				if (!strcmp(ca, "Non"))
					ca = "NonFuel";
				else if (!strcmp(ca, "O1A"))
					ca = "O1a";
				else if (!strcmp(ca, "O1B"))
					ca = "O1b";
				m_defaultFuelFBP = ca;
			}
		}
	}

	return this;
}
