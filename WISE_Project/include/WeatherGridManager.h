/**
 * WISE_Project: WeatherGridManager.h
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

#if !defined(AFX_WEATHERGRIDMANAGER_H__F6FF3756_3BFF_42FA_AC96_CA773D6F0A5C__INCLUDED_)
#define AFX_WEATHERGRIDMANAGER_H__F6FF3756_3BFF_42FA_AC96_CA773D6F0A5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>


namespace Project
{
	class CWeatherPolyFilter;

	class CWeatherGridManager
	{
		CWFGMProject * m_pProject;
		ULONG AddGridFilter(CWeatherPolyFilter *pFilter);

	public:
		std::vector<CWeatherPolyFilter *> m_arWeatherGrid;
		CWeatherGridManager(CWFGMProject *project);
		virtual ~CWeatherGridManager();

		ULONG GetSize();

		CWeatherPolyFilter *FindName(const TCHAR *name) const;
	};
};

#endif // !defined(AFX_WEATHERGRIDMANAGER_H__F6FF3756_3BFF_42FA_AC96_CA773D6F0A5C__INCLUDED_)
