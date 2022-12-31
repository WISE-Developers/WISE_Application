/**
 * WISE_Project: AttributeGridFilter.cpp
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

#ifdef _MSC_VER
#include "stdafx.h"
#endif

#include "AttributeGridFilter.h"
#include "CWFGM_AttributeFilter.h"
#include "str_printf.h"

using namespace std::string_literals;



Project::AttributeGridFilter::AttributeGridFilter()
{
	m_filter = boost::make_intrusive<CCWFGM_AttributeFilter>();
	PolymorphicUserData v = (void*)this;
	m_filter->put_UserData(v);
}


Project::AttributeGridFilter::~AttributeGridFilter() { }


std::string Project::AttributeGridFilter::DisplayName() const
{
	std::string name;
	if (m_name.length())
		name = strprintf(_T("[Attribute]: %s"), m_name.c_str());
	else
		name = "[Attribute]: (No Name)"s;
	return name;
}


auto Project::AttributeGridFilter::Duplicate() const -> AttributeGridFilter*
{
	return Duplicate([] { return new AttributeGridFilter(); });
}


auto Project::AttributeGridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> AttributeGridFilter*
{
	return dynamic_cast<AttributeGridFilter*>(GridFilter::Duplicate(constructor));
}


void Project::AttributeGridFilter::Clone(boost::intrusive_ptr<ICWFGM_GridEngine> *grid) const
{
	boost::intrusive_ptr<ICWFGM_CommonBase> gf;
	m_filter->Clone(&gf);
	*grid = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(gf);

	if (*grid)
	{
		PolymorphicUserData v = (void *)this;
		(*grid)->put_UserData(v);
	}
}


bool Project::AttributeGridFilter::SetAttributePoint(const XY_Point &pt, const NumericVariant &attribute)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	if (gf)
		return SUCCEEDED(gf->SetAttributePoint(pt, attribute));
	return false;
}


bool Project::AttributeGridFilter::SetAttributeLine(const XY_Point &pt1, const XY_Point &pt2, const NumericVariant &attribute)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	if (gf)
		return SUCCEEDED(gf->SetAttributeLine(pt1, pt2, attribute));
	return false;
}


NumericVariant Project::AttributeGridFilter::GetAttribute(const XY_Point &pt) const
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	NumericVariant break_var;
	if (gf)
	{
		const NumericVariant empty;
		grid::AttributeValue break_valid{ grid::AttributeValue::NOT_SET };
		const HRESULT hr = gf->GetAttributePoint(pt, &break_var, &break_valid);
		if (FAILED(hr) || (break_valid == grid::AttributeValue::NOT_SET))
			break_var = empty;
	}
	return break_var;
}


bool Project::AttributeGridFilter::ResetAttribute(const NumericVariant &attribute)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	if (gf)
		return SUCCEEDED(gf->ResetAttribute(attribute));
	return false;
}


std::uint16_t Project::AttributeGridFilter::AttributeType() const
{
	std::uint16_t type = CCWFGM_AttributeFilter::VT_ILLEGAL;
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	if (gf)
		if (FAILED(gf->get_OptionType(&type)))
			type = CCWFGM_AttributeFilter::VT_ILLEGAL;
	return type;
}


std::uint16_t Project::AttributeGridFilter::AttributeType(std::uint16_t type)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	if (gf)
		if (FAILED(gf->put_OptionType(type)))
			return CCWFGM_AttributeFilter::VT_ILLEGAL;
	return type;
}


USHORT Project::AttributeGridFilter::Key() const
{
	USHORT type = gsl::narrow_cast<USHORT>(-1);
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	if (gf)
		if (FAILED(gf->get_OptionKey(&type)))
			type = gsl::narrow_cast<USHORT>(-1);
	return type;
}


USHORT Project::AttributeGridFilter::Key(USHORT key)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	if (gf)
		if (FAILED(gf->put_OptionKey(key)))
			return (USHORT)-1;
	return key;
}


HRESULT Project::AttributeGridFilter::Import(const Project::CWFGMProject* /*project*/, const std::string &prj_filename, const std::string &name)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	if (gf)
		return gf->ImportAttributeGrid(prj_filename, name);
	return E_FAIL;
}


HRESULT Project::AttributeGridFilter::Export(const std::string &prj_filename, const std::string &name)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_AttributeFilter>(m_filter);
	if (gf)
		return gf->ExportAttributeGrid(prj_filename, name, TypeString());
	return E_FAIL;
}
