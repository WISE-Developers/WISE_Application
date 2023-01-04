/**
 * WISE_Project: BasicScenarioOptions.h
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

#ifndef __BASIC_SCENARIO_OPTIONS_H
#define __BASIC_SCENARIO_OPTIONS_H

#include "cwfgmp_config.h"
#include "CWFGM_Scenario.h"
#include "CWFGM_TemporalAttributeFilter.h"
#include "WTime.h"
#include "linklist.h"

using namespace HSS_Time;


namespace Project
{
	class CWFGMPROJECT_EXPORT BasicScenarioOptions {
	public:
		WTimeSpan	m_displayInterval,		// how often the output is to be stopped and re-displayed
					m_temporalThreshold_Accel;	// same as m_temporalThreshold but during ignition acceleration (for points)

		double		m_perimeterResolution;		// maximum distance between 2 vertices on a fire perimeter
		double		m_perimeterSpacing;		// minimum distance between 2 vertices on a fire perimeter
		double		m_spatialThreshold;		// maximum distance a vertex can travel in one time step

		bool		m_bBoundaryStop;
		bool		m_bSpotting;
		bool		m_bBreaching;
		bool		m_bWeatherTemporalInterpolate,
					m_bWeatherSpatialInterpolate,
					m_bPrecipitationInterpolation,
					m_bWindInterpolation,
					m_bWindInterpolationVector,
					m_bTempRHInterpolation,
					m_bCalcSpatialFWI,		// if FALSE, then we just apply IDW to FWI like we do the WX inputs
					m_bHistoryInterpolation,
					m_bDynamicSpatialThreshold;
		bool		m_bPurgeNonDisplayable;
		bool		m_bSuppressConcaveTightFill;
		bool		m_bCardinalDirection;
		bool		m_bIndependentFires;

	public:
		BasicScenarioOptions();
		BasicScenarioOptions(const TCHAR *group_name);
		BasicScenarioOptions(const BasicScenarioOptions &toCopy);
		BasicScenarioOptions(CCWFGM_Scenario *scenario);

		void SaveToIniFile(const TCHAR *group_name) const;
		bool SaveToScenario(CCWFGM_Scenario *scenario) const;

		const BasicScenarioOptions &operator=(const BasicScenarioOptions &toCopy);
	};


	class CWFGMPROJECT_EXPORT CuringNode : public MinNode {
	public:
		CuringNode *LN_Succ() const { return (CuringNode *)MinNode::LN_Succ(); };
		CuringNode *LN_Pred() const { return (CuringNode *)MinNode::LN_Pred(); };

		WTimeSpan m_date;
		double m_curing;

		CuringNode() = default;
		CuringNode(const CuringNode &cn)				{ *this = cn; }
		CuringNode &operator=(const CuringNode &toCopy) { m_date = toCopy.m_date; m_curing = toCopy.m_curing; return *this; }
	};


	class CWFGMPROJECT_EXPORT CuringOptions {
	public:
		MinListTempl<CuringNode> curing;

	public:
		CuringOptions();
		CuringOptions(const TCHAR *szGroup);
		CuringOptions(const CuringOptions &toCopy);
		CuringOptions(CCWFGM_TemporalAttributeFilter *filter);
		~CuringOptions();

		void SaveToIniFile(const TCHAR *group_name) const;
		bool SaveToScenario(CCWFGM_TemporalAttributeFilter *filter) const;

		const CuringOptions &operator=(const CuringOptions &toCopy);

	private:
		void initEmpty();
	};
};

#endif
