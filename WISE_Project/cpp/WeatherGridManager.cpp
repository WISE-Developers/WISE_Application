/**
 * WISE_Project: WeatherGridManager.cpp
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

#include "cwfgmp_config.h"
#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

#include "WeatherGridFilter.h"
#include "WeatherGridManager.h"
#include "CWFGMProject.h"
#include <boost/algorithm/string.hpp>


Project::CWeatherGridManager::CWeatherGridManager(CWFGMProject *project)
{
	m_pProject=project;
}


Project::CWeatherGridManager::~CWeatherGridManager()
{
	CWeatherPolyFilter *pFilter;
    for (size_t i = 0; i < m_arWeatherGrid.size(); )
	{
		pFilter=(CWeatherPolyFilter *)(m_arWeatherGrid[0]);
        m_arWeatherGrid.erase(m_arWeatherGrid.begin(), m_arWeatherGrid.begin() + 1);
		delete pFilter;
	}
}


auto Project::CWeatherGridManager::FindName(const TCHAR *name) const -> CWeatherPolyFilter*
{
    for (size_t i = 0; i < m_arWeatherGrid.size(); i++)
	{
		CWeatherPolyFilter *filter = m_arWeatherGrid[i];
		if (boost::iequals(filter->m_name, name))
			return filter;
	}
	return nullptr;
}


ULONG Project::CWeatherGridManager::AddGridFilter(CWeatherPolyFilter *pFilter)
{
    m_arWeatherGrid.push_back(pFilter);
    ULONG index = m_arWeatherGrid.size() - 1;
	return index;
}


ULONG Project::CWeatherGridManager::GetSize()
{
    return (ULONG)m_arWeatherGrid.size();
}
