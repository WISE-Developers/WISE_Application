/**
 * WISE_Project: ScenarioCollection.Serialize.cpp
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
#include <stdlib.h>
#include "ScenarioCollection.h"
#include <assert.h>
#include "CWFGMProject.h"

#include "angles.h"
#include "WeatherGridFilter.h"

using namespace std::string_literals;


std::string Project::ScenarioCollection::CollectLoadWarnings()
{
	std::string warning = m_loadWarning;
	Project::Scenario *f = FirstScenario();
	while (f->LN_Succ())
	{
		warning = warning + f->m_loadWarning;
		PolymorphicAttribute v;
		if (SUCCEEDED(f->m_scenario->GetAttribute(CWFGM_ATTRIBUTE_LOAD_WARNING, &v)))
		{
			std::string w = GetPolymorphicAttributeData(v, ""s);

			if (w.length())
				warning = warning + w;
		}
		f = f->LN_Succ();
	}
	return warning;
}
