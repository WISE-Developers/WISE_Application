/**
 * WISE_Project: AssetStats.h
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

#include "cwfgmp_config.h"
#include "FGMOutputs.h"
#include "IJsonConvert.h"
#include "WTime.h"
#include "poly.h"
#include "CWFGM_Scenario.h"

#include <string>
#include <vector>
#include <map>
#include <functional>


class CWFGMPROJECT_EXPORT AssetArrival : public IJsonConvert {
public:
	virtual nlohmann::json toJson() override;
	virtual void fromJson(const nlohmann::json& /*obj*/) override { throw std::logic_error("Not implemented"); }

public:
	AssetArrival();
	AssetArrival(const AssetArrival& toCopy);
	explicit AssetArrival(AssetArrival&& toMove);
	~AssetArrival();
	AssetArrival& operator=(const AssetArrival& aa);

	HSS_Time::WTimeSpan arrivalTime;
	MinListTempl<CriticalPath> *criticalPath;		// left this as a pointer for now since it's cheaper to move that around than even the move operator for the object, just need to manage the pointer a bit in the various constructors, etc.
};


namespace Project
{
	class CWFGMPROJECT_EXPORT AssetGlobalStats : public IJsonConvert
	{
	public:
		virtual nlohmann::json toJson() override;
		virtual void fromJson(const nlohmann::json& /*obj*/) override { throw std::logic_error("Not implemented"); }

	public:
		///<summary>
		///The name of the asset.
		///</summary>
		std::string assetName;
		///<summary>
		///The geometry ID of the asset that was reached.
		///</summary>
		std::int32_t assetGeometryId;
		///<summary>
		///A list of arrival times to compute statistics from.
		///</summary>
		std::vector<AssetArrival> arrivalTimes;
		///<summary>
		///The number of times the asset was reached.
		///</summary>
		std::uint64_t arrivalCount{ 0 };
	};


	class CWFGMPROJECT_EXPORT AssetArrivalDetails : public IJsonConvert
	{
	public:
		virtual nlohmann::json toJson() override;
		virtual void fromJson(const nlohmann::json& /*obj*/) override { throw std::logic_error("Not implemented"); }

	public:
		///<summary>
		///The name of the asset that was reached.
		///</summary>
		std::string assetName;
		///<summary>
		///The index of the geometry in the asset shapefile that was reached.
		///</summary>
		std::uint64_t geographyIndex;
	};


	class CWFGMPROJECT_EXPORT AssetTimestepStats : public IJsonConvert
	{
	public:
		AssetTimestepStats(const HSS_Time::WTime& time) : timestep(0), datetime(time), elapsedTime(HSS_Time::WTimeSpan(0)), timestepDuration(HSS_Time::WTimeSpan(0)) { }
		AssetTimestepStats(const AssetTimestepStats&) = default;
		AssetTimestepStats(AssetTimestepStats&&) = default;
		AssetTimestepStats& operator=(const AssetTimestepStats&) = default;

		virtual nlohmann::json toJson() override;
		virtual void fromJson(const nlohmann::json& /*obj*/) override { throw std::logic_error("Not implemented"); }

	public:
		///<summary>
		///The timestep index in the simulation.
		///</summary>
		std::uint32_t timestep;
		///<summary>
		///The simulation time that the asset was reached.
		///</summary>
		HSS_Time::WTime datetime;
		///<summary>
		///The amount of time that has elapsed since the simulation start.
		///</summary>
		HSS_Time::WTimeSpan elapsedTime;
		///<summary>
		///The length of time that was simulated in this timestep.
		///</summary>
		HSS_Time::WTimeSpan timestepDuration;
		///<summary>
		///The details of an asset that was reached in this timestep.
		///</summary>
		std::vector<AssetArrivalDetails> assetArrivals;
	};


	class CWFGMPROJECT_EXPORT AssetScenarioStats : public IJsonConvert
	{
	public:
		virtual nlohmann::json toJson() override;
		virtual void fromJson(const nlohmann::json& /*obj*/) override { throw std::logic_error("Not implemented"); }

	public:
		std::string scenarioName;
		std::string stationName;
		std::vector<AssetTimestepStats> timestepStats;
	};


	class CWFGMPROJECT_EXPORT IgnitionDescription
	{
	public:
		XY_Point* points{ nullptr };
		std::size_t count{ 0 };
		int ignitionType{ 100 };
	};


	class CWFGMPROJECT_EXPORT AssetDescription : public IJsonConvert
	{
	public:
		virtual nlohmann::json toJson() override;
		virtual void fromJson(const nlohmann::json& /*obj*/) override { throw std::logic_error("Not implemented"); }

	public:
		typedef std::variant<std::string, double, std::int64_t, bool, std::monostate> JsonType;

	public:
		std::string assetName;
		std::int32_t assetGeometryId{ 0 };
		std::map<std::string, JsonType> attributes;
	};


	class CWFGMPROJECT_EXPORT AssetStatsCollection
	{
	public:
		AssetStatsCollection() = default;
		AssetStatsCollection(AssetStatsCollection&&) = default;
		AssetStatsCollection(const AssetStatsCollection&) = delete;
		AssetStatsCollection(AssetStatsCollection&) = delete;
		AssetStatsCollection& operator=(const AssetStatsCollection&) = delete;

		bool save(const std::string& filename, class CWFGMProject* project, const IgnitionDescription& ignition);

	public:
		///<summary>
		///Per-asset statistics for arrival time.
		///</summary>
		std::map<std::tuple<std::string, std::uint32_t>, AssetGlobalStats> assetStats;
		///<summary>
		///An array of asset arrival times for each timestep, per-sub-scenario.
		///</summary>
		std::map<std::string, AssetScenarioStats> timestepStats;
		///<summary>
		///A list of assets that are part of the simulation.
		///</summary>
		std::vector<AssetDescription> assetDescriptions;
		///<summary>
		///Arrival time stats that aren't specific to any asset.
		///</summary>
		AssetGlobalStats globalStats;
	};

};
