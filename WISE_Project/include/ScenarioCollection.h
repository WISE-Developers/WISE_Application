/**
 * WISE_Project: ScenarioCollection.h
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

#if !defined(AFX_SCENARIOCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
#define AFX_SCENARIOCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cwfgmp_config.h"

#include <vector>

#ifdef __MAPLAYER
#include <map>
#endif

#include "BurnPeriodOption.h"
#include "FireCollection.h"
#include "FuelCollection.h"
#include "GridFilterCollection.h"
#include "vectorcollection.h"
#include "assetcollection.h"
#include "targetcollection.h"
#include "WeatherCollection.h"
#include "FBPOptions.h"
#include "BasicScenarioOptions.h"
#include "CumulativeGridOutput.h"
#include "results.h"
#include "Dlgcnvt.h"
#include "CWFGM_Scenario.h"
#include "CWFGM_TemporalAttributeFilter.h"
#include "ISerializeProto.h"
#include "objectcache_mt.h"
#include "CWFGM_Scenario.h"
#include "CWFGM_WeatherGrid.h"

#include "projectScenario.pb.h"


namespace Project
{
	class CWFGMProject;
	class CWeatherGridFilter;
};
#include "WeatherGridManager.h"

namespace Project
{
	struct CWFGMPROJECT_EXPORT flags
	{
		union
		{
			ULONG value;
			struct
			{
				unsigned fuelgrid		: 1;	// 0
				unsigned fuelpatch		: 1;	// 1
				unsigned temporal		: 1;	// 2
				unsigned temporaltime   : 1;    // 3
				unsigned spatialWS		: 1;	// 4
				unsigned spatialWD		: 1;	// 5
				unsigned greenup		: 1;	// 6
				unsigned cbh			: 1;	// 7
				unsigned pc				: 1;	// 8
				unsigned df				: 1;	// 9
				unsigned pgc			: 1;	// 10
				unsigned wxpatch		: 1;	// 11
				unsigned ignitions		: 1;	// 12
				unsigned ignitiontime	: 1;	// 13
				unsigned primarywx		: 1;	// 14
				unsigned wxtimes		: 1;	// 15
				unsigned wxlocation		: 1;	// 16
				unsigned wxstation		: 1;	// 17
				unsigned wxdata			: 1;	// 18
				unsigned treeheight		: 1;	// 19
				unsigned fuelload		: 1;	// 20
				unsigned ovd			: 1;	//21
			};
		};
	};
};


namespace Project
{
	struct ScenarioSerializationData : public ISerializationData
	{
		const GridFilterCollection& gridFilterCollection;
		const VectorCollection& vectorCollection;
		const AssetCollection& assetCollection;
		const TargetCollection& targetCollection;
		const WeatherStationCollection& weatherCollection;
		const FireCollection& fireCollection;

		ScenarioSerializationData(const GridFilterCollection& gfc, const VectorCollection& vc, const AssetCollection &ac, const TargetCollection &tc,
			const WeatherStationCollection& wsc, const FireCollection& fc)
			: gridFilterCollection(gfc), vectorCollection(vc), assetCollection(ac), targetCollection(tc), weatherCollection(wsc), fireCollection(fc) { }
	};


	class VectorExportOptions;
	class ScenarioCollection;

	class CWFGMPROJECT_EXPORT Scenario : public MinNode, public ISerializeProto
	{
	public:
		constexpr static std::int16_t ERROR_MULTIPLE_WINDSPEED_GRIDS = -1;
		constexpr static std::int16_t ERROR_MULTIPLE_WINDDIRECTION_GRIDS = -2;
		constexpr static std::int16_t ERROR_SPATIAL_AND_TEMPORAL_MIXED = -3;
		constexpr static std::int16_t ERROR_WEATHER_PATCH_TIMES_NOT_OVERLAP = -4;
		constexpr static std::int16_t ERROR_WEATHER_GRID_TIMES_NOT_OVERLAP = -5;
		constexpr static std::int16_t ERROR_WEATHER_PATCH_AFTER_WEATHERGRID = -6;
		constexpr static std::int16_t ERROR_MULTIPLE_GREENUP_GRIDS = -7;
		constexpr static std::int16_t ERROR_MULTIPLE_CBH_GRIDS = -8;
		constexpr static std::int16_t ERROR_MULTIPLE_PC_GRIDS = -9;
		constexpr static std::int16_t ERROR_MULTIPLE_PDF_GRIDS = -10;
		constexpr static std::int16_t ERROR_MULTIPLE_PCG_GRIDS = -11;
		constexpr static std::int16_t ERROR_MULTIPLE_FBP_FUEL_GRIDS = -12;
		constexpr static std::int16_t ERROR_FUEL_PATCH_AFTER_FUELGRID = -13;
		constexpr static std::int16_t ERROR_WEATHER_STREAM_TIMES_NOT_SPAN = -14;
		constexpr static std::int16_t ERROR_MULTIPLE_WEATHER_STREAM_STATION = -15;
		constexpr static std::int16_t ERROR_MULTIPLE_WEATHER_STREAM_TOO_CLOSE = -16;
		constexpr static std::int16_t ERROR_MULTIPLE_WEATHER_STREAM_NO_PRIMARY = -17;
		constexpr static std::int16_t ERROR_NO_IGNITIONS = -18;
		constexpr static std::int16_t ERROR_IGNITION_START_TOO_LATE = -19;
		constexpr static std::int16_t ERROR_IGNITION_START_TOO_EARLY = -20;
		constexpr static std::int16_t ERROR_MULTIPLE_STREAMS_NO_INTERP = -21;
		constexpr static std::int16_t ERROR_NO_WEATHER_DATA = -22;
		constexpr static std::int16_t ERROR_MULTIPLE_TREEHEIGHT_GRIDS = -23;
		constexpr static std::int16_t ERROR_MULTIPLE_FUELLOAD_GRIDS = -24;
		constexpr static std::int16_t ERROR_MULTIPLE_STOCHATIC_SCENARIOS = -25;
		constexpr static std::int16_t SCENARIO_ERROR_MULTIPLE_STOCHATIC_SCENARIOS = -26;
		constexpr static std::int16_t ERROR_MULTIPLE_OVD_GRIDS = -27;

	public:
		HRESULT GetBurnedBox(const HSS_Time::WTime &time, XY_Rectangle *utm) const;
		HRESULT IsXYBurned(const XY_Point &pt, const HSS_Time::WTime &time, bool &burned) const;
		bool ExportScenarioInfo(FILE *fp);

		class IgnitionOptions : public RefNode<Fire>
		{
		public:
			IgnitionOptions() = default;

			DEVICE IgnitionOptions* LN_Succ() const { return (IgnitionOptions*)RefNode<Fire>::LN_Succ(); };
			DEVICE IgnitionOptions* LN_Pred() const { return (IgnitionOptions*)RefNode<Fire>::LN_Pred(); };

			std::string		m_name;											// the sub-scenario's name

			DECLARE_OBJECT_CACHE_MT(IgnitionOptions, IgnitionOptions);
		};

		// an empty, unusable scenario object - you can use the constructor to
									// either attach a CWFGM scenario to an existing ICWFGM_Scenario COM
									// object, or as a start to a completely new scenario
		Scenario(const ScenarioCollection *scenarioCollection, const HSS_Time::WTimeManager *timeManager, ICWFGM_GridEngine *grid);
		Scenario(const Project::Scenario &scenario, const HSS_Time::WTimeManager *timeManager, ICWFGM_GridEngine *grid, bool isChild);
		Scenario(const Project::Scenario& scenario, const HSS_Time::WTimeManager *timeManager, ICWFGM_GridEngine* grid, IgnitionOptions* ignition, bool isChild);
		~Scenario();

		LONG CheckFiltersAndPatches(flags in_flags, bool warnOnFireBeforeScenario = false);

		Project::Scenario *LN_Succ() const { return (Project::Scenario *)MinNode::LN_Succ(); };
		Project::Scenario *LN_Pred() const { return (Project::Scenario *)MinNode::LN_Pred(); };

	    ICWFGM_GridEngine *gridEngine() const;

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		google::protobuf::Message* serialize(const SerializeProtoOptions& options, ISerializationData* userData) override;
		Project::Scenario *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		Project::Scenario *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name, ISerializationData* userData) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
		
	public:
		// used to load/save from/to binary files
		bool AssignNewGrid(ICWFGM_GridEngine *oldgrid, ICWFGM_GridEngine *newgrid);

		class WSNode : public RefNode<Project::WeatherStream>
		{
		public:
		    WSNode(const HSS_Time::WTimeManager *manager) : m_valid(false), m_startTime(0, manager), m_endTime(0, manager), m_ignitionTime(0, manager) { }

			DEVICE WSNode* LN_Succ() const { return (WSNode*)RefNode<Project::WeatherStream>::LN_Succ(); };
			DEVICE WSNode* LN_Pred() const { return (WSNode*)RefNode<Project::WeatherStream>::LN_Pred(); };

			bool			m_valid;										// whether the rest of the data here is valid
			std::string		m_name;											// the sub-scenario's name
			HSS_Time::WTime m_startTime;									// an override for the scenario start time
			HSS_Time::WTime m_endTime;										// an override for the scenario end time
			HSS_Time::WTime m_ignitionTime;									// an override for the ignition start time
			std::optional<double> m_windDirection;							// an override for the wind direction for all times in the weather stream
			std::optional<double> m_deltaWindDirection;						// an override for the wind direction delta

			DECLARE_OBJECT_CACHE_MT(WSNode, WSNode);
		};

		class GFNode : public RefNode<GridFilter>
		{
		public:
			DEVICE GFNode* LN_Succ() const { return (GFNode*)RefNode<GridFilter>::LN_Succ(); };
			DEVICE GFNode* LN_Pred() const { return (GFNode*)RefNode<GridFilter>::LN_Pred(); };

			std::vector<std::string> m_subNames;							// a list of sub-scenario names that the filter should be applied to

			DECLARE_OBJECT_CACHE_MT(GFNode, GFNode);
		};

		IgnitionOptions* AddFire(Fire* fire, bool constructing = false);	// adds a ignition/fire to this scenario, a scenario needs at least one fire to
																			// simulate
		bool RemoveFire(Fire *fire);			// various other routines to handle the set of fires attached to this scenario
		ULONG GetFireCount() const { return m_fireList.GetCount(); };
		Fire *FireAtIndex(ULONG index) const { return m_fireList.IndexPtr(index); };
		IgnitionOptions* IgnitionOptionsAtIndex(ULONG index) const { return m_fireList.IndexNode(index); };
		ULONG IndexOfFire(Fire *fire) const { return m_fireList.FindPtrIndex(fire); };

		WSNode *AddStream(WeatherStream *stream, bool constructing = false);
		HRESULT RemoveStream(WeatherStream *stream);
		/// <summary>
		/// Filter the applied weather streams to only the ones specified in the vector.
		/// </summary>
		HRESULT FilterStreams(const std::vector<WSNode*>& streams);
		HRESULT FilterFilters();
		ULONG GetStreamCount() const { return m_streamList.GetCount(); };
		WeatherStream *StreamAtIndex(ULONG index) const  { return m_streamList.IndexPtr(index); }
		ULONG IndexOfStream(WeatherStream *stream) const { return m_streamList.FindPtrIndex(stream); }
		// routines to get/set various options to this scenario
		GFNode *InsertFilter(GridFilter *filter, ULONG index, bool during_serialization = false);
		bool RemoveFilter(GridFilter *filter);

		HRESULT ExportFireFronts(Fire *fires, HSS_Time::WTime &start_time, HSS_Time::WTime &end_time, VectorExportOptions *options, USHORT flags,
			std::string_view driver_name, const std::string &export_projection, const std::filesystem::path &file_name,
			const TCHAR *fgm_name, const TCHAR *prometheus_version, ::UnitConvert::STORAGE_UNIT perimeter_units,
			::UnitConvert::STORAGE_UNIT area_units, ::UnitConversion *uc = nullptr, const USHORT child_flags = 0x0001) const;
		HRESULT ExportCriticalPath(const Asset* a, const std::uint32_t index, const std::uint16_t flags,
			std::string_view driver_name, std::string& export_projection, const std::filesystem::path& file_path) const;
		HRESULT BuildCriticalPath(const Asset* a, const std::uint32_t index, const std::uint16_t flags, MinListTempl<CriticalPath>& polyset) const;

		ULONG GetFilterCount() const { return m_filterList.GetCount(); };
		GridFilter *FilterAtIndex(ULONG index) const;
		ULONG IndexOfFilter(GridFilter *filter) const;

		bool AddVector(Vector *vector, bool during_serialization = false);
		bool RemoveVector(Vector *vector);
		ULONG GetVectorCount() const { return m_vFilterList.GetCount(); };
		Vector *VectorAtIndex(ULONG index) const { return m_vFilterList.IndexPtr(index); };
		ULONG IndexOfVector(Vector *v) const { return m_vFilterList.FindPtrIndex(v); };

	    bool AddAsset(Asset* asset, ULONG operation, bool during_serialization = false);
	    bool RemoveAsset(Asset* asset);
	    ULONG GetAssetCount() const { return m_assetList.GetCount(); };
	    Asset* AssetAtIndex(ULONG index) const { return m_assetList.IndexPtr(index); };
	    ULONG IndexOfAsset(Asset* asset) const { return m_assetList.FindPtrIndex(asset); };


		FBPOptions GetFBPOptions() const { return FBPOptions(m_scenario.get()); };
		bool SetFBPOptions(const FBPOptions &options) { bool retval = options.SaveToScenario(m_scenario.get()); return retval; }

		double GetDefaultFMC(bool &active) const;
		void SetDefaultFMC(double fmc, bool active);

		BasicScenarioOptions GetBasicOptions() const { BasicScenarioOptions bsc(m_scenario.get()); return bsc; }
		bool SetBasicOptions(const BasicScenarioOptions &bsc) { return bsc.SaveToScenario(m_scenario.get()); }

		HRESULT Simulation_State();
		HRESULT Simulation_Reset(bool init_child_cache = true);
		// sets up a scenario ready to simulate a fire growth - return success or failure
		HRESULT Simulation_Step();
		// tells the simulation to proceed to the next output interval - returns 0 for failure,
		// 1 for success, or SUCCESS_SCENARIO_SIMULATION_COMPLETE for success and finished.
		HRESULT Simulation_StepBack();
		HRESULT Simulation_Clear();
		WTime Simulation_StartTime() const { PolymorphicAttribute v; m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_START_TIME, &v); HSS_Time::WTime time(m_timeManager); VariantToTime_(v, &time); time.SetTimeManager(m_timeManager); return time; };
		// retrieve basic things
		WTime Simulation_CurrentTime() const;
		WTime Simulation_EndTime() const { PolymorphicAttribute v; m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_END_TIME, &v); HSS_Time::WTime time(m_timeManager); VariantToTime_(v, &time); time.SetTimeManager(m_timeManager); return time; };
		HRESULT Simulation_StartTime(const HSS_Time::WTime &time) { PolymorphicAttribute v = time; return m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_START_TIME, v); }
		// set basic things
		HRESULT Simulation_EndTime(const HSS_Time::WTime &time) { PolymorphicAttribute v = time; return m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_END_TIME, v); };
		HRESULT Ignition_OverrideStartTime(const HSS_Time::WTime& time) { PolymorphicAttribute v = time; return m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_IGNITION_START_TIME_OVERRIDE, v); };

		HRESULT ValidateStreams() { return m_weatherGrid->Valid(0, this->Simulation_StartTime(), Simulation_EndTime() - Simulation_StartTime(), 0, 0); };

		std::string										m_name,
														m_subName,
														m_comments,
														m_loadWarning;
		boost::intrusive_ptr<CCWFGM_Scenario>			m_scenario;
		Layer *											m_layerThread;
		volatile std::int32_t							m_simStatus;
		std::vector<Scenario*>							m_childArray;
		Scenario*										m_parent;
		APTR											m_appendVectors;
		std::weak_ptr<validation::validation_object>	m_validation;

		APTR m_worker;

		bool ExportFilteredFuelGrid(const std::string &grid_file_name, CCWFGM_FuelMap *fuelMap, std::int32_t compression = 0);
	    bool ExportHeader(FILE *fp);

		WeatherStream* GetPrimaryStream() const;
		HRESULT SetPrimaryStream(WeatherStream *stream);

		HRESULT GetBurnConditionDays(USHORT *days);
		HRESULT GetDayBurnCondition(const HSS_Time::WTime &tday, CBurnPeriodOption *option);
		HRESULT GetIndexBurnCondition(USHORT index, CBurnPeriodOption *option);
		HRESULT SetDayBurnCondition(const HSS_Time::WTime &tday, const CBurnPeriodOption &option);
		HRESULT ClearBurnConditions();
		bool CorrectBurnConditionRange(const CBurnPeriodOption &defaultoption, std::shared_ptr<validation::validation_object> valid, const std::string& name);

		const ScenarioCollection *GetScenarioCollection() { return m_scenarioCollection; }

	protected:
		std::vector<XY_Point>	m_PCOM_arBurnt;
		std::vector<ULONG>			m_PCOM_arStepPointsNum;
		const ScenarioCollection	*m_scenarioCollection;
		boost::intrusive_ptr<CCWFGM_WeatherGrid>	m_weatherGrid;
		boost::intrusive_ptr<CCWFGM_TemporalAttributeFilter> m_temporalFilter;

		RefList<Fire, IgnitionOptions>	m_fireList;
		RefList<WeatherStream, WSNode>	m_streamList;		// each scenario may also have a number of WeatherStreams associated with it
		RefList<GridFilter, GFNode>		m_filterList;		// each scenario may also have an ordered set of grid filters associated with it
		RefList<Vector>					m_vFilterList;
	    RefList<Asset>					m_assetList;
		const HSS_Time::WTimeManager	*m_timeManager;

		bool duplicateVectorList(const Project::Scenario &s);
	    bool duplicateAssetList(const Scenario& s);
		bool duplicateTargetList(const Scenario& s);
		bool duplicateFireList(const Project::Scenario &s);
		bool duplicateStreamList(const Project::Scenario &s);
		bool duplicateFilterList(const Project::Scenario &s);

	private:
		void ExportScenarioParameters(FILE *fp);
		void ExportScenarioWxStreamInfo(FILE *fp);
		void ExportScenarioFireInfo(FILE *fp);

	public:
		const HSS_Time::WTimeManager *TimeManager() const { return m_timeManager; }
		Layer *layerThread() const { return m_layerThread; }
		CCWFGM_WeatherGrid *GetWeatherGrid() const { return m_weatherGrid.get(); }
		CCWFGM_TemporalAttributeFilter *GetTemporalAttributeFilter() const { return m_temporalFilter.get(); }

		bool GetGreenupTimes(HSS_Time::WTimeSpan *start, HSS_Time::WTimeSpan *end, bool *effective);
		bool SetGreenupTimes(const HSS_Time::WTimeSpan &start, const HSS_Time::WTimeSpan &end, bool effective);

		bool GetStandingGrassTimes(HSS_Time::WTimeSpan *start, HSS_Time::WTimeSpan *end, bool *effective);
		bool SetStandingGrassTimes(const HSS_Time::WTimeSpan &start, const HSS_Time::WTimeSpan &end, bool effective);

		bool GetCuringDetails(CuringOptions &curing);
		bool SetCuringDetails(const CuringOptions &curing);

	private:
		bool getTimes(HSS_Time::WTimeSpan *start, HSS_Time::WTimeSpan *end, USHORT option, bool *effective);
		bool setTimes(const HSS_Time::WTimeSpan &start, const HSS_Time::WTimeSpan &end, USHORT option, bool effective);

		struct iterator
		{
			ULONG _min, _i, _max;
			bool wholeSet;
		};

	public:
		std::uint32_t GetNumberSteps(); // return the number of calculation intervals that we have a fire state
		ULONG GetNumberFires(HSS_Time::WTime &time);
		bool GetStepsArray(std::uint32_t *size, std::vector<HSS_Time::WTime> *times);
		bool GetStatsArray(std::uint32_t f, HSS_Time::WTime &time, SHORT stat, std::uint32_t *size, double *stats);
		XY_Point *GetVectorArray(std::uint32_t f, HSS_Time::WTime &time, std::uint32_t *size);
		HRESULT GetStats(std::uint32_t fire, HSS_Time::WTime& time, SHORT stat, std::uint16_t discretization, PolymorphicAttribute* stats); // calculates and returns a generic stat for the entire fire
		HRESULT GetXYStats(const XY_Point& min_pt, const XY_Point& max_pt, HSS_Time::WTime& mintime, HSS_Time::WTime& time, std::uint16_t stat, std::uint32_t interp_method, std::uint16_t discretization, NumericVariant* stats) const;
		HRESULT GetXYStatsSet(const XY_Point& min_pt, const XY_Point& max_pt, HSS_Time::WTime& mintime, HSS_Time::WTime& time, std::uint32_t interp_method, USHORT discretization, std::vector<XYStat>& stats) const;
		HRESULT GetStatsPercentage(ULONG fire, HSS_Time::WTime& time, SHORT stat, double greater_equal, double less_than, double* stats);

	private:
		CCWFGM_Scenario* getScenario(const HSS_Time::WTime& time, std::uint32_t& f) const;
		void autoWxStation();

	public:
		DECLARE_OBJECT_CACHE_MT(Scenario, Scenario)
	};
}//Project namespace


namespace Project
{
	class CWFGMPROJECT_EXPORT ScenarioCollection : public ISerializeProto
	{
	protected:
		MinListTempl<Scenario>	m_scenarioList;

	public:
		bool ExportScenarioInfo(FILE *fp);
		ScenarioCollection(CWFGMProject *project, const FBPOptions *fbpo, const BasicScenarioOptions *bso, const CBurnPeriodOption *bo, const CuringOptions *co);
		virtual ~ScenarioCollection();

		void AddScenario(Project::Scenario *scenario)			{ m_scenarioList.AddTail(scenario); };
		void RemoveScenario(Project::Scenario *scenario)		{ m_scenarioList.Remove(scenario); };
		ULONG GetCount() const									{ return m_scenarioList.GetCount(); };
		Project::Scenario *FirstScenario() const				{ return m_scenarioList.LH_Head(); };
		Project::Scenario *ScenarioAtIndex(ULONG index) const	{ return m_scenarioList.IndexNode(index); };
		ULONG IndexOf(Project::Scenario *scenario) const		{ return m_scenarioList.NodeIndex(scenario); };

		ULONG GetCountUsing(Fire *fire);
		ULONG GetCountUsing(WeatherStream *stream);
		ULONG GetCountUsing(WeatherStation *station);
		ULONG GetCountUsing(GridFilter *filter);
		ULONG GetCountUsing(Vector *vector);
		ULONG GetCountUsing(Asset* asset);

		Project::Scenario *FindName(const char *name) const;

		bool AssignNewGrid(ICWFGM_GridEngine *oldgrid, ICWFGM_GridEngine *newgrid);

		const FBPOptions *m_fbpOptions;
		const BasicScenarioOptions *m_bsOptions;		// both of these from the app class (right now anyway)
		const CBurnPeriodOption *m_burnOptions;
		const CuringOptions		*m_curingOptions;
		CWFGMProject *m_project;

		std::string CollectLoadWarnings();

	protected:
		std::string m_loadWarning;

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		WISE::ProjectProto::ScenarioCollection* serialize(const SerializeProtoOptions& options) override;
		ScenarioCollection *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIRECOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
