/**
 * WISE_Project: WeatherGridFilter.cpp
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

#include "CWFGM_WeatherGridFilter.h"
#include "WeatherGridFilter.h"
#include "CWFGMProject.h"
#include "angles.h"
#include "cpoints.h"
#include "XY_PolyType.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Project::CWeatherGridFilter::CWeatherGridFilter()
{
	m_filter = boost::make_intrusive<CCWFGM_WeatherGridFilter>();
	PolymorphicUserData v = (void*)this;
	m_filter->put_UserData(v);
}


Project::CWeatherGridFilter::CWeatherGridFilter(class CWeatherPolyFilter *filter)
{
	m_filter = filter->m_filter;
	PolymorphicUserData v = this;
	m_filter->put_UserData(v);
}


Project::CWeatherPolyFilter::CWeatherPolyFilter()
{
	m_filter = boost::make_intrusive<CCWFGM_WeatherGridFilter>();
	PolymorphicUserData v = (void*)this;
	m_filter->put_UserData(v);
	m_color = 0;
	m_lineWidth = 1;
	m_flags = 0x0;
}



Project::CWeatherGridFilter::~CWeatherGridFilter() { }


Project::CWeatherPolyFilter::~CWeatherPolyFilter() { }


std::string Project::CWeatherGridFilter::DisplayName() const
{
	return m_name;
}


void Project::CWeatherGridBase::Clone(boost::intrusive_ptr<ICWFGM_GridEngine>* grid) const
{
	boost::intrusive_ptr<ICWFGM_CommonBase> gf;
	m_filter->Clone(&gf);
	*grid = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(gf);
}


auto Project::CWeatherGridBase::Duplicate() const -> CWeatherGridBase*
{
	throw std::logic_error("This class can't be directly instantiated.");
}


auto Project::CWeatherGridBase::Duplicate(std::function<GridFilter*(void)> constructor) const -> CWeatherGridBase*
{
	auto gf = dynamic_cast<CWeatherGridBase*>(GridFilter::Duplicate(constructor));
	return gf;
}


auto Project::CWeatherPolyFilter::Duplicate() const -> CWeatherPolyFilter*
{
	return Duplicate([] { return new CWeatherPolyFilter(); });
}


auto Project::CWeatherPolyFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> CWeatherPolyFilter*
{
	auto gf = dynamic_cast<CWeatherPolyFilter*>(CWeatherGridBase::Duplicate(constructor));
	if (gf)
	{
		auto pgf = dynamic_cast<CWeatherPolyFilter*>(gf);
		pgf->m_color = m_color;
		pgf->m_lineWidth = m_lineWidth;
		pgf->m_flags = m_flags;
	}
	return gf;
}


HRESULT Project::CWeatherPolyFilter::ImportPolygons(const std::filesystem::path &file_name, const std::vector<std::string_view> &permissible_drivers)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	return vf->ImportPolygons(file_name, permissible_drivers);
}


HRESULT Project::CWeatherPolyFilter::ImportPolygonsWFS(const std::string &server, const std::string &layer, const std::string &uid, const std::string &pwd)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	return vf->ImportPolygonsWFS(server, layer, uid, pwd);
}


HRESULT Project::CWeatherPolyFilter::ExportPolygons(std::string_view driver_name, const std::string &export_projection, const std::filesystem::path &file_name)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	return vf->ExportPolygons(driver_name, export_projection, file_name);
}


std::uint32_t Project::CWeatherPolyFilter::AddPolygon(const XY_Point &points, std::uint32_t num_points)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	std::uint32_t num;
	XY_Poly *poly = NULL;
	try
	{
		poly = new XY_Poly(&points, num_points);
	}
	catch (std::bad_alloc & cme)
	{
		return static_cast<std::uint32_t>(-1);
	}
	HRESULT r = vf->AddPolygon(*poly, &num);
	delete poly;

	if (SUCCEEDED(r))
		return num;
	return static_cast<std::uint32_t>(-1);
}


bool Project::CWeatherPolyFilter::ClearPolygon(std::uint32_t index)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	return SUCCEEDED(vf->ClearPolygon(index));
}


double Project::CWeatherPolyFilter::Area()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	double area;
	vf->GetArea(&area);
	return area;
}


std::uint32_t Project::CWeatherPolyFilter::GetPolygonMaxSize()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolygonSize((ULONG)-1, &num)))
		return num;
	return 0;
}


std::uint32_t Project::CWeatherPolyFilter::GetPolygonCount()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolygonCount(&num)))
		return num;
	return 0;
}


std::uint32_t Project::CWeatherPolyFilter::GetPolygonSize(ULONG index)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolygonSize(index, &num)))
		return num;
	return 0;
}


XY_Point *Project::CWeatherPolyFilter::GetPolygon(std::uint32_t index, XY_Point *points, std::uint32_t *size)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	XY_Point *p;
	if (!points)
	{
		if (!(*size))
			if (FAILED(vf->GetPolygonSize(index, size)))
				return NULL;
		try
		{
			p = new XY_Point[*size];
		}
		catch (std::bad_alloc& cme)
		{
			return nullptr;
		}
	}
	else
		p = points;

	XY_Poly poly(XY_Poly::ALLOCSTYLE_NEWDELETE);
	poly.Attach(p, *size);
	HRESULT r = vf->GetPolygon(index, size, &poly);
	poly.Detach();

	if (SUCCEEDED(r))
		return p;				// success so return either points (if provided) or the memory we had to allocate
	if (p != points)
		delete [] p;				// failure so conditional memory clean-up
	return NULL;
}


bool Project::CWeatherPolyFilter::ClearAllPolygons()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	return SUCCEEDED(vf->ClearPolygon((ULONG)-1));
}


void Project::CWeatherPolyFilter::SetTemperature(double temp)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	vf->put_Temperature(temp);
}


void Project::CWeatherPolyFilter::SetRH(double rh)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	vf->put_RH(rh);
}


void Project::CWeatherPolyFilter::SetPrecipitation(double precip)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	vf->put_Precipitation(precip);
}


void Project::CWeatherPolyFilter::SetWindSpeed(double ws)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	vf->put_WindSpeed(ws);
}


void Project::CWeatherPolyFilter::SetWindDirection(double wd)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	vf->put_WindDirection(wd);
}


double Project::CWeatherPolyFilter::GetTemperature()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	double ws;
	vf->get_Temperature(&ws);
	return ws;
}


double Project::CWeatherPolyFilter::GetRH()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	double ws;
	vf->get_RH(&ws);
	return ws;
}


double Project::CWeatherPolyFilter::GetPrecipitation()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	double ws;
	vf->get_Precipitation(&ws);
	return ws;
}


double Project::CWeatherPolyFilter::GetWindSpeed()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	double ws;
	vf->get_WindSpeed(&ws);
	return ws;
}


double Project::CWeatherPolyFilter::GetWindDirection()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	double wd;
	vf->get_WindDirection(&wd);
	return wd;
}


USHORT Project::CWeatherPolyFilter::GetTemperature_Operation()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	USHORT op;
	vf->get_TemperatureOperation(&op);
	return op;
}


USHORT Project::CWeatherPolyFilter::GetRH_Operation()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	USHORT op;
	vf->get_RHOperation(&op);
	return op;
}


USHORT Project::CWeatherPolyFilter::GetPrecipitation_Operation()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	USHORT op;
	vf->get_PrecipitationOperation(&op);
	return op;
}

USHORT Project::CWeatherPolyFilter::GetWD_Operation()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	USHORT op;
	vf->get_WindDirectionOperation(&op);
	return op;
}


USHORT Project::CWeatherPolyFilter::GetWS_Operation()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	USHORT op;
	vf->get_WindSpeedOperation(&op);
	return op;
}


void Project::CWeatherPolyFilter::SetTemperature_Operation(std::uint16_t op)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	vf->put_TemperatureOperation(op);
}


void Project::CWeatherPolyFilter::SetRH_Operation(std::uint16_t op)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	vf->put_RHOperation(op);
}


void Project::CWeatherPolyFilter::SetPrecipitation_Operation(std::uint16_t op)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	vf->put_PrecipitationOperation(op);
}


void Project::CWeatherPolyFilter::SetWD_Operation(std::uint16_t op)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	vf->put_WindDirectionOperation(op);
}


void Project::CWeatherPolyFilter::SetWS_Operation(std::uint16_t op)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	vf->put_WindSpeedOperation(op);
}


HRESULT Project::CWeatherGridBase::SetStartTime(const WTime &time)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	PolymorphicAttribute v = time;
	return vf->SetAttribute(CWFGM_WEATHER_OPTION_START_TIME, v);
}


HRESULT Project::CWeatherGridBase::SetEndTime(const WTime &time)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WeatherGridFilter>(m_filter);
	PolymorphicAttribute v = time;
	return vf->SetAttribute(CWFGM_WEATHER_OPTION_END_TIME, v);
}


HRESULT Project::CWeatherGridBase::SetStartTimeOfDay(const WTimeSpan &time)
{
	return ERROR_NOT_SUPPORTED | ERROR_SEVERITY_WARNING;		// not all interfaces support this
}


HRESULT Project::CWeatherGridBase::SetEndTimeOfDay(const WTimeSpan &time)
{
	return ERROR_NOT_SUPPORTED | ERROR_SEVERITY_WARNING;
}
