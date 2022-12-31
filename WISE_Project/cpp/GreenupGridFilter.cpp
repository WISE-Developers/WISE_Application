/**
 * WISE_Project: GreenupGridFilter.cpp
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
#include "GreenupGridFilter.h"
#include "FireEngine_ext.h"
#include "FuelCom_ext.h"
#include "str_printf.h"


Project::GreenupGridFilter::GreenupGridFilter()
{
	AttributeType(CCWFGM_AttributeFilter::VT_I1);
}


Project::CBHGridFilter::CBHGridFilter()
{
	AttributeType(CCWFGM_AttributeFilter::VT_R4);
}


Project::TreeHeightGridFilter::TreeHeightGridFilter()
{
	AttributeType(CCWFGM_AttributeFilter::VT_R4);
}


Project::FuelLoadGridFilter::FuelLoadGridFilter()
{
	AttributeType(CCWFGM_AttributeFilter::VT_R4);
}


void Project::GreenupGridFilter::Reset()
{
	NumericVariant v = static_cast<std::int8_t>(0);
	ResetAttribute(v);
	Key(CWFGM_SCENARIO_OPTION_GREENUP);
}


void Project::CBHGridFilter::Reset()
{
	NumericVariant v = static_cast<float>(0);
	ResetAttribute(v);
	Key(FUELCOM_ATTRIBUTE_CBH);
}


void Project::TreeHeightGridFilter::Reset()
{
	NumericVariant v = static_cast<float>(0);
	ResetAttribute(v);
	Key(FUELCOM_ATTRIBUTE_TREE_HEIGHT);
}


void Project::FuelLoadGridFilter::Reset()
{
	NumericVariant v = static_cast<float>(0);
	ResetAttribute(v);
	Key(FUELCOM_ATTRIBUTE_FUELLOAD);
}


std::string Project::GreenupGridFilter::DisplayName() const
{
	std::string name;
	if (m_name.length())
		name = strprintf(_T("Green-up: %s"), m_name.c_str());
	else
		name = _T("Green-up: [No Name]");
	return name;
}


std::string Project::CBHGridFilter::DisplayName() const
{
	std::string name;
	if (m_name.length())
		name = strprintf(_T("Crown Base Height: %s"), m_name.c_str());
	else
		name = _T("Crown Base Height: [No Name]");
	return name;
}


std::string Project::TreeHeightGridFilter::DisplayName() const
{
	std::string name;
	if (m_name.length())
		name = strprintf(_T("Tree Height: %s"), m_name.c_str());
	else
		name = _T("Tree Height: [No Name]");
	return name;
}


std::string Project::FuelLoadGridFilter::DisplayName() const
{
	std::string name;
	if (m_name.length())
		name = strprintf(_T("Fuel Load: %s"), m_name.c_str());
	else
		name = _T("Fuel Load: [No Name]");
	return name;
}


auto Project::GreenupGridFilter::Duplicate() const -> GreenupGridFilter*
{
	return Duplicate([] { return new GreenupGridFilter(); });
}


auto Project::GreenupGridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> GreenupGridFilter*
{
	return dynamic_cast<GreenupGridFilter*>(AttributeGridFilter::Duplicate(constructor));
}


auto Project::CBHGridFilter::Duplicate() const -> CBHGridFilter*
{
	return Duplicate([] { return new CBHGridFilter(); });
}


auto Project::CBHGridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> CBHGridFilter*
{
	return dynamic_cast<CBHGridFilter*>(AttributeGridFilter::Duplicate(constructor));
}


auto Project::TreeHeightGridFilter::Duplicate() const -> TreeHeightGridFilter*
{
	return Duplicate([] { return new TreeHeightGridFilter(); });
}


auto Project::TreeHeightGridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> TreeHeightGridFilter*
{
	return dynamic_cast<TreeHeightGridFilter*>(AttributeGridFilter::Duplicate(constructor));
}


auto Project::FuelLoadGridFilter::Duplicate() const -> FuelLoadGridFilter*
{
	return Duplicate([] { return new FuelLoadGridFilter(); });
}


auto Project::FuelLoadGridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> FuelLoadGridFilter*
{
	return dynamic_cast<FuelLoadGridFilter*>(AttributeGridFilter::Duplicate(constructor));
}
