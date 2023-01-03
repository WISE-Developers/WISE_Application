/**
 * WISE_Project: CWFGMProject.proto.cpp
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

#ifdef _MSC_VER
#include <winerror.h>
#endif
#include "types.h"
#include "cwfgmProject.pb.h"
#include "CWFGMProject.h"
#include "doubleBuilder.h"
#include "minizip_stream.h"
#include "PrintReportOptions.h"
#include "AttributeGridFilter.h"
#include "ReplaceGridFilter.h"
#include "FuelCom_ext.h"
#include "WeatherGridFilter.h"
#include "WindDirectionGrid.h"
#include "WindSpeedGrid.h"
#include <fstream>
#include "SummaryCollection.h"
#include "geo_poly.h"
#include "str_printf.h"
#include "stdchar.h"

#include "filesystem.hpp"

#include <google/protobuf/util/json_util.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "zip_helper.h"
#include <gsl/pointers>
#include "boost_bimap.h"
#include "str_printf.h"

using namespace std::string_literals;


boost::bimap<WISE::ProjectProto::GlobalStatistics, WISE::ProjectProto::Project_Outputs_GridOutput_StatisticsType> gridStatMap =
makeBimap<WISE::ProjectProto::GlobalStatistics, WISE::ProjectProto::Project_Outputs_GridOutput_StatisticsType>({
	{WISE::ProjectProto::GlobalStatistics::TEMPERATURE, WISE::ProjectProto::Project_Outputs_GridOutput::TEMPERATURE},
	{WISE::ProjectProto::GlobalStatistics::DEW_POINT, WISE::ProjectProto::Project_Outputs_GridOutput::DEW_POINT},
	{WISE::ProjectProto::GlobalStatistics::RELATIVE_HUMIDITY, WISE::ProjectProto::Project_Outputs_GridOutput::RELATIVE_HUMIDITY},
	{WISE::ProjectProto::GlobalStatistics::WIND_DIRECTION, WISE::ProjectProto::Project_Outputs_GridOutput::WIND_DIRECTION},
	{WISE::ProjectProto::GlobalStatistics::WIND_SPEED, WISE::ProjectProto::Project_Outputs_GridOutput::WIND_SPEED},
	{WISE::ProjectProto::GlobalStatistics::PRECIPITATION, WISE::ProjectProto::Project_Outputs_GridOutput::PRECIPITATION},
	{WISE::ProjectProto::GlobalStatistics::FFMC, WISE::ProjectProto::Project_Outputs_GridOutput::FFMC},
	{WISE::ProjectProto::GlobalStatistics::ISI, WISE::ProjectProto::Project_Outputs_GridOutput::ISI},
	{WISE::ProjectProto::GlobalStatistics::FWI, WISE::ProjectProto::Project_Outputs_GridOutput::FWI},
	{WISE::ProjectProto::GlobalStatistics::BUI, WISE::ProjectProto::Project_Outputs_GridOutput::BUI},
	{WISE::ProjectProto::GlobalStatistics::MAX_FI, WISE::ProjectProto::Project_Outputs_GridOutput::FI},
	{WISE::ProjectProto::GlobalStatistics::MAX_FL, WISE::ProjectProto::Project_Outputs_GridOutput::FL},
	{WISE::ProjectProto::GlobalStatistics::MAX_ROS, WISE::ProjectProto::Project_Outputs_GridOutput::ROS},
	{WISE::ProjectProto::GlobalStatistics::MAX_SFC, WISE::ProjectProto::Project_Outputs_GridOutput::SFC},
	{WISE::ProjectProto::GlobalStatistics::MAX_CFC, WISE::ProjectProto::Project_Outputs_GridOutput::CFC},
	{WISE::ProjectProto::GlobalStatistics::MAX_TFC, WISE::ProjectProto::Project_Outputs_GridOutput::TFC},
	{WISE::ProjectProto::GlobalStatistics::MAX_CFB, WISE::ProjectProto::Project_Outputs_GridOutput::CFB},
	{WISE::ProjectProto::GlobalStatistics::RAZ, WISE::ProjectProto::Project_Outputs_GridOutput::RAZ},
	{WISE::ProjectProto::GlobalStatistics::BURN_GRID, WISE::ProjectProto::Project_Outputs_GridOutput::BURN_GRID},
	{WISE::ProjectProto::GlobalStatistics::FIRE_ARRIVAL_TIME, WISE::ProjectProto::Project_Outputs_GridOutput::FIRE_ARRIVAL_TIME},
	{WISE::ProjectProto::GlobalStatistics::HROS, WISE::ProjectProto::Project_Outputs_GridOutput::HROS},
	{WISE::ProjectProto::GlobalStatistics::FROS, WISE::ProjectProto::Project_Outputs_GridOutput::FROS},
	{WISE::ProjectProto::GlobalStatistics::BROS, WISE::ProjectProto::Project_Outputs_GridOutput::BROS},
	{WISE::ProjectProto::GlobalStatistics::RSS, WISE::ProjectProto::Project_Outputs_GridOutput::RSS},
	{WISE::ProjectProto::GlobalStatistics::ACTIVE_PERIMETER, WISE::ProjectProto::Project_Outputs_GridOutput::ACTIVE},
	{WISE::ProjectProto::GlobalStatistics::BURN, WISE::ProjectProto::Project_Outputs_GridOutput::BURN},
	{WISE::ProjectProto::GlobalStatistics::BURN_PERCENTAGE, WISE::ProjectProto::Project_Outputs_GridOutput::BURN_PERCENTAGE},
	{WISE::ProjectProto::GlobalStatistics::CRITICAL_PATH, WISE::ProjectProto::Project_Outputs_GridOutput::CRITICAL_PATH},
	{WISE::ProjectProto::GlobalStatistics::CRITICAL_PATH_PERCENTAGE, WISE::ProjectProto::Project_Outputs_GridOutput::CRITICAL_PATH_MEAN},
	{WISE::ProjectProto::GlobalStatistics::FIRE_ARRIVAL_TIME_MIN, WISE::ProjectProto::Project_Outputs_GridOutput::FIRE_ARRIVAL_TIME_MIN},
	{WISE::ProjectProto::GlobalStatistics::FIRE_ARRIVAL_TIME_MAX, WISE::ProjectProto::Project_Outputs_GridOutput::FIRE_ARRIVAL_TIME_MAX},
	{WISE::ProjectProto::GlobalStatistics::TOTAL_FUEL_CONSUMED, WISE::ProjectProto::Project_Outputs_GridOutput::TOTAL_FUEL_CONSUMED},
	{WISE::ProjectProto::GlobalStatistics::SURFACE_FUEL_CONSUMED, WISE::ProjectProto::Project_Outputs_GridOutput::SURFACE_FUEL_CONSUMED},
	{WISE::ProjectProto::GlobalStatistics::CROWN_FUEL_CONSUMED, WISE::ProjectProto::Project_Outputs_GridOutput::CROWN_FUEL_CONSUMED},
	{WISE::ProjectProto::GlobalStatistics::RADIATIVE_POWER, WISE::ProjectProto::Project_Outputs_GridOutput::RADIATIVE_POWER},
	{WISE::ProjectProto::GlobalStatistics::HFI, WISE::ProjectProto::Project_Outputs_GridOutput::HFI},
	{WISE::ProjectProto::GlobalStatistics::HCFB, WISE::ProjectProto::Project_Outputs_GridOutput::HCFB},
	{WISE::ProjectProto::GlobalStatistics::HROS_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::HROS_MAP},
	{WISE::ProjectProto::GlobalStatistics::FROS_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::FROS_MAP},
	{WISE::ProjectProto::GlobalStatistics::BROS_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::BROS_MAP},
	{WISE::ProjectProto::GlobalStatistics::RSS_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::RSS_MAP},
	{WISE::ProjectProto::GlobalStatistics::RAZ_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::RAZ_MAP},
	{WISE::ProjectProto::GlobalStatistics::FMC_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::FMC_MAP},
	{WISE::ProjectProto::GlobalStatistics::CFB_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::CFB_MAP},
	{WISE::ProjectProto::GlobalStatistics::CFC_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::CFC_MAP},
	{WISE::ProjectProto::GlobalStatistics::SFC_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::SFC_MAP},
	{WISE::ProjectProto::GlobalStatistics::TFC_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::TFC_MAP},
	{WISE::ProjectProto::GlobalStatistics::FI_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::FI_MAP},
	{WISE::ProjectProto::GlobalStatistics::FL_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::FL_MAP},
	{WISE::ProjectProto::GlobalStatistics::CURINGDEGREE_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::CURINGDEGREE_MAP},
	{WISE::ProjectProto::GlobalStatistics::GREENUP_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::GREENUP_MAP},
	{WISE::ProjectProto::GlobalStatistics::PC_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::PC_MAP},
	{WISE::ProjectProto::GlobalStatistics::PDF_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::PDF_MAP},
	{WISE::ProjectProto::GlobalStatistics::CBH_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::CBH_MAP},
	{WISE::ProjectProto::GlobalStatistics::TREE_HEIGHT_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::TREE_HEIGHT_MAP},
	{WISE::ProjectProto::GlobalStatistics::FUEL_LOAD_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::FUEL_LOAD_MAP},
	{WISE::ProjectProto::GlobalStatistics::CFL_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::CFL_MAP},
	{WISE::ProjectProto::GlobalStatistics::GRASSPHENOLOGY_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::GRASSPHENOLOGY_MAP},
	{WISE::ProjectProto::GlobalStatistics::ROSVECTOR_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::ROSVECTOR_MAP},
	{WISE::ProjectProto::GlobalStatistics::DIRVECTOR_MAP, WISE::ProjectProto::Project_Outputs_GridOutput::DIRVECTOR_MAP}
	});

boost::bimap<WISE::ProjectProto::GlobalStatistics, WISE::ProjectProto::Project_Outputs_StatsOutput_StatisticsType> statStatMap =
makeBimap<WISE::ProjectProto::GlobalStatistics, WISE::ProjectProto::Project_Outputs_StatsOutput_StatisticsType>({
    { WISE::ProjectProto::GlobalStatistics::DATE_TIME, WISE::ProjectProto::Project_Outputs_StatsOutput::DATE_TIME },
	{ WISE::ProjectProto::GlobalStatistics::ELAPSED_TIME, WISE::ProjectProto::Project_Outputs_StatsOutput::ELAPSED_TIME },
	{ WISE::ProjectProto::GlobalStatistics::TIME_STEP_DURATION, WISE::ProjectProto::Project_Outputs_StatsOutput::TIME_STEP_DURATION },
    { WISE::ProjectProto::GlobalStatistics::TEMPERATURE, WISE::ProjectProto::Project_Outputs_StatsOutput::TEMPERATURE },
    { WISE::ProjectProto::GlobalStatistics::DEW_POINT, WISE::ProjectProto::Project_Outputs_StatsOutput::DEW_POINT },
    { WISE::ProjectProto::GlobalStatistics::RELATIVE_HUMIDITY, WISE::ProjectProto::Project_Outputs_StatsOutput::RELATIVE_HUMIDITY },
    { WISE::ProjectProto::GlobalStatistics::WIND_SPEED, WISE::ProjectProto::Project_Outputs_StatsOutput::WIND_SPEED },
    { WISE::ProjectProto::GlobalStatistics::WIND_DIRECTION, WISE::ProjectProto::Project_Outputs_StatsOutput::WIND_DIRECTION },
    { WISE::ProjectProto::GlobalStatistics::PRECIPITATION, WISE::ProjectProto::Project_Outputs_StatsOutput::PRECIPITATION },
    { WISE::ProjectProto::GlobalStatistics::HFFMC, WISE::ProjectProto::Project_Outputs_StatsOutput::HFFMC },
    { WISE::ProjectProto::GlobalStatistics::HISI, WISE::ProjectProto::Project_Outputs_StatsOutput::HISI },
    { WISE::ProjectProto::GlobalStatistics::DMC, WISE::ProjectProto::Project_Outputs_StatsOutput::DMC },
    { WISE::ProjectProto::GlobalStatistics::DC, WISE::ProjectProto::Project_Outputs_StatsOutput::DC },
    { WISE::ProjectProto::GlobalStatistics::HFWI, WISE::ProjectProto::Project_Outputs_StatsOutput::HFWI },
    { WISE::ProjectProto::GlobalStatistics::BUI, WISE::ProjectProto::Project_Outputs_StatsOutput::BUI },
    { WISE::ProjectProto::GlobalStatistics::FFMC, WISE::ProjectProto::Project_Outputs_StatsOutput::FFMC },
    { WISE::ProjectProto::GlobalStatistics::ISI, WISE::ProjectProto::Project_Outputs_StatsOutput::ISI },
    { WISE::ProjectProto::GlobalStatistics::FWI, WISE::ProjectProto::Project_Outputs_StatsOutput::FWI },
    { WISE::ProjectProto::GlobalStatistics::TIMESTEP_AREA, WISE::ProjectProto::Project_Outputs_StatsOutput::TIMESTEP_AREA },
    { WISE::ProjectProto::GlobalStatistics::TIMESTEP_BURN_AREA, WISE::ProjectProto::Project_Outputs_StatsOutput::TIMESTEP_BURN_AREA },
    { WISE::ProjectProto::GlobalStatistics::TOTAL_AREA, WISE::ProjectProto::Project_Outputs_StatsOutput::TOTAL_AREA },
    { WISE::ProjectProto::GlobalStatistics::TOTAL_BURN_AREA, WISE::ProjectProto::Project_Outputs_StatsOutput::TOTAL_BURN_AREA },
    { WISE::ProjectProto::GlobalStatistics::AREA_GROWTH_RATE, WISE::ProjectProto::Project_Outputs_StatsOutput::AREA_GROWTH_RATE },
    { WISE::ProjectProto::GlobalStatistics::EXTERIOR_PERIMETER, WISE::ProjectProto::Project_Outputs_StatsOutput::EXTERIOR_PERIMETER },
    { WISE::ProjectProto::GlobalStatistics::EXTERIOR_PERIMETER_GROWTH_RATE, WISE::ProjectProto::Project_Outputs_StatsOutput::EXTERIOR_PERIMETER_GROWTH_RATE },
    { WISE::ProjectProto::GlobalStatistics::ACTIVE_PERIMETER, WISE::ProjectProto::Project_Outputs_StatsOutput::ACTIVE_PERIMETER },
    { WISE::ProjectProto::GlobalStatistics::ACTIVE_PERIMETER_GROWTH_RATE, WISE::ProjectProto::Project_Outputs_StatsOutput::ACTIVE_PERIMETER_GROWTH_RATE },
    { WISE::ProjectProto::GlobalStatistics::TOTAL_PERIMETER, WISE::ProjectProto::Project_Outputs_StatsOutput::TOTAL_PERIMETER },
    { WISE::ProjectProto::GlobalStatistics::TOTAL_PERIMETER_GROWTH_RATE, WISE::ProjectProto::Project_Outputs_StatsOutput::TOTAL_PERIMETER_GROWTH_RATE },
    { WISE::ProjectProto::GlobalStatistics::FI_LT_10, WISE::ProjectProto::Project_Outputs_StatsOutput::FI_LT_10 },
    { WISE::ProjectProto::GlobalStatistics::FI_10_500, WISE::ProjectProto::Project_Outputs_StatsOutput::FI_10_500 },
    { WISE::ProjectProto::GlobalStatistics::FI_500_2000, WISE::ProjectProto::Project_Outputs_StatsOutput::FI_500_2000 },
    { WISE::ProjectProto::GlobalStatistics::FI_2000_4000, WISE::ProjectProto::Project_Outputs_StatsOutput::FI_2000_4000 },
    { WISE::ProjectProto::GlobalStatistics::FI_4000_10000, WISE::ProjectProto::Project_Outputs_StatsOutput::FI_4000_10000 },
    { WISE::ProjectProto::GlobalStatistics::FI_GT_10000, WISE::ProjectProto::Project_Outputs_StatsOutput::FI_GT_10000 },
    { WISE::ProjectProto::GlobalStatistics::ROS_0_1, WISE::ProjectProto::Project_Outputs_StatsOutput::ROS_0_1 },
    { WISE::ProjectProto::GlobalStatistics::ROS_2_4, WISE::ProjectProto::Project_Outputs_StatsOutput::ROS_2_4 },
    { WISE::ProjectProto::GlobalStatistics::ROS_5_8, WISE::ProjectProto::Project_Outputs_StatsOutput::ROS_5_8 },
    { WISE::ProjectProto::GlobalStatistics::ROS_9_14, WISE::ProjectProto::Project_Outputs_StatsOutput::ROS_9_14 },
    { WISE::ProjectProto::GlobalStatistics::ROS_GT_15, WISE::ProjectProto::Project_Outputs_StatsOutput::ROS_GT_15 },
    { WISE::ProjectProto::GlobalStatistics::MAX_ROS, WISE::ProjectProto::Project_Outputs_StatsOutput::MAX_ROS },
    { WISE::ProjectProto::GlobalStatistics::MAX_FI, WISE::ProjectProto::Project_Outputs_StatsOutput::MAX_FI },
    { WISE::ProjectProto::GlobalStatistics::MAX_FL, WISE::ProjectProto::Project_Outputs_StatsOutput::MAX_FL },
    { WISE::ProjectProto::GlobalStatistics::MAX_CFB, WISE::ProjectProto::Project_Outputs_StatsOutput::MAX_CFB },
    { WISE::ProjectProto::GlobalStatistics::MAX_CFC, WISE::ProjectProto::Project_Outputs_StatsOutput::MAX_CFC },
    { WISE::ProjectProto::GlobalStatistics::MAX_SFC, WISE::ProjectProto::Project_Outputs_StatsOutput::MAX_SFC },
    { WISE::ProjectProto::GlobalStatistics::MAX_TFC, WISE::ProjectProto::Project_Outputs_StatsOutput::MAX_TFC },
    { WISE::ProjectProto::GlobalStatistics::TOTAL_FUEL_CONSUMED, WISE::ProjectProto::Project_Outputs_StatsOutput::TOTAL_FUEL_CONSUMED },
    { WISE::ProjectProto::GlobalStatistics::CROWN_FUEL_CONSUMED, WISE::ProjectProto::Project_Outputs_StatsOutput::CROWN_FUEL_CONSUMED },
    { WISE::ProjectProto::GlobalStatistics::SURFACE_FUEL_CONSUMED, WISE::ProjectProto::Project_Outputs_StatsOutput::SURFACE_FUEL_CONSUMED },
    { WISE::ProjectProto::GlobalStatistics::NUM_ACTIVE_VERTICES, WISE::ProjectProto::Project_Outputs_StatsOutput::NUM_ACTIVE_VERTICES },
    { WISE::ProjectProto::GlobalStatistics::NUM_VERTICES, WISE::ProjectProto::Project_Outputs_StatsOutput::NUM_VERTICES },
    { WISE::ProjectProto::GlobalStatistics::CUMULATIVE_VERTICES, WISE::ProjectProto::Project_Outputs_StatsOutput::CUMULATIVE_VERTICES },
    { WISE::ProjectProto::GlobalStatistics::CUMULATIVE_ACTIVE_VERTICES, WISE::ProjectProto::Project_Outputs_StatsOutput::CUMULATIVE_ACTIVE_VERTICES },
    { WISE::ProjectProto::GlobalStatistics::NUM_ACTIVE_FRONTS, WISE::ProjectProto::Project_Outputs_StatsOutput::NUM_ACTIVE_FRONTS },
    { WISE::ProjectProto::GlobalStatistics::NUM_FRONTS, WISE::ProjectProto::Project_Outputs_StatsOutput::NUM_FRONTS },
    { WISE::ProjectProto::GlobalStatistics::MEMORY_USED_START, WISE::ProjectProto::Project_Outputs_StatsOutput::MEMORY_USED_START },
    { WISE::ProjectProto::GlobalStatistics::MEMORY_USED_END, WISE::ProjectProto::Project_Outputs_StatsOutput::MEMORY_USED_END },
    { WISE::ProjectProto::GlobalStatistics::NUM_TIMESTEPS, WISE::ProjectProto::Project_Outputs_StatsOutput::NUM_TIMESTEPS },
    { WISE::ProjectProto::GlobalStatistics::NUM_DISPLAY_TIMESTEPS, WISE::ProjectProto::Project_Outputs_StatsOutput::NUM_DISPLAY_TIMESTEPS },
    { WISE::ProjectProto::GlobalStatistics::NUM_EVENT_TIMESTEPS, WISE::ProjectProto::Project_Outputs_StatsOutput::NUM_EVENT_TIMESTEPS },
    { WISE::ProjectProto::GlobalStatistics::NUM_CALC_TIMESTEPS, WISE::ProjectProto::Project_Outputs_StatsOutput::NUM_CALC_TIMESTEPS },
    { WISE::ProjectProto::GlobalStatistics::TICKS, WISE::ProjectProto::Project_Outputs_StatsOutput::TICKS },
    { WISE::ProjectProto::GlobalStatistics::PROCESSING_TIME, WISE::ProjectProto::Project_Outputs_StatsOutput::PROCESSING_TIME },
    { WISE::ProjectProto::GlobalStatistics::GROWTH_TIME, WISE::ProjectProto::Project_Outputs_StatsOutput::GROWTH_TIME }
	});


HRESULT Project::CWFGMProject::serialize(const std::string& filename, const std::uint32_t file_major_version,
	const std::function<void(google::protobuf::Message*)> updateCallback,
	const std::function<std::string(void)>& password_callback)
{
	HRESULT retval = ERROR_SEVERITY_ERROR;
	SerializeProtoOptions options;
	options.setVerboseOutput(false);
	//only compress arrays in JSON files
	if (boost::iends_with(filename, ".fgmj"))
	{
		options.setZipOutput(true);
		options.setVerboseFloat(true);
	}
	options.setFileVersion(file_major_version);

	auto data = new WISE::ProjectProto::PrometheusData();
	data->set_allocated_project(serialize(options));
	if (updateCallback)
		updateCallback(data);

	if (!m_preExecutionCommand_Linux.empty() ||
		!m_preExecutionCommand_Windows.empty()) {
		auto cmd = new WISE::ProjectProto::PrometheusData_Command();
		data->set_allocated_preexecution(cmd);
		cmd->set_version(1);
		cmd->set_linuxcommand(m_preExecutionCommand_Linux);
		cmd->set_windowscommand(m_preExecutionCommand_Windows);
	}
	if (!m_postExecutionCommand_Linux.empty() ||
		!m_postExecutionCommand_Windows.empty()) {
		auto cmd = new WISE::ProjectProto::PrometheusData_Command();
		data->set_allocated_postexecution(cmd);
		cmd->set_version(1);
		cmd->set_linuxcommand(m_postExecutionCommand_Linux);
		cmd->set_windowscommand(m_postExecutionCommand_Windows);
	}


	if (data->has_project())
	{
		if (boost::iends_with(filename, ".fgmj"))
		{
			std::string json;
			google::protobuf::util::JsonOptions options1;
			options1.add_whitespace = true;
			options1.always_print_primitive_fields = true;
			options1.preserve_proto_field_names = true;

			(void)google::protobuf::util::MessageToJsonString(*data, &json, options1);

			std::ofstream out(filename);
			if (out.is_open())
			{
				out << json;
				out.close();
				retval = S_OK;
			}
			else
				retval = ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_WARNING;
		}
		else
		{
			std::string password = password_callback();
			hss::protobuf::io::MinizipOutputStream zip(filename, password);
			if (!data->SerializeToZeroCopyStream(&zip))
				retval = ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_ERROR;
			else
				retval = S_OK;
			zip.Close();
		}
	}
	delete data;
	return retval;
}

HRESULT Project::CWFGMProject::deserialize(const std::string& filename, std::uint32_t &file_major_version, const std::function<std::string(void)>& password_callback, google::protobuf::Message** result,
	std::shared_ptr<validation::validation_object> valid, const std::string& name)
{
	HRESULT retval = ERROR_SEVERITY_ERROR;
	WISE::ProjectProto::PrometheusData* data = nullptr;
	file_major_version = 0;

	if (boost::iends_with(filename, ".fgmj"))
	{
		std::ifstream in(filename);
		if (in.is_open())
		{
			google::protobuf::util::JsonParseOptions options;
			options.ignore_unknown_fields = true;

			std::string json((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
			data = new WISE::ProjectProto::PrometheusData();
			try {
				if (google::protobuf::util::JsonStringToMessage(json, data, options).ok())
				{
					delete data;
					data = nullptr;
				}
			}
			catch (std::exception& e) {
				return ERROR_FILE_FORMAT_INVALID | ERROR_SEVERITY_WARNING;
			}
		}
		else
			retval = ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_WARNING;
	}
	else
	{
		std::ifstream in(filename, std::ios::binary);
		if (in.is_open())
		{
			char sig[4];
			in.read(sig, 4);
			//zip encoded
			if ((sig[0] == '\x50' && sig[1] == '\x4b' && sig[2] == '\x03' && sig[3] == '\x04') ||//zip
				(sig[0] == '\x1f' && sig[1] == '\x8b') ||//gzip
				(sig[0] == '\x78' && sig[1] == '\x9c'))//zlib
			{
				in.close();

				hss::protobuf::io::MinizipInputStream zip(filename, password_callback);
				if (zip.GetLastError() == -108)
					retval = ERROR_INVALID_PASSWORD | ERROR_SEVERITY_WARNING;
				else if (zip.GetLastError() == 0)
				{
					data = new WISE::ProjectProto::PrometheusData();
					if (!data->ParseFromZeroCopyStream(&zip))
					{
						delete data;
						data = nullptr;
					}
				}
			}
			else
			{
				//reset the stream
				in.clear();
				in.seekg(0, std::ios::beg);
				//try to parse the data
				data = new WISE::ProjectProto::PrometheusData();
				if (!data->ParseFromIstream(&in))
				{
					delete data;
					data = nullptr;
				}
				in.close();
			}
		}
		else
			retval = ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_WARNING;
	}

	if (data)
	{
		fs::path p(filename);
		fs::current_path(p.parent_path());

		//write the input files to disk
		if (data->inputfiles_size())
		{
			for (auto& file : data->inputfiles())
			{
				fs::path path = fs::proximate(file.filename());
				//the file hasn't already been exported
				if (!fs::exists(path))
				{
					fs::create_directories(path.parent_path());

					std::ofstream writer(path.string(), std::ios::binary);
					writer.unsetf(std::ios::skipws);
					std::copy(file.data().begin(), file.data().end(), std::ostream_iterator<char>(writer));
					writer.close();

					if (path.extension() != ".kmz") {
						//if the file was a zip, extract it
						if (file.data()[0] == '\x50' && file.data()[1] == '\x4b' && file.data()[2] == '\x03' && file.data()[3] == '\x04')
						{
							auto pathToArchive = fs::absolute(path);
							zip::extract(pathToArchive.string(), pathToArchive.parent_path().string(), false);
						}
					}
				}
			}
			//write out the updated fgm on complete
			mModified = true;
		}

		if (data->has_preexecution()) {
			if (data->preexecution().version() != 1) {
				if (valid)
					/// <summary>
					/// The object version is not supported. The Prometheus project is not supported by this version of Prometheus.
					/// </summary>
					/// <type>user</type>
					valid->add_child_validation("WISE.ProjectProto.Project.Command", "preExecution", validation::error_level::WARNING, validation::id::version_mismatch, std::to_string(data->preexecution().version()));
				weak_assert(0);
				throw ISerializeProto::DeserializeError("CWFGMProject.Project.Command: Version invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
			}
			m_preExecutionCommand_Linux = data->preexecution().linuxcommand();
			m_preExecutionCommand_Windows = data->preexecution().windowscommand();
		}
		if (data->has_postexecution()) {
			if (data->postexecution().version() != 1) {
				if (valid)
					/// <summary>
					/// The object version is not supported. The Prometheus project is not supported by this version of Prometheus.
					/// </summary>
					/// <type>user</type>
					valid->add_child_validation("WISE.ProjectProto.Project.Command", "postExecution", validation::error_level::WARNING, validation::id::version_mismatch, std::to_string(data->postexecution().version()));
				weak_assert(0);
				throw ISerializeProto::DeserializeError("CWFGMProject.Project.Command: Version invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
			}
			m_postExecutionCommand_Linux = data->postexecution().linuxcommand();
			m_postExecutionCommand_Windows = data->postexecution().windowscommand();
		}

		if (data->has_project())
		{
			try
			{
				deserialize(data->project(), valid, name);
				file_major_version = data->project().version();
				retval = S_OK;
			}
			catch (std::exception &e)
			{
				auto ee = dynamic_cast_assert<ISerializeProto::DeserializeError*>(&e);
				if ((ee) && (ee->hr != 0))
						retval = ee->hr;
				else
					retval = ERROR_FILE_FORMAT_INVALID | ERROR_SEVERITY_WARNING;
			}
		}
		if (result)
			*result = data;
		else
			delete data;
	}

	return retval;
}


std::string Project::CWFGMProject::serializeOutputs(const SerializeProtoOptions& options, bool binary)
{
	std::string retval;

	const gsl::not_null<WISE::ProjectProto::Project_Outputs*> outputs = m_outputs.serialize(options);

	if (binary)
		outputs->SerializeToString(&retval);
	else
	{
		google::protobuf::util::JsonOptions options1;
		options1.add_whitespace = options.useVerboseOutput();
		options1.always_print_primitive_fields = true;
		options1.preserve_proto_field_names = true;

		if (google::protobuf::util::MessageToJsonString(*outputs, &retval, options1).ok())
			std::cerr << "Failed to save output data";
	}

	delete outputs.get();

	return std::move(retval);
}


std::int32_t Project::CWFGMProject::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return options.fileVersion();
}


WISE::ProjectProto::Project* Project::CWFGMProject::serialize(const SerializeProtoOptions& options) {
	auto project = new WISE::ProjectProto::Project();
	project->set_version(serialVersionUid(options));

	project->set_comments(m_comments);
	project->set_loadbalancing((WISE::ProjectProto::Project::LoadBalancingType)m_loadBalancing);
	auto fuels = m_fuelMap->serialize(options);
	project->set_allocated_fuels(fuels);
	project->set_allocated_grid(m_grid->serialize(options));
	if (m_csProjectionName.length() > 0)
	{
		if (project->grid().has_projection()
			&& ((!project->grid().projection().has_filename())
				|| (project->grid().projection().filename().value().length() == 0)))
			project->mutable_grid()->mutable_projection()->set_allocated_filename(createProtobufObject(m_csProjectionName));
	}
	if (m_csElevationName.length() > 0)
	{
		if (project->grid().has_elevation() && (!project->grid().elevation().has_filename() || project->grid().elevation().filename().value().length() == 0))
			project->mutable_grid()->mutable_elevation()->set_allocated_filename(createProtobufObject(m_csElevationName));
	}

	if (options.fileVersion() == 1)
		project->set_allocated_fuelcollection(dynamic_cast_assert<WISE::ProjectProto::FuelCollection *>(m_fuelCollection.serialize(options)));
	else {
		FuelCollectionSerializationData fd;
		fd.fuelMap = fuels;
		m_fuelCollection.serialize(options, &fd);
	}
	project->set_allocated_ignitions(dynamic_cast_assert<WISE::ProjectProto::FireCollection*>(m_fireCollection.serialize(options)));
	project->set_allocated_stations(m_weatherCollection.serialize(options));
	project->set_allocated_grids(dynamic_cast_assert<WISE::ProjectProto::GridCollection*>(m_gridFilterCollection.serialize(options)));
	project->set_allocated_vectors(dynamic_cast_assert<WISE::ProjectProto::VectorCollection*>(m_vectorCollection.serialize(options)));
	project->set_allocated_assets(dynamic_cast_assert<WISE::ProjectProto::AssetCollection*>(m_assetCollection.serialize(options)));
	project->set_allocated_targets(dynamic_cast_assert<WISE::ProjectProto::TargetCollection*>(m_targetCollection.serialize(options)));
	project->set_allocated_scenarios(m_scenarioCollection.serialize(options));
	if ((options.fileVersion() == 1) || (m_viewCollection.GetViewCount()))
		project->set_allocated_views(dynamic_cast_assert<WISE::ProjectProto::ViewCollection*>(m_viewCollection.serialize(options)));

	project->set_elevationname(m_csElevationName);
	project->set_gridname(m_csGridName);
	project->set_projectionname(m_csProjectionName);

	project->set_allocated_outputs(m_outputs.serialize(options));

	auto now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	tm utc_tm;
#if _MSC_VER
	gmtime_s(&utc_tm, &tt);
#else
	gmtime_r(&tt, &utc_tm);
#endif
	WTime startTime(utc_tm.tm_year + 1900, 6, 15, 12, 0, 0, m_timeManager);
	project->set_allocated_projectstarttime(HSS_Time::Serialization::TimeSerializer::serializeTime(startTime, options.fileVersion()));
	project->set_allocated_timezonesettings(HSS_Time::Serialization::TimeSerializer::serializeTimeZone(m_timeManager->m_worldLocation, options.fileVersion()));

	return project;
}


auto Project::CWFGMProject::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> CWFGMProject*
{
	auto project = dynamic_cast<const WISE::ProjectProto::Project*>(&proto);
	CWFGMProject* retval = this;

	if (!project)
	{
		if (valid)
			/// <summary>
			/// The object passed as a Prometheus project is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.Project", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("CWFGMProject: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if ((project->version() != 1) && (project->version() != 2))
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The Prometheus project is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.Project", name, validation::error_level::WARNING, validation::id::version_mismatch, std::to_string(project->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("CWFGMProject: Version invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	/// <summary>
	/// Child validations for a Prometheus project.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.Project", name);
	auto myValid = vt.lock();

	m_loadBalancing = (Project::LoadBalancingType)project->loadbalancing();

	m_fuelCollection.Delete();
	m_fuelMap = nullptr;
	try {
		m_fuelMap = new CCWFGM_FuelMap();
	} catch (std::exception &e) {
		/// <summary>
		/// The COM object could not be instantiated.
		/// </summary>
		/// <type>internal</type>
		if (valid)
			valid->add_child_validation("WISE.ProjectProto.Project", "fuelCollection", validation::error_level::SEVERE, validation::id::cannot_allocate, "CLSID_CWFGM_FuelMap");
		return nullptr;
	}
	boost::intrusive_ptr<ICWFGM_GridEngine> oldgrid = m_gridEngine;
	m_grid = nullptr;
	m_gridEngine = nullptr;
	try {
		m_grid = new CCWFGM_Grid();
	}
	catch (std::exception& e) {
		/// <summary>
		/// The COM object's interface could not be retrieved.
		/// </summary>
		/// <type>internal</type>
		if (valid)
			valid->add_child_validation("WISE.ProjectProto.Project", "grid", validation::error_level::SEVERE, validation::id::cannot_obtain_interface, "ICWFGM_GridEngine");
		return nullptr;
	}
	m_gridEngine = m_grid;

	m_comments = project->comments();

	if (((project->version() > 1) && (!project->has_fuels())) ||
		((!project->has_fuels()) && (!project->has_fuelcollection()))) {
		/// <summary>
		/// The file doesn't contain any fuel information at all.
		/// </summary>
		/// <type>user</type>
		if (valid)
			valid->add_child_validation("WISE.ProjectProto.Project", "fuels", validation::error_level::SEVERE, validation::id::missing_fuels, "");
		return nullptr;
	}

	if (project->has_fuels())
		if (!m_fuelMap->deserialize(project->fuels(), myValid, "fuels"))
		{
			/// <summary>
			/// The COM object's interface could not be retrieved.
			/// </summary>
			/// <type>internal</type>
			if (valid)
				valid->add_child_validation("WISE.ProjectProto.Project", "fuels", validation::error_level::SEVERE, validation::id::cannot_obtain_interface, "IPersistProtobufStream");
			weak_assert(0);
			retval = nullptr;
		}

	m_fuelCollection.m_fuelMap = m_fuelMap;
	m_gridEngine->put_LayerManager(m_layerManager.get());
	m_grid->put_FuelMap(m_fuelMap.get());

	if (project->version() == 1) {
		if (project->has_fuelcollection())
			if (!m_fuelCollection.deserialize(project->fuelcollection(), myValid, "fuelCollection"))
			{
				weak_assert(0);
				return nullptr;
			}
	}
	else
	{
		if (!m_fuelCollection.deserialize(project->fuels(), myValid, "fuels")) {
			weak_assert(0);
			return nullptr;
		}
	}

	if (project->has_grid())
	{
		if (!m_grid->deserialize(project->grid(), myValid, "grid"))
		{
			weak_assert(0);
			retval = nullptr;
		}
		if (project->grid().has_projection() && project->grid().projection().has_filename())
			m_csProjectionName = project->grid().projection().filename().value();
		if (project->grid().has_elevation() && project->grid().elevation().has_filename())
			m_csElevationName = project->grid().elevation().filename().value();
	}
	else {
		/// <summary>
		/// The file doesn't contain any fuel information at all.
		/// </summary>
		/// <type>user</type>
		if (valid)
			valid->add_child_validation("WISE.ProjectProto.Project", "grid", validation::error_level::SEVERE, validation::id::missing_fuel_grid, "");
		retval = nullptr;
	}

	ICWFGM_CommonData* data;
	if (FAILED(m_grid->GetCommonData(nullptr, &data))) {
		weak_assert(0);
		return nullptr;
	}
	m_timeManager = data->m_timeManager;

	auto timeData = HSS_Time::Serialization::TimeSerializer::deserializeTime(project->projectstarttime(), valid, "CWFGMProject.ProjectStartTime");
	if (project->has_timezonesettings())
	{
		HSS_Time::Serialization::TimeSerializer::deserializeTimeZone(project->timezonesettings(), (WorldLocation&)m_timeManager->m_worldLocation, valid, "CWFGMProject.TimeZone");
	}
	// legacy files
	else
	{
		WorldLocation& worldLocation = (WorldLocation&)m_timeManager->m_worldLocation;
		if (timeData.timezone > 0)
			worldLocation.SetTimeZoneOffset(timeData.timezone);
		else
		{
			worldLocation.m_timezone(WTimeSpan(timeData.offset));
			if (timeData.dst)
			{
				worldLocation.m_endDST(WTimeSpan(366, 0, 0, 0));
				worldLocation.m_amtDST(WTimeSpan(timeData.dst));
			}
		}
	}

	m_gridFilterCollection.AssignNewGrid(m_fuelMap.get(), m_gridEngine.get());
	m_vectorCollection.AssignNewGrid(m_gridEngine.get());
	m_assetCollection.AssignNewGrid(m_gridEngine.get());
	m_targetCollection.AssignNewGrid(m_gridEngine.get());
	m_fireCollection.AssignNewGrid(m_gridEngine.get());
	m_weatherCollection.AssignNewGrid(m_gridEngine.get());
	m_scenarioCollection.AssignNewGrid(oldgrid.get(), m_gridEngine.get());

	PolymorphicAttribute attr;
	double temp;

	if (project->has_vectors())
		if (!m_vectorCollection.deserialize(project->vectors(), myValid, "vectors"))
			retval = nullptr;

	if (project->has_assets())
		if (!m_assetCollection.deserialize(project->assets(), myValid, "assets"))
			retval = nullptr;

	if (project->has_targets())
		if (!m_targetCollection.deserialize(project->targets(), myValid, "targets"))
			retval = nullptr;

	if (project->has_grids())
		if (!m_gridFilterCollection.deserialize(project->grids(), myValid, "grids"))
			retval = nullptr;

	if (project->has_stations()) {
		if (!m_weatherCollection.deserialize(project->stations(), myValid, "stations"))
			retval = nullptr;
	}
	else {
		/// <summary>
		/// The file doesn't contain any weather information at all.
		/// </summary>
		/// <type>user</type>
		if (valid)
			valid->add_child_validation("WISE.ProjectProto.Project", "stations", validation::error_level::SEVERE, validation::id::missing_weather, "");
		else
			retval = nullptr;
	}

	if (project->has_ignitions()) {
		if (!m_fireCollection.deserialize(project->ignitions(), myValid, "ignitions"))
			retval = nullptr;
	}
	else {
		/// <summary>
		/// The file doesn't contain any ignition information at all.
		/// </summary>
		/// <type>user</type>
		if (valid)
			valid->add_child_validation("WISE.ProjectProto.Project", "ignitions", validation::error_level::SEVERE, validation::id::missing_ignitions, "");
		else
			retval = nullptr;
	}

	if (project->has_scenarios()) {
		if (!m_scenarioCollection.deserialize(project->scenarios(), myValid, "scenarios"))
			retval = nullptr;
	}
	else {
		/// <summary>
		/// The file doesn't contain any scenario information at all.
		/// </summary>
		/// <type>user</type>
		if (valid)
			valid->add_child_validation("WISE.ProjectProto.Project", "scenarios", validation::error_level::SEVERE, validation::id::missing_scenarios, "");
		else
			retval = nullptr;
	}

	if (project->has_views())
		m_viewCollection.deserialize(project->views(), myValid, "views");
	RecordUsedFuels();

	InitializeCoorConverter();

	((WorldLocation *)&m_timeManager->m_worldLocation)->UpdateTimeZone();

	m_csElevationName = project->elevationname();
	m_csGridName = project->gridname();
	m_csProjectionName = project->projectionname();

	if (project->has_outputs())
	{
		FGMSerializationData data;
		data.m_timeManager = m_timeManager;
		m_outputs.deserialize(project->outputs(), myValid, "outputs", &data);
	}

	if (project->has_timestepsettings())
		m_timestepSettings.deserialize(project->timestepsettings(), myValid, "timestepSettings");

	CollectLoadWarnings();

	return retval;
}


HRESULT Project::CWFGMProject::PrintReportJson(const TCHAR* szPath, const Project::SummaryCollection* collection, const PrintReportOptions& PrntRptOptns,
	const TCHAR* VersionNumber, const TCHAR* ReleaseDate, const TCHAR* FGMName, const std::vector<Scenario*>& scenarios, const ::UnitConversion* uc, bool addWeather)
{
	std::vector<WISE::ProjectProto::ScenarioReport*> reports;
	for (auto& currScenario : scenarios)
	{
		auto summary = new WISE::ProjectProto::ScenarioReport();
		//the settings that were used to generate the report
		summary->set_output_application_settings(PrntRptOptns.getApplicationSettings());
		summary->set_output_scenario_settings(PrntRptOptns.getScenarioSettings());
		summary->set_output_landscape_properties(PrntRptOptns.getLandscapeProperties());
		summary->set_output_input_files(PrntRptOptns.getInputFiles());
		summary->set_output_fuel_patches(PrntRptOptns.getFBPFuelPatches());
		summary->set_output_weather_patches_grids(PrntRptOptns.getWxPatchesAndWxGrids());
		summary->set_output_geo_data(PrntRptOptns.getGeoData());
		summary->set_output_ignitions(PrntRptOptns.getIgnitions());
		summary->set_output_weather_streams(PrntRptOptns.getWxStreams());
		summary->set_output_fuel_types(PrntRptOptns.getFBPFuelTypes());
		summary->set_output_scenario_comments(PrntRptOptns.getScenarioComments());

		//the Prometheus details
		summary->set_prometheus_version(std::string(VersionNumber));
		summary->set_prometheus_date(std::string(ReleaseDate));
		//the name of the file being run
		if (FGMName)
			summary->set_allocated_accel_timestep(createProtobufObject(std::string(FGMName)));
		//the current time
		WTime now = WTime::Now(m_timeManager, 0);
		summary->set_report_date(now.ToString(WTIME_FORMAT_STRING_ISO8601));

		WTime tempStartTime(m_timeManager);
		WTime tempEndTime(m_timeManager);
		WTime scenarioStartTime(m_timeManager);
		WTime scenarioEndTime(m_timeManager);
		scenarioStartTime = currScenario->Simulation_StartTime();
		scenarioEndTime = currScenario->Simulation_EndTime();

		//scenario settings
		if (PrntRptOptns.getScenarioSettings() == 1)
		{
			summary->set_allocated_scenario_name(createProtobufObject(currScenario->m_name));
			summary->set_allocated_scenario_start(createProtobufObject(scenarioStartTime.ToString(WTIME_FORMAT_STRING_ISO8601)));
			summary->set_allocated_scenario_end(createProtobufObject(scenarioEndTime.ToString(WTIME_FORMAT_STRING_ISO8601)));

			for (ULONG i = 0; i < currScenario->GetFireCount(); i++)
				summary->add_ignition_ref(currScenario->FireAtIndex(i)->m_name);

			//fuel breaks
			if (PrntRptOptns.getGeoData() == 1)
			{
				for (ULONG i = 0; i < currScenario->GetVectorCount(); i++)
				{
					auto fb = currScenario->VectorAtIndex(i);
					auto fuelBreak = summary->add_fuel_breaks();
					fuelBreak->set_name(std::string(fb->Name()));
					fuelBreak->set_width(fb->GetPolyLineWidth());
					if (fb->Comments().length())
						fuelBreak->set_allocated_comment(createProtobufObject(std::string(fb->Comments())));
				}
			}

			//assets
			if (PrntRptOptns.getAssetData() == 1)
			{
				for (ULONG i = 0; i < currScenario->GetAssetCount(); i++)
				{
					auto fb = currScenario->AssetAtIndex(i);
					auto asset = summary->add_assets();
					asset->set_name(std::string(fb->Name()));
					double b = fb->GetPolyLineWidth();
					if (b != 0.0)
						asset->set_allocated_boundary(createProtobufObject(fb->GetPolyLineWidth()));
					if (fb->Comments().length())
						asset->set_allocated_comment(createProtobufObject(std::string(fb->Comments())));
				}
			}

			//grid filters
			for (ULONG i = 0; i < currScenario->GetFilterCount(); i++)
				summary->add_grid_filter_ref(currScenario->FilterAtIndex(i)->m_name);

			//weather streams, skip if they're going to be output later
			if (PrntRptOptns.getWxStreams() == 0)
			{
				for (ULONG i = 0; i < currScenario->GetStreamCount(); i++)
				{
					auto ws = currScenario->StreamAtIndex(i);
					auto stream = summary->add_weather_streams();
					stream->set_is_primary(ws == currScenario->GetPrimaryStream());
					if (ws->m_weatherStation()->m_name.length() == 0)
						stream->set_station_name("(No Name)");
					else
						stream->set_station_name(ws->m_weatherStation()->m_name);
					if (ws->m_name.length() == 0)
						stream->set_stream_name("(No Name)");
					else
						stream->set_stream_name(ws->m_name);
					ws->GetStartTime(tempStartTime);
					stream->set_start_time(tempStartTime.ToString(WTIME_FORMAT_STRING_ISO8601));
					ws->GetEndTime(tempEndTime);
					stream->set_end_time(tempEndTime.ToString(WTIME_FORMAT_STRING_ISO8601));
				}
			}

			//comments
			if (PrntRptOptns.getScenarioComments() == 1)
			{
				if (currScenario->m_comments.length() == 0)
					summary->set_allocated_scenario_comment(createProtobufObject(""s));
				else
					summary->set_allocated_scenario_comment(createProtobufObject(currScenario->m_comments));
			}
		}

		if (PrntRptOptns.getApplicationSettings() == 1)
		{
			USHORT days;
			currScenario->GetBurnConditionDays(&days);
			WTime start(scenarioStartTime);
			WTime end(scenarioEndTime);
			start.PurgeToDay(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
			end.PurgeToDay(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
			start -= WTimeSpan(1, 0, 0, 0);
			end += WTimeSpan(1, 13, 0, 0);
			//burning conditions
			for (USHORT i = 0; i < days; i++)
			{
				CBurnPeriodOption condition(m_timeManager);
				currScenario->GetIndexBurnCondition(i, &condition);
				tempStartTime = WTime(condition.m_time, scenarioStartTime.GetTimeManager());

				//only output burning conditions that overlap the scenario time
				if (tempStartTime >= start && tempStartTime <= end)
				{
					auto burning = summary->add_burn_conditions();
					burning->set_condition_date(tempStartTime.ToString(WTIME_FORMAT_STRING_ISO8601));
					if (condition.eff_Start)
						burning->set_allocated_start_hour(createProtobufObject(condition.m_Start.ToString(WTIME_FORMAT_STRING_ISO8601)));
					if (condition.eff_End)
						burning->set_allocated_stop_hour(createProtobufObject(condition.m_End.ToString(WTIME_FORMAT_STRING_ISO8601)));
					if (condition.eff_MinISI)
						burning->set_allocated_min_isi(createProtobufObject(condition.m_MinISI));
					if (condition.eff_MinFWI)
						burning->set_allocated_min_fwi(createProtobufObject(condition.m_MinFWI));
					if (condition.eff_MaxWS)
						burning->set_allocated_max_ws(createProtobufObject(condition.m_MaxWS));
					if (condition.eff_MinRH)
						burning->set_allocated_min_rh(createProtobufObject(condition.m_MinRH));
				}
			}

			BasicScenarioOptions basicOptions = currScenario->GetBasicOptions();
			if (!basicOptions.m_bWeatherSpatialInterpolate)
				summary->set_weather_spatial_interp(WISE::ProjectProto::ScenarioReport::None);
			else if (basicOptions.m_bHistoryInterpolation)
				summary->set_weather_spatial_interp(WISE::ProjectProto::ScenarioReport::RecursiveFromSpatiallyInterpWeather);
			else if (basicOptions.m_bCalcSpatialFWI)
				summary->set_weather_spatial_interp(WISE::ProjectProto::ScenarioReport::FromSpatiallyInterpWeather);
			else
				summary->set_weather_spatial_interp(WISE::ProjectProto::ScenarioReport::SpatiallyInterp);

			//fire behaviour
			FBPOptions fbpopt = currScenario->GetFBPOptions();
			if (fbpopt.m_specifiedFMCActive)
			{
				summary->set_allocated_fmc_override_enabled(createProtobufObject(true));
				summary->set_allocated_fmc_override(createProtobufObject(fbpopt.m_specifiedFMC));
			}
			else
				summary->set_allocated_fmc_override_enabled(createProtobufObject(false));

			summary->set_allocated_terrain_effect(createProtobufObject(fbpopt.m_topography != 0));

			WTimeSpan g_start, g_end;
			bool g_effective;

			//greenup
			currScenario->GetGreenupTimes(&g_start, &g_end, &g_effective);
			if (g_effective)
			{
				tempStartTime = currScenario->Simulation_StartTime();
				tempEndTime = tempStartTime;
				tempStartTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				tempEndTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				tempStartTime += g_start;
				tempEndTime += g_end;
				auto greenup = summary->add_greenup();
				greenup->set_allocated_enabled(createProtobufObject(true));
				greenup->set_start_date(tempStartTime.ToString(WTIME_FORMAT_STRING_ISO8601));
				greenup->set_end_date(tempEndTime.ToString(WTIME_FORMAT_STRING_ISO8601));
			}

			//grass phenology
			currScenario->GetStandingGrassTimes(&g_start, &g_end, &g_effective);
			if (g_effective)
			{
				tempStartTime = currScenario->Simulation_StartTime();
				tempEndTime = tempStartTime;
				tempStartTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				tempEndTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				tempStartTime += g_start;
				tempEndTime += g_end;
				auto phonology = summary->add_grass_phenology();
				phonology->set_allocated_enabled(createProtobufObject(true));
				phonology->set_start_date(tempStartTime.ToString(WTIME_FORMAT_STRING_ISO8601));
				phonology->set_end_date(tempEndTime.ToString(WTIME_FORMAT_STRING_ISO8601));
			}

			//grass curing
			CuringOptions cOptions;
			currScenario->GetCuringDetails(cOptions);
			if (cOptions.curing.GetCount())
			{
				for (auto node : cOptions.curing)
				{
					WTime startTime(currScenario->Simulation_StartTime());
					startTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
					startTime += node->m_date;

					WTime endTime(currScenario->Simulation_StartTime());
					endTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
					if (node->LN_Succ()->LN_Succ())
						endTime += node->LN_Succ()->m_date;
					else
						endTime += WTimeSpan(365, 0, 0, 0);
					endTime -= WTimeSpan(1, 0, 0, 0);

					auto curing = summary->add_grass_curing();
					curing->set_start_date(startTime.ToString(WTIME_FORMAT_STRING_ISO8601));
					curing->set_end_date(endTime.ToString(WTIME_FORMAT_STRING_ISO8601));
					curing->set_allocated_value(createProtobufObject(node->m_curing));
				}
			}

			summary->set_allocated_breaching_enabled(createProtobufObject(basicOptions.m_bBreaching != 0));
			summary->set_allocated_spotting_enabled(createProtobufObject(basicOptions.m_bSpotting != 0));
			summary->set_allocated_percentile_ros_enabled(createProtobufObject(fbpopt.m_growthPercentileEnabled != 0));
			if (fbpopt.m_growthPercentileEnabled)
				summary->set_allocated_percentile_ros(createProtobufObject(fbpopt.m_growthPercentile));

			//propagation
			summary->set_allocated_display_interval(createProtobufObject(basicOptions.m_displayInterval.ToString(WTIME_FORMAT_STRING_ISO8601)));
			summary->set_allocated_accel_timestep(createProtobufObject(basicOptions.m_temporalThreshold_Accel.ToString(WTIME_FORMAT_STRING_ISO8601)));
			if (basicOptions.m_bDynamicSpatialThreshold)
				summary->set_allocated_dynamic_threshold(createProtobufObject(true));
			else
			{
				summary->set_allocated_distance_resolution(createProtobufObject(basicOptions.m_spatialThreshold));
				summary->set_allocated_perimeter_resolution(createProtobufObject(basicOptions.m_perimeterResolution));
			}
			summary->set_allocated_stop_at_boundary(createProtobufObject(basicOptions.m_bBoundaryStop != 0));
			summary->set_allocated_purge_non_displayable(createProtobufObject(basicOptions.m_bPurgeNonDisplayable != 0));

			PolymorphicAttribute delta;
			bool dx, dy;
			double deltax, deltay;
			bool deltab;
			dx = SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_DX, &delta));
			if (dx)
				VariantToDouble_(delta, &deltax);
			else
				deltax = 0.0;
			dy = SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_DY, &delta));
			if (dy)
				VariantToDouble_(delta, &deltay);
			else
				deltay = 0.0;
			if (deltax || deltay)
			{
				summary->set_allocated_dx(createProtobufObject(deltax));
				summary->set_allocated_dy(createProtobufObject(deltay));
			}
			if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_DT, &delta)))
			{
			    HSS_Time::WTimeSpan tdelta;
				VariantToTimeSpan_(delta, &tdelta);
				if (tdelta.GetTotalSeconds())
				{
     				summary->set_allocated_dt(createProtobufObject(tdelta.ToString(WTIME_FORMAT_TIME)));
     			}
			}
			if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_OWD, &delta))) {
				VariantToDouble_(delta, &deltax);
				if (deltax != -1.0)
					summary->set_allocated_owd(createProtobufObject(deltax));
			}
			if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_DWD, &delta)))
			{
				VariantToDouble_(delta, &deltax);
				if (deltax)
				    summary->set_allocated_dwd(createProtobufObject(deltax));
			}
			if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_FALSE_ORIGIN, &delta)))
			{
				VariantToBoolean_(delta, &deltab);
				summary->set_allocated_use_false_origin(createProtobufObject(deltab));
			}
			if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_FALSE_SCALING, &delta)))
			{
				VariantToBoolean_(delta, &deltab);
				summary->set_allocated_use_false_scaling(createProtobufObject(deltab));
			}
		}

		//Landscape properties
		if (PrntRptOptns.getLandscapeProperties() == 1)
		{
			USHORT xdim, ydim;
			GetDimensions(&xdim, &ydim);
			double res;
			GetResolution(&res);

			//grid size
			summary->set_allocated_grid_units(createProtobufObject(uc->UnitName(uc->SmallDistanceDisplay(), true)));
			summary->set_allocated_grid_cell_size(createProtobufObject(res));
			summary->set_allocated_grid_columns(createProtobufObject((int)xdim));
			summary->set_allocated_grid_rows(createProtobufObject((int)ydim));

			//grid location
			double latitude, longitude;
			GetLocation(&latitude, &longitude);
			summary->set_allocated_grid_ll_x(createProtobufObject(RADIAN_TO_DEGREE(longitude)));
			summary->set_allocated_grid_ll_y(createProtobufObject(RADIAN_TO_DEGREE(latitude)));

			//elevation
			double _minelev, _maxelev;
			GetMinMaxElev(&_minelev, &_maxelev);
			summary->set_allocated_grid_min_elev(createProtobufObject(_minelev));
			summary->set_allocated_grid_max_elev(createProtobufObject(_maxelev));
			summary->set_allocated_grid_mean_elev(createProtobufObject(GetMeanElevation()));
			summary->set_allocated_grid_median_elev(createProtobufObject(GetMedianElevation()));

			//timezone
			std::string timezone;
			if (DaylightSavings())
			{
				for (int i = 0; m_timeManager->m_worldLocation.m_dst_timezones[i].m_code != NULL; i++)
				{
					if (m_timeManager->m_worldLocation.m_dst_timezones[i].m_timezone == m_timeManager->m_worldLocation.m_timezone())
					{
						timezone += std::string(m_timeManager->m_worldLocation.m_dst_timezones[i].m_code) + " " + std::string(m_timeManager->m_worldLocation.m_dst_timezones[i].m_name) + " ";
						break;
					}
				}
				WTimeSpan ts = m_timeManager->m_worldLocation.m_timezone() + m_timeManager->m_worldLocation.m_amtDST();
				timezone += ts.ToString(WTIME_FORMAT_TIME);
			}
			else
			{
				for (int i = 0; m_timeManager->m_worldLocation.m_std_timezones[i].m_code != NULL; i++)
				{
					if (m_timeManager->m_worldLocation.m_std_timezones[i].m_timezone == m_timeManager->m_worldLocation.m_timezone())
					{
						timezone += std::string(m_timeManager->m_worldLocation.m_std_timezones[i].m_code) + " " + std::string(m_timeManager->m_worldLocation.m_std_timezones[i].m_name) + " ";
						break;
					}
				}
				timezone += m_timeManager->m_worldLocation.m_timezone().ToString(WTIME_FORMAT_TIME);
			}
			summary->set_allocated_time_zone(createProtobufObject(timezone));

			if (m_comments.length())
				summary->set_allocated_project_comment(createProtobufObject(m_comments));
			else
				summary->set_allocated_project_comment(createProtobufObject(""s));
		}

		//Input files
		if (PrntRptOptns.getInputFiles() == 1)
		{
			summary->set_allocated_projection_file(createProtobufObject(m_csProjectionName));
			summary->set_allocated_fuel_grid_file(createProtobufObject(m_csGridName));
			if (m_fuelCollection.m_csLUTFileName.has_value() && m_fuelCollection.m_csLUTFileName.value().length())
				summary->set_allocated_lookup_table_file(createProtobufObject(m_fuelCollection.m_csLUTFileName.value()));
			if (m_csElevationName.length() != 0)
				summary->set_allocated_elevation_grid_file(createProtobufObject(m_csElevationName));
			else
				summary->set_allocated_elevation_grid_file(createProtobufObject("Not Imported!"s));

			for (ULONG i = 0; i < currScenario->GetFilterCount(); i++)
			{
				auto pFilter = dynamic_cast<AttributeGridFilter*>(currScenario->FilterAtIndex(i));
				if (pFilter)
				{
					summary->add_attribute_filters(pFilter->DisplayName());
				}
			}
		}

		//FBP fuel type grid
		if (PrntRptOptns.getFBPFuelPatches() == 1)
		{
			for (ULONG i = 0; i < currScenario->GetFilterCount(); i++)
			{
				auto pFilter = dynamic_cast<ReplaceGridFilter*>(currScenario->FilterAtIndex(i));
				if (pFilter)
				{
					auto patch = summary->add_fuel_patches();

					if (pFilter->m_name.length() == 0)
						patch->set_name("No Name"s);
					else
						patch->set_name(pFilter->m_name);

					ICWFGM_Fuel* from_fuel, * to_fuel;
					UCHAR from_index;
					if (SUCCEEDED(pFilter->GetRelationship(&from_fuel, &from_index, &to_fuel)))
					{
						std::string from_name, to_name;
						if (from_fuel == (ICWFGM_Fuel*)~0)
							from_name = _T("All Combustible Fuels");
						else if (from_fuel == (ICWFGM_Fuel*)(-2))
							from_name = _T("NODATA");
						else if (from_fuel)
						{
							HRESULT hr = from_fuel->get_Name(&from_name);
							if (FAILED(hr)) {
								delete summary;
								throw std::logic_error("Failed");
							}
						}
						else if (from_index != (UCHAR)-1)
						{
							long file_index = -1, export_index;
							fuelMap()->FuelAtIndex(from_index, &file_index, &export_index, &from_fuel);
							from_name = strprintf(_T("Index %d:"), file_index);
						}
						else
							from_name = _T("All Fuels");

						if (to_fuel)
						{
							CCWFGM_FuelMap* fuelMap = this->fuelMap();
							Fuel* f = Fuel::FromCOM(to_fuel);
							to_name = f->DisplayName(fuelMap);
						}
						else
							to_name = _T("No Fuel");

						patch->set_from_fuel(from_name);
						patch->set_to_fuel(to_name);
					}

					if (pFilter->m_comments.length())
						patch->set_allocated_comment(createProtobufObject(pFilter->m_comments));
				}
				else
				{
					auto xFilter = dynamic_cast<PolyReplaceGridFilter*>(currScenario->FilterAtIndex(i));
					if (xFilter)
					{
						auto patch = summary->add_fuel_patches();

						if (xFilter->m_name.length() == 0)
							patch->set_name("No Name"s);
						else
							patch->set_name(xFilter->m_name);

						ICWFGM_Fuel* from_fuel, * to_fuel;
						UCHAR from_index;
						if (SUCCEEDED(xFilter->GetRelationship(&from_fuel, &from_index, &to_fuel)))
						{
							std::string from_name, to_name;
							if (from_fuel == (ICWFGM_Fuel*)~0)
								from_name = _T("All Combustible Fuels");
							else if (from_fuel == (ICWFGM_Fuel*)(-2))
								from_name = _T("NODATA");
							else if (from_fuel)
							{
								HRESULT hr = from_fuel->get_Name(&from_name);
								if (FAILED(hr)) {
									delete summary;
									throw std::logic_error("Failed");
								}
							}
							else if (from_index != (UCHAR)-1)
							{
								long file_index = -1, export_index;
								fuelMap()->FuelAtIndex(from_index, &file_index, &export_index, &from_fuel);
								from_name = strprintf(_T("Index %d:"), file_index);
							}
							else
								from_name = _T("All Fuels");

							if (to_fuel)
							{
								CCWFGM_FuelMap* fuelMap = this->fuelMap();
								Fuel* f = Fuel::FromCOM(to_fuel);
								to_name = f->DisplayName(fuelMap);
							}
							else
								to_name = _T("No Fuel");

							patch->set_from_fuel(from_name);
							patch->set_to_fuel(to_name);
						}

						double area = fabs(xFilter->Area());
						std::string areaUnit = ::UnitConversion::UnitName(uc->AreaDisplay(), true);
						uc->ConvertUnit(area, STORAGE_FORMAT_M2, uc->AreaDisplay());

						patch->set_allocated_area(createProtobufObject(area));
						if (xFilter->m_comments.length())
							patch->set_allocated_comment(createProtobufObject(xFilter->m_comments));
					}
				}
			}
		}

		//Used fuel types
		if (PrntRptOptns.getFBPFuelTypes())
		{
			UCHAR		count, unique_count;
			long		ASCII_index, export_index;

			ICWFGM_Fuel* fuel = NULL;

			const FuelCollection* fc = &m_fuelCollection;
			auto fuelMap = fc->m_fuelMap;
			fuelMap->GetFuelCount(&count, &unique_count);

			for (UCHAR i = 0; i < unique_count; i++)
			{
				if (SUCCEEDED(fuelMap->FuelAtIndex(i, &ASCII_index, &export_index, &fuel)))
				{
					Fuel* f = Fuel::FromCOM(fuel);
					ICWFGM_FBPFuel* fbp = dynamic_cast<ICWFGM_FBPFuel*>(fuel);

					//Check if the fuel is active
					if (f->m_flags & Fuel::FLAG_USED)
					{
						auto type = summary->add_fuel_types();
						type->set_name(f->DisplayName(m_fuelMap.get()));

						//Check if there's differences
						type->set_is_modified((f->m_flags & (Fuel::FLAG_MODIFIED | Fuel::FLAG_MODIFIED_NAME | Fuel::FLAG_NONSTANDARD)) && (!f->IsNonFuel()));

						if (type->is_modified())
							f->PrintReportChanges(&m_fuelCollection, (void*)type);
						else
						{
							bool ret = false;
							fuel->IsC6FuelType(&ret);

							PolymorphicAttribute temp;
							HRESULT curResult;
							if (ret)
							{
								curResult = f->m_fuel->GetAttribute(FUELCOM_ATTRIBUTE_CBH, &temp);
								if (curResult == S_OK)
								{
									double fVal;
									VariantToDouble_(temp, &fVal);
									type->set_allocated_cbh(createProtobufObject(fVal));
								}
							}
							curResult = f->m_fuel->GetAttribute(FUELCOM_ATTRIBUTE_PDF, &temp);
							if (curResult == S_OK)
							{
								double fVal;
								VariantToDouble_(temp, &fVal);
								fVal *= 100.0;
								type->set_allocated_pdf(createProtobufObject(fVal));
							}
							curResult = f->m_fuel->GetAttribute(FUELCOM_ATTRIBUTE_PC, &temp);
							if (curResult == S_OK)
							{
								double fVal;
								VariantToDouble_(temp, &fVal);
								fVal *= 100.0;
								type->set_allocated_pc(createProtobufObject(fVal));
							}
							curResult = f->m_fuel->GetAttribute(FUELCOM_ATTRIBUTE_CURINGDEGREE, &temp);
							if (curResult == S_OK)
							{
								double fVal;
								VariantToDouble_(temp, &fVal);
								fVal *= 100.0;
								type->set_allocated_curing_degree(createProtobufObject(fVal));
							}
							curResult = f->m_fuel->GetAttribute(FUELCOM_ATTRIBUTE_GFL, &temp);
							if (curResult == S_OK)
							{
								double fVal;
								VariantToDouble_(temp, &fVal);
								fVal *= 10.0;
								type->set_allocated_gfl(createProtobufObject(fVal));
							}
						}
					}
				}
			}
		}

		//more weather data
		if (PrntRptOptns.getWxStreams() == 1)
		{
			double resolution;
			GetResolution(&resolution);
			for (ULONG i = 0; i < currScenario->GetStreamCount(); i++)
			{
				auto ws = currScenario->StreamAtIndex(i);
				auto stream = summary->add_weather_streams();
				stream->set_is_primary(ws == currScenario->GetPrimaryStream());
				if (ws->m_weatherStation()->m_name.length() == 0)
					stream->set_station_name("(No Name)"s);
				else
					stream->set_station_name(ws->m_weatherStation()->m_name);
				if (ws->m_name.length() == 0)
					stream->set_stream_name("(No Name)"s);
				else
					stream->set_stream_name(ws->m_name);
				ws->GetStartTime(tempStartTime);
				stream->set_start_time(tempStartTime.ToString(WTIME_FORMAT_STRING_ISO8601));
				ws->GetEndTime(tempEndTime);
				stream->set_end_time(tempEndTime.ToString(WTIME_FORMAT_STRING_ISO8601));

				stream->set_is_imported_file(ws->IsAnyImportedFromFile());
				stream->set_is_imported_ensemble(ws->IsAnyImportedFromEnsemble());
				stream->set_is_diurnal(ws->IsAnyDailyObservations());
				stream->set_is_user_modified(ws->IsAnyModified());

				if (ws->m_weatherStation()->m_comments.length() > 0)
					stream->set_allocated_station_comment(createProtobufObject(ws->m_weatherStation()->m_comments));
				if (ws->m_comments.length() > 0)
					stream->set_allocated_stream_comment(createProtobufObject(ws->m_comments));

				double x, y, lat, lon;
				ws->m_weatherStation()->GridLocation(&x, &y);
				GetLLByCoord(x, y, &lat, &lon);
				stream->set_allocated_station_x(createProtobufObject(lon));
				stream->set_allocated_station_y(createProtobufObject(lat));
				stream->set_allocated_station_elev(createProtobufObject(ws->m_weatherStation()->Elevation()));

				//yesterday
				stream->set_allocated_yesterday_ffmc(createProtobufObject(ws->Initial_FFMC()));
				stream->set_allocated_yesterday_dmc(createProtobufObject(ws->Initial_DMC()));
				stream->set_allocated_yesterday_dc(createProtobufObject(ws->Initial_DC()));
				stream->set_allocated_yesterday_precip(createProtobufObject(ws->Initial_Rain()));

				//today
				stream->set_allocated_today_hffmc(createProtobufObject(ws->Initial_HFFMC()));
				WeatherOptions wo = ws->GetWeatherOptions();
				if (wo.m_FFMC == 2)
					stream->set_allocated_today_hffmc_time(createProtobufObject(ws->Initial_HFFMCTime().GetHours()));

				stream->set_allocated_specified_fwi(createProtobufObject(ws->UseSpecifiedFWI() != 0));

				//hourly FFMC calculation method
				if (wo.m_FFMC == 0)
					stream->set_hffmc_method(WISE::ProjectProto::ScenarioReport_WeatherStream::VanWagner);
				else if (wo.m_FFMC == 2)
					stream->set_hffmc_method(WISE::ProjectProto::ScenarioReport_WeatherStream::Lawson);

				//Diurnal parameters
				if (ws->IsAnyDailyObservations()) {
					stream->set_allocated_temp_alpha(createProtobufObject(wo.m_temp_alpha));
					stream->set_allocated_temp_beta(createProtobufObject(wo.m_temp_beta));
					stream->set_allocated_temp_gamma(createProtobufObject(wo.m_temp_gamma));
					stream->set_allocated_wind_alpha(createProtobufObject(wo.m_wind_alpha));
					stream->set_allocated_wind_beta(createProtobufObject(wo.m_wind_beta));
					stream->set_allocated_wind_gamma(createProtobufObject(wo.m_wind_gamma));
				}

				// weather entries
				if (addWeather) {
					WTime nextTime((ULONGLONG)0, m_timeManager);
					WTimeSpan duration;
					WTimeSpan oneDay(1, 0, 0, 0);

					ws->GetValidTimeRange(nextTime, duration);
					WTime endTime(nextTime);
					endTime += duration;

					WTime sStartTime(currScenario->Simulation_StartTime()),
						sEndTime(currScenario->Simulation_EndTime());
					sStartTime.PurgeToHour(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
					sEndTime.PurgeToHour(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
					if (sEndTime < currScenario->Simulation_EndTime())
						sEndTime += WTimeSpan(0, 1, 0, 0);

					if ((sStartTime > nextTime) && (sStartTime < endTime))
						nextTime = sStartTime;
					if ((sEndTime < endTime) && (sEndTime > nextTime))
						endTime = sEndTime;

					while (nextTime <= endTime) {
						IWXData wx;
						IFWIData ifwi;
						DFWIData dfwi;
						if (ws->GetInstantaneousValues(nextTime, 0, &wx, &ifwi, &dfwi)) {
							auto entry = stream->add_wx_data();
							entry->set_time(nextTime.ToString(WTIME_FORMAT_STRING_ISO8601));

							auto p_wx = new WISE::ProjectProto::ScenarioReport_WeatherStream_WeatherStreamEntry_HourlyWeather();
							p_wx->set_allocated_dewpoint(createProtobufObject(wx.DewPointTemperature));
							p_wx->set_allocated_rh(createProtobufObject(wx.RH * 100.0));
							p_wx->set_allocated_precip(createProtobufObject(wx.Precipitation));
							p_wx->set_allocated_ws(createProtobufObject(wx.WindSpeed));
							p_wx->set_allocated_wd(createProtobufObject(CARTESIAN_TO_COMPASS_DEGREE(RADIAN_TO_DEGREE(wx.WindDirection))));
							p_wx->set_allocated_temp(createProtobufObject(wx.Temperature));
							entry->set_allocated_wx(p_wx);

							auto p_ifwi = new WISE::ProjectProto::ScenarioReport_WeatherStream_WeatherStreamEntry_HourlyFwi();
							p_ifwi->set_allocated_ffmc(createProtobufObject(ifwi.FFMC));
							p_ifwi->set_allocated_fwi(createProtobufObject(ifwi.FWI));
							p_ifwi->set_allocated_isi(createProtobufObject(ifwi.ISI));
							entry->set_allocated_hfwi(p_ifwi);

							auto p_dfwi = new WISE::ProjectProto::ScenarioReport_WeatherStream_WeatherStreamEntry_DailyFwi();
							p_dfwi->set_allocated_ffmc(createProtobufObject(dfwi.dFFMC));
							p_dfwi->set_allocated_isi(createProtobufObject(dfwi.dISI));
							p_dfwi->set_allocated_fwi(createProtobufObject(dfwi.dFWI));
							p_dfwi->set_allocated_dmc(createProtobufObject(dfwi.dDMC));
							p_dfwi->set_allocated_dc(createProtobufObject(dfwi.dDC));
							p_dfwi->set_allocated_bui(createProtobufObject(dfwi.dBUI));
							entry->set_allocated_dfwi(p_dfwi);
						}
						nextTime += WTimeSpan(0, 1, 0, 0);
					}
				}
			}
		}

		//Weather Patches and Weather Grids in Scenario
		if (PrntRptOptns.getWxPatchesAndWxGrids() == 1)
		{
			for (ULONG i = 0; i < currScenario->GetFilterCount(); i++)
			{
				auto pFilter = dynamic_cast<CWeatherPolyFilter*>(currScenario->FilterAtIndex(i));
				if (pFilter)
				{
					auto patch = summary->add_weather_patch_grid();
					patch->set_patch_type(WISE::ProjectProto::ScenarioReport_WeatherPatch::WeatherPatch);

					patch->set_name(pFilter->m_name);

					WTime tempTime(m_timeManager);
					pFilter->GetStartTime(tempTime);
					patch->set_start_time(tempTime.ToString(WTIME_FORMAT_STRING_ISO8601));
					pFilter->GetEndTime(tempTime);
					patch->set_end_time(tempTime.ToString(WTIME_FORMAT_STRING_ISO8601));

					if (pFilter->GetWindDirection() != -1)
						patch->set_allocated_wind_direction(createProtobufObject(pFilter->GetWindDirection()));
					if (pFilter->GetWindSpeed() != -1)
						patch->set_allocated_wind_speed(createProtobufObject(pFilter->GetWindSpeed()));

					USHORT op;
					std::string temp;
					if ((op = pFilter->GetTemperature_Operation()) != (USHORT)-1) {
						switch (op) {
						case 0: temp = strprintf("Temperature = %.3lf" DEGREE_SYMBOL "\n", pFilter->GetTemperature()); break;
						case 1: temp = strprintf("Temperature += %.3lf" DEGREE_SYMBOL "\n", pFilter->GetTemperature()); break;
						case 2: temp = strprintf("Temperature -= %.3lf" DEGREE_SYMBOL "\n", pFilter->GetTemperature()); break;
						case 3: temp = strprintf("Temperature *= %.3lf" DEGREE_SYMBOL "\n", pFilter->GetTemperature()); break;
						case 4: temp = strprintf("Temperature /= %.3lf" DEGREE_SYMBOL "\n", pFilter->GetTemperature()); break;
						}
						patch->add_operations(temp);
					}

					if ((op = pFilter->GetRH_Operation()) != (USHORT)-1) {
						switch (op) {
						case 0: temp = strprintf("RH = %.3lf%%\n", pFilter->GetRH()); break;
						case 1: temp = strprintf("RH += %.3lf%%\n", pFilter->GetRH()); break;
						case 2: temp = strprintf("RH -= %.3lf%%\n", pFilter->GetRH()); break;
						case 3: temp = strprintf("RH *= %.3lf%%\n", pFilter->GetRH()); break;
						case 4: temp = strprintf("RH /= %.3lf%%\n", pFilter->GetRH()); break;
						}
						patch->add_operations(temp);
					}

					if ((op = pFilter->GetPrecipitation_Operation()) != (USHORT)-1) {
						switch (op) {
						case 0: temp = strprintf("Precipitation = %.1lf %s\n", uc->ConvertUnit(pFilter->GetPrecipitation(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()), uc->UnitName(uc->SmallMeasureDisplay(), true).c_str()); break;
						case 1: temp = strprintf("Precipitation += %.1lf %s\n", uc->ConvertUnit(pFilter->GetPrecipitation(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()), uc->UnitName(uc->SmallMeasureDisplay(), true).c_str()); break;
						case 2: temp = strprintf("Precipitation -= %.1lf %s\n", uc->ConvertUnit(pFilter->GetPrecipitation(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()), uc->UnitName(uc->SmallMeasureDisplay(), true).c_str()); break;
						case 3: temp = strprintf("Precipitation *= %.1lf %s\n", uc->ConvertUnit(pFilter->GetPrecipitation(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()), uc->UnitName(uc->SmallMeasureDisplay(), true).c_str()); break;
						case 4: temp = strprintf("Precipitation /= %.1lf %s\n", uc->ConvertUnit(pFilter->GetPrecipitation(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()), uc->UnitName(uc->SmallMeasureDisplay(), true).c_str()); break;
						}
						patch->add_operations(temp);
					}

					if ((op = pFilter->GetWS_Operation()) != (USHORT)-1) {
						switch (op) {
						case 0: temp = strprintf("Wind Speed = %.1lf %s\n", uc->ConvertUnit(pFilter->GetWindSpeed(), STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay()), uc->UnitName(uc->VelocityDisplay(), true).c_str()); break;
						case 1: temp = strprintf("Wind Speed += %.1lf %s\n", uc->ConvertUnit(pFilter->GetWindSpeed(), STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay()), uc->UnitName(uc->VelocityDisplay(), true).c_str()); break;
						case 2: temp = strprintf("Wind Speed -= %.1lf %s\n", uc->ConvertUnit(pFilter->GetWindSpeed(), STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay()), uc->UnitName(uc->VelocityDisplay(), true).c_str()); break;
						case 3: temp = strprintf("Wind Speed *= %.1lf %s\n", uc->ConvertUnit(pFilter->GetWindSpeed(), STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay()), uc->UnitName(uc->VelocityDisplay(), true).c_str()); break;
						case 4: temp = strprintf("Wind Speed /= %.1lf %s\n", uc->ConvertUnit(pFilter->GetWindSpeed(), STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay()), uc->UnitName(uc->VelocityDisplay(), true).c_str()); break;
						}
						patch->add_operations(temp);
					}

					if ((op = pFilter->GetWD_Operation()) != (USHORT)-1) {
						switch (op) {
						case 0: temp = strprintf("Wind Direction = %.3lf" DEGREE_SYMBOL "\n", pFilter->GetWindDirection()); break;
						case 1: temp = strprintf("Wind Direction += %.3lf" DEGREE_SYMBOL "\n", pFilter->GetWindDirection()); break;
						case 2: temp = strprintf("Wind Direction -= %.3lf" DEGREE_SYMBOL "\n", pFilter->GetWindDirection()); break;
						}
						patch->add_operations(temp);
					}

					if (pFilter->m_comments.length())
						patch->set_allocated_comment(createProtobufObject(pFilter->m_comments));
				}
				else
				{
					auto qFilter = dynamic_cast<CWeatherGridFilter*>(currScenario->FilterAtIndex(i));
					if (qFilter)
					{
						auto patch = summary->add_weather_patch_grid();
						patch->set_patch_type(WISE::ProjectProto::ScenarioReport_WeatherPatch::WeatherGrid);

						patch->set_name(qFilter->m_name);

						WTime tempTime(m_timeManager);
						qFilter->GetStartTime(tempTime);
						patch->set_start_time(tempTime.ToString(WTIME_FORMAT_STRING_ISO8601));
						qFilter->GetEndTime(tempTime);
						patch->set_end_time(tempTime.ToString(WTIME_FORMAT_STRING_ISO8601));

						if (qFilter->m_comments.length())
							patch->set_allocated_comment(createProtobufObject(qFilter->m_comments));
					}
					else
					{
						auto rFilter = dynamic_cast<CWindDirectionGrid*>(currScenario->FilterAtIndex(i));
						auto sFilter = dynamic_cast<CWindSpeedGrid*>(currScenario->FilterAtIndex(i));
						auto tFilter = dynamic_cast<CWeatherGridBase*>(currScenario->FilterAtIndex(i));
						if (tFilter)
						{
							auto patch = summary->add_weather_patch_grid();

							if (rFilter) {
								patch->set_patch_type(WISE::ProjectProto::ScenarioReport_WeatherPatch::DirectionGrid);

								for (USHORT j = 0; j < rFilter->GetSectorCount(); j++)
								{
									for (USHORT k = 0; k < rFilter->GetRangeCount(j); k++)
									{
										patch->add_files(rFilter->GetRangeString(j, k));
									}
								}
							} else if (sFilter) {
								patch->set_patch_type(WISE::ProjectProto::ScenarioReport_WeatherPatch::SpeedGrid);

								for (USHORT j = 0; j < sFilter->GetSectorCount(); j++)
								{
									for (USHORT k = 0; k < sFilter->GetRangeCount(j); k++)
									{
										patch->add_files(sFilter->GetRangeString(j, k));
									}
								}
							}

							patch->set_name(tFilter->m_name);

							WTime tempTime(m_timeManager);
							tFilter->GetStartTime(tempTime);
							patch->set_start_time(tempTime.ToString(WTIME_FORMAT_STRING_ISO8601));
							tFilter->GetEndTime(tempTime);
							patch->set_end_time(tempTime.ToString(WTIME_FORMAT_STRING_ISO8601));

							WTimeSpan tempTimeSpan;
							tFilter->GetStartTimeOfDay(tempTimeSpan);
							patch->set_allocated_start_hour(createProtobufObject(tempTimeSpan.ToString(WTIME_FORMAT_STRING_ISO8601)));
							tFilter->GetEndTimeOfDay(tempTimeSpan);
							patch->set_allocated_end_hour(createProtobufObject(tempTimeSpan.ToString(WTIME_FORMAT_STRING_ISO8601)));

							if (tFilter->m_comments.length())
								patch->set_allocated_comment(createProtobufObject(tFilter->m_comments));
						}
					}
				}
			}
		}

		//Fires in Scenario
		if (PrntRptOptns.getIgnitions() == 1)
		{
			for (ULONG i = 0; i < currScenario->GetFireCount(); i++)
			{
				auto ignition = summary->add_ignitions();
				Fire* f = currScenario->FireAtIndex(i);

				ignition->set_name(f->m_name);
				ignition->set_start_time(f->IgnitionTime().ToString(WTIME_FORMAT_STRING_ISO8601));
				PolymorphicAttribute v;
				WTime t((std::uint64_t)0, m_timeManager);
				if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITION_START_TIME_OVERRIDE, &v)) &&
					SUCCEEDED(VariantToTime_(v, &t)))
				{
					if (t.GetTotalMicroSeconds() > 0)
						ignition->set_allocated_start_time_override(createProtobufObject(t.ToString(WTIME_FORMAT_STRING_ISO8601)));
				}
				if (f->m_comments.length())
					ignition->set_allocated_comment(createProtobufObject(f->m_comments));

				if (f->IgnitionType(0) == CWFGM_FIRE_IGNITION_POINT)
				{
					ignition->set_ignition_type(WISE::ProjectProto::ScenarioReport_Ignition::Point);

					std::uint32_t size = 0;
					USHORT type;
					XY_Point* pt = f->GetIgnition(0, &type, &size);

					GeoPoint point(*pt);
					point.setStoredUnits(GeoPoint::UTM);
					ignition->set_allocated_pt_loc(point.getProtobuf(false));

					delete[] pt;
				}
				else if (f->IgnitionType(0) == CWFGM_FIRE_IGNITION_LINE)
					ignition->set_ignition_type(WISE::ProjectProto::ScenarioReport_Ignition::Line);
				else
					ignition->set_ignition_type(WISE::ProjectProto::ScenarioReport_Ignition::Polygon);
			}
		}

		reports.push_back(summary);
	}

	HRESULT retval{ S_OK };
	if (szPath)
	{
		if (reports.size() == 1)
		{
			if (boost::iends_with(std::string(szPath), ".bin"))
			{
				std::ofstream out(szPath);
				if (out.is_open())
				{
					reports[0]->SerializeToOstream(&out);
					out.close();
				}
				else
					retval = ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_WARNING;
			}
			else
			{
				std::string json;
				google::protobuf::util::JsonOptions options1;
				options1.add_whitespace = false;
				options1.always_print_primitive_fields = true;
				options1.preserve_proto_field_names = true;
				options1.always_print_enums_as_ints = true;

				google::protobuf::util::MessageToJsonString(*reports[0], &json, options1);

				std::ofstream out(szPath);
				if (out.is_open())
				{
					out << json;
					out.close();
				}
				else
					retval = ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_WARNING;
			}
		}
		else if (reports.size() > 1)
		{
			auto group = new WISE::ProjectProto::ScenarioReportGroup();
			for (auto report : reports)
				group->add_reports()->Swap(report);

			if (boost::iends_with(std::string(szPath), ".bin"))
			{
				std::ofstream out(szPath);
				if (out.is_open())
				{
					group->SerializeToOstream(&out);
					out.close();
				}
				else
					retval = ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_WARNING;
			}
			else
			{
				std::string json;
				google::protobuf::util::JsonOptions options1;
				options1.add_whitespace = false;
				options1.always_print_primitive_fields = true;
				options1.preserve_proto_field_names = true;
				options1.always_print_enums_as_ints = true;

				google::protobuf::util::MessageToJsonString(*group, &json, options1);

				std::ofstream out(szPath);
				if (out.is_open())
				{
					out << json;
					out.close();
				}
				else
					retval = ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_WARNING;
			}

			delete group;
		}
	}
	else if (collection)
	{
		for (auto report : reports)
			collection->m_collection->add_reports()->Swap(report);
	}

	//cleanup
	for (auto summary : reports)
		delete summary;
	reports.clear();

	return retval;
}


HRESULT Project::CWFGMProject::ExportReportJson(const TCHAR* szPath, const SummaryCollection* collection)
{
	HRESULT retval{ S_OK };
	if (boost::iends_with(std::string(szPath), ".bin"))
	{
		std::ofstream out(szPath);
		if (out.is_open())
		{
			collection->m_collection->SerializeToOstream(&out);
			out.close();
		}
		else
			retval = ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_WARNING;
	}
	else
	{
		std::string json;
		google::protobuf::util::JsonOptions options1;
		options1.add_whitespace = false;
		options1.always_print_primitive_fields = true;
		options1.preserve_proto_field_names = true;
		options1.always_print_enums_as_ints = true;

		google::protobuf::util::MessageToJsonString(*collection->m_collection, &json, options1);

		std::ofstream out(szPath);
		if (out.is_open())
		{
			out << json;
			out.close();
		}
		else
			retval = ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_WARNING;
	}

	return retval;
}


unsigned short modifiableKeys[] = {
	FUELCOM_ATTRIBUTE_M3M4_C2_A,
	FUELCOM_ATTRIBUTE_M3M4_C2_B,
	FUELCOM_ATTRIBUTE_M3M4_C2_C,
	FUELCOM_ATTRIBUTE_M4_D1_A,
	FUELCOM_ATTRIBUTE_A,
	FUELCOM_ATTRIBUTE_B,
	FUELCOM_ATTRIBUTE_C,
	FUELCOM_ATTRIBUTE_M4_D1_B,
	FUELCOM_ATTRIBUTE_M4_D1_C,
	FUELCOM_ATTRIBUTE_Q,
	FUELCOM_ATTRIBUTE_BUI0,
	FUELCOM_ATTRIBUTE_MAXBE,
	FUELCOM_ATTRIBUTE_CFL,
	FUELCOM_ATTRIBUTE_TREE_HEIGHT,
	FUELCOM_ATTRIBUTE_O1AB_MATTED_A,
	FUELCOM_ATTRIBUTE_O1AB_MATTED_B,
	FUELCOM_ATTRIBUTE_O1AB_MATTED_C,
	FUELCOM_ATTRIBUTE_O1AB_STANDING_A,
	FUELCOM_ATTRIBUTE_O1AB_STANDING_B,
	FUELCOM_ATTRIBUTE_O1AB_STANDING_C,
	FUELCOM_ATTRIBUTE_CBH,
	FUELCOM_ATTRIBUTE_PDF,
	FUELCOM_ATTRIBUTE_PC,
	FUELCOM_ATTRIBUTE_CURINGDEGREE,
	FUELCOM_ATTRIBUTE_GFL,
	0
};


void Project::Fuel::PrintReportChanges(FuelCollection* m_fuelCollection, void* m_data)
{
	auto type = static_cast<WISE::ProjectProto::ScenarioReport_FuelType*>(m_data);

	Fuel* defaultFuel = NULL;

	//Get the default fuel
	if (m_fuelCollection->m_csLUTFileName == "Canada")
		defaultFuel = FindDefault(m_fuelCollection->m_fuelCollection_Canada, NULL, NULL);

	else if (m_fuelCollection->m_csLUTFileName == "New Zealand")
		defaultFuel = FindDefault(NULL, m_fuelCollection->m_fuelCollection_NewZealand, NULL);

	else if (m_fuelCollection->m_csLUTFileName == "Tasmania")
		defaultFuel = FindDefault(NULL, NULL, m_fuelCollection->m_fuelCollection_Tasmania);

	if (!defaultFuel) // can happen if m_csLUTFileName is empty
		defaultFuel = FindDefault(m_fuelCollection->m_fuelCollection_Canada, m_fuelCollection->m_fuelCollection_NewZealand, m_fuelCollection->m_fuelCollection_Tasmania);

	//Check for differences and build both the unmodified and modified lists
	UCHAR		count, unique_count, ii;
	long		ASCII_index, export_index, tmp;

	//Get the full fuel name
	Fuel* f = Fuel::FromCOM(this->m_fuel.get());

	type->set_name(f->DisplayName(m_fuelCollection->m_fuelMap.get()));

	PolymorphicAttribute temp, dTemp;

	//Check for mixed fuel types
	boost::intrusive_ptr<ICWFGM_FBPFuel> pC2;
	boost::intrusive_ptr<ICWFGM_Fuel> pc2;
	boost::intrusive_ptr<ICWFGM_FBPFuel> pD1;
	boost::intrusive_ptr<ICWFGM_Fuel> pd1;

	boost::intrusive_ptr<ICWFGM_FBPFuel> fbpfuel = boost::dynamic_pointer_cast<ICWFGM_FBPFuel>(this->m_fuel);

	if (fbpfuel != NULL)
	{
		fbpfuel->get_C2(&pc2);
		fbpfuel->get_D1(&pd1);

		if (pc2 != nullptr)
			pC2 = boost::dynamic_pointer_cast<ICWFGM_FBPFuel>(pc2);
		if (pd1 != nullptr)
			pD1 = boost::dynamic_pointer_cast<ICWFGM_FBPFuel>(pd1);
	}

	//Check each attribute key for a value
	short j = 0;
	unsigned short curKey = modifiableKeys[j++];
	while (curKey != 0)
	{
		HRESULT curResult = f->m_fuel->GetAttribute(curKey, &temp);
		HRESULT defResult;
		if (defaultFuel)
			defResult = defaultFuel->m_fuel->GetAttribute(curKey, &dTemp);
		else
			defResult = !curResult;		// this looks odd, but is done for the logic in the following if statement

		bool valid = false;
		double fVal;

		//Check if they both returned the same value
		if ((defaultFuel) && (curResult == defResult))
		{
			//Check if they both returned S_OK else that means they both failed meaning they're equal and no change to report
			if (curResult == S_OK)
			{
				//Need to compare the values
				VariantToDouble_(temp, &fVal);
				double dVal;
				VariantToDouble_(dTemp, &dVal);

				if (fVal != dVal)
				{
					//Values are different, need to output it
					valid = true;
				}
			}
			else if (pC2 != NULL || pD1 != NULL)
			{
				//Try getting value from there
				if (pC2)
				{
					curResult = pC2->GetAttribute(curKey, &temp);
					if (curResult == S_OK)
					{
						VariantToDouble_(temp, &fVal);
						valid = true;
					}
				}
				else if (pD1)
				{
					curResult = pD1->GetAttribute(curKey, &temp);
					if (curResult == S_OK)
					{
						VariantToDouble_(temp, &fVal);
						valid = true;
					}
				}
			}
		}
		else
		{	//One failed and one didn't
			//Check that this isn't a mixed fuel type
			if (curResult != E_UNEXPECTED)
			{
				VariantToDouble_(temp, &fVal);
				valid = true;
			}
		}

		if (valid)
		{
			switch (curKey)
			{
			case FUELCOM_ATTRIBUTE_M3M4_C2_A:
				type->set_allocated_m3m4_c2_a(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_M3M4_C2_B:
				type->set_allocated_m3m4_c2_b(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_M3M4_C2_C:
				type->set_allocated_m3m4_c2_c(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_M4_D1_A:
				type->set_allocated_m4_d1_a(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_M4_D1_B:
				type->set_allocated_m4_d1_b(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_M4_D1_C:
				type->set_allocated_m4_d1_c(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_A:
				type->set_allocated_a(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_B:
				type->set_allocated_b(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_C:
				type->set_allocated_c(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_Q:
				type->set_allocated_q(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_BUI0:
				type->set_allocated_bui0(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_MAXBE:
				type->set_allocated_maxbe(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_CFL:
				fVal *= 10.0;
				type->set_allocated_cfl(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_TREE_HEIGHT:
				type->set_allocated_tree_height(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_O1AB_MATTED_A:
				type->set_allocated_o1ab_matted_a(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_O1AB_MATTED_B:
				type->set_allocated_o1ab_matted_b(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_O1AB_MATTED_C:
				type->set_allocated_o1ab_matted_c(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_O1AB_STANDING_A:
				type->set_allocated_o1ab_standing_a(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_O1AB_STANDING_B:
				type->set_allocated_o1ab_standing_b(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_O1AB_STANDING_C:
				type->set_allocated_o1ab_standing_c(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_CBH:
				type->set_allocated_cbh(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_PDF:
				fVal *= 100.0;
				type->set_allocated_pdf(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_PC:
				fVal *= 100.0;
				type->set_allocated_pc(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_CURINGDEGREE:
				fVal *= 100.0;
				type->set_allocated_curing_degree(createProtobufObject(fVal));
				break;
			case FUELCOM_ATTRIBUTE_GFL:
				//this gets a *10 in PrintOptionalParams but not here
				type->set_allocated_gfl(createProtobufObject(fVal));
				break;
			}
		}

		curKey = modifiableKeys[j++];
	}
}


std::int32_t Project::FGMOutputs::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return 1;
}


WISE::ProjectProto::Project_Outputs* Project::FGMOutputs::serialize(const SerializeProtoOptions& options) {
	auto outputs = new WISE::ProjectProto::Project_Outputs();

	for (size_t i = 0; i < summaries.size(); i++)
	{
		auto summary = outputs->add_summaries();
		summary->set_version(1);
		summary->set_scenarioname(summaries[i].scenarioName);
		summary->set_filename(summaries[i].filename);
		auto metadata = new WISE::ProjectProto::Project_Outputs_SummaryOutput_MetadataOutput();
		if (summaries[i].applicationName.has_value())
			metadata->set_allocated_applicationname(createProtobufObject(summaries[i].applicationName.value()));
		if (summaries[i].geoData.has_value())
			metadata->set_allocated_geodata(createProtobufObject(summaries[i].geoData.value()));
		if (summaries[i].assetData.has_value())
			metadata->set_allocated_assetdata(createProtobufObject(summaries[i].assetData.value()));
		if (summaries[i].outputScenarioName.has_value())
			metadata->set_allocated_scnearioname(createProtobufObject(summaries[i].outputScenarioName.value()));
		if (summaries[i].scenarioComments.has_value())
			metadata->set_allocated_scenariocomments(createProtobufObject(summaries[i].scenarioComments.value()));
		if (summaries[i].inputs.has_value())
			metadata->set_allocated_inputs(createProtobufObject(summaries[i].inputs.value()));
		if (summaries[i].landscape.has_value())
			metadata->set_allocated_landscape(createProtobufObject(summaries[i].landscape.value()));
		if (summaries[i].fbpPatches.has_value())
			metadata->set_allocated_fbppatches(createProtobufObject(summaries[i].fbpPatches.value()));
		if (summaries[i].wxPatches.has_value())
			metadata->set_allocated_wxpatches(createProtobufObject(summaries[i].wxPatches.value()));
		if (summaries[i].ignitions.has_value())
			metadata->set_allocated_ignitions(createProtobufObject(summaries[i].ignitions.value()));
		if (summaries[i].wxStreams.has_value())
			metadata->set_allocated_wxstreams(createProtobufObject(summaries[i].wxStreams.value()));
		if (summaries[i].fbp.has_value())
			metadata->set_allocated_fbp(createProtobufObject(summaries[i].fbp.value()));
		if (summaries[i].wxData.has_value())
			metadata->set_allocated_wxdata(createProtobufObject(summaries[i].wxData.value()));
		summary->set_allocated_metadata(metadata);
		summary->set_allocated_streamoutput(createProtobufObject(summaries[i].shouldStream));
	}

	for (size_t i = 0; i < stats.size(); i++)
	{
		auto& stat = stats[i];
		for (auto& entry : stat.entries)
		{
       		auto p_st = outputs->add_stats();
       		p_st->set_version(1);
			p_st->set_scenarioname(stat.scenarioName);
			p_st->set_filename(stat.filename);
			p_st->set_filetype((WISE::ProjectProto::Project_Outputs_StatsOutput::StatsFileType)stat.fileType);
			p_st->set_allocated_streamoutput(createProtobufObject(stat.shouldStream));
			if (entry.use_stream)
		    {
				if (entry.streamName.length() > 0)
					p_st->set_streamname(entry.streamName);
			    else
					p_st->set_streamindex(entry.stream_idx);
     		}
     		else
     		{
     			double resolution;
     			double xll, yll;
     			m_project->GetResolution(&resolution);
     			m_project->GetXLLCorner(&xll);
     			m_project->GetYLLCorner(&yll);
     			XY_Point pt;
				pt.x = entry.location.x * resolution + xll;
				pt.y = entry.location.y * resolution + yll;
     			GeoPoint point(pt);
     			point.setStoredUnits(GeoPoint::UTM);
     			p_st->set_allocated_location(point.getProtobuf(options.useVerboseFloats()));
     		}
     
			for (size_t ii = 0; ii < entry.columns.size(); ii++)
     		{
				p_st->add_globalcolumns(statStatMap.right.at((WISE::ProjectProto::Project_Outputs_StatsOutput_StatisticsType)entry.columns[ii]));
     		}

			auto options_ = new ::WISE::ProjectProto::Project_DiscretizedStatsOptions();
			options_->set_version(1);
			options_->set_discretize(stat.discretize);
			p_st->set_allocated_discretizedoptions(options_);
     	}
	}

	for (size_t i = 0; i < vectors.size(); i++)
	{
		auto vector = outputs->add_vectors();
		vector->set_version(1);
		vector->set_scenarioname(vectors[i].scenarioName);
		for (auto& f : vectors[i].subScenarioNames)
		{
			vector->add_subscenarioname()
				->set_value(f);
		}
		vector->set_filename(vectors[i].filename);
		vector->set_multipleperimeters(vectors[i].multiplePerimeters);
		vector->set_removeislands(vectors[i].removeIslands);
		vector->set_mergecontacting(vectors[i].mergeContacting);
		vector->set_activeperimeters(vectors[i].activePerimeters);
		if (vectors[i].coverageName.size() > 0)
			vector->set_allocated_coverage_name(createProtobufObject(vectors[i].coverageName));
		auto metadata = new WISE::ProjectProto::Project_Outputs_VectorOutput_MetadataOutput();
		if (vectors[i].applicationVersion.has_value())
			metadata->set_applicationversion(vectors[i].applicationVersion.value());
		if (vectors[i].outputScenarioName.has_value())
			metadata->set_scenarioname(vectors[i].outputScenarioName.value());
		if (vectors[i].jobName.has_value())
			metadata->set_jobname(vectors[i].jobName.value());
		if (vectors[i].ignitionName.has_value())
			metadata->set_ignitionname(vectors[i].ignitionName.value());
		if (vectors[i].simulationDate.has_value())
			metadata->set_simulationdate(vectors[i].simulationDate.value());
		if (vectors[i].fireSize.has_value())
			metadata->set_firesize(vectors[i].fireSize.value());
		if (vectors[i].perimeterTotal.has_value())
			metadata->set_perimetertotal(vectors[i].perimeterTotal.value());
		if (vectors[i].perimeterActive.has_value())
			metadata->set_perimeteractive(vectors[i].perimeterActive.value());
		if (vectors[i].wxValues.has_value())
			metadata->set_wxvalues(vectors[i].wxValues.value());
		if (vectors[i].fwiValues.has_value())
			metadata->set_fwivalues(vectors[i].fwiValues.value());
		if (vectors[i].ignitionLocation.has_value())
			metadata->set_allocated_ignitionlocation(createProtobufObject(vectors[i].ignitionLocation.value()));
		if (vectors[i].ignitionAttributes.has_value())
			metadata->set_allocated_ignitionattributes(createProtobufObject(vectors[i].ignitionAttributes.value()));
		if (vectors[i].maxBurnDistance.has_value())
			metadata->set_allocated_maxburndistance(createProtobufObject(vectors[i].maxBurnDistance.value()));
		if (vectors[i].assetArrivalTime.has_value())
			metadata->set_allocated_assetarrivaltime(createProtobufObject(vectors[i].assetArrivalTime.value()));
		if (vectors[i].assetCount.has_value())
			metadata->set_allocated_assetarrivalcount(createProtobufObject(vectors[i].assetCount.value()));
		if (vectors[i].identifyFinalPerimeter.has_value())
			metadata->set_allocated_identifyfinalperimeter(createProtobufObject(vectors[i].identifyFinalPerimeter.value()));
		if (vectors[i].simulationStatus.has_value())
			metadata->set_allocated_simulationstatus(createProtobufObject(vectors[i].simulationStatus.value()));
		if (vectors[i].areaUnits.has_value())
			metadata->set_areaunits((WISE::ProjectProto::Project_Outputs_VectorOutput_MetadataOutput_AreaUnits)vectors[i].areaUnits.value());
		if (vectors[i].perimeterUnits.has_value())
			metadata->set_perimeterunits((WISE::ProjectProto::Project_Outputs_VectorOutput_MetadataOutput_PerimeterUnits)vectors[i].perimeterUnits.value());
		vector->set_allocated_metadata(metadata);

		auto times = new WISE::ProjectProto::Project_Outputs_VectorOutput_PerimeterTime();
		times->set_allocated_starttime(HSS_Time::Serialization::TimeSerializer().serializeTime(vectors[i].startTime, options.fileVersion()));
		times->set_allocated_endtime(HSS_Time::Serialization::TimeSerializer().serializeTime(vectors[i].endTime, options.fileVersion()));
		vector->set_allocated_perimetertime(times);

		for (auto& f : vectors[i].timeOverrides)
		{
			auto b = vector->add_perimetertimeoverride();
			b->set_subscenarioname(f.subName);
			b->set_allocated_starttime(HSS_Time::Serialization::TimeSerializer().serializeTime(f.startTime, options.fileVersion()));
			b->set_allocated_endtime(HSS_Time::Serialization::TimeSerializer().serializeTime(f.endTime, options.fileVersion()));
		}

		vector->set_allocated_streamoutput(createProtobufObject(vectors[i].shouldStream));
	}

	for (size_t i = 0; i < fuelgrids.size(); i++) {
		auto fgo = outputs->add_fuelgrids();
		fgo->set_version(1);
		fgo->set_filename(fuelgrids[i].filename);
		fgo->set_scenarioname(fuelgrids[i].scenarioName);
		if (!fuelgrids[i].subScenarioName.empty())
			fgo->set_allocated_scenarioname(new std::string(fuelgrids[i].subScenarioName));
		fgo->set_allocated_streamoutput(createProtobufObject(fuelgrids[i].shouldStream));
		if (fuelgrids[i].coverageName.size() > 0)
			fgo->set_allocated_coverage_name(createProtobufObject(fuelgrids[i].coverageName));

		switch (fuelgrids[i].compression)
		{
		case GDALExporter::CompressionType::JPEG:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::JPEG);
			break;
		case GDALExporter::CompressionType::LZW:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::LZW);
			break;
		case GDALExporter::CompressionType::PACKBITS:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::PACKBITS);
			break;
		case GDALExporter::CompressionType::DEFLATE:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::DEFLATE);
			break;
		case GDALExporter::CompressionType::CCITTRLE:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::CCITTRLE);
			break;
		case GDALExporter::CompressionType::CCITTFAX3:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::CCITTFAX3);
			break;
		case GDALExporter::CompressionType::CCITTFAX4:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::CCITTFAX4);
			break;
		case GDALExporter::CompressionType::LZMA:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::LZMA);
			break;
		case GDALExporter::CompressionType::ZSTD:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::ZSTD);
			break;
		case GDALExporter::CompressionType::LERC:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::LERC);
			break;
		case GDALExporter::CompressionType::LERC_DEFLATE:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::LERC_DEFLATE);
			break;
		case GDALExporter::CompressionType::LERC_ZSTD:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::LERC_ZSTD);
			break;
		case GDALExporter::CompressionType::WEBP:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::WEBP);
			break;
		default:
			fgo->set_compression(WISE::ProjectProto::Project_Outputs::NONE);
			break;
		}
	}

	for (size_t i = 0; i < grids.size(); i++)
	{
		for (size_t j = 0; j < grids[i].outputs.size(); j++) {
			auto grid = outputs->add_grids();
			grid->set_version(1);
			grid->set_filename(grids[i].outputs[j].filename);
			grid->set_scenarioname(grids[i].scenarioName);
			for (auto& f : grids[i].subScenarioNames)
			{
				grid->add_subscenarioname()
					->set_value(f);
			}
			for (auto& f : grids[i].timeOverrides)
			{
				auto b = grid->add_subscenarioexporttimes();
				b->set_subscenarioname(f._subName);
				if (f._startTime.IsValid())
					b->set_allocated_starttime(HSS_Time::Serialization::TimeSerializer().serializeTime(f._startTime, options.fileVersion()));
				if (f._endTime.IsValid())
					b->set_allocated_time(HSS_Time::Serialization::TimeSerializer().serializeTime(f._endTime, options.fileVersion()));
			}
			if ((grids[i].startExportTime.IsValid()) && grids[i].startExportTime.GetTotalMicroSeconds())
				grid->set_allocated_startexporttime(HSS_Time::Serialization::TimeSerializer().serializeTime(grids[i].startExportTime, options.fileVersion()));
			grid->set_allocated_exporttime(HSS_Time::Serialization::TimeSerializer().serializeTime(grids[i].exportTime, options.fileVersion()));
			grid->set_interpolation((WISE::ProjectProto::Project_Outputs_GridOutput_InterpolationMethod)grids[i].interpolation);
			if (grids[i].interpolation == 4) {
				auto options2 = new ::WISE::ProjectProto::Project_DiscretizedStatsOptions();
				options2->set_version(1);
				options2->set_discretize(grids[i].discretize);
				grid->set_allocated_discretizedoptions(options2);
			}
			if (grids[i].gridOptionsSet) {
				auto options2 = new ::WISE::ProjectProto::Project_Outputs_GridOutputOptions();
				options2->set_version(1);
				if (grids[i].scale.has_value())
					options2->set_scale(grids[i].scale.value());
				else if (grids[i].useFuelMap.has_value())
					options2->set_usefuelmap(grids[i].useFuelMap.value());
				else
					options2->set_resolution(grids[i].resolution);
				if (grids[i].outputOriginMode != (std::uint8_t)-1)
					options2->set_dynamicorigin((::WISE::ProjectProto::Project_Outputs_GridOutputOptions_GridOutputOriginOptions)grids[i].outputOriginMode);
				else {
					GeoPoint geo(grids[i].origin);
					geo.setStoredUnits(GeoPoint::UTM);
					options2->set_allocated_location(geo.getProtobuf(options.useVerboseFloats()));
				}
				grid->set_allocated_gridoptions(options2);
			}
			grid->set_globalstatistics(gridStatMap.right.at((WISE::ProjectProto::Project_Outputs_GridOutput_StatisticsType)grids[i].outputs[j].statistic));
			if (grids[i].outputs[j].assetName.length()) {
				grid->set_allocated_assetname(createProtobufObject(grids[i].outputs[j].assetName));
				grid->set_allocated_assetindex(createProtobufObject(grids[i].outputs[j].assetIndex));
			}
			if (!grids[i].outputs[j].coverageName.empty())
				grid->set_allocated_coverage_name(createProtobufObject(grids[i].outputs[j].coverageName));

			grid->set_allocated_streamoutput(createProtobufObject(grids[i].shouldStream));
			grid->set_allocated_outputnodata(createProtobufObject(grids[i].outputNodata));
			grid->set_allocated_minimizeoutput(createProtobufObject(grids[i].minimizeOutput));

			switch (grids[i].compression)
			{
			case GDALExporter::CompressionType::JPEG:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::JPEG);
				break;
			case GDALExporter::CompressionType::LZW:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::LZW);
				break;
			case GDALExporter::CompressionType::PACKBITS:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::PACKBITS);
				break;
			case GDALExporter::CompressionType::DEFLATE:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::DEFLATE);
				break;
			case GDALExporter::CompressionType::CCITTRLE:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::CCITTRLE);
				break;
			case GDALExporter::CompressionType::CCITTFAX3:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::CCITTFAX3);
				break;
			case GDALExporter::CompressionType::CCITTFAX4:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::CCITTFAX4);
				break;
			case GDALExporter::CompressionType::LZMA:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::LZMA);
				break;
			case GDALExporter::CompressionType::ZSTD:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::ZSTD);
				break;
			case GDALExporter::CompressionType::LERC:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::LERC);
				break;
			case GDALExporter::CompressionType::LERC_DEFLATE:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::LERC_DEFLATE);
				break;
			case GDALExporter::CompressionType::LERC_ZSTD:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::LERC_ZSTD);
				break;
			case GDALExporter::CompressionType::WEBP:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::WEBP);
				break;
			default:
				grid->set_compression(WISE::ProjectProto::Project_Outputs::NONE);
				break;
			}
		}
	}

	auto jobOutput = new WISE::ProjectProto::Project_Outputs_JobOutput();
	if (job.saveJob)
		jobOutput->set_allocated_savejob(createProtobufObject(true));
	if (job.filename.length() > 0)
		jobOutput->set_allocated_filename(createProtobufObject(job.filename));
	outputs->set_allocated_job(jobOutput);

	for (auto& asset : assetOutputs)
	{
		auto nAsset = outputs->add_assetstats();
		nAsset->set_filename(asset.filename);
		nAsset->set_scenarioname(asset.scenarioName);
		nAsset->set_allocated_streamoutput(createProtobufObject(asset.shouldStream));
		nAsset->set_version(1);
		nAsset->set_filetype((WISE::ProjectProto::Project_Outputs_StatsOutput_StatsFileType)asset.fileType);
		nAsset->set_allocated_criticalpathembedded(createProtobufObject(asset.criticalPath));
		nAsset->set_criticalpathfilename(asset.criticalPathFilename);
	}

	return outputs;
}


auto Project::FGMOutputs::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> FGMOutputs*
{
	throw std::logic_error("Incorrect FGM output deserialization");
}


auto Project::FGMOutputs::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name, ISerializationData* data) -> FGMOutputs*
{
	auto serial = dynamic_cast<FGMSerializationData*>(data);

	if (!serial)
		throw std::invalid_argument("FGM Outputs: Incomplete initialization");

	auto outputs = dynamic_cast<const WISE::ProjectProto::Project_Outputs*>(&proto);

	if (!outputs)
	{
		if (valid)
			/// <summary>
			/// The object passed as an output is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.Project.Outputs", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("FGMOutputs: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}

	for (int i = 0; i < outputs->summaries_size(); i++)
	{
		auto proto = outputs->summaries(i);
		if (proto.version() != 1) {
			if (valid)
				/// <summary>
				/// The object version is not supported. The summary output is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.Project.Outputs.SummaryOutput", strprintf("summaries[%d]", i), validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(proto.version()));
			weak_assert(0);
			throw std::invalid_argument("StatsOutputs: Version is invalid");
		}

		auto vt2 = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.Outputs.SummaryOutput", strprintf("seasonal[%d]", i));
		auto v2 = vt2.lock();

		SummaryOutput output;
		output.scenarioName = proto.scenarioname();
		if (!output.scenarioName.length()) {
			if (v2)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v2->add_child_validation("string", "scenarioName", validation::error_level::SEVERE, validation::id::missing_name, output.scenarioName);
		}

		output.filename = proto.filename();
		if (!output.filename.length()) {
			if (v2)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				v2->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::missing_filename, output.filename);
		}

		output.shouldStream = proto.has_streamoutput() && proto.streamoutput().value();
		if (proto.has_metadata())
		{
			if (proto.metadata().has_applicationname())
				output.applicationName = proto.metadata().applicationname().value();
			else
				output.applicationName = false;
			if (proto.metadata().has_geodata())
				output.geoData = proto.metadata().geodata().value();
			else
				output.geoData = false;
			if (proto.metadata().has_assetdata())
				output.assetData = proto.metadata().assetdata().value();
			else
				output.assetData = false;
			if (proto.metadata().has_scnearioname())
				output.outputScenarioName = proto.metadata().scnearioname().value();
			else
				output.outputScenarioName = false;
			if (proto.metadata().has_scenariocomments())
				output.scenarioComments = proto.metadata().scenariocomments().value();
			else
				output.scenarioComments = false;
			if (proto.metadata().has_inputs())
				output.inputs = proto.metadata().inputs().value();
			else
				output.inputs = false;
			if (proto.metadata().has_landscape())
				output.landscape = proto.metadata().landscape().value();
			else
				output.landscape = false;
			if (proto.metadata().has_fbppatches())
				output.fbpPatches = proto.metadata().fbppatches().value();
			else
				output.fbpPatches = false;
			if (proto.metadata().has_wxpatches())
				output.wxPatches = proto.metadata().wxpatches().value();
			else
				output.wxPatches = false;
			if (proto.metadata().has_ignitions())
				output.ignitions = proto.metadata().ignitions().value();
			else
				output.ignitions = false;
			if (proto.metadata().has_wxstreams())
				output.wxStreams = proto.metadata().wxstreams().value();
			else
				output.wxStreams = false;
			if (proto.metadata().has_fbp())
				output.fbp = proto.metadata().fbp().value();
			else
				output.fbp = false;
			if (proto.metadata().has_wxdata())
				output.wxData = proto.metadata().wxdata().value();
			else
				output.wxData = false;
		}

		summaries.push_back(output);
	}

	for (int i = 0; i < outputs->stats_size(); i++)
	{
		auto proto = outputs->stats(i);
		if (proto.version() != 1) {
			if (valid)
				/// <summary>
				/// The object version is not supported. The stats output is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.Project.Outputs.StatsOutput", strprintf("stats[%d]", i), validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(proto.version()));
			weak_assert(0);
			throw std::invalid_argument("StatsOutputs: Unknown version");
		}

		/// <summary>
		/// Child validations for a stats export.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.Outputs.StatsOutput", strprintf("stats[%d]", i));
		auto statsValid = vt.lock();

		stats::StatsEntry entry;
		if (proto.weatherSource_case() == WISE::ProjectProto::Project_Outputs_StatsOutput::WeatherSourceCase::WEATHERSOURCE_NOT_SET)
		{
			entry.use_stream = false;
			entry.stream_idx = (std::uint32_t)-1;
		}
		else if (proto.weatherSource_case() == WISE::ProjectProto::Project_Outputs_StatsOutput::WeatherSourceCase::kStreamIndex)
		{
			entry.use_stream = true;
			entry.stream_idx = proto.streamindex();
		}
		else if (proto.weatherSource_case() == WISE::ProjectProto::Project_Outputs_StatsOutput::WeatherSourceCase::kStreamName)
		{
			entry.use_stream = true;
			entry.streamName = proto.streamname();
			if (!entry.streamName.length()) {
				if (statsValid)
					/// <summary>
					/// The scenario's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					statsValid->add_child_validation("string", "streamName", validation::error_level::SEVERE, validation::id::missing_name, entry.streamName);
			}
		}
		else
		{
			CCoordinateConverter& convert(m_project->m_CoorConverter);

			entry.use_stream = false;
			if (proto.has_location())
			{
				GeoPoint geo(proto.location());
				geo.setStoredUnits(GeoPoly::UTM);
				geo.setConverter([&convert](std::uint8_t type, double x, double y, double z) -> std::tuple<double, double, double>
					{
						XY_Point loc = convert.start()
							.fromPoints(x, y, z)
							.asLatLon()
							.endInUTM()
							.to2DPoint();
						return std::make_tuple(loc.x, loc.y, 0.0);
					});

				double resolution;
				double xll, yll;
				m_project->GetResolution(&resolution);
				m_project->GetXLLCorner(&xll);
				m_project->GetYLLCorner(&yll);
				entry.location.x = (geo.getPoint(statsValid, "location.x").x - xll) / resolution;
				entry.location.y = (geo.getPoint(statsValid, "location.y").y - yll) / resolution;
			}
			else {
				entry.use_stream = true;
				entry.stream_idx = 0;
			}
		}

HSS_PRAGMA_WARNING_PUSH
HSS_PRAGMA_GCC(GCC diagnostic ignored "-Wdeprecated-declarations")
HSS_PRAGMA_SUPPRESS_MSVC_WARNING(4996)
		for (auto ii = 0; ii < proto.columns_size(); ii++)
		{
HSS_PRAGMA_SUPPRESS_MSVC_WARNING(4996)
			const auto j = proto.columns(ii);
			entry.columns.push_back(j);
		}
HSS_PRAGMA_WARNING_POP
		for (auto ii = 0; ii < proto.globalcolumns_size(); ii++)
		{
			const auto j = proto.globalcolumns(ii);
			if (bimapContains(statStatMap, j))
				entry.columns.push_back(statStatMap.left.at(j));
		}
// ***** need to make sure that the columns aren't empty

		stats::StatsFileType filetype;
		switch (proto.filetype())
		{
		case WISE::ProjectProto::Project_Outputs_StatsOutput::CSV:
			filetype = stats::StatsFileType::COMMA_SEPARATED_VALUE;
			break;
		case WISE::ProjectProto::Project_Outputs_StatsOutput::JSON_COLUMN:
			filetype = stats::StatsFileType::JSON_COLUMN;
			break;
		case WISE::ProjectProto::Project_Outputs_StatsOutput::JSON_ROW:
			filetype = stats::StatsFileType::JSON_ROW;
			break;
		default:
			if (boost::iends_with(proto.filename(), ".json"))
				filetype = stats::StatsFileType::JSON_ROW;
			else
				filetype = stats::StatsFileType::COMMA_SEPARATED_VALUE;
			break;
		}

		//only combined files for JSON exports
		if (filetype == stats::StatsFileType::JSON_COLUMN || filetype == stats::StatsFileType::JSON_ROW)
		{
			//check to see if another output with the same filename and scenarioName already exists
			std::vector<StatsOutputs>::iterator it = std::find_if(stats.begin(), stats.end(), [&proto](const StatsOutputs& o) {
					return boost::iequals(o.filename, proto.filename()) && boost::iequals(o.scenarioName, proto.scenarioname());
				});
			if (it == stats.end())
			{
				StatsOutputs so;
				so.scenarioName = proto.scenarioname();
				if (!so.scenarioName.length()) {
					if (statsValid)
						/// <summary>
						/// The scenario's name (to identify the output) is missing.
						/// </summary>
						/// <type>user</type>
						statsValid->add_child_validation("string", "scenarioName", validation::error_level::SEVERE, validation::id::missing_name, so.scenarioName);
				}
				so.filename = proto.filename();
				if (!so.filename.length()) {
					if (statsValid)
						/// <summary>
						/// The scenario's name (to identify the output) is missing.
						/// </summary>
						/// <type>user</type>
						statsValid->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::missing_filename, so.filename);
				}
				so.fileType = filetype;
				if (proto.has_discretizedoptions()) {
					auto dopt = proto.discretizedoptions();
					if (dopt.version() != 1) {
						if (statsValid)
							/// <summary>
							/// The object version is not supported. The grid output is not supported by this version of Prometheus.
							/// </summary>
							/// <type>user</type>
							statsValid->add_child_validation("WISE.ProjectProto.Project.Outputs.DiscretizedStatsOptions", strprintf("grids[%d]", i), validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(proto.version()));
						weak_assert(0);
						throw std::invalid_argument("DiscretizedStatsOptions: Unknown version");
					}
					so.discretize = (std::uint16_t)dopt.discretize();
					if ((so.discretize < 1) || (so.discretize > 1000))
					{
						//				m_loadWarning = "Warning: WISE.ProjectProto.Project.Outputs.GridOutput: Invalid specified discretization value";
						weak_assert(0);
						if (statsValid) {
							/// <summary>
							/// The scenario's override FMC value is invalid.
							/// </summary>
							/// <type>user</type>
							auto vt2 = validation::conditional_make_object(statsValid, "WISE.ProjectProto.Project.Outputs.DiscretizedStatsOptions", "discretizedOptions");
							auto dValid = vt2.lock();

							dValid->add_child_validation("uint32", "discretize", validation::error_level::WARNING, validation::id::value_invalid, std::to_string(so.discretize), { false, 0.0 }, { true, 1000.0 });
						}
						so.discretize = 1000;
					}
				}
				so.shouldStream = proto.has_streamoutput() && proto.streamoutput().value();
				so.entries.push_back(entry);
				stats.push_back(std::move(so));
			}
			else
				it->entries.push_back(entry);
		}
		else
		{
			StatsOutputs so;
			so.scenarioName = proto.scenarioname();
			if (!so.scenarioName.length()) {
				if (statsValid)
					/// <summary>
					/// The scenario's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					statsValid->add_child_validation("string", "scenarioName", validation::error_level::SEVERE, validation::id::missing_name, so.scenarioName);
			}
			so.filename = proto.filename();
			if (!so.filename.length()) {
				if (statsValid)
					/// <summary>
					/// The scenario's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					statsValid->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::missing_filename, so.filename);
			}
			so.fileType = filetype;
			if (proto.has_discretizedoptions()) {
				auto dopt = proto.discretizedoptions();
				if (dopt.version() != 1) {
					if (statsValid)
						/// <summary>
						/// The object version is not supported. The grid output is not supported by this version of Prometheus.
						/// </summary>
						/// <type>user</type>
						statsValid->add_child_validation("WISE.ProjectProto.Project.Outputs.DiscretizedStatsOptions", strprintf("grids[%d]", i), validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(proto.version()));
					weak_assert(0);
					throw std::invalid_argument("DiscretizedStatsOptions: Unknown version");
				}
				so.discretize = (std::uint16_t)dopt.discretize();
				if ((so.discretize < 1) || (so.discretize > 1000))
				{
					//				m_loadWarning = "Warning: WISE.ProjectProto.Project.Outputs.GridOutput: Invalid specified discretization value";
					weak_assert(0);
					if (statsValid) {
						/// <summary>
						/// The scenario's override FMC value is invalid.
						/// </summary>
						/// <type>user</type>
						auto vt2 = validation::conditional_make_object(statsValid, "WISE.ProjectProto.Project.Outputs.DiscretizedStatsOptions", "discretizedOptions");
						auto dValid = vt2.lock();

						dValid->add_child_validation("uint32", "discretize", validation::error_level::WARNING, validation::id::value_invalid, std::to_string(so.discretize), { false, 0.0 }, { true, 1000.0 });
					}
					so.discretize = 1000;
				}
			}
			so.shouldStream = proto.has_streamoutput() && proto.streamoutput().value();
			so.entries.push_back(entry);
			stats.push_back(std::move(so));
		}
	}

	for (int i = 0; i < outputs->vectors_size(); i++)
	{
		auto proto1 = outputs->vectors(i);
		if (proto1.version() != 1) {
			if (valid)
				/// <summary>
				/// The object version is not supported. The vector output is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.Project.Outputs.VectorOutput", strprintf("vectors[%d]", i), validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(proto1.version()));
			weak_assert(0);
			throw std::invalid_argument("StatsOutputs: Unknown version");
		}

		/// <summary>
		/// Child validations for vector outputs.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.Outputs.VectorOutput", strprintf("vectors[%d]", i));
		auto vectorValid = vt.lock();

		VectorOutput vector;
		vector.scenarioName = proto1.scenarioname();
		if (!vector.scenarioName.length()) {
			if (vectorValid)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				vectorValid->add_child_validation("string", "scenarioName", validation::error_level::SEVERE, validation::id::missing_name, vector.scenarioName);
		}
		for (auto& f : proto1.subscenarioname())
		{
			//remove empty strings
			if (f.value().length() > 0)
				vector.subScenarioNames.push_back(f.value());
		}
		vector.filename = proto1.filename();
		if (!vector.filename.length()) {
			if (vectorValid)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				vectorValid->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::missing_filename, vector.filename);
		}
		if (proto1.has_coverage_name())
			vector.coverageName = proto1.coverage_name().value();
		vector.multiplePerimeters = proto1.multipleperimeters();
		vector.removeIslands = proto1.removeislands();
		vector.mergeContacting = proto1.mergecontacting();
		vector.activePerimeters = proto1.activeperimeters();
		vector.shouldStream = proto1.has_streamoutput() && proto1.streamoutput().value();
		if (proto1.has_metadata())
		{
			vector.applicationVersion = proto1.metadata().applicationversion();
			vector.outputScenarioName = proto1.metadata().scenarioname();
			vector.jobName = proto1.metadata().jobname();
			vector.ignitionName = proto1.metadata().ignitionname();
			vector.simulationDate = proto1.metadata().simulationdate();
			vector.fireSize = proto1.metadata().firesize();
			vector.perimeterTotal = proto1.metadata().perimetertotal();
			vector.perimeterActive = proto1.metadata().perimeteractive();
			vector.wxValues = proto1.metadata().wxvalues();
			vector.fwiValues = proto1.metadata().fwivalues();
			vector.ignitionLocation = proto1.metadata().has_ignitionlocation() && proto1.metadata().ignitionlocation().value();
			vector.ignitionAttributes = proto1.metadata().has_ignitionattributes() && proto1.metadata().ignitionattributes().value();
			vector.maxBurnDistance =  proto1.metadata().has_maxburndistance() && proto1.metadata().maxburndistance().value();
			vector.assetArrivalTime = proto1.metadata().has_assetarrivaltime() && proto1.metadata().assetarrivaltime().value();
			vector.assetCount = proto1.metadata().has_assetarrivalcount() && proto1.metadata().assetarrivalcount().value();
			vector.identifyFinalPerimeter = proto1.metadata().has_identifyfinalperimeter() && proto1.metadata().identifyfinalperimeter().value();
			vector.simulationStatus = proto1.metadata().has_simulationstatus() && proto1.metadata().simulationstatus().value();

			if (proto1.metadata().area_case() == WISE::ProjectProto::Project_Outputs_VectorOutput_MetadataOutput::kAreaUnits)
				vector.areaUnits = proto1.metadata().areaunits();
			if (proto1.metadata().perimeter_case() == WISE::ProjectProto::Project_Outputs_VectorOutput_MetadataOutput::kPerimeterUnits)
				vector.perimeterUnits = proto1.metadata().perimeterunits();
		}
		if (proto1.has_perimetertime())
		{
			/// <summary>
			/// Child validations for perimeter times.
			/// </summary>
			auto vt2 = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.Outputs.VectorOutput.PerimeterTime", "perimeterTime");
			auto vv2 = vt2.lock();

			auto time = HSS_Time::Serialization::TimeSerializer().deserializeTime(proto1.perimetertime().starttime(), serial->m_timeManager, vv2, "startTime");
			vector.startTime = WTime(*time);
			delete time;
			time = HSS_Time::Serialization::TimeSerializer().deserializeTime(proto1.perimetertime().endtime(), serial->m_timeManager, vv2, "endTime");
			vector.endTime = WTime(*time);

			if (vector.startTime.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
				if (vv2)
					/// <summary>
					/// The end time contains seconds.
					/// </summary>
					/// <type>user</type>
					vv2->add_child_validation("HSS.Times.WTime", "endTime", validation::error_level::WARNING, validation::id::time_invalid, vector.startTime.ToString(WTIME_FORMAT_STRING_ISO8601), "Seconds will be purged.");
				vector.startTime.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
			}
			if (vector.endTime.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
				if (vv2)
					/// <summary>
					/// The end time contains seconds.
					/// </summary>
					/// <type>user</type>
					vv2->add_child_validation("HSS.Times.WTime", "endTime", validation::error_level::WARNING, validation::id::time_invalid, vector.endTime.ToString(WTIME_FORMAT_STRING_ISO8601), "Seconds will be purged.");
				vector.endTime.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
			}

			if ((vectorValid) && (vector.startTime > vector.endTime)) {
				/// <summary>
				/// The scenario start time occurs after the end time.
				/// </summary>
				/// <type>user</type>
				vv2->add_child_validation("HSS.Times.WTime", { "startTime", "endTime" }, validation::error_level::WARNING, validation::id::time_invalid, { vector.startTime.ToString(WTIME_FORMAT_STRING_ISO8601), vector.endTime.ToString(WTIME_FORMAT_STRING_ISO8601) });
			}
			delete time;
		}

		for (auto& f : proto1.perimetertimeoverride())
		{
			/// <summary>
			/// Child validations for perimeter times.
			/// </summary>
			auto vt2 = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.Outputs.VectorOutput.PerimeterTimeOverride", "perimeterTimeOverride");
			auto vv2 = vt2.lock();

			if (f.subscenarioname().length() > 0 && (f.has_endtime() || f.has_starttime()))
			{
				WTime* start = nullptr;
				if (f.has_starttime())
					start = HSS_Time::Serialization::TimeSerializer().deserializeTime(f.starttime(), serial->m_timeManager, vv2, "startTime");
				WTime* end = nullptr;
				if (f.has_endtime())
					end = HSS_Time::Serialization::TimeSerializer().deserializeTime(f.endtime(), serial->m_timeManager, vv2, "endTime");
				//if either the start or end time weren't overriden, use the regular start/end time which will be 0 if it wasn't set

				WTime realStart(start == nullptr ? vector.startTime : *start), realEnd(end == nullptr ? vector.endTime : *end);

				if (realStart.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
					if (vv2)
						/// <summary>
						/// The end time contains seconds.
						/// </summary>
						/// <type>user</type>
						vv2->add_child_validation("HSS.Times.WTime", "endTime", validation::error_level::WARNING, validation::id::time_invalid, realStart.ToString(WTIME_FORMAT_STRING_ISO8601), "Seconds will be purged.");
					realStart.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				}
				if (realEnd.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
					if (vv2)
						/// <summary>
						/// The end time contains seconds.
						/// </summary>
						/// <type>user</type>
						vv2->add_child_validation("HSS.Times.WTime", "endTime", validation::error_level::WARNING, validation::id::time_invalid, realEnd.ToString(WTIME_FORMAT_STRING_ISO8601), "Seconds will be purged.");
					realEnd.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				}

				if ((vv2) && (realStart > realEnd)) {
					/// <summary>
					/// The scenario start time occurs after the end time.
					/// </summary>
					/// <type>user</type>
					vv2->add_child_validation("HSS.Times.WTime", { "startTime", "endTime" }, validation::error_level::WARNING, validation::id::time_invalid, { vector.startTime.ToString(WTIME_FORMAT_STRING_ISO8601), vector.endTime.ToString(WTIME_FORMAT_STRING_ISO8601) });
				}
				vector.timeOverrides.emplace_back(VectorOutputTimeOverride(f.subscenarioname(), realStart, realEnd));
				if (start)
					delete start;
				if (end)
					delete end;
			}
		}

		vectors.push_back(vector);
	}

	for (int i = 0; i < outputs->fuelgrids_size(); i++) {
		auto proto1 = outputs->fuelgrids(i);
		if (proto1.version() != 1) {
			if (valid)
				/// <summary>
				/// The object version is not supported. The grid output is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.Project.Outputs.FuelGridOutput", strprintf("fuelgrids[%d]", i), validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(proto1.version()));
			weak_assert(0);
			throw std::invalid_argument("StatsOutputs: Unknown version");
		}

		/// <summary>
		/// Child validations for grid outputs.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.Outputs.GridOutput", strprintf("grids[%d]", i));
		auto gridValid = vt.lock();
		FuelGridOutput go;

		go.filename = proto1.filename();
		if (!go.filename.length()) {
			if (gridValid)
				/// <summary>
				/// The scenario's filename (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				gridValid->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::missing_filename, go.filename);
		}
		go.scenarioName = proto1.scenarioname();
		if (!go.scenarioName.length()) {	// we will allow this to export the project's fuel grid
			bool found = false;
			for (int ii = 0; ii < fuelgrids.size(); ii++) {
				if (fuelgrids[ii].scenarioName.empty()) {
					found = true;
					break;
				}
			}
			if (found) {	// we only need to export one project fuel grid
				if (gridValid)
					/// <summary>
					/// The scenario's name (to identify the output) is missing.
					/// </summary>
					/// <type>user</type>
					gridValid->add_child_validation("string", "scenarioName", validation::error_level::SEVERE, validation::id::missing_name, go.scenarioName);
			}
		}
		if (proto1.has_subscenarioname())
			go.subScenarioName = proto1.subscenarioname().value();
		go.shouldStream = proto1.has_streamoutput() && proto1.streamoutput().value();
		if (proto1.has_coverage_name())
			go.coverageName = proto1.coverage_name().value();
		switch (proto1.compression())
		{
		case WISE::ProjectProto::Project_Outputs::JPEG:
			go.compression = GDALExporter::CompressionType::JPEG;
			break;
		case WISE::ProjectProto::Project_Outputs::LZW:
			go.compression = GDALExporter::CompressionType::LZW;
			break;
		case WISE::ProjectProto::Project_Outputs::PACKBITS:
			go.compression = GDALExporter::CompressionType::PACKBITS;
			break;
		case WISE::ProjectProto::Project_Outputs::DEFLATE:
			go.compression = GDALExporter::CompressionType::DEFLATE;
			break;
		case WISE::ProjectProto::Project_Outputs::CCITTRLE:
			go.compression = GDALExporter::CompressionType::CCITTRLE;
			break;
		case WISE::ProjectProto::Project_Outputs::CCITTFAX3:
			go.compression = GDALExporter::CompressionType::CCITTFAX3;
			break;
		case WISE::ProjectProto::Project_Outputs::CCITTFAX4:
			go.compression = GDALExporter::CompressionType::CCITTFAX4;
			break;
		case WISE::ProjectProto::Project_Outputs::LZMA:
			go.compression = GDALExporter::CompressionType::LZMA;
			break;
		case WISE::ProjectProto::Project_Outputs::ZSTD:
			go.compression = GDALExporter::CompressionType::ZSTD;
			break;
		case WISE::ProjectProto::Project_Outputs::LERC:
			go.compression = GDALExporter::CompressionType::LERC;
			break;
		case WISE::ProjectProto::Project_Outputs::LERC_DEFLATE:
			go.compression = GDALExporter::CompressionType::LERC_DEFLATE;
			break;
		case WISE::ProjectProto::Project_Outputs::LERC_ZSTD:
			go.compression = GDALExporter::CompressionType::LERC_ZSTD;
			break;
		case WISE::ProjectProto::Project_Outputs::WEBP:
			go.compression = GDALExporter::CompressionType::WEBP;
			break;
		default:
			go.compression = GDALExporter::CompressionType::NONE;
			break;
		}

		fuelgrids.push_back(std::move(go));
	}

	for (int i = 0; i < outputs->grids_size(); i++)
	{
		auto proto = outputs->grids(i);
		if (proto.version() != 1) {
			if (valid)
				/// <summary>
				/// The object version is not supported. The grid output is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.Project.Outputs.GridOutput", strprintf("grids[%d]", i), validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(proto.version()));
			weak_assert(0);
			throw std::invalid_argument("StatsOutputs: Unknown version");
		}

		/// <summary>
		/// Child validations for grid outputs.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.Outputs.GridOutput", strprintf("grids[%d]", i));
		auto gridValid = vt.lock();

		GridOutput go;
HSS_PRAGMA_WARNING_PUSH
HSS_PRAGMA_GCC(GCC diagnostic ignored "-Wdeprecated-declarations")
		if (proto.StatsTypes_case() == WISE::ProjectProto::Project_Outputs_GridOutput::kStatistics)
HSS_PRAGMA_SUPPRESS_MSVC_WARNING(4996)
			go.statistic = proto.statistics();
		else
		{
			if (bimapContains(gridStatMap, proto.globalstatistics()))
				go.statistic = gridStatMap.left.at(proto.globalstatistics());
			//the user passed an invalid statistic, skip it. should maybe add load warnings
			else
				continue;
		}
HSS_PRAGMA_WARNING_POP
		go.filename = proto.filename();
		if (!go.filename.length()) {
			if (gridValid)
				/// <summary>
				/// The scenario's filename (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				gridValid->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::missing_filename, go.filename);
		}
		if (proto.has_coverage_name())
			go.coverageName = proto.coverage_name().value();
		if (proto.has_assetname())
			go.assetName = proto.assetname().value();
		if (proto.has_assetindex())
			go.assetIndex = proto.assetindex().value();
		else
			go.assetIndex = -1;

		GridOutputs grid, *g0;
		grid.scenarioName = proto.scenarioname();
		if (!grid.scenarioName.length()) {
			if (gridValid)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				gridValid->add_child_validation("string", "scenarioName", validation::error_level::SEVERE, validation::id::missing_name, grid.scenarioName);
		}

		for (auto& f : proto.subscenarioname())
		{
			//remove empty strings
			if (f.value().length() > 0)
				grid.subScenarioNames.push_back(f.value());
		}
		for (auto& f : proto.subscenarioexporttimes())
		{
			auto vt2 = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.Outputs.GridOutput.TimeOverride", "subScenarioExportTimes");
			auto vv2 = vt2.lock();

			if (f.subscenarioname().length() > 0 && (f.has_time() || f.has_starttime()))
			{
				auto time = HSS_Time::Serialization::TimeSerializer().deserializeTime(f.time(), serial->m_timeManager, gridValid, f.subscenarioname());
				if (!time)
					time = new WTime((ULONGLONG)-1, serial->m_timeManager);

				auto starttime = HSS_Time::Serialization::TimeSerializer().deserializeTime(f.starttime(), serial->m_timeManager, gridValid, f.subscenarioname());
				if (!starttime)
					starttime = new WTime((ULONGLONG)-1, serial->m_timeManager);

				if (starttime->GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
					if (vv2)
						/// <summary>
						/// The end time contains seconds.
						/// </summary>
						/// <type>user</type>
						vv2->add_child_validation("HSS.Times.WTime", "startTime", validation::error_level::WARNING, validation::id::time_invalid, starttime->ToString(WTIME_FORMAT_STRING_ISO8601), "Seconds will be purged.");
					starttime->PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				}
				if (time->GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
					if (vv2)
						/// <summary>
						/// The end time contains seconds.
						/// </summary>
						/// <type>user</type>
						vv2->add_child_validation("HSS.Times.WTime", "time", validation::error_level::WARNING, validation::id::time_invalid, time->ToString(WTIME_FORMAT_STRING_ISO8601), "Seconds will be purged.");
					time->PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				}

				if ((vv2) && ((*starttime) > (*time))) {
					/// <summary>
					/// The scenario start time occurs after the end time.
					/// </summary>
					/// <type>user</type>
					vv2->add_child_validation("HSS.Times.WTime", { "startTime", "time" }, validation::error_level::WARNING, validation::id::time_invalid, { starttime->ToString(WTIME_FORMAT_STRING_ISO8601), time->ToString(WTIME_FORMAT_STRING_ISO8601) });
				}

				grid.timeOverrides.emplace_back(GridOutputTimeOverride(f.subscenarioname(), WTime(*starttime), WTime(*time)));
				delete starttime;
				delete time;
			}
		}
		auto time = HSS_Time::Serialization::TimeSerializer().deserializeTime(proto.startexporttime(), serial->m_timeManager, gridValid, "startExportTime");
		if (time) {
			grid.startExportTime = WTime(*time);
			delete time;
		} else {
			grid.startExportTime = WTime(0ULL, serial->m_timeManager);
		}
		time = HSS_Time::Serialization::TimeSerializer().deserializeTime(proto.exporttime(), serial->m_timeManager, gridValid, "exportTime");
		grid.exportTime = WTime(*time);
		delete time;

		if (grid.startExportTime.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
			if (gridValid)
				/// <summary>
				/// The end time contains seconds.
				/// </summary>
				/// <type>user</type>
				gridValid->add_child_validation("HSS.Times.WTime", "startExportTime", validation::error_level::WARNING, validation::id::time_invalid, grid.startExportTime.ToString(WTIME_FORMAT_STRING_ISO8601), "Seconds will be purged.");
			grid.startExportTime.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
		}
		if (grid.exportTime.GetMicroSeconds(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST)) {
			if (gridValid)
				/// <summary>
				/// The end time contains seconds.
				/// </summary>
				/// <type>user</type>
				gridValid->add_child_validation("HSS.Times.WTime", "exportTime", validation::error_level::WARNING, validation::id::time_invalid, grid.exportTime.ToString(WTIME_FORMAT_STRING_ISO8601), "Seconds will be purged.");
			grid.exportTime.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
		}

		if ((gridValid) && (grid.startExportTime > grid.exportTime)) {
			/// <summary>
			/// The scenario start time occurs after the end time.
			/// </summary>
			/// <type>user</type>
			gridValid->add_child_validation("HSS.Times.WTime", { "startExportTime", "exportTime" }, validation::error_level::WARNING, validation::id::time_invalid, { grid.startExportTime.ToString(WTIME_FORMAT_STRING_ISO8601), grid.exportTime.ToString(WTIME_FORMAT_STRING_ISO8601) });
		}

		grid.interpolation = proto.interpolation();
		if ((grid.interpolation == 4) && (proto.has_discretizedoptions())) {
			auto dopt = proto.discretizedoptions();
			if (dopt.version() != 1) {
				if (gridValid)
					/// <summary>
					/// The object version is not supported. The grid output is not supported by this version of Prometheus.
					/// </summary>
					/// <type>user</type>
					gridValid->add_child_validation("WISE.ProjectProto.Project.Outputs.DiscretizedStatsOptions", strprintf("grids[%d]", i), validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(proto.version()));
				weak_assert(0);
				throw std::invalid_argument("DiscretizedStatsOptions: Unknown version");
			}
			grid.discretize = (std::uint16_t)dopt.discretize();
			if ((grid.discretize < 1) || (grid.discretize > 1000))
			{
				//				m_loadWarning = "Warning: WISE.ProjectProto.Project.Outputs.GridOutput: Invalid specified discretization value";
				weak_assert(0);
				if (gridValid) {
					/// <summary>
					/// The scenario's override FMC value is invalid.
					/// </summary>
					/// <type>user</type>
					auto vt2 = validation::conditional_make_object(gridValid, "WISE.ProjectProto.Project.Outputs.DiscretizedStatsOptions", "discretizedOptions");
					auto dValid = vt2.lock();

					dValid->add_child_validation("uint32", "discretize", validation::error_level::WARNING, validation::id::value_invalid, std::to_string(grid.discretize), { false, 0.0 }, { true, 1000.0 });
				}
				grid.discretize = 1000;
			}
		}
		grid.shouldStream = proto.has_streamoutput() && proto.streamoutput().value();

		grid.outputNodata = proto.has_outputnodata() && proto.outputnodata().value();
		grid.minimizeOutput = proto.has_minimizeoutput() && proto.minimizeoutput().value();
		switch (proto.compression())
		{
		case WISE::ProjectProto::Project_Outputs::JPEG:
			grid.compression = GDALExporter::CompressionType::JPEG;
			break;
		case WISE::ProjectProto::Project_Outputs::LZW:
			grid.compression = GDALExporter::CompressionType::LZW;
			break;
		case WISE::ProjectProto::Project_Outputs::PACKBITS:
			grid.compression = GDALExporter::CompressionType::PACKBITS;
			break;
		case WISE::ProjectProto::Project_Outputs::DEFLATE:
			grid.compression = GDALExporter::CompressionType::DEFLATE;
			break;
		case WISE::ProjectProto::Project_Outputs::CCITTRLE:
			grid.compression = GDALExporter::CompressionType::CCITTRLE;
			break;
		case WISE::ProjectProto::Project_Outputs::CCITTFAX3:
			grid.compression = GDALExporter::CompressionType::CCITTFAX3;
			break;
		case WISE::ProjectProto::Project_Outputs::CCITTFAX4:
			grid.compression = GDALExporter::CompressionType::CCITTFAX4;
			break;
		case WISE::ProjectProto::Project_Outputs::LZMA:
			grid.compression = GDALExporter::CompressionType::LZMA;
			break;
		case WISE::ProjectProto::Project_Outputs::ZSTD:
			grid.compression = GDALExporter::CompressionType::ZSTD;
			break;
		case WISE::ProjectProto::Project_Outputs::LERC:
			grid.compression = GDALExporter::CompressionType::LERC;
			break;
		case WISE::ProjectProto::Project_Outputs::LERC_DEFLATE:
			grid.compression = GDALExporter::CompressionType::LERC_DEFLATE;
			break;
		case WISE::ProjectProto::Project_Outputs::LERC_ZSTD:
			grid.compression = GDALExporter::CompressionType::LERC_ZSTD;
			break;
		case WISE::ProjectProto::Project_Outputs::WEBP:
			grid.compression = GDALExporter::CompressionType::WEBP;
			break;
		default:
			grid.compression = GDALExporter::CompressionType::NONE;
			break;
		}

		if (proto.has_gridoptions()) {
			auto ge = proto.gridoptions();
			grid.gridOptionsSet = true;
			if (ge.OutputResolution_case() == WISE::ProjectProto::Project_Outputs_GridOutputOptions::OutputResolutionCase::kResolution)
				grid.resolution = ge.resolution();
			else {
				double resolution;
				m_project->GetResolution(&resolution);
				if (ge.OutputResolution_case() == WISE::ProjectProto::Project_Outputs_GridOutputOptions::OutputResolutionCase::kScale) {
					grid.scale = ge.scale();
					if (grid.scale < 0.0)
						grid.resolution = resolution / grid.scale.value();
					else if (grid.scale > 0.0)
						grid.resolution = resolution * grid.scale.value();
				}
				else {
					grid.useFuelMap = true;
				}
			}
			if (ge.OutputOrigin_case() == WISE::ProjectProto::Project_Outputs_GridOutputOptions::OutputOriginCase::kLocation)
				grid.origin = XY_Point(ge.location().point().x().value(), ge.location().point().y().value());
			else
				grid.outputOriginMode = ge.dynamicorigin();
		}

		if (g0 = findGridOutputMatch(grid, go.statistic))
			g0->outputs.push_back(std::move(go));
		else {
			grid.outputs.push_back(std::move(go));
			grids.push_back(std::move(grid));
		}
	}

	if (outputs->has_job())
	{
		/// <summary>
		/// Child validations for job outputs.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.Outputs.JobOutput", "job");
		auto jobValid = vt.lock();

		job.saveJob = outputs->job().has_savejob() && outputs->job().savejob().value();
		if (outputs->job().has_filename())
			job.filename = outputs->job().filename().value();
		if ((job.saveJob) && (jobValid) && (!job.filename.length())) {
			/// <summary>
			/// The job's filename (to identify the output) is missing.
			/// </summary>
			/// <type>user</type>
			jobValid->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::missing_filename, job.filename);
		}
	}

	for (int i = 0; i < outputs->assetstats_size(); i++)
	{
		auto asset = outputs->assetstats(i);
		if (asset.version() != 1) {
			if (valid)
				/// <summary>
				/// The object version is not supported. The grid output is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.Project.Outputs.AssetStatsOutput", strprintf("assetStats[%d]", i), validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(asset.version()));
			weak_assert(0);
			throw std::invalid_argument("AssetOutput: Unknown version");
		}

		/// <summary>
		/// Child validations for asset stat outputs.
		/// </summary>
		auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.Outputs.AssetStatsOutput", strprintf("assetStats[%d]", i));
		auto assetValid = vt.lock();

		AssetOutput entry;
		entry.filename = asset.filename();
		if (!entry.filename.length()) {
			if (assetValid)
				/// <summary>
				/// The scenario's filename (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				assetValid->add_child_validation("string", "filename", validation::error_level::SEVERE, validation::id::missing_filename, entry.filename);
		}
		entry.scenarioName = asset.scenarioname();
		if (!entry.scenarioName.length()) {
			if (assetValid)
				/// <summary>
				/// The scenario's name (to identify the output) is missing.
				/// </summary>
				/// <type>user</type>
				assetValid->add_child_validation("string", "scenarioName", validation::error_level::SEVERE, validation::id::missing_name, entry.scenarioName);
		}
		entry.shouldStream = asset.has_streamoutput() && asset.streamoutput().value();
		entry.criticalPath = asset.has_criticalpathembedded() && asset.criticalpathembedded().value();
		entry.criticalPathFilename = asset.criticalpathfilename();

		stats::StatsFileType filetype;
		switch (asset.filetype())
		{
		case WISE::ProjectProto::Project_Outputs_StatsOutput::CSV:
			entry.fileType = stats::StatsFileType::COMMA_SEPARATED_VALUE;
			break;
		case WISE::ProjectProto::Project_Outputs_StatsOutput::JSON_COLUMN:
			entry.fileType = stats::StatsFileType::JSON_COLUMN;
			break;
		case WISE::ProjectProto::Project_Outputs_StatsOutput::JSON_ROW:
			entry.fileType = stats::StatsFileType::JSON_ROW;
			break;
		default:
			if (boost::iends_with(asset.filename(), ".json"))
				entry.fileType = stats::StatsFileType::JSON_ROW;
			else
				entry.fileType = stats::StatsFileType::COMMA_SEPARATED_VALUE;
			break;
		}

		assetOutputs.push_back(std::move(entry));
	}

	return this;
}


auto Project::FGMOutputs::findGridOutputMatch(const GridOutputs& grid, std::int32_t statistic) const -> GridOutputs*
{
	USHORT mask = 0, emask;
	if ((statistic == PARA_BURNT) || (statistic == PARA_BURNT_MEAN))	// burn grids are on their own
		return nullptr;
	if ((statistic == PARA_CRITICAL_PATH) || (statistic == PARA_CRITICAL_PATH_MEAN))	// critical path grids are also on their own
		return nullptr;
	if (statistic < 0)				// entire map of stat's values, not limited to the bounds of the fire, on its own
		mask |= 1;
	else if (statistic <= 9)		// weather and FWI values
		mask |= 2;
	else if ((statistic < 20) || (statistic == 24) || (statistic == 25))
		mask |= 4;					// fire stat's
	else
		mask |= 8;					// other grids of FWI values
	for (auto& g0 : grids)
	{
		if (g0.scenarioName != grid.scenarioName)
			continue;
		if (g0.subScenarioNames != grid.subScenarioNames)
			continue;
		if (g0.startExportTime != grid.startExportTime)
			continue;
		if (g0.exportTime != grid.exportTime)
			continue;
		if (g0.interpolation != grid.interpolation)
			continue;
		if (grid.interpolation == 4) { // discretization mode
			if (g0.discretize != grid.discretize)
				continue;
		}
		if (g0.shouldStream != grid.shouldStream)
			continue;
		if (g0.outputNodata != grid.outputNodata)
			continue;
		if (g0.minimizeOutput != grid.minimizeOutput)
			continue;
		if (g0.resolution != grid.resolution)
			continue;

		if (g0.gridOptionsSet != grid.gridOptionsSet)
			continue;
		if (g0.originValid != grid.originValid)
			continue;
		if (g0.originValid) {
			if (!g0.origin.Equals(grid.origin))
				continue;
		} else {
			if (g0.outputOriginMode != grid.outputOriginMode)
				continue;
		}
		if (g0.scale != grid.scale)
			continue;
		if (g0.useFuelMap != grid.useFuelMap)
			continue;
		emask = 0;
		bool found = false;
		for (auto& g1 : g0.outputs) {
			if ((g1.statistic == PARA_BURNT) || (g1.statistic == PARA_BURNT_MEAN) || (g1.statistic == PARA_CRITICAL_PATH) || (g1.statistic == PARA_CRITICAL_PATH_MEAN)) {
				found = true;
				continue;
			}
		}
		if (found)
			continue;
		for (auto& g1 : g0.outputs)
		{
			if (g1.statistic < 0)			// entire map of stat's values, not limited to the bounds of the fire, on its own
				emask |= 1;
			else if (g1.statistic <= 9)		// weather and FWI values
				emask |= 2;
			else if ((g1.statistic < 20) || (g1.statistic == 24) || (g1.statistic == 25))
				emask |= 4;					// fire stat's
			else
				emask |= 8;					// other grids of FWI values
		}
		if (!(emask & mask))					// different group/category of stat's
			continue;
		return (GridOutputs*)&g0;
	}
	return nullptr;
}
