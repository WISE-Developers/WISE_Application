/**
 * WISE_Project: FGMOutputs.h
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
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <atomic>
#include <mutex>

#ifdef _MSC_VER
#include <afx.h>
#endif

#include "cwfgmp_config.h"
#include "WTime.h"
#include "ISerializeProto.h"
#include "CumulativeGridOutput.h"
#include "GDALExporter.h"
#include "points.h"
#include "AssetStats.h"
#include "cwfgmProject.pb.h"
#include "ScenarioCollection.h"

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 8)
#endif /* HSS_SHOULD_PRAGMA_PACK */

namespace Project
{
	struct CWFGMPROJECT_EXPORT SummaryOutput
	{
		SummaryOutput();
		SummaryOutput(const SummaryOutput& toCopy);
		~SummaryOutput();

		void cleanup();
		void makeCumulativeOutput();
		std::uint32_t cumulativeSize();
		void exportCumulative();

		std::string scenarioName;
		std::string filename;

		std::optional<bool> applicationName;
		std::optional<bool> geoData;
        std::optional<bool> assetData;
		std::optional<bool> outputScenarioName;
		std::optional<bool> scenarioComments;
		std::optional<bool> inputs;
		std::optional<bool> landscape;
		std::optional<bool> fbpPatches;
		std::optional<bool> wxPatches;
		std::optional<bool> ignitions;
		std::optional<bool> wxStreams;
		std::optional<bool> fbp;
		std::optional<bool> wxData;
		bool shouldStream = false;
		std::atomic_bool exported{ false };

		/**
		 * Holds a collection of outputs for sub-scenario outputs.
		 */
		struct SummaryCollection* m_collection;

#ifdef _MANAGED
		void* m_mutex;
#else
		std::mutex* m_mutex;
#endif

		void lock();
		void unlock();
	};


	struct VectorOutputTimeOverride
	{
		VectorOutputTimeOverride() : startTime(0, nullptr), endTime(0, nullptr) { }
		VectorOutputTimeOverride(const std::string& name, const HSS_Time::WTime& s, const HSS_Time::WTime& e) : subName(name), startTime(s), endTime(e) { }

		std::string subName;
		HSS_Time::WTime startTime;
		HSS_Time::WTime endTime;
	};


	struct CWFGMPROJECT_EXPORT VectorOutput
	{
		VectorOutput();
		VectorOutput(const VectorOutput& toCopy);
		VectorOutput(VectorOutput&& toTransfer);
		~VectorOutput();

		std::string scenarioName;
		std::vector<std::string> subScenarioNames;
		std::string filename;
		std::string coverageName;

		bool multiplePerimeters;
		bool removeIslands;
		bool mergeContacting;
		bool activePerimeters;

		std::optional<bool> applicationVersion;
		std::optional<bool> outputScenarioName;
		std::optional<bool> jobName;
		std::optional<bool> ignitionName;
		std::optional<bool> ignitionLocation;
		std::optional<bool> ignitionAttributes;
		std::optional<bool> maxBurnDistance;
		std::optional<bool> simulationDate;
		std::optional<bool> fireSize;
		std::optional<bool> perimeterTotal;
		std::optional<bool> perimeterActive;
		std::optional<bool> wxValues;
		std::optional<bool> fwiValues;
	    std::optional<bool> assetArrivalTime;
	    std::optional<bool> assetCount;
	    std::optional<bool> identifyFinalPerimeter;
		std::optional<bool> simulationStatus;
		std::optional<std::uint8_t> areaUnits;
		std::optional<std::uint8_t> perimeterUnits;

		HSS_Time::WTime startTime;
		HSS_Time::WTime endTime;
		bool shouldStream;
		volatile std::uint32_t exported;
		std::vector<VectorOutputTimeOverride> timeOverrides;

		std::mutex* m_mutex;

		void lock();
		void unlock();

		/// <summary>
		/// Get the start time for a given sub-scenario.
		/// </summary>
		/// <param name="subName">The name of the sub-scenario to get the start time for.</param>
		/// <returns>An overriden start time for the given sub-scenario. If the sub-scenario name is empty the default time will always be returned</returns>
		HSS_Time::WTime& getStartTime(const std::string& subName);
		/// <summary>
		/// Get the end time for a given sub-scenario.
		/// </summary>
		/// <param name="subName">The name of the sub-scenario to get the end time for.</param>
		/// <returns>An overriden end time for the given sub-scenario. If the sub-scenario name is empty the default time will always be returned</returns>
		HSS_Time::WTime& getEndTime(const std::string& subName);
	};


	struct CWFGMPROJECT_EXPORT FuelGridOutput {
		FuelGridOutput();
		FuelGridOutput(const FuelGridOutput& toCopy);
		FuelGridOutput& operator=(const FuelGridOutput&) = delete;
		FuelGridOutput(FuelGridOutput&& toMove);
		~FuelGridOutput();

		std::string scenarioName, subScenarioName;
		std::string filename;
		std::string coverageName;

		GDALExporter::CompressionType compression{ GDALExporter::CompressionType::NONE };
		std::atomic_bool file_exported{ false };
		bool shouldStream = false;
	};


	struct CWFGMPROJECT_EXPORT GridOutput
	{
		GridOutput();
		GridOutput(GridOutput& toCopy) = delete;
		GridOutput(const GridOutput& toCopy) = delete;
		GridOutput& operator=(const GridOutput&) = delete;
		GridOutput(GridOutput&& toMove);
		~GridOutput();

		void cleanup();

		std::string filename;
		std::int16_t statistic;
		std::string assetName;
		std::int32_t assetIndex = -1;
		double* m_d = nullptr;
		GDALExtras::CumulativeGridOutput<double>* m_darray = nullptr;
		std::string coverageName;

		std::atomic_bool checkPaths{ false };
		volatile std::uint64_t lastOutputCount = 0;

		std::mutex* m_mutex;

		void lock();
		void unlock();
	};


	struct GridOutputTimeOverride
	{
		GridOutputTimeOverride() : _startTime(0, nullptr), _endTime(0, nullptr) { }
		GridOutputTimeOverride(GridOutputTimeOverride&& toMove) : _startTime(toMove._startTime), _endTime(toMove._endTime), _subName(toMove._subName) { }
		GridOutputTimeOverride(GridOutputTimeOverride& toCopy) = delete;
		GridOutputTimeOverride(const std::string& name, const HSS_Time::WTime& st, const HSS_Time::WTime& t) : _subName(name), _startTime(st), _endTime(t) { }

		std::string _subName;
		HSS_Time::WTime _startTime, _endTime;
	};


	struct CWFGMPROJECT_EXPORT GridOutputs
	{
		GridOutputs() : startExportTime(0, nullptr), exportTime(0, nullptr) { }
		GridOutputs(GridOutputs&& toMove);
		GridOutputs(GridOutputs&) = delete;
		GridOutputs(const GridOutputs&) = delete;
		GridOutputs& operator=(const GridOutputs&) = delete;

		std::string scenarioName;
		std::vector<std::string> subScenarioNames;
		HSS_Time::WTime startExportTime;
		HSS_Time::WTime exportTime;
		std::int8_t interpolation = 0;
		std::uint16_t discretize = 10;
		bool shouldStream = false,
			outputNodata = false,
			minimizeOutput = false;
		bool gridOptionsSet = false;
		bool originValid = false;
		std::uint8_t outputOriginMode = (std::uint8_t)-1;	// from GridOutputOriginOptions.  Current values:
			// FUELMAP_ORIGIN = 0;         // use whatever origin is given by the fuel map, unchanged
			// PRECISE_PERIMETER = 1;      // regardless of fuel map grid origin and scale, start the grid export origin at precisely the left and lower edge intersection of the fire perimeter
			// MINIMIZE_PERIMETER = 2;     // from the fuel map origin and the export resolution, calculate the closest even multiple to where to start the grid
			// MINIMIZE_PERIMETER2 = 3;    // from the fuel map origin and the fuel map resolution, calculate the closest even multiple to where to start the egrid
			// CENTER_PERIMETER = 4;       // regardless of the fuel map grid origin and scale, use the center [(max_x - min_x) / 2], etc. so that there's an equal portion of subcell at each extent of the perimeter

		XY_Point origin;
		double resolution = -1.0;			// from GridOutputOptions.OutputResolution
		std::optional<double> scale;
		std::optional<bool> useFuelMap;

		std::atomic_bool file_exported{ false };
		std::vector<GridOutput> outputs;
		std::vector<GridOutputTimeOverride> timeOverrides;
		GDALExporter::CompressionType compression{ GDALExporter::CompressionType::NONE };

		/// <summary>
		/// Get the starting export time for a given sub-scenario.
		/// </summary>
		/// <param name="subName">The name of the sub-scenario to get the export time for.</param>
		/// <returns>An overriden export time for the given sub-scenario. If the sub-scenario name is empty the default time will always be returned</returns>
		HSS_Time::WTime& getStartExportTime(const std::string& subName);

		/// <summary>
		/// Get the (ending) export time for a given sub-scenario.
		/// </summary>
		/// <param name="subName">The name of the sub-scenario to get the export time for.</param>
		/// <returns>An overriden export time for the given sub-scenario. If the sub-scenario name is empty the default time will always be returned</returns>
		HSS_Time::WTime& getExportTime(const std::string& subName);
	};


    //stats related helper classes
    namespace stats
    {
    	enum class StatsFileType
    	{
    		COMMA_SEPARATED_VALUE = 1,
    		JSON_ROW = 2,
    		JSON_COLUMN = 3
    	};
    
    	struct CWFGMPROJECT_EXPORT StatsEntry
    	{
    		std::uint32_t stream_idx{ (std::uint32_t)-1 };
    		std::string streamName;
    		XY_Point location;
    		std::vector<int> columns;
    		bool use_stream{ true };
    	};
    }


	struct CWFGMPROJECT_EXPORT StatsOutputs
	{
		StatsOutputs() = default;
		StatsOutputs(StatsOutputs&& toMove);
		StatsOutputs(StatsOutputs&) = delete;
		StatsOutputs(const StatsOutputs&) = delete;
		StatsOutputs& operator=(const StatsOutputs&) = delete;

		std::string scenarioName;
		std::string filename;
	    stats::StatsFileType fileType;
		std::uint16_t discretize = 10;
		std::vector<stats::StatsEntry> entries;
		bool shouldStream = false;

		std::atomic_bool file_exported{ false };
	};


    struct CWFGMPROJECT_EXPORT AssetOutput
    {
    	AssetOutput();
    	AssetOutput(AssetOutput&& toMove);
    	~AssetOutput();
    
    	AssetOutput(AssetOutput&) = delete;
    	AssetOutput(const AssetOutput&) = delete;
    	AssetOutput& operator=(const AssetOutput&) = delete;
    
    	std::string scenarioName;
    	std::string filename;
		std::string criticalPathFilename;
		stats::StatsFileType fileType;
    	bool shouldStream = false;
		bool criticalPath = false;
		volatile std::uint64_t exportCount = 0;
    	AssetStatsCollection* m_statBuilder;
    
    	std::atomic_bool file_exported{ false };
    
    #ifdef _MANAGED
    	void* m_mutex;
    #else
    	std::mutex* m_mutex;
    #endif
    	
    	void cleanup();
    	void lock();
    	void unlock();
    };


	struct JobOutput
	{
		JobOutput() : saveJob(false) { }

		bool saveJob;
		std::string filename;
	};


	class FGMSerializationData : public ISerializationData
	{
	public:
		HSS_Time::WTimeManager* m_timeManager;
	};


	class FGMOutputs : public ISerializeProto
	{
	public:
		FGMOutputs(class CWFGMProject* project) : m_project(project) { };
		FGMOutputs(GridOutputs&) = delete;
		FGMOutputs(const GridOutputs&) = delete;
		FGMOutputs& operator=(const GridOutputs&) = delete;

		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual WISE::ProjectProto::Project_Outputs* serialize(const SerializeProtoOptions& options) override;
		virtual FGMOutputs* deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		virtual FGMOutputs* deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name, ISerializationData* data) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	public:
		std::vector<SummaryOutput> summaries;
		std::vector<VectorOutput> vectors;
		std::vector<GridOutputs> grids;
		std::vector<FuelGridOutput> fuelgrids;
		std::vector<StatsOutputs> stats;
	    std::vector<AssetOutput> assetOutputs;

		JobOutput job;

		GridOutputs* findGridOutputMatch(const GridOutputs& grid, std::int32_t statistic) const;

	private:
		class CWFGMProject* m_project = nullptr;
	};
};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif /* HSS_SHOULD_PRAGMA_PACK */
