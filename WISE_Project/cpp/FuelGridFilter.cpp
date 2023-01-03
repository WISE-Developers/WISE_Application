/**
 * WISE_Project: FuelGridFilter.cpp
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
#include "FuelGridFilter.h"
#include "str_printf.h"
#include "CWFGM_AttributeFilter.h"



Project::FuelGridFilter::FuelGridFilter()
{
	AttributeType(CCWFGM_AttributeFilter::VT_UI1);
}


void Project::FuelGridFilter::Reset()
{
	NumericVariant v(gsl::narrow_cast<std::uint8_t>(0));
	ResetAttribute(v);
	Key((USHORT)-1);
}


void Project::FuelGridFilter::SetFuelMap(CCWFGM_FuelMap *fuelMap)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	gf->put_FuelMap(fuelMap);
}


std::string Project::FuelGridFilter::DisplayName() const
{
	std::string name;
	if (m_name.length())
		name = strprintf(_T("Fuel: %s"), m_name.c_str());
	else
		name = _T("Fuel: [No Name]");
	return name;
}
