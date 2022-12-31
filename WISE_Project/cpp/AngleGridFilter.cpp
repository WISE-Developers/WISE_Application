/**
 * WISE_Project: AngleGridFilter.cpp
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
#include "CWFGM_AttributeFilter.h"
#include "PercentConiferGridFilter.h"
#include "FireEngine_ext.h"
#include "FuelCom_ext.h"
#include "CWFGMProject.h"
#include "str_printf.h"
#include "AngleGridFilter.h"


Project::AngleGridFilter::AngleGridFilter() : AttributeGridFilter() {
	AttributeType(CCWFGM_AttributeFilter::VT_I2);
}


Project::OVDGridFilter::OVDGridFilter() : AngleGridFilter() {
}


void Project::OVDGridFilter::Reset() {
	NumericVariant v = static_cast<std::int16_t>(0);
	ResetAttribute(v);
	Key(CWFGM_SCENARIO_OPTION_GRID_OVD);
}


std::string Project::OVDGridFilter::DisplayName() const {
	std::string name;
	if (m_name.length())
		name = strprintf(_T("Override Vector Direction (OVD): %s"), m_name.c_str());
	else
		name = _T("Override Vector Direction (OVD): [No Name]");
	return name;
}


const std::string Project::OVDGridFilter::TypeString() const {
	return _T("Override Vector Direction (OVD) Grid");
}


HRESULT Project::AngleGridFilter::Import(const class CWFGMProject* project, const std::string& prj_filename, const std::string& name) {
	HRESULT hr1, hr = AttributeGridFilter::Import(project, prj_filename, name);
	const bool neg = acceptNegatives();
	if (SUCCEEDED(hr)) {
		USHORT x, y, x_max, y_max;
		if (FAILED(hr1 = m_filter->GetDimensions(0, &x_max, &y_max)))
			return hr1;
		for (x = 0; x < x_max; x++)
			for (y = 0; y < y_max; y++) {
				{
					XY_Point pt;
					pt.x = project->invertX((static_cast<double>(x)) + 0.5);
					pt.y = project->invertY((static_cast<double>(y)) + 0.5);
					NumericVariant var = GetAttribute(pt);
					if (!std::holds_alternative<std::monostate>(var)) {
						SHORT val;
						variantToInt16(var, &val);
						if (neg) {
							if ((val < -360) || (val > 360))
								return E_UNEXPECTED;
						}
						else {
							if ((val < 0) || (val > 360))
								return E_UNEXPECTED;
						}
					}
				}
			}
	}
	return hr;
}
