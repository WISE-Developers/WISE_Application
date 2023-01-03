/**
 * WISE_Project: GISOptions.cpp
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
#include "cwinapp.h"
#else
#include "AfxIniSettings.h"
#endif
#include "GISOptions.h"
#include "GridCom_ext.h"


Project::GISOptions::GISOptions()
{
	m_initialSize = 10000.0;
	m_growSize = 5000.0;
	m_reactionSize = 1000.0;
	m_allowGrow = true;
}


#ifdef _MSC_VER

Project::GISOptions::GISOptions(const TCHAR *group_name)
{
#if _DLL
	CWinAppExt* app = (CWinAppExt*)AfxGetApp();
#else
	IniSettings* app = AfxGetApp();
#endif
	m_initialSize = app->GetProfileDouble(group_name, _T("Initial Size"), 10000.0);
	m_growSize = app->GetProfileDouble(group_name, _T("Growth Size"), 5000.0);
	m_reactionSize = app->GetProfileDouble(group_name, _T("Reaction Size"), 1000.0);
	m_allowGrow = app->GetProfileBOOL(group_name, _T("Allow Growth"), true) ? true : false;
}


void Project::GISOptions::SaveToIniFile(const TCHAR *group_name) const 
{
#if _DLL
	CWinAppExt* app = (CWinAppExt*)AfxGetApp();
#else
	IniSettings* app = AfxGetApp();
#endif

	app->WriteProfileDouble(group_name, _T("Initial Size"), m_initialSize, _T("%.2lf"));
	app->WriteProfileDouble(group_name, _T("Growth Size"), m_growSize, _T("%.2lf"));
	app->WriteProfileDouble(group_name, _T("Reaction Size"), m_reactionSize, _T("%.2lf"));
	app->WriteProfileBOOL(group_name, _T("Allow Growth"), m_allowGrow);
}

#endif


Project::GISOptions::GISOptions(const GISOptions *toCopy)
{
	if (toCopy)
		*this = *toCopy;
	else
	{
		m_initialSize = 10000.0;
		m_growSize = 5000.0;
		m_reactionSize = 1000.0;
		m_allowGrow = TRUE;
	}
}


Project::GISOptions::GISOptions(CCWFGM_Grid *grid)
{
	PolymorphicAttribute v;
	try
	{
		/*POLYMORPHIC CHECK*/
		if (SUCCEEDED(grid->GetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, &v)))	m_initialSize = std::get<double>(v);
		if (SUCCEEDED(grid->GetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, &v)))		m_growSize = std::get<double>(v);
		if (SUCCEEDED(grid->GetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, &v)))		m_reactionSize = std::get<double>(v);
		if (SUCCEEDED(grid->GetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, &v)))	m_allowGrow = std::get<bool>(v);
	}
	catch (std::bad_variant_access &)
	{
		weak_assert(false);;
	}
}


Project::GISOptions::GISOptions(CCWFGM_AttributeFilter *filter)
{
	PolymorphicAttribute v;
	try
	{
		/*POLYMORPHIC CHECK*/
		if (SUCCEEDED(filter ->GetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, &v)))		m_initialSize = std::get<double>(v);
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, &v)))		m_growSize = std::get<double>(v);
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, &v)))		m_reactionSize = std::get<double>(v);
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, &v)))	m_allowGrow = std::get<bool>(v);
	}
	catch (std::bad_variant_access &)
	{
		weak_assert(false);;
	}
}


Project::GISOptions::GISOptions(CCWFGM_PolyReplaceGridFilter *filter)
{
	PolymorphicAttribute v;
	try
	{
		/*POLYMORPHIC CHECK*/
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, &v)))		m_initialSize = std::get<double>(v);
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, &v)))		m_growSize = std::get<double>(v);
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, &v)))		m_reactionSize = std::get<double>(v);
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, &v)))	m_allowGrow = std::get<bool>(v);
	}
	catch (std::bad_variant_access &)
	{
		weak_assert(false);;
	}
}


Project::GISOptions::GISOptions(CCWFGM_WeatherGridFilter *filter)
{
	PolymorphicAttribute v;
	try
	{
		/*POLYMORPHIC CHECK*/
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, &v)))		m_initialSize = std::get<double>(v);
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, &v)))		m_growSize = std::get<double>(v);
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, &v)))		m_reactionSize = std::get<double>(v);
		if (SUCCEEDED(filter->GetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, &v)))	m_allowGrow = std::get<bool>(v);
	}
	catch (std::bad_variant_access &)
	{
		weak_assert(false);;
	}
}


Project::GISOptions::GISOptions(ICWFGM_VectorEngine *vector)
{
	PolymorphicAttribute v;
	try
	{
		/*POLYMORPHIC CHECK*/
		if (SUCCEEDED(vector->GetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, &v)))		m_initialSize = std::get<double>(v);
		if (SUCCEEDED(vector->GetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, &v)))		m_growSize = std::get<double>(v);
		if (SUCCEEDED(vector->GetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, &v)))		m_reactionSize = std::get<double>(v);
		if (SUCCEEDED(vector->GetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, &v)))	m_allowGrow = std::get<bool>(v);
	}
	catch (std::bad_variant_access &)
	{
		weak_assert(false);;
	}
}


Project::GISOptions::GISOptions(CCWFGM_Ignition *ignition)
{
	PolymorphicAttribute v;
	try
	{
		/*POLYMORPHIC CHECK*/
		if (SUCCEEDED(ignition->GetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, &v)))	m_initialSize = std::get<double>(v);
		if (SUCCEEDED(ignition->GetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, &v)))		m_growSize = std::get<double>(v);
		if (SUCCEEDED(ignition->GetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, &v)))		m_reactionSize = std::get<double>(v);
		if (SUCCEEDED(ignition->GetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, &v)))	m_allowGrow = std::get<bool>(v);
	}
	catch (std::bad_variant_access &)
	{
		weak_assert(false);;
	}
}


bool Project::GISOptions::SaveToGrid(CCWFGM_Grid *grid) const
{
	PolymorphicAttribute v;
	v = m_initialSize;	if (FAILED(grid->SetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, v)))					return false;
	v = m_growSize;		if (FAILED(grid->SetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, v)))						return false;
	v = m_reactionSize; if (FAILED(grid->SetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, v)))						return false;
	v = m_allowGrow;	if (FAILED(grid->SetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, v)))					return false;
	return true;
}


bool Project::GISOptions::SaveToFilter(CCWFGM_AttributeFilter *filter) const
{
	PolymorphicAttribute v;
	v = m_initialSize;	if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, v)))					return false;
	v = m_growSize;		if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, v)))					return false;
	v = m_reactionSize; if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, v)))					return false;
	v = m_allowGrow;	if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, v)))				return false;
	return true;
}


bool Project::GISOptions::SaveToFilter(CCWFGM_PolyReplaceGridFilter *filter) const
{
	PolymorphicAttribute v;
	v = m_initialSize;	if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, v)))					return false;
	v = m_growSize;		if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, v)))					return false;
	v = m_reactionSize; if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, v)))					return false;
	v = m_allowGrow;	if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, v)))				return false;
	return true;
}


bool Project::GISOptions::SaveToFilter(CCWFGM_WeatherGridFilter *filter) const
{
	PolymorphicAttribute v;
	v = m_initialSize;	if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, v)))					return false;
	v = m_growSize;		if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, v)))					return false;
	v = m_reactionSize; if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, v)))					return false;
	v = m_allowGrow;	if (FAILED(filter->SetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, v)))				return false;
	return true;
}


bool Project::GISOptions::SaveToVector(ICWFGM_VectorEngine *vector) const
{
	PolymorphicAttribute v;
	v = m_initialSize;	if (FAILED(vector->SetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, v)))					return false;
	v = m_growSize;		if (FAILED(vector->SetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, v)))					return false;
	v = m_reactionSize; if (FAILED(vector->SetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, v)))					return false;
	v = m_allowGrow;	if (FAILED(vector->SetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, v)))				return false;
	return true;
}


bool Project::GISOptions::SaveToIgnition(CCWFGM_Ignition *ignition) const
{
	PolymorphicAttribute v;
	v = m_initialSize;	if (FAILED(ignition->SetAttribute(CWFGM_GRID_ATTRIBUTE_INITIALSIZE, v)))				return false;
	v = m_growSize;		if (FAILED(ignition->SetAttribute(CWFGM_GRID_ATTRIBUTE_GROWTHSIZE, v)))					return false;
	v = m_reactionSize; if (FAILED(ignition->SetAttribute(CWFGM_GRID_ATTRIBUTE_BUFFERSIZE, v)))					return false;
	v = m_allowGrow;	if (FAILED(ignition->SetAttribute(CWFGM_GRID_ATTRIBUTE_GIS_CANRESIZE, v)))				return false;
	return true;
}


auto Project::GISOptions::operator=(const GISOptions &toCopy) -> const GISOptions&
{
	m_initialSize = toCopy.m_initialSize;
	m_growSize = toCopy.m_growSize;
	m_reactionSize = toCopy.m_reactionSize;
	m_allowGrow = toCopy.m_allowGrow;
	return *this;
}
