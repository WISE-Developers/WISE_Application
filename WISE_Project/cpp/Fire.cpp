/**
 * WISE_Project: Fire.cpp
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
#include <shlwapi.h>
#endif
#include "StdFireCollection.h"
#include "ScenarioCollection.h"
#include "angles.h"
#include <assert.h>
#include "XY_PolyType.h"
#include "str_printf.h"
#include "CWFGMProject.h"

#include <gsl/gsl_util>
#include <boost/algorithm/string.hpp>


Project::StdFire::StdFire(const StdFireCollection &fireCollection)
	: m_fireCollection(fireCollection)
{
	m_fire = boost::make_intrusive<CCWFGM_Ignition>();

	PolymorphicUserData v = (void*)this;
	m_fire->put_UserData(v);
}


Project::StdFire::StdFire(const StdFireCollection &fireCollection, const StdFire &fire)
	: m_fireCollection(fireCollection)
{
	boost::intrusive_ptr<ICWFGM_CommonBase> f;
	HRESULT hr;
	hr = fire.m_fire->Clone(&f);
	if (FAILED(hr))
		throw std::logic_error("Failed");
	m_fire = boost::dynamic_pointer_cast<CCWFGM_Ignition>(f);
	PolymorphicUserData v = (void*)this;
	m_fire->put_UserData(v);
	m_name = fire.m_name;
	m_comments = fire.m_comments;
}


Project::StdFire::StdFire(const StdFireCollection &fireCollection, boost::intrusive_ptr<CCWFGM_Ignition> &fire)
	: m_fireCollection(fireCollection)
{
	m_fire = fire;
	PolymorphicUserData v = (void*)this;
	m_fire->put_UserData(v);
}


Project::StdFire::~StdFire() { }


HRESULT Project::StdFire::SetIgnition(std::uint32_t index, std::uint16_t ignition_type, std::uint32_t ignition_size, XY_Point *points)
{
	if (!ignition_size)		return E_FAIL;
	if (!points)			return E_FAIL;

	if (ignition_size == 1)
		ignition_type = CWFGM_FIRE_IGNITION_POINT;

	XY_PolyConst p(points, ignition_size);
	return m_fire->SetIgnition(index, ignition_type, p);
}


HRESULT Project::StdFire::ImportIgnition(const std::string &file_name, const std::vector<std::string> *pd)
{
	return m_fire->ImportIgnition(file_name, pd, nullptr);
}


HRESULT Project::StdFire::ImportIgnitionWFS(const std::string &server, const std::string &layer, const std::string &uid, const std::string &pwd)
{
	return m_fire->ImportIgnitionWFS(server, layer, uid, pwd);
}


HRESULT Project::StdFire::ExportIgnition(const std::string &driver_name, const std::string &export_projection, const std::string &file_name)
{
	return m_fire->ExportIgnition(driver_name, export_projection, file_name);
}


XY_Point *Project::StdFire::GetIgnition(std::uint32_t index, std::uint16_t *ignition_type, std::uint32_t *ignition_size) const
{
	HRESULT hr{ S_OK };
	XY_Point *pt = nullptr;
	if (!(*ignition_size))
	{
		hr = m_fire->GetIgnitionSize(index, ignition_size);
		if (FAILED(hr))
			return pt;
		if (!(*ignition_size))
			return pt;
	}

	XY_Poly ig(XY_Poly::ALLOCSTYLE_NEWDELETE);
	ig.SetNumPoints(*ignition_size);

	hr = m_fire->GetIgnition(index, ignition_type, &ig);
	if (FAILED(hr))
		return nullptr;

	return ig.Detach();
}


// Set the ignition time of this fire
HRESULT Project::StdFire::HIgnitionTime(const WTime &time)
{	
	return m_fire->SetIgnitionTime(time);
}


std::uint32_t Project::StdFire::GetIgnitionSize(std::uint32_t index)
{
	std::uint32_t size = 0;
	m_fire->GetIgnitionSize(index, &size);
	return size;
}


std::uint32_t Project::StdFire::GetIgnitionMaxSize()
{
	std::uint32_t size = 0;
	m_fire->GetIgnitionSize(gsl::narrow_cast<std::uint32_t>(-1), &size);
	return size;
}


void Project::StdFire::ExportBasicInfo(FILE *fp)
{
// this is for exporting scenario fire information to form part of history file. 
// export fire name
	std::string csBuf;
	csBuf = strprintf(_T("\t%s\n"), m_name.c_str());
	_fputts(csBuf.c_str(), fp);

	WTime timeToExport(0ULL, m_fireCollection.m_project->m_timeManager);
// export start time
	timeToExport = IgnitionTime();
	csBuf = timeToExport.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_ABBREV | WTIME_FORMAT_DATE |
	    WTIME_FORMAT_TIME | WTIME_FORMAT_EXCLUDE_SECONDS);
	csBuf = _T("\t\tStart Time:\t")+csBuf+_T("\n");
	_fputts(csBuf.c_str(),fp);
// export ignition type
	ULONG i = 0;
	short igType = IgnitionType(i);
	while (igType != CWFGM_FIRE_IGNITION_UNDEFINED)
	{
		switch(igType)
		{
		case CWFGM_FIRE_IGNITION_POINT:
			csBuf = _T("\t\tIgnition Type:\tIgnition Point");
			break;
		case CWFGM_FIRE_IGNITION_LINE:
			csBuf = _T("\t\tIgnition Type:\tIgnition Line");
			break;
		case CWFGM_FIRE_IGNITION_POLYGON_OUT:
		case CWFGM_FIRE_IGNITION_POLYGON_IN:
			csBuf = _T("\t\tIgnition Type:\tIgnition Polygon");
			break;
		}
		igType = IgnitionType(++i);
	};
	_fputts((csBuf+"\n").c_str(),fp);
}


Project::StdFireCollection::StdFireCollection(const CWFGMProject *project)
    : m_project(project)
{
}


bool Project::StdFireCollection::AssignNewGrid(ICWFGM_GridEngine *grid)
{
	StdFire *gf = FirstFire();

	ICWFGM_CommonData* data;
	if (FAILED(grid->GetCommonData(nullptr, &data)))
		return false;

	while (gf->LN_Succ())
	{
		gf->m_fire->put_GridEngine(grid);
		gf->m_fire->put_CommonData(data);
		gf = gf->LN_Succ();
	}
	return true;
}


WTime Project::StdFire::IgnitionTime()
{
	HSS_Time::WTime t(0ULL, m_fireCollection.m_project->m_timeManager);
	if (FAILED(m_fire->GetIgnitionTime(&t)))
		t = HSS_Time::WTime(0ULL, m_fireCollection.m_project->m_timeManager);
	return t;
}


Project::StdFireCollection::~StdFireCollection()
{
	StdFire *fire;
	while ((fire = m_fireList.RemHead()))
		delete fire;
}


auto Project::StdFireCollection::FindName(const std::string& name) const -> StdFire*
{
	StdFire *f = m_fireList.LH_Head();
	while (f->LN_Succ())
	{
		if (boost::equals(f->m_name, name))
			return f;
		f = f->LN_Succ();
	}
	return nullptr;
}
