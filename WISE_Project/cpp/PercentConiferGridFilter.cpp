/**
 * WISE_Project: PercentConiferGridFilter.cpp
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


Project::PercentGridFilter::PercentGridFilter()
	: AttributeGridFilter()
{
	AttributeType(CCWFGM_AttributeFilter::VT_I1);
}


Project::PercentConiferGridFilter::PercentConiferGridFilter() : PercentGridFilter() { }


Project::PercentCureGrassGridFilter::PercentCureGrassGridFilter() : PercentGridFilter() { }


Project::PercentDeadFirGridFilter::PercentDeadFirGridFilter() : PercentGridFilter() { }


void Project::PercentConiferGridFilter::Reset()
{
	NumericVariant v(gsl::narrow_cast<std::int8_t>(0));
	ResetAttribute(v);
	Key(FUELCOM_ATTRIBUTE_PC);
}


void Project::PercentCureGrassGridFilter::Reset()
{
	NumericVariant v(gsl::narrow_cast<std::int8_t>(0));
	ResetAttribute(v);
	Key(FUELCOM_ATTRIBUTE_CURINGDEGREE);
}


void Project::PercentDeadFirGridFilter::Reset()
{
	NumericVariant v(gsl::narrow_cast<std::int8_t>(0));
	ResetAttribute(v);
	Key(FUELCOM_ATTRIBUTE_PDF);
}


std::string Project::PercentConiferGridFilter::DisplayName() const
{
	std::string name;
	if (m_name.length())
		name = strprintf(_T("Percent Conifer: %s"), m_name.c_str());
	else
		name = _T("Percent Conifer: [No Name]");
	return name;
}


std::string Project::PercentCureGrassGridFilter::DisplayName() const
{
	std::string name;
	if (m_name.length())
		name = strprintf(_T("Degree of curing (%%): %s"), m_name.c_str());
	else
		name = _T("Degree of curing (%): [No Name]");
	return name;
}


std::string Project::PercentDeadFirGridFilter::DisplayName() const
{
	std::string name;
	if (m_name.length())
		name = strprintf(_T("Percent Dead Fir: %s"), m_name.c_str());
	else
		name = _T("Percent Dead Fir: [No Name]");
	return name;
}


HRESULT Project::PercentGridFilter::Import(const CWFGMProject *project, const std::string& prj_filename, const std::string& name)
{
	HRESULT hr1, hr = AttributeGridFilter::Import(project, prj_filename, name);
	if (SUCCEEDED(hr))
	{
		USHORT x, y, x_max, y_max;
		if (FAILED(hr1 = m_filter->GetDimensions(0, &x_max, &y_max)))
			return hr1;
		for (x = 0; x < x_max; x++)
			for (y = 0; y < y_max; y++)
			{
				XY_Point pt;
				pt.x = project->invertX((static_cast<double>(x)) + 0.5);
				pt.y = project->invertY((static_cast<double>(y)) + 0.5);
				NumericVariant var = GetAttribute(pt);
				if (!std::holds_alternative<std::monostate>(var)) {
					SHORT val;
					variantToInt16(var, &val);
					if ((val < 0) || (val > 100))
						return E_UNEXPECTED;
				}
			}
	}
	return hr;
}


auto Project::PercentGridFilter::Duplicate() const -> PercentGridFilter*
{
	return Duplicate([] { return new PercentGridFilter(); });
}


auto Project::PercentGridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> PercentGridFilter*
{
	return dynamic_cast<PercentGridFilter*>(AttributeGridFilter::Duplicate(constructor));
}


auto Project::PercentConiferGridFilter::Duplicate() const -> PercentConiferGridFilter*
{
	return Duplicate([] { return new PercentConiferGridFilter(); });
}


auto Project::PercentConiferGridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> PercentConiferGridFilter*
{
	return dynamic_cast<PercentConiferGridFilter*>(PercentGridFilter::Duplicate(constructor));
}


auto Project::PercentCureGrassGridFilter::Duplicate() const -> PercentCureGrassGridFilter*
{
	return Duplicate([] { return new PercentCureGrassGridFilter(); });
}


auto Project::PercentCureGrassGridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> PercentCureGrassGridFilter*
{
	return dynamic_cast<PercentCureGrassGridFilter*>(PercentGridFilter::Duplicate(constructor));
}


auto Project::PercentDeadFirGridFilter::Duplicate() const -> PercentDeadFirGridFilter*
{
	return Duplicate([] { return new PercentDeadFirGridFilter(); });
}


auto Project::PercentDeadFirGridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> PercentDeadFirGridFilter*
{
	return dynamic_cast<PercentDeadFirGridFilter*>(PercentGridFilter::Duplicate(constructor));
}
