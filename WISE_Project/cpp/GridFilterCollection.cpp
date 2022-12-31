/**
 * WISE_Project: GridFilterCollection.cpp
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
#include "stdafx.h"
#endif
#include "ScenarioCollection.h"
#include "WeatherGridFilter.h"
#include "WindDirectionGrid.h"
#include "WindSpeedGrid.h"
#include "CWFGMProject.h"
#include "GreenupGridFilter.h"
#include "ReplaceGridFilter.h"
#include "FuelGridFilter.h"
#include "PercentConiferGridFilter.h"
#include "AngleGridFilter.h"


Project::GridFilterCollection::~GridFilterCollection()
{
	GridFilter *gf;
	while (gf = m_filterList.RemHead())
		delete gf;
}


template<typename T>
std::uint32_t Project::GridFilterCollection::GetCount(bool wxpatch_islandscape) const
{
	if constexpr (std::is_integral<T>())
		return m_filterList.GetCount();
	else
	{
		GridFilter* gf = m_filterList.LH_Head();
		ULONG cnt = 0;
		while (gf->LN_Succ())
		{
			//try to cast the filter to the requested type
			auto test = dynamic_cast<T*>(gf);
			if (test)
			{
				if constexpr (std::is_same<T, CWeatherPolyFilter>())
				{
					CWeatherPolyFilter* wpf = static_cast<CWeatherPolyFilter*>(test);
					if ((wxpatch_islandscape) && (wpf->GetLandscapeWxFlag()))
						cnt++;
					else if ((!wxpatch_islandscape) && (!wpf->GetLandscapeWxFlag()))
						cnt++;
				}
				else
					cnt++;
			}
			gf = gf->LN_Succ();
		}
		return cnt;
	}
}
//this will need done for all classes that this method needs called for
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<std::uint32_t>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::CWindDirectionGrid>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::CWindSpeedGrid>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::GreenupGridFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::CWeatherPolyFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::ReplaceGridFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::FuelGridFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::PercentConiferGridFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::OVDGridFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::PercentDeadFirGridFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::PercentCureGrassGridFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::CBHGridFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::TreeHeightGridFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::FuelLoadGridFilter>(bool) const;
template CWFGMPROJECT_EXPORT std::uint32_t Project::GridFilterCollection::GetCount<Project::PolyReplaceGridFilter>(bool) const;


auto Project::GridFilterCollection::FindName(const char *name) const -> GridFilter*
{
	GridFilter *gf = FirstFilter();
	while (gf->LN_Succ())
	{
		if (gf->m_name == name)
			return gf;
		gf = gf->LN_Succ();
	}
	return nullptr;
}


bool Project::GridFilterCollection::AssignNewGrid(CCWFGM_FuelMap *fuelMap, ICWFGM_GridEngine *grid)
{
	m_fuelMap = fuelMap;
	m_gridEngine = grid;
	GridFilter *gf = FirstFilter();

	ICWFGM_CommonData* data;
	if (FAILED(grid->GetCommonData(nullptr, &data)))
		return false;

	while (gf->LN_Succ())
	{
		gf->SetFuelMap(m_fuelMap.get());
		gf->m_filter->put_LayerManager(m_project->m_layerManager.get());
		if (FAILED(gf->m_filter->PutGridEngine(nullptr, grid)))
			return false;
		gf->m_filter->PutCommonData(nullptr, data);
		gf = gf->LN_Succ();
	}
	return true;
}


void Project::GridFilterCollection::UpdateDeprecatedWeatherFilters()
{
	GridFilter *gf = FirstFilter();
	while (gf->LN_Succ())
	{
		auto cwg = dynamic_cast<CWeatherGridFilter*>(gf);
		if (cwg)
		{
			{
				m_loadWarning += "The weather grid named \"";
				m_loadWarning += gf->m_name;
				m_loadWarning += "\" is an unknown type and has not been converted to a new-style weather grid.  This may represent an error in this FGM.\n";
			}
		}
		gf = gf->LN_Succ();
	}
}
