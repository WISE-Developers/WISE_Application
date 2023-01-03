/**
 * WISE_Project: FBPOptions.h
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

#ifndef __FBP_OPTIONS_H
#define __FBP_OPTIONS_H

#include "cwfgmp_config.h"
#include "CWFGM_Scenario.h"
#include "FireEngine.h"
#include "WTime.h"

namespace Project
{
	class CWFGMPROJECT_EXPORT FBPOptions
	{
	public:
		double	m_specifiedFMC;		// specified foliar moisture content used for conifers (and parts of mixedwoods)
		double	m_defaultElevation;	// default elevation to use to calculate FMC when no elevation data is available (used if m_specifiedFMC is negative (invalid) )
		double	m_growthPercentile;	// 0 to 100 (or -1 to turn it off) - 50% is a standard growth equation, other values adjust the ROS based on the standard bell curve as calculated by John Braun
		bool	m_topography,		// whether topography is considered during fire growth (on/off)
			m_extinguishment,		// whether fire extinguishment is on or off
			m_wind,					// whether we use the wind or not during fire growth
			m_growthPercentileEnabled,
			m_specifiedFMCActive;
		HSS_Time::WTimeSpan m_greenupStart, m_greenupEnd;
		HSS_Time::WTimeSpan m_standingStart, m_standingEnd;

	public:
		FBPOptions();

		FBPOptions(const TCHAR *group_name);
		FBPOptions(const FBPOptions &toCopy);
		FBPOptions(CCWFGM_Scenario *scenario);

		void SaveToIniFile(const TCHAR *group_name) const;

		bool SaveToScenario(CCWFGM_Scenario *scenario) const;

		const FBPOptions &operator=(const FBPOptions &toCopy);
	};
};

#endif
