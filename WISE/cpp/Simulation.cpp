/**
 * WISE_Application: Simulation.cpp
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
#endif //_DLL
#include "WISE.h"
#include "VectorExportOptions.h"
#include "PrintReportOptions.h"
#include "Dlgcnvt.h"
#include "misc_native.h"
#include "StatsBuilder.h"
#include "kmllib.h"
#include "Stats.h"
#include "str_printf.h"
#include "pevents.h"
#include "COMInit.h"

#ifdef __GNUC__
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

#ifdef _MSC_VER
#include <psapi.h>
#endif

#include <vector>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/chrono.hpp>

#if !defined(__USE_GSL) && __has_include("gsl/gsl")
#include <gsl/gsl>
#define __USE_GSL 1
#endif

#include "filesystem.hpp"
#include "semaphore.h"


using namespace std::string_literals;

// the code below has been morphed from the code found in CommonScenarioViewImpl.cpp

#define RETURN_MSG(hr, msg)	{ last_error_msg = ErrorMsg(hr, msg); last_error = hr; return (hr); }


IMPLEMENT_OBJECT_CACHE_MT_NO_TEMPLATE(SimMessage, SimMessage, 1024*1024/sizeof(SimMessage), true, 16)


struct worker_struct
{
	CWorkerThread *worker;
	SPARCS_P *sp;
	Project::Scenario *scenario;
	std::uint32_t s_index;
	std::uint32_t n_index;
	std::uint32_t b_index;
	CRWThreadSemaphore *m_lock;
	MinListTempl<SimMessage> *m_list;
	neosmart::neosmart_event_t m_event;
	std::string m_clockStr;
	HRESULT m_state;
	std::uint32_t groupAffinity;
	SummaryType m_justSummary;
	ICWFGM_FWI* m_fwi;
};


/// <summary>
/// Details about scenarios that should be run for a single index
/// when using the external index load balancing type.
/// </summary>
class ScenarioRun
{
	std::vector<Project::Scenario*> scenarios;
	size_t index = 0;

    public:
	/// <summary>
	/// Add a new scenario to run as part of this entry.
	/// </summary>
	void addScenario(Project::Scenario* scenario) { this->scenarios.push_back(scenario); }

	/// <summary>
	/// Get the next scenario to run. Will return null if there are no scenarios left to run.
	/// </summary>
	Project::Scenario* takeScenario() { if (index >= scenarios.size()) return nullptr; return scenarios[index++]; }

	/// <summary>
	/// Check to see if there are any scenarios left to run.
	/// </summary>
	bool moreScenarios() { return index < scenarios.size(); }

	/// <summary>
	/// Check to see if all of the stored scenarios have already been run.
	/// </summary>
	bool isEmpty() { return index >= scenarios.size(); }

	ScenarioRun() = default;

	ScenarioRun(Project::Scenario* scenario) { this->scenarios.push_back(scenario); }

	void setIndex(int i) { index = i; }
	size_t getIndex() const { return index; }
};


static CThreadCriticalSection *__thread_lock;
static volatile std::uint32_t __thread_start_count = 0;
static volatile std::uint32_t * volatile __group_threads = nullptr;


static void exportReport(int& s_export_i, const int s_export_cnt, worker_struct* ws, SimMessage* sm);
static void exportReport(const Project::SummaryOutput& out, Project::SummaryCollection* collection, worker_struct* ws, SimMessage* sm, Project::Scenario* s);
static void exportStats(const int s_export_i, const int s_export_cnt, int& c_export_i, const int c_export_cnt, worker_struct* ws, SimMessage* sm);
static void exportAssetStats(const int f_export_i, const int f_export_cnt, int& c_export_i, const int c_export_cnt, worker_struct* ws, SimMessage* sm);
static void exportWxGrids(const int ac_export_i, const int ac_export_cnt, int& w_export_i, const int w_export_cnt, worker_struct* ws, SimMessage* sm);
static void exportFuelGrids(const int c_export_i, const int c_export_cnt, int& f_export_i, const int f_export_cnt, worker_struct* ws, SimMessage* sm);
static void exportVectors(const int w_export_i, const int w_export_cnt, worker_struct* ws, int& v_export_i, const int v_export_cnt, SimMessage* sm);
static void exportGrids(const int v_export_i, const int v_export_cnt, int& g_export_i, const int g_export_cnt, worker_struct* ws, SimMessage* sm);


void createDirectory(const std::string& filename)
{
	auto path = fs::absolute(filename).parent_path();
	try
	{
		fs::create_directories(path);
	}
	catch (std::exception &e) {}
}

static void decrementThread(worker_struct * ws);


/// <summary>
/// Check to see if a given scenario name has cumulative outputs or only single (sub)scenario
/// outputs. This will return true for scenarios that have no sub-scenarios.
/// </summary>
/// <returns>True if the scenario has cumulative outputs, false if it has no outputs or outputs for a single (sub)scenario.</returns>
static bool hasCumulativeOutputs(const Project::FGMOutputs& outputs, const std::string& scenario)
{
	for (auto& grid : outputs.grids)
	{
		if (boost::equals(grid.scenarioName, scenario))
		{
			if (grid.subScenarioNames.size() == 0)
				return true;
		}
	}
	for (auto& vector : outputs.vectors)
	{
		if (boost::equals(vector.scenarioName, scenario))
		{
			if (vector.subScenarioNames.size() == 0)
				return true;
		}
	}
	return false;
}


static UINT AFX_CDECL worker_thread(void *parm)
{
#if _DLL
		hss::InitializeCOM(COINIT_MULTITHREADED);
#endif
		struct worker_struct* ws = (struct worker_struct*)parm;
		ws->worker->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);

		std::uint16_t state = 0;
		std::uint32_t grid_export = 0, grid_complete = 0;
		std::uint32_t vector_export = 0, vector_complete = 0;
		std::uint32_t csv_export = 0, csv_complete = 0;

		int s_export_i = 0, g_export_i = -1, v_export_i = -1, w_export_i = -1, c_export_i = -1, ac_export_i = -1, f_export_i = -1;
		const int s_export_cnt = (int)ws->sp->m_project->m_outputs.summaries.size(),
			g_export_cnt = (int)ws->sp->m_project->m_outputs.grids.size(),
			v_export_cnt = (int)ws->sp->m_project->m_outputs.vectors.size(),
			w_export_cnt = (int)ws->sp->m_project->m_outputs.grids.size(),
			c_export_cnt = (int)ws->sp->m_project->m_outputs.stats.size(),
			ac_export_cnt = (int)ws->sp->m_project->m_outputs.assetOutputs.size(),
			f_export_cnt = (int)ws->sp->m_project->m_outputs.fuelgrids.size();
		bool any_export = (s_export_cnt + g_export_cnt + v_export_cnt + w_export_cnt + c_export_cnt + ac_export_cnt + f_export_cnt) ? true : false;

		WTime currentTime(0, nullptr);

		if (ws->m_justSummary != SummaryType::NONE) {
			SimMessage* sm = new SimMessage;
			sm->stepInfo.scenario = ws->scenario;
			sm->stepInfo.s_index = ws->s_index;
			sm->stepInfo.timestamp = 0;
			sm->stepInfo.retval = S_OK;
			sm->stepInfo.finished = TRUE;

			Project::SummaryOutput out;
			out.applicationName = true;
			out.fbp = true;
			out.fbpPatches = true;
			std::string ext;
			if (ws->m_justSummary == SummaryType::TEXT)
				ext = "txt";
			else if (ws->m_justSummary == SummaryType::BINARY)
				ext = "bin";
			else
				ext = "json";
			if (ws->s_index != (std::uint32_t)-1)
				out.filename = (boost::format("summaries/%s_%d_%d.%s") % ws->sp->m_id % ws->n_index % ws->s_index % ext).str();
			else
				out.filename = (boost::format("summaries/%s_%d.%s") % ws->sp->m_id % ws->n_index % ext).str();
			out.geoData = true;
			out.assetData = true;
			out.ignitions = true;
			out.inputs = true;
			out.landscape = true;
			out.outputScenarioName = true;
			out.scenarioComments = true;
			out.shouldStream = false;
			out.wxPatches = true;
			out.wxStreams = true;

			exportReport(out, nullptr, ws, sm, ws->scenario);

			decrementThread(ws);
			ws->scenario->Simulation_Clear();

			ws->m_lock->Lock();
			ws->m_list->AddTail(sm);
			ws->m_lock->Unlock();
			neosmart::SetEvent(ws->m_event);
		}
		else if (!any_export) {
			SimMessage* sm = new SimMessage;
			sm->stepInfo.scenario = ws->scenario;
			sm->stepInfo.s_index = ws->s_index;
			sm->stepInfo.timestamp = 0;
			sm->stepInfo.retval = S_OK;
			sm->stepInfo.finished = TRUE;

			decrementThread(ws);
			ws->m_lock->Lock();
			ws->m_list->AddTail(sm);
			ws->m_lock->Unlock();
			SetEvent(ws->m_event);
		}
		else while (true) {
			HRESULT retval;
			SimMessage* sm;

			if (state == 0)
			{
				try
				{
					retval = ws->scenario->Simulation_Step();
				}
				catch (...)
				{
					weak_assert(false);
					retval = SUCCESS_SCENARIO_SIMULATION_COMPLETE;
				}

				currentTime = ws->scenario->Simulation_CurrentTime();

				sm = new SimMessage;
				sm->stepInfo.scenario = ws->scenario;
				sm->stepInfo.s_index = ws->s_index;
				sm->stepInfo.timestamp = currentTime.GetTotalSeconds();
				sm->stepInfo.retval = retval;

				if (ws->worker->ShutdownRequested() || ws->scenario->m_simStatus == 1)
				{
					sm->stepInfo.finished = true;
					decrementThread(ws);
				}
				else if ((retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE) ||
					(retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_EXTENTS) ||
					(retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_ASSET) ||
					(retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI90) ||
					(retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI95) ||
					(retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI100) ||
					(retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_RH) ||
					(retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_PRECIP)/* ||
					(retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_AREA) ||
					(retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_BURNDISTANCE)*/)
				{
					try {
						state = 1;				// going to now start to export data

						PolymorphicAttribute v;
						double val;
						ULONGLONG uval;
						WTime endTime = ws->scenario->Simulation_EndTime();
						ws->scenario->GetStats((std::uint32_t)-1, endTime, CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_REALTIME, ws->sp->m_project->m_timestepSettings.discretize, &v);
						VariantToDouble_(v, &val);
#ifdef _NO_MFC
						WTimeSpan nts(gsl::narrow_cast<std::int64_t>(val), false);
#else
#ifdef _MSC_VER
						COleDateTimeSpan tspan(val);
						WTimeSpan nts(tspan);
#else
						WTimeSpan nts((std::int64_t)(val * 24.0 * 60.0 * 60.0));
#endif
#endif
						sm->stepInfo.duration = nts;

						ws->scenario->GetStats((std::uint32_t)-1, endTime, CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_TICKS, ws->sp->m_project->m_timestepSettings.discretize, &v);
						VariantToUInt64_(v, &uval);
						sm->stepInfo.ticks = uval;

						sm->stepInfo.finished = false;
						decrementThread(ws);
					}
					catch (std::exception&)
					{
						weak_assert(false);
						sm->stepInfo.finished = true;
					}
				}
				else if (FAILED(retval)) {
					sm->stepInfo.finished = true;
					decrementThread(ws);
				}
				else
					sm->stepInfo.finished = false;
			}
			else {
				sm = new SimMessage;
				sm->stepInfo.scenario = ws->scenario;
				sm->stepInfo.s_index = ws->s_index;
				sm->stepInfo.timestamp = 0;
				sm->stepInfo.retval = S_OK;
				if (ws->worker->ShutdownRequested() || ws->scenario->m_simStatus == 1)
				{
					sm->stepInfo.finished = true;
				}
				else
				{
					try {
						exportReport(s_export_i, s_export_cnt, ws, sm);
						exportStats(s_export_i, s_export_cnt, c_export_i, c_export_cnt, ws, sm);
						exportFuelGrids(c_export_i, c_export_cnt, f_export_i, f_export_cnt, ws, sm);
						exportAssetStats(f_export_i, f_export_cnt, ac_export_i, ac_export_cnt, ws, sm);
						exportWxGrids(ac_export_i, ac_export_cnt, w_export_i, w_export_cnt, ws, sm);
						exportVectors(w_export_i, w_export_cnt, ws, v_export_i, v_export_cnt, sm);
						exportGrids(v_export_i, v_export_cnt, g_export_i, g_export_cnt, ws, sm);

						if ((g_export_cnt == g_export_i) &&
							(f_export_cnt == f_export_i) &&
							(c_export_cnt == c_export_i) &&
							(w_export_cnt == w_export_i) &&
							(v_export_cnt == v_export_i) &&
							(s_export_cnt == s_export_i) &&
							(ac_export_cnt == ac_export_i))
							sm->stepInfo.finished = true;
						else
							sm->stepInfo.finished = false;
					}
					catch (std::exception&)
					{
						weak_assert(false);
						sm->stepInfo.finished = true;
					}
				}
			}

			if (sm->stepInfo.timestamp != 0)
			{
				WTime time(sm->stepInfo.timestamp, sm->stepInfo.scenario->Simulation_StartTime().GetTimeManager());
				for (int i = 0; i < ws->sp->m_project->m_timestepSettings.outputStatisticsCount(); i++)
				{
					PolymorphicAttribute stat;
					auto out = ws->sp->m_project->m_timestepSettings.outputStatistic(i);
					if (out == Service::Statistic::BURNED_AREA)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_AREA, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("BURNED_AREA"), temp));
					}
					else if (out == Service::Statistic::TOTAL_PERIMETER)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_TOTAL_PERIMETER, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("TOTAL_PERIMETER"), temp));
					}
					else if (out == Service::Statistic::EXTERIOR_PERIMETER)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_EXTERIOR_PERIMETER, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("EXTERIOR_PERIMETER"), temp));
					}
					else if (out == Service::Statistic::ACTIVE_PERIMETER)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_ACTIVE_PERIMETER, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("ACTIVE_PERIMETER"), temp));
					}
					else if (out == Service::Statistic::TOTAL_PERIMETER_CHANGE)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_TOTAL_PERIMETER_CHANGE, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("TOTAL_PERIMETER_CHANGE"), temp));
					}
					else if (out == Service::Statistic::TOTAL_PERIMETER_GROWTH)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_TOTAL_PERIMETER_GROWTH, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("TOTAL_PERIMETER_GROWTH"), temp));
					}
					else if (out == Service::Statistic::EXTERIOR_PERIMETER_CHANGE)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_EXTERIOR_PERIMETER_CHANGE, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("EXTERIOR_PERIMETER_CHANGE"), temp));
					}
					else if (out == Service::Statistic::EXTERIOR_PERIMETER_GROWTH)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_EXTERIOR_PERIMETER_GROWTH, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("EXTERIOR_PERIMETER_GROWTH"), temp));
					}
					else if (out == Service::Statistic::ACTIVE_PERIMETER_CHANGE)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_ACTIVE_PERIMETER_CHANGE, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("ACTIVE_PERIMETER_CHANGE"), temp));
					}
					else if (out == Service::Statistic::ACTIVE_PERIMETER_GROWTH)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_ACTIVE_PERIMETER_GROWTH, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("ACTIVE_PERIMETER_GROWTH"), temp));
					}
					else if (out == Service::Statistic::AREA_CHANGE)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_AREA_CHANGE, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("AREA_CHANGE"), temp));
					}
					else if (out == Service::Statistic::AREA_GROWTH)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_AREA_GROWTH, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("AREA_GROWTH"), temp));
					}
					else if (out == Service::Statistic::NUM_VERTICES)
					{
						UINT temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_NUM_POINTS, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToUInt32_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("NUM_VERTICES"), temp));
					}
					else if (out == Service::Statistic::NUM_ACTIVE_POINTS)
					{
						UINT temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_NUM_ACTIVE_POINTS, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToUInt32_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("NUM_ACTIVE_POINTS"), temp));
					}
					else if (out == Service::Statistic::CUMULATIVE_NUM_ACTIVE_POINTS)
					{
						UINT temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_CUMULATIVE_NUM_ACTIVE_POINTS, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToUInt32_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("CUMULATIVE_NUM_ACTIVE_POINTS"), temp));
					}
					else if (out == Service::Statistic::CUMULATIVE_NUM_POINTS)
					{
						UINT temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_CUMULATIVE_NUM_POINTS, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToUInt32_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("CUMULATIVE_NUM_POINTS"), temp));
					}
					else if (out == Service::Statistic::NUM_FRONTS)
					{
						UINT temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_NUM_FRONTS, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToUInt32_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("NUM_FRONTS"), temp));
					}
					else if (out == Service::Statistic::NUM_ACTIVE_FRONTS)
					{
						UINT temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_NUM_ACTIVE_FRONTS, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToUInt32_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("NUM_ACTIVE_FRONTS"), temp));
					}
					else if (out == Service::Statistic::ROS)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_ROS, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("ROS"), temp));
					}
					else if (out == Service::Statistic::CFB)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_CFB, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("CFB"), temp));
					}
					else if (out == Service::Statistic::CFC)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_CFC, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("CFC"), temp));
					}
					else if (out == Service::Statistic::SFC)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_SFC, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("SFC"), temp));
					}
					else if (out == Service::Statistic::TFC)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_TFC, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("TFC"), temp));
					}
					else if (out == Service::Statistic::FI)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_FI, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("FI"), temp));
					}
					else if (out == Service::Statistic::HFI)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_HFI, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("HFI"), temp));
					}
					else if (out == Service::Statistic::HCFB)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_HCFB, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("HCFB"), temp));
					}
					else if (out == Service::Statistic::FLAMELENGTH)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_FLAMELENGTH, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("FLAMELENGTH"), temp));
					}
					else if (out == Service::Statistic::TIMESTEP_REALTIME)
					{
						ULONGLONG temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_TIMESTEP_REALTIME, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToUInt64_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("TIMESTEP_REALTIME"), (std::uint64_t)temp));
					}
					else if (out == Service::Statistic::TIMESTEP_CUMULATIVE_REALTIME)
					{
						double temp;
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_REALTIME, ws->sp->m_project->m_timestepSettings.discretize, &stat)) && SUCCEEDED(VariantToDouble_(stat, &temp)))
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("TIMESTEP_CUMULATIVE_REALTIME"), temp));
					}
					else if (out == Service::Statistic::TIMESTEP_CUMULATIVE_BURNING_SECS)
					{
						if (SUCCEEDED(ws->scenario->GetStats(-1, time, CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_BURNING_SECS, ws->sp->m_project->m_timestepSettings.discretize, &stat)))
						{
							WTimeSpan span;
							VariantToTimeSpan_(stat, &span);
							sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("TIMESTEP_CUMULATIVE_BURNING_SECS"), span.ToString(WTIME_FORMAT_STRING_ISO8601)));
						}
					}
					else if (out == Service::Statistic::SCENARIO_CURRENT_TIME)
					{
						sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("SCENARIO_CURRENT_TIME"), time.ToString(WTIME_FORMAT_STRING_ISO8601)));
					}
					else if (out == Service::Statistic::SCENARIO_NAME)
					{
						sm->stepInfo.statistics.emplace_back(std::make_tuple(std::string("SCENARIO_NAME"), sm->stepInfo.scenario->m_name));
					}
				}
			}

			bool break_me = sm->stepInfo.finished;

			if (break_me)
				ws->scenario->Simulation_Clear();

			ws->m_lock->Lock();
			ws->m_list->AddTail(sm);
			ws->m_lock->Unlock();
			neosmart::SetEvent(ws->m_event);

			if (break_me)
				break;
		}

#if _DLL
		CoUninitialize();
#endif
	return 0;
}


void decrementThread(worker_struct * ws)
{
	__thread_lock->Lock();

#ifdef _DEBUG
	if (!__group_threads[ws->groupAffinity])
		std::cout << "bad!" << std::endl;
	else
#endif

	InterlockedDecrement(&__group_threads[ws->groupAffinity]);
	__thread_lock->Unlock();
}


void exportGrids(const int v_export_i, const int v_export_cnt, int& g_export_i, const int g_export_cnt, worker_struct* ws, SimMessage* sm) {
	if (v_export_i == v_export_cnt) {
		if (g_export_i == -1)
			g_export_i++;
		while (g_export_i < g_export_cnt) {
			auto& out = ws->sp->m_project->m_outputs.grids[g_export_i++];
			std::string subName = ws->scenario->m_subName;
			if (boost::iequals(ws->scenario->m_name, out.scenarioName) &&
				//this is either a cumulative output or an output for this specific sub-scenario
				(out.subScenarioNames.size() == 0 || std::find(out.subScenarioNames.begin(), out.subScenarioNames.end(), subName) != out.subScenarioNames.end()) &&
				(out.outputs[0].statistic >= 10))
			{
				// perform grid export
				WTime sss = out.getStartExportTime(subName);
				WTime ss = out.getExportTime(subName);
				if (!sss.GetTotalMicroSeconds() || !sss.IsValid())
					sss = ws->scenario->Simulation_StartTime();
				sss.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				ss.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				if ((sss == ws->scenario->Simulation_EndTime()) && (ws->scenario->Simulation_EndTime() > ws->scenario->Simulation_CurrentTime()))
					sss = ws->scenario->Simulation_CurrentTime();
				if ((ss == ws->scenario->Simulation_EndTime()) && (ws->scenario->Simulation_EndTime() > ws->scenario->Simulation_CurrentTime()))
					ss = ws->scenario->Simulation_CurrentTime();

				if (out.outputs[0].filename.size() > 0)
				{
					std::uint16_t Method;
					switch (out.interpolation)
					{
					case 2:		Method = 1; break;
					case 1:		Method = 0; break;
					case 0:		Method = 2;	break;
					case 3:		Method = 4; break;
					case 4:		Method = 3;	break;
					case 5:		Method = 5; break;
					default:	Method = 0; break;
					}

					//were files written to disk or were cumulative outputs just updated
					bool fileWritten = false;
					//when files were supposed to be written were they skipped because they were empty or do they actually exist
					std::vector<bool> fileExists(out.outputs.size(), false);
					if (ws->scenario->m_parent && out.subScenarioNames.size() == 0)
					{
						weak_assert(out.outputs[0].m_darray->count() < ws->scenario->m_parent->m_childArray.size());
						out.outputs[0].lock();
						Project_XYStatOptions options;
						options.mintime = &sss;
						options.time = &ss;
						options.method = Method;
						options.discretization = out.discretize;
						bool done = false;

						Project::Asset* a = nullptr;
						ULONG a_index = 0;
						if ((out.outputs.size() == 1) && ((out.outputs[0].statistic == PARA_CRITICAL_PATH) || (out.outputs[0].statistic == PARA_CRITICAL_PATH_MEAN))) {
							if (!out.outputs[0].assetName.length()) {
								a = (Project::Asset*)~NULL;
								a_index = (ULONG)-1;
								out.outputs[0].statistic = PARA_CRITICAL_PATH_MEAN;
							}
							else {
								a = ws->sp->m_project->m_assetCollection.FindName(out.outputs[0].assetName);
								a_index = (ULONG)out.outputs[0].assetIndex;
							}
						}
						done = ws->sp->m_project->ExportParameterGrid(out.outputs, ws->scenario, a, a_index, options,
							out.outputNodata, out.minimizeOutput, true, nullptr, nullptr, nullptr, 1,
							out.compression, &ws->sp->m_unitConversion);
						if (!done)
							out.outputs[0].m_darray->write(nullptr);		// this is needed when the export routine fails - still need to record that we tried
						weak_assert(out.outputs[0].m_darray->count() <= ws->scenario->m_parent->m_childArray.size());

						if (ws->sp->m_tempOutputInterval > 0)
						{
							//race condition on the output count
							std::uint32_t outputCount = out.outputs[0].m_darray->count();
							if (outputCount != out.outputs[0].lastOutputCount && !(outputCount % ws->sp->m_tempOutputInterval))
							{
								out.outputs[0].lastOutputCount = outputCount;

								std::string path;
								for (auto &output : out.outputs) {
									std::string csPath = output.filename;
									path = strprintf("%u_%s", outputCount, csPath.c_str());
									createDirectory(path);
									std::int16_t ParaOption;
									if (output.statistic == PARA_BURNT)
										ParaOption = PARA_BURNT_MEAN;
									else if (output.statistic == PARA_CRITICAL_PATH)
										ParaOption = PARA_CRITICAL_PATH_MEAN;
									else
										ParaOption = output.statistic;
									//output won't necessarily be on the same count as the requested output, probably fine for temp files - agreed
									ws->sp->m_project->ExportParameterGrid(path.c_str(), ParaOption, out.outputNodata, out.minimizeOutput,
										true, output.m_darray, out.compression, &ws->sp->m_unitConversion);
									if (ws->sp->m_keepTempFiles > 0)
									{
										auto last = outputCount - (ws->sp->m_tempOutputInterval * ws->sp->m_keepTempFiles);
										if (last > 0)
										{
											std::string oldPath = strprintf("%u_%s", last, csPath.c_str());
											try
											{
												//deleting the entire directory causes issues since when running multiple scenarios with
												//sub-scenarios at the same time so just delete old output files
												auto fs_path = fs::path(oldPath);
												if (fs::exists(fs_path))
													fs::remove(fs_path);
											}
											catch (...) {
											}
										}
									}
								}
							}
						}

						//lock after count check only works if you assume the output array count will never be greater than the child array size
						if (ws->scenario->m_parent->m_childArray.size() == out.outputs[0].m_darray->count())
						{
							bool expected = false;
							if (out.file_exported.compare_exchange_strong(expected, true))
							{
								Project_XYStatOptions options;
								options.mintime = &sss;
								options.time = &ss;
								options.method = Method;
								options.discretization = out.discretize;
								ws->sp->m_project->ExportParameterGrid(out.outputs, ws->scenario, a, a_index, options,
									out.outputNodata, out.minimizeOutput, true, nullptr, nullptr, nullptr, 2,
									out.compression, &ws->sp->m_unitConversion);
								fileWritten = true;
								auto fileIndex = 0;
								for (auto &output : out.outputs) {
									std::string name, csPath(output.filename);
									name = ws->scenario->m_parent->m_name;
									sm->stepInfo.status_msg = strprintf("Exported '%s' for Scenario '%s'.", csPath.c_str(), name.c_str());
									fileExists[fileIndex++] = fs::exists(output.filename);
									output.cleanup();
								}
							}
						}
						out.outputs[0].unlock();
					}
					else
					{
						Project_XYStatOptions options;
						options.mintime = &sss;
						options.time = &ss;
						options.method = Method;
						options.discretization = out.discretize;

						Project::Asset* a = nullptr;
						ULONG a_index = 0;
						if ((out.outputs.size() == 1) && ((out.outputs[0].statistic == PARA_CRITICAL_PATH) || (out.outputs[0].statistic == PARA_CRITICAL_PATH_MEAN))) {
							if (!out.outputs[0].assetName.length()) {
								a = (Project::Asset*)~NULL;
								a_index = (ULONG)-1;
								out.outputs[0].statistic = PARA_CRITICAL_PATH_MEAN;
							}
							else {
								a = ws->sp->m_project->m_assetCollection.FindName(out.outputs[0].assetName);
								a_index = (ULONG)out.outputs[0].assetIndex;
							}
						}
						ws->sp->m_project->ExportParameterGrid(out.outputs, ws->scenario, a, a_index, options,
							out.outputNodata, out.minimizeOutput, (out.outputs[0].statistic == PARA_CRITICAL_PATH_MEAN) ? true : false, nullptr, nullptr, nullptr, 3,
							out.compression, &ws->sp->m_unitConversion);
						fileWritten = true;
						auto fileIndex = 0;
						for (auto &output : out.outputs)
						{
							std::string name, csPath(output.filename);
							if (ws->scenario->m_parent)
								name = ws->scenario->m_parent->m_name;
							else
								name = ws->scenario->m_name;
							sm->stepInfo.status_msg = strprintf("Exported '%s' for Scenario '%s'.", csPath.c_str(), name.c_str());
							fileExists[fileIndex++] = fs::exists(output.filename);
							output.cleanup();
						}
					}
					//only return the output filename if streaming was requested for the file
					if (out.shouldStream && fileWritten)
					{
						for (size_t i = 0; i < out.outputs.size(); i++)
						{
							sm->stepInfo.outputFiles.push_back({ out.outputs[i].filename, 3, fileExists[i], out.outputs[i].coverageName });
						}
					}

					break;
				}
			}
		}
	}
}


void exportVectors(const int w_export_i, const int w_export_cnt, worker_struct* ws, int& v_export_i, const int v_export_cnt, SimMessage* sm) {
	if (w_export_i == w_export_cnt) {
		if (v_export_i == -1)
			v_export_i++;
		ws->scenario->GetWeatherGrid()->SetCache(ws->scenario->layerThread(), 0, false);

		while (v_export_i < v_export_cnt)
		{
			auto& out = ws->sp->m_project->m_outputs.vectors[v_export_i];
			v_export_i++;
			std::string scenName = out.scenarioName;
			std::string subName = ws->scenario->m_subName;

			if (boost::iequals(scenName, ws->scenario->m_name) &&
				//this is either a cumulative output or an output for this specific sub-scenario
				((out.subScenarioNames.size() == 0) || (std::find(out.subScenarioNames.begin(), out.subScenarioNames.end(), subName) != out.subScenarioNames.end())))
			{
				WTime start((ULONGLONG)0, ws->sp->m_project->m_timeManager);
				WTime end((ULONGLONG)0, ws->sp->m_project->m_timeManager);
				if (out.multiplePerimeters)
				{
					start = out.getStartTime(subName);
					end = out.getEndTime(subName);
					start.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
					end.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
					if ((end == ws->scenario->Simulation_EndTime()) && (ws->scenario->Simulation_EndTime() > ws->scenario->Simulation_CurrentTime()))
						end = ws->scenario->Simulation_CurrentTime();
				}
				else
				{
					start = out.getStartTime(subName);
					start.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
					if ((start == ws->scenario->Simulation_EndTime()) && (ws->scenario->Simulation_EndTime() > ws->scenario->Simulation_CurrentTime()))
						start = ws->scenario->Simulation_CurrentTime();
					end = start;
				}

				if (out.filename.size() > 0)
				{
					std::string csPath(out.filename);

					createDirectory(out.filename);

#ifdef _MSC_VER
					boost::replace_all(csPath, "/", "\\");
#endif
					std::string ext = fs::path(csPath).extension().string();
					if (ext.length() == 0)
						csPath += ".gpkg"s;
					auto [driver_name, projection_name] = ws->sp->guessDriverNameFromFilename(csPath.c_str());

					USHORT flags = 0;
					if (out.activePerimeters)			flags |= SCENARIO_EXPORT_SUBSET_ACTIVE;
					if (out.removeIslands)				flags |= SCENARIO_EXPORT_SUBSET_EXTERIOR;
					if (out.mergeContacting)			flags |= SCENARIO_EXPORT_COMBINE_SET;

					Project::VectorExportOptions options;
					options.ActivePerimeter = out.perimeterActive.has_value() && out.perimeterActive.value();
					options.Comment1 = ""s;
					options.DateTime = out.simulationDate.has_value() && out.simulationDate.value();
					options.FGMName = out.jobName.has_value() && out.jobName.value();
					options.FireSize = out.fireSize.has_value() && out.fireSize.value();
					options.IgnitionName = out.ignitionName.has_value() && out.ignitionName.value();
					options.IgnitionAttributes = out.ignitionAttributes.has_value() && out.ignitionAttributes.value();
					options.OutputAll = false;
					options.WxValues = out.wxValues.has_value() && out.wxValues.value();
					options.FWIValues = out.fwiValues.has_value() && out.fwiValues.value();
					options.MaxBurnDistance = out.maxBurnDistance.has_value() && out.maxBurnDistance.value();
					options.IgnitionLocation = out.ignitionLocation.has_value() && out.ignitionLocation.value();
					options.TotalPerimeter = out.perimeterTotal.has_value() && out.perimeterTotal.value();
					options.PrometheusVersion = out.applicationVersion.has_value() && out.applicationVersion.value();
					options.ScenarioName = out.outputScenarioName.has_value() && out.outputScenarioName.value();
					options.AssetFirstArrivalTime = out.assetArrivalTime.has_value() && out.assetArrivalTime.value();
					options.AssetArrivalCount = out.assetCount.has_value() && out.assetCount.value();
					options.IdentifyFinal = out.identifyFinalPerimeter.has_value() && out.identifyFinalPerimeter.value();
					options.SimulationStatus = out.simulationStatus.has_value() && out.simulationStatus.value();

					std::uint64_t p_unit, a_unit;
					if (out.perimeterUnits.has_value())
					{
						switch (out.perimeterUnits.value())
						{
						case 0:	p_unit = STORAGE_FORMAT_KM; break;
						case 1:	p_unit = STORAGE_FORMAT_M; break;
						case 2:	p_unit = STORAGE_FORMAT_MILE; break;
						case 3:	p_unit = STORAGE_FORMAT_FOOT; break;
						case 4:	p_unit = STORAGE_FORMAT_YARD; break;
						case 5:	p_unit = STORAGE_FORMAT_CHAIN; break;
						}
					}
					else
					{
						p_unit = ws->sp->m_unitConversion.AltDistanceDisplay();
					}

					if (out.areaUnits.has_value())
					{
						switch (out.areaUnits.value())
						{
						case 0:	a_unit = STORAGE_FORMAT_KM2; break;
						case 1:	a_unit = STORAGE_FORMAT_M2; break;
						case 2:	a_unit = STORAGE_FORMAT_HECTARE; break;
						case 3:	a_unit = STORAGE_FORMAT_MILE2; break;
						case 4:	a_unit = STORAGE_FORMAT_FT2; break;
						case 5:	a_unit = STORAGE_FORMAT_YD2; break;
						case 6:	a_unit = STORAGE_FORMAT_ACRE; break;
						}
					}
					else
					{
						a_unit = ws->sp->m_unitConversion.AreaDisplay();
					}

					std::string version("Prometheus ");
					version += VER_PROMETHEUS_VERSION_FULL_STRING;

					bool really_exported = true;
					{
						std::uint16_t child_flags;
						out.lock();
						auto e = InterlockedIncrement(&out.exported);
						if (ws->scenario->m_parent)
						{
							if (e == ws->scenario->m_parent->m_childArray.size())
								child_flags = 2;
							else
							{
								child_flags = 0;
								really_exported = false;
							}
						}
						else
							child_flags = 1;
						sm->stepInfo.retval = ws->scenario->ExportFireFronts(nullptr, start, end, &options, flags,
							driver_name, projection_name, csPath, nullptr, version.c_str(), p_unit, a_unit, &ws->sp->m_unitConversion, child_flags);
						if (child_flags != 0)
							sm->stepInfo.status_msg = strprintf("Exported '%s' for Scenario '%s'.", csPath.c_str(), ws->scenario->m_name.c_str());
						else
							sm->stepInfo.status_msg = strprintf("Child scenario %s.%u exported to %s.", ws->scenario->m_name.c_str(), ws->s_index, csPath.c_str());

						out.unlock();
					}

					if (really_exported)
					{
						//only return the output filename if streaming was requested for the file
						if (out.shouldStream)
						{
							sm->stepInfo.outputFiles.push_back({ out.filename, 2, true, out.coverageName });
						}
						// perform vector export
						if (SUCCEEDED(sm->stepInfo.retval))
						{
							WTime GMTstartTime(start, WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST, 1);
							WTimeSpan offset(GMTstartTime - start);
							HRESULT hr2 = ws->sp->KMLHelper(csPath.c_str(), offset);
							if (FAILED(hr2))
								sm->stepInfo.retval = hr2;
						}
						else if (sm->stepInfo.retval == E_FAIL)
						{
							if (ws->scenario->GetNumberFires(end) == 0)
							{
								sm->stepInfo.retval = ERROR_ATTEMPT_EXPORT_EMPTY_FIRE;
							}
						}
					}
					break;
				}
			}
		}
	}
}


void exportFuelGrids(const int c_export_i, const int c_export_cnt, int& f_export_i, const int f_export_cnt, worker_struct* ws, SimMessage* sm) {
	if (c_export_i == c_export_cnt) {
		if (f_export_i == -1)
			f_export_i++;
		while (f_export_i < f_export_cnt) {
			bool expected = false;
			auto& out = ws->sp->m_project->m_outputs.fuelgrids[f_export_i];
			f_export_i++;
			if (out.scenarioName.empty()) {
				if (out.file_exported.compare_exchange_strong(expected, true)) {
					ws->sp->m_project->ExportGrid(out.filename, out.compression);

					//only return the output filename if streaming was requested for the file
					if (out.shouldStream)
						sm->stepInfo.outputFiles.push_back({ out.filename, 3, true, out.coverageName });
					break;
				}
			}
			else {
				if (out.scenarioName.compare(ws->scenario->m_name) == 0) {
					if (out.subScenarioName.empty()) {
						if (out.file_exported.compare_exchange_strong(expected, true)) {
							ws->scenario->ExportFilteredFuelGrid(out.filename, ws->sp->m_project->m_fuelCollection.m_fuelMap.get(), (std::int32_t)out.compression);

							//only return the output filename if streaming was requested for the file
							if (out.shouldStream)
								sm->stepInfo.outputFiles.push_back({ out.filename, 3, true, out.coverageName });
							break;
						}
					}
					else {
						for (int i = 0; i < ws->scenario->m_childArray.size(); i++) {
							Project::Scenario* s = ws->scenario->m_childArray[i];
							if (out.subScenarioName.compare(s->m_name) == 0) {
								if (out.file_exported.compare_exchange_strong(expected, true)) {
									s->ExportFilteredFuelGrid(out.filename, ws->sp->m_project->m_fuelCollection.m_fuelMap.get(), (std::int32_t)out.compression);

									//only return the output filename if streaming was requested for the file
									if (out.shouldStream)
										sm->stepInfo.outputFiles.push_back({ out.filename, 3, true, out.coverageName });
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}


void exportWxGrids(const int ac_export_i, const int ac_export_cnt, int& w_export_i, const int w_export_cnt, worker_struct* ws, SimMessage* sm) {
	if (ac_export_i == ac_export_cnt) {
		if (w_export_i == -1)
			w_export_i++;
		while (w_export_i < w_export_cnt) {
			auto& out = ws->sp->m_project->m_outputs.grids[w_export_i++];
			std::string scenName(out.scenarioName);
			std::string subName = ws->scenario->m_subName;
			// output any weather grid exports before we turn off the cache
			if (boost::iequals(scenName, ws->scenario->m_name) &&
				//this is either a cumulative output or an output for this specific sub-scenario
				(out.subScenarioNames.size() == 0 || std::find(out.subScenarioNames.begin(), out.subScenarioNames.end(), subName) != out.subScenarioNames.end()) &&
				(out.outputs[0].statistic < 10))
			{
				WTime ss = out.getExportTime(subName);
				ss.PurgeToSecond(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				if ((ss == ws->scenario->Simulation_EndTime()) && (ws->scenario->Simulation_EndTime() > ws->scenario->Simulation_CurrentTime()))
					ss = ws->scenario->Simulation_CurrentTime();

				if (out.outputs[0].filename.size() > 0)
				{
					std::uint16_t Method;
					switch (out.interpolation)
					{
					case 2:		Method = 1; break;
					case 1:		Method = 0; break;
					case 0:		Method = 2; break;
					case 3:		Method = 4; break;
					case 4:		Method = 3; break;
					case 5:		Method = 5; break;
					default:	Method = 0; break;
					}

					//were files written to disk or were cumulative outputs just updated
					bool fileWritten = false;
					//when files were supposed to be written were they skipped because they were empty or do they actually exist
					std::vector<bool> fileExists(out.outputs.size(), false);
					if (ws->scenario->m_parent && out.subScenarioNames.size() == 0)
					{
						weak_assert(out.outputs[0].m_darray->count() < ws->scenario->m_parent->m_childArray.size());
						out.outputs[0].lock();
						Project_XYStatOptions options;
						options.mintime = &ss;
						options.time = &ss;
						options.method = Method;
						options.discretization = out.discretize;
						bool done = ws->sp->m_project->ExportParameterGrid(out.outputs, ws->scenario, nullptr, 0, options,
							out.outputNodata, out.minimizeOutput, true, nullptr, nullptr, nullptr, 1,
							out.compression, &ws->sp->m_unitConversion);
						if (!done)
							out.outputs[0].m_darray->write(nullptr);
						weak_assert(out.outputs[0].m_darray->count() <= ws->scenario->m_parent->m_childArray.size());
						if (ws->scenario->m_parent->m_childArray.size() == out.outputs[0].m_darray->count()) {
							bool expected = false;
							if (out.file_exported.compare_exchange_strong(expected, true))
							{
								for (auto& output : out.outputs) {
									std::string csPath(output.filename);
									createDirectory(csPath);
								}
								ws->sp->m_project->ExportParameterGrid(out.outputs, ws->scenario, nullptr, 0, options,
									out.outputNodata, out.minimizeOutput, true, nullptr, nullptr, nullptr, 2,
									out.compression, &ws->sp->m_unitConversion);
								fileWritten = true;
								auto fileIndex = 0;
								for (auto &output : out.outputs)
								{
									std::string name, csPath(output.filename);
									name = ws->scenario->m_parent->m_name;
									sm->stepInfo.status_msg = strprintf("Exported '%s' for Scenario '%s'.", csPath.c_str(), name.c_str());
									fileExists[fileIndex++] = fs::exists(output.filename);
									output.cleanup();
								}
							}
						}
						out.outputs[0].unlock();
					}
					else
					{
						Project_XYStatOptions options;
						options.mintime = &ss;
						options.time = &ss;
						options.method = Method;
						options.discretization = out.discretize;
						ws->sp->m_project->ExportParameterGrid(out.outputs, ws->scenario, nullptr, 0, options,
							out.outputNodata, out.minimizeOutput, false, nullptr, nullptr, nullptr, 3,
							out.compression, &ws->sp->m_unitConversion);
						fileWritten = true;
						auto fileIndex = 0;
						for (auto &output : out.outputs)
						{
							std::string name, csPath(out.outputs[0].filename);
							if (ws->scenario->m_parent)
								name = ws->scenario->m_parent->m_name;
							else
								name = ws->scenario->m_name;
							sm->stepInfo.status_msg = strprintf("Exported '%s' for Scenario '%s'.", csPath.c_str(), name.c_str());
							fileExists[fileIndex++] = fs::exists(output.filename);
							output.cleanup();
						}
					}
					//only return the output filename if streaming was requested for the file
					if (out.shouldStream && fileWritten)
					{
						for (size_t i = 0; i < out.outputs.size(); i++)
						{
							sm->stepInfo.outputFiles.push_back({ out.outputs[i].filename, 3, fileExists[i], out.outputs[i].coverageName });
						}
					}
					break;
				}
			}
		}
	}
}


void exportReport(const Project::SummaryOutput& out, Project::SummaryCollection* collection, worker_struct *ws,
	SimMessage *sm, Project::Scenario *s)
{
	createDirectory(out.filename);
	std::string fileName(out.filename);

	Project::PrintReportOptions PrntRptOptns;
	bool addWeather = false;
	PrntRptOptns.setPath(fileName);

	if (out.applicationName.has_value())		PrntRptOptns.setApplicationSettings(out.applicationName.value());
	if (out.fbpPatches.has_value())				PrntRptOptns.setFBPFuelPatches(out.fbpPatches.value());
	if (out.ignitions.has_value())				PrntRptOptns.setIgnitions(out.ignitions.value());
	if (out.geoData.has_value())				PrntRptOptns.setGeoData(out.geoData.value());
	if (out.assetData.has_value())				PrntRptOptns.setAssetData(out.assetData.value());
	if (out.outputScenarioName.has_value())		PrntRptOptns.setScenarioSettings(out.outputScenarioName.value());
	if (out.scenarioComments.has_value())		PrntRptOptns.setScenarioComments(out.scenarioComments.value());
	if (out.inputs.has_value())					PrntRptOptns.setInputFiles(out.inputs.value());
	if (out.landscape.has_value())				PrntRptOptns.setLandscapeProperties(out.landscape.value());
	if (out.fbpPatches.has_value())				PrntRptOptns.setFBPFuelPatches(out.fbpPatches.value());
	if (out.fbp.has_value())					PrntRptOptns.setFBPFuelTypes(out.fbp.value());
	if (out.wxStreams.has_value())				PrntRptOptns.setWxStreams(out.wxStreams.value());
	if (out.wxPatches.has_value())				PrntRptOptns.setWxPatchesAndWxGrids(out.wxPatches.value());
	if (out.wxData.has_value())                 addWeather = out.wxData.value();

	std::string version(VER_PROMETHEUS_VERSION_FULL_STRING);
	std::string releaseDate(VER_RELEASE_DATE_SHORT_STRING);

	if (collection)
		ws->sp->m_project->PrintReport(collection, PrntRptOptns, version.c_str(), releaseDate.c_str(),
			ws->sp->m_id.c_str(), s, &ws->sp->m_unitConversion, addWeather);
	else
	{
		ws->sp->m_project->PrintReport(fileName.c_str(), PrntRptOptns, version.c_str(), releaseDate.c_str(),
			ws->sp->m_id.c_str(), s, &ws->sp->m_unitConversion, addWeather);

		//only return the output filename if streaming was requested for the file
		if (out.shouldStream)
		{
			sm->stepInfo.outputFiles.push_back({ out.filename, 1, true });
		}
	}
	sm->stepInfo.status_msg = strprintf("Exported '%s' for Scenario '%s'.", fileName.c_str(), ws->scenario->m_name.c_str());
}


void exportReport(int& s_export_i, int s_export_cnt, worker_struct* ws, SimMessage* sm)
{
	bool expected = false;
	while (s_export_i < s_export_cnt)
	{
		auto& out = ws->sp->m_project->m_outputs.summaries[s_export_i];
		s_export_i++;
		std::string scenName(out.scenarioName);
		if (boost::iequals(scenName, ws->scenario->m_name))
		{
			bool isSubscenario = ((ws->scenario->m_parent) && (boost::iends_with(out.filename, ".json") || boost::iends_with(out.filename, ".bin")));
			//TODO combine summary files
			if (isSubscenario)
			{
				out.lock();
				//export the current sub-scenario
				exportReport(out, out.m_collection, ws, sm, ws->scenario);
				//everything has been exported, save the file
				if ((out.cumulativeSize() == ws->scenario->m_parent->m_childArray.size()) &&
					out.exported.compare_exchange_strong(expected, true))
				{
					//write the summary file to disk
					std::string fileName(out.filename);
					ws->sp->m_project->ExportReportJson(fileName.c_str(), out.m_collection);

					//only return the output filename if streaming was requested for the file
					if (out.shouldStream)
					{
						sm->stepInfo.outputFiles.push_back({ out.filename, 1, true });
					}

					//cleanup the protobuf objects
					out.cleanup();
				}
				out.unlock();
			}
			else if (out.exported.compare_exchange_strong(expected, true))
			{
				if (out.filename.size() > 0)
				{
					if (ws->scenario->m_parent)
						exportReport(out, nullptr, ws, sm, ws->scenario->m_parent);
					else
						exportReport(out, nullptr, ws, sm, ws->scenario);
				}
				break;
			}
		}
	}
}


void exportStats(const int s_export_i, const int s_export_cnt, int& c_export_i, const int c_export_cnt, worker_struct* ws, SimMessage* sm) {
	bool expected = false;
	if (s_export_i == s_export_cnt) {
		if (c_export_i == -1)
			c_export_i++;
		while (c_export_i < c_export_cnt)
		{
			auto& out = ws->sp->m_project->m_outputs.stats[c_export_i];
			c_export_i++;
			std::string scenName(out.scenarioName);
			// output any weather grid exports before we turn off the cache
			if (boost::iequals(scenName, ws->scenario->m_name))
			{
				if (out.file_exported.compare_exchange_strong(expected, true))
				{
					exportStats(ws->sp->m_unitConversion, ws->sp->m_project, ws->m_fwi, out, ws->scenario, sm);	// this call seems to call create_directories() for all paths of control

					//only return the output filename if streaming was requested for the file
					if (out.shouldStream)
						sm->stepInfo.outputFiles.push_back({ out.filename, 4, true });
				}
			}
		}
	}
}


void exportAssetStats(const int f_export_i, const int f_export_cnt, int& ac_export_i, const int ac_export_cnt, worker_struct* ws, SimMessage* sm) {
	bool expected = false;
	if (f_export_i == f_export_cnt) {
		if (ac_export_i == -1)
			ac_export_i++;
		while (ac_export_i < ac_export_cnt) {
			auto& out = ws->sp->m_project->m_outputs.assetOutputs[ac_export_i];
			ac_export_i++;
			std::string scenName(out.scenarioName);
			// output any weather grid exports before we turn off the cache
			if (boost::iequals(scenName, ws->scenario->m_name))
			{
				if (ws->scenario->m_parent)
				{
					out.lock();

					out.exportCount++;
					bool write = (out.exportCount == ws->scenario->m_parent->m_childArray.size()) &&
						out.file_exported.compare_exchange_strong(expected, true);
					exportAssetStats(ws->sp->m_unitConversion, ws->sp->m_project, ws->sp, ws->m_fwi, out, ws->scenario, sm, write);	// this calls create_directories() eventually
					if (write)
					{
						//only return the output filename if streaming was requested for the file
						if (out.shouldStream)
							sm->stepInfo.outputFiles.push_back({ out.filename, 5, true });
						out.cleanup();
					}

					out.unlock();
				}
				else
				{
					if (out.file_exported.compare_exchange_strong(expected, true))
					{
						exportAssetStats(ws->sp->m_unitConversion, ws->sp->m_project, ws->sp, ws->m_fwi, out, ws->scenario, sm, true);

						//only return the output filename if streaming was requested for the file
						if (out.shouldStream)
							sm->stepInfo.outputFiles.push_back({ out.filename, 5, true });
					}
				}
			}
		}
	}
}


std::int16_t SPARCS_P::SimulationCalculateContinue(Project::Scenario **s, std::uint32_t *s_cnt)
{
	m_lock->Lock();
	SimMessage *m = m_list.RemHead();
	m_lock->Unlock();

	if (m)
	{
		Project::Scenario *scenario = m->stepInfo.scenario;
		*s = scenario;
		*s_cnt = m->stepInfo.s_index;
		worker_struct *ws = (worker_struct *)scenario->m_worker;
		ScenarioStepInfo &ssi = m->stepInfo;

		WTime tm(ssi.timestamp, m_project->m_timeManager);
		std::string clockStr = tm.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_DATE | WTIME_FORMAT_TIME | WTIME_FORMAT_YEAR | WTIME_FORMAT_EXCLUDE_SECONDS | WTIME_FORMAT_ABBREV);

		if (ssi.statistics.size() > 0)
			EmitStatistics(ssi.statistics);
		if (ssi.outputFiles.size() > 0)
		{
			if (m_proto)
			{
				for (auto &o : ssi.outputFiles)
					m_client.streamOutputFile(o.outputFile, o.coverageName, o.outputFileType, o.exists, m_managerId, m_proto);
			}
		}

		bool can_finish = ssi.finished;
		if (can_finish)
		{
			if (FAILED(ssi.retval))
			{
				last_error_msg = ErrorMsg(ssi.retval, nullptr);
				last_error = ssi.retval;
				std::string str = scenario->m_name;
				str += ": Error: ";
				str += last_error_msg;
				UpdateStatus(0, WISEStatus::SCENARIO_FAILED, str, -1, OUTPUT_STATUS_ALL);
			}
			else
			{
				UpdateStatus(1, WISEStatus::COMPLETE, scenario->m_name, -1, OUTPUT_STATUS_ALL);
			}
			if ((!scenario->m_parent) && (scenario->m_childArray.size()))
			{
				scenario->Simulation_Clear();
			}
			if (ws)
			{
				if (ws->worker)
				{
                    ws->worker->SynchronizeShutdown();		// wait for it to complete
                    delete ws->worker;
				}
                delete ws;
				scenario->m_worker = nullptr;
			}
			delete m;
			return 1;
		}
		else
		{
			if ((ssi.retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE) ||
				(ssi.retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_EXTENTS) ||
				(ssi.retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_ASSET) ||
				(ssi.retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI90) ||
				(ssi.retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI95) ||
				(ssi.retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_FI100) ||
				(ssi.retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_RH) ||
				(ssi.retval == SUCCESS_SCENARIO_SIMULATION_COMPLETE_STOPCONDITION_PRECIP))
			{
				UpdateStatus(1, WISEStatus::SCENARIO_COMPLETED, scenario->m_name, -1, OUTPUT_STATUS_ALL);

				std::string msg;
				std::uint16_t level;
				short qos;
				if (scenario->m_parent)
				{
					level = 1;
					qos = 1;
					msg = (boost::format("Simulation %s.%d.%d.%d completed in time:, %s, %I64u") % scenario->m_name % ws->b_index % ws->s_index % scenario->m_parent->m_childArray.size() % ssi.duration.ToString(WTIME_FORMAT_TIME | WTIME_FORMAT_CONDITIONAL_TIME) % ssi.ticks).str();
				}
				else
				{
					level = 2;
					qos = -1;
					msg = (boost::format("Simulation %s completed in time:, %s, %I64u") % scenario->m_name % ssi.duration.ToString(WTIME_FORMAT_TIME | WTIME_FORMAT_CONDITIONAL_TIME) % ssi.ticks).str();
				}
				UpdateStatus(level, WISEStatus::INFORMATION, msg, qos, OUTPUT_STATUS_ALL);
				// simulation ran successfully to completion and all data is about to be exported
				delete m;
				return 2;
			}
			else if (ssi.retval == S_OK)
			{
				if (ssi.timestamp > 0)
				{
					std::string msg;
					if (scenario->m_parent)
						msg = (boost::format("%s.%d has simulated to time %s.") % scenario->m_name % ws->s_index % clockStr).str();
					else
						msg = (boost::format("%s has simulated to time %s.") % scenario->m_name % clockStr).str();
					UpdateStatus(3, WISEStatus::INFORMATION, msg, -1, OUTPUT_STATUS_ALL);
				}
				else
				{
					UpdateStatus(2, WISEStatus::INFORMATION, ssi.status_msg, -1, OUTPUT_STATUS_ALL);
				}
			}
		}
		delete m;
		return 0;
	}
	return -1;
}


HRESULT SPARCS_P::InitializeSubScenarios(Project::Scenario *scenario)
{
	HRESULT hr = S_OK;

	if ((scenario) && (!scenario->m_worker))
	{
		HRESULT state = scenario->Simulation_State();
		if ((state != SUCCESS_SCENARIO_SIMULATION_RESET) && (state != SUCCESS_SCENARIO_SIMULATION_RUNNING))
		{
			// has to be stopped before it's reset
			scenario->Simulation_Clear();
			hr = scenario->Simulation_Reset(false);

			if (FAILED(hr))
			{
				std::string error_msg = "Error : Simulation could not be initialized."s;
				switch (hr)
				{
				case ERROR_SCENARIO_NO_FIRES:					error_msg += "\nNo ignitions are attached to the scenario."; break;
				case ERROR_SCENARIO_BAD_TIMES:					error_msg += "\nVerify the scenario start time, end time."; break;
				case ERROR_SCENARIO_BAD_TIMESTEPS:				error_msg += "\nVerify the calculation and output intervals."; break;
				case ERROR_WEATHER_STREAM_NOT_ASSIGNED:			error_msg += "\nNo weather streams are attached to the scenario."; break;
				case ERROR_GRID_UNINITIALIZED:					error_msg += "\nVerify grid data, latitude, longitude, and time zone."; break;
				case ERROR_GRID_WEATHER_INVALID_DATES:			error_msg += "\nOne or more weather stations contains weather data that is not complete or present for the entire simulation time."; break;
				case ERROR_GRID_WEATHER_STATION_ALREADY_PRESENT:error_msg += "\nOne or more weather stations has multiple streams attached."; break;
				case ERROR_GRID_PRIMARY_STREAM_UNSPECIFIED:		error_msg += "\nNo primary weather stream was specified."; break;
				case ERROR_GRID_WEATHERSTATIONS_TOO_CLOSE:		error_msg += "\nOne or more weather stations are less than 100m apart."; break;
				case ERROR_GRID_WEATHERSTREAM_TIME_OVERLAPS:	error_msg += "\nOne or more weather streams specify overlapping time intervals in the same weather station."; break;
				case ERROR_GRID_TIME_OUT_OF_RANGE:				error_msg += "\nOne or more weather grids have invalid times/dates."; break;
				case ERROR_SCENARIO_MULTIPLE_WD_GRIDS:			error_msg += "\nOne or more wind direction grids apply at the same time."; break;
				case ERROR_SCENARIO_MULTIPLE_WS_GRIDS:			error_msg += "\nOne or more wind speed grids apply at the same time."; break;
				}

				// need to deal with this error message
				if (hr != ERROR_SCENARIO_BAD_STATE)
                {
                    const char* cmsg = error_msg.c_str();
					RETURN_MSG(hr, cmsg);
                }
			}
		}
	}
	return hr;
}


HRESULT SPARCS_P::CleanupSubScenarios(Project::Scenario *parent)
{
	HRESULT hr = S_OK;

	if (parent->m_childArray.size())
		parent->Simulation_Clear();

	return hr;
}


HRESULT SPARCS_P::RunSimulation(Project::Scenario *scenario, SummaryType justSummary, int balanceIndex)
{
	HRESULT hr = S_OK;

	if ((scenario) && (!scenario->m_worker))
	{
		HRESULT state = scenario->Simulation_State();
		if ((state != SUCCESS_SCENARIO_SIMULATION_RESET) && (state != SUCCESS_SCENARIO_SIMULATION_RUNNING))
		{
			bool multi = m_remainingScenarioCnt > (2 * CWorkerThreadPool::NumberIdealProcessors());
			PolymorphicAttribute vmulti(multi);
			scenario->m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_SINGLETHREADING, vmulti);
			m_remainingScenarioCnt--;

			scenario->Simulation_Clear();		// has to be stopped before it's reset
			hr = scenario->Simulation_Reset(false);
			if (FAILED(hr))
			{
				std::string error_msg = "Error : Simulation could not be initialized.";
				switch (hr)
				{
					case ERROR_SCENARIO_NO_FIRES:			error_msg += "\nNo ignitions are attached to the scenario."; break;
					case ERROR_SCENARIO_BAD_TIMES:			error_msg += "\nVerify the scenario start time, end time."; break;
					case ERROR_SCENARIO_BAD_TIMESTEPS:		error_msg += "\nVerify the calculation and output intervals."; break;
					case ERROR_WEATHER_STREAM_NOT_ASSIGNED:		error_msg += "\nNo weather streams are attached to the scenario."; break;
					case ERROR_GRID_UNINITIALIZED:			error_msg += "\nVerify grid data, latitude, longitude, and time zone."; break;
					case ERROR_GRID_WEATHER_INVALID_DATES:		error_msg += "\nOne or more weather stations contains weather data that is not complete or present for the entire simulation time."; break;
					case ERROR_GRID_WEATHER_STATION_ALREADY_PRESENT:error_msg += "\nOne or more weather stations has multiple streams attached."; break;
					case ERROR_GRID_PRIMARY_STREAM_UNSPECIFIED:	error_msg += "\nNo primary weather stream was specified."; break;
					case ERROR_GRID_WEATHERSTATIONS_TOO_CLOSE:	error_msg += "\nOne or more weather stations are less than 100m apart."; break;
					case ERROR_GRID_WEATHERSTREAM_TIME_OVERLAPS:	error_msg += "\nOne or more weather streams specify overlapping time intervals in the same weather station."; break;
					case ERROR_GRID_TIME_OUT_OF_RANGE:		error_msg += "\nOne or more weather grids have invalid times/dates."; break;
					case ERROR_SCENARIO_MULTIPLE_WD_GRIDS:		error_msg += "\nOne or more wind direction grids apply at the same time."; break;
					case ERROR_SCENARIO_MULTIPLE_WS_GRIDS:		error_msg += "\nOne or more wind speed grids apply at the same time."; break;
				}
				// need to deal with this error message
				if (hr != ERROR_SCENARIO_BAD_STATE) {
					RETURN_MSG(hr, error_msg.c_str());
				}
			}
		}
		else if (state == SUCCESS_SCENARIO_SIMULATION_RESET) {
			bool multi = m_remainingScenarioCnt > (2 * CWorkerThreadPool::NumberIdealProcessors());
			PolymorphicAttribute vmulti(multi);
			scenario->m_scenario->SetAttribute(CWFGM_SCENARIO_OPTION_SINGLETHREADING, vmulti);
			m_remainingScenarioCnt--;
		}

		if (justSummary == SummaryType::NONE)
		{
			//turn on the weather cache for the child
			if (scenario->m_parent)
				scenario->GetWeatherGrid()->SetCache(scenario->layerThread(), 0, true);

			USHORT xSize, ySize;
			m_project->GetDimensions(&xSize, &ySize);
			std::uint32_t p_s = (std::uint32_t)ySize * (std::uint32_t)xSize;
			for (auto &out : m_project->m_outputs.grids)
			{
				std::string scenName(out.scenarioName);
				std::string subName = scenario->m_subName;
				if (boost::iequals(scenName, scenario->m_name) &&
					//this is either a cumulative output or an output for this specific sub-scenario
					(out.subScenarioNames.size() == 0 || std::find(out.subScenarioNames.begin(), out.subScenarioNames.end(), subName) != out.subScenarioNames.end())) {
					if (out.outputs[0].statistic >= 10)
					{
						for (auto& output : out.outputs)
						{
							if (output.filename.size() > 0)
							{
								bool expected = false;
								if (output.checkPaths.compare_exchange_strong(expected, true))
								{
									createDirectory(output.filename);
								}
								output.lock();
								try {
									// this can't be an atomic_bool because we are initializing for all threads, not just ours
									if (!output.m_darray) {
										output.m_d = new double[p_s];
										memset(output.m_d, 0, sizeof(double) * p_s);
										output.m_darray = new GDALExtras::CumulativeGridOutput<double>(xSize, ySize);
									}
								}
								catch (std::bad_alloc&) {
									fprintf(stderr, "bad alloc 1 in RunSimulation[%d]\n", p_s);
									exit(-1);
								}
								output.unlock();
							}
						}
					}
					else
					{
						for (auto& output : out.outputs)
						{
							if (output.filename.size() > 0)
							{
								bool expected = false;
								if (output.checkPaths.compare_exchange_strong(expected, true))
								{
									createDirectory(output.filename);
								}
								output.lock();
								// this can't be an atomic_bool because we are initializing for all threads, not just ours
								try {
									if (!output.m_d)
									{
										output.m_d = new double[p_s];
										memset(output.m_d, 0, sizeof(double) * p_s);
									}
								}
								catch (std::bad_alloc&) {
									fprintf(stderr, "bad alloc 2 in RunSimulation[%d]\n", p_s);
									exit(-1);
								}
								output.unlock();
							}
						}
					}
				}
			}
			//if this is a sub-scenario
			if (scenario->m_parent)
			{
				//initialize cumulative summary outputs if present
				for (auto& out : m_project->m_outputs.summaries)
				{
					std::string scenName(out.scenarioName);
					//if the output is for this scenario
					if (boost::iequals(scenName, scenario->m_name))
					{
						//a filename was specified
						if (out.filename.size() > 0 &&
							//the output type is either JSON or binary
							(boost::iends_with(out.filename, ".json") || boost::iends_with(out.filename, ".bin")))
						{
							out.lock();
							out.makeCumulativeOutput();
							out.unlock();
						}
					}
				}
			}
		}

		__thread_lock->Lock();
		std::uint32_t group = 0;
		for (std::uint32_t g = 1; g < CWorkerThreadPool::NumberNUMA(); g++)
		{
			if (__group_threads[g] < __group_threads[group] && __group_threads[g] < CWorkerThreadPool::NumberProcessors(g))
				group = g;
		}
		InterlockedIncrement(&__group_threads[group]);
		__thread_lock->Unlock();

		struct worker_struct *ws = new worker_struct;
		scenario->m_worker = ws;
		ws->scenario = scenario;
		if (scenario->m_parent)
		{
			ws->n_index = m_project->m_scenarioCollection.IndexOf(scenario->m_parent);
			ws->s_index = std::distance(scenario->m_parent->m_childArray.begin(), std::find(scenario->m_parent->m_childArray.begin(), scenario->m_parent->m_childArray.end(), scenario));
			ws->b_index = balanceIndex;
		}
		else
		{
			ws->n_index = m_project->m_scenarioCollection.IndexOf(scenario);
			ws->s_index = (std::uint32_t)-1;
			ws->b_index = 0;
		}
		ws->sp = this;
		ws->m_list = &m_list;
		ws->m_lock = m_lock;
		ws->m_event = m_event;
		ws->groupAffinity = group;
		ws->m_justSummary = justSummary;
		ws->m_fwi = m_fwi.get();
		ws->worker = new CWorkerThread(worker_thread, ws);

		ws->worker->CreateThread();
		CWorkerThreadPool::SetThreadAffinityToMask(ws->worker->GetHandle(), group + 100000);

		UpdateStatus(1, WISEStatus::SCENARIO_STARTED, scenario->m_name, -1, OUTPUT_STATUS_ALL);

		__thread_start_count++;

		if (scenario->m_parent)
		{
			std::string msg = (boost::format("Simulation %s.%d.%d.%d started") % scenario->m_name % balanceIndex % ws->s_index % scenario->m_parent->m_childArray.size()).str();
			UpdateStatus(1, WISEStatus::INFORMATION, msg, 1, OUTPUT_STATUS_ALL);
			DebugStatus(2, WISEStatus::SCENARIO_STARTED, (boost::format("Child scenario %d of %d starting.") % ws->s_index % ws->scenario->m_parent->m_childArray.size()).str());
		}
	}

	RETURN_MSG(hr, NULL);
}


void SPARCS_P::StaticInitialization()
{
	__thread_lock = new CThreadCriticalSection();
	__group_threads = new std::uint32_t[CWorkerThreadPool::NumberNUMA()];
	std::fill(__group_threads, __group_threads + CWorkerThreadPool::NumberNUMA(), 0);
}


void SPARCS_P::StaticDeconstruction()
{
	if (__thread_lock)
	{
		delete __thread_lock;
		__thread_lock = nullptr;
	}
	if (__group_threads)
	{
		delete[] __group_threads;
		__group_threads = nullptr;
	}
}


void SPARCS_P::SetupThreads(std::int32_t coreCount, std::int32_t coreOffset, const std::string& coreMask, bool useSharedMemory, std::string* memDump)
{
	interprocess_comms::initial_settings settings;
	get_core_settings(settings.numa_lock, settings.job_count, settings.cpu_skip);
	if (useSharedMemory)
		m_sharedMemory = interprocess_comms::get_shared_memory("WISE7", settings);
	if (m_sharedMemory && memDump)
	{
		std::ostringstream stream;
		m_sharedMemory->dump_to_stream(stream);
		*memDump = stream.str();
	}
	std::uint32_t cores_available = 0;
	bool used_shared_memory = false;

	//an override was set on the command line
	if (coreCount > 0)
		cores_available = coreCount;
	//use the value set in the protobuf file
	else if (m_sharedMemory)
	{
		auto [count, offset] = m_sharedMemory->get_next_available_block();
		cores_available = count;
		coreOffset = offset;
		used_shared_memory = true;
	}
	//nobody set a valid value
	if (cores_available == 0)
		cores_available = max((CWorkerThreadPool::NumberProcessors(0) * 3 + 3) / 4, CWorkerThreadPool::NumberProcessors(0) - 2);

	//force a minimum of two cores, one will run the event loop one will simulate
	if (cores_available < 2)
	{
		cores_available = 2;
		m_loadWarnings += "Requested core count too low (";
		m_loadWarnings += std::to_string(cores_available);
		m_loadWarnings += "), forcing the use of 2 cores.\n";
	}

	//use -1 so that the event loop processor isn't used to run worker threads
	CWorkerThreadPool::SetNumberIdealProcessors(0, cores_available - 1);

	//make this signed because the comparison that uses it becoming huge below 0 is horrible
	std::int64_t cores_allocated = cores_available - 1;
	std::uint64_t group_mask = 0;
	std::uint32_t j, jb = 0, jc = 0;

	std::string maskToUse(coreMask);
	size_t maxLength;
	if (maskToUse.length() == 0 && coreOffset > 0)
	{
		maskToUse.insert(0, coreOffset, '0');
		maxLength = max(CWorkerThreadPool::NumberProcessors(), (std::uint32_t)(cores_available + coreOffset));
	}
	else
		maxLength = max(CWorkerThreadPool::NumberProcessors(), (std::uint32_t)(cores_available + std::count(maskToUse.begin(), maskToUse.end(), '0')));

	//make sure the mask is long enough to cover all requested/real CPUs whether it needs to use them or not
	if (maskToUse.length() < maxLength)
		maskToUse.insert(0, maxLength - maskToUse.length(), '1');

	//flip the CPU mask around
	std::reverse(maskToUse.begin(), maskToUse.end());

	std::uint8_t isFirstProcessor = 1;
	std::uint64_t eventLoopMask = 0;
	std::vector<std::uint32_t> indices;

	for (std::uint32_t i = 0; i < CWorkerThreadPool::NumberNUMA(); i++)
	{
		group_mask = 0;
		for (j = 0; (j < CWorkerThreadPool::NumberProcessors(i)) && (cores_allocated >= 0); j++)
		{
			//this job is allowed to run on this core
			if (maskToUse[jb + j] == '1')
			{
				if (isFirstProcessor == 1)
				{
					isFirstProcessor = 2;
					eventLoopMask = ((std::uint64_t)1) << j;
				}
				else
					group_mask |= ((std::uint64_t)1) << j;
				indices.push_back(jb + j);
				cores_allocated--;
			}
			//somebody else is going to be running on this core
			else
				__group_threads[i]++;
		}

		if (group_mask > 0)
		{
			if (isFirstProcessor == 2)
			{
				isFirstProcessor = 0;
				CWorkerThreadPool::StoreThreadAffinityMask(CWorkerThreadPool::EVENT_LOOP_MASK, i, eventLoopMask);
			}
			CWorkerThreadPool::StoreThreadAffinityMask(100000 + i, i, group_mask);

			size_t foundCores = std::bitset<64>(group_mask).count();
			for (std::uint32_t ji = 0; ji < foundCores; ji++)
			{
				CWorkerThreadPool::StoreThreadAffinityMask(jc + ji, i, group_mask);
			}
			jc += foundCores;
		}
		jb += j;
	}

	if ((!used_shared_memory) && (m_sharedMemory))
		m_sharedMemory->use_specific_block(indices);
}


void SPARCS_P::requestNextScenarioIndex()
{
	if (m_project->loadBalancing() == Project::LoadBalancingType::EXTERNAL_COUNTER)
		m_client.requestScenarioIndex();
}


std::int32_t SPARCS_P::fetchNextScenarioIndex(std::int32_t defaultValue)
{
	//an external application is sending scenario indices over MQTT
	if (m_project->loadBalancing() == Project::LoadBalancingType::EXTERNAL_COUNTER)
		return m_client.nextScenarioIndex(defaultValue);
	//we are reading scenario indices from a local file
	else if (m_project->loadBalancing() == Project::LoadBalancingType::LOCAL_FILE)
	{
		if (!m_balanceIndices.size())
		{
			auto fl = fs::current_path() / "balance.txt";
			if (fs::exists(fl))
			{
				std::ifstream balanceFile;
				balanceFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
				try
				{
					balanceFile.open(fl, std::fstream::in);
					std::string line;
					while (std::getline(balanceFile, line))
					{
						m_balanceIndices.push_back(std::stoi(line));
					}
					balanceFile.close();
				}
				catch (std::ifstream::failure& e)
				{
					std::cerr << "Exception openining load balance file: " << e.what() << std::endl;
				}
			}
			if (!m_balanceIndices.size())
				m_balanceIndices.push_back(defaultValue);
		}

		if (m_balanceIndicesIndex < m_balanceIndices.size())
		{
			auto toret = m_balanceIndices[m_balanceIndicesIndex];
			m_balanceIndicesIndex++;
			return toret;
		}
		return std::numeric_limits<std::int32_t>::max(); // when you're done the list, return an impossible index
	}

	//there is no load balancing
	return defaultValue;
}


/**
 * Get an ISO 8601 string of the current date/time in UTC.
 */
std::string getISO8601String()
{
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::gmtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%FT%TZ");
	return oss.str();
}


HRESULT SPARCS_P::RunSimulations(SummaryType justSummary)
{
	SimMessage *msg = new SimMessage();		// queue up the cache

	Project::Scenario *scenario = m_project->m_scenarioCollection.FirstScenario();
	std::uint32_t cores_in_use = 0;
	//an array to hold all of the scenarios that may need run
	std::vector<ScenarioRun*> to_run;
	std::uint32_t next_index = 0;
	//is some type of load balancing being used and we're not just outputing summary files
	bool microManage = (m_project->loadBalancing() == Project::LoadBalancingType::EXTERNAL_COUNTER ||
		m_project->loadBalancing() == Project::LoadBalancingType::LOCAL_FILE) &&
		(justSummary == SummaryType::NONE);
	std::uint32_t cores_available = CWorkerThreadPool::NumberIdealProcessors();
	//lock the main thread to the event loop processor
	CWorkerThreadPool::SetThreadAffinityToMask(CWorkerThreadPool::GetCurrentThread(), CWorkerThreadPool::EVENT_LOOP_MASK);

	//we know we'll need at least one of these so request it now
	if (microManage)
		requestNextScenarioIndex();
	//if not load balancing all scenarios will be dumped into a single entry
	else
		to_run.push_back(new ScenarioRun());

	UpdateStatus(2, WISEStatus::STARTED, "Running scenarios", -1, OUTPUT_STATUS_ALL);
	DebugStatus(0, WISEStatus::INFORMATION, (boost::format("Starting simulation at %s.") % getISO8601String()).str());

#ifdef __USE_GSL
	auto cleanup = gsl::finally([this, &msg, &to_run, &justSummary]
		{
#else
	auto cleanup = [this, &msg, &to_run, &justSummary]
	{
#endif
		delete msg;

		//delete the scenario run entries
		for (auto sr : to_run)
			delete sr;
		to_run.clear();

		auto s = m_project->m_scenarioCollection.FirstScenario();
		while (s->LN_Succ())
		{
			CleanupSubScenarios(s);
			s = s->LN_Succ();
		}

		time_t now;
		time(&now);
		char timeBuf[sizeof("2000-01-01T00:00:00Z")];
		strftime(timeBuf, sizeof(timeBuf), "%FT%TZ", gmtime(&now));
		std::string completeTime = timeBuf;
		std::uint64_t maxMemory = CWorkerThreadPool::GetPeakMemoryUsage();
		std::vector<std::tuple<std::string, Service::StatisticType>> stats;
		stats.emplace_back(std::make_tuple("WISE_START_TIME", m_startTime));
		stats.emplace_back(std::make_tuple("WISE_END_TIME", completeTime));
		stats.emplace_back(std::make_tuple("WISE_PEAK_MEMORY", maxMemory));

		if (justSummary == SummaryType::VALIDATE)
			UpdateStatus(0, WISEStatus::COMPLETE, "WISE.EXE operations", 1, OUTPUT_STATUS_FILE, stats);
		else
		{
			UpdateStatus(2, WISEStatus::COMPLETE, "Running scenarios", -1, OUTPUT_STATUS_ALL);
			UpdateStatus(0, WISEStatus::COMPLETE, "WISE.EXE operations", 1, OUTPUT_STATUS_ALL, stats);
		}
#ifdef __USE_GSL
		} );
#else
	};
#endif

	m_remainingScenarioCnt = 0;

	//initialize the sub-scenarios of parent scenarios and add all
	//scenarios that need run to the list
	while (scenario->LN_Succ())
	{
		InitializeSubScenarios(scenario);
		if (scenario->m_childArray.size() > 0)
		{
			m_remainingScenarioCnt += scenario->m_childArray.size();
			if (microManage)
			{
				//if it has cumulative outputs, run all of its sub-scenarios in the same WISE instance
				if (hasCumulativeOutputs(m_project->m_outputs, scenario->m_name))
				{
					auto sr = new ScenarioRun();
					for (auto s : scenario->m_childArray)
					{
						sr->addScenario(s);
					}
					to_run.push_back(sr);
				}
				//if there are no cumulative outputs then each scenario can be load balanced to different machines
				else
				{
					for (auto s : scenario->m_childArray)
					{
						to_run.push_back(new ScenarioRun(s));
					}
				}
			}
			//if there is no load balancing, dump everything into the first entry
			else
			{
				for (auto s : scenario->m_childArray)
				{
					to_run[0]->addScenario(s);
				}
			}
		}
		else {
			m_remainingScenarioCnt++;
			if (microManage)
				to_run.push_back(new ScenarioRun(scenario));
			//if there is no load balancing, dump everything into the first entry
			else
				to_run[0]->addScenario(scenario);
		}
		scenario = scenario->LN_Succ();
	}

	//just validate the FGM, don't run it
	if (justSummary == SummaryType::VALIDATE)
	{
#ifndef __USE_GSL
		cleanup();
#endif

		return S_OK;
	}
	//run the simulations
	else
	{
		if (microManage) {
			std::uint32_t old_index = next_index;
			next_index = fetchNextScenarioIndex(next_index);
			// ***** need to adjust m_numScenariosRemaining here to account for other processes elsewhere running some stuff
			m_remainingScenarioCnt -= (next_index - old_index - 1);
		}
	while ((next_index < to_run.size()) && (cores_in_use < cores_available))
	{
		auto nowRunning = to_run[next_index];
		scenario = nowRunning->takeScenario();
		if (!m_stop_requested && scenario && scenario->m_simStatus == 0)
		{
			//poke the CPU usage method to keep it up to date
			CWorkerThreadPool::GetCurrentCpuUsage();
			if (SUCCEEDED(RunSimulation(scenario, justSummary, next_index)))
				cores_in_use++;
			else
			{
				std::string str = scenario->m_name;
				str += ": Error: ";
				str += last_error_msg;
					UpdateStatus(0, WISEStatus::SCENARIO_FAILED, str, -1, OUTPUT_STATUS_ALL);
			}
		}

		//get the next index to run
		if (nowRunning->isEmpty())
		{
			if (microManage)
			{
				if (cores_in_use < cores_available)
					next_index = fetchNextScenarioIndex(next_index + 1);
			}
			else
				next_index++;
		}
	}

	if (cores_in_use)
	{
	    while (true)
		{
			//prepare to start another scenario
			neosmart::WaitForEvent(m_event, (std::uint64_t)-1);

			SHORT result = 0;
			Project::Scenario* s;
			std::uint32_t s_cnt;
			while (result >= 0)
			{
				// some thread completed running a simulation and can now start exporting, so can start another simulation
				if ((result = SimulationCalculateContinue(&s, &s_cnt)) == 2)
				{
					if (s->m_parent)
						DebugStatus(2, WISEStatus::SCENARIO_COMPLETED, (boost::format("Child scenario %s.%d simulated.") % s->m_name % s_cnt).str());
					else
						DebugStatus(2, WISEStatus::SCENARIO_COMPLETED, (boost::format("Child scenario %s simulated.") % s->m_name).str());

					//cores_in_use--;
					if (cores_in_use < (cores_available * 2))
					{
						//get the next index to run
						if (microManage)
						{
							//if the index is already out of bounds or the current index is out of scenarios to run
							if (next_index >= to_run.size() || to_run[next_index]->isEmpty())
								next_index = fetchNextScenarioIndex(next_index + 1);
						}

						if (next_index < to_run.size())
						{
							auto nowRunning = to_run[next_index];
							scenario = nowRunning->takeScenario();

							if ((!m_stop_requested) && scenario && scenario->m_simStatus == 0)
							{
								//poke the CPU usage method to keep it up to date
								CWorkerThreadPool::GetCurrentCpuUsage();
								if (SUCCEEDED(RunSimulation(scenario, justSummary, next_index)))
									cores_in_use++;
								else {
									// what to do?
								}
							}

							if (!microManage && nowRunning->isEmpty())
								next_index++;
						}
					}
				}
				// a thread has completely exited after completing exporting, so count down how many things are running
				else if (result == 1)
				{
					if (s->m_parent)
						DebugStatus(2, WISEStatus::SCENARIO_COMPLETED, (boost::format("Child scenario %s.%d complete.") % s->m_name % s_cnt).str());
					else
						DebugStatus(2, WISEStatus::SCENARIO_COMPLETED, (boost::format("Child scenario %s complete.") % s->m_name).str());

					if (cores_in_use)
						cores_in_use--;

					//TODO do we need this here? or is the start after the simulation is run (before exporting) enough?
					if (cores_in_use < (cores_available * 2))
					{
						//get the next index to run
						if (microManage)
						{
							//if the index is already out of bounds or the current index is out of scenarios to run
							if (next_index >= to_run.size() || to_run[next_index]->isEmpty())
								next_index = fetchNextScenarioIndex(next_index + 1);
						}

						if (next_index < to_run.size())
						{
							auto nowRunning = to_run[next_index];
							scenario = nowRunning->takeScenario();

							if (!m_stop_requested && scenario && scenario->m_simStatus == 0)
							{
								//poke the CPU usage method to keep it up to date
								CWorkerThreadPool::GetCurrentCpuUsage();
								if (SUCCEEDED(RunSimulation(scenario, justSummary, next_index)))
									cores_in_use++;
							}

							if (!microManage && nowRunning->isEmpty())
								next_index++;
						}
					}

					if (cores_in_use == 0)
					{
						while (SimulationCalculateContinue(&s, &s_cnt) != -1)
							;

#ifndef __USE_GSL
							cleanup();
#endif

						return S_OK;
				}
			}
	    }
	}
	}
	}
	DebugStatus(0, WISEStatus::INFORMATION, (boost::format("Simulations complete at %s.") % getISO8601String()).str());

#ifndef __USE_GSL
	cleanup();
#endif

	return S_OK;
}


void SPARCS_P::Shutdown(const std::vector<std::string>& topic, JSON::ShutdownMessage message)
{
	UpdateStatus(1, WISEStatus::SHUTDOWN_REQUESTED, "Stopping scenarios", -1, OUTPUT_STATUS_MQTT);
	m_stop_requested = true;
	Project::Scenario *scenario = m_project->m_scenarioCollection.FirstScenario();
	while (scenario->LN_Succ())
	{
		if (scenario->m_worker)
		{
			((worker_struct*)scenario->m_worker)->worker->SignalShutdownThread();
		}
		scenario = scenario->LN_Succ();
	}
}


void SPARCS_P::UpdateJob(const std::vector<std::string>& topic, std::vector<JSON::UpdateMessage> messages)
{
	Project::Scenario *scenario = m_project->m_scenarioCollection.FirstScenario();
	while (scenario->LN_Succ())
	{
		std::string name(scenario->m_name);
		std::vector<JSON::UpdateMessage>::iterator it = std::find_if(messages.begin(), messages.end(), [&name](const JSON::UpdateMessage& other) { return other.name.compare(name) == 0; });
		if (it != messages.end() && it->status == 1)
		{
			scenario->m_simStatus = 1;
			if (scenario->m_worker)
				((worker_struct*)scenario->m_worker)->worker->SignalShutdownThread();
		}
		scenario = scenario->LN_Succ();
	}
}


std::tuple<const char*, const char*> SPARCS_P::guessDriverNameFromFilename(const char *filename)
{
	std::string ext = fs::path(filename).extension().string();
	const char *driver_name;
	const char* projection_name = "";
    if (boost::iequals(ext, ".SHP"))	driver_name = "ESRI Shapefile";
	else if (boost::iequals(ext, ".GPKG"))	driver_name = "GeoPackage vector";
	else if (boost::iequals(ext, ".TAB"))	driver_name = "MapInfo File";
	else if (boost::iequals(ext, ".DAT"))	driver_name = "MapInfo File";
	else if (boost::iequals(ext, ".MAP"))	driver_name = "MapInfo File";
	else if (boost::iequals(ext, ".ID"))	driver_name = "MapInfo File";
	else if (boost::iequals(ext, ".IND"))	driver_name = "MapInfo File";
	else if (boost::iequals(ext, ".JSON")) {
		driver_name = "GeoJSON";
		projection_name = SRS_WKT_WGS84_LAT_LONG;
	}
	else if (boost::iequals(ext, ".GEOJSON")) {
		driver_name = "GeoJSON";
		projection_name = SRS_WKT_WGS84_LAT_LONG;
	}
	else if (boost::iequals(ext, ".CSV"))	driver_name = "CSV";
	else if (boost::iequals(ext, ".GPX"))	driver_name = "GPX";
	else if (boost::iequals(ext, ".KMZ") || boost::iequals(ext, ".KML"))
	{
		driver_name = "LIBKML";
		projection_name = SRS_WKT_WGS84_LAT_LONG;
	}
	else					driver_name = "ARCInfo Generate";

	return { driver_name, projection_name };
}


HRESULT SPARCS_P::KMLHelper(const char *filename, const WTimeSpan &offset)
{
	if (!filename)
		return S_OK;
	fs::path path(filename);
	if (boost::iequals(path.extension().string(), ".kml") || boost::iequals(path.extension().string(), ".kmz"))
	{
		if (!fs::exists(path))
			return E_FAIL;

		KML::KmlHelper helper(path);
		if (!helper.process(path, offset))
			return E_FAIL;
	}
	return S_OK;
}


std::string SPARCS_P::FileNameToExeDirPath(const char* filename)
{
	if (m_path.length() == 0)
	{
#ifdef _MSC_VER
		std::vector<TCHAR> path;
		DWORD copied = 0;
		do
		{
			path.resize(path.size() + MAX_PATH);
			copied = ::GetModuleFileName(NULL, &path.at(0), path.size());
		} while (copied >= path.size());

		m_path = fs::path(&path.at(0)).parent_path().string();
#else
        std::array<char, 512> buffer{};
        size_t len = readlink("/proc/self/exe", buffer.data(), 512);
		m_path = fs::path(buffer.data()).parent_path().string();
#endif
		m_path += "/";
	}
	std::string fullPath(m_path);
	fullPath += filename;
	return fullPath;
}

bool SPARCS_P::initMQTT()
{
	return m_client.initMQTT();
}

bool SPARCS_P::deinitMQTT()
{
	return m_client.deinitMQTT();
}

void SPARCS_P::updateMQTT(std::uint16_t level, MQTTStatus _status, const char *status_msg, short qos)
{
	m_client.updateMQTT(level, _status, std::string(status_msg), qos);
}

void SPARCS_P::debugMQTT(std::uint16_t level, MQTTStatus _status, const char *status_msg)
{
	m_client.debugMQTT(level, _status, std::string(status_msg));
}

void SPARCS_P::emitStatisticsMQTT(std::uint16_t level, MQTTStatus _status, const char *status_msg, const std::vector<std::tuple<std::string, Service::StatisticType>> &statistics)
{
	m_client.updateMQTT(level, _status, status_msg, statistics);
}
