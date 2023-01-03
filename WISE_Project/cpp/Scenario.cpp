/**
 * WISE_Project: Scenario.cpp
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

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

#if _DLL
#include <afxdisp.h>
#include <shlwapi.h>
#endif

#include "ScenarioCollection.h"
#include "FireCollection.h"
#include "WeatherCollection.h"
#include "GridFilter.h"
#include "angles.h"
#include "CWFGMProject.h"
#include "str_printf.h"

#include "WindSpeedGrid.h"
#include "WindDirectionGrid.h"
#include "GreenupGridFilter.h"
#include "PercentConiferGridFilter.h"
#include "AngleGridFilter.h"
#include "FuelGridFilter.h"
#include "ReplaceGridFilter.h"
#include "convert.h"
#include "VectorExportOptions.h"
#include "FuelCom_ext.h"
#include "CWFGM_TemporalAttributeFilter.h"
#include "ScenarioExportRules.h"

#include "insert_ordered_map.h"
#include <omp.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using namespace std::string_literals;
using namespace HSS_Time;


IMPLEMENT_OBJECT_CACHE_MT_NO_TEMPLATE(Project::Scenario, Scenario, 128 * 1024 / sizeof(Scenario), true, 16)
IMPLEMENT_OBJECT_CACHE_MT_NO_TEMPLATE(Project::Scenario::IgnitionOptions, IgnitionOptions, 64 * 1024 / sizeof(IgnitionOptions), true, 16)
IMPLEMENT_OBJECT_CACHE_MT_NO_TEMPLATE(Project::Scenario::WSNode, WSNode, 1024 * 1024 / sizeof(WSNode), true, 16)
IMPLEMENT_OBJECT_CACHE_MT_NO_TEMPLATE(Project::Scenario::GFNode, GFNode, 128 * 1024 / sizeof(GFNode), true, 16)


Project::Scenario::Scenario(const ScenarioCollection *scenarioCollection, const WTimeManager *timeManager, ICWFGM_GridEngine *grid)
	: m_simStatus(0), m_validation({}),
	m_timeManager(timeManager)
{
	m_scenarioCollection = scenarioCollection;

	HRESULT hr;
	m_scenario = boost::make_intrusive<CCWFGM_Scenario>();
	m_weatherGrid = boost::make_intrusive<CCWFGM_WeatherGrid>();
	m_temporalFilter = boost::make_intrusive<CCWFGM_TemporalAttributeFilter>();

	if (scenarioCollection->m_project->m_layerManager)
	{
		hr = scenarioCollection->m_project->m_layerManager->NewLayerThread(&m_layerThread);
		if (FAILED(hr))
			throw std::logic_error("Failed");
	}
	else
		m_layerThread = nullptr;

	PolymorphicUserData v(this);
	m_scenario->put_UserData(v);

	boost::intrusive_ptr<ICWFGM_GridEngine> ge = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(m_weatherGrid);
	boost::intrusive_ptr<ICWFGM_GridEngine> te = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(m_temporalFilter);
	ge->put_UserData(v);
	te->put_UserData(v);

	if (scenarioCollection->m_project->m_layerManager)
	{
		ge->put_LayerManager(scenarioCollection->m_project->m_layerManager.get());
		te->put_LayerManager(scenarioCollection->m_project->m_layerManager.get());
	}

	ICWFGM_CommonData data;
	data.m_timeManager = (WTimeManager*)m_timeManager;
	ge->PutCommonData(nullptr, &data);
	te->PutCommonData(nullptr, &data);
	m_scenario->PutCommonData(nullptr, &data);

	m_scenario->PutGridEngine(layerThread(), te.get());
	te->PutGridEngine(layerThread(), ge.get());

	if (grid)
	{
		if (scenarioCollection->m_project->m_layerManager)
			grid->put_LayerManager(scenarioCollection->m_project->m_layerManager.get());
		ge->PutGridEngine(nullptr, grid);
		te->PutGridEngine(nullptr, grid);
		ge->PutGridEngine(layerThread(), grid);
		grid->PutGridEngine(layerThread(), ge.get());
	}

	if (((grid) && (CWFGMProject::m_appMode > 0)))
		m_weatherGrid->SetCache(layerThread(), 1, true);

	if (m_scenarioCollection->m_fbpOptions)
	{
		SetFBPOptions(*m_scenarioCollection->m_fbpOptions);
		SetGreenupTimes(m_scenarioCollection->m_fbpOptions->m_greenupStart, m_scenarioCollection->m_fbpOptions->m_greenupEnd, true);
		SetStandingGrassTimes(m_scenarioCollection->m_fbpOptions->m_standingStart, m_scenarioCollection->m_fbpOptions->m_standingEnd, true);
	}
	if (m_scenarioCollection->m_bsOptions)		SetBasicOptions(*m_scenarioCollection->m_bsOptions);
	if (m_scenarioCollection->m_curingOptions)	SetCuringDetails(*m_scenarioCollection->m_curingOptions);

	PolymorphicAttribute v1;
	int max_thread_count;
		max_thread_count = CWorkerThreadPool::NumberIdealProcessors();

	v1 = max_thread_count;
	m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_MULTITHREADING, v1);
	v1 = true;
	m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_FORCE_AFFINITY, v1);

	bool active;
	double FMC = m_scenarioCollection->m_project->GetDefaultFMC(active);
	SetDefaultFMC(FMC, active);

	m_parent = nullptr;
	m_worker = nullptr;
	m_appendVectors = nullptr;
}


double Project::Scenario::GetDefaultFMC(bool &active) const
{
	double m_specifiedFMC = -1.0;
	PolymorphicAttribute v;
	if (m_scenario)
	{
		if (SUCCEEDED(m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_SPECIFIED_FMC, &v)))
		{
			VariantToDouble_(v, &m_specifiedFMC);
		}
		if (SUCCEEDED(m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_SPECIFIED_FMC_ACTIVE, &v)))
		{
			VariantToBoolean_(v, &active);
		}
	}
	return m_specifiedFMC;
}


void Project::Scenario::SetDefaultFMC(double fmc, bool active)
{
	PolymorphicAttribute v(fmc);
	m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_SPECIFIED_FMC, v);
	v = active;
	m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_SPECIFIED_FMC_ACTIVE, v);
}


Project::Scenario::Scenario(const Project::Scenario &scenario, const WTimeManager *timeManager, ICWFGM_GridEngine *grid, bool isChild) :
	m_simStatus(0), m_validation({}),
	m_timeManager(timeManager)
{
	m_scenarioCollection = scenario.m_scenarioCollection;

	boost::intrusive_ptr<ICWFGM_CommonBase> s, t;
	HRESULT hr = scenario.m_scenario->Clone(&s);
	m_weatherGrid = new CCWFGM_WeatherGrid();
	hr = scenario.m_temporalFilter->Clone(&t);

	if (m_scenarioCollection->m_project->m_layerManager)
	{
		hr = m_scenarioCollection->m_project->m_layerManager->NewLayerThread(&m_layerThread);
		if (FAILED(hr))
			throw std::logic_error("Failed");
	}
	else
		m_layerThread = nullptr;

	m_scenario = boost::dynamic_pointer_cast<CCWFGM_Scenario>(s);
	m_temporalFilter = boost::dynamic_pointer_cast<CCWFGM_TemporalAttributeFilter>(t);

	boost::intrusive_ptr<ICWFGM_GridEngine> ge = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(m_weatherGrid);
	boost::intrusive_ptr<ICWFGM_GridEngine> te = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(m_temporalFilter);

	if (m_scenarioCollection->m_project->m_layerManager)
	{
		ge->put_LayerManager(m_scenarioCollection->m_project->m_layerManager.get());
		te->put_LayerManager(m_scenarioCollection->m_project->m_layerManager.get());
	}

	ICWFGM_CommonData data;
	data.m_timeManager = (WTimeManager*)m_timeManager;
	ge->PutCommonData(nullptr, &data);
	te->PutCommonData(nullptr, &data);
	m_scenario->PutCommonData(nullptr, &data);

	m_scenario->PutGridEngine(layerThread(), te.get());
	te->PutGridEngine(layerThread(), ge.get());

	if (grid)
	{
		if (m_scenarioCollection->m_project->m_layerManager)
			grid->put_LayerManager(m_scenarioCollection->m_project->m_layerManager.get());
		ge->PutGridEngine(nullptr, grid);
		te->PutGridEngine(nullptr, grid);
		ge->PutGridEngine(layerThread(), grid);
		grid->PutGridEngine(layerThread(), ge.get());
	}

	if ((!isChild) && (grid) && (CWFGMProject::m_appMode > 0))
		m_weatherGrid->SetCache(layerThread(), 1, true);

	PolymorphicUserData v = this;
	m_scenario->put_UserData(v);
	ge->put_UserData(v);
	m_name = scenario.m_name;
	m_comments = scenario.m_comments;

	duplicateFireList(scenario);
	duplicateStreamList(scenario);
	duplicateFilterList(scenario);
	duplicateVectorList(scenario);
	duplicateAssetList(scenario);
	duplicateTargetList(scenario);

	int max_thread_count;
	max_thread_count = CWorkerThreadPool::NumberIdealProcessors();

	PolymorphicAttribute v1 = max_thread_count;
	m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_MULTITHREADING, v1);
	v1 = true;
	m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_FORCE_AFFINITY, v1);

	m_parent = nullptr;
	m_worker = nullptr;
	m_appendVectors = nullptr;
}


Project::Scenario::Scenario(const Scenario& scenario, const WTimeManager* timeManager, ICWFGM_GridEngine* grid, IgnitionOptions* ignition, bool isChild)
	: Scenario(scenario, timeManager, grid, isChild)
{
	//remove all ignitions - calls another constructor that could have created 
	while (this->m_fireList.GetCount())
		RemoveFire(m_fireList.LH_Head()->LN_Ptr());

	Scenario::IgnitionOptions* nnode;
	//add the new ignition to the list
	nnode = AddFire(ignition->LN_Ptr(), true);
}


Project::Scenario::~Scenario()
{
	weak_assert(!m_worker);
	HRESULT state = Simulation_State();
	if ((state == SUCCESS_SCENARIO_SIMULATION_RUNNING) ||
		(state == SUCCESS_SCENARIO_SIMULATION_RESET) ||
		(state == SUCCESS_SCENARIO_SIMULATION_COMPLETE) ||
		(state == SUCCESS_SCENARIO_SIMULATION_COMPLETE_EXTENTS) ||
		(state == SUCCESS_SCENARIO_SIMULATION_COMPLETE_ASSET) ||
		(state == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI90) ||
		(state == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI95) ||
		(state == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI100) ||
		(state == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_RH) ||
		(state == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_PRECIP) ||
		(state == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_AREA) ||
		(state == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_BURNDISTANCE))
	{
		Simulation_Clear();		// stop any running simulation
	}

	if ((!m_parent) && (CWFGMProject::m_appMode > 0))
		m_weatherGrid->SetCache(layerThread(), 1, false);

	while (GetFireCount() > 0)
	{
		Fire *f = FireAtIndex(0);
		RemoveFire(f);
	}
	while (GetStreamCount() > 0)
	{
		WeatherStream *ws = StreamAtIndex(0);
		RemoveStream(ws);
	}
	while (GetFilterCount() > 0)
	{
		GridFilter *gf = FilterAtIndex(0);
		RemoveFilter(gf);
	}
	while (GetVectorCount() > 0)
	{
		Vector *v = VectorAtIndex(0);
		RemoveVector(v);
	}
	while (GetAssetCount() > 0)
	{
		Asset* a = AssetAtIndex(0);
		RemoveAsset(a);
	}
	boost::intrusive_ptr<ICWFGM_GridEngine> ge = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(m_weatherGrid);
	boost::intrusive_ptr<ICWFGM_GridEngine> te = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(m_temporalFilter);

	boost::intrusive_ptr<ICWFGM_GridEngine> grid;
	if (ge)
		ge->GetGridEngine(nullptr, &grid);
	if ((grid.get()) && (ge.get()))
	{
		grid->PutGridEngine(layerThread(), nullptr);
		ge->PutGridEngine(layerThread(), nullptr);
		te->PutGridEngine(layerThread(), nullptr);
		ge->PutGridEngine(nullptr, nullptr);
		te->PutGridEngine(nullptr, nullptr);
	}
	else if (te.get()) {
		te->PutGridEngine(layerThread(), nullptr);
		te->PutGridEngine(nullptr, nullptr);
	}
	m_scenario->PutGridEngine(layerThread(), nullptr);
	if (m_layerThread)
		m_scenarioCollection->m_project->m_layerManager->DeleteLayerThread(m_layerThread);

	weak_assert(!m_childArray.size());
}


// This routine checks the current scenario configuration and returns an error code if it is not valid.
// a flags (declared in ScenarioCollection.h) struct should be provided as input; this tells the method
// which parts of the scenario configuration should be validated.
//
// Here's how in_flags works:
//
//  fuelgrid	- a one in this bit indicates that we should be checking for multiple fbp fuel type grids
//  fuelpatch	- a one in this bit indicates that we should be ensuring that all fuel patches are above fuel grids.
//	temporal	- a one in this bit indicates that we should be checking for mixed temporal and spatial weather grids.
//	spatialWS	- a one in this bit indicates that we should be checking for multiple spatial wind speed grids.
//	spatialWD	- a one in this bit indicates that we should be checking for multiple spatial wind dirction grids.
//	greenup		- a one in this bit indicates that we should be checking for multiple greenup grids.
//	cbh			- a one in this bit indicates that we should be checking for multiple crown base height grids.
//	pc			- a one in this bit indicates that we should be checking for multiple percent configer grids.
//	df			- a one in this bit indicates that we should be checking for multiple percent dead fir grids.
//	pgc			- a one in this bit indicates that we should be checking for multiple percent grass curing grids.
//	wxpatch		- a one in this bit indicates that we should be checking for ensuring that all weather patches are above all weather grids.
//	ignitions	- a one in this bit indicates that we should be ensuring that there is at least one ignition specified.
//	ignitiontime- a one in this bit indicates that we should be ensuring that all ignitions have valid times.
//	primarywx	- a one in this bit indicates that we should be ensuring that there is a primary weather stream specified.
//	wxtimes		- a one in this bit indicates that we should be ensuring that weather stream times span the entire scenario.
//  wxlocation	- a one in this bit indicates that we should be ensuring that no two active weather stations are closer than 100 m.
//  wxstation	- a one in this bit indicates that we should be checking for multiple weather streams in a single weather station.
//  wxdata		- a one in this bit indicates that we should be checking that wx data is present for at least one stream.
//
// Return codes can be found in ScenarioCollection.h.
//
LONG Project::Scenario::CheckFiltersAndPatches(flags in_flags, bool warnOnFireBeforeScenario /* = false*/)
{
	ULONG count;
	flags results = { 0 };

	WTime startTime(Simulation_StartTime()), endTime(Simulation_EndTime());

	for (ULONG i = 0; i < GetFilterCount(); i++)
	{
		GridFilter *pFilter = FilterAtIndex(i);

		auto wsg = dynamic_cast<CWindSpeedGrid*>(pFilter);
		if (wsg)
		{
			if ((in_flags.spatialWS || in_flags.temporal) && results.temporal)
			{
				weak_assert(false);						// shouldn't occur any more
				return Scenario::ERROR_SPATIAL_AND_TEMPORAL_MIXED; // spatial and temporal interpolation may not be mixed
			}
			results.spatialWS = 1;
			continue;
		}

		auto wdg = dynamic_cast<CWindDirectionGrid*>(pFilter);
		if (wdg)
		{
			if ((in_flags.spatialWD || in_flags.temporal) && results.temporal)
			{
				weak_assert(false);						// shouldn't occur any more
				return Scenario::ERROR_SPATIAL_AND_TEMPORAL_MIXED; // spatial and temporal interpolation may not be mixed
			}
			results.spatialWD = 1;
			continue;
		}

		auto wpf = dynamic_cast<CWeatherPolyFilter*>(pFilter);
		if (wpf)
		{
			if ((in_flags.wxpatch || in_flags.spatialWD || in_flags.spatialWS || in_flags.temporal) && (results.spatialWD || results.spatialWS || results.temporal))
			{
				return Scenario::ERROR_WEATHER_PATCH_AFTER_WEATHERGRID; // all weather patches must occur before all spatial and temporal wind grids
			}
			WTime wxPatchStartTime((ULONGLONG)0, m_timeManager);
			wpf->GetStartTime(wxPatchStartTime);
			WTime wxPatchEndTime((ULONGLONG)0, m_timeManager);
			wpf->GetEndTime(wxPatchEndTime);
			if (in_flags.temporaltime && ((wxPatchStartTime > endTime) || (startTime > wxPatchEndTime)))
			{
				return Scenario::ERROR_WEATHER_PATCH_TIMES_NOT_OVERLAP; // the time range must overlap with the scenario times
			}
			continue;
		}

		auto wgf = dynamic_cast<CWeatherGridFilter*>(pFilter);
		if (wgf)
		{
			weak_assert(false);						// these aren't even used any more
			if ((in_flags.spatialWD || in_flags.spatialWS || in_flags.temporal) && (results.spatialWD || results.spatialWS))
			{
				weak_assert(false);						// shouldn't occur any more
				return Scenario::ERROR_SPATIAL_AND_TEMPORAL_MIXED; // spatial and temporal interpolation may not be mixed
			}
			WTime wxGridStartTime((ULONGLONG)0, m_timeManager);
			wgf->GetStartTime(wxGridStartTime);
			WTime wxGridEndTime((ULONGLONG)0, m_timeManager);
			wgf->GetEndTime(wxGridEndTime);
			if (in_flags.temporaltime && (wxGridStartTime > endTime) || (startTime > wxGridEndTime) )
			{
				return Scenario::ERROR_WEATHER_GRID_TIMES_NOT_OVERLAP; // the time range must overlap with the scenario times
			}
			results.temporal = 1;
			continue;
		}

		auto ggf = dynamic_cast<GreenupGridFilter*>(pFilter);
		if (ggf)
		{
			if (in_flags.greenup && results.greenup)
			{
				return Scenario::ERROR_MULTIPLE_GREENUP_GRIDS; // only a single greenup grid filter may be selected
			}
			results.greenup = 1;
			continue;
		}

		auto cgf = dynamic_cast<CBHGridFilter*>(pFilter);
		if (cgf)
		{
			if (in_flags.cbh && results.cbh)
			{
				return Scenario::ERROR_MULTIPLE_CBH_GRIDS; // only a single CBH grid filter may be selected
			}
			results.cbh = 1;
			continue;
		}

		auto pcgf = dynamic_cast<PercentConiferGridFilter*>(pFilter);
		if (pcgf)
		{
			if (in_flags.pc && results.pc)
			{
				return Scenario::ERROR_MULTIPLE_PC_GRIDS; // only a single percent conifger grid filter may be selected
			}
			results.pc = 1;
			continue;
		}

		auto pdfgf = dynamic_cast<PercentDeadFirGridFilter*>(pFilter);
		if (pdfgf)
		{
			if (in_flags.df && results.df)
			{
				return Scenario::ERROR_MULTIPLE_PDF_GRIDS; // only a single percent dead fir grid filter may be selected
			}
			results.df = 1;
			continue;
		}

		auto pcggf = dynamic_cast<PercentCureGrassGridFilter*>(pFilter);
		if (pcggf)
		{
			if (in_flags.pgc && results.pgc)
			{
				return Scenario::ERROR_MULTIPLE_PCG_GRIDS; // only a single percent grass curing grid filter may be selected
			}
			results.pgc = 1;
			continue;
		}

		auto ovdgf = dynamic_cast<OVDGridFilter*>(pFilter);
		if (ovdgf)
		{
			if (in_flags.ovd && results.ovd)
			{
				return Scenario::ERROR_MULTIPLE_OVD_GRIDS; // only a single OVD grid filter may be selected
			}
			results.ovd = 1;
			continue;
		}

		auto thgf = dynamic_cast<TreeHeightGridFilter*>(pFilter);
		if (thgf)
		{
			if (in_flags.treeheight && results.treeheight)
			{
				return Scenario::ERROR_MULTIPLE_TREEHEIGHT_GRIDS; // only a single percent grass curing grid filter may be selected
			}
			results.pgc = 1;
			continue;
		}

		auto flgf = dynamic_cast<FuelLoadGridFilter*>(pFilter);
		if (flgf)
		{
			if (in_flags.fuelload && results.fuelload)
			{
				return Scenario::ERROR_MULTIPLE_FUELLOAD_GRIDS; // only a single percent grass curing grid filter may be selected
			}
			results.pgc = 1;
			continue;
		}

		auto fgf = dynamic_cast<FuelGridFilter*>(pFilter);
		if (fgf)
		{
			if (in_flags.fuelgrid && results.fuelgrid)
			{
				return Scenario::ERROR_MULTIPLE_FBP_FUEL_GRIDS; // only a single fuel grid filter may be selected
			}
			results.fuelgrid = 1;
			continue;
		}

		auto prgf = dynamic_cast<PolyReplaceGridFilter*>(pFilter);
		if (prgf)
		{
			if ((in_flags.fuelgrid || in_flags.fuelpatch) && results.fuelgrid)
			{
				return Scenario::ERROR_FUEL_PATCH_AFTER_FUELGRID; // all fuel patches must occur before all fbp fuel grids
			}
			results.fuelpatch = 1;
			continue;
		}
	}

	HRESULT hr;
	PolymorphicAttribute var;
	count = GetStreamCount();
	bool spatial;
	if (FAILED(hr = m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_SPATIAL, &var)))
	{
		weak_assert(false);
		return hr;
	}
	VariantToBoolean_(var, &spatial);

	if (count > 1 && !spatial)
	{
		return Scenario::ERROR_MULTIPLE_STREAMS_NO_INTERP;
	}
	if (in_flags.wxdata && count == 0)
	{
		return Scenario::ERROR_NO_WEATHER_DATA;
	}
	if (in_flags.wxlocation || in_flags.wxstation || in_flags.wxtimes)
	{
		for (ULONG i = 0; i < count; i++)
		{
			WeatherStream *WStream = StreamAtIndex(i);
			WeatherStation *WStation = WStream->m_weatherStation();

			if (in_flags.wxtimes)
			{
				WTime wxStart(m_timeManager);
				WTimeSpan wxDuration;
				if (FAILED(hr = WStream->GetValidTimeRange(wxStart, wxDuration)))
				{
					weak_assert(false);
					return hr;
				}

				if (wxStart > startTime || ((wxStart + wxDuration) <= endTime))
				{
					return Scenario::ERROR_WEATHER_STREAM_TIMES_NOT_SPAN; // all weather streams must span the entire duration of the scenario
				}
			}

			if (in_flags.wxlocation || in_flags.wxstation)
			{
				double x1, x2, y1, y2, dx, dy, z = 0;
				x1 = RADIAN_TO_DEGREE(WStation->Longitude());
				y1 = RADIAN_TO_DEGREE(WStation->Latitude());
				m_scenarioCollection->m_project->m_CoorConverter.LatlonToSource(1, &x1, &y1, &z);

				boost::intrusive_ptr<ICWFGM_GridEngine> ge;
				Layer *layerThread;
				m_scenario->GetGridEngine(&layerThread, &ge);

				for (ULONG j = i + 1; j < count; j++)
				{
					WeatherStation *WStation2 = StreamAtIndex(j)->m_weatherStation();
					if (in_flags.wxstation)
					{
						if (WStation == WStation2)
						{
							return Scenario::ERROR_MULTIPLE_WEATHER_STREAM_STATION; // only a single weather stream may be active for a given weather station
						}
					}
					if (in_flags.wxlocation)
					{
						x2 = RADIAN_TO_DEGREE(WStation2->Longitude());
						y2 = RADIAN_TO_DEGREE(WStation2->Latitude());

						m_scenarioCollection->m_project->m_CoorConverter.LatlonToSource(1, &x2, &y2, &z);

						dx = (x2 - x1);
						dy = (y2 - y1);
						if ((dx * dx + dy * dy) < 10000.0) // 10000.0 = 100.0 * 100.0
						{
							return Scenario::ERROR_MULTIPLE_WEATHER_STREAM_TOO_CLOSE; // no two weather stations can be closer than 100m apart
						}
					}
				}
			}
		}
	}

	if (spatial && in_flags.primarywx && GetPrimaryStream() == NULL)
	{
		return Scenario::ERROR_MULTIPLE_WEATHER_STREAM_NO_PRIMARY; // a primary weather stream must be specified
	}

	count = GetFireCount();
	if (in_flags.ignitions)
	{
		if (count == 0)
		{
			return Scenario::ERROR_NO_IGNITIONS; // at least one ignition must be specified
		}
	}

	if (in_flags.ignitiontime)
	{
		BOOL fire_times = false;
		for (ULONG i = 0; i < count; i++)
		{
			Fire *f = FireAtIndex(i);
			if(f->IgnitionTime() >= endTime)
			{
				return Scenario::ERROR_IGNITION_START_TOO_LATE; // no ignition may have a start time later than the scenario end time
			}
			if (f->IgnitionTime() < startTime)
			{
				fire_times = true;
			}
		}
		// this is an odd place from which to pop up a messagebox...but will never happen by default and keeping the box here helps keep logic simple and consistent elsewhere..
		if (fire_times && warnOnFireBeforeScenario
#if _DLL
                && MessageBox(NULL, _T("There are one or more ignitions with start times earlier\nthan the start of the scenario.  Do you want to continue?"), _T("Warning"), MB_OKCANCEL | MB_ICONINFORMATION) == IDCANCEL
#endif
            )
		{
			return Scenario::ERROR_IGNITION_START_TOO_EARLY; // warn if any ignition has a start time that is earlier than the scenario start time
		}
	}

	return S_OK;
}


HRESULT Project::Scenario::Simulation_State()
{
	if (m_childArray.size())
	{
		return m_childArray[0]->Simulation_State();
	}
	return m_scenario->Simulation_State();
}


HRESULT Project::Scenario::Simulation_Reset(bool init_child_cache)
{
	HRESULT hr = S_OK;

	if (!m_parent) {
		hss::insert_ordered_map<std::string, std::vector<WSNode*>> ws;
		std::vector<WSNode*> globals;
		WSNode* wsnode = m_streamList.LH_Head();

		//find all weather streams that can be split into sub-scenarios
		while (wsnode->LN_Succ())
		{
			if (wsnode->m_valid)
			{
				std::string name;
				if (wsnode->m_name.length())
					name = wsnode->m_name;
				else
					name = (boost::format("%s_%s") % this->m_name % wsnode->LN_Ptr()->m_name).str();
				ws.insert({ name, std::vector<WSNode*>() });
				ws[name].push_back(wsnode);
			}
			else
				globals.push_back(wsnode);
			wsnode = wsnode->LN_Succ();
		}

		if (ws.size())
		{
			for (auto& node : ws.insertOrdered())
			{
				Scenario* s = new Scenario(*this, m_timeManager, m_scenarioCollection->m_project->gridEngine(), true);
				std::vector<WSNode*> streams;
				std::copy(globals.begin(), globals.end(), std::back_inserter(streams));
				std::copy(node.second.begin(), node.second.end(), std::back_inserter(streams));
				WSNode* n = node.second[0];
				s->FilterStreams(streams);
				s->Simulation_StartTime(n->m_startTime);
				s->Simulation_EndTime(n->m_endTime);
				s->Ignition_OverrideStartTime(n->m_ignitionTime);
				s->m_subName = node.first;
				s->FilterFilters();
				m_childArray.push_back(s);
				s->m_parent = this;
			}
		}
	}

	//there are sub-scenarios, initialize them
	if (m_childArray.size())
	{
		int i;
		int thread_id = -1;
        volatile bool stop = false;
#pragma omp parallel for num_threads(CWorkerThreadPool::NumberIdealProcessors()) firstprivate(thread_id) shared(hr, stop)
		for (i = 0; i < m_childArray.size(); i++)
		{
            if (stop)
                continue;
			if (thread_id == -1)
			{
				thread_id = omp_get_thread_num();
				CWorkerThread::native_handle_type thread = CWorkerThreadPool::GetCurrentThread();
				CWorkerThreadPool::SetThreadAffinityToMask(thread, thread_id);
			}
			Scenario* ss = m_childArray[i];
			HRESULT phr = ss->m_scenario->Simulation_Reset(nullptr, "");
			if (init_child_cache)
				ss->m_weatherGrid->SetCache(ss->layerThread(), 0, true);
			if (FAILED(phr))
			{
				hr = phr;
				stop = true;
			}
		}
	}
	//no sub-scenarios to create, initialize for a single-scenario run
	else
	{
		hr = m_scenario->Simulation_Reset(nullptr, "");
		if (SUCCEEDED(hr))
			hr = m_weatherGrid->SetCache(layerThread(), 0, true);
	}

	return hr;
}


HRESULT Project::Scenario::Simulation_Step()
{
	if (!m_childArray.size())
		return m_scenario->Simulation_Step();

	HRESULT hr = S_OK;
	ULONG complete_count = 0;
	int si;
	int thread_id = -1;
#pragma omp parallel for num_threads(CWorkerThreadPool::NumberIdealProcessors()) firstprivate(thread_id)
	for (si = 0; si < m_childArray.size(); si++)
	{
		if (thread_id == -1)
		{
			thread_id = omp_get_thread_num();
            CWorkerThread::native_handle_type thread = CWorkerThreadPool::GetCurrentThread();
			CWorkerThreadPool::SetThreadAffinityToMask(thread, thread_id);
		}
		HRESULT hr1 = m_childArray[si]->Simulation_Step();
		if (FAILED(hr1))
			hr = hr1;
		if ((hr1 == SUCCESS_SCENARIO_SIMULATION_COMPLETE) ||
			(hr1 == SUCCESS_SCENARIO_SIMULATION_COMPLETE_EXTENTS) ||
			(hr1 == SUCCESS_SCENARIO_SIMULATION_COMPLETE_ASSET) ||
			(hr1 == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI90) ||
			(hr1 == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI95) ||
			(hr1 == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI100) ||
			(hr1 == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_RH) ||
			(hr1 == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_PRECIP) ||
			(hr1 == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_AREA) ||
			(hr1 == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_BURNDISTANCE))
			complete_count++;
	}
	if (complete_count == m_childArray.size())
		return SUCCESS_SCENARIO_SIMULATION_COMPLETE;
	return hr;
}


HRESULT Project::Scenario::Simulation_StepBack()
{
	if (!m_childArray.size())
		return m_scenario->Simulation_StepBack();

	HRESULT hr = S_OK;
	for (auto s : m_childArray)
	{
		if (FAILED(hr = s->Simulation_StepBack()))
			break;
	}
	return hr;
}


HRESULT Project::Scenario::Simulation_Clear()
{
	HRESULT hr = m_scenario->Simulation_Clear(), hr1;
	if (m_childArray.size())
	{
		int si;

		int thread_id = -1;
#pragma omp parallel for num_threads(CWorkerThreadPool::NumberIdealProcessors()) firstprivate(thread_id)
		for (si = 0; si < m_childArray.size(); si++) {
			if (thread_id == -1)
			{
				thread_id = omp_get_thread_num();
				CWorkerThread::native_handle_type thread = CWorkerThreadPool::GetCurrentThread();
				CWorkerThreadPool::SetThreadAffinityToMask(thread, thread_id);
			}
			Scenario* s = m_childArray[si];
			if (FAILED(hr1 = s->Simulation_Clear()))
				if (hr1 != ERROR_SCENARIO_BAD_STATE)	// ignore if it's already cleared (PSaaS sequence)
					hr = hr1;
			hr1 = s->m_weatherGrid->SetCache(layerThread(), 0, false);
		}

		for (auto ss : m_childArray)
			delete ss;
		m_childArray.clear();
	}
	else
		hr = m_weatherGrid->SetCache(layerThread(), 0, false);
	return hr;
}


auto Project::Scenario::GetPrimaryStream() const -> WeatherStream*
{
	boost::intrusive_ptr<CCWFGM_WeatherStream> stream;
	m_weatherGrid->get_PrimaryStream(&stream);
	if (!stream)
		return nullptr;
	std::uint32_t index;
	m_weatherGrid->IndexOfStream(stream.get(), &index);
	WeatherStream *s = m_streamList.IndexPtr(index);
	return s;
}


HRESULT Project::Scenario::SetPrimaryStream(WeatherStream *stream)
{
	if (stream)
		return m_weatherGrid->put_PrimaryStream(stream->m_stream.get());
	else
		return m_weatherGrid->put_PrimaryStream(nullptr);
}


struct wx_sort
{
	Project::WeatherStream* stream;
	Project::WeatherStation* station;
	double x, y;
	double distance;
};

static int compare_wx(void const* a, void const* b)
{
	wx_sort* wa = (wx_sort*)a;
	wx_sort* wb = (wx_sort*)b;
	double d = wa->distance - wb->distance;
	if (d < 0.0)
		return -1;
	if (d > 0.0)
		return 1;
	return 0;
}


ICWFGM_GridEngine *Project::Scenario::gridEngine() const
{
	boost::intrusive_ptr<ICWFGM_GridEngine> ge;
	Layer *layerThread;
	m_scenario->GetGridEngine(&layerThread, &ge);
	weak_assert(layerThread == this->layerThread());
	return ge.get();
}


bool Project::Scenario::AssignNewGrid(ICWFGM_GridEngine *oldgrid, ICWFGM_GridEngine *newgrid)
{
	boost::intrusive_ptr<ICWFGM_GridEngine> ge = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(m_weatherGrid);
	boost::intrusive_ptr<ICWFGM_GridEngine> te = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(m_temporalFilter);

	if (!m_layerThread)
	{
		HRESULT hr = m_scenarioCollection->m_project->m_layerManager->NewLayerThread(&m_layerThread);
		if (FAILED(hr))
			throw std::logic_error("Failed");

		ge->put_LayerManager(m_scenarioCollection->m_project->m_layerManager.get());
		te->put_LayerManager(m_scenarioCollection->m_project->m_layerManager.get());

		m_scenario->PutGridEngine(layerThread(), te.get());
		te->PutGridEngine(layerThread(), ge.get());
	}
	if (oldgrid)
		oldgrid->PutGridEngine(layerThread(), nullptr);
	if (newgrid)
		newgrid->put_LayerManager(m_scenarioCollection->m_project->m_layerManager.get());
	ge->PutGridEngine(nullptr, newgrid);
	te->PutGridEngine(nullptr, newgrid);

	if (newgrid)
		newgrid->PutGridEngine(layerThread(), ge.get());
	if (m_filterList.GetCount())
	{
		GridFilter *gf = m_filterList.LH_Tail()->LN_Ptr();
		gf->m_filter->PutGridEngine(layerThread(), newgrid);

	}
	else
	{
		ge->PutGridEngine(layerThread(), newgrid);
	}

	if ((newgrid) && (CWFGMProject::m_appMode > 0))
		m_weatherGrid->SetCache(layerThread(), 1, true);

	return true;
}


Project::Scenario::IgnitionOptions* Project::Scenario::AddFire(Fire *fire, bool constructing)
{
	if (!constructing)
	{
		HRESULT state = Simulation_State();
		if ((state == SUCCESS_SCENARIO_SIMULATION_RUNNING) || (state == SUCCESS_SCENARIO_SIMULATION_RESET))
		{
			weak_assert(false);
			return nullptr;
		}
	}
	if (IndexOfFire(fire) != static_cast<ULONG>(-1))
		return nullptr;
	HRESULT added = m_scenario->AddIgnition(fire->m_fire.get());
	if (SUCCEEDED(added) || (added == ERROR_SCENARIO_FIRE_ALREADY_ADDED))
	{
		IgnitionOptions* fn;
		if (!(fn = m_fireList.FindPtr(fire))) {
			fn = new IgnitionOptions;
			fn->LN_Ptr(fire);
			m_fireList.AddTail(fn);
		}
		return fn;
	}
	return nullptr;
}


bool Project::Scenario::RemoveFire(Fire *fire)
{
	IgnitionOptions *fn = m_fireList.FindPtr(fire);
	bool retval = false;
	if (fn)
	{
		retval = SUCCEEDED(m_scenario->RemoveIgnition(((Fire *)fn->ln_Ptr)->m_fire.get()));
		m_fireList.Remove(fn);
		delete fn;
	}
	return retval;
}


auto Project::Scenario::AddStream(WeatherStream *stream, bool constructing) -> WSNode*
{
	if (!constructing)
	{
		HRESULT state = Simulation_State();
		if ((state == SUCCESS_SCENARIO_SIMULATION_RUNNING) || (state == SUCCESS_SCENARIO_SIMULATION_RESET))
		{
			weak_assert(false);
			return nullptr;
		}
	}
	if (!stream)
		return nullptr;
	ULONG index = IndexOfStream(stream);
	if (index != (ULONG)-1)
		return nullptr;
	HRESULT added = m_weatherGrid->AddStream(stream->m_stream.get());
	if (SUCCEEDED(added) || (added == ERROR_WEATHER_STREAM_ALREADY_ADDED))
	{
		WSNode* sn;
		if (!(sn = m_streamList.FindPtr(stream))) {
			sn = new WSNode(m_timeManager);
			sn->ln_Ptr = stream;
			m_streamList.AddTail(sn);
		}
		return sn;
	}
	return nullptr;
}


HRESULT Project::Scenario::RemoveStream(WeatherStream *stream)
{
	WSNode *sn = m_streamList.FindPtr(stream);
	HRESULT hr;
	if (sn)
	{
		if (SUCCEEDED(hr = m_weatherGrid->RemoveStream(sn->LN_Ptr()->m_stream.get())))
		{
			m_streamList.Remove(sn);
			delete sn;
			return hr;
		}
		else
		{
			return hr;
		}
	}
	return E_UNEXPECTED;
}


HRESULT Project::Scenario::FilterStreams(const std::vector<WSNode*>& streams)
{
	std::vector<WeatherStream*> toRemove;
	WSNode* remaining = nullptr;
	auto primary = GetPrimaryStream();
	bool needsNewPrimary = false;
	WSNode* sn = m_streamList.LH_Head();
	while (sn->LN_Succ())
	{
		if (std::none_of(streams.begin(), streams.end(), [sn](WSNode* node) -> bool { return node->LN_Ptr() == sn->LN_Ptr(); }))
		{
			if (primary == sn->LN_Ptr())
				needsNewPrimary = true;
			toRemove.push_back(sn->LN_Ptr());
		}
		else if (!remaining)
			remaining = sn;
		sn = sn->LN_Succ();
	}

	for (auto _sn : toRemove)
		RemoveStream(_sn);
	if (remaining && needsNewPrimary)
		SetPrimaryStream(remaining->LN_Ptr());

	return S_OK;
}


HRESULT Project::Scenario::FilterFilters()
{
	//if it has a sub-name filter the filter list
	if (m_subName.length())
	{
		std::vector<GridFilter*> toRemove;
		GFNode* gn = m_filterList.LH_Head();
		std::string myName = m_subName;

		while (gn->LN_Succ())
		{
			if (gn->m_subNames.size() && std::none_of(gn->m_subNames.begin(), gn->m_subNames.end(), [&myName](const std::string& str) -> bool { return boost::equals(str, myName); }))
			{
				toRemove.push_back(gn->LN_Ptr());
			}
			gn = gn->LN_Succ();
		}

		for (auto _gn : toRemove)
			RemoveFilter(_gn);
	}

	return S_OK;
}


auto Project::Scenario::FilterAtIndex(ULONG index) const -> GridFilter*
{
	GridFilter *gf = m_filterList.IndexPtr(index);
	return gf;
}


ULONG Project::Scenario::IndexOfFilter(GridFilter *filter) const
{
	ULONG i = m_filterList.FindPtrIndex(filter);
	if (i == (ULONG)-1)
		return i;
	return i;
}


auto Project::Scenario::InsertFilter(GridFilter *filter, ULONG index, bool during_serialization) -> GFNode*
{
	if (!during_serialization)
	{
		HRESULT state = Simulation_State();
		if ((state == SUCCESS_SCENARIO_SIMULATION_RUNNING) || (state == SUCCESS_SCENARIO_SIMULATION_RESET))
			return nullptr;
	}
	if (IndexOfFilter(filter) != (ULONG)-1)
		return nullptr;

	if (!index)
	{
		boost::intrusive_ptr<ICWFGM_GridEngine> ge;
		m_weatherGrid->GetGridEngine(layerThread(), &ge);

										// get the original grid engine (could be weather filter)
		filter->m_filter->PutGridEngine(layerThread(), ge.get());// try to attach our original grid engine to the filter
		m_weatherGrid->PutGridEngine(layerThread(), filter->m_filter.get());

										// if we did, then attach our scenario to the new filter
		GFNode *pn = new GFNode;
		pn->LN_Ptr(filter);
		m_filterList.AddHead(pn);					// and record that this filter is on our list
		return pn;
	}
	else
	{
		GFNode *pn = m_filterList.IndexNode(index - 1);	// get the grid filter that we have to insert after
		if (pn)
		{
			GridFilter *gf = pn->LN_Ptr();
			boost::intrusive_ptr<ICWFGM_GridEngine> ge;
			gf->m_filter->GetGridEngine(layerThread(), &ge);
			filter->m_filter->PutGridEngine(layerThread(), ge.get());
			gf->m_filter->PutGridEngine(layerThread(), filter->m_filter.get());		// if we did, then update that filter to link to the new filter

			GFNode *pn1 = new GFNode;
			pn1->LN_Ptr(filter);
			m_filterList.Insert(pn1, pn);			// and record that this filter is on our list
			return pn1;
		}
	}
	return nullptr;
}


bool Project::Scenario::RemoveFilter(GridFilter *filter)
{
	HRESULT state = Simulation_State();
	if ((state == SUCCESS_SCENARIO_SIMULATION_RUNNING) || (state == SUCCESS_SCENARIO_SIMULATION_RESET))
		return false;
	GFNode *pn = m_filterList.FindPtr(filter);
	if (pn)
	{
		if (!pn->LN_Pred()->LN_Pred())
		{
			boost::intrusive_ptr<ICWFGM_GridEngine> ge;
			filter->m_filter->GetGridEngine(layerThread(), &ge);
			m_weatherGrid->PutGridEngine(layerThread(), ge.get());
										// take this filter out of the list of filters for the scenario
		}
		else
		{
			boost::intrusive_ptr<ICWFGM_GridEngine> ge;
			GridFilter *pred = pn->LN_Pred()->LN_Ptr();		// get a link to the previous grid filter for this scenario
			filter->m_filter->GetGridEngine(layerThread(), &ge);
			pred->m_filter->PutGridEngine(layerThread(), ge.get());	// take this filter out of the list of filters
		}
		filter->m_filter->PutGridEngine(layerThread(), nullptr);
		// delete the filter's association to this scenario
		m_filterList.Remove(pn);
		delete pn;							// do clean-up off our list
		return true;
	}
	return false;
}


bool Project::Scenario::AddVector(Vector *vector, bool during_serialization)
{
	if (!during_serialization)
	{
		HRESULT state = Simulation_State();
		if ((state == SUCCESS_SCENARIO_SIMULATION_RUNNING) || (state == SUCCESS_SCENARIO_SIMULATION_RESET))
		{
			weak_assert(false);
			return false;
		}
	}
	if (IndexOfVector(vector) != (ULONG)-1)
		return false;
	HRESULT hr;
	if ((SUCCEEDED(hr = m_scenario->AddVectorEngine(vector->m_filter.get()))) || (hr == ERROR_SCENARIO_VECTORENGINE_ALREADY_ADDED))
	{
		if (!m_vFilterList.FindPtr(vector))
		{
			RefNode<Vector> *pn = new RefNode<Vector>;
			pn->LN_Ptr(vector);
			m_vFilterList.AddTail(pn);
		}
		return true;
	}
	return false;
}


bool Project::Scenario::RemoveVector(Vector *vector)
{
	RefNode<Vector> *pn = m_vFilterList.FindPtr(vector);
	bool success = false;
	if (pn)
	{
		if (SUCCEEDED(m_scenario->RemoveVectorEngine(vector->m_filter.get())))
		{
			success = true;
			m_vFilterList.Remove(pn);
			delete pn;
		}
	}
	return success;
}


bool Project::Scenario::AddAsset(Asset* asset, ULONG operation, bool during_serialization)
{
	if (!during_serialization)
	{
		HRESULT state = Simulation_State();
		if ((state == SUCCESS_SCENARIO_SIMULATION_RUNNING) || (state == SUCCESS_SCENARIO_SIMULATION_RESET))
		{
			weak_assert(false);
			return false;
		}
	}
	if (IndexOfAsset(asset) != (ULONG)-1)
		return false;
	HRESULT hr;
	if ((SUCCEEDED(hr = m_scenario->AddAsset(asset->m_filter.get(), operation))) || (hr == ERROR_SCENARIO_ASSET_ALREADY_ADDED))
	{
		if (!m_assetList.FindPtr(asset))
		{
			RefNode<Asset>* pn = new RefNode<Asset>;
			pn->LN_Ptr(asset);
			m_assetList.AddTail(pn);
		}
		return true;
	}
	return false;
}


bool Project::Scenario::RemoveAsset(Asset* asset)
{
	RefNode<Asset>* pn = m_assetList.FindPtr(asset);
	BOOL success = false;
	if (pn)
	{
		if (SUCCEEDED(m_scenario->RemoveAsset(asset->m_filter.get())))
		{
			success = true;
			m_assetList.Remove(pn);
			delete pn;
		}
	}
	return success;
}


bool Project::Scenario::duplicateFireList(const Scenario &s)
{
	IgnitionOptions *node = s.m_fireList.LH_Head(), *nnode;
	while (node->LN_Succ())
	{
		if (!(nnode = AddFire(node->LN_Ptr(), true)))
			return false;
		node = node->LN_Succ();
	}
	return true;
}


bool Project::Scenario::duplicateVectorList(const Scenario &s)
{
	RefNode<Vector> *node = s.m_vFilterList.LH_Head();;
	while (node->LN_Succ()) {
		if (!AddVector(node->LN_Ptr(), true))
			return false;
		node = node->LN_Succ();
	}
	return true;
}


bool Project::Scenario::duplicateAssetList(const Scenario& s)
{
	std::uint32_t operation;
	if (SUCCEEDED(s.m_scenario->GetAssetOperation(nullptr, &operation)))
		m_scenario->SetAssetOperation(nullptr, operation);

	RefNode<Asset>* node = s.m_assetList.LH_Head();
	while (node->LN_Succ()) {
		s.m_scenario->GetAssetOperation(node->LN_Ptr()->m_filter.get(), &operation);
		if (!AddAsset(node->LN_Ptr(), operation, true))
			return false;
		node = node->LN_Succ();
	}
	return true;
}


bool Project::Scenario::duplicateTargetList(const Scenario& s)
{
	ICWFGM_Target* target = nullptr;
	unsigned long index = (unsigned long)-1, sub_index = (unsigned long)-1;
	if (SUCCEEDED(s.m_scenario->GetWindTarget(&target, &index, &sub_index)) && (target))
		m_scenario->SetWindTarget(target, index, sub_index);

	target = nullptr;
	index = (unsigned long)-1; sub_index = (unsigned long)-1;
	if (SUCCEEDED(s.m_scenario->GetVectorTarget(&target, &index, &sub_index)) && (target))
		m_scenario->SetVectorTarget(target, index, sub_index);

	return true;
}


bool Project::Scenario::duplicateStreamList(const Scenario &s)
{
	WSNode *node = s.m_streamList.LH_Head();
	while (node->LN_Succ())
	{
		WSNode* n = AddStream(node->LN_Ptr(), true);
		if (!n)
		{
			weak_assert(false);
			return false;
		}
		if ((n->m_valid = node->m_valid))
		{
			n->m_name = node->m_name;
			n->m_startTime = WTime(node->m_startTime);
			n->m_endTime = WTime(node->m_endTime);
		}
		node = node->LN_Succ();
	}
	return true;
}


bool Project::Scenario::duplicateFilterList(const Scenario &s)
{
	GFNode *node = s.m_filterList.LH_Head();
	USHORT i = 0;
	while (node->LN_Succ())
	{
		auto n = InsertFilter(node->LN_Ptr(), i, true);
		if (!n)
			return false;
		std::copy(node->m_subNames.begin(), node->m_subNames.end(), std::back_inserter(n->m_subNames));
		node = node->LN_Succ(); i++;
	}
	return true;
}


HRESULT Project::Scenario::ExportFireFronts(Fire* fires, WTime& start_time, WTime& end_time, VectorExportOptions* options, USHORT flags,
	const std::string& driver_name, const std::string& export_projection, const std::string& file_name,
	const TCHAR* fgm_name, const TCHAR* prometheus_version, ::UnitConvert::STORAGE_UNIT perimeter_units,
	::UnitConvert::STORAGE_UNIT area_units, ::UnitConversion* uc, const USHORT child_flags) const
{
	VectorExportOptions o;
	if (!options)
		options = &o;

	ULONG i, cnt;

	ScenarioExportRules rules;

	if ((options->PrometheusVersion) && (prometheus_version))
		rules.AddAttributeString(_T("VERSION"), prometheus_version);

	if ((options->FGMName) && (fgm_name))
		rules.AddAttributeString(_T("FGM"), fgm_name);

	if (options->ScenarioName)
		rules.AddAttributeString(_T("SCENARIO"), m_name.c_str());

	if (options->IgnitionName)
	{
		cnt = GetFireCount();
		for (i = 0; i < cnt; i++)
		{
			Fire *f = FireAtIndex(i);
			rules.AddAttributeString(f->m_fire.get(), _T("IGNITION"), f->m_name.c_str());
		}
	}

	if (options->IgnitionAttributes)
	{
		std::uint32_t ai, att_count;
		cnt = GetFireCount();
		for (i = 0; i < cnt; i++)
		{
			Fire* f = FireAtIndex(i);
			f->m_fire->GetIgnitionAttributeCount(&att_count);
			for (ai = 0; ai < att_count; ai++)
			{
				std::string str, str1;
				f->m_fire->GetIgnitionAttributeName(ai, &str);
				GDALVariant value;
				//don't allow custom attributes to use "Name", this is the only attribute
				//that is default exported already and it will confuse other exports
				if (boost::iequals(str, "Name"s))
				{
					str1 = "C_Name";
				}
				else if (boost::iequals(str, "area"s))
				{
					str1 = "C_Area";
				}
				else
					str1 = str;

				if (SUCCEEDED(f->m_fire->GetIgnitionAttributeValue(i, str, &value)))
				{
					if (std::holds_alternative<std::int32_t>(value))
					{
						rules.AddAttributeInt32(f->m_fire.get(), str1.c_str(), std::get<std::int32_t>(value));
					}
					else if (std::holds_alternative<std::int64_t>(value))
					{
						rules.AddAttributeInt64(f->m_fire.get(), str1.c_str(), std::get<std::int64_t>(value));
					}
					else if (std::holds_alternative<double>(value))
					{
						rules.AddAttributeDouble(f->m_fire.get(), str1.c_str(), std::get<double>(value));
					}
					else if (std::holds_alternative<std::string>(value))
					{
						std::string vstr = std::get<std::string>(value);
						rules.AddAttributeString(f->m_fire.get(), str1.c_str(), vstr.c_str());
					}
				}
			}
		}
	}

	if (options->IgnitionLocation)
	{
		rules.AddStatistic(_T("IGN_LAT"), CWFGM_FIRE_STAT_IGNITION_LATITUDE, 0);
		rules.AddStatistic(_T("IGN_LONG"), CWFGM_FIRE_STAT_IGNITION_LONGITUDE, 0);
	}

	if (options->MaxBurnDistance)
	{
		rules.AddStatistic(_T("BURN_DIST"), CWFGM_FIRE_STAT_MAXIMUM_BURN_DISTANCE, uc ? uc->DistanceDisplay() : 0);
	}

	if (options->DateTime)
	{
		rules.AddStatistic(_T("DATE"), CWFGM_FIRE_STAT_DATE, 0);
		rules.AddStatistic(_T("TIME"), CWFGM_FIRE_STAT_TIME, 0);
		if ((!_tcsicmp(driver_name.c_str(), _T("LIBKML"))) || (!_tcsicmp(driver_name.c_str(), _T("KML"))))
			rules.AddStatistic(_T("TIMESTAMP"), CWFGM_FIRE_STAT_DATETIME, 0);
	}

	if (options->AssetFirstArrivalTime)
	{
		rules.AddStatistic(_T("ASSET_TIME"), CWFGM_FIRE_STAT_ASSET_FIRST_ARRVIAL_TIME, 0);
		rules.AddStatistic(_T("ASSET_SECS"), CWFGM_FIRE_STAT_ASSET_FIRST_ARRVIAL_SECS, 0);
	}

	if (options->AssetArrivalCount)
		rules.AddStatistic(_T("ASSET_CNT"), CWFGM_FIRE_STAT_ASSET_ARRIVAL_COUNT, 0);

	if (options->FireSize)
		rules.AddStatistic(_T("AREA"), CWFGM_FIRE_STAT_AREA, area_units);

	if (options->TotalPerimeter)
	{
		if ((flags & SCENARIO_EXPORT_COMBINE_SET) && (!(flags & (SCENARIO_EXPORT_SUBSET_EXTERIOR | SCENARIO_EXPORT_SUBSET_ACTIVE))))
			rules.AddStatistic(_T("PERIMETER"), CWFGM_FIRE_STAT_EXTERIOR_PERIMETER, perimeter_units);
		else
			rules.AddStatistic(_T("PERIMETER"), CWFGM_FIRE_STAT_TOTAL_PERIMETER, perimeter_units);
	}

	if (options->ActivePerimeter)
	{
		if (boost::iequals(driver_name, "ESRI Shapefile"s))
			rules.AddStatistic(_T("ACTV_PERIM"), CWFGM_FIRE_STAT_ACTIVE_PERIMETER, perimeter_units);
		else
			rules.AddStatistic(_T("ACTIVE PERIMETER"), CWFGM_FIRE_STAT_ACTIVE_PERIMETER, perimeter_units);
	}

	if (options->IdentifyFinal)
	{
		if (boost::iequals(driver_name, "ESRI Shapefile"s))
			rules.AddStatistic(_T("FNL_PERIM"), CWFGM_FIRE_STAT_FINAL_PERIMETER, perimeter_units);
		else
			rules.AddStatistic(_T("FINAL PERIMETER"), CWFGM_FIRE_STAT_FINAL_PERIMETER, perimeter_units);
	}

	if (options->SimulationStatus) {
		rules.AddStatistic(_T("STATUS"), CWFGM_FIRE_STAT_SIMULATION_STATUS, 0);
	}

	if (options->WxValues)
	{
		WTime newTime = Simulation_StartTime();

		WTime ldt(newTime, WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST, -1),
			lst(newTime, WTIME_FORMAT_AS_LOCAL, -1),
			noonStdTime(newTime);

		noonStdTime.PurgeToDay(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
		if (lst != ldt)
			noonStdTime += newTime.GetTimeManager()->m_worldLocation.m_amtDST();
		noonStdTime += WTimeSpan(0, 12, 0, 0);

		WeatherStream* ws = GetPrimaryStream();
		IWXData wx;
		ws->GetInstantaneousValues(noonStdTime, 0, &wx, nullptr, nullptr);

		WTime start(newTime);
		ws->GetStartTime(start);
		for (LONG ii = 1; ii < 24; ii++)
		{
			IWXData wx2;
			WTime time(noonStdTime);
			time -= WTimeSpan(0, ii, 0, 0);
			if (time >= start)
			{
				if (ws->GetInstantaneousValues(time, 0, &wx2, NULL, NULL))
					wx.Precipitation += wx2.Precipitation;
				else
					break;
			}
			else
			{
				wx.Precipitation += ws->Initial_Rain();
				break;
			}
		}

		if (wx.WindDirection == -1.0)
		{
			wx.WindDirection = 0.0;
		}
		else
		{
			wx.WindDirection = CARTESIAN_TO_COMPASS_DEGREE(RADIAN_TO_DEGREE(wx.WindDirection));
			if ((wx.WindDirection <= 0.0000001) && (wx.WindSpeed != 0.0))
				wx.WindDirection = 360.0;
		}
		wx.RH *= 100.0;

		std::string date = newTime.ToString(WTIME_FORMAT_DATE | WTIME_FORMAT_STRING_YYYYhMMhDD | WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
		rules.AddAttributeString(_T("WX_D0_DATE"), date.c_str());
		double temp = wx.Temperature;
		if (uc)
			temp = uc->ConvertUnit(temp, STORAGE_FORMAT_CELSIUS, uc->TempDisplay());
		rules.AddAttributeDouble(_T("WX_D0_Temp"), temp);
		rules.AddAttributeDouble(_T("WX_D0_RH"), wx.RH);
		temp = wx.WindSpeed;
		if (uc)
			temp = uc->ConvertUnit(temp, STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay());
		rules.AddAttributeDouble(_T("WX_D0_WS"), temp);
		rules.AddAttributeDouble(_T("WX_D0_WD"), wx.WindDirection);
		temp = wx.Precipitation;
		if (uc)
			temp = uc->ConvertUnit(temp, STORAGE_FORMAT_MM, uc->SmallMeasureDisplay());
		rules.AddAttributeDouble(_T("WX_D0_Rain"), temp);
	}

	if (options->FWIValues)
	{
		WTime newTime = Simulation_StartTime();

		WTime ldt(newTime, WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST, -1),
			lst(newTime, WTIME_FORMAT_AS_LOCAL, -1),
			noonStdTime(newTime);

		noonStdTime.PurgeToDay(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
		if (lst != ldt)
			noonStdTime += newTime.GetTimeManager()->m_worldLocation.m_amtDST();
		noonStdTime += WTimeSpan(0, 12, 0, 0);

		WeatherStream* ws = GetPrimaryStream();
		DFWIData dfwi;
		if (ws)
			ws->GetInstantaneousValues(noonStdTime, 0, nullptr, nullptr, &dfwi);
		else {
			dfwi.dBUI = dfwi.dDC = dfwi.dDMC = dfwi.dFFMC = dfwi.dFWI = dfwi.dISI = -1.0;
		}

		if (!rules.FindAttributeName(_T("WX_D0_DATE")))
		{
			std::string date = newTime.ToString(WTIME_FORMAT_DATE | WTIME_FORMAT_STRING_YYYYhMMhDD | WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
			rules.AddAttributeString(_T("WX_D0_DATE"), date.c_str());
		}
		rules.AddAttributeDouble(_T("WX_D0_FFMC"), dfwi.dFFMC);
		rules.AddAttributeDouble(_T("WX_D0_DMC"), dfwi.dDMC);
		rules.AddAttributeDouble(_T("WX_D0_DC"), dfwi.dDC);
		rules.AddAttributeDouble(_T("WX_D0_dISI"), dfwi.dISI);
		rules.AddAttributeDouble(_T("WX_D0_dBUI"), dfwi.dBUI);
		rules.AddAttributeDouble(_T("WX_D0_dFWI"), dfwi.dFWI);
	}

	if (options->Comment1.length())				// DESCRIPTION is a reserved word that hides the other properties in google earth, so put it to COMMENTS
		rules.AddAttributeString(_T("COMMENTS"), options->Comment1.c_str());

	if (!boost::iequals(driver_name, "ESRI Shapefile"s))
	{
		rules.AddAttributeString(_T("COLOR"), _T("ff0000ff"));
		rules.AddAttributeString(_T("WIDTH"), _T("1.0"));
	}

	if (m_childArray.size())
	{
		((Project::Scenario *)this)->m_appendVectors = nullptr;
		rules.AddOperation(true, false, (ULONGLONG)&m_appendVectors);
	}
	else if (m_parent)
		rules.AddOperation(true, (child_flags & 0x0002) ? true : false, (ULONGLONG)&m_parent->m_appendVectors);	// 2nd lowest order bit says whether it's the last one or not

	const WTimeManager *tm = start_time.GetTimeManager();
	HRESULT hr;
	if (fires)
		hr = m_scenario->ExportFires(fires->m_fire.get(), start_time, end_time, flags, driver_name, export_projection, file_name, &rules);
	else
		hr = m_scenario->ExportFires(nullptr, start_time, end_time, flags, driver_name, export_projection, file_name, &rules);

	// low order bit says to loop over children
	if (SUCCEEDED(hr) && m_childArray.size() && (child_flags & 0x0001))
	{
		for (auto s : m_childArray)
		{
			// last one
			if (!s->LN_Succ()->LN_Succ())
			{
				rules.AddOperation(true, true, (ULONGLONG)&m_appendVectors);
			}
			if (fires)
				hr = s->m_scenario->ExportFires(fires->m_fire.get(), start_time, end_time, flags, driver_name, export_projection, file_name, &rules);
			else
				hr = s->m_scenario->ExportFires(nullptr, start_time, end_time, flags, driver_name, export_projection, file_name, &rules);
		}
	}

	if (SUCCEEDED(hr))
	{
		start_time.SetTimeManager(tm);
		end_time.SetTimeManager(tm);
	}
	return hr;
}


HRESULT Project::Scenario::ExportCriticalPath(const Asset* a, const std::uint32_t index, const std::uint16_t flags,
	const char* driver_name, const char* export_projection, const char* file_path) const {
	ScenarioExportRules rules;

	rules.AddAttributeString(_T("SCENARIO"), m_name.c_str());
	if (a) {
		rules.AddAttributeString(_T("ASSET"), a->Name().c_str());
		rules.AddAttributeInt32(_T("ASSET IDX"), index);
	}
	rules.AddStatistic(_T("Timestamp"), CWFGM_FIRE_STAT_DATETIME, 0);
	rules.AddStatistic(_T("ROS"), CWFGM_FIRE_STAT_ROS, 0);
	rules.AddStatistic(_T("FI"), CWFGM_FIRE_STAT_FI, 0);
	rules.AddStatistic(_T("CFB"), CWFGM_FIRE_STAT_CFB, 0);
	rules.AddStatistic(_T("TFC"), CWFGM_FIRE_STAT_TFC, 0);

	return m_scenario->ExportCriticalPath(a->m_filter.get(), index, flags, driver_name, export_projection, file_path, &rules);
}


HRESULT Project::Scenario::BuildCriticalPath(const Asset* a, const std::uint32_t index, const std::uint16_t flags, MinListTempl<CriticalPath>& polyset) const {
	ScenarioExportRules rules;

	rules.AddAttributeString(_T("SCENARIO"), m_name.c_str());
	if (a) {
		rules.AddAttributeString(_T("ASSET"), a->Name().c_str());
		rules.AddAttributeInt32(_T("ASSET IDX"), index);
	}
	rules.AddStatistic(_T("Timestamp"), CWFGM_FIRE_STAT_DATETIME, 0);
	rules.AddStatistic(_T("ROS"), CWFGM_FIRE_STAT_ROS, 0);
	rules.AddStatistic(_T("FI"), CWFGM_FIRE_STAT_FI, 0);
	rules.AddStatistic(_T("CFB"), CWFGM_FIRE_STAT_CFB, 0);
	rules.AddStatistic(_T("TFC"), CWFGM_FIRE_STAT_TFC, 0);

	HRESULT hr = m_scenario->BuildCriticalPath(a->m_filter.get(), index, flags, polyset, &rules);
	if (SUCCEEDED(hr)) {
		if (flags & 1) {		// if we're going to convert to lat/lon (from UTM)
			PolymorphicAttribute var;
			if (FAILED(hr = m_scenarioCollection->m_project->gridEngine()->GetAttribute(0, CWFGM_GRID_ATTRIBUTE_SPATIALREFERENCE, &var)))
				return hr;
			std::string projection = std::get<std::string>(var);

			OGRSpatialReferenceH oSourceSRS = CCoordinateConverter::CreateSpatialReferenceFromWkt(projection.c_str());
			OGRSpatialReferenceH oTargetSRS = CCoordinateConverter::CreateSpatialReferenceFromStr(SRS_WKT_WGS84_LAT_LONG);

			CriticalPath* rn = (CriticalPath *)polyset.LH_Head();
			while (rn->LN_Succ()) {
				rn->ConvertPoly(oSourceSRS, oTargetSRS);
				rn = rn->LN_Succ();
			}

			if (oSourceSRS)
				CCoordinateConverter::DestroySpatialReference(oSourceSRS);
			if (oTargetSRS)
				CCoordinateConverter::DestroySpatialReference(oTargetSRS);
		}
	}
	return hr;
}


bool Project::Scenario::GetGreenupTimes(WTimeSpan *start, WTimeSpan *end, bool *effective) {
	return getTimes(start, end, CWFGM_SCENARIO_OPTION_GREENUP, effective);
}


bool Project::Scenario::GetStandingGrassTimes(WTimeSpan *start, WTimeSpan *end, bool *effective)
{
	return getTimes(start, end, CWFGM_SCENARIO_OPTION_GRASSPHENOLOGY, effective);
}


bool Project::Scenario::getTimes(WTimeSpan *start, WTimeSpan *end, USHORT option, bool *effective)
{
	USHORT i, count;
	WTimeVariant time;
	PolymorphicAttribute val;
	bool val_valid;
	m_temporalFilter->Count(option, &count);
	bool on_set = false;
	bool off_set = false;
	for (i = 0; i < count; i++)
	{
		if (FAILED(m_temporalFilter->TimeAtIndex(option, i, &time)))
			return false;
		if (FAILED(m_temporalFilter->GetOptionKey(option, time, &val, &val_valid)))
			return false;
		if (val_valid)
		{
			bool on;
			VariantToBoolean_(val, &on);
			
			HSS_Time::WTimeSpan ts;
			try { ts = std::get<WTimeSpan>(time); } catch (std::bad_variant_access &) { weak_assert(false); return false; };

			if (on)
			{
				on_set = true;
				*start = ts;
			}
			else
			{
				off_set = true;
				*end = ts;
			}
		}
	}

	if (!on_set)
	{
		start->SetTotalSeconds(0);
	}
	if (!off_set)
	{
		WTime endtime = Simulation_EndTime();
		long days = endtime.IsLeapYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST) ? 366 : 365;
		end->SetTotalSeconds((days + 1) * 24 * 60 * 60 - 1);
	}
	*effective = on_set && off_set;
	return true;
}


bool Project::Scenario::SetGreenupTimes(const WTimeSpan &start, const WTimeSpan &end, bool effective)
{
	return setTimes(start, end, CWFGM_SCENARIO_OPTION_GREENUP, effective);
}


bool Project::Scenario::SetStandingGrassTimes(const WTimeSpan &start, const WTimeSpan &end, bool effective)
{
	return setTimes(start, end, CWFGM_SCENARIO_OPTION_GRASSPHENOLOGY, effective);
}


bool Project::Scenario::setTimes(const WTimeSpan &start, const WTimeSpan &end, USHORT option, bool effective)
{
	USHORT i, count;
	WTimeVariant time;
	PolymorphicAttribute val;
	bool val_valid;
	m_temporalFilter->Count(option, &count);
	bool on_set = false;
	bool off_set = false;
	for (i = 0; i < count; i++)
	{
		if (FAILED(m_temporalFilter->TimeAtIndex(option, i, &time)))
			return false;
		if (FAILED(m_temporalFilter->GetOptionKey(option, time, &val, &val_valid)))
			return false;
		if (val_valid)
		{
			bool on;
			VariantToBoolean_(val, &on);
			
			WTimeSpan wtime;
			try { wtime = std::get<WTimeSpan>(time); } catch (std::bad_variant_access &) { weak_assert(false); return false; };

			if (on)
			{
				on_set = true;
				if (start != wtime)
				{
					if (FAILED(m_temporalFilter->SetOptionKey(option, time, val, 0)))
						return false;
					if (FAILED(m_temporalFilter->SetOptionKey(option, start, val, effective)))
						return false;
				}
			}
			else
			{
				off_set = true;
				if (end != wtime)
				{
					if (FAILED(m_temporalFilter->SetOptionKey(option, time, val, 0)))
						return false;
					if (FAILED(m_temporalFilter->SetOptionKey(option, end, val, effective)))
						return false;
				}
			}
		}
	}
	if (!on_set)
	{
		val = true;
		if (FAILED(m_temporalFilter->SetOptionKey(option, start, val, effective)))
			return false;
	}
	if (!off_set)
	{
		val = false;
		if (FAILED(m_temporalFilter->SetOptionKey(option, end, val, effective)))
			return false;
	}
	return true;
}


bool Project::Scenario::GetCuringDetails(CuringOptions &curing)
{
	CuringOptions c(m_temporalFilter.get());
	curing = c;
	return (c.curing.GetCount() > 0);
}


bool Project::Scenario::SetCuringDetails(const CuringOptions &curing)
{
	return curing.SaveToScenario(m_temporalFilter.get());
}
