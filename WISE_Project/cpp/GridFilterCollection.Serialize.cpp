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

#if _DLL
#include "StdAfx.h"
#endif
#include "ScenarioCollection.h"
#include "WeatherGridFilter.h"
#include "WindDirectionGrid.h"
#include "WindSpeedGrid.h"
#include "CWFGMProject.h"

using namespace std::string_literals;


std::string Project::GridFilterCollection::CollectLoadWarnings()
{
	std::string warning = m_loadWarning;
	GridFilter *f = FirstFilter();
	while (f->LN_Succ())
	{
		warning += f->m_loadWarning;
		PolymorphicAttribute v;
		if (SUCCEEDED(f->m_filter->GetAttribute(0, CWFGM_ATTRIBUTE_LOAD_WARNING, &v)))
		{
			std::string w = GetPolymorphicAttributeData(v, ""s);

			if (w.length())
				warning += w;
		}
		f = f->LN_Succ();
	}
	return warning;
}
