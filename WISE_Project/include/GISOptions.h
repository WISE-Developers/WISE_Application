/**
 * WISE_Project: GISOptions.h
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

#ifndef __GIS_OPTIONS_H
#define __GIS_OPTIONS_H

#include "cwfgmp_config.h"
#include "CWFGM_AttributeFilter.h"
#include "CWFGM_Grid.h"
#include "CWFGM_Fire.h"
#include "CWFGM_PolyReplaceGridFilter.h"
#include "CWFGM_WeatherGridFilter.h"
#include "ICWFGM_VectorEngine.h"
#include "WTime.h"

using namespace HSS_Time;


namespace Project
{
	class CWFGMPROJECT_EXPORT GISOptions
	{
	public:
		double	m_initialSize;			// initial buffer around the ignition(s) to form a bounding box
		double	m_growSize;				// how much the data should grow at a time, in any direction
		double	m_reactionSize;			// minimum buffer size between the bounding box and the containing fire(s) before it grows
		bool	m_allowGrow;			// whether the grid is allowed to grow or not

	public:
		GISOptions();

#ifdef _MSC_VER
		GISOptions(const TCHAR *group_name);
#endif

		GISOptions(const GISOptions *toCopy);
		GISOptions(CCWFGM_Grid *grid);
		GISOptions(CCWFGM_AttributeFilter *filter);
		GISOptions(CCWFGM_PolyReplaceGridFilter *filter);
		GISOptions(CCWFGM_WeatherGridFilter *filter);
		GISOptions(ICWFGM_VectorEngine *vector);
		GISOptions(CCWFGM_Ignition *ignition);

#ifdef _MSC_VER
		void SaveToIniFile(const TCHAR *group_name) const;
#endif

		bool SaveToGrid(CCWFGM_Grid *grid) const;
		bool SaveToFilter(CCWFGM_AttributeFilter *filter) const;
		bool SaveToFilter(CCWFGM_PolyReplaceGridFilter *filter) const;
		bool SaveToFilter(CCWFGM_WeatherGridFilter *filter) const;
		bool SaveToVector(ICWFGM_VectorEngine *vector) const;
		bool SaveToIgnition(CCWFGM_Ignition *ignition) const;

		// CObject overrides
		const GISOptions &operator=(const GISOptions &toCopy);
	};
};

#endif
