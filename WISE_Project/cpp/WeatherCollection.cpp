/**
 * WISE_Project: WeatherCollection.cpp
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
#include <stdlib.h>
#include "WeatherCollection.h"
#include "angles.h"
#include "BurnPeriodOption.h"
#include "results.h"
#include "FontHelper.h"
#include <boost/algorithm/string.hpp>
#include "str_printf.h"
#include "CWFGMProject.h"

using namespace std::string_literals;
using namespace HSS_Time;


Project::WeatherStationCollection::WeatherStationCollection(const CWFGMProject *project)
: m_project(project) {
}


Project::WeatherStationCollection::WeatherStationCollection() {
	m_project = nullptr;
}


Project::WeatherStationCollection::~WeatherStationCollection()
{
	WeatherStation *station;
	while (station = m_stationList.RemHead())
		delete station;
}


HRESULT Project::WeatherStationCollection::PCOM_CreateWeatherStation(const char *name, WeatherStation **station)
{
	HRESULT hr = S_OK;
	WeatherStation *pStation = New(this);
	pStation->m_name = name;
		AddStation(pStation);
	if (station)
		*station = pStation;
	return hr;
}


HRESULT Project::WeatherStationCollection::PCOM_SetWeatherStation(WeatherStation *pStation, CCWFGM_Grid *grid, double Latitude, double Longitude, double Elevation)
{
	HRESULT hr;
	PolymorphicAttribute v;
	double x, y, res;
	((CWFGMProject *)m_project)->GetResolution(&res);
	((CWFGMProject *)m_project)->GetXYByLL(Latitude, Longitude, &x, &y);
	x /= res;
	y /= res;
	if (pStation->GridLocation(x, y))
	{
		if (Elevation < 0) {
			double elevation;
			if (SUCCEEDED(hr = grid->GetAttribute(CWFGM_GRID_ATTRIBUTE_DEFAULT_ELEVATION, &v)))
			{
				VariantToDouble_(v, &elevation);
				hr = pStation->Elevation(elevation);
			}
		}
		else
			hr = pStation->Elevation(Elevation);
	}
	else
		hr = E_FAIL;

	return hr;
}


auto Project::WeatherStationCollection::FindName(const TCHAR *name) const -> WeatherStation*
{
	WeatherStation *ws = m_stationList.LH_Head();
	while (ws->LN_Succ())
	{
		if (boost::equals(ws->m_name, name))
			return ws;
		ws = ws->LN_Succ();
	}
	ws = m_stationList.LH_Head();
	while (ws->LN_Succ())
	{
		if (boost::iequals(ws->m_name, name))
			return ws;
		ws = ws->LN_Succ();
	}
	return nullptr;
}


void Project::WeatherStream::ExportBasicInfo(FILE *fp)
{
	std::string csBuf;
	csBuf = strprintf(_T("\t%s ( %s )"), m_name.c_str(), m_station->m_name.c_str());
	_fputts((csBuf+"\n").c_str(),fp);
	WTime timeToExport((ULONGLONG)0, m_station->m_collection->m_project->m_timeManager);
	//export start time;
	GetStartTime(timeToExport);
	csBuf=timeToExport.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_ABBREV | WTIME_FORMAT_DATE | WTIME_FORMAT_TIME | WTIME_FORMAT_EXCLUDE_SECONDS);
	csBuf=_T("\t\tStart Time:\t") + csBuf;
	_fputts((csBuf + _T("\n")).c_str(),fp);
	// export end time
	GetEndTime(timeToExport);
	csBuf=timeToExport.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_ABBREV | WTIME_FORMAT_DATE | WTIME_FORMAT_TIME | WTIME_FORMAT_EXCLUDE_SECONDS);
	csBuf=_T("\t\tEnd Time:\t") + csBuf;
	_fputts((csBuf + _T("\n")).c_str(),fp);
}


bool Project::WeatherStation::GridLocation(double *x, double *y) const
{
	XY_Point pt;
	HRESULT hr = m_station->GetLocation(&pt);
	if (SUCCEEDED(hr))
	{
		*x = pt.x;
		*y = pt.y;
	}
	return SUCCEEDED(hr) ? true : false;
}


double Project::WeatherStation::Latitude() const
{
	PolymorphicAttribute var;
	double lat;
	if (FAILED(m_station->GetAttribute(CWFGM_GRID_ATTRIBUTE_LATITUDE, &var)))
		return 0.0;
	if (FAILED(VariantToDouble_(var, &lat)))
		return 0.0;
	return lat;
}


double Project::WeatherStation::Longitude() const
{
	PolymorphicAttribute var;
	double lon;
	if (FAILED(m_station->GetAttribute(CWFGM_GRID_ATTRIBUTE_LONGITUDE, &var)))
		return 0.0;
	if (FAILED(VariantToDouble_(var, &lon)))
		return 0.0;
	return lon;
}


double Project::WeatherStation::Elevation() const
{
	PolymorphicAttribute var;
	double l;
	bool b;

	if (FAILED(m_station->GetAttribute(CWFGM_GRID_ATTRIBUTE_DEFAULT_ELEVATION_SET, &var)))
		return 0.0;
	if (FAILED(VariantToBoolean_(var, &b)))
		return 0.0;
	if (!b)
		return 0.0;
	if (FAILED(m_station->GetAttribute(CWFGM_GRID_ATTRIBUTE_DEFAULT_ELEVATION, &var)))
		return 0.0;
	if (FAILED(VariantToDouble_(var, &l)))
		return 0.0;
	return l;
}


bool Project::WeatherStation::GridLocation(double x, double y)
{
	XY_Point pt(x, y);
	HRESULT hr = m_station->SetLocation(pt);
	return SUCCEEDED(hr);
}


HRESULT Project::WeatherStation::Elevation(double elevation)
{
	return m_station->SetAttribute(CWFGM_GRID_ATTRIBUTE_DEFAULT_ELEVATION, elevation);
}


auto Project::WeatherStationCollection::New(const WTimeManager *timeManager) const -> WeatherStation*
{
	WeatherStation *ws = new WeatherStation(this);
	if ((ws) && (m_project) && (m_project->gridEngine()))
		ws->m_station->put_GridEngine(m_project->gridEngine());
	return ws;
}


auto Project::WeatherStationCollection::New(const WeatherStation &s) const -> WeatherStation*
{
	WeatherStation *ws = new WeatherStation(s);
	if ((ws) && (m_project) && (m_project->gridEngine()))
		ws->m_station->put_GridEngine(m_project->gridEngine());
	return ws;
}


bool Project::WeatherStationCollection::AssignNewGrid(ICWFGM_GridEngine *grid)
{
	WeatherStation *gf = FirstStation();

	ICWFGM_CommonData* data;
	if (FAILED(grid->GetCommonData(nullptr, &data)))
		return false;

	while (gf->LN_Succ())
	{
		gf->m_station->put_GridEngine(grid);
		gf->m_station->put_CommonData(data);
		gf = gf->LN_Succ();
	}
	return true;
}
