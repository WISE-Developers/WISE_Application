/**
 * WISE_Project: AssetStats.cpp
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
#include "StdAfx.h"
#endif

#include "CWFGMProject.h"
#include "GeoJSONHelper.h"
#include "AssetStats.h"
#include "filesystem.hpp"
#include "str_printf.h"
#include <fstream>
#include "macros.h"

#define BOOST_ALL_NO_LIB
#define BOOST_NO_CXX98_BINDERS
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>


using json = nlohmann::json;
using namespace std::string_literals;
using namespace boost::accumulators;


AssetArrival::AssetArrival() {
	criticalPath = new MinListTempl<CriticalPath>();
}


AssetArrival::AssetArrival(const AssetArrival& toCopy) {
	arrivalTime = toCopy.arrivalTime;
	if (toCopy.criticalPath) {
		criticalPath = new MinListTempl<CriticalPath>();
		CriticalPath *pn = (CriticalPath *)toCopy.criticalPath->LH_Head();
		while (pn->LN_Succ()) {
			CriticalPath* cp = new CriticalPath(*pn);
			criticalPath->AddTail(cp);
			pn = pn->LN_Succ();
		}
	}
	else
		criticalPath = nullptr;
}


AssetArrival::AssetArrival(AssetArrival&& toMove) {
	arrivalTime = toMove.arrivalTime;
	criticalPath = toMove.criticalPath;
	toMove.criticalPath = nullptr;
}


AssetArrival& AssetArrival::operator=(const AssetArrival& aa) {
	arrivalTime = aa.arrivalTime;
	if (aa.criticalPath) {
		criticalPath = new MinListTempl<CriticalPath>();
		auto pn = aa.criticalPath->LH_Head();
		while (pn->LN_Succ()) {
			CriticalPath* cp = new CriticalPath(*pn);
			criticalPath->AddTail(cp);
			pn = pn->LN_Succ();
		}
	}
	else
		criticalPath = nullptr;
	return *this;
}

AssetArrival::~AssetArrival() {
	if (criticalPath) {
		CriticalPath* cp;
		while (cp = criticalPath->RemHead()) {
			delete cp;
		}
		delete criticalPath;
	}
}


bool Project::AssetStatsCollection::save(const std::string& filename, class CWFGMProject* project, const IgnitionDescription& ignition) {
	json js;
	js["type"s] = "FeatureCollection"s;
	js["features"s] = json::array();
	auto feature = HSS_MATH_HELPER::convertToGeoJSON(ignition.points, ignition.count, (HSS_MATH_HELPER::FeatureType)ignition.ignitionType, project);

	json properties = json::object();

	//global stats that aren't necessarily related to any asset
	if (globalStats.arrivalCount > 0)
	{
		accumulator_set<std::int64_t, features<tag::mean, tag::median, tag::min, tag::max>> acc;
		for (auto& time : globalStats.arrivalTimes)
			acc(time.arrivalTime.GetTotalSeconds());

		auto mean_ = boost::accumulators::mean(acc);
		properties["min_arrival_time"s] = boost::accumulators::min(acc);
		properties["max_arrival_time"s] = boost::accumulators::max(acc);
		properties["mean_arrival_time"s] = mean_;
		if (globalStats.arrivalCount < 3)
			properties["median_arrival_time"s] = mean_;
		else
			properties["median_arrival_time"s] = boost::accumulators::median(acc);
	}
	properties["arrival_count"s] = globalStats.arrivalCount;

	properties["scenarios"s] = json::array();
	for (auto& scenario : timestepStats)
	{
		properties["scenarios"s].push_back(scenario.second.toJson());
	}

	//skip outputting averages if there is only a single scenario
	if (timestepStats.size())
	{
		properties["assets"s] = json::array();
		for (auto& asset : assetStats)
		{
			properties["assets"s].push_back(asset.second.toJson());
		}
	}

	properties["asset_descriptions"s] = json::array();
	for (auto& asset : assetDescriptions)
	{
		properties["asset_descriptions"s].push_back(asset.toJson());
	}

	feature["properties"s] = properties;

	js["features"s].push_back(feature);

	//ensure that the path to the file exists
	fs::path path(filename);
	path = fs::absolute(path).parent_path();
	fs::create_directories(path);

	//open the file first so if it fails we don't have to do any of the other work
	std::ofstream writeFile;
	writeFile.open(filename);

	writeFile << js.dump(4, ' ', false, json::error_handler_t::relaxed);

	writeFile.close();

	return true;
}


json Project::AssetDescription::toJson()
{
	json retval;
	retval["asset_name"s] = assetName;
	retval["geometry_index"s] = assetGeometryId;
	for (auto& attr : attributes)
	{
		if (std::holds_alternative<std::string>(attr.second))
			retval[attr.first] = std::get<std::string>(attr.second);
		else if (std::holds_alternative<double>(attr.second))
			retval[attr.first] = std::get<double>(attr.second);
		else if (std::holds_alternative<std::int64_t>(attr.second))
			retval[attr.first] = std::get<std::int64_t>(attr.second);
		else if (std::holds_alternative<bool>(attr.second))
			retval[attr.first] = std::get<bool>(attr.second);
		else
			retval[attr.first] = nullptr;
	}

	return retval;
}


json Project::AssetScenarioStats::toJson()
{
	json retval;
	retval["scenario_name"s] = scenarioName;
	retval["station_name"s] = stationName;
	bool initialized = false;
	for (auto& timestep : timestepStats)
	{
		if (timestep.assetArrivals.size())
		{
			if (!initialized)
			{
				retval["timesteps"s] = json::array();
				initialized = true;
			}
			retval["timesteps"s].push_back(timestep.toJson());
		}
	}

	return retval;
}


json Project::AssetTimestepStats::toJson()
{
	json retval;
	retval["timestep"s] = timestep;
	retval["date_time"s] = datetime.ToString(WTIME_FORMAT_STRING_ISO8601);
	retval["elapsed_time"s] = elapsedTime.ToString(WTIME_FORMAT_STRING_ISO8601);
	retval["timestep_duration"s] = timestepDuration.ToString(WTIME_FORMAT_STRING_ISO8601);
	retval["asset_arrival"s] = json::array();
	for (auto& arrival : assetArrivals)
		retval["asset_arrival"s].push_back(arrival.toJson());

	return retval;
}


json Project::AssetArrivalDetails::toJson()
{
	json retval;
	retval["asset_name"s] = assetName;
	retval["geometry_index"s] = geographyIndex;

	return retval;
}


json Project::AssetGlobalStats::toJson()
{
	json retval;
	retval["asset_name"s] = assetName;
	retval["geometry_index"s] = assetGeometryId;
	if (arrivalCount > 0)
	{
		accumulator_set<std::int64_t, features<tag::mean, tag::median, tag::min, tag::max>> acc;
		for (auto& time : arrivalTimes)
			acc(time.arrivalTime.GetTotalSeconds());

		WTimeSpan minArrivalTime(boost::accumulators::min(acc));
		WTimeSpan maxArrivalTime(boost::accumulators::max(acc));
		WTimeSpan meanArrivalTime(boost::accumulators::mean(acc));

		retval["min_arrival_time"s] = minArrivalTime.ToString(WTIME_FORMAT_STRING_ISO8601);
		retval["max_arrival_time"s] = maxArrivalTime.ToString(WTIME_FORMAT_STRING_ISO8601);
		retval["mean_arrival_time"s] = meanArrivalTime.ToString(WTIME_FORMAT_STRING_ISO8601);
		if (arrivalCount < 3)
			retval["median_arrival_time"s] = meanArrivalTime.ToString(WTIME_FORMAT_STRING_ISO8601);
		else
		{
			WTimeSpan medianArrivalTime(boost::accumulators::median(acc));
			retval["median_arrival_time"s] = medianArrivalTime.ToString(WTIME_FORMAT_STRING_ISO8601);
		}

		bool paths = false;
		for (auto& time : arrivalTimes) {
			if ((time.criticalPath) && (time.criticalPath->GetCount())) {
				paths = true;
				break;
			}
		}
		if (paths) {
			retval["critical_paths"s] = json::array();
			for (auto& time : arrivalTimes) {
				retval["critical_paths"s].push_back(time.toJson());
			}
		}
	}
	retval["arrival_count"s] = arrivalCount;

	return retval;
}


json AssetArrival::toJson() {
	json retval;
	if (criticalPath) {
		retval["paths"] = json::array();
		CriticalPath *cp = (CriticalPath *)criticalPath->LH_Head();
		while (cp->LN_Succ()) {
			json f;
			f["features"] = json::array();
			auto poly = cp->LH_Head();
			ULONG cnt = 0;
			while (poly->LN_Succ()) {
				if (poly->NumPoints() == 1) {
					json p;
					p["type"s] = "Feature"s;

					json a;
					a["Name"s] = strprintf("Point%u", cnt);
					for (auto& attribute : poly->m_attributes) {
						if (std::holds_alternative<std::int32_t>(attribute.attributeValue))
							a[attribute.attributeName] = std::get<std::int32_t>(attribute.attributeValue);
						else if (std::holds_alternative<std::int64_t>(attribute.attributeValue))
							a[attribute.attributeName] = std::get<std::int64_t>(attribute.attributeValue);
						else if (std::holds_alternative<double>(attribute.attributeValue)) {
							double value = std::get<double>(attribute.attributeValue);
							if (!stricmp(attribute.attributeName.c_str(), "ROS"))
								value = ROUND_DECIMAL(value, 2);
							if (!stricmp(attribute.attributeName.c_str(), "CFB"))
								value = ROUND_DECIMAL(value, 4);
							if (!stricmp(attribute.attributeName.c_str(), "FI"))
								value = ROUND_DECIMAL(value, 2);
							if (!stricmp(attribute.attributeName.c_str(), "TFC"))
								value = ROUND_DECIMAL(value, 2);
							a[attribute.attributeName] = value;
						}
						else if (std::holds_alternative<std::string>(attribute.attributeValue))
							a[attribute.attributeName] = std::get<std::string>(attribute.attributeValue);
						else if (std::holds_alternative<GDALTime>(attribute.attributeValue)) {
							GDALTime time(std::get<GDALTime>(attribute.attributeValue));

#ifdef _MSC_VER
							struct tm _tm;
							gmtime_s(&_tm, &time.theTime);
#else
							struct tm _tm;
							gmtime_r(&time.theTime, &_tm);
#endif

							a[attribute.attributeName] = strprintf("%4d-%02d-%02dT%02d:%02d:%02d.%03d",
								_tm.tm_year + 1900,
								_tm.tm_mon + 1,
								_tm.tm_mday,
								_tm.tm_hour,
								_tm.tm_min,
								_tm.tm_sec,
								time.millisecs);
						}
					}
					p["properties"s] = a;

					json g;
					g["type"s] = "Point"s;
					g["coordinates"s] = json::array();
					auto point = poly->LH_Head();
					g["coordinates"s].push_back(ROUND_DECIMAL(point->x, 6));
					g["coordinates"s].push_back(ROUND_DECIMAL(point->y, 6));
					p["geometry"s] = g;

					f["features"s].push_back(p);
				}
				poly = poly->LN_Succ();
				cnt++;
			}
			retval["paths"s].push_back(f);
			cp = cp->LN_Succ();
		}
		}

	return retval;
}
