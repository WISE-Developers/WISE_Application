/**
 * WISE_Grid_Module: Stats.cpp
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
#include "WISE.h"
#include "statsentry.h"
#include "Stats.h"
#include "VectorExportOptions.h"
#include "PrintReportOptions.h"
#include "StatsConfig.h"
#include "StatsBuilder.h"
#include "macros.h"
#include "str_printf.h"
#include "stdchar.h"

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

#include <vector>
#include "filesystem.hpp"
#include <algorithm>
#include <bitset>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/chrono.hpp>
#include <gsl/gsl>

using namespace std::string_literals;
using namespace HSS_Time;

#include "filesystem.hpp"


constexpr std::int32_t thirtyFirstPosition = static_cast<std::int32_t>(0x40000000);
constexpr std::int32_t thirtySecondPosition = static_cast<std::int32_t>(0x80000000);


#define DEFAULT_ENTRIES_NUM_STATS 67
const struct viewentry default_entries[DEFAULT_ENTRIES_NUM_STATS] = {	// order matches enums for StatsOutput::StatisticsType
/* 0 */
	{ _T("Date and Time"),				_T("%s"),		_T("%s"),		_T("Date and Time"),		_T(""),									thirtyFirstPosition | Project::CWFGMStatsEntry::OPERATION_DATETIME },
	{ _T("Elapsed Time"),				_T("%s"),		_T("%s"),		_T("Elapsed Time"),			_T(""),									thirtyFirstPosition | Project::CWFGMStatsEntry::OPERATION_TIMESPAN | (1 << 16) },
	{ _T("Time Step\nDuration"),		_T("%s"),		_T("%s"),		_T("Time Step Duration"),	_T(""),									thirtyFirstPosition | Project::CWFGMStatsEntry::OPERATION_TIMESPAN | (3 << 16),					0, 0, NULL, true },
	{ _T("Temp\n"),						_T("%s(%s)"),	_T("%.*lf"),	_T("Temp"),					_T("Temperature (%s)"),					thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (1 << 16),					1 },
	{ _T("Dew Point\n"),				_T("%s(%s)"),	_T("%.*lf"),	_T("Dew Pt."),				_T("Dew Point (%s)"),					thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (18 << 16),					1, 0, NULL, true },
	{ _T("RH\n(%)"),					_T("%s"),		_T("%.*lf"),	_T("RH"), 					_T("Relative humidity (%%)"),			0x00000000 | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (2 << 16),					1 },
	{ _T("WS\n"),						_T("%s(%s)"),	_T("%.*lf"),	_T("WS"),					_T("Wind speed (%s)"),					thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (4 << 16),					1 },
	{ _T("WD\n"),						_T("%s(%s)"),	_T("%.*lf"),	_T("WD"),					_T("Wind direction (%s)"),				thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (3 << 16),					0 },
	{ _T("Precip\n"),					_T("%s(%s)"),	_T("%.*lf"),	_T("Precip"),				_T("Precipitation (%s)"),				thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (5 << 16),					1 }, //originally the second is "%s (%s)", the third is "%.*lf"
/* 9 */
	{ _T("HFFMC"),						_T("%s"),		_T("%.*lf"),	_T("HFFMC"),				_T("Hourly Fine Fuel Moisture Code"),	0x00000000 | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (6 << 16),					1 },
	{ _T("HISI"),						_T("%s"),		_T("%.*lf"),	_T("HISI"),					_T("Hourly Initial Spread Index"),		0x00000000 | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (11 << 16),					1 },
	{ _T("DMC"),						_T("%s"),		_T("%.*lf"),	_T("DMC"),					_T("Daily Duff Moisture Code"),			0x00000000 | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (8 << 16),					1 },
	{ _T("DC"),							_T("%s"),		_T("%.*lf"),	_T("DC"),					_T("Daily Drought Code"),				0x00000000 | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (7 << 16),					1 },
	{ _T("HFWI"),						_T("%s"),		_T("%.*lf"),	_T("HFWI"),					_T("Hourly Fire Weather Index"),		0x00000000 | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (12 << 16),					1 },
	{ _T("BUI"),						_T("%s"),		_T("%.*lf"),	_T("BUI"),					_T("Daily Buildup Index"),				thirtyFirstPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (10 << 16),					1 },
	{ _T("FFMC"),						_T("%s"),		_T("%.*lf"),	_T("FFMC"),					_T("Daily Fine Fuel Moisture Code"),	0x00000000 | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (9 << 16),					1, 0, NULL, true },
	{ _T("ISI"),						_T("%s"),		_T("%.*lf"),	_T("ISI"),					_T("Daily Initial Spread Index"),		0x00000000 | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (16 << 16),					1, 0, NULL, true },
	{ _T("FWI"),						_T("%s"),		_T("%.*lf"),	_T("FWI"),					_T("Daily Fire Weather Index"),			0x00000000 | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (17 << 16),					1, 0, NULL, true },
/* 18 */
	{ _T("Time Step\nArea"),			_T("%s (%s)"),	_T("%.*lf"),	_T("Time Step Area"),		_T(""),									thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_AREA_CHANGE << 16),		2, 0, NULL, true },
	{ _T("Time Step\nBurn Area"),		_T("%s (%s)"),	_T("%.*lf"),	_T("Time Step Burn Area"),		_T(""),								thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_BURNED_CHANGE << 16),		2, 0, NULL, true },
	{ _T("Area"),						_T("%s (%s)"),	_T("%.*lf"),	_T("Area"),					_T(""),									thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_AREA << 16),			2 },
	{ _T("Total\nBurn Area"),			_T("%s (%s)"),	_T("%.*lf"),	_T("Total Burn Area"),			_T(""),								thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_BURNED << 16),		2, 0, NULL, true },
	{ _T("Area\nGrowth Rate"),			_T("%s (%s)"),	_T("%.*lf"),	_T("Area Growth Rate"),		_T(""),									thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_AREA_GROWTH << 16),		2, 0, NULL, true },
	{ _T("Perimeter\n"),				_T("%s (%s)"),	_T("%.*lf"),	_T("Perimeter"),			_T(""),									thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_EXTERIOR_PERIMETER << 16),	2 },
	{ _T("Perimeter Growth\nRate"),		_T("%s (%s)"),	_T("%.*lf"),	_T("Perimeter Growth Rate"),_T(""),									thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_EXTERIOR_PERIMETER_GROWTH << 16),2,0, NULL, true },
	{ _T("Active\nPerimeter"),			_T("%s (%s)"),	_T("%.*lf"),	_T("Active Perimeter"),		_T(""),									thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_ACTIVE_PERIMETER << 16),	2 },
	{ _T("Active Perimeter\nGrowth Rate"),	_T("%s (%s)"),		_T("%.*lf"),	_T("Perimeter Growth Rate"),	_T(""),						thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_ACTIVE_PERIMETER_GROWTH << 16),2, 0, NULL, true },
	{ _T("Total Perimeter\n"),		_T("%s (%s)"),		_T("%.*lf"),	_T("Total Perimeter"),		_T(""),									thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_TOTAL_PERIMETER << 16),	2, 0, NULL, true },
	{ _T("Total Perimeter Growth\nRate"),	_T("%s (%s)"),		_T("%.*lf"),	_T("Total Perimeter Growth Rate"),_T(""),					thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_TOTAL_PERIMETER_GROWTH << 16),	2, 0, NULL, true },
/* 29 */
	{ _T("% FI\n< 10 (kW/m)"),			_T("%s"),		_T("%.*lf"),	_T("FI (<10)"),				_T("Percent FI < 10 (kW/m)"),			thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (1 << 16),					2, 0, NULL, true },
	{ _T("% FI\n10 - 500 (kW/m)"),		_T("%s"),		_T("%.*lf"),	_T("FI (10-500)"),			_T("Percent FI 10 - 500 (kW/m)"),		thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (2 << 16),					2, 0, NULL, true },
	{ _T("% FI\n500 - 2000 (kW/m)"),	_T("%s"),		_T("%.*lf"),	_T("FI (500-2000)"),		_T("Percent FI 500 - 2000 (kW/m)"),		thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (3 << 16),					2, 0, NULL, true },
	{ _T("% FI\n2000 - 4000 (kW/m)"),	_T("%s"),		_T("%.*lf"),	_T("FI (2000-4000)"),		_T("Percent FI 2000 - 4000 (kW/m)"),	thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (4 << 16),					2, 0, NULL, true },
	{ _T("% FI\n4000 - 10000 (kW/m)"),	_T("%s"),		_T("%.*lf"),	_T("FI (4000-10000)"),		_T("Percent FI 4000 - 10000 (kW/m)"),	thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (5 << 16),					2, 0, NULL, true },
	{ _T("% FI\n> 10000 (kW/m)"),		_T("%s"),		_T("%.*lf"),	_T("FI (>10000)"),			_T("Percent FI > 10000 (kW/m)"),		thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (6 << 16),					2, 0, NULL, true },
/* 35 */
	{ _T("% ROS\n0 - 1 (m/min)"),		_T("%s"),		_T("%.*lf"),	_T("ROS (0-1)"),			_T("Percent ROS 0 - 1 (m/min)"),		thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (7 << 16),					2, 0, NULL, true },
	{ _T("% ROS\n2 - 4 (m/min)"),		_T("%s"),		_T("%.*lf"),	_T("ROS (2-4)"),			_T("Percent ROS 2 - 4 (m/min)"),		thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (8 << 16),					2, 0, NULL, true },
	{ _T("% ROS\n5 - 8 (m/min)"),		_T("%s"),		_T("%.*lf"),	_T("ROS (5-8)"),			_T("Percent ROS 5 - 8 (m/min)"),		thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (9 << 16),					2, 0, NULL, true },
	{ _T("% ROS\n9 - 14 (m/min)"),		_T("%s"),		_T("%.*lf"),	_T("ROS (9-14)"),			_T("Percent ROS 9 - 14 (m/min)"),		thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (10 << 16),					2, 0, NULL, true },
	{ _T("% ROS\n> 15 (m/min)"),		_T("%s"),		_T("%.*lf"),	_T("ROS (>15)"),			_T("Percent ROS > 15 (m/min)"),			thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_STATS_FBP | (11 << 16),					2, 0, NULL, true },
/* 40 */
	{ _T("Maximum\nROS"),				_T("%s (%s)"),	_T("%.*lf"),	_T("Maximum ROS"),			_T("Maximum rate of spread (%s)"),		thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (40 << 16),					2, 0, NULL, true},
	{ _T("Maximum\nFI"),				_T("%s (%s)"),	_T("%.*lf"),	_T("Maximum FI"),			_T("Maximum fire intensity (%s)"),		thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (41 << 16),					2, 0, NULL, true},
	{ _T("Maximum\nFlame Length"),		_T("%s (%s)"),	_T("%.*lf"),	_T("Maximum Flame Length"),	_T("Maximum flame length (%s)"),		thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (46 << 16),					2, 0, NULL, true},
	{ _T("Maximum\nCFB"),				_T("%s (%%)"),	_T("%.*lf"),	_T("Maximum CFB"),			_T("Maximum crown fraction burned (%%)"),0x00000000 | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (42 << 16),					2, 0, NULL, true},
	{ _T("Maximum\nCFC"),				_T("%s (%s)"),	_T("%.*lf"),	_T("Maximum CFC"),			_T("Maximum crown fuel consumption (%s)"),thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (43 << 16),					2, 0, NULL, true},
	{ _T("Maximum\nSFC"),				_T("%s (%s)"),	_T("%.*lf"),	_T("Maximum SFC"),			_T("Maximum surface fuel consumption (%s)"),thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (44 << 16),					2, 0, NULL, true},
	{ _T("Maximum\nTFC"),				_T("%s (%s)"),	_T("%.*lf"),	_T("Maximum TFC"),			_T("Maximum total fuel consumption (%s)"),thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (45 << 16),					2, 0, NULL, true},
	{ _T("Time Step\nTotal Fuel Consumed"),		_T("%s (%s)"),	_T("%.*lf"),	_T("Time Step Total Fuel Consumed"),		_T(""),			thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_TOTAL_FUEL_CONSUMED << 16),		2, 0, NULL, true },
	{ _T("Time Step\nCrown Fuel Consumed"),		_T("%s (%s)"),	_T("%.*lf"),	_T("Time Step Crown Fuel Consumed"),		_T(""),			thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_CROWN_FUEL_CONSUMED << 16),		2, 0, NULL, true },
	{ _T("Time Step\nSurface Fuel Consumed"),	_T("%s (%s)"),	_T("%.*lf"),	_T("Time Step Surface Fuel Consumed"),		_T(""),			thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_SURFACE_FUEL_CONSUMED << 16),		2, 0, NULL, true },
/* 50 */
	{ _T("Active\nVertices"),			_T("%s"),		_T("%d"),		_T("Active Vertices"),		_T(""),									0x00000000 | Project::CWFGMStatsEntry::OPERATION_INT | (CWFGM_FIRE_STAT_NUM_ACTIVE_POINTS << 16),		0, 0, NULL, true },
	{ _T("Vertices"),					_T("%s"),		_T("%d"),		_T("Vertices"),				_T(""),									0x00000000 | Project::CWFGMStatsEntry::OPERATION_INT | (CWFGM_FIRE_STAT_NUM_POINTS << 16),		0, 0, NULL, true },
	{ _T("Cumulative\nVertices"),		_T("%s"),		_T("%d"),		_T("Cumulative Vertices"),	_T(""),									0x00000000 | Project::CWFGMStatsEntry::OPERATION_INT | (CWFGM_FIRE_STAT_CUMULATIVE_NUM_POINTS << 16),	0, 0, NULL, true },
	{ _T("Cumulative\nActive Vertices"),_T("%s"),		_T("%d"),		_T("Cumulative Active Vertices"),_T(""),							0x00000000 | Project::CWFGMStatsEntry::OPERATION_INT | (CWFGM_FIRE_STAT_CUMULATIVE_NUM_ACTIVE_POINTS << 16),0, 0, NULL, true },
	{ _T("Active\nFronts"),				_T("%s"),		_T("%d"),		_T("Active Fronts"),		_T(""),									0x00000000 | Project::CWFGMStatsEntry::OPERATION_INT | (CWFGM_FIRE_STAT_NUM_ACTIVE_FRONTS << 16),		0, 0, NULL, true },
	{ _T("Fronts"),						_T("%s"),		_T("%d"),		_T("Fronts"),				_T(""),									0x00000000 | Project::CWFGMStatsEntry::OPERATION_INT | (CWFGM_FIRE_STAT_NUM_FRONTS << 16),		0, 0, NULL, true },
	{ _T("Memory\n(used, start)"),		_T("%s"),		_T("%d"),		_T("Memory (used, start)"),	_T(""),									0x00000000 | Project::CWFGMStatsEntry::OPERATION_ULONGLONG | (CWFGM_FIRE_STAT_TIMESTEP_MEMORY_BEGIN_USED << 16), 0, 0, NULL, TRUE },
	{ _T("Memory\n(used, end)"),		_T("%s"),		_T("%d"),		_T("Memory (used, end)"),	_T(""),									0x00000000 | Project::CWFGMStatsEntry::OPERATION_ULONGLONG | (CWFGM_FIRE_STAT_TIMESTEP_MEMORY_END_USED << 16), 0, 0, NULL, TRUE },
/* 58 */
	{ _T("Number\nTimesteps"),			_T("%s"),		_T("%d"),		_T("Number Timesteps"),		_T(""),									0x00000000 | Project::CWFGMStatsEntry::OPERATION_ULONGLONG | (CWFGM_FIRE_STAT_NUM_TIMESTEPS << 16),	0, 0, NULL, true },
	{ _T("Number\nDisplay Timesteps"),	_T("%s"),		_T("%d"),		_T("Number Display Timesteps"),	_T(""),								0x00000000 | Project::CWFGMStatsEntry::OPERATION_ULONGLONG | (CWFGM_FIRE_STAT_NUM_DISPLAY_TIMESTEPS << 16),	0, 0, NULL, true },
	{ _T("Number\nEvent Timesteps"),	_T("%s"),		_T("%d"),		_T("Number Event Timesteps"),	_T(""),								0x00000000 | Project::CWFGMStatsEntry::OPERATION_ULONGLONG | (CWFGM_FIRE_STAT_NUM_EVENT_TIMESTEPS << 16),	0, 0, NULL, true },
	{ _T("Number\nCalc. Timesteps"),	_T("%s"),		_T("%d"),		_T("Number Calc. Timesteps"),	_T(""),								0x00000000 | Project::CWFGMStatsEntry::OPERATION_ULONGLONG | (CWFGM_FIRE_STAT_NUM_CALC_TIMESTEPS << 16),	0, 0, NULL, true },

	{ _T("Ticks"),						_T("%s"),		_T("%d"),		_T("Ticks"),				_T(""),									0x00000000 | Project::CWFGMStatsEntry::OPERATION_INT | (CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_TICKS << 16), 0, 0, NULL, TRUE },
	{ _T("Elapsed\nProcessing Time"),	_T("%s"),		_T("%s"),		_T("Elapsed Processing Time"),_T(""),								0x00000000 | Project::CWFGMStatsEntry::OPERATION_TIMESPAN | (CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_REALTIME << 16), 0, 0, NULL, true },
/* 64 */
	{ _T("Area\nGrowth Time"),			_T("%s"),		_T("%s"),		_T("Area Growth Time"),		_T(""),									thirtyFirstPosition | Project::CWFGMStatsEntry::OPERATION_TIMESPAN | (CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_BURNING_SECS << 16), 0, 0, NULL, true },

/* 74, unused */
	{ _T("Time Step\nRadiative Power"),	_T("%s (%s)"),	_T("%.*lf"),	_T("Radiative Power"),		_T(""),									thirtySecondPosition | Project::CWFGMStatsEntry::OPERATION_DOUBLE | (CWFGM_FIRE_STAT_RADIATIVE_POWER << 16),		2, 0, NULL, true },

	{ NULL }
};

template<typename V, typename... T>
constexpr auto array_of(T&&... t) -> std::array<V, sizeof...(T)>
{
	return { { std::forward<T>(t)... } };
}

const std::array<std::string, DEFAULT_ENTRIES_NUM_STATS> default_entry_ids = array_of<std::string>(	// order matches enums for StatsOutput::StatisticsType
/* 0 */
	"date_time"s,
	"elapsed_time"s,
	"time_step_duration"s,
	"temperature"s,
	"dew_point"s,
	"relative_humidity"s,
	"wind_speed"s,
	"wind_direction"s,
	"precipitation"s,
/* 9 */
	"hffmc"s,
	"hisi"s,
	"dmc"s,
	"dc"s,
	"hfwi"s,
	"bui"s,
	"ffmc"s,
	"isi"s,
	"fwi"s,
/* 18 */
	"timestep_area"s,
	"timestep_burn_area"s,
	"total_area"s,
	"total_burn_area"s,
	"area_growth_rate"s,
	"exterior_perimeter"s,
	"exterior_perimeter_growth_rate"s,
	"active_perimeter"s,
	"active_perimeter_growth_rate"s,
	"total_perimeter"s,
	"total_perimeter_growth_rate"s,
/* 29 */
	"fi_lt_10"s,
	"fi_10_500"s,
	"fi_500_2000"s,
	"fi_2000_4000"s,
	"fi_4000_10000"s,
	"fi_gt_10000"s,
/* 35 */
	"ros_0_1"s,
	"ros_2_4"s,
	"ros_5_8"s,
	"ros_9_14"s,
	"ros_gt_15"s,
/* 40 */
	"max_ros"s,
	"max_fi"s,
	"max_fl"s,
	"max_cfb"s,
	"max_cfc"s,
	"max_sfc"s,
	"max_tfc"s,
	"total_fuel_consumed"s,
	"crown_fuel_consumed"s,
	"surface_fuel_consumed"s,
/* 50 */
	"num_active_vertices"s,
	"num_vertices"s,
	"cumulative_vertices"s,
	"cumulative_active_vertices"s,
	"num_active_fronts"s,
	"num_fronts"s,
	"memory_used_start"s,
	"memory_used_end"s,
/* 58 */
	"num_timesteps"s,
	"num_display_timesteps"s,
	"num_event_timesteps"s,
	"num_calc_timesteps"s,

	"ticks"s,
	"processing_time"s,
/* 64 */
	"growth_time"s,

	"radiative_power"s,
	""s
);


static std::string getUnits(UnitConversion* unitConversion, LONG operation)
{
	switch (operation & 0x0000ffff)
	{
	case Project::CWFGMStatsEntry::OPERATION_DOUBLE:
		switch ((operation & 0x0fffffff) >> 16)
		{
		case 1: /* temperature */
			return unitConversion->UnitName(unitConversion->TempDisplay(), true);
		case 3: /* wa */
			return _T(DEGREE_SYMBOL);
		case 4: /* ws */
			return unitConversion->UnitName(unitConversion->VelocityDisplay(), true);
		case 5: /* precip */
			return unitConversion->UnitName(unitConversion->SmallMeasureDisplay(), true);
		case 18: /* dew point */
			return unitConversion->UnitName(unitConversion->TempDisplay(), true);

		case CWFGM_FIRE_STAT_AREA_CHANGE:
		case CWFGM_FIRE_STAT_BURNED_CHANGE:
		case CWFGM_FIRE_STAT_BURNED:
		case CWFGM_FIRE_STAT_AREA:
			return unitConversion->UnitName(unitConversion->AreaDisplay(), true);

		case CWFGM_FIRE_STAT_TOTAL_FUEL_CONSUMED:
		case CWFGM_FIRE_STAT_CROWN_FUEL_CONSUMED:
		case CWFGM_FIRE_STAT_SURFACE_FUEL_CONSUMED:
			return unitConversion->UnitName(unitConversion->MassDisplay(), true);

		case CWFGM_FIRE_STAT_RADIATIVE_POWER:
			return unitConversion->UnitName(unitConversion->PowerDisplay(), true);

		case CWFGM_FIRE_STAT_AREA_GROWTH:
			return unitConversion->UnitName(unitConversion->AreaDisplay(), true);

		case CWFGM_FIRE_STAT_ACTIVE_PERIMETER:
		case CWFGM_FIRE_STAT_TOTAL_PERIMETER:
		case CWFGM_FIRE_STAT_EXTERIOR_PERIMETER:
			return unitConversion->UnitName(unitConversion->AltDistanceDisplay(), true);

		case CWFGM_FIRE_STAT_ACTIVE_PERIMETER_GROWTH:
		case CWFGM_FIRE_STAT_TOTAL_PERIMETER_GROWTH:
		case CWFGM_FIRE_STAT_EXTERIOR_PERIMETER_GROWTH:
			return unitConversion->UnitName(unitConversion->AltDistanceDisplay() | STORAGE_FORMAT_HOUR, true);

		case 40:
			return unitConversion->UnitName(unitConversion->AltVelocityDisplay(), true);

		case 41:
			return unitConversion->UnitName(unitConversion->IntensityDisplay(), true);

		case 46:
			return unitConversion->UnitName(unitConversion->SmallDistanceDisplay(), true);

		case 43:
		case 44:
		case 45:
			return unitConversion->UnitName(unitConversion->MassAreaDisplay(), true);
		}
		break;
	}
	return {};
}


static void doStats(Project::CWFGMProject* project, const ICWFGM_FWI* comfwi, const Project::StatsOutputs& stats, const Project::stats::StatsEntry& entry,
	Project::Scenario *s, ::UnitConversion *unitConversion, Project::CWFGMStatsEntryCollection& collection, FILE *f, const WTime& start_time,
	const WTime& end_time, const WTime& time)
{
	bool na;
	Project::WeatherStream* wstream = nullptr;
	if (entry.use_stream)
	{
		if (entry.streamName.length() > 0)
		{
			std::uint32_t count = s->GetStreamCount();
			for (int i = 0; i < count; i++)
			{
				Project::WeatherStream* str = s->StreamAtIndex(i);
				std::string name = str->m_name;
				if (boost::iequals(name, entry.streamName))
				{
					wstream = str;
					break;
				}
			}
		}
		else
			wstream = s->StreamAtIndex(entry.stream_idx);
		if (wstream)
		{
			WTime start((ULONGLONG)0, time.GetTimeManager());
			WTimeSpan duration;
			wstream->GetValidTimeRange(start, duration);
			na = ((time < start) || ((start + duration) < time));
		}
		else
			na = true;
	}
	else
		na = false;

	Project::BasicScenarioOptions bso(s->m_scenario.get());
	double temp = 0, rh = 0, precip = 0,
		ws = 0, wa = 0, ffmc = 0, dc = 0, dmc = 0, bui = 0, isi = 0,
		fwi = 0, tffmc = 0, sisi = 0, sfwi = 0, dew = 0, d_fwi = 0, d_isi = 0;
	double dailyFFMC;

	if (entry.use_stream)
	{
		if ((!na) && (wstream))
		{
			IWXData wx;
			IFWIData ifwi;
			DFWIData dfwi;
			wstream->GetInstantaneousValues(time, 0, NULL, &ifwi, NULL);
			ffmc = ifwi.FFMC;
			isi = ifwi.ISI;
			fwi = ifwi.FWI;

			wstream->GetInstantaneousValues(time, (bso.m_bWeatherTemporalInterpolate) ? CWFGM_GETWEATHER_INTERPOLATE_TEMPORAL : 0, &wx, &ifwi, &dfwi);
			temp = wx.Temperature;
			rh = wx.RH;
			precip = wx.Precipitation;
			ws = wx.WindSpeed;
			wa = wx.WindDirection;
			dew = wx.DewPointTemperature;
			tffmc = ifwi.FFMC;
			dc = dfwi.dDC;
			dmc = dfwi.dDMC;
			bui = dfwi.dBUI;
			d_fwi = dfwi.dFWI;
			d_isi = dfwi.dISI;
			sisi = ifwi.ISI;
			sfwi = ifwi.FWI;

			dailyFFMC = dfwi.dFFMC;
		}
	}
	else
	{
		if (s)
		{
			ICWFGM_GridEngine* grid = s->gridEngine();
			auto layerThread = s->layerThread();

			Project::WeatherStream* pStream = s->StreamAtIndex(0);
			Project::BasicScenarioOptions bso1(s->m_scenario.get());
			ULONG stream_interpolate_method = bso1.m_bWeatherTemporalInterpolate ? CWFGM_GETWEATHER_INTERPOLATE_TEMPORAL : 0;
			stream_interpolate_method |= bso1.m_bWeatherSpatialInterpolate ? CWFGM_GETWEATHER_INTERPOLATE_SPATIAL : 0;
			stream_interpolate_method |= bso1.m_bPrecipitationInterpolation ? CWFGM_GETWEATHER_INTERPOLATE_PRECIP : 0;
			stream_interpolate_method |= bso1.m_bWindInterpolation ? CWFGM_GETWEATHER_INTERPOLATE_WIND : 0;
			stream_interpolate_method |= bso1.m_bWindInterpolationVector ? CWFGM_GETWEATHER_INTERPOLATE_WIND_VECTOR : 0;
			stream_interpolate_method |= bso1.m_bTempRHInterpolation ? CWFGM_GETWEATHER_INTERPOLATE_TEMP_RH : 0;

			stream_interpolate_method |= bso1.m_bHistoryInterpolation ? CWFGM_GETWEATHER_INTERPOLATE_HISTORY : 0;
			stream_interpolate_method |= bso1.m_bCalcSpatialFWI ? CWFGM_GETWEATHER_INTERPOLATE_CALCFWI : 0;

			IWXData wx;
			IFWIData ifwi, hfwi;
			DFWIData dfwi;
			bool wx_valid;
			XY_Rectangle cacheBox;

			grid->GetWeatherData(layerThread, entry.location, time, stream_interpolate_method, &wx, &ifwi, &dfwi, &wx_valid, &cacheBox);
			temp = wx.Temperature;
			rh = wx.RH;
			precip = wx.Precipitation;
			ws = wx.WindSpeed;
			wa = wx.WindDirection;
			dew = wx.DewPointTemperature;

			tffmc = ifwi.FFMC;
			sisi = ifwi.ISI;
			sfwi = ifwi.FWI;

			dailyFFMC = dfwi.dFFMC;
			dc = dfwi.dDC;
			dmc = dfwi.dDMC;
			bui = dfwi.dBUI;
			d_isi = dfwi.dISI;
			d_fwi = dfwi.dFWI;

			WTime hr(time);
			hr.PurgeToHour(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
			grid->GetWeatherData(layerThread, entry.location, hr, stream_interpolate_method, NULL, &hfwi, NULL, &wx_valid, &cacheBox);
			ffmc = hfwi.FFMC;
			isi = hfwi.ISI;
			fwi = hfwi.FWI;
		}
	}

	Project::CWFGMStatsEntry* se = collection.FirstStat();
	std::string strText;
	PolymorphicAttribute pattr;
	WTimeSpan ts;
	std::uint64_t ull_value;
	double dbl_value, val;
	std::int32_t ii_value;

	while (se->LN_Succ())
	{
		WTime t(time);
		switch (se->m_operation & 0x0000ffff)
		{
		case Project::CWFGMStatsEntry::OPERATION_STATS_FBP:
			if (!s)
				break;

			switch ((se->m_operation & 0x0fffffff) >> 16)
			{
			case 1:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 0.0, 10.0, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			case 2:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 10.0, 500.0, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			case 3:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 500.0, 2000.0, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			case 4:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 2000.0, 4000.0, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			case 5:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 4000.0, 10000.0, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			case 6:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 10000.0, 1.0e50, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			case 7:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, 0.0, 1.5, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			case 8:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, 1.5, 4.5, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			case 9:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, 4.5, 8.5, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			case 10:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, 8.5, 14.5, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			case 11:
				s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, 14.5, 1.0e50, &val);
				strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				break;
			}
			break;

		case Project::CWFGMStatsEntry::OPERATION_DATETIME:
			strText = time.ToString(WTIME_FORMAT_TIME | WTIME_FORMAT_WITHDST | WTIME_FORMAT_DATE |
				WTIME_FORMAT_STRING_YYYYhMMhDD | WTIME_FORMAT_AS_LOCAL);
			break;

		case Project::CWFGMStatsEntry::OPERATION_TIMESPAN:
			switch ((se->m_operation & 0x0fffffff) >> 16)
			{
			case 1:
				ts = time - start_time;
				break;
			case 2:
				ts = end_time - time;
				break;
			case 3:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TIMESTEP_DURATION_SECS, stats.discretize, &pattr);
				VariantToTimeSpan_(pattr, &ts);
				break;
			case CWFGM_FIRE_STAT_TIMESTEP_REALTIME:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TIMESTEP_REALTIME, stats.discretize, &pattr);
				VariantToUInt64_(pattr, &ull_value);
				ts = WTimeSpan(ull_value, false);
				break;
			case CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_REALTIME:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_REALTIME, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				ts = WTimeSpan(gsl::narrow_cast<std::int64_t>(dbl_value), false);
				break;
			case CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_BURNING_SECS:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_BURNING_SECS, stats.discretize, &pattr);
				VariantToTimeSpan_(pattr, &ts);
				break;
			}
			strText = ts.ToString(WTIME_FORMAT_TIME);
			break;

		case Project::CWFGMStatsEntry::OPERATION_STEPCOLOR:
			ts = time - start_time;
			break;

		case Project::CWFGMStatsEntry::OPERATION_INT:
			s->GetStats((ULONG)-1, t, ((se->m_operation & 0x0fffffff) >> 16), stats.discretize, &pattr);
			VariantToInt32_(pattr, &ii_value);
			strText = strprintf("%d", ii_value);
			break;

		case Project::CWFGMStatsEntry::OPERATION_ULONGLONG:
			s->GetStats((ULONG)-1, t, ((se->m_operation & 0x0fffffff) >> 16), stats.discretize, &pattr);
			VariantToUInt64_(pattr, &ull_value);
			strText = strprintf("%I64u", ull_value);
			break;

		case Project::CWFGMStatsEntry::OPERATION_DOUBLE:
			switch ((se->m_operation & 0x0fffffff) >> 16)
			{
			case 1: /* temperature */
				val = na ? -9999.0 : unitConversion->ConvertUnit(temp, STORAGE_FORMAT_CELSIUS, unitConversion->TempDisplay());
				break;
			case 2: /* rh */
				val = na ? -9999.0 : rh * 100.0;
				break;
			case 3: /* wa */
				val = na ? -9999.0 : CARTESIAN_TO_COMPASS_DEGREE(RADIAN_TO_DEGREE(wa));
				if ((val <= 0.0000001) && (ws != 0.0))
					val = 360.0;
				break;
			case 4: /* ws */
				val = na ? -9999.0 : unitConversion->ConvertUnit(ws, STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, unitConversion->VelocityDisplay());
				break;
			case 5: /* precip */
				val = na ? -9999.0 : unitConversion->ConvertUnit(precip, STORAGE_FORMAT_MM, unitConversion->SmallMeasureDisplay());
				break;
			case 6: /* ffmc */
				val = na ? -9999.0 : ffmc;
				break;
			case 7: /* dc */
				val = na ? -9999.0 : dc;
				break;
			case 8: /* dmc */
				val = na ? -9999.0 : dmc;
				break;
			case 9: /* daily standard ffmc */
				val = na ? -9999.0 : dailyFFMC;
				break;
			case 10:	// daily bui
				val = na ? -9999.0 : bui;
				break;
			case 11:	// hourly isi
				val = na ? -9999.0 : isi;
				break;
			case 12:	// hourly fwi
				val = na ? -9999.0 : fwi;
				break;
			case 13:
				val = na ? -9999.0 : tffmc;
				break;
			case 14:
				val = na ? -9999.0 : sisi;
				break;
			case 15:
				val = na ? -9999.0 : sfwi;
				break;
			case 16:
				val = na ? -9999.0 : d_isi;
				break;
			case 17:
				val = na ? -9999.0 : d_fwi;
				break;
			case 18:
				val = na ? -9999.0 : unitConversion->ConvertUnit(dew, STORAGE_FORMAT_CELSIUS, unitConversion->TempDisplay());
				break;

			case CWFGM_FIRE_STAT_AREA_CHANGE:
			case CWFGM_FIRE_STAT_BURNED_CHANGE:
			case CWFGM_FIRE_STAT_BURNED:
			case CWFGM_FIRE_STAT_AREA:
				s->GetStats((ULONG)-1, t, (se->m_operation & 0x0fffffff) >> 16, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M2, unitConversion->AreaDisplay());
				break;

			case CWFGM_FIRE_STAT_TOTAL_FUEL_CONSUMED:
			case CWFGM_FIRE_STAT_CROWN_FUEL_CONSUMED:
			case CWFGM_FIRE_STAT_SURFACE_FUEL_CONSUMED:
				s->GetStats((ULONG)-1, t, (se->m_operation & 0x0fffffff) >> 16, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_KG, unitConversion->MassDisplay());
				break;

			case CWFGM_FIRE_STAT_RADIATIVE_POWER:
				s->GetStats((ULONG)-1, t, (se->m_operation & 0x0fffffff) >> 16, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_KILOJOULE | STORAGE_FORMAT_SECOND, unitConversion->PowerDisplay()); break;
				break;

			case CWFGM_FIRE_STAT_AREA_GROWTH:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_AREA_GROWTH, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M2 | STORAGE_FORMAT_MINUTE, unitConversion->AreaDisplay() | STORAGE_FORMAT_HOUR); break;
				break;

			case CWFGM_FIRE_STAT_ACTIVE_PERIMETER:
			case CWFGM_FIRE_STAT_TOTAL_PERIMETER:
			case CWFGM_FIRE_STAT_EXTERIOR_PERIMETER:
				s->GetStats((ULONG)-1, t, (se->m_operation & 0x0fffffff) >> 16, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M, unitConversion->AltDistanceDisplay()); 	// RWB: turned this back on 070417
				break;

			case CWFGM_FIRE_STAT_ACTIVE_PERIMETER_GROWTH:
			case CWFGM_FIRE_STAT_TOTAL_PERIMETER_GROWTH:
			case CWFGM_FIRE_STAT_EXTERIOR_PERIMETER_GROWTH:
				s->GetStats((ULONG)-1, t, (se->m_operation & 0x0fffffff) >> 16, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M | STORAGE_FORMAT_MINUTE, unitConversion->AltDistanceDisplay() | STORAGE_FORMAT_HOUR);
				break;

			case 40:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M | STORAGE_FORMAT_MINUTE, unitConversion->AltVelocityDisplay());
				break;

			case 41:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_FI, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KILOWATT_SECOND << 0x20) | STORAGE_FORMAT_M, unitConversion->IntensityDisplay());
				break;

			case 46:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_FLAMELENGTH, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M, unitConversion->SmallDistanceDisplay());
				break;

			case 42:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_CFB, stats.discretize, &pattr);
				VariantToDouble_(pattr, &val);
				break;

			case 43:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_CFC, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KG << 0x20) | STORAGE_FORMAT_M2, unitConversion->MassAreaDisplay());
				break;

			case 44:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_SFC, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KG << 0x20) | STORAGE_FORMAT_M2, unitConversion->MassAreaDisplay());
				break;

			case 45:
				s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TFC, stats.discretize, &pattr);
				VariantToDouble_(pattr, &dbl_value);
				val = unitConversion->ConvertUnit(dbl_value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KG << 0x20) | STORAGE_FORMAT_M2, unitConversion->MassAreaDisplay());
				break;
			}
			if ((se->m_operation & 0x0fffffff) >> 16 == CWFGM_FIRE_STAT_AREA_CHANGE && val < 0)
			{
				std::string fmt;
				fmt = "0(";
				fmt += se->m_attributeFormat();
				fmt += ")";
				if (fabs(val) <= 0.001)
					val = 0;
				strText = strprintf(fmt.c_str(), se->m_resolution, val);
			}
			else
			{
				if (na && (val == -9999.0))
				{
					strText = "N/A";
				}
				else
				{
					strText = strprintf(se->m_attributeFormat().c_str(), se->m_resolution, val);
				}
			}
			break;
		}

		fprintf(f, "%s", strText.c_str());

		se = se->LN_Succ();
		if (se->LN_Succ())
			fprintf(f, ", ");
		else
			fprintf(f, "\n");
	}
}


static void doStats(Project::CWFGMProject* project, const ICWFGM_FWI* comfwi, const Project::StatsOutputs& stats, const Project::stats::StatsEntry& entry,
	Project::Scenario* s, ::UnitConversion* unitConversion, Project::CWFGMStatsEntryCollection& collection, StatsBuilder* builder,
	const WTime& start_time, const WTime& end_time, const WTime& time, std::int64_t timestep)
{
	std::vector<StatsBuilder::StatsType> list;
	list.push_back(timestep);
	bool na = true;
	Project::WeatherStream* wstream = nullptr;
	if (entry.use_stream)
	{
		if (entry.streamName.length() > 0)
		{
			std::uint32_t count = s->GetStreamCount();
			for (int i = 0; i < count; i++)
			{
				Project::WeatherStream* str = s->StreamAtIndex(i);
				std::string name = str->m_name;
				if (boost::iequals(name, entry.streamName))
				{
					wstream = str;
					break;
				}
			}
		}
		else
			wstream = s->StreamAtIndex(entry.stream_idx);
		if (wstream)
		{
			WTime start((ULONGLONG)0, time.GetTimeManager());
			WTimeSpan duration;
			wstream->GetValidTimeRange(start, duration);
			na = ((time < start) || ((start + duration) < time));
		}
	}

	Project::BasicScenarioOptions bso(s->m_scenario.get());
	double temp = 0, rh = 0, precip = 0, ws = 0, wa = 0, ffmc = 0, dc = 0,
		dmc = 0, bui = 0, isi = 0, fwi = 0, tffmc = 0, sisi = 0, sfwi = 0, dew = 0, d_fwi = 0, d_isi = 0;
	bool weatherValid = false, fwiValid = false, buiValid = false, tmpValid = false;
	double dailyFFMC;

	if (entry.use_stream)
	{
		if ((!na) && (wstream))
		{
			IWXData wx;
			IFWIData ifwi;
			DFWIData dfwi;
			if (wstream->GetInstantaneousValues(time, 0, NULL, &ifwi, NULL))
			{
				ffmc = ifwi.FFMC;
				isi = ifwi.ISI;
				fwi = ifwi.FWI;
				fwiValid = true;
			}

			if (wstream->GetInstantaneousValues(time, (bso.m_bWeatherTemporalInterpolate) ? CWFGM_GETWEATHER_INTERPOLATE_TEMPORAL : 0, &wx, &ifwi, &dfwi))
			{
				temp = wx.Temperature;
				rh = wx.RH;
				precip = wx.Precipitation;
				ws = wx.WindSpeed;
				wa = wx.WindDirection;
				dew = wx.DewPointTemperature;
				tffmc = ifwi.FFMC;
				dc = dfwi.dDC;
				dmc = dfwi.dDMC;
				bui = dfwi.dBUI;
				d_fwi = dfwi.dFWI;
				d_isi = dfwi.dISI;
				sisi = ifwi.ISI;
				sfwi = ifwi.FWI;

				dailyFFMC = dfwi.dFFMC;

				weatherValid = true;
			}
		}
	}

	Project::CWFGMStatsEntry* se = collection.FirstStat();
	PolymorphicAttribute pattr;
	WTimeSpan ts;
	std::uint64_t ull_value;
	double dbl_value, val;
	std::int32_t ii_value;

	while (se->LN_Succ())
	{
		WTime t(time);
		switch (se->m_operation & 0x0000ffff)
		{
		case Project::CWFGMStatsEntry::OPERATION_STATS_FBP:
			if (!s)
				break;

			switch ((se->m_operation & 0x0fffffff) >> 16)
			{
			case 1:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 0.0, 10.0, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			case 2:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 10.0, 500.0, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			case 3:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 500.0, 2000.0, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			case 4:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 2000.0, 4000.0, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			case 5:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 4000.0, 10000.0, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			case 6:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_FI, 10000.0, 1.0e50, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			case 7:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, 0.0, 1.5, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			case 8:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, 1.5, 4.5, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			case 9:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, 4.5, 8.5, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			case 10:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, 8.5, 14.5, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			case 11:
				if (SUCCEEDED(s->GetStatsPercentage((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, 14.5, 1.0e50, &val)))
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
				break;
			}
			break;

		case Project::CWFGMStatsEntry::OPERATION_DATETIME:
			list.push_back(time.ToString(WTIME_FORMAT_STRING_ISO8601));
			break;

		case Project::CWFGMStatsEntry::OPERATION_TIMESPAN:
			switch ((se->m_operation & 0x0fffffff) >> 16)
			{
			case 1:
				ts = time - start_time;
				tmpValid = true;
				break;
			case 2:
				ts = end_time - time;
				tmpValid = true;
				break;
			case 3:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TIMESTEP_DURATION_SECS, stats.discretize, &pattr)))
				{
					VariantToTimeSpan_(pattr, &ts);
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case CWFGM_FIRE_STAT_TIMESTEP_REALTIME:
			{
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TIMESTEP_REALTIME, stats.discretize, &pattr)))
				{
				    VariantToUInt64_(pattr, &ull_value);
					ts = WTimeSpan(ull_value, false);
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			}
			case CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_REALTIME:
			{
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_REALTIME, stats.discretize, &pattr)))
				{
				    VariantToDouble_(pattr, &dbl_value);
					ts = WTimeSpan(gsl::narrow_cast<std::int64_t>(dbl_value), false);
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			}
			case CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_BURNING_SECS:
			{
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TIMESTEP_CUMULATIVE_BURNING_SECS, stats.discretize, &pattr)))
				{
					VariantToTimeSpan_(pattr, &ts);
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			}
			}
			if (tmpValid)
				list.push_back(ts.ToString(WTIME_FORMAT_STRING_ISO8601));
			else
				list.push_back(std::monostate());
			break;

		case Project::CWFGMStatsEntry::OPERATION_STEPCOLOR:
			ts = time - start_time;
			tmpValid = true;
			break;

		case Project::CWFGMStatsEntry::OPERATION_INT:
			if (SUCCEEDED(s->GetStats((ULONG)-1, t, ((se->m_operation & 0x0fffffff) >> 16), stats.discretize, &pattr)) &&
			    SUCCEEDED(VariantToInt32_(pattr, &ii_value)))
			{
				list.push_back(gsl::narrow_cast<std::int64_t>(ii_value));
				tmpValid = true;
			}
			else
				tmpValid = false;
			break;

		case Project::CWFGMStatsEntry::OPERATION_ULONGLONG:
			if (SUCCEEDED(s->GetStats((ULONG)-1, t, ((se->m_operation & 0x0fffffff) >> 16), stats.discretize, &pattr)) &&
				SUCCEEDED(VariantToUInt64_(pattr, &ull_value)))
			{
				list.push_back(gsl::narrow_cast<std::int64_t>(ull_value));
				tmpValid = true;
			}
			else
				tmpValid = false;
			break;

		case Project::CWFGMStatsEntry::OPERATION_DOUBLE:
			switch ((se->m_operation & 0x0fffffff) >> 16)
			{
			case 1: /* temperature */
				if (weatherValid)
				{
					val = unitConversion->ConvertUnit(temp, STORAGE_FORMAT_CELSIUS, unitConversion->TempDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 2: /* rh */
				if (weatherValid)
				{
					val = rh * 100.0;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 3: /* wa */
				if (weatherValid)
				{
					val = CARTESIAN_TO_COMPASS_DEGREE(RADIAN_TO_DEGREE(wa));
					if ((val <= 0.0000001) && (ws != 0.0))
						val = 360.0;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 4: /* ws */
				if (weatherValid)
				{
					val = unitConversion->ConvertUnit(ws, STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, unitConversion->VelocityDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 5: /* precip */
				if (weatherValid)
				{
					val = unitConversion->ConvertUnit(precip, STORAGE_FORMAT_MM, unitConversion->SmallMeasureDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 6: /* ffmc */
				if (fwiValid)
				{
					val = ffmc;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 7: /* dc */
				if (weatherValid)
				{
					val = dc;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 8: /* dmc */
				if (weatherValid)
				{
					val = dmc;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 9: /* daily standard ffmc */
				if (weatherValid)
				{
					val = dailyFFMC;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 10:	// daily bui
				if (weatherValid)
				{
					val = bui;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 11:	// hourly isi
				if (fwiValid)
				{
					val = isi;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 12:	// hourly fwi
				if (fwiValid)
				{
					val = fwi;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 13:
				if (weatherValid)
				{
					val = tffmc;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 14:
				if (weatherValid)
				{
					val = sisi;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 15:
				if (weatherValid)
				{
					val = sfwi;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 16:
				if (weatherValid)
				{
					val = d_isi;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 17:
				if (weatherValid)
				{
					val = d_fwi;
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			case 18:
				if (weatherValid)
				{
					val = unitConversion->ConvertUnit(dew, STORAGE_FORMAT_CELSIUS, unitConversion->TempDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case CWFGM_FIRE_STAT_AREA_CHANGE:
			case CWFGM_FIRE_STAT_BURNED_CHANGE:
			case CWFGM_FIRE_STAT_BURNED:
			case CWFGM_FIRE_STAT_AREA:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, (se->m_operation & 0x0fffffff) >> 16, stats.discretize, &pattr)))
				{
				    VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M2, unitConversion->AreaDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case CWFGM_FIRE_STAT_TOTAL_FUEL_CONSUMED:
			case CWFGM_FIRE_STAT_CROWN_FUEL_CONSUMED:
			case CWFGM_FIRE_STAT_SURFACE_FUEL_CONSUMED:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, (se->m_operation & 0x0fffffff) >> 16, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_KG, unitConversion->MassDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case CWFGM_FIRE_STAT_RADIATIVE_POWER:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, (se->m_operation & 0x0fffffff) >> 16, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_KILOJOULE | STORAGE_FORMAT_SECOND, unitConversion->PowerDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case CWFGM_FIRE_STAT_AREA_GROWTH:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_AREA_GROWTH, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M2 | STORAGE_FORMAT_MINUTE, unitConversion->AreaDisplay() | STORAGE_FORMAT_HOUR);
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case CWFGM_FIRE_STAT_ACTIVE_PERIMETER:
			case CWFGM_FIRE_STAT_TOTAL_PERIMETER:
			case CWFGM_FIRE_STAT_EXTERIOR_PERIMETER:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, (se->m_operation & 0x0fffffff) >> 16, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M, unitConversion->AltDistanceDisplay()); 	// RWB: turned this back on 070417
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case CWFGM_FIRE_STAT_ACTIVE_PERIMETER_GROWTH:
			case CWFGM_FIRE_STAT_TOTAL_PERIMETER_GROWTH:
			case CWFGM_FIRE_STAT_EXTERIOR_PERIMETER_GROWTH:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, (se->m_operation & 0x0fffffff) >> 16, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M | STORAGE_FORMAT_MINUTE, unitConversion->AltDistanceDisplay() | STORAGE_FORMAT_HOUR);
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case 40:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_ROS, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M | STORAGE_FORMAT_MINUTE, unitConversion->AltVelocityDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case 41:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_FI, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KILOWATT_SECOND << 0x20) | STORAGE_FORMAT_M, unitConversion->IntensityDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case 46:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_FLAMELENGTH, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, STORAGE_FORMAT_M, unitConversion->SmallDistanceDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case 42:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_CFB, stats.discretize, &pattr)))
				{
				    VariantToDouble_(pattr, &val);
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case 43:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_CFC, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KG << 0x20) | STORAGE_FORMAT_M2, unitConversion->MassAreaDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case 44:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_SFC, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KG << 0x20) | STORAGE_FORMAT_M2, unitConversion->MassAreaDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;

			case 45:
				if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TFC, stats.discretize, &pattr)))
				{
					VariantToDouble_(pattr, &dbl_value);
					val = unitConversion->ConvertUnit(dbl_value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KG << 0x20) | STORAGE_FORMAT_M2, unitConversion->MassAreaDisplay());
					tmpValid = true;
				}
				else
					tmpValid = false;
				break;
			}
			if ((se->m_operation & 0x0fffffff) >> 16 == CWFGM_FIRE_STAT_AREA_CHANGE && val < 0)
			{
				if (fabs(val) <= 0.001)
					val = 0;
				std::string text = (boost::format("0(%lf)") % ROUND_DECIMAL(val, se->m_resolution)).str();
				list.push_back(text);
			}
			else
			{
				if (tmpValid)
					list.push_back(ROUND_DECIMAL(val, se->m_resolution));
				else
					list.push_back(std::monostate());
			}
			break;
		}

		se = se->LN_Succ();
	}

	builder->AddRow(list);
}


HRESULT exportStatsCSV(UnitConversion& unitConversion, Project::CWFGMProject *project, const ICWFGM_FWI* fwi,
	const Project::StatsOutputs& stats, Project::Scenario* s, SimMessage* sm)
{
	Project::CWFGMStatsEntryCollection collection(&unitConversion);
	ExportOptions eo(NULL);
	Project::CWFGMStatsEntry* se;
	//just quit if there is nothing to export
	if (!stats.entries.size())
		return S_OK;
	const Project::stats::StatsEntry& entry = stats.entries[0];

	for (auto i = 0; i < entry.columns.size(); i++)
	{
		if (entry.columns[i] >= DEFAULT_ENTRIES_NUM_STATS)
			return E_INVALIDARG;
		const viewentry* ve = &default_entries[entry.columns[i]];
		Project::CWFGMStatsEntry* se = collection.New();
		se->Initialize(ve);
		collection.AddStat(se);
		collection.MakeDisplayable(se, !ve->defaultHidden);
	}

    try
    {
        fs::create_directories(fs::path(stats.filename).parent_path());
    }
    catch (fs::filesystem_error& err)
    {
        return ERROR_NOACCESS | ERROR_SEVERITY_WARNING;
    }

	FILE* f;
	f = fopen(stats.filename.c_str(), "w");
	if (!f)
		return ERROR_NOACCESS | ERROR_SEVERITY_WARNING;

	se = collection.FirstStat();
	while (se->LN_Succ())
	{
		std::string attributeName(se->m_attributeName());
		boost::replace_all(attributeName, "\n"s, " "s);
		boost::replace_all(attributeName, "\r"s, ""s);
		fprintf(f, "\"%s\"", attributeName.c_str());
		se = se->LN_Succ();
		if (se->LN_Succ())
			fprintf(f, ", ");
		else
			fprintf(f, "\n");
	}

	std::uint32_t i1, steps = s->GetNumberSteps();

	WTime	st = s->Simulation_StartTime(),
			et = s->Simulation_EndTime();

	if (steps > 0)
	{
		WTime zero(0ULL, project->m_timeManager);
		std::vector<HSS_Time::WTime> times(steps, zero);

		s->GetStepsArray(&steps, &times);	//get the steps
		for (i1 = 0; i1 < steps; i1++)
		{
			doStats(project, fwi, stats, entry, s, &unitConversion, collection, f, st, et, times[i1]);
		}
	}

	fclose(f);
	return S_OK;
}


HRESULT exportStatsJson(UnitConversion& unitConversion, Project::CWFGMProject* project, const ICWFGM_FWI* fwi,
	const Project::StatsOutputs& stats, Project::Scenario* s, SimMessage* sm)
{
	//for building the stats file
	StatsFileBuilder builder(s->m_name);
	for (auto& entry : stats.entries)
	{
		StatsBuilder* groupBuilder;
		if (entry.use_stream)
		{
			if (entry.streamName.length() > 0)
				groupBuilder = builder.NewStatsBuilder(entry.streamName);
			else
				groupBuilder = builder.NewStatsBuilder(entry.stream_idx);
		}
		else
			groupBuilder = builder.NewStatsBuilder();

		Project::CWFGMStatsEntryCollection collection(&unitConversion);
		ExportOptions eo(NULL);
		Project::CWFGMStatsEntry* se;

		groupBuilder->AddColumn("timestep"s, "Current Simulation Timestep"s);

		for (auto i = 0; i < entry.columns.size(); i++)
		{
			//this is an invalid statistic
			if (entry.columns[i] >= DEFAULT_ENTRIES_NUM_STATS)
				return E_INVALIDARG;
			//create the entry that will be used to generate the rows
			const viewentry* ve = &default_entries[entry.columns[i]];
			se = collection.New();
			se->Initialize(ve);
			collection.AddStat(se);
			//create a header for the statistic in the output file
			std::string units = getUnits(&unitConversion, se->m_operation);
			std::string description;
			if (se->m_attributeTipText().length())
				description = se->m_attributeTipText();
			else
				description = se->m_attributeShortText();
			std::string name = default_entry_ids[entry.columns[i]];
			groupBuilder->AddColumn(name, description, units);
		}

		std::uint32_t i1, steps = s->GetNumberSteps();

		WTime	st = s->Simulation_StartTime(),
			et = s->Simulation_EndTime();

		if (steps > 0)
		{
			WTime zero(0ULL, project->m_timeManager);
			std::vector<HSS_Time::WTime> times(steps, zero);

			s->GetStepsArray(&steps, &times);	//get the steps
			for (i1 = 0; i1 < steps; i1++)
				doStats(project, fwi, stats, entry, s, &unitConversion, collection, groupBuilder, st, et, times[i1], i1);
		}
	}

	if (stats.fileType == Project::stats::StatsFileType::JSON_COLUMN)
		builder.SaveJSON(stats.filename, StatsFileFormat::JSON_COLUMN, true);
	else
		builder.SaveJSON(stats.filename, StatsFileFormat::JSON_ROW, true);

	return S_OK;
}


HRESULT exportStats(UnitConversion& unitConversion, Project::CWFGMProject* project, const ICWFGM_FWI* fwi,
	const Project::StatsOutputs& stats, Project::Scenario* s, SimMessage* sm)
{
	if (stats.fileType == Project::stats::StatsFileType::JSON_COLUMN ||
		stats.fileType == Project::stats::StatsFileType::JSON_ROW)
		return exportStatsJson(unitConversion, project, fwi, stats, s, sm);
	else
		return exportStatsCSV(unitConversion, project, fwi, stats, s, sm);
}


HRESULT exportAssetStats(UnitConversion& unitConversion, Project::CWFGMProject* project, SPARCS_P *sp, const ICWFGM_FWI* fwi,
	Project::AssetOutput& stats, Project::Scenario* s, SimMessage* sm, bool write)
{
	//for building the stats file
	bool oneRun = stats.m_statBuilder == nullptr && write;
	Project::AssetStatsCollection* builder = stats.m_statBuilder;
	if (builder == nullptr)
	{
		builder = new Project::AssetStatsCollection();
		if (!oneRun)
			stats.m_statBuilder = builder;

		for (ULONG i = 0; i < s->GetAssetCount(); i++)
		{
			auto asset = s->AssetAtIndex(i);
			auto assetName = std::string(asset->Name());
			ULONG geometryCount = asset->GetPolyCount();

			for (ULONG j = 0; j < geometryCount; j++)
			{
				Project::AssetDescription desc;
				desc.assetName = assetName;
				desc.assetGeometryId = j;
				std::uint32_t attributeCount;
				asset->m_filter->GetPolyLineAttributeCount(&attributeCount);
				for (ULONG k = 0; k < attributeCount; k++)
				{
					PolymorphicAttribute value;
					std::string cstr;
					asset->m_filter->GetPolyLineAttributeName(k, &cstr);
					if (boost::iequals(cstr, "asset_name"s))
						cstr = "c_asset_name";
					else if (boost::iequals(cstr, "geometry_id"s))
						cstr = "c_geometry_id";
					if (SUCCEEDED(asset->m_filter->GetPolyLineAttributeValue(j, cstr, &value)))
					{
						if (std::holds_alternative<std::int32_t>(value))
							desc.attributes[cstr] = std::int64_t{ std::get<std::int32_t>(value) };
						else if (std::holds_alternative<std::int64_t>(value))
							desc.attributes[cstr] = std::get<std::int64_t>(value);
						else if (std::holds_alternative<double>(value))
							desc.attributes[cstr] = std::get<double>(value);
						else if (std::holds_alternative<std::string>(value))
							desc.attributes[cstr] = std::get<std::string>(value);
					}
				}
				builder->assetDescriptions.push_back(std::move(desc));
			}
		}
	}

	//create a new section in the file
	std::string scenarioName;
	if (s->m_parent && s->m_subName.length())
		scenarioName = s->m_subName;
	else
		scenarioName = s->m_name;
	Project::AssetScenarioStats& scenarioStats = builder->timestepStats[scenarioName];
	scenarioStats.scenarioName = scenarioName;

	auto stream = s->GetPrimaryStream();
	if (stream)
		scenarioStats.stationName = stream->m_weatherStation()->m_name;

	std::uint32_t i1, steps = s->GetNumberSteps();

	WTime	st = s->Simulation_StartTime();

	WTime arrivalStart(st);

	//find the ignition start time
	if (s->GetFireCount())
	{
		auto fire = s->FireAtIndex(0);
		WTime tm(0ULL, project->m_timeManager);
		fire->m_fire->GetIgnitionTime(&tm);

		//handle ignition override times
		PolymorphicAttribute v;
		if (SUCCEEDED(s->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITION_START_TIME_OVERRIDE, &v)))
		{
			WTime oTime(arrivalStart);
		    VariantToTime_(v, &oTime);
			if (oTime.GetTotalMicroSeconds() > 0)
				tm = oTime;
		}

		if (tm < arrivalStart)
		{
			Project::CBurnPeriodOption option(project->m_timeManager);
			if (SUCCEEDED(s->GetDayBurnCondition(st, &option)))
			{
				if (option.eff_Start)
				{
					WTime temp(st);
					temp.PurgeToDay(WTIME_FORMAT_WITHDST | WTIME_FORMAT_AS_LOCAL);
					temp += option.m_Start;

					if (arrivalStart < temp)
						arrivalStart = temp;
				}
			}
		}
		else
			arrivalStart = tm;
	}

	if (steps > 0)
	{
		WTime zero(0ULL, project->m_timeManager);
		std::vector<HSS_Time::WTime> times(steps, zero);

		s->GetStepsArray(&steps, &times);	//get the steps
		PolymorphicAttribute v;
		for (i1 = 0; i1 < steps; i1++)
		{
			WTime t(times[i1]);
			WTimeSpan timespan = t - arrivalStart;
			WTimeSpan tempTs;
			//don't allow negative arrival times, assume 0
			if (timespan.GetTotalSeconds() < 0)
				timespan = WTimeSpan(0);
			Project::AssetTimestepStats timestepStats(t);
			timestepStats.timestep = i1;
			timestepStats.elapsedTime = timespan;
			if (SUCCEEDED(s->GetStats((ULONG)-1, t, CWFGM_FIRE_STAT_TIMESTEP_DURATION_SECS, 10, &v)) &&
					SUCCEEDED(VariantToTimeSpan_(v, &tempTs)))
				timestepStats.timestepDuration = tempTs;

			for (ULONG i = 0; i < s->GetAssetCount(); i++)
			{
				auto asset = s->AssetAtIndex(i);
				auto assetName = asset->Name();
				std::uint32_t geometryCount;
				s->m_scenario->GetAssetTimeCount(asset->m_filter.get(), &geometryCount);
				if (geometryCount > 0)
				{
					for (std::uint32_t j = 0; j < geometryCount; j++)
					{
						bool arrived = 0;
						WTime arriveTime(0, project->m_timeManager);
						s->m_scenario->GetAssetTime(asset->m_filter.get(), j, &arrived, &arriveTime);
						//the fire arrived at this asset in this timestep
						if (arrived && arriveTime == t)
						{
							Project::AssetArrivalDetails details;
							details.assetName = assetName;
							details.geographyIndex = j;
							timestepStats.assetArrivals.push_back(std::move(details));

							Project::AssetGlobalStats& assetStats = builder->assetStats[std::make_tuple(assetName, j)];
							assetStats.assetName = assetName;
							assetStats.assetGeometryId = j;

							AssetArrival aa;
							aa.arrivalTime = timespan;
							if (stats.criticalPath)
								s->BuildCriticalPath(asset, j, 1, *aa.criticalPath);
							AssetArrival aa2(aa);

							assetStats.arrivalTimes.push_back(aa);
							assetStats.arrivalCount++;

							builder->globalStats.arrivalTimes.push_back(aa2);
							builder->globalStats.arrivalCount++;

							if (stats.criticalPathFilename.length()) {
								auto [driver_name, projection_name] = sp->guessDriverNameFromFilename(stats.criticalPathFilename.c_str());
								s->ExportCriticalPath(asset, j, 0, driver_name, projection_name, stats.criticalPathFilename.c_str());
							}
						}
					}
				}
			}
			scenarioStats.timestepStats.push_back(std::move(timestepStats));
		}
	}

	if (write)
	{
		//add ignition details
		Project::IgnitionDescription ignitionDesc;
		if (s->GetFireCount())
		{
			auto fire = s->FireAtIndex(0);
			std::uint32_t size = 0;
			std::uint16_t type;
			ignitionDesc.ignitionType = fire->IgnitionType(0);
			auto ignition = fire->GetIgnition(0, &type, &size);
			if (ignition)
			{
				ignitionDesc.points = ignition;
				ignitionDesc.count = size;
			}
		}

		builder->save(stats.filename, project, ignitionDesc);
		if (ignitionDesc.points)
			delete[] ignitionDesc.points;
	}

	if (oneRun)
		delete builder;

	return S_OK;
}
