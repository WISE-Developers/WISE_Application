/**
 * WISE_Project: CWFGMProject.cpp
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
#include "propsysreplacement.h"
#include "GDALExporter.h"
#include <cpl_string.h>
#include "str_printf.h"
#include "filesystem.hpp"
#include <string>
#include <algorithm>
#include <cctype>
#include <ctime>
#include "CWFGMProject.h"
#include "angles.h"
#include "ReplaceGridFilter.h"
#include "WeatherGridFilter.h"
#include "WindDirectionGrid.h"
#include "WindSpeedGrid.h"
#include "PrintReportOptions.h"
#include "AttributeGridFilter.h"
#include "FuelCom_ext.h"
#include "str_printf.h"
#include "Dlgcnvt.h"
#include "stdchar.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <omp.h>
#include "raytrace.h"

using namespace std::string_literals;
using namespace HSS_Time;


#if !defined(_DLL) || _DLL == 0
class CWinAppProjectImpl : public CWinAppProject
{
protected:
	Project::FBPOptions              *m_FBPOptions;
	Project::BasicScenarioOptions    *m_basicScenarioOptions;
	Project::CBurnPeriodOption       *m_burnPeriodOptions;
	Project::ViewCollection          *m_configurationOptions;
	Project::CuringOptions           *m_curingOptions;

public:
    CWinAppProjectImpl();
    virtual ~CWinAppProjectImpl();

public:
	virtual Project::FBPOptions* getFBPOptions() override { return m_FBPOptions; }
	virtual Project::BasicScenarioOptions* getBasicScenarioOptions() override { return m_basicScenarioOptions; }
	virtual Project::CBurnPeriodOption* getBurnPeriodOptions() override { return m_burnPeriodOptions; }
	virtual Project::ViewCollection* getConfigurationOptions() override { return m_configurationOptions; }
	virtual Project::CuringOptions* getCuringOptions() override { return m_curingOptions; }
	virtual const HSS_Types::RuntimeClass *findClass(const char *name) override;
};

CWinAppProjectImpl::CWinAppProjectImpl()
{
    m_FBPOptions = new Project::FBPOptions("FBP Options");
    m_basicScenarioOptions = new Project::BasicScenarioOptions("Basic Scenario Options");
    m_burnPeriodOptions = new Project::CBurnPeriodOption("Burn Options");
    m_configurationOptions = new Project::ViewCollection("Configuration Options");
    m_curingOptions = new Project::CuringOptions("Grass Curing Options");
}

CWinAppProjectImpl::~CWinAppProjectImpl()
{
    if (m_FBPOptions)
    {
        m_FBPOptions->SaveToIniFile("FBP Options");
        delete m_FBPOptions;
        m_FBPOptions = nullptr;
    }
    if (m_basicScenarioOptions)
    {
        m_basicScenarioOptions->SaveToIniFile("Basic Scenario Options");
        delete m_basicScenarioOptions;
        m_basicScenarioOptions = nullptr;
    }
    if (m_burnPeriodOptions)
    {
        m_burnPeriodOptions->SaveToIniFile("Burn Options");
        delete m_burnPeriodOptions;
        m_burnPeriodOptions = nullptr;
    }
    if (m_configurationOptions)
    {
        m_configurationOptions->SaveToIniFile("Configuration Options");
        delete m_configurationOptions;
        m_configurationOptions = nullptr;
    }
    if (m_curingOptions)
    {
        m_curingOptions->SaveToIniFile("Grass Curing Options");
        delete m_curingOptions;
        m_curingOptions = nullptr;
    }
}

const HSS_Types::RuntimeClass* CWinAppProjectImpl::findClass(const char *name)
{
    return nullptr;
}

static CWinAppProjectImpl s_appImpl;

CWinAppProject* AfxGetProjectApp() { return &s_appImpl; }
#endif


std::int32_t Project::CWFGMProject::m_appMode = 0;
std::int32_t Project::CWFGMProject::m_managedMode = 0;

Project::CWFGMProject::CWFGMProject() :
	m_loadBalancing(Project::LoadBalancingType::NONE),
	m_grid((FuelCollection::m_defaultCountry != FuelCollection::DEFAULTCOUNTRY_NONE) ? boost::make_intrusive<CCWFGM_Grid>() : nullptr),
	m_gridEngine(m_grid),
	m_timeManager(m_grid ? &m_grid->m_timeManager : nullptr),
    m_fireCollection(this),
    m_weatherCollection(this),
    m_scenarioCollection(this,
    (m_appMode > 0) ? (AfxGetProjectApp()->getFBPOptions()) : nullptr,
    (m_appMode > 0) ? (AfxGetProjectApp()->getBasicScenarioOptions()) : nullptr,
    (m_appMode > 0) ? (AfxGetProjectApp()->getBurnPeriodOptions()) : nullptr,
	(m_appMode > 0) ? (AfxGetProjectApp()->getCuringOptions()) : nullptr),
    m_viewCollection((m_appMode > 0) ? (AfxGetProjectApp()->getConfigurationOptions()) : nullptr),
	m_gridFilterCollection(this),
    m_vectorCollection(this),
	m_assetCollection(this),
	m_targetCollection(this),
	m_outputs(this)
{
	weak_assert(m_appMode);
	HRESULT hr;
	boost::intrusive_ptr<ICWFGM_CommonBase> c;
	switch (FuelCollection::m_defaultCountry)
	{
	case FuelCollection::DEFAULTCOUNTRY_CANADA:
							hr = FuelCollection::m_fuelMap_Canada->Clone(&c);
							m_fuelMap = dynamic_cast<CCWFGM_FuelMap *>(c.get());
							break;
	case FuelCollection::DEFAULTCOUNTRY_NEWZEALAND:	hr = FuelCollection::m_fuelMap_NewZealand->Clone(&c);
							m_fuelMap = dynamic_cast<CCWFGM_FuelMap *>(c.get());
							break;
	case FuelCollection::DEFAULTCOUNTRY_TASMANIA:	hr = FuelCollection::m_fuelMap_Tasmania->Clone(&c);
							m_fuelMap = dynamic_cast<CCWFGM_FuelMap *>(c.get());
							break;
	case FuelCollection::DEFAULTCOUNTRY_NONE:
							hr = S_OK;
							break;
	default:
		throw std::logic_error("Not supported");
	}
	if (FAILED(hr))
		throw std::logic_error("Failure");

	m_gridFilterCollection.AssignNewGrid(m_fuelMap.get(), m_gridEngine.get());
	m_vectorCollection.AssignNewGrid(m_gridEngine.get());
	m_assetCollection.AssignNewGrid(m_gridEngine.get());
	m_targetCollection.AssignNewGrid(m_gridEngine.get());
	m_fireCollection.AssignNewGrid(m_gridEngine.get());
	m_weatherCollection.AssignNewGrid(m_gridEngine.get());
	m_scenarioCollection.AssignNewGrid(NULL, m_gridEngine.get());

	if (m_grid) {
		hr = m_grid->put_FuelMap(m_fuelMap.get());
		if (FAILED(hr))
			throw std::logic_error("Failure");
	}

	m_resolution = -1.0;

	m_fuelCollection.m_fuelMap = m_fuelMap;
	switch (FuelCollection::m_defaultCountry)
	{
	case FuelCollection::DEFAULTCOUNTRY_CANADA:
		FuelCollection::m_fuelCollection_Canada->Clone(&m_fuelCollection);
		m_fuelCollection.m_csLUTFileName = "Canada";
		break;
	case FuelCollection::DEFAULTCOUNTRY_NEWZEALAND:
		FuelCollection::m_fuelCollection_NewZealand->Clone(&m_fuelCollection);
		m_fuelCollection.m_csLUTFileName = "New Zealand";
		break;
	case FuelCollection::DEFAULTCOUNTRY_TASMANIA:
		FuelCollection::m_fuelCollection_Tasmania->Clone(&m_fuelCollection);
		m_fuelCollection.m_csLUTFileName = "Tasmania";
		break;
	}

	m_pool = nullptr;
}								// the green-up.  Scenarios created later on (by the user) won't have
								// this problem 'cause everything else will have been set up already.

Project::CWFGMProject::CWFGMProject(CCWFGM_FuelMap *fuelMap, FuelCollection *fuelCollection, bool resetLUTName) :
	m_loadBalancing(Project::LoadBalancingType::NONE),
	m_grid(boost::make_intrusive<CCWFGM_Grid>()),
	m_gridEngine(m_grid),
	m_timeManager(m_grid ? &m_grid->m_timeManager : nullptr),
	m_fireCollection(this),
    m_weatherCollection(this),
    m_scenarioCollection(this,
    (m_appMode > 0) ? (AfxGetProjectApp()->getFBPOptions()) : nullptr,
    (m_appMode > 0) ? (AfxGetProjectApp()->getBasicScenarioOptions()) : nullptr,
    (m_appMode > 0) ? (AfxGetProjectApp()->getBurnPeriodOptions()) : nullptr,
	(m_appMode > 0) ? (AfxGetProjectApp()->getCuringOptions()) : nullptr),
    m_viewCollection((m_appMode > 0) ? (AfxGetProjectApp()->getConfigurationOptions()) : nullptr),
    m_gridFilterCollection(this),
	m_vectorCollection(this),
	m_assetCollection(this),
	m_targetCollection(this),
	m_outputs(this)
{
	weak_assert(m_appMode);

	boost::intrusive_ptr<ICWFGM_CommonBase> c;
	fuelMap->Clone(&c);
	m_fuelMap = boost::dynamic_pointer_cast<CCWFGM_FuelMap>(c);

	m_gridEngine->put_LayerManager(m_layerManager.get());

	m_gridFilterCollection.AssignNewGrid(m_fuelMap.get(), m_gridEngine.get());
	m_vectorCollection.AssignNewGrid(m_gridEngine.get());
	m_assetCollection.AssignNewGrid(m_gridEngine.get());
	m_targetCollection.AssignNewGrid(m_gridEngine.get());
	m_fireCollection.AssignNewGrid(m_gridEngine.get());
	m_weatherCollection.AssignNewGrid(m_gridEngine.get());
	m_scenarioCollection.AssignNewGrid(NULL, m_gridEngine.get());

	m_resolution = -1.0;

	HRESULT hr = m_grid->put_FuelMap(m_fuelMap.get());
	if (FAILED(hr))
		throw std::logic_error("Failure");

	m_fuelCollection.m_fuelMap = m_fuelMap;
	fuelCollection->Clone(&m_fuelCollection);

	if (resetLUTName)
	{
		switch (FuelCollection::m_defaultCountry)
		{
		case FuelCollection::DEFAULTCOUNTRY_CANADA:
			m_fuelCollection.m_csLUTFileName = "Canada";
			break;
		case FuelCollection::DEFAULTCOUNTRY_NEWZEALAND:
			m_fuelCollection.m_csLUTFileName = "New Zealand";
			break;
		case FuelCollection::DEFAULTCOUNTRY_TASMANIA:
			m_fuelCollection.m_csLUTFileName = "Tasmania";
			break;
		}
	}

	m_pool = nullptr;
}


Project::CWFGMProject::~CWFGMProject()
{
	if (m_pool)
		delete m_pool;
}


CWorkerThreadPool *Project::CWFGMProject::GetPool()
{
	CRWThreadSemaphoreEngage lock(m_poolLock);
	if (!m_pool)
		m_pool = new CWorkerThreadPool(nullptr, nullptr, CWorkerThreadPool::NumberIdealProcessors(0), THREAD_PRIORITY_LOWEST, true);
	return m_pool;
}


void Project::CWFGMProject::AssignNewGrid(CCWFGM_Grid *grid)
{
	boost::intrusive_ptr<ICWFGM_GridEngine> oldgrid = m_gridEngine;
	m_grid = grid;
	m_gridEngine = m_grid;

	ICWFGM_CommonData* data;
	if (FAILED(grid->GetCommonData(nullptr, &data)))
		return;
	m_timeManager = data->m_timeManager;

	m_gridFilterCollection.AssignNewGrid(m_fuelMap.get(), m_gridEngine.get());
	m_vectorCollection.AssignNewGrid(m_gridEngine.get());
	m_assetCollection.AssignNewGrid(m_gridEngine.get());
	m_targetCollection.AssignNewGrid(m_gridEngine.get());
	m_fireCollection.AssignNewGrid(m_gridEngine.get());
	m_weatherCollection.AssignNewGrid(m_gridEngine.get());
	m_scenarioCollection.AssignNewGrid(oldgrid.get(), m_gridEngine.get());
	RecordUsedFuels();
}


void Project::CWFGMProject::GetLocation(double *latitude, double *longitude) const
{
	*latitude = m_timeManager->m_worldLocation.m_latitude();
	*longitude = m_timeManager->m_worldLocation.m_longitude();
}


bool Project::CWFGMProject::InsideCanada() const
{
	double l1, l2;
	GetLocation(&l1, &l2);
	return m_timeManager->m_worldLocation.InsideCanada();
}


bool Project::CWFGMProject::InsideNewZealand() const
{
	double l1, l2;
	GetLocation(&l1, &l2);
	return m_timeManager->m_worldLocation.InsideNewZealand();
}


bool Project::CWFGMProject::InsideTasmania() const
{
	double l1, l2;
	GetLocation(&l1, &l2);
	return m_timeManager->m_worldLocation.InsideTasmania();
}


bool Project::CWFGMProject::InsideAustraliaMainland() const
{
	double l1, l2;
	GetLocation(&l1, &l2);
	return m_timeManager->m_worldLocation.InsideAustraliaMainland();
}


HRESULT Project::CWFGMProject::GetDimensions(USHORT *xdim, USHORT *ydim) const
{
	return getDimensions(xdim, ydim);
}


HRESULT Project::CWFGMProject::GetResolution(double *resolution) const
{
	if (m_resolution == -1.0)
	{
		HRESULT hr;
		if (FAILED(hr = getResolution(&m_resolution)))
			return hr;
	}
	*resolution = m_resolution;
	return S_OK;
}


HRESULT Project::CWFGMProject::GetXLLCorner(double *xllCorner) const
{
	PolymorphicAttribute v;
	HRESULT hr = gridEngine()->GetAttribute(0, CWFGM_GRID_ATTRIBUTE_XLLCORNER, &v);
	if (SUCCEEDED(hr))
		VariantToDouble_(v, xllCorner);
	return hr;
}


HRESULT Project::CWFGMProject::GetYLLCorner(double *yllCorner) const
{
	PolymorphicAttribute v;
	HRESULT hr = gridEngine()->GetAttribute(0, CWFGM_GRID_ATTRIBUTE_YLLCORNER, &v);

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(hr))
		VariantToDouble_(v, yllCorner);
	return hr;
}


HRESULT Project::CWFGMProject::GetXURCorner(double *xurCorner) const
{
	PolymorphicAttribute v;
	HRESULT hr = gridEngine()->GetAttribute(0, CWFGM_GRID_ATTRIBUTE_XURCORNER, &v);

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(hr))
		VariantToDouble_(v, xurCorner);
	return hr;
}


HRESULT Project::CWFGMProject::GetYURCorner(double *yurCorner) const
{
	PolymorphicAttribute v;
	HRESULT hr = gridEngine()->GetAttribute(0, CWFGM_GRID_ATTRIBUTE_YURCORNER, &v);

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(hr))
		VariantToDouble_(v, yurCorner);
	return hr;
}


HRESULT Project::CWFGMProject::GetLLCorner(XY_Point *pt) const
{
	HRESULT hr = GetXLLCorner(&pt->x);
	if (SUCCEEDED(hr))
		hr = GetYLLCorner(&pt->y);
	return hr;
}


HRESULT Project::CWFGMProject::GetURCorner(XY_Point *pt) const
{
	HRESULT hr = GetXURCorner(&pt->x);
	if (SUCCEEDED(hr))
		hr = GetYURCorner(&pt->y);
	return hr;
}


HRESULT Project::CWFGMProject::GetExtents(XY_Point *ll, XY_Point *ur) const
{
	HRESULT hr = GetLLCorner(ll);
	if (SUCCEEDED(hr))
		hr = GetURCorner(ur);
	return hr;
}


USHORT Project::CWFGMProject::convertX(double x) const
{
	double xll, r;
	if (SUCCEEDED(GetXLLCorner(&xll)))
		if (SUCCEEDED(GetResolution(&r)))
			return static_cast<std::uint16_t>((x - xll) / r);
	weak_assert(0);
	return (USHORT)-1;
}


USHORT Project::CWFGMProject::convertY(double y) const
{
	double yll, r;
	if (SUCCEEDED(GetYLLCorner(&yll)))
		if (SUCCEEDED(GetResolution(&r)))
			return static_cast<std::uint16_t>((y - yll) / r);
	weak_assert(0);
	return (USHORT)-1;
}


HRESULT Project::CWFGMProject::convert(XY_Point* pt) const
{
	double r;
	XY_Point ll;
	if (SUCCEEDED(GetLLCorner(&ll)))
		if (SUCCEEDED(GetResolution(&r))) {
			ll.x = 0.0 - ll.x;
			ll.y = 0.0 - ll.y;
			pt->TranslateXY(ll);
			pt->ScaleXY(1.0 / r);
			return S_OK;
		}
	return E_FAIL;
}


double Project::CWFGMProject::invertX(double x) const
{
	double xll, r;
	if (SUCCEEDED(GetXLLCorner(&xll)))
		if (SUCCEEDED(GetResolution(&r)))
			return x * r + xll;
	weak_assert(0);
	return -1.0;
}


double Project::CWFGMProject::invertY(double y) const
{
	double yll, r;
	if (SUCCEEDED(GetYLLCorner(&yll)))
		if (SUCCEEDED(GetResolution(&r)))
			return y * r + yll;
	weak_assert(0);
	return -1.0;
}


HRESULT Project::CWFGMProject::invert(XY_Point *pt) const
{
	double r;
	XY_Point ll;
	if (SUCCEEDED(GetLLCorner(&ll)))
		if (SUCCEEDED(GetResolution(&r))) {
			pt->ScaleXY(r);
			pt->TranslateXY(ll);
			return S_OK;
		}
	return E_FAIL;
}


const ::TimeZoneInfo *Project::CWFGMProject::GuessTimeZone(SHORT set)
{
	return m_timeManager->m_worldLocation.GuessTimeZone(set);
}


WTimeSpan Project::CWFGMProject::GetTimeZone() const
{
	return m_timeManager->m_worldLocation.m_timezone();
}


std::uint32_t Project::CWFGMProject::GetTimeZoneId() const
{
	if (m_timeManager->m_worldLocation.m_timezoneInfo())
		return m_timeManager->m_worldLocation.m_timezoneInfo()->m_id;
	else
		return 0;
}


HRESULT Project::CWFGMProject::setLocation(double latitude, double longitude)
{
	HRESULT hr;
	PolymorphicAttribute v(latitude);
	if (FAILED(hr = m_grid->SetAttribute(CWFGM_GRID_ATTRIBUTE_LATITUDE, v)))	return hr;
	v = longitude;
	if (FAILED(hr = m_grid->SetAttribute(CWFGM_GRID_ATTRIBUTE_LONGITUDE, v)))	return hr;
	return S_OK;
}


HRESULT Project::CWFGMProject::SetTimeZone(std::uint32_t timezoneId)
{
	if (timezoneId == 0)
		return E_INVALIDARG;
	auto timezone = WorldLocation::TimeZoneFromId(timezoneId);
	if (!timezone)
		return E_INVALIDARG;

	HRESULT hr;
	PolymorphicAttribute var(timezoneId);
	WTimeSpan diff = m_timeManager->m_worldLocation.m_timezone() - timezone->m_timezone;
	WorldLocation& worldLocation = (WorldLocation&)(m_timeManager->m_worldLocation);
	worldLocation.SetTimeZoneOffset(timezoneId);

	WeatherStation* ws = m_weatherCollection.FirstStation();
	while (ws->LN_Succ())
	{
		WeatherStream* wss = ws->FirstStream();
		while (wss->LN_Succ())
		{
			ICWFGM_CommonData data;
			data.m_timeManager = m_timeManager;
			wss->put_CommonData(&data);
			wss = wss->LN_Succ();
		}
		ws = ws->LN_Succ();
	}

	adjustProjectTimes(diff);

	return S_OK;
}


HRESULT Project::CWFGMProject::SetTimeZoneTS(const WTimeSpan &Timezone)
{
	WTimeSpan diff = m_timeManager->m_worldLocation.m_timezone() - Timezone;
	PolymorphicAttribute var(Timezone);
	HRESULT hr;
	WorldLocation& worldLocation = (WorldLocation&)(m_timeManager->m_worldLocation);
	worldLocation.m_timezone(Timezone);

	WeatherStation *ws = m_weatherCollection.FirstStation();
	while (ws->LN_Succ())
	{
		WeatherStream *wss = ws->FirstStream();
		while (wss->LN_Succ())
		{
			ICWFGM_CommonData data;
			data.m_timeManager = m_timeManager;
			wss->put_CommonData(&data);
			wss = wss->LN_Succ();
		}
		ws = ws->LN_Succ();
	}

	adjustProjectTimes(diff);

	return S_OK;
}


void Project::CWFGMProject::adjustProjectTimes(const WTimeSpan &diff)
{
	if (diff.GetTotalSeconds())
	{
		WTime t((ULONGLONG)0, m_timeManager);

		Project::Scenario *s = m_scenarioCollection.FirstScenario();
		while (s->LN_Succ())
		{
			t = s->Simulation_StartTime();
			if (t.GetTime(0))
			{
				t += diff;
				s->Simulation_StartTime(t);
			}
			t = s->Simulation_EndTime();
			if (t.GetTime(0))
			{
				t += diff;
				s->Simulation_EndTime(t);
			}

			s = s->LN_Succ();
		}

		Fire *f = m_fireCollection.FirstFire();
		while (f->LN_Succ())
		{
			t = f->IgnitionTime();
			if (t.GetTime(0))
			{
				t += diff;
				f->IgnitionTime(t);
			}
			f = f->LN_Succ();
		}

		GridFilter *gf = m_gridFilterCollection.FirstFilter();
		while (gf->LN_Succ())
		{
			auto wgb = dynamic_cast<CWeatherGridBase*>(gf);
			if (wgb)
			{
				wgb->GetStartTime(t);
				if (t.GetTime(0))
				{
					t += diff;
					wgb->SetStartTime(t);
				}

				if (t.GetTime(0))
				{
					wgb->GetEndTime(t);
					t += diff;
					wgb->SetEndTime(t);
				}
			}
			gf = gf->LN_Succ();
		}
	}
}


bool Project::CWFGMProject::DaylightSavings(bool bUseDST)
{
	return _daylightSavings(bUseDST, false);
}


bool Project::CWFGMProject::_daylightSavings(bool bUseDST, bool oldSerialize)
{
	bool needs_changed = (bUseDST == (m_timeManager->m_worldLocation.m_endDST() != WTimeSpan(366, 0, 0, 0)));

	if ((needs_changed) || (oldSerialize))
	{
		WorldLocation& worldLocation = (WorldLocation&)m_timeManager->m_worldLocation;
		if (bUseDST)
		{
			worldLocation.m_startDST(WTimeSpan(0));
			worldLocation.m_endDST(WTimeSpan(366, 0, 0, 0));
		}
		else
		{
			worldLocation.m_startDST(WTimeSpan(0));
			worldLocation.m_endDST(WTimeSpan(0));
		}

		WeatherStation *ws = m_weatherCollection.FirstStation();
		while (ws->LN_Succ())
		{
			WeatherStream *wss = ws->FirstStream();
			while (wss->LN_Succ())
			{
				ICWFGM_CommonData data;
				data.m_timeManager = m_timeManager;
				wss->put_CommonData(&data);
				wss = wss->LN_Succ();
			}
			ws = ws->LN_Succ();
		}
	}

	if ((needs_changed) && (!oldSerialize))
	{
		WTimeSpan diff;
		if (!bUseDST)	diff = m_timeManager->m_worldLocation.m_amtDST();
		else			diff = WTimeSpan((LONGLONG)0) - m_timeManager->m_worldLocation.m_amtDST();
		adjustProjectTimes(diff);
	}

	return bUseDST;
}


bool Project::CWFGMProject::DaylightSavings() const
{
	return m_timeManager->m_worldLocation.dstExists();
}


std::string Project::CWFGMProject::GetAsciiGridHeader() const
{
	PolymorphicAttribute v;
	std::string header = "";

	if (SUCCEEDED(m_grid->GetAttribute(CWFGM_GRID_ATTRIBUTE_ASCII_GRIDFILE_HEADER, &v)))
	{
		std::string w = GetPolymorphicAttributeData(v, ""s);

		if (w.length())
			header = std::move(w);
	}

	return header;
}


void Project::CWFGMProject::ExportHeader(FILE *fp, USHORT *xSize, USHORT *ySize) const
{
	USHORT ncols, nrows;
	std::string header = GetAsciiGridHeader();
	//this seems like a bad idea
	const_cast<CWFGMProject*>(this)->GetDimensions(&ncols, &nrows);
	*xSize=ncols;
	*ySize=nrows;

	if (header.length())
	{
		_ftprintf(fp, _T("%s"), header.c_str());
	}
	else
	{
		double xllcorner,yllcorner;
		double resolution;
		//this seems like a bad idea
		const_cast<CWFGMProject*>(this)->GetResolution(&resolution);
		const_cast<CWFGMProject*>(this)->GetXLLCorner(&xllcorner);
		const_cast<CWFGMProject*>(this)->GetYLLCorner(&yllcorner);
		_ftprintf(fp,_T("ncols         %d\n"), ncols);
		_ftprintf(fp,_T("nrows         %d\n"), nrows);
		_ftprintf(fp,_T("xllcorner     %.3f\n"), xllcorner);
		_ftprintf(fp,_T("yllcorner     %.3f\n"), yllcorner);
		_ftprintf(fp,_T("cellsize      %d\n"), gsl::narrow_cast<int>(resolution));
		_ftprintf(fp,_T("NODATA_value  -9999\n"));
	}
}


bool Project::CWFGMProject::ExportLookupTable(const std::string &szName)
{
	if(szName.length() == 0)
		return false;
	USHORT result = m_fuelCollection.ExportFuelMap(szName.c_str());
	if (result == (USHORT)-1)
		return false;
	return true;
}


void Project::CWFGMProject::GetMinMaxElev(double *MinElev, double *MaxElev)
{
	PolymorphicAttribute v;
	grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_MIN_ELEVATION, &v);
	if (!v.index())
	{
		*MinElev = 0.0;
		*MaxElev = 0.0;
	}
	else
	{
		VariantToDouble_(v, MinElev);
		grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_MAX_ELEVATION, &v);
		VariantToDouble_(v, MaxElev);
	}
}

void Project::CWFGMProject::GetMinMaxSlope(double *MinSlope, double *MaxSlope)
{
	PolymorphicAttribute v;
	grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_MIN_SLOPE, &v);
	if (!v.index())
	{
		*MinSlope = 0.0;
		*MaxSlope = 0.0;
	}
	else
	{
		VariantToDouble_(v, MinSlope);
		grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_MAX_SLOPE, &v);
		VariantToDouble_(v, MaxSlope);
	}
}

void Project::CWFGMProject::GetMinMaxAzimuth(double *MinAzimuth, double *MaxAzimuth)
{
	PolymorphicAttribute v;
	grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_MIN_AZIMUTH, &v);
	if (!v.index())
	{
		*MinAzimuth = 0.0;
		*MaxAzimuth = 0.0;
	}
	else
	{
		VariantToDouble_(v, MinAzimuth);
		grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_MAX_AZIMUTH, &v);
		VariantToDouble_(v, MaxAzimuth);
	}
}

bool Project::CWFGMProject::GetLLByCoord(const double x, const double y, double *Lat, double *Lon)
{
	double lat, lon;
	lat = y; lon = x;
	return GetLLByAbsoluteCoord(lon, lat, Lat, Lon);
}


bool Project::CWFGMProject::GetLLByAbsoluteCoord(const double x, const double y, double* Lat, double* Lon)
{
	double lat, lon;
	lat = y; lon = x;
	bool result;
	m_CoorConverter.start()
		.fromPoints(lon, lat, 0.0)
		.asSource()
		.endInDegrees()
		.toPoints(Lon, Lat, &result);
	return result;
}


bool Project::CWFGMProject::GetXYByLL(const double Lat, const double Lon, double *X, double *Y)
{
	double lat, lon;
	bool result;
	m_CoorConverter.start()
		.fromPoints(Lon, Lat, 0.0)
		.asLatLon()
		.endInUTM()
		.toPoints(&lon, &lat, &result);
	*X = lon;
	*Y = lat;
	return result;
}

double Project::CWFGMProject::GetDefaultElevation()
{
	double elev;
	PolymorphicAttribute v;
	m_grid->GetAttribute(CWFGM_GRID_ATTRIBUTE_DEFAULT_ELEVATION, &v);
	VariantToDouble_(v, &elev);
	return elev;
}

bool Project::CWFGMProject::DEMPresent()
{
	bool elev;
	PolymorphicAttribute v;
	HRESULT hr = m_gridEngine->GetAttribute(0, CWFGM_GRID_ATTRIBUTE_DEM_PRESENT, &v);

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(hr))
		hr = VariantToBoolean_(v, &elev);
	if ((SUCCEEDED(hr)) && (elev))
		return true;
	return false;
}


void Project::CWFGMProject::RecordUsedFuels()
{
	XY_Point pt;
	USHORT x, y;
	USHORT sizeX, sizeY;
	m_gridEngine->GetDimensions(0, &sizeX,&sizeY);

	long import_index, export_index;
	ICWFGM_Fuel *fuel_ptr;

	for (UCHAR i = 0; i < 255; i++)
	{
		if (SUCCEEDED(m_fuelMap->FuelAtIndex(i, &import_index, &export_index, &fuel_ptr)))
		{
			if (fuel_ptr)
			{
				Fuel::FromCOM(fuel_ptr)->m_flags &= (~(Fuel::FLAG_USED));
			}
		}
	}

	for (x = 0; x < sizeX; x++)
		for (y = 0; y < sizeY; y++)
		{
			UCHAR oldFuel;
			bool oldFuelValid;
			WTime time(0, m_timeManager);
			pt.x = invertX((double)x + 0.5);
			pt.y = invertY((double)y + 0.5);
			m_gridEngine->GetFuelIndexData(0, pt,time,&oldFuel, &oldFuelValid, nullptr);
			if ((oldFuelValid) && SUCCEEDED(m_fuelMap->FuelAtIndex(oldFuel, &import_index, &export_index, &fuel_ptr)))
				if (fuel_ptr)
				{
					Fuel::FromCOM(fuel_ptr)->m_flags |= Fuel::FLAG_USED;
				}
		}

	GridFilter *gf = m_gridFilterCollection.FirstFilter();
	while (gf->LN_Succ())
	{
		auto rgb = dynamic_cast<ReplaceGridBase*>(gf);
		if (rgb)
		{
			ICWFGM_Fuel *from, *to;
			UCHAR from_i;
			if (SUCCEEDED(rgb->GetRelationship(&from, &from_i, &to)))
			{
				if (from && (from != (ICWFGM_Fuel *)~0) && (from != (ICWFGM_Fuel *)(-2)))
				{
					Fuel::FromCOM(from)->m_flags |= Fuel::FLAG_USED;
				}
				else if (from_i != (UCHAR)-1)
				{
					if (SUCCEEDED(m_fuelMap->FuelAtIndex(from_i, &import_index, &export_index, &to)))
						if (to)
						{
							Fuel::FromCOM(to)->m_flags |= Fuel::FLAG_USED;
						}
				}
				if (to)
				{
					Fuel::FromCOM(to)->m_flags |= Fuel::FLAG_USED;
				}
			}
		}
		gf = gf->LN_Succ();
	}
}


void Project::CWFGMProject::InitializeCoorConverter()
{
	PolymorphicAttribute v;
	m_grid->GetAttribute(CWFGM_GRID_ATTRIBUTE_SPATIALREFERENCE, &v);
	std::string csProject;

	/*POLYMORPHIC CHECK*/
	try { csProject = std::get<std::string>(v); } catch (std::bad_variant_access &) { weak_assert(false); return; };

	m_CoorConverter.SetSourceProjection(csProject.c_str());
}


//TODO cross platform
bool Project::CWFGMProject::ExportOperatingHistory(const TCHAR *szPath)
{
	FILE *fp=NULL;
	_tfopen_s(&fp, szPath, _T("w"));
	if(fp==NULL)
		return false;
	_fputts(m_comments.c_str(),fp);
	m_scenarioCollection.ExportScenarioInfo(fp);
	fclose(fp);
	return true;
}


bool Project::CWFGMProject::ExportGrid(const std::string &szPath, GDALExporter::CompressionType compression)
{
	return SUCCEEDED(m_grid->ExportGrid(szPath, (std::uint32_t)compression));
}


bool Project::CWFGMProject::ExportElevationGrid(const std::string &szPath)
{
	return SUCCEEDED(m_grid->ExportElevation(szPath));
}

bool Project::CWFGMProject::ExportSlopeGrid(const std::string &szPath)
{
	return SUCCEEDED(m_grid->ExportSlope(szPath));
}


bool Project::CWFGMProject::ExportAspectGrid(const std::string &szPath)
{
	return SUCCEEDED(m_grid->ExportAspect(szPath));
}


double Project::CWFGMProject::GetMeanElevation()
{
	double elev;
	PolymorphicAttribute v;
	HRESULT hr;
	hr = grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_MEAN_ELEVATION, &v);

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(hr))
		hr = VariantToDouble_(v, &elev);
	if (SUCCEEDED(hr))
		return elev;
	return 0.0;
}


double Project::CWFGMProject::GetMedianElevation()
{
	double elev;
	PolymorphicAttribute v;
	HRESULT hr;
	hr = grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_MEDIAN_ELEVATION, &v);

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(hr))
		hr = VariantToDouble_(v, &elev);
	if (SUCCEEDED(hr))
		return elev;
	return 0.0;
}


double Project::CWFGMProject::GetDefaultFMC(bool &active) const
{
	if (!grid())
	{
		active = false;
		return 120.0;
	}
	double fmc;
	PolymorphicAttribute v;
	HRESULT hr = grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_DEFAULT_FMC_ACTIVE, &v);

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(hr))
	{
	    hr = VariantToBoolean_(v, &active);
	}
	else
	{
		active = false;
	}
	hr = grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_DEFAULT_FMC, &v);
	if (SUCCEEDED(hr))
		hr = VariantToDouble_(v, &fmc);
	if (SUCCEEDED(hr))
		return fmc;
	return 120.0;
}


void Project::CWFGMProject::SetDefaultFMC(double fmc, bool active)
{
	grid()->SetAttribute(CWFGM_GRID_ATTRIBUTE_DEFAULT_FMC, fmc);
	grid()->SetAttribute(CWFGM_GRID_ATTRIBUTE_DEFAULT_FMC_ACTIVE, active);
}


struct export_parms_stat
{
	std::int16_t ParaOption;
	std::uint16_t statistic;
	double* d_array;
	bool* b_array;
	std::uint64_t units{ 0 };
};


struct alignas(8) export_parms
{
	export_parms(USHORT cnt)
	{
		stats_cnt = cnt;
		stats = new export_parms_stat[cnt];
	}
	~export_parms() { delete[] stats; }

	alignas(8) ULONG VOLATILE p_i;
	std::uint32_t p_s;
	Project::CWFGMProject *_this;
	const Project::Scenario *scenario;
	const WTime *mintime, *time;
	export_parms_stat* stats;
	std::uint16_t stats_cnt;
	std::uint16_t xSize, ySize;
	std::uint32_t interp_method;
	std::uint16_t discretization;
	XY_Rectangle bounds;
	std::uint64_t wx_method;
	export_iteration ei_fcn;
	APTR parm;
	bool VOLATILE aborted;
	bool explicitNodata;
};


static UINT AFX_CDECL stepBuildGrid(APTR parameter)
{
	try {
		export_parms *ep = (export_parms *)parameter;

	std::vector<XYStat> safe_stat(ep->stats_cnt);
	for (std::uint16_t k = 0; k < ep->stats_cnt; k++) {
			XYStat s;
			s.stat = ep->stats[k].statistic;
		safe_stat[k] = s;
		}

		while (true)
		{
			ULONG p_i = InterlockedIncrement(&ep->p_i);
			if ((p_i >= ep->p_s) || (ep->aborted))
				return 1;

			if (ep->ei_fcn)
				if (!ep->ei_fcn(ep->parm, p_i))
				{
					ep->aborted = true;
					return 1;
				}
			XY_Point pt, pt2, ptm;
			pt.x = (double)(p_i % ep->xSize);
			pt.y = (double)(p_i / ep->xSize);

			if ((ep->stats_cnt == 1) && ((ep->stats->ParaOption == PARA_BURNT) || (ep->stats->ParaOption == PARA_BURNT_MEAN))) {
				ptm.x = pt.x + 0.5;
				ptm.y = pt.y + 0.5;
				ptm.x = ep->_this->invertX(ptm.x);
				ptm.y = ep->_this->invertY(ptm.y);
			}
			else {
				pt2.x = pt.x + 1.0;
				pt2.y = pt.y + 1.0;
				pt2.x = ep->_this->invertX(pt2.x);
				pt2.y = ep->_this->invertY(pt2.y);
			}

			pt.x = ep->_this->invertX(pt.x);
			pt.y = ep->_this->invertY(pt.y);

			double value;

			if (ep->stats_cnt == 1)
			{
				if ((ep->stats->ParaOption == PARA_BURNT) || (ep->stats->ParaOption == PARA_BURNT_MEAN))
				{
					short burned;
					if (!ep->bounds.PointInside(pt))
						burned = 0;
					else
					{
						bool b;
						WTime time(*ep->time);
						if (FAILED(ep->scenario->IsXYBurned(ptm, time, b)))
							burned = 0;
						else
							burned = b ? 1 : 0;
					}
					if (ep->stats->ParaOption == PARA_BURNT)
						ep->stats->b_array[p_i] = burned ? true : false;
					else {
						if (ep->explicitNodata)
							ep->stats->d_array[p_i] = burned ? burned : -9999.0;
						else
							ep->stats->d_array[p_i] = burned;
					}
				}
				else if (((ep->stats->ParaOption > 9) && (ep->stats->ParaOption < 200)) || (ep->stats->ParaOption < 0))
				{
					if ((ep->stats->ParaOption >= 0) &&
						(!ep->bounds.PointInside(pt))) {
						if ((ep->stats->ParaOption == PARA_ARRIVAL_TIME) ||
							(ep->stats->ParaOption == PARA_MIN_ARRIVAL_TIME) ||
							(ep->stats->ParaOption == PARA_MAX_ARRIVAL_TIME))
							value = -9999.0;
					else if (ep->explicitNodata)
						value = -9999.0;
						else
							value = 0.0;
					}
					else {
						NumericVariant vresult;
						WTime mintime(*ep->mintime);
						WTime time(*ep->time);
						HRESULT Burned = ep->scenario->GetXYStats(pt, pt2, mintime, time, ep->stats->statistic, ep->interp_method, ep->discretization, &vresult);

						if (FAILED(Burned))
						{
							if ((ep->stats->ParaOption == PARA_ARRIVAL_TIME) ||
								(ep->stats->ParaOption == PARA_MIN_ARRIVAL_TIME) ||
								(ep->stats->ParaOption == PARA_MAX_ARRIVAL_TIME))
								value = -9999.0;
						else if (ep->explicitNodata)
							value = -9999.0;
							else
								value = 0.0;
						}
						else
						{
							if (!vresult.index()) {
								value = -9999.0;
							}
							else if (!variantToDouble(vresult, &value)) {
								bool bv;
								bool hr2 = variantToBoolean(vresult, &bv);
								weak_assert(hr2);
								value = bv ? 1.0 : 0.0;
							}

							if ((ep->stats->ParaOption == PARA_RAZ) ||
								(ep->stats->ParaOption == PARA_FBP_RAZ_MAP) ||
								(ep->stats->ParaOption == CWFGM_FIRE_STAT_DIRECTIONVECTOR))
							{
								if (value == 0.0)
									value = Constants::TwoPi<double>();
							}
						}
					}

					if (value != -9999.0) {
						if (ep->stats->ParaOption == PARA_CROWN_FUEL_CONSUMED || ep->stats->ParaOption == PARA_SURFACE_FUEL_CONSUMED ||
							ep->stats->ParaOption == PARA_TOTAL_FUEL_CONSUMED || ep->stats->ParaOption == PARA_FUEL_LOAD_MAP || ep->stats->ParaOption == PARA_CFL_MAP)
							ep->stats->d_array[p_i] = ::UnitConversion::ConvertUnit(value, STORAGE_FORMAT_KG, ep->stats->units);
						else if (ep->stats->ParaOption == PARA_RADIATIVE_POWER)
							ep->stats->d_array[p_i] = ::UnitConversion::ConvertUnit(value, STORAGE_FORMAT_KILOJOULE | STORAGE_FORMAT_SECOND, ep->stats->units);
						else if (ep->stats->ParaOption == PARA_FBP_BROS || ep->stats->ParaOption == PARA_FBP_FROS ||
							ep->stats->ParaOption == PARA_FBP_HROS || ep->stats->ParaOption == PARA_ROS)
							ep->stats->d_array[p_i] = ::UnitConversion::ConvertUnit(value, STORAGE_FORMAT_M | STORAGE_FORMAT_MINUTE, ep->stats->units);
						else if (ep->stats->ParaOption == PARA_TFC || ep->stats->ParaOption == PARA_CFC ||
							ep->stats->ParaOption == PARA_SFC)
							ep->stats->d_array[p_i] = ::UnitConversion::ConvertUnit(value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KG << 0x20) | STORAGE_FORMAT_M2, ep->stats->units);
						else if (ep->stats->ParaOption == PARA_FLAMELENGTH)
							ep->stats->d_array[p_i] = ::UnitConversion::ConvertUnit(value, STORAGE_FORMAT_M, ep->stats->units);
						else if (ep->stats->ParaOption == PARA_FI)
							ep->stats->d_array[p_i] = ::UnitConversion::ConvertUnit(value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KILOWATT_SECOND << 0x20) | STORAGE_FORMAT_M, ep->stats->units);
						else if (ep->stats->ParaOption == PARA_PC_MAP || ep->stats->ParaOption == PARA_PDF_MAP || ep->stats->ParaOption == PARA_CURINGDEGREE_MAP ||
							ep->stats->ParaOption == PARA_BURNED_CHANGE || ep->stats->ParaOption == PARA_BURNED)
							ep->stats->d_array[p_i] = value * 100.0;
						else if (ep->stats->ParaOption == PARA_TREE_HEIGHT_MAP || ep->stats->ParaOption == PARA_CBH_MAP)
							ep->stats->d_array[p_i] = UnitConversion::ConvertUnit(value, STORAGE_FORMAT_M, ep->stats->units);
						else
							ep->stats->d_array[p_i] = value;
					}
					else
						ep->stats->d_array[p_i] = value;
				}
				else
				{
					ICWFGM_GridEngine* ge = ep->scenario->gridEngine();
					weak_assert(ge);

					IWXData wx;
					IFWIData ifwi;
					DFWIData dfwi;
					bool wx_valid = false;
					std::uint64_t startUnits = 0;
					XY_Rectangle cacheBounds;
					if (SUCCEEDED(ge->GetWeatherData(ep->scenario->layerThread(), pt, *ep->time, ep->wx_method, &wx, &ifwi, &dfwi, &wx_valid, &cacheBounds)))
					{
						if (wx_valid)
						{
							switch (ep->stats->ParaOption)
							{
							case PARA_TEMP:
								value = wx.Temperature;
								startUnits = STORAGE_FORMAT_CELSIUS;
								break;
							case PARA_DEW:
								value = wx.DewPointTemperature;
								startUnits = STORAGE_FORMAT_CELSIUS;
								break;
							case PARA_RH:
								value = wx.RH * 100.0;
								startUnits = 0;
								break;
							case PARA_WD:
								value = CARTESIAN_TO_COMPASS_DEGREE(RADIAN_TO_DEGREE(wx.WindDirection));
								if (value == 0.0) value = 360.0;
								startUnits = 0;
								break;
							case PARA_WS:
								value = std::max(0.0, min(200.0, wx.WindSpeed));
								startUnits = STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR;
								break;
							case PARA_GUST:
								value = std::max(0.0, min(200.0, wx.WindGust));
								startUnits = STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR;
								break;
							case PARA_PRECIP:
								value = wx.Precipitation;
								startUnits = STORAGE_FORMAT_MM;
								break;
							case PARA_FFMC:
								value = ifwi.FFMC;
								startUnits = 0;
								break;
							case PARA_ISI:
								value = ifwi.ISI;
								startUnits = 0;
								break;
							case PARA_FWI:
								value = ifwi.FWI;
								startUnits = 0;
								break;
							case PARA_SBUI:
								value = dfwi.dBUI;
								startUnits = 0;
								break;
							}
						}
						else
							value = -1.0;
					}
					else
						value = -1.0;
					ep->stats->d_array[p_i] = ::UnitConversion::ConvertUnit(value, startUnits, ep->stats->units);
				}
			}
			else {
				if (((ep->stats->ParaOption > 9) && (ep->stats->ParaOption < 200)) || (ep->stats->ParaOption < 0))
				{
				std::uint16_t k;
					if (!ep->bounds.PointInside(pt))
					{
						for (k = 0; k < ep->stats_cnt; k++) {
							if ((ep->stats[k].ParaOption == PARA_ARRIVAL_TIME) ||
								(ep->stats[k].ParaOption == PARA_MIN_ARRIVAL_TIME) ||
								(ep->stats[k].ParaOption == PARA_MAX_ARRIVAL_TIME))
								ep->stats[k].d_array[p_i] = -9999.0;
						else if (ep->explicitNodata)
							ep->stats[k].d_array[p_i] = -9999.0;
							else
								ep->stats[k].d_array[p_i] = 0.0;
						}
					}
					else {
						NumericVariant vresult;
						WTime mintime(*ep->mintime);
						WTime time(*ep->time);
					HRESULT Burned = ep->scenario->GetXYStatsSet(pt, pt2, mintime, time, ep->interp_method, ep->discretization, safe_stat);
						if (FAILED(Burned))
						{
							for (k = 0; k < ep->stats_cnt; k++) {
								if ((ep->stats[k].ParaOption == PARA_ARRIVAL_TIME) ||
									(ep->stats[k].ParaOption == PARA_MIN_ARRIVAL_TIME) ||
									(ep->stats[k].ParaOption == PARA_MAX_ARRIVAL_TIME))
									ep->stats[k].d_array[p_i] = -9999.0;
							else if (ep->explicitNodata)
								ep->stats[k].d_array[p_i] = -9999.0;
								else
									ep->stats[k].d_array[p_i] = 0.0;
							}
						}
						else {
							for (k = 0; k < ep->stats_cnt; k++)
							{
								vresult = safe_stat[k].value;
								if (!vresult.index()) {
									value = -9999.0;
								}
								else if (!variantToDouble(vresult, &value)) {
									bool bv;
									bool hr2 = variantToBoolean(vresult, &bv);
									weak_assert(hr2);
									value = bv ? 1.0 : 0.0;
								}
								if ((ep->stats[k].ParaOption == PARA_RAZ) ||
									(ep->stats[k].ParaOption == PARA_FBP_RAZ_MAP) ||
									(ep->stats[k].ParaOption == CWFGM_FIRE_STAT_DIRECTIONVECTOR))
								{
									if (value == 0.0)
										value = Constants::TwoPi<double>();
								}
								if (value != -9999.0) {
									if (ep->stats[k].ParaOption == PARA_CROWN_FUEL_CONSUMED || ep->stats[k].ParaOption == PARA_SURFACE_FUEL_CONSUMED ||
										ep->stats[k].ParaOption == PARA_CROWN_FUEL_CONSUMED || ep->stats->ParaOption == PARA_FUEL_LOAD_MAP || ep->stats->ParaOption == PARA_CFL_MAP)
										ep->stats[k].d_array[p_i] = ::UnitConversion::ConvertUnit(value, STORAGE_FORMAT_KG, ep->stats[k].units);
									else if (ep->stats[k].ParaOption == PARA_RADIATIVE_POWER)
										ep->stats[k].d_array[p_i] = ::UnitConversion::ConvertUnit(value, STORAGE_FORMAT_KILOJOULE | STORAGE_FORMAT_SECOND, ep->stats[k].units);
									else if (ep->stats[k].ParaOption == PARA_FBP_BROS || ep->stats[k].ParaOption == PARA_FBP_FROS ||
										ep->stats[k].ParaOption == PARA_FBP_HROS || ep->stats[k].ParaOption == PARA_ROS)
										ep->stats[k].d_array[p_i] = ::UnitConversion::ConvertUnit(value, STORAGE_FORMAT_M | STORAGE_FORMAT_MINUTE, ep->stats[k].units);
									else if (ep->stats[k].ParaOption == PARA_TFC || ep->stats[k].ParaOption == PARA_CFC ||
										ep->stats[k].ParaOption == PARA_SFC)
										ep->stats[k].d_array[p_i] = ::UnitConversion::ConvertUnit(value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KG << 0x20) | STORAGE_FORMAT_M2, ep->stats[k].units);
									else if (ep->stats[k].ParaOption == PARA_FLAMELENGTH)
										ep->stats[k].d_array[p_i] = ::UnitConversion::ConvertUnit(value, STORAGE_FORMAT_M, ep->stats[k].units);
									else if (ep->stats[k].ParaOption == PARA_FI)
										ep->stats[k].d_array[p_i] = ::UnitConversion::ConvertUnit(value, ((UnitConvert::STORAGE_UNIT)STORAGE_FORMAT_KILOWATT_SECOND << 0x20) | STORAGE_FORMAT_M, ep->stats[k].units);
									else if (ep->stats->ParaOption == PARA_PC_MAP || ep->stats->ParaOption == PARA_PDF_MAP || ep->stats->ParaOption == PARA_CURINGDEGREE_MAP ||
										ep->stats->ParaOption == PARA_BURNED_CHANGE || ep->stats->ParaOption == PARA_BURNED)
										ep->stats->d_array[p_i] = value * 100.0;
									else if (ep->stats->ParaOption == PARA_TREE_HEIGHT_MAP || ep->stats->ParaOption == PARA_CBH_MAP)
										ep->stats->d_array[p_i] = UnitConversion::ConvertUnit(value, STORAGE_FORMAT_M, ep->stats[k].units);
									else
										ep->stats[k].d_array[p_i] = value;
								}
								else
									ep->stats[k].d_array[p_i] = value;
							}
						}
					}
				}
				else
				{
					ICWFGM_GridEngine* ge = ep->scenario->gridEngine();
					weak_assert(ge);

					IWXData wx;
					IFWIData ifwi;
					DFWIData dfwi;
					std::uint64_t startUnits = 0;
				for (std::uint16_t k = 0; k < ep->stats_cnt; k++)
					{
						XY_Rectangle cacheBounds;
						bool wx_valid = false;
						if (SUCCEEDED(ge->GetWeatherData(ep->scenario->layerThread(), pt, *ep->time, ep->wx_method, &wx, &ifwi, &dfwi, &wx_valid, &cacheBounds)))
						{
							if (wx_valid)
							{
								switch (ep->stats[k].ParaOption)
								{
								case PARA_TEMP:
									value = wx.Temperature;
									startUnits = STORAGE_FORMAT_CELSIUS;
									break;
								case PARA_DEW:
									value = wx.DewPointTemperature;
									startUnits = STORAGE_FORMAT_CELSIUS;
									break;
								case PARA_RH:
									value = wx.RH * 100.0;
									startUnits = 0;
									break;
								case PARA_WD:
									value = CARTESIAN_TO_COMPASS_DEGREE(RADIAN_TO_DEGREE(wx.WindDirection));
									if (value == 0.0) value = 360.0;
									startUnits = 0;
									break;
								case PARA_WS:
									value = std::max(0.0, min(200.0, wx.WindSpeed));
									startUnits = STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR;
									break;
								case PARA_GUST:
									value = std::max(0.0, min(200.0, wx.WindGust));
									startUnits = STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR;
									break;
								case PARA_PRECIP:
									value = wx.Precipitation;
									startUnits = STORAGE_FORMAT_MM;
									break;
								case PARA_FFMC:
									value = ifwi.FFMC;
									startUnits = 0;
									break;
								case PARA_ISI:
									value = ifwi.ISI;
									startUnits = 0;
									break;
								case PARA_FWI:
									value = ifwi.FWI;
									startUnits = 0;
									break;
								case PARA_SBUI:
									value = dfwi.dBUI;
									startUnits = 0;
									break;
								}
							}
							else
								value = -1.0;
						}
						else
							value = -1.0;
						ep->stats[k].d_array[p_i] = ::UnitConversion::ConvertUnit(value, startUnits, ep->stats->units);
					}
				}
			}
		}
	}
	catch (std::exception& except) {
		fprintf(stderr, "stepBuildGrid: Exception %s\n", except.what());
	}
}


#include <gdal.h>


template<class type>
void flip(type *buffer, std::uint16_t xSize, std::uint16_t ySize)
{
	auto rows = ySize / 2;
	auto* tempRow = new type[xSize];

	for (auto rowIndex = 0; rowIndex < rows; rowIndex++)
	{
		memcpy(tempRow, buffer + rowIndex * xSize, xSize * sizeof(type));
		memcpy(buffer + rowIndex * xSize, buffer + (ySize - rowIndex - 1) * xSize, xSize * sizeof(type));
		memcpy(buffer + (ySize - rowIndex - 1) * xSize, tempRow, xSize * sizeof(type));
	}
	
	delete [] tempRow;
}


template<class type>
void flip(type *buffer, type *tempRow, std::uint16_t xSize, std::uint16_t ySize)
{
	auto rows = ySize / 2;

	for (auto rowIndex = 0; rowIndex < rows; rowIndex++)
	{
		memcpy(tempRow, buffer + rowIndex * xSize, xSize * sizeof(type));
		memcpy(buffer + rowIndex * xSize, buffer + (ySize - rowIndex - 1) * xSize, xSize * sizeof(type));
		memcpy(buffer + (ySize - rowIndex - 1) * xSize, tempRow, xSize * sizeof(type));
	}
}


template<typename T>
bool Project::CWFGMProject::ExportParameterGrid(std::vector<T*> &arr, const Scenario *scenario, const Asset* asset, const ULONG asset_index, const Project_XYStatOptions& options,
    std::vector<std::int16_t> const &ParaOption, bool explicitNodata, USHORT &left, USHORT &right, USHORT &bottom, USHORT &top,
	export_setsize ess_fcn, export_iteration ei_fcn, APTR parm, ::UnitConversion* const unitConversion) {
    static_assert(std::is_same<T, double>::value || std::is_same<T, bool>::value, "Invalid export type.");

    if (!GetPool())
        return false;
	if (arr.size() != ParaOption.size())
		return false;

#ifdef _DEBUG
	if (scenario->m_parent)
	{
		for (USHORT k = 0; k < arr.size(); k++)
			weak_assert((ParaOption[k] != PARA_BURNT) && (ParaOption[k] != PARA_CRITICAL_PATH));
	}
#endif

    ULONG interp_method;
    export_parms ep(arr.size());
    ULONG wx_method = 0;
    std::uint16_t xSize, ySize;
	ep._this = this;
	ep.explicitNodata = explicitNodata;
    GetDimensions(&xSize, &ySize);

    switch (options.method)
	{
		case 0:		interp_method = SCENARIO_XYSTAT_TECHNIQUE_IDW; break;
		case 1:		interp_method = SCENARIO_XYSTAT_TECHNIQUE_AREA_WEIGHTING; break;
		case 2:		interp_method = SCENARIO_XYSTAT_TECHNIQUE_CLOSEST_VERTEX; break;
		case 3:		interp_method = SCENARIO_XYSTAT_TECHNIQUE_DISCRETIZE; break;
		case 4:		interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE; break;
		case 5:		interp_method = SCENARIO_XYSTAT_TECHNIQUE_VORONOI_OVERLAP; break;
		default:	weak_assert(0);
			        return FALSE;
    }

	for (USHORT k = 0; k < arr.size(); k++)
	{
		if constexpr (std::is_same<T, double>::value)
			ep.stats[k].d_array = arr[k];
		else if constexpr (std::is_same<T, bool>::value)
			ep.stats[k].b_array = arr[k];
		ep.stats[k].ParaOption = ParaOption[k];
		if ((ParaOption[k] > 9) || (ParaOption[k] < 0))
		{
			switch (ParaOption[k])
			{
			case PARA_FI:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FI;
				ep.stats[k].units = unitConversion ? unitConversion->IntensityDisplay() : 0;
				break;
			case PARA_HFI:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_HFI;
				break;
			case PARA_HCFB:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_HCFB;
				break;
			case PARA_FLAMELENGTH:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FLAMELENGTH;
				ep.stats[k].units = unitConversion ? unitConversion->SmallDistanceDisplay() : 0;
				break;
			case PARA_FBP_FMC_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FMC;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_FBP_CFB_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_CFB;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_FBP_CFC_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_CFC;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_FBP_SFC_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_SFC;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_FBP_TFC_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_TFC;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_FBP_FI_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FI;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_FBP_FLAMELENGTH_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FLAMELENGTH;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_CURINGDEGREE_MAP:
				ep.stats[k].statistic = FUELCOM_ATTRIBUTE_CURINGDEGREE;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_GREENUP_MAP:
				ep.stats[k].statistic = CWFGM_SCENARIO_OPTION_GREENUP;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_PC_MAP:
				ep.stats[k].statistic = FUELCOM_ATTRIBUTE_PC;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_PDF_MAP:
				ep.stats[k].statistic = FUELCOM_ATTRIBUTE_PDF;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_CBH_MAP:
				ep.stats[k].statistic = FUELCOM_ATTRIBUTE_CBH;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				ep.stats[k].units = unitConversion ? unitConversion->SmallDistanceDisplay() : 0;
				break;
			case PARA_TREE_HEIGHT_MAP:
				ep.stats[k].statistic = FUELCOM_ATTRIBUTE_TREE_HEIGHT;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				ep.stats[k].units = unitConversion ? unitConversion->SmallDistanceDisplay() : 0;
				break;
			case PARA_FUEL_LOAD_MAP:
				ep.stats[k].statistic = FUELCOM_ATTRIBUTE_FUELLOAD;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				ep.stats[k].units = unitConversion ? unitConversion->MassDisplay() : 0;
				break;
			case PARA_CFL_MAP:
				ep.stats[k].statistic = FUELCOM_ATTRIBUTE_CFL;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				ep.stats[k].units = unitConversion ? unitConversion->MassDisplay() : 0;
				break;
			case PARA_GRASSPHENOLOGY_MAP:
				ep.stats[k].statistic = CWFGM_SCENARIO_OPTION_GRASSPHENOLOGY;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_ROSVECTOR_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_ROSVECTOR;
				ep.stats[k].units = unitConversion ? unitConversion->AltVelocityDisplay() : 0;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_DIRVECTOR_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_DIRECTIONVECTOR;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_ROS:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_ROS;
				ep.stats[k].units = unitConversion ? unitConversion->AltVelocityDisplay() : 0;
				break;
			case PARA_SFC:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_SFC;
				ep.stats[k].units = unitConversion ? unitConversion->MassAreaDisplay() : 0;
				break;
			case PARA_CFC:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_CFC;
				ep.stats[k].units = unitConversion ? unitConversion->MassAreaDisplay() : 0;
				break;
			case PARA_TFC:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_TFC;
				ep.stats[k].units = unitConversion ? unitConversion->MassAreaDisplay() : 0;
				break;
			case PARA_CFB:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_CFB;
				break;
			case PARA_RAZ:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_RAZ;
				break;
			case PARA_FBP_HROS:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FBP_ROS;
				ep.stats[k].units = unitConversion ? unitConversion->AltVelocityDisplay() : 0;
				break;
			case PARA_FBP_FROS:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FBP_FROS;
				ep.stats[k].units = unitConversion ? unitConversion->AltVelocityDisplay() : 0;
				break;
			case PARA_FBP_BROS:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FBP_BROS;
				ep.stats[k].units = unitConversion ? unitConversion->AltVelocityDisplay() : 0;
				break;
			case PARA_FBP_RSS:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FBP_RSI;
				break;
			case PARA_FBP_HROS_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FBP_ROS;
				ep.stats[k].units = unitConversion ? unitConversion->AltVelocityDisplay() : 0;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_FBP_FROS_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FBP_FROS;
				ep.stats[k].units = unitConversion ? unitConversion->AltVelocityDisplay() : 0;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_FBP_BROS_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FBP_BROS;
				ep.stats[k].units = unitConversion ? unitConversion->AltVelocityDisplay() : 0;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_FBP_RSS_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_FBP_RSI;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_FBP_RAZ_MAP:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_RAZ;
				interp_method = SCENARIO_XYSTAT_TECHNIQUE_CALCULATE | (1 << CWFGM_SCENARIO_OPTION_WEATHER_IGNORE_CACHE);
				break;
			case PARA_ARRIVAL_TIME:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_TIME;
				break;
			case PARA_ACTIVE:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_ACTIVE;
				break;
			case PARA_BURNED:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_BURNED;
				break;
			case PARA_BURNED_CHANGE:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_BURNED_CHANGE;
				break;
			case PARA_TOTAL_FUEL_CONSUMED:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_TOTAL_FUEL_CONSUMED;
				ep.stats[k].units = unitConversion ? unitConversion->MassDisplay() : 0;
				break;
			case PARA_SURFACE_FUEL_CONSUMED:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_SURFACE_FUEL_CONSUMED;
				ep.stats[k].units = unitConversion ? unitConversion->MassDisplay() : 0;
				break;
			case PARA_CROWN_FUEL_CONSUMED:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_CROWN_FUEL_CONSUMED;
				ep.stats[k].units = unitConversion ? unitConversion->MassDisplay() : 0;
				break;
			case PARA_RADIATIVE_POWER:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_RADIATIVE_POWER;
				ep.stats[k].units = unitConversion ? unitConversion->PowerDisplay() : 0;
				break;
			case PARA_MIN_ARRIVAL_TIME:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_ENTRY_TIME;
				break;
			case PARA_MAX_ARRIVAL_TIME:
				ep.stats[k].statistic = CWFGM_FIRE_STAT_EXIT_TIME;
				break;
			case PARA_BURNT:
			case PARA_BURNT_MEAN:
			case PARA_CRITICAL_PATH:
			case PARA_CRITICAL_PATH_MEAN:
				break;
			default:
				weak_assert(0);
				return false;
			}
		}
		else
		{
			switch (ParaOption[k])
			{
			case PARA_TEMP:
			case PARA_DEW:
				ep.stats[k].units = unitConversion ? unitConversion->TempDisplay() : 0;
				break;
			case PARA_WS:
			case PARA_GUST:
				ep.stats[k].units = unitConversion ? unitConversion->VelocityDisplay() : 0;
				break;
			case PARA_PRECIP:
				ep.stats[k].units = unitConversion ? unitConversion->SmallMeasureDisplay() : 0;
				break;
			}
			BasicScenarioOptions bso = scenario->GetBasicOptions();
			if (bso.m_bWeatherTemporalInterpolate)		wx_method |= CWFGM_GETWEATHER_INTERPOLATE_TEMPORAL;
			if (bso.m_bWeatherSpatialInterpolate)		wx_method |= CWFGM_GETWEATHER_INTERPOLATE_SPATIAL;
			if (bso.m_bPrecipitationInterpolation)		wx_method |= CWFGM_GETWEATHER_INTERPOLATE_PRECIP;
			if (bso.m_bWindInterpolation)				wx_method |= CWFGM_GETWEATHER_INTERPOLATE_WIND;
			if (bso.m_bWindInterpolationVector)			wx_method |= CWFGM_GETWEATHER_INTERPOLATE_WIND_VECTOR;
			if (bso.m_bTempRHInterpolation)				wx_method |= CWFGM_GETWEATHER_INTERPOLATE_TEMP_RH;
			if (bso.m_bHistoryInterpolation)			wx_method |= CWFGM_GETWEATHER_INTERPOLATE_HISTORY;
			if (bso.m_bCalcSpatialFWI)					wx_method |= CWFGM_GETWEATHER_INTERPOLATE_CALCFWI;
		}
	}

    {
        CRWThreadSemaphoreEngage lock(m_poolLock, TRUE);

        if ((interp_method & 0x0fffffff) == SCENARIO_XYSTAT_TECHNIQUE_CALCULATE)
		{
            WTime start(scenario->Simulation_StartTime()),
                end(scenario->Simulation_EndTime());
            scenario->gridEngine()->Valid(scenario->layerThread(), start, (end - start), static_cast<std::uint32_t>(1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE), nullptr);
            wx_method |= (1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE);
        }

		HRESULT exists = E_FAIL;
		if (CWFGMProject::m_appMode < 0)
			exists = scenario->GetWeatherGrid()->SetCache(scenario->layerThread(), 1, true);
        scenario->gridEngine()->PreCalculationEvent(scenario->layerThread(), *options.time, static_cast<std::uint32_t>(1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE), nullptr);
        scenario->gridEngine()->PreCalculationEvent(scenario->layerThread(), *options.time, static_cast<std::uint32_t>(1 | (1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE)), nullptr);

		Vector* ven = scenario->VectorAtIndex(0);
		if (ven)
		{
			while (ven->LN_Succ())
			{
				ven->m_filter->PreCalculationEvent(*options.time, static_cast<std::uint32_t>(1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE), nullptr);
				ven->m_filter->PreCalculationEvent(*options.time, static_cast<std::uint32_t>(1 | (1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE)), nullptr);
				ven = ven->LN_Succ();
			}
		}
		Asset* an = scenario->AssetAtIndex(0);
		if (an)
		{
			while (an->LN_Succ())
			{
				an->m_filter->PreCalculationEvent(*options.time, (ULONG)(1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE), nullptr);
				an->m_filter->PreCalculationEvent(*options.time, (ULONG)(1 | (1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE)), nullptr);
				an = an->LN_Succ();
			}
		}

        WTime time((std::uint64_t)0, m_timeManager);

        HRESULT hr;
		if (ParaOption[0] > 9)
		{
			XY_Rectangle rect;
			hr = scenario->GetBurnedBox(*options.time, &rect);
			left = convertX(rect.m_min.x);	// convert from meters to grid units
			right = convertX(rect.m_max.x);
			top = convertY(rect.m_max.y);
			bottom = convertY(rect.m_min.y);
		} else {
            hr = S_OK;
            left = 0;
            bottom = 0;
            right = xSize - 1;
            top = ySize - 1;
        }

		std::uint32_t p_s;
		if (SUCCEEDED(hr))
			p_s = (std::uint32_t)ySize * (std::uint32_t)xSize;
		else
			p_s = 0;

        if (ess_fcn)
            if (!ess_fcn(parm, p_s))
                return false;

		if (p_s > 0)
		{
			ep.aborted = false;
			if ((ParaOption[0] > 9) && FAILED(hr)) {
				for (std::uint32_t p_i = 0; p_i < p_s; p_i++) {
					if constexpr (std::is_same<T, double>::value) {
						if (explicitNodata)
							ep.stats[0].d_array[p_i] = -9999.0;
						else
							ep.stats[0].d_array[p_i] = 0;
					}
					else if constexpr (std::is_same<T, bool>::value)
						ep.stats[0].b_array[p_i] = false;
					else
						static_assert(!std::is_same<T, double>::value && !std::is_same<T, bool>::value);
				}
			}
			else if ((ParaOption[0] == PARA_CRITICAL_PATH) || (ParaOption[0] == PARA_CRITICAL_PATH_MEAN)) {
				MinListTempl<CriticalPath> polyset;

				for (std::uint32_t p_i = 0; p_i < p_s; p_i++) {
					if (ParaOption[0] == PARA_CRITICAL_PATH) {
						if (ep.stats[0].b_array)
							ep.stats[0].b_array[p_i] = false;
					}
					else {
						if (ep.stats[0].d_array) {
							if (explicitNodata)
								ep.stats[0].d_array[p_i] = -9999.0;
							else
								ep.stats[0].d_array[p_i] = 0.0;
						}
					}
				}

				if (SUCCEEDED(scenario->BuildCriticalPath(asset, asset_index, 2, polyset)) && (polyset.GetCount())) {
					CriticalPath* poly = polyset.LH_Head();
					bool bounds_set = false;
					XY_Rectangle bounds;
					poly = polyset.LH_Head();
					while (poly->LN_Succ()) {
						CriticalPathPoint* p1 = (CriticalPathPoint*)poly->LH_Head();
						while (p1->LN_Succ()) {
							CriticalPathPointData* ppd = (CriticalPathPointData*)p1->LH_Head();
							while (ppd->LN_Succ()) {
								if (!bounds_set) {
									bounds.m_max = bounds.m_min = *ppd;
									bounds_set = true;
								}
								else {
									bounds.EncompassPoint(*ppd);
								}
								ppd = ppd->LN_Succ();
							}
							p1 = p1->LN_Succ();
						}
						poly = poly->LN_Succ();
					}
					left = (USHORT)floor(bounds.m_min.x);
					right = (USHORT)ceil(bounds.m_max.x);
					bottom = (USHORT)floor(bounds.m_min.y);
					top = (USHORT)ceil(bounds.m_max.y);

					poly = polyset.LH_Head();
					while (poly->LN_Succ()) {
						CriticalPathPoint* pp = (CriticalPathPoint*)poly->LH_Head();
						CriticalPathPoint* pn = pp->LN_Succ();
						while (pn->LN_Succ()) {
							auto pt_p = (CriticalPathPointData*)pp->LH_Head();
							auto pt_n = (CriticalPathPointData*)pn->LH_Head();
							if ((pt_p->LN_Succ()) && (pt_n->LN_Succ())) {
								USHORT x1 = (USHORT)floor(pt_p->x), x2 = (USHORT)floor(pt_n->x);
								USHORT y1 = (USHORT)floor(pt_p->y), y2 = (USHORT)floor(pt_n->y);
								if ((x1 == x2) || (y1 == y2)) {
									if (x1 > x2) {
										USHORT x = x1;
										x1 = x2;
										x2 = x;
									}
									if (y1 > y2) {
										USHORT y = y1;
										y1 = y2;
										y2 = y;
									}
									for (USHORT x = x1; x <= x2; x++) {
										for (USHORT y = y1; y <= y2; y++) {
											ULONG p_i = (ULONG)y * (ULONG)xSize + (ULONG)x;
											weak_assert(x < xSize);
											weak_assert(y < ySize);
											if (p_i < p_s) {
												if (ParaOption[0] == PARA_CRITICAL_PATH) {
													if (ep.stats[0].b_array)
														ep.stats[0].b_array[p_i] = true;
												}
												else {
													if (ep.stats[0].d_array)
														ep.stats[0].d_array[p_i] = 1.0;
												}
											}
										}
									}
								}
								else {
									const XYZ_Point	start(pt_p->x, pt_p->y, 0.0),
										end(pt_n->x, pt_n->y, 0.0),
										offset(floor(pt_p->x) - pt_p->x, floor(pt_p->y) - pt_p->y, 0.0),
										step(1.0, 1.0, 1.0);
									ULONG exit_index = (ULONG)floor(pt_n->y) * xSize + (ULONG)floor(pt_n->x);
									RayParms parms;
									parms.start = start;
									parms.path = end;
									parms.m_path_valid = true;
									parms.offset = offset;
									parms.step_size = step;
									parms.bounds_max_z = 1.0;
									double dist = pt_n->DistanceToSquared(*pt_p);
									parms.fcn = [&](APTR parameter, const XYZ_Point* entry, const XYZ_Point* exit)
									{
										USHORT x = (USHORT)floor((entry->x + exit->x) * 0.5);
										USHORT y = (USHORT)floor((entry->y + exit->y) * 0.5);
										ULONG p_i = (ULONG)y * (ULONG)xSize + (ULONG)x;
										weak_assert(x < xSize);
										weak_assert(y < ySize);
										if (p_i < p_s) {
											if (ParaOption[0] == PARA_CRITICAL_PATH) {
												if (ep.stats[0].b_array)
													ep.stats[0].b_array[p_i] = true;
											}
											else {
												if (ep.stats[0].d_array)
													ep.stats[0].d_array[p_i] = 1.0;
											}
										}
										if (p_i == exit_index)
											return 0;
										double dist2 = exit->XY_DistanceToSquared(*pt_p);
										if (dist2 > dist) {
											weak_assert(0);
											return 0;
										}
										return 1;
									};
									parms.parameter = nullptr;
									parms.DX = pt_n->x - pt_p->x;
									parms.DY = pt_n->y - pt_p->y;
									parms.RayTrace();
								}
							}
							pp = pn;
							pn = pn->LN_Succ();
						}
						poly = poly->LN_Succ();
					}

					while (poly = polyset.RemHead()) {
						delete poly;
					}
				}
			}
			else {
				ep.p_i = (ULONG)-1;
				ep.p_s = p_s;
				ep.xSize = xSize;
				ep.ySize = ySize;
				ep.scenario = scenario;
				ep.mintime = options.mintime;
				ep.time = options.time;
				ep.interp_method = interp_method;
				ep.discretization = options.discretization;
				ep.bounds.m_min.x = invertX(left);
				ep.bounds.m_max.x = invertX(right + 1);
				ep.bounds.m_max.y = invertY(top + 1);
				ep.bounds.m_min.y = invertY(bottom);
				ep.wx_method = wx_method;
				ep.ei_fcn = ei_fcn;
				ep.parm = parm;
				m_pool->SetJobFunction(stepBuildGrid, &ep);
				m_pool->StartJob();
				m_pool->BlockOnJob();
			}
		}
		else
			ep.aborted = true;


		an = scenario->AssetAtIndex(0);
		if (an)
		{
			while (an->LN_Succ())
			{
				an->m_filter->PostCalculationEvent(*options.time, (std::uint32_t)(1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE), nullptr);
				an->m_filter->PostCalculationEvent(*options.time, (std::uint32_t)(1 | (1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE)), nullptr);
				an = an->LN_Succ();
			}
		}
		ven = scenario->VectorAtIndex(0);
		if (ven)
		{
			while (ven->LN_Succ())
			{
				ven->m_filter->PostCalculationEvent(*options.time, (std::uint32_t)(1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE), nullptr);
				ven->m_filter->PostCalculationEvent(*options.time, (std::uint32_t)(1 | (1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE)), nullptr);
				ven = ven->LN_Succ();
			}
		}

        scenario->gridEngine()->PostCalculationEvent(scenario->layerThread(), *options.time, (ULONG)(1 | (1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE)), nullptr);
        scenario->gridEngine()->PostCalculationEvent(scenario->layerThread(), *options.time, (ULONG)(1 << CWFGM_SCENARIO_OPTION_WEATHER_ALTERNATE_CACHE), nullptr);
		if ((exists == S_OK) && (CWFGMProject::m_appMode < 0))
			scenario->GetWeatherGrid()->SetCache(scenario->layerThread(), 1, false);
	}

    return !ep.aborted;
}


template<typename T>
bool Project::CWFGMProject::ExportParameterGrid(const TCHAR *szPath, T* const arr, USHORT xSize,
	USHORT ySize, std::int16_t ParaOption, bool /*explicitNodata*/, USHORT left, USHORT right,
	USHORT bottom, USHORT top, bool minimizeOutput, bool shouldFlip, GDALExporter::CompressionType compression, UnitConversion* const unitConversion)
{
	bool smaller;
	T *sarr;
	std::uint16_t sxSize, sySize;
	if (minimizeOutput)
	{
		sxSize = right - left + 1;
		sySize = top - bottom + 1;
		if ((sxSize != xSize) && (sySize != ySize))
		{
			std::uint32_t new_size = (std::uint32_t)sxSize * (std::uint32_t)sySize;
			sarr = new T[new_size];
			std::uint16_t x, y;
			for (x = 0; x < sxSize; x++)
				for (y = 0; y < sySize; y++)
				{
					std::uint64_t s_index = y * sxSize + x;
					std::uint64_t a_index = (y + bottom) * xSize + (x + left);
					sarr[s_index] = arr[a_index];
				}
			smaller = true;
		}
		else
		{
			sxSize = xSize;
			sySize = ySize;
			sarr = arr;
			smaller = false;
		}
	}
	else
	{
		left = bottom = 0;
		right = xSize - 1;
		top = ySize - 1;
		sxSize = xSize;
		sySize = ySize;
		sarr = arr;
		smaller = false;
	}

	if ((ParaOption == PARA_RAZ) ||
		(ParaOption == PARA_FBP_RAZ_MAP) ||
		(ParaOption == PARA_DIRVECTOR_MAP)) {
		std::int64_t i, size = (std::int64_t)sxSize * (std::int64_t)sySize;
		for (i = 0; i < size; i++) {
			if ((sarr[i] != -1.0) && (sarr[i] != 0.0)) {
				double a = sarr[i];
				sarr[i] = (RADIAN_TO_DEGREE(a));
			}
			else
				sarr[i] = -9999.0;
		}
	}

	std::string stat_name = statName(ParaOption, unitConversion);

	double m_xllcorner, m_yllcorner;
	GetXLLCorner(&m_xllcorner);
	GetYLLCorner(&m_yllcorner);
    GDALExporter exporter;
    exporter.AddTag("TIFFTAG_SOFTWARE", "Prometheus");
    exporter.AddTag("TIFFTAG_GDAL_NODATA", "-9999");
	exporter.setExportCompress(compression);
	char mbstr[100];
#if _MSC_VER
    __time64_t long_time;
    _time64(&long_time);
    struct tm newtime;
    _localtime64_s(&newtime, &long_time);
    std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S %Z", &newtime);
#else
    std::time_t long_time = std::time(nullptr);
    std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S %Z", std::localtime(&long_time));
#endif
    exporter.AddTag("TIFFTAG_DATETIME", mbstr);
    PolymorphicAttribute v;
    m_grid->GetAttribute(CWFGM_GRID_ATTRIBUTE_SPATIALREFERENCE, &v);
	std::string str = GetPolymorphicAttributeData(v, ""s);
    exporter.setProjection(str.c_str());
    exporter.setSize(sxSize, sySize);
    exporter.setPixelResolution(m_resolution, m_resolution);
	if (smaller)
		exporter.setLowerLeft(m_xllcorner + m_resolution * left, m_yllcorner + m_resolution * bottom);
	else
		exporter.setLowerLeft(m_xllcorner, m_yllcorner);
	if (ParaOption == PARA_ARRIVAL_TIME)
        exporter.setPrecision(3);
	else if (ParaOption < 0)
		exporter.setPrecision(7);
	else if ((ParaOption != PARA_BURNT) && (ParaOption != PARA_CRITICAL_PATH)) {
        exporter.setPrecision(2);
        exporter.setWidth(9);
	}
    GDALExporter::ExportResult res = GDALExporter::ExportResult::ERROR_ACCESS;
    if constexpr (std::is_same<T, bool>::value)
	{
		if (shouldFlip)
            flip<bool>(sarr, sxSize, sySize);
		res = exporter.Export(sarr, szPath, stat_name.c_str());
		//flip the grid back so that the original array is unchanged
		if ((shouldFlip) && (sarr == arr))
			flip<bool>(sarr, sxSize, sySize);
    }
	else if constexpr (std::is_same<T, double>::value)
	{
        if (shouldFlip)
            flip<double>(sarr, sxSize, sySize);
		res = exporter.Export(sarr, szPath, stat_name.c_str());
		//flip the grid back so that the original array is unchanged
		if ((shouldFlip) && (sarr == arr))
			flip<double>(sarr, sxSize, sySize);
    }

	if (sarr != arr)
		delete[] sarr;

    return res != GDALExporter::ExportResult::ERROR_ACCESS;
}


template<typename T>
bool Project::CWFGMProject::ExportParameterGrid(const TCHAR *szPath, T* const acc, T* const acc2, T* const _min, T* const _max, T* const stdm, T* const stds, uint64_t * const cnt,
	USHORT xSize, USHORT ySize, std::int16_t ParaOption, bool explicitNodata, USHORT left, USHORT right, USHORT bottom, USHORT top,
	bool minimizeOutput, bool shouldFlip, GDALExporter::CompressionType compression, UnitConversion* const unitConversion)
{
	bool smaller;
	T *sacc, *sacc2, *smin, *smax, *scnt, *s_stdm, *s_stds, *stddev;		// s_stdm, s_stds are used to calculate standard deviation, algorithm is found at https://stackoverflow.com/questions/895929/how-do-i-determine-the-standard-deviation-stddev-of-a-set-of-values
	USHORT sxSize, sySize;
	if (minimizeOutput)
	{
		sxSize = right - left + 1;
		sySize = top - bottom + 1;
		if ((sxSize != xSize) && (sySize != ySize))
		{
			std::uint32_t new_size = (std::uint32_t)sxSize * (std::uint32_t)sySize;
			sacc = new T[new_size];
			sacc2 = new T[new_size];
			smin = new T[new_size];
			smax = new T[new_size];
			scnt = new T[new_size];
			s_stdm = new T[new_size];
			s_stds = new T[new_size];
			stddev = new T[new_size];
			USHORT x, y;
			for (x = 0; x < sxSize; x++)
				for (y = 0; y < sySize; y++)
				{
					std::uint64_t s_index = y * sxSize + x;
					std::uint64_t a_index = (y + bottom) * xSize + (x + left);
					sacc[s_index] = acc[a_index];
					sacc2[s_index] = acc2[a_index];
					smin[s_index] = _min[a_index];
					smax[s_index] = _max[a_index];
					scnt[s_index] = cnt[a_index];
					s_stdm[s_index] = stdm[a_index];
					s_stds[s_index] = stds[a_index];
				}
			smaller = true;
		}
		else
		{
			sxSize = xSize;
			sySize = ySize;
			sacc = acc;
			sacc2 = acc2;
			smin = _min;
			smax = _max;
			scnt = new T[xSize * ySize];
			for (std::uint64_t ii = 0; ii < (xSize * ySize); ii++)
				scnt[ii] = (double)cnt[ii];
			s_stdm = stdm;
			s_stds = stds;
			stddev = new T[xSize * ySize];
			smaller = false;
		}
	}
	else
	{
		left = bottom = 0;
		right = xSize - 1;
		top = ySize - 1;
		sxSize = xSize;
		sySize = ySize;
		sacc = acc;
		sacc2 = acc2;
		smin = _min;
		smax = _max;
		scnt = new T[xSize * ySize];
		for (std::uint64_t ii = 0; ii < (xSize * ySize); ii++)
			scnt[ii] = (double)cnt[ii];
		s_stdm = stdm;
		s_stds = stds;
		stddev = new T[xSize * ySize];
		smaller = false;
	}

	double m_xllcorner, m_yllcorner;
	GetXLLCorner(&m_xllcorner);
	GetYLLCorner(&m_yllcorner);
	GDALExporter exporter;
	exporter.AddTag("TIFFTAG_SOFTWARE", "Prometheus");
	exporter.AddTag("TIFFTAG_GDAL_NODATA", "-9999");
	exporter.setExportCompress(compression);
	char mbstr[100];
#if _MSC_VER
    __time64_t long_time;
    _time64(&long_time);
    struct tm newtime;
    _localtime64_s(&newtime, &long_time);
    std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S %Z", &newtime);
#else
    std::time_t long_time = std::time(nullptr);
    std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S %Z", std::localtime(&long_time));
#endif
	exporter.AddTag("TIFFTAG_DATETIME", mbstr);
	PolymorphicAttribute v;
	m_grid->GetAttribute(CWFGM_GRID_ATTRIBUTE_SPATIALREFERENCE, &v);
	std::string cref = GetPolymorphicAttributeData(v, ""s);
	exporter.setProjection(cref.c_str());
	exporter.setSize(sxSize, sySize);
	exporter.setPixelResolution(m_resolution, m_resolution);
	if (smaller)
		exporter.setLowerLeft(m_xllcorner + m_resolution * left, m_yllcorner + m_resolution * bottom);
	else
		exporter.setLowerLeft(m_xllcorner, m_yllcorner);
	if (ParaOption == PARA_ARRIVAL_TIME)
		exporter.setPrecision(3);
	else if (ParaOption < 0)
		exporter.setPrecision(7);
	else if ((ParaOption != PARA_BURNT) && (ParaOption != PARA_CRITICAL_PATH)) {
		exporter.setPrecision(2);
		exporter.setWidth(9);
	}
	GDALExporter::ExportResult res = GDALExporter::ExportResult::ERROR_ACCESS;
	if constexpr (std::is_same<T, bool>::value)
	{
		if (shouldFlip)
			flip<bool>(sacc, sxSize, sySize);
		res = exporter.Export(sacc, szPath);
		//flip the grid back so that the original array is unchanged
		if (shouldFlip)
			flip<bool>(sacc, sxSize, sySize);
	}
	else if constexpr (std::is_same<T, double>::value)
	{
		if (shouldFlip)
		{
			T *tempRow = new T[sxSize];
			flip<T>(sacc, tempRow, sxSize, sySize);
			flip<T>(sacc2, tempRow, sxSize, sySize);
			flip<T>(smin, tempRow, sxSize, sySize);
			flip<T>(smax, tempRow, sxSize, sySize);
			flip<T>(scnt, tempRow, sxSize, sySize);
			flip<T>(s_stdm, tempRow, sxSize, sySize);
			flip<T>(s_stds, tempRow, sxSize, sySize);
			delete[] tempRow;
		}

		if (explicitNodata) {
			std::int64_t size = (std::int64_t)sxSize * (std::int64_t)sySize;
			for (std::int64_t ii = 0; ii < size; ii++)
			{
				if (scnt[ii] == 0.0) {
					s_stds[ii] = s_stdm[ii] = smax[ii] = smin[ii] = sacc[ii] = -9999.0;
					if (sacc2)
						sacc2[ii] = -9999.0;
				}
			}
		}

		std::string stat, stat_name = statName(ParaOption, unitConversion);

		void *context;
		res = exporter.GDALExport_Open(szPath, GDT_Float64, (ParaOption == PARA_RAZ) ? 8 : 7, &context);
		int l = 1;
		stat = "Accumulated "; stat += stat_name; res = exporter.GDALExport_Append(sacc, "Prometheus Statistic", stat.c_str(), l++, &context);
		if (ParaOption == PARA_RAZ)
		{
			exporter.GDALExport_Append(sacc2, "Prometheus Statistic", (stat + " 2").c_str(), l++, &context);
		}
		stat = "Minimum "; stat += stat_name; res = exporter.GDALExport_Append(smin, "Prometheus Statistic", stat.c_str(), l++, &context);
		stat = "Maximum "; stat += stat_name; res = exporter.GDALExport_Append(smax, "Prometheus Statistic", stat.c_str(), l++, &context);

		stat = "Count of "; stat += stat_name; res = exporter.GDALExport_Append(scnt, "Prometheus Statistic", stat.c_str(), l++, &context);

		std::int64_t size = (std::int64_t)sxSize * (std::int64_t)sySize;
		for (std::int64_t ii = 0; ii < size; ii++)
		{
			if ((ParaOption != PARA_RAZ) &&
				(ParaOption != PARA_FBP_RAZ_MAP) &&
				(ParaOption != PARA_DIRVECTOR_MAP))
			{
				if (scnt[ii] != 0.0)
					stddev[ii] = sacc[ii] / scnt[ii];
				else
					stddev[ii] = 0.0;
			}
			else
			{
				if ((sacc[ii] != -1.0) && (sacc[ii] != 0.0)) {
					double angle = RADIAN_TO_DEGREE(CARTESIAN_TO_COMPASS_RADIAN(atan2(sacc2[ii], sacc[ii])));
					stddev[ii] = angle;
				}
				else
				{
					stddev[ii] = -9999.0;
				}
			}
		}
		stat = "Mean "; stat += stat_name; res = exporter.GDALExport_Append(stddev, "Prometheus Statistic", stat.c_str(), l++, &context);

		for (std::int64_t ii = 0; ii < size; ii++)
		{
			if ((ParaOption != PARA_RAZ) &&
				(ParaOption != PARA_FBP_RAZ_MAP) &&
				(ParaOption != PARA_DIRVECTOR_MAP))
			{
				if (scnt[ii] >= 1.0)
					stddev[ii] = sqrt(s_stds[ii] / (scnt[ii]));
				else if (explicitNodata)
					stddev[ii] = -9999.0;
				else
					stddev[ii] = 0.0;
			}
			else
			{
				if ((sacc[ii] != -1.0) && (sacc[ii] != 0.0))
				{
					stddev[ii] = sqrt(0.0 - log(sacc2[ii] * sacc2[ii] + sacc[ii] * sacc[ii]));	// from https://stackoverflow.com/questions/13928404/calculating-standard-deviation-of-angles
				}
				else
					stddev[ii] = -9999.0;
			}
		}
		stat = "Std. Dev. "; stat += stat_name; res = exporter.GDALExport_Append(stddev, "Prometheus Statistic", stat.c_str(), l++, &context);

		for (std::int64_t ii = 0; ii < size; ii++)
		{
			if (scnt[ii] >= 1.0)
				stddev[ii] = stddev[ii] / sqrt(scnt[ii]);		// seems that we don't need the -1 for the whole population, just a sample population
			else
				stddev[ii] = 0.0;
		}
		stat = "Std. Err. "; stat += stat_name; res = exporter.GDALExport_Append(stddev, "Prometheus Statistic", stat.c_str(), l++, &context);

		res = exporter.GDALExport_Complete(&context);

		//flip the grid back so that the original array is unchanged
		if ((shouldFlip) && (sacc == acc))
		{
			T *tempRow = new T[sxSize];
			flip<T>(sacc, tempRow, sxSize, sySize);
			flip<T>(sacc2, tempRow, sxSize, sySize);
			flip<T>(smin, tempRow, sxSize, sySize);
			flip<T>(smax, tempRow, sxSize, sySize);
			flip<T>(s_stdm, tempRow, sxSize, sySize);
			flip<T>(s_stds, tempRow, sxSize, sySize);
			delete[] tempRow;
		}
	}
	if (sacc != acc)
		delete[] sacc;
	if (sacc2 != acc2)
		delete[] sacc2;
	if (smin != _min)
		delete[] smin;
	if (smax != _max)
		delete[] smax;
	if (s_stdm != stdm)
		delete[] s_stdm;
	if (s_stds != stds)
		delete[] s_stds;
	delete[] scnt;
	delete[] stddev;

	return res != GDALExporter::ExportResult::ERROR_ACCESS;
}


const std::string Project::CWFGMProject::statName(const std::int16_t ParaOption, UnitConversion* const unitConversion)
{
	switch (ParaOption)
	{
	case PARA_FI:						return strprintf("FI (%s)", unitConversion->UnitName(unitConversion->IntensityDisplay(), true).c_str());
	case PARA_HFI:						return strprintf("HFI (%s)", unitConversion->UnitName(unitConversion->IntensityDisplay(), true).c_str());
	case PARA_FLAMELENGTH:				return strprintf("Flame Length (%s)", unitConversion->UnitName(unitConversion->SmallDistanceDisplay(), true).c_str());
	case PARA_FBP_FMC_MAP:				return "FMC";
	case PARA_CURINGDEGREE_MAP:			return "Curing Degree (%)";
	case PARA_GREENUP_MAP:				return "Green-up";
	case PARA_PC_MAP:					return "PC (%)";
	case PARA_PDF_MAP:					return "PDF (%)";
	case PARA_CBH_MAP:					return "CBH (m)";
	case PARA_TREE_HEIGHT_MAP:			return "Tree Height (m)";
	case PARA_FUEL_LOAD_MAP:			return strprintf("Fuel Load (%s)", unitConversion->UnitName(unitConversion->MassAreaDisplay(), true).c_str());
	case PARA_CFL_MAP:					return strprintf("CFL (%s)", unitConversion->UnitName(unitConversion->MassAreaDisplay(), true).c_str());
	case PARA_GRASSPHENOLOGY_MAP:		return "Grass Phenology";
	case PARA_ROS:						return strprintf("ROS (%s)", unitConversion->UnitName(unitConversion->AltVelocityDisplay(), true).c_str());
	case PARA_SFC:						return strprintf("SFC (%s)", unitConversion->UnitName(unitConversion->MassAreaDisplay(), true).c_str());
	case PARA_CFC:						return strprintf("CFC (%s)", unitConversion->UnitName(unitConversion->MassAreaDisplay(), true).c_str());
	case PARA_TFC:						return strprintf("TFC (%s)", unitConversion->UnitName(unitConversion->MassAreaDisplay(), true).c_str());
	case PARA_CFB:						return "CFB (0..1)";
	case PARA_HCFB:						return "HCFB (0..1)";
	case PARA_FBP_RAZ_MAP:
	case PARA_RAZ:						return "RAZ (" DEGREE_SYMBOL ")"; 
	case PARA_DIRVECTOR_MAP:			return "(" DEGREE_SYMBOL ")";
	case PARA_BURNT_MEAN:
	case PARA_BURNT:					return "Burn Grid"; 
	case PARA_CRITICAL_PATH_MEAN:
	case PARA_CRITICAL_PATH:			return "Critical Path Grid";
	case PARA_ARRIVAL_TIME:				return "Fire Arrival Time";
	case PARA_FBP_HROS_MAP:
	case PARA_FBP_HROS:					return strprintf("HROS (%s)", unitConversion->UnitName(unitConversion->AltVelocityDisplay(), true).c_str());
	case PARA_FBP_FROS_MAP:
	case PARA_FBP_FROS:					return strprintf("FROS (%s)", unitConversion->UnitName(unitConversion->AltVelocityDisplay(), true).c_str());
	case PARA_FBP_BROS_MAP:
	case PARA_FBP_BROS:					return strprintf("BROS (%s)", unitConversion->UnitName(unitConversion->AltVelocityDisplay(), true).c_str());
	case PARA_ROSVECTOR_MAP:			return strprintf("ROSv (%s)", unitConversion->UnitName(unitConversion->AltVelocityDisplay(), true).c_str());
	case PARA_FBP_RSS_MAP:
	case PARA_FBP_RSS:					return "RSS"; 
	case PARA_TEMP:						return strprintf("Temperature (%s)", unitConversion->UnitName(unitConversion->TempDisplay(), true).c_str());
	case PARA_DEW:						return strprintf("Dew Point (%s)", unitConversion->UnitName(unitConversion->TempDisplay(), true).c_str());
	case PARA_RH:						return "Relative Humidity (%)";
	case PARA_WD:						return "Wind Direction (" DEGREE_SYMBOL ")";
	case PARA_WS:						return strprintf("Wind Speed (%s)", unitConversion->UnitName(unitConversion->VelocityDisplay(), true).c_str());
	case PARA_GUST:						return strprintf("Wind Gust (%s)", unitConversion->UnitName(unitConversion->VelocityDisplay(), true).c_str());
	case PARA_PRECIP:					return strprintf("Precipitation (%s)", unitConversion->UnitName(unitConversion->SmallMeasureDisplay(), true).c_str());
	case PARA_FFMC:						return "sFFMC";
	case PARA_ISI:						return "sISI"; 
	case PARA_FWI:						return "sFWI"; 
	case PARA_SBUI:						return "sBUI"; 
	case PARA_ACTIVE:					return "Active";
	case PARA_BURNED:					return "Burned (%)";
	case PARA_BURNED_CHANGE:			return "Percentage Burned (%)";
	case PARA_MIN_ARRIVAL_TIME:			return "Fire Arrival Time (Min)";
	case PARA_MAX_ARRIVAL_TIME:			return "Fire Arrival Time (Max)";
	case PARA_TOTAL_FUEL_CONSUMED:		return strprintf("Total Fuel Consumed (%s)", unitConversion->UnitName(unitConversion->MassDisplay(), true).c_str());
	case PARA_SURFACE_FUEL_CONSUMED:	return strprintf("Surface Fuel Consumed (%s)", unitConversion->UnitName(unitConversion->MassDisplay(), true).c_str());
	case PARA_CROWN_FUEL_CONSUMED:		return strprintf("Crown Fuel Consumed (%s)", unitConversion->UnitName(unitConversion->MassDisplay(), true).c_str());
	case PARA_RADIATIVE_POWER:			return strprintf("Radiative Power (%s)", unitConversion->UnitName(unitConversion->PowerDisplay(), true).c_str());
	}
	return "unknown";
}


template<typename T>
bool Project::CWFGMProject::ExportParameterGrid(const TCHAR *szPath, std::int16_t ParaOption, bool explicitNodata, bool minimizeOutput,
	bool multilayer, GDALExtras::CumulativeGridOutput<T> &output, GDALExporter::CompressionType compression, UnitConversion* const unitConversion, T maximum)
{
    USHORT xSize, ySize;
    bool retval = false;

    GetDimensions(&xSize, &ySize);
	ULONG p_s = (ULONG)xSize * (ULONG)ySize;

	if (p_s > 0 && output.isSameSize(xSize, ySize))
    {
		GDALExporter::FILETYPE type = GDALExporter::detectType(szPath);
		if ((multilayer) && (ParaOption != PARA_BURNT) && (ParaOption != PARA_CRITICAL_PATH) && (type != GDALExporter::FILETYPE::ASCII)) { }
		else
		{
			//normalize the output to a given maximum value
			if (maximum > 0)
				output.normalize(maximum, false, -9999.0, explicitNodata, false);
			//average the output
			else
			{
				if ((ParaOption == PARA_BURNT_MEAN) || (ParaOption == PARA_CRITICAL_PATH_MEAN) || (ParaOption == PARA_PRECIP))
					output.normalize(false, -9999.0, explicitNodata, false);
				else
				{
					if ((ParaOption == PARA_RAZ) ||
						(ParaOption == PARA_FBP_RAZ_MAP) ||
						(ParaOption == PARA_WD) ||
						(ParaOption == PARA_DIRVECTOR_MAP))
					{
						output.modify([&](T *acc1, T *acc2, T* _min, T* _max, T* m_stdm, T* m_stds, std::uint64_t *cnt)
						{
							std::int64_t size = (std::int64_t)xSize * (std::int64_t)ySize;
							int thread_id = -1;
#pragma omp parallel for num_threads(CWorkerThreadPool::NumberIdealProcessors(0)) firstprivate(thread_id)
							for (std::int64_t i = 0; i < size; i++)
							{
								if (thread_id == -1) {
									thread_id = omp_get_thread_num();
									CWorkerThread::native_handle_type thread = CWorkerThreadPool::GetCurrentThread();
									CWorkerThreadPool::SetThreadAffinityToMask(thread, thread_id);
								}
								if ((cnt[i]) || (!explicitNodata))
								{
									double angle = RADIAN_TO_DEGREE(CARTESIAN_TO_COMPASS_RADIAN(atan2(acc2[i], acc1[i])));
									acc1[i] = angle;
								}
								else
									acc1[i] = -9999.0;
							}
						});
					}
					else if ((ParaOption >= 10) && ((ParaOption < 20) || (ParaOption > 23)))
						output.normalizepercell(false, -9999.0, explicitNodata, false);
				}
			}
		}

		std::uint32_t left, right, bottom, top;
		if (output.isEmpty())
			return false;
		output.boundingBox(left, bottom, right, top,
			((ParaOption == PARA_BURNT) ||
				(ParaOption == PARA_BURNT_MEAN) ||
				(ParaOption == PARA_CRITICAL_PATH) ||
				(ParaOption == PARA_CRITICAL_PATH_MEAN)) ? true : false);

        //save the normalized grid, requires modify because the grid will be flipped during export
        output.modify([&](T *acc1, T *acc2, T* _min, T* _max, T* stdm, T* stds, uint64_t *cnt)
            {
				if ((multilayer) && (ParaOption != PARA_BURNT) && (ParaOption != PARA_CRITICAL_PATH) && (type != GDALExporter::FILETYPE::ASCII))
				{
					retval = ExportParameterGrid(szPath, acc1, acc2, _min, _max, stdm, stds, cnt, xSize, ySize, ParaOption, explicitNodata, (USHORT)left, (USHORT)right, (USHORT)bottom, (USHORT)top, minimizeOutput, true, compression, unitConversion);
				}
				else
				{
					retval = ExportParameterGrid(szPath, acc1, xSize, ySize, ParaOption, explicitNodata, (USHORT)left, (USHORT)right, (USHORT)bottom, (USHORT)top, minimizeOutput, true, compression, unitConversion);
				}
			});
    }

    return retval;
}


bool Project::CWFGMProject::ExportParameterGrid(const TCHAR *szPath, std::int16_t ParaOption, bool explicitNodata, bool minimizeOutput,
	bool multilayer, GDALExtras::CumulativeGridOutput<double> *output, GDALExporter::CompressionType compression, UnitConversion* const unitConversion, double maximum)
{
	return ExportParameterGrid(szPath, ParaOption, explicitNodata, minimizeOutput, multilayer, *output, compression, unitConversion, maximum);
}


template<typename T>
bool Project::CWFGMProject::exportParameterGridArray(std::vector<GDALExtras::CumulativeGridOutput<T> *> &outputs, const Scenario *scenario, const Asset *asset, const ULONG asset_index, const Project_XYStatOptions& options,
    std::vector<std::int16_t> &ParaOption, bool explicitNodata, export_setsize ess_fcn,
		export_iteration ei_fcn, APTR parm, ::UnitConversion* const unitConversion)
{
	if (outputs.size() != ParaOption.size())
		return false;

    USHORT xSize, ySize;
    bool retval = false;

    GetDimensions(&xSize, &ySize);
	std::uint32_t p_s = (std::uint32_t)xSize * (std::uint32_t)ySize;

	for (auto &output : outputs) {
		if (!output->isSameSize(xSize, ySize))
			return false;
	}

    if (p_s > 0) {
		std::vector<T*> t;

		for (size_t i = 0; i < outputs.size(); i++) {
			//create a temporary array
			T* tarr = nullptr;
			try
			{
				tarr = new T[p_s];
				t.push_back(tarr);
			}
			catch(std::bad_alloc&)
			{
				return false;
			}
		}

		USHORT left, right, bottom, top;
        //compute the grid output
        retval = ExportParameterGrid(t, /*wxTime,*/ scenario, asset, asset_index, options, ParaOption, explicitNodata,
			left, right, bottom, top, ess_fcn, ei_fcn, parm, unitConversion);

        if (retval) {
			for (size_t j = 0; j < outputs.size(); j++) {
				auto &output = outputs[j];
				T *tarr = t[j];
				//add the temporary array to the cumulative array
				output->write([&](T *acc1, T *acc2, T* _min, T* _max, T* stdm, T* stds, std::uint64_t *cnt)
					{
						int64_t size = (std::int64_t)xSize * (std::int64_t)ySize;
						int thread_id = -1;
#pragma omp parallel for num_threads(CWorkerThreadPool::NumberIdealProcessors(0)) firstprivate(thread_id)
						for (std::int64_t i = 0; i < size; i++)
						{
							if (thread_id == -1)
							{
								thread_id = omp_get_thread_num();
								CWorkerThread::native_handle_type thread = CWorkerThreadPool::GetCurrentThread();
								CWorkerThreadPool::SetThreadAffinityToMask(thread, thread_id);
							}
							if ((ParaOption[j] == PARA_BURNT) || (ParaOption[j] == PARA_CRITICAL_PATH))
							{
								weak_assert(0);
							}
							else if ((ParaOption[j] == PARA_BURNT_MEAN) || (ParaOption[j] == PARA_CRITICAL_PATH_MEAN) || (ParaOption[j] == PARA_PRECIP))
							{
								if (tarr[i] != -9999.0) {
								if (cnt[i])
								{
									if (_min[i] > tarr[i])
										_min[i] = tarr[i];
									else if (_max[i] < tarr[i])
										_max[i] = tarr[i];
								}
								else
								{
									_max[i] = _min[i] = tarr[i];
								}
								acc1[i] += tarr[i];
								cnt[i]++;
								T tmpm = stdm[i];
								stdm[i] += (tarr[i] - tmpm) / cnt[i];
								stds[i] += (tarr[i] - tmpm) * (tarr[i] - stdm[i]);
							}
							}
							else
							{
								if ((ParaOption[j] == PARA_RAZ) ||
									(ParaOption[j] == PARA_FBP_RAZ_MAP) ||
									(ParaOption[j] == PARA_WD) ||
									(ParaOption[j] == PARA_DIRVECTOR_MAP))
								{
									if (tarr[i] != 0.0)
									{
										XY_Point raz(1.0, 0.0);
										raz.RotateXY_Cartesian(tarr[i]);
										acc1[i] += raz.x;
										acc2[i] += raz.y;
										cnt[i]++;
									}
								} else if ((tarr[i] != 0.0) && (tarr[i] != -9999.0)) {		// this is needed when combining sub-scenario outputs of arrival time
									if (cnt[i])	{
										if (_min[i] > tarr[i])
											_min[i] = tarr[i];
										else if (_max[i] < tarr[i])
											_max[i] = tarr[i];
									} else {
										_max[i] = _min[i] = tarr[i];
									}
									acc1[i] += tarr[i];
									cnt[i]++;
									T tmpm = stdm[i];
									stdm[i] += (tarr[i] - tmpm) / cnt[i];
									stds[i] += (tarr[i] - tmpm) * (tarr[i] - stdm[i]);
								}
							}
						}

#ifdef _DEBUG
						for (int64_t ii = 0; ii < size; ii++)
						{

#ifdef __GNUC__
							weak_assert(!__isnan(_min[ii]));
							weak_assert(!__isnan(_max[ii]));
							weak_assert(!__isnan(acc1[ii]));
							weak_assert(!__isnan(acc2[ii]));
							weak_assert(!__isnan(stdm[ii]));
							weak_assert(!__isnan(stds[ii]));
#else
							weak_assert(!_isnan(_min[ii]));
							weak_assert(!_isnan(_max[ii]));
							weak_assert(!_isnan(acc1[ii]));
							weak_assert(!_isnan(acc2[ii]));
							weak_assert(!_isnan(stdm[ii]));
							weak_assert(!_isnan(stds[ii]));
#endif

						}
#endif

					});
				}
		}

		for (auto tarr : t)
		{
			if (tarr)
				delete[] tarr;
		}
    }

    return retval;
}


bool Project::CWFGMProject::exportParameterGridArray(std::vector<GDALExtras::CumulativeGridOutput<double> *> *outputs, const Scenario *scenario, const Asset *asset, const ULONG asset_index, const Project_XYStatOptions& options,
	std::vector<std::int16_t> &ParaOption, bool explicitNodata, export_setsize ess_fcn, export_iteration ei_fcn, APTR parm, ::UnitConversion* const unitConversion)
{
	return exportParameterGridArray(*outputs, scenario, asset, asset_index, options, ParaOption, explicitNodata, ess_fcn, ei_fcn, parm, unitConversion);
}


bool Project::CWFGMProject::ExportParameterGrid(const TCHAR *szPath, const Scenario *scenario, const Asset *asset, const ULONG asset_index, const Project_XYStatOptions& options, /*USHORT method, USHORT discretization,*/
	std::int16_t ParaOption, bool explicitNodata, bool minimizeOutput, bool multilayer, export_setsize ess_fcn, export_iteration ei_fcn, APTR parm,
	GDALExporter::CompressionType compression, ::UnitConversion* const unitConversion)
{
	if (szPath==nullptr || (!(*szPath)))
		return false;

	USHORT xSize, ySize;

    GetDimensions(&xSize, &ySize);

	std::uint32_t p_s = (std::uint32_t)xSize * (std::uint32_t)ySize;

	if (p_s == 0)
		return false;

	bool combine = (scenario->m_childArray.size() || scenario->m_parent);					// if it has no parent and no children, then it's not part of a set to combine

	combine |= (ParaOption == PARA_CRITICAL_PATH_MEAN) && (asset);							// only if we're exporting critical paths...

	if (!combine) {	// if it has no parent and no children, then it's not part of a set to combine
		double *d_array = nullptr;
		bool *b_array = nullptr;
        try
        {
			if ((ParaOption == PARA_BURNT) || (ParaOption == PARA_CRITICAL_PATH))
				b_array = new bool[p_s];
            else
                d_array = new double[p_s];
        }
        catch (std::bad_alloc&)
        {
            return false;
        }

        bool retval = false;
		USHORT left, right, bottom, top;
		std::vector<std::int16_t> p;
		p.push_back(ParaOption);
		if ((ParaOption == PARA_BURNT) || (ParaOption == PARA_CRITICAL_PATH_MEAN)) {
			std::vector<bool*> b;
			b.push_back(b_array);
            retval = ExportParameterGrid(b, scenario, asset, asset_index, options, p, explicitNodata, left, right, bottom, top, ess_fcn, ei_fcn, parm, unitConversion);
            if (retval)
                retval = ExportParameterGrid(szPath, b_array, xSize, ySize, ParaOption, explicitNodata, left, right, bottom, top, minimizeOutput, true, compression, unitConversion);
        }
		else
		{
			std::vector<double*> d;
			d.push_back(d_array);

            retval = ExportParameterGrid(d, scenario, asset, asset_index, options, p, explicitNodata, left, right, bottom, top, ess_fcn, ei_fcn, parm, unitConversion);
			if (retval)
			{
				retval = ExportParameterGrid(szPath, d_array, xSize, ySize, ParaOption, explicitNodata, left, right, bottom, top, minimizeOutput, true, compression, unitConversion);
			}
        }

        if (b_array)
            delete[] b_array;
        if (d_array)
            delete[] d_array;

        return retval;
	}
	else
	{
		std::vector<GDALExtras::CumulativeGridOutput<double> *> vd;
		std::vector<SHORT> vs;
		GDALExtras::CumulativeGridOutput<double> *d_array = nullptr;
		try
		{
			if (ParaOption == PARA_BURNT)
				ParaOption = PARA_BURNT_MEAN;
			else if (ParaOption == PARA_CRITICAL_PATH)
				ParaOption = PARA_CRITICAL_PATH_MEAN;
			d_array = new GDALExtras::CumulativeGridOutput<double>(xSize, ySize);
		}
		catch(std::bad_alloc&)
		{
			return false;
		}

		bool retval = false;
		if (d_array)
		{
			vd.push_back(d_array);
			vs.push_back(ParaOption);

			int i;
			for (i = 0; i < (int)scenario->m_childArray.size(); i++)
			{
				retval |= exportParameterGridArray(&vd, scenario->m_childArray[i], asset, asset_index, options, vs, explicitNodata, ess_fcn, ei_fcn, parm, unitConversion);
			}
			if (retval)
				retval = ExportParameterGrid(szPath, ParaOption, explicitNodata, minimizeOutput, multilayer, *d_array, compression, unitConversion);
			delete d_array;
		}

		return retval;
	}
}


bool Project::CWFGMProject::ExportParameterGrid(std::vector<GridOutput> &outputs, const Scenario *scenario, const Asset *asset, const ULONG asset_index, const Project_XYStatOptions& options,
	bool explicitNodata, bool minimizeOutput, bool multilayer, export_setsize ess_fcn,
	export_iteration ei_fcn, APTR parm, USHORT mode, GDALExporter::CompressionType compression, ::UnitConversion* const unitConversion)
{
	if ((mode == 3) && (outputs.size() == 1)) {
		std::int16_t ParaOption = outputs[0].statistic;
		if (ParaOption == PARA_CRITICAL_PATH_MEAN) {
			if ((asset) || (asset_index == (ULONG)-1)) {
				bool retval = false;
				Asset* a = m_assetCollection.FirstAsset();
				while (a->LN_Succ()) {
					if ((a == asset) || (asset == (Asset*)~NULL)) {
						std::uint32_t pc = a->GetPolyCount();
						for (std::uint32_t pi = 0; pi < pc; pi++) {
							if ((pi == asset_index) || (asset_index == (ULONG)-1)) {
								retval |= ExportParameterGrid(outputs, scenario, a, pi, options, explicitNodata, minimizeOutput, multilayer, ess_fcn, ei_fcn, parm, mode & 1, compression, unitConversion);
							}
						}
					}
					a = a->LN_Succ();
				}
				retval = ExportParameterGrid(outputs, scenario, asset, asset_index, options, explicitNodata, minimizeOutput, multilayer, ess_fcn, ei_fcn, parm, mode & 2, compression, unitConversion);
				return retval;
			}
		}
	}

	bool combine = (scenario->m_childArray.size() || scenario->m_parent);					// if it has no parent and no children, then it's not part of a set to combine

	if (outputs.size() == 1) {
		std::int16_t ParaOption = outputs[0].statistic;
		combine |= (ParaOption == PARA_CRITICAL_PATH_MEAN) && (asset);						// only if we're exporting critical paths...
	}

	if (!combine) {
		weak_assert(mode == 3);
		USHORT left, right, bottom, top;
		bool retval;
		if (mode & 1)
		{
			if ((outputs.size() == 1) && ((outputs[0].statistic == PARA_BURNT) || ((outputs[0].statistic == PARA_CRITICAL_PATH)))) {
				USHORT xSize, ySize;
				GetDimensions(&xSize, &ySize);

				std::uint32_t p_s = (std::uint32_t)ySize * (std::uint32_t)xSize;
				if (p_s == 0)
					return false;

				bool *b_array = nullptr;
				try
				{
					b_array = new bool[p_s];
					for (std::uint32_t i = 0; i < p_s; i++)
						b_array[i] = false;
				}
				catch(std::bad_alloc&)
				{
					return false;
				}

				std::vector<bool *> b;
				std::vector<std::int16_t> p;
				b.push_back(b_array);
				p.push_back(outputs[0].statistic);
							// this will accumulate a burn grid into an array
				retval = ExportParameterGrid(b, scenario, asset, asset_index, options, p, explicitNodata, left, right, bottom, top, ess_fcn, ei_fcn, parm, unitConversion);
				if ((mode & 2) && (retval))
				{			// ... and this will do the actual output to a file
					retval |= ExportParameterGrid(outputs[0].filename.c_str(), b_array, xSize, ySize, outputs[0].statistic, explicitNodata, left, right, bottom, top, minimizeOutput, true, compression, unitConversion);
				}
				delete[] b_array;
				return retval;
			}
			else
			{
				std::vector<double *> d;
				std::vector<std::int16_t> p;

				for (auto &output : outputs)
				{
					d.push_back(output.m_d);
					p.push_back(output.statistic);
				}
							// this will accumulate results into an array of doubles
				retval = ExportParameterGrid(d, scenario, asset, asset_index, options, p, explicitNodata, left, right, bottom, top, ess_fcn, ei_fcn, parm, unitConversion);
			}
		}
		else
			retval = true;

		if (mode & 2)		// if we are ready to dump to a file...
		{
			if (retval)
			{
				USHORT xSize, ySize;
				GetDimensions(&xSize, &ySize);

				retval = false;
				for (size_t j = 0; j < outputs.size(); j++)
				{
					auto &output = outputs[j];	// this will do the actual output to a file
					retval |= ExportParameterGrid(output.filename.c_str(), output.m_d, xSize, ySize, output.statistic, explicitNodata, left, right, bottom, top, minimizeOutput, true, compression, unitConversion);
				}
			}
		}
		return retval;
	} 
	else	// in this 'else', we know we are combining sub-scenario's so no desire for now to try to play with settings for grid export resolution or LL corner
	{
		bool retval;
		if (mode & 1)
		{
			retval = false;
			std::vector<GDALExtras::CumulativeGridOutput<double> *> vd;
			std::vector<std::int16_t> vs;

			for (auto &output : outputs)
			{
				vd.push_back(output.m_darray);
				if ((output.statistic != PARA_BURNT) && (output.statistic != PARA_CRITICAL_PATH))
					vs.push_back(output.statistic);
				else if (output.statistic == PARA_BURNT)
					vs.push_back(PARA_BURNT_MEAN);
				else
					vs.push_back(PARA_CRITICAL_PATH_MEAN);
			}

			if (!scenario->m_childArray.size()) {
				retval = exportParameterGridArray(&vd, scenario, asset, asset_index, options, vs, explicitNodata, ess_fcn, ei_fcn, parm, unitConversion);
			} else {
				int i;
				for (i = 0; i < scenario->m_childArray.size(); i++) {
					retval |= exportParameterGridArray(&vd, scenario->m_childArray[i], asset, asset_index, options, vs, explicitNodata, ess_fcn, ei_fcn, parm, unitConversion);
				}
			}
		}
		else
			retval = true;

		if (mode & 2)
		{
			if (retval)
			{
				retval = false;
				for (auto &output : outputs)
				{
					std::int16_t ParaOption;
					if ((output.statistic != PARA_BURNT) && (output.statistic != PARA_CRITICAL_PATH))
						ParaOption = output.statistic;
					else if (output.statistic == PARA_BURNT)
						ParaOption = PARA_BURNT_MEAN;
					else
						ParaOption = PARA_CRITICAL_PATH_MEAN;
					retval |= ExportParameterGrid(output.filename.c_str(), ParaOption, explicitNodata, minimizeOutput, multilayer, output.m_darray, compression, unitConversion);
				}
			}
		}
		return retval;
	}
}


HRESULT Project::CWFGMProject::getDimensions(USHORT *xdim, USHORT *ydim) const
{
	HRESULT hr = gridEngine()->GetDimensions(nullptr, xdim, ydim);
	return hr;
}


HRESULT Project::CWFGMProject::getResolution(double *resolution) const
{
	PolymorphicAttribute v;
	HRESULT hr = m_gridEngine->GetAttribute(nullptr, CWFGM_GRID_ATTRIBUTE_PLOTRESOLUTION, &v);

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(hr))
        hr = VariantToDouble_(v, resolution);
	return hr;
}


std::string Project::CWFGMProject::FormatXY(const double x, const double y, UnitConvert::STORAGE_UNIT to_format, const ::UnitConversion *uc, bool include_units)
{
	std::string str;

	switch (to_format)
	{
	case STORAGE_COORDINATE_DEGREE:
	case STORAGE_COORDINATE_DEGREE_MINUTE:
	case STORAGE_COORDINATE_DEGREE_MINUTE_SECOND: {
		double lat, lon;
		GetLLByCoord(x, y, &lat, &lon);
		return FormatLatLon(lat, lon, to_format, uc, include_units);
	}

	case STORAGE_COORDINATE_UTM: {
		if (include_units)
			str = strprintf(_T("%.3lf, %.3lf m"), x, y);
		else
			str = strprintf(_T("%.3lf, %.3lf"), x, y);
	}
									break;

	case STORAGE_COORDINATE_RELATIVE_DISTANCE: {
		double X, Y;
		double xllcorner, yllcorner;
		GetXLLCorner(&xllcorner);
		GetYLLCorner(&yllcorner);
		X = x;
		Y = y;
		X -= xllcorner;
		Y -= yllcorner;
		X = uc->ConvertUnit(X, STORAGE_FORMAT_M, uc->DistanceDisplay());
		Y = uc->ConvertUnit(Y, STORAGE_FORMAT_M, uc->DistanceDisplay());
		if (include_units)
			str = strprintf(_T("%.3lf, %.3lf %s"), X, Y, uc->UnitName(uc->DistanceDisplay(), true).c_str());
		else
			str = strprintf(_T("%.3lf, %.3lf"), X, Y);
	}
												break;

    #ifdef DEBUG
	default:
		weak_assert(0);
    #endif
	}
	return str;
}


std::string Project::CWFGMProject::FormatLatLon(const double latitude, const double longitude, UnitConvert::STORAGE_UNIT to_format, const ::UnitConversion *uc, bool include_units)
{
	std::string str;

	switch (to_format)
	{
	case STORAGE_COORDINATE_DEGREE:		str = strprintf(_T("%.6lf%c,%.6lf%c"), latitude, 0xb0, longitude ,0xb0);
						break;
	case STORAGE_COORDINATE_DEGREE_MINUTE: {
						double latMin, lonMin;
						latMin = (latitude - (int)latitude) * 60.0;
						lonMin = (longitude - (int)longitude) * 60.0;
						str = strprintf(_T("%d%c%.2lf',%d%c%.2lf'"),
							(int)latitude,0xb0, fabs(latMin),
							(int)longitude, 0xb0, fabs(lonMin));
						break;
						    }
	case STORAGE_COORDINATE_DEGREE_MINUTE_SECOND: {
						double latMin, lonMin, latSec, lonSec;
						latMin = (latitude - (int)latitude) * 60.0;
						lonMin = (longitude - (int)longitude) * 60.0;
						latSec = (latMin - (int)latMin) * 60.0;
						lonSec = (lonMin - (int)lonMin) * 60.0;
						str = strprintf(_T("%d%c%d'%.2lf\",%d%c%d'%.2lf\""),
							(int)latitude, 0xb0, (int)fabs(latMin), fabs(latSec),
							(int)longitude, 0xb0, (int)fabs(lonMin), fabs(lonSec));
						break;
						}

	case STORAGE_COORDINATE_UTM:
	case STORAGE_COORDINATE_RELATIVE_DISTANCE: {
		double x, y;
		GetXYByLL(latitude, longitude, &x, &y);
		double resolution;
		GetResolution(&resolution);
		x /= resolution;
		y /= resolution;
		return FormatXY(x, y, to_format, uc, include_units);
	}
#ifdef DEBUG
	default:
		weak_assert(0);
#endif
	}
	return str;
}

std::string Project::CWFGMProject::printableString(const std::string &str, USHORT indent_cnt)
{
	std::string resultString("");

	boost::char_separator<char> sep("\n");
	boost::tokenizer<boost::char_separator<char>> tokens(str, sep);
	for (const auto& t : tokens)
	{
		for (auto i = 0; i < indent_cnt; i++)
			resultString += "\t";
		resultString += boost::trim_copy(t) + "\n";
	}
	resultString += "\n";
	return resultString;
}


HRESULT Project::CWFGMProject::PrintReport(const TCHAR *szPath, const PrintReportOptions &PrntRptOptns, const TCHAR *VersionNumber, const TCHAR *ReleaseDate,
	const TCHAR *FGMName, Scenario *currScenario, const ::UnitConversion *uc, bool addWeather)
{
	fs::path path(szPath);
	if (boost::iequals(path.extension().string(), ".json") || boost::iequals(path.extension().string(), ".bin"))
	{
		std::vector<Scenario*> vec;
		vec.push_back(currScenario);
		return PrintReportJson(szPath, nullptr, PrntRptOptns, VersionNumber, ReleaseDate, FGMName, vec, uc, addWeather);
	}
	else
		return PrintReportTxt(szPath, PrntRptOptns, VersionNumber, ReleaseDate, FGMName, currScenario, uc);
}


HRESULT Project::CWFGMProject::PrintReport(const SummaryCollection* collection, const class PrintReportOptions& PrntRptOptns, const TCHAR* VersionNumber,
	const TCHAR* ReleaseDate, const TCHAR* FGMName, Scenario* currScenario, const ::UnitConversion* uc, bool addWeather)
{
	std::vector<Scenario*> vec;
	vec.push_back(currScenario);
	return PrintReportJson(nullptr, collection, PrntRptOptns, VersionNumber, ReleaseDate, FGMName, vec, uc, addWeather);
}


HRESULT Project::CWFGMProject::PrintReportTxt(const TCHAR *szPath, const PrintReportOptions &PrntRptOptns, const TCHAR *VersionNumber, const TCHAR *ReleaseDate, const TCHAR *FGMName, Scenario *currScenario, const ::UnitConversion *uc)
{
    FILE* outputFile;
#if _MSC_VER
	errno_t err = _tfopen_s(&outputFile, szPath, _T("w+"));
	if (outputFile == NULL)
#if _DLL
		return ERROR_FILE_NOT_FOUND;
#else
		return ResultCodes::FILE_NOT_FOUND;
#endif
#else
    outputFile = fopen(szPath, "w+");
    if (!outputFile)
        return ResultCodes::FILE_NOT_FOUND;
#endif

	std::string resultString;
	WTime today = WTime::Now(m_timeManager, WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
	resultString += _T("Prometheus Fire Growth Model (Version ") + std::string(VersionNumber) + _T(" released on ") + ReleaseDate + _T(")\n\n");
	if (FGMName)
		resultString += _T("FGM File Name: ") + std::string(FGMName) + _T("\n");
	resultString += _T("Date of Report: ") + today.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_DATE | WTIME_FORMAT_YEAR | WTIME_FORMAT_TIME | WTIME_FORMAT_EXCLUDE_SECONDS) + _T("\n");

	WTime tempStartTime(m_timeManager);
	WTime tempEndTime(m_timeManager);
	tempStartTime = currScenario->Simulation_StartTime();
	tempEndTime = currScenario->Simulation_EndTime();

	resultString += _T("\nSimulation Settings:\n");
	if (PrntRptOptns.getScenarioSettings() == 1)
	{
		resultString += _T("\tScenario Inputs:\n");

		resultString += _T("\t\tName: ") + currScenario->m_name + _T("\n");
		resultString += _T("\t\tStart Time: ");
		resultString += tempStartTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_YEAR | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE);
		resultString += "\n";
		resultString += _T("\t\tEnd Time:   ");
		resultString += tempEndTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_YEAR | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE);
		resultString += "\n";

		//Fires in Scenario
		if (currScenario->GetFireCount() != 0)
			resultString += _T("\t\tIgnitions:\n");
		for (int i = 0; i < (int)currScenario->GetFireCount(); i++)
		{
			resultString += _T("\t\t\tName: ") + currScenario->FireAtIndex(i)->m_name + "\n";
			PolymorphicAttribute v;
			WTime t((std::uint64_t)0, m_timeManager);
			if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITION_START_TIME_OVERRIDE, &v)) &&
				SUCCEEDED(VariantToTime_(v, &t)))
			{
				if (t.GetTotalMicroSeconds() > 0) {
					resultString += "\t\t\t\tStart Time Override: ";
					resultString += t.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_YEAR | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE);
					resultString += "\n";
				}
			}
		}

		if (PrntRptOptns.getGeoData() == 1)
		{
			//Vectors in Scenario
			resultString += "\t\tFuel Breaks:\n";

			std::string temp;
			double res;
			GetResolution(&res);

			for (ULONG i = 0; i < currScenario->GetVectorCount(); i++)
			{
				resultString += "\t\t\tName: " + std::string(currScenario->VectorAtIndex(i)->Name()) + "\n";
				temp = strprintf("\t\t\t\tWidth: %.1lf\n", currScenario->VectorAtIndex(i)->GetPolyLineWidth());
				resultString += temp;
				resultString += _T("\t\t\t\tComments:\n");
				resultString += printableString(currScenario->VectorAtIndex(i)->Comments(), 3);
			}
		}

		if ((PrntRptOptns.getAssetData() == 1) && (currScenario->GetAssetCount()))
		{
			//Assets in Scenario
			resultString += "\t\tAssets:\n";

			std::string temp;
			double res;
			GetResolution(&res);

			for (ULONG i = 0; i < currScenario->GetAssetCount(); i++)
			{
				resultString += "\t\t\tName: "s + currScenario->AssetAtIndex(i)->Name() + "\n"s;
				temp = strprintf("\t\t\t\tBoundary: %.1lf\n", currScenario->AssetAtIndex(i)->GetPolyLineWidth());
				resultString += temp;
				resultString += "\t\t\t\tComments:\n"s;
				resultString += printableString(currScenario->AssetAtIndex(i)->Comments(), 3);
			}
		}

		//Grid Filters in Scenario
		if (currScenario->GetFilterCount() != 0)
			resultString += _T("\t\tActive Grids and Patches:\n");
		for (int i = 0; i < (int)currScenario->GetFilterCount(); i++)
		{
			resultString += "\t\t\tName: " + currScenario->FilterAtIndex(i)->m_name + "\n";
		}

		//Weather Streams in Scenario
		if (currScenario->GetStreamCount() == 1)
			resultString += _T("\t\tWeather Stream:\n");
		else if (currScenario->GetStreamCount() > 1)
		{
			resultString += _T("\t\tPrimary Weather Stream:\n");
			if (currScenario->GetPrimaryStream())
				resultString += _T("\t\t\t") + currScenario->GetPrimaryStream()->m_name + _T("\n");
			else
				resultString += _T("\t\t\tNo primary weather stream is assigned.\n");
			resultString += _T("\t\tWeather Streams:\n");
		}
		for (int i = 0; i < (int)currScenario->GetStreamCount(); i++)
		{
			WeatherStream *ws = currScenario->StreamAtIndex(i);
			std::string name, streamName = ws->m_name;
			std::string stationName = ws->m_weatherStation()->m_name;
			if (streamName.length() == 0)
			{
				streamName = "(No Name)";
			}
			if (stationName.length() == 0)
			{
				stationName = "(No Name)";
			}
			name = strprintf("%s: %s ", stationName.c_str(), streamName.c_str());

			resultString += _T("\t\t\t") + name;

			ws->GetStartTime(tempStartTime);
			resultString += _T("\t\t[");
			resultString += tempStartTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE | WTIME_FORMAT_YEAR);
			ws->GetEndTime(tempEndTime);
			resultString += _T("] - [");
			resultString += tempEndTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE | WTIME_FORMAT_YEAR);
			resultString += _T("]\n");
		}
		if (PrntRptOptns.getScenarioComments() == 1)
		{
			resultString += _T("\t\tComments:\n");
			resultString += printableString(currScenario->m_comments, 2);
		}
	}
	if (PrntRptOptns.getApplicationSettings() == 1)
	{
		resultString += "\tBurning Conditions:\n";

		std::string temp;
		USHORT days;
		currScenario->GetBurnConditionDays(&days);

		for (unsigned short i = 0; i < days; i++)
		{
			CBurnPeriodOption condition(m_timeManager);
			currScenario->GetIndexBurnCondition(i, &condition);

			if (i == 0)
				resultString += "\t\tDate           Start       End   HISI >  HFWI >    WS >    RH <\n";
			tempStartTime = condition.m_time;
			tempStartTime.SetTimeManager(m_timeManager);
			temp = strprintf("\t\t%s", tempStartTime.ToString(WTIME_FORMAT_DATE | WTIME_FORMAT_STRING_YYYY_MM_DD | WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST).c_str());	resultString += temp;
			if (condition.eff_Start)
				temp = strprintf("%10s", condition.m_Start.ToString(WTIME_FORMAT_TIME).c_str());
			else
				temp = _T("            ");
			resultString += temp;

			if (condition.eff_End)
				temp = strprintf("%10s", condition.m_End.ToString(WTIME_FORMAT_TIME).c_str());
			else
				temp = _T("            ");
			resultString += temp;

			if (condition.eff_MinISI)
				temp = strprintf("%8.0lf", condition.m_MinISI);
			else
				temp = _T("        ");
			resultString += temp;

			if (condition.eff_MinFWI)
				temp = strprintf("%8.0lf", condition.m_MinFWI);
			else
				temp = _T("        ");
			resultString += temp;

			if (condition.eff_MaxWS)
				temp = strprintf("%8.2lf", condition.m_MaxWS);
			else
				temp = _T("        ");
			resultString += temp;

			if (condition.eff_MinRH)
				temp = strprintf("%8.0lf\n", condition.m_MinRH);
			else
				temp = _T("        \n");
			resultString += temp;
		}

		resultString += "\tFire Weather: ";
		BasicScenarioOptions basicOptions = currScenario->GetBasicOptions();

		if (basicOptions.m_bWeatherSpatialInterpolate) { resultString += "True\n"; }
		else { resultString += "False\n"; }

		if (basicOptions.m_bWeatherSpatialInterpolate)
		{
			int mode;
			if (basicOptions.m_bHistoryInterpolation)
				mode = 2;
			else if (basicOptions.m_bCalcSpatialFWI)
				mode = 1;
			else
				mode = 0;

			resultString += "\t\tSpatially interpolate FWI System values: ";
			if (mode == 0) { resultString += "True\n"; }
			else { resultString += "False\n"; }
			resultString += "\t\tCalculate FWI System values from spatially interpolated weather: ";
			if (mode == 1) { resultString += "True\n"; }
			else { resultString += "False\n"; }
			resultString += "\t\tRecursively calculate FWI System values from spatially interpolated weather: ";
			if (mode == 2) { resultString += "True\n"; }
			else { resultString += "False\n"; }
		}

		resultString += "\tFire Behavior:\n";
		FBPOptions fbpopt = currScenario->GetFBPOptions();

		resultString += "\t\tFMC Settings:\n";

		if (!fbpopt.m_specifiedFMCActive)
			resultString += "\t\t\tFMC (%) Override: False\n";
		else
		{
			temp = strprintf("%.3lf", fbpopt.m_specifiedFMC);
			resultString += "\t\t\tFMC (%) Override: " + temp + "\n";
		}

		resultString += "\t\tTerrain Effect On:                   ";
		if (fbpopt.m_topography) { resultString += " True\n"; }
		else { resultString += "False\n"; }

		resultString += "\t\tGreen-up on: ";
		WTimeSpan g_start, g_end;
		bool g_effective;
		currScenario->GetGreenupTimes(&g_start, &g_end, &g_effective);
		if (g_effective)
		{
			tempStartTime = currScenario->Simulation_StartTime();
			tempEndTime = tempStartTime;
			tempStartTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
			tempEndTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
			tempStartTime += g_start;
			tempEndTime += g_end;
			resultString += "                         True\n";
			std::string tempStr;
			tempStr = strprintf("\t\t\tStart Date: %s\n\t\t\tEnd Date:   %s\n",
				tempStartTime.ToString(WTIME_FORMAT_DATE | WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST).c_str(),
				tempEndTime.ToString(WTIME_FORMAT_DATE | WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST).c_str());
			resultString += tempStr;
		}
		else
		{
			resultString += "                        False\n";
		}

		resultString += "\t\tGrass Phenology on: ";
		currScenario->GetStandingGrassTimes(&g_start, &g_end, &g_effective);
		if (g_effective)
		{
			tempStartTime = currScenario->Simulation_StartTime();
			tempEndTime = tempStartTime;
			tempStartTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
			tempEndTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
			tempStartTime += g_start;
			tempEndTime += g_end;
			resultString += "True\n";
			std::string tempStr;
			tempStr = strprintf("\t\t\tStanding Start Date: %s\n\t\t\tStanding End Date:   %s\n",
				tempStartTime.ToString(WTIME_FORMAT_DATE | WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST).c_str(),
				tempEndTime.ToString(WTIME_FORMAT_DATE | WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST).c_str());
			resultString += tempStr;
		}
		else
		{
			resultString += "                 False\n";
		}

		resultString += "\t\tGrass Curing on: ";
		CuringOptions cOptions;
		currScenario->GetCuringDetails(cOptions);
		if (cOptions.curing.GetCount())
		{
			resultString += "True\n";
			std::string tempStr;
			tempStr = strprintf("\t\t\tStart Date       End Date         Degree of curing (%%)\n");
			resultString += tempStr;
			auto node = cOptions.curing.LH_Head();
			while (node->LN_Succ())
			{
				WTime startTime(currScenario->Simulation_StartTime());
				startTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				startTime += node->m_date;

				WTime endTime(currScenario->Simulation_StartTime());
				endTime.PurgeToYear(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
				if (node->LN_Succ()->LN_Succ())
					endTime += node->LN_Succ()->m_date;
				else
					endTime += WTimeSpan(365, 0, 0, 0);
				endTime -= WTimeSpan(1, 0, 0, 0);
				tempStr = strprintf("\t\t\t%-12s    %-12s    %13.1lf\n",
					startTime.ToString(WTIME_FORMAT_DATE | WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST).c_str(),
					endTime.ToString(WTIME_FORMAT_DATE | WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST).c_str(),
					node->m_curing);
				resultString += tempStr;
				node = node->LN_Succ();
			}
		}
		else
		{
			resultString += "                    False\n";
		}

		resultString += "\t\tBreaching: ";
		if (basicOptions.m_bBreaching) { temp = strprintf("%*s\n", 31, "True"); resultString += temp; }
		else { temp = strprintf("%*s\n", 31, "False"); resultString += temp; }

		resultString += "\t\tSpotting: ";
		if (basicOptions.m_bSpotting) { temp = strprintf("%*s\n", 32, "True"); resultString += temp; }
		else { temp = strprintf("%*s\n", 32, "False"); resultString += temp; }

		resultString += "\t\tPercentile Rate of Spread: ";
		if (fbpopt.m_growthPercentileEnabled) { temp = strprintf("%*s\n", 15, "True"); resultString += temp; }
		else { temp = strprintf("%*s\n", 15, "False"); resultString += temp; }

		if (fbpopt.m_growthPercentileEnabled)
		{
			resultString += "\t\t\tPercentile Rate of Spread: ";
			temp = strprintf("%*.1lf\n", 10, fbpopt.m_growthPercentile);
			resultString += temp;
		}

		resultString += _T("\tPropagation:\n");
		WTimeSpan displayIntr = basicOptions.m_displayInterval;

		resultString += _T("\t\tDisplay interval: ");
		resultString += displayIntr.ToString(WTIME_FORMAT_TIME);
		resultString += _T("\n");

		resultString += _T("\t\tMaximum time step during acceleration: ");
		resultString += basicOptions.m_temporalThreshold_Accel.ToString(WTIME_FORMAT_TIME);
		resultString += _T("\n");

		if (basicOptions.m_bDynamicSpatialThreshold)
			temp = strprintf("%*s", 8, "Dynamic");
		else
			temp = strprintf("%*.2lf", 8, basicOptions.m_spatialThreshold);
		resultString += "\t\tDistance resolution (Grid Cells): "s + temp + "\n"s;

		if (basicOptions.m_bDynamicSpatialThreshold)
			temp = strprintf("%*s", 7, "Dynamic");
		else
			temp = strprintf("%*.2lf", 7, basicOptions.m_perimeterResolution);
		resultString += "\t\tPerimeter resolution (Grid Cells): "s + temp + "\n"s;

		resultString += "\t\tStop fire spread at data boundary: ";
		if (basicOptions.m_bBoundaryStop) { temp = strprintf("%*s\n", 7, "True"); resultString += temp; }
		else { temp = strprintf("%*s\n", 7, "False"); resultString += temp; }

		if (basicOptions.m_bPurgeNonDisplayable)
			temp = strprintf("%*s", 8, "True");
		else
			temp = strprintf("%*s", 8, "False");
		resultString += "\t\tPurge non-displayable time steps: "s + temp + "\n"s;

		PolymorphicAttribute delta;
		bool dx, dy;
		double deltax, deltay;
		bool deltab;
		dx = SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_DX, &delta));
		if (dx)
			VariantToDouble_(delta, &deltax);
		else
			deltax = 0.0;
		dy = SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_DY, &delta));
		if (dy)
			VariantToDouble_(delta, &deltay);
		else
			deltay = 0.0;
		if ((deltax != 0.0) || (deltay != 0.0))
		{
			temp = strprintf("\t\tIgnition delta-X:    %.3lf\n", deltax);
			resultString += temp;
			temp = strprintf("\t\tIgnition delta-Y:    %.3lf\n", deltay);
			resultString += temp;
		}
		if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_DT, &delta)))
		{
			HSS_Time::WTimeSpan tdelta;
			VariantToTimeSpan_(delta, &tdelta);
			if (tdelta.GetTotalSeconds())
			{
				temp = strprintf("\t\tIgnition delta-Time: %s\n", tdelta.ToString(WTIME_FORMAT_TIME).c_str());
				resultString += temp;
			}
		}
		if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_OWD, &delta))) {
			VariantToDouble_(delta, &deltax);
			if (deltax != -1.0) {
				temp = strprintf("\t\tIgnition override-WD: %.0lf\n", deltax);
				resultString += temp;
			}
		}
		if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_DWD, &delta)))
		{
			VariantToDouble_(delta, &deltax);
			if (deltax != 0.0)
			{
				temp = strprintf("\t\tIgnition delta-WD: %.0lf\n", deltax);
				resultString += temp;
			}
		}
		if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_FALSE_ORIGIN, &delta)))
		{
			VariantToBoolean_(delta, &deltab);
			if (deltab)
				resultString += "\t\tFalse origin: enabled\n";
			else
				resultString += "\t\tFalse origin: disabled\n";
		}
		if (SUCCEEDED(currScenario->m_scenario->GetAttribute(CWFGM_SCENARIO_OPTION_FALSE_SCALING, &delta)))
		{
			VariantToBoolean_(delta, &deltab);
			if (deltab)
				resultString += "\t\tFalse scaling: enabled\n";
			else
				resultString += "\t\tFalse scaling: disabled\n";
		}
	}
	if (PrntRptOptns.getLandscapeProperties() == 1)
	{
		//Landscape Properties
		resultString += "\nLandscape Properties:\n"s;

		resultString += "\tGrid Information:\n"s;
		USHORT xdim, ydim;
		GetDimensions(&xdim, &ydim);
		double res;
		GetResolution(&res);
		double res_display = uc->ConvertUnit(res, STORAGE_FORMAT_M, uc->SmallDistanceDisplay());

		std::string temp;
		temp = strprintf("\t\tCell Size (%s): %.1lf\n\t\tColumns and Rows: %d x %d\n", uc->UnitName(uc->SmallDistanceDisplay(), true).c_str(), res_display, (int)xdim, (int)ydim);

		resultString += temp;

		std::string unit = uc->UnitName(uc->DistanceDisplay(), true);
		temp = strprintf("\t\tGrid Size: %.2lf %s x %.2lf %s\n",
			uc->ConvertUnit(xdim*res, STORAGE_FORMAT_M, uc->DistanceDisplay()), unit.c_str(),
			uc->ConvertUnit(ydim*res, STORAGE_FORMAT_M, uc->DistanceDisplay()), unit.c_str());

		resultString += temp;

		double latitude, longitude;
		GetLocation(&latitude, &longitude);
		temp = strprintf("\n\tLocation of Lower Left Corner:\n\t\t%s\n\n",
			FormatLatLon(RADIAN_TO_DEGREE(latitude), RADIAN_TO_DEGREE(longitude), (uc->CoordinateDisplay() == STORAGE_COORDINATE_RELATIVE_DISTANCE ? STORAGE_COORDINATE_DEGREE : uc->CoordinateDisplay()),
				uc, true).c_str());
		resultString += temp;

		temp = strprintf("\tElevation Statistics (%s):\n", uc->UnitName(uc->SmallDistanceDisplay(), true).c_str());
		resultString += temp;

		double _minelev, _maxelev;
		GetMinMaxElev(&_minelev, &_maxelev);
		temp = strprintf("\t\tMin:    %.1lf\n", uc->ConvertUnit(_minelev, STORAGE_FORMAT_M, uc->SmallDistanceDisplay()));
		resultString += temp;
		temp = strprintf("\t\tMax:    %.1lf\n", uc->ConvertUnit(_maxelev, STORAGE_FORMAT_M, uc->SmallDistanceDisplay()));
		resultString += temp;
		temp = strprintf("\t\tMean:   %.1lf\n", uc->ConvertUnit(GetMeanElevation(), STORAGE_FORMAT_M, uc->SmallDistanceDisplay()));
		resultString += temp;
		temp = strprintf("\t\tMedian: %.1lf\n\n", uc->ConvertUnit(GetMedianElevation(), STORAGE_FORMAT_M, uc->SmallDistanceDisplay()));
		resultString += temp;

		resultString += "\tTime Zone Settings:\n"s;
		resultString += "\t\tTime Zone: "s;

		if (DaylightSavings())
		{
			for (int i = 0; m_timeManager->m_worldLocation.m_dst_timezones[i].m_code != NULL; i++)
			{
				if (m_timeManager->m_worldLocation.m_dst_timezones[i].m_timezone == m_timeManager->m_worldLocation.m_timezone())
				{
					resultString += std::string(m_timeManager->m_worldLocation.m_dst_timezones[i].m_code) + " " + std::string(m_timeManager->m_worldLocation.m_dst_timezones[i].m_name) + " ";
					break;
				}
			}
			WTimeSpan ts = m_timeManager->m_worldLocation.m_timezone() + m_timeManager->m_worldLocation.m_amtDST();
			resultString += ts.ToString(WTIME_FORMAT_TIME);
			resultString += _T("\n");
		}
		else
		{
			for (int i = 0; m_timeManager->m_worldLocation.m_std_timezones[i].m_code != NULL; i++)
			{
				if (m_timeManager->m_worldLocation.m_std_timezones[i].m_timezone == m_timeManager->m_worldLocation.m_timezone())
				{
					resultString += std::string(m_timeManager->m_worldLocation.m_std_timezones[i].m_code) + " " + std::string(m_timeManager->m_worldLocation.m_std_timezones[i].m_name) + " ";
					break;
				}
			}
			resultString += m_timeManager->m_worldLocation.m_timezone().ToString(WTIME_FORMAT_TIME);
			resultString += "\n";
		}

		resultString += _T("\n\tProject Comments:\n");
		resultString += printableString(m_comments, 2);
	}
	if (PrntRptOptns.getInputFiles() == 1)
	{
		//Input Files
		resultString += "\nLandscape Grids:\n"s;
		resultString += "\tProjection:     "s + m_csProjectionName + "\n"s;
		resultString += "\tFuel Grid:      "s + m_csGridName + "\n"s;
		if (m_fuelCollection.m_csLUTFileName.has_value() && m_fuelCollection.m_csLUTFileName.value().length())
			resultString += "\tLook-up Table:  "s + m_fuelCollection.m_csLUTFileName.value() + "\n"s;

		resultString += "\tElevation Grid: "s;
		if (m_csElevationName.length() != 0)
			resultString += m_csElevationName + "\n"s;
		else
			resultString += "Not Imported!\n"s;

		for (ULONG i = 0; i < currScenario->GetFilterCount(); i++)
		{
			auto pFilter = dynamic_cast<Project::AttributeGridFilter*>(currScenario->FilterAtIndex(i));
			if (pFilter)
			{
				resultString += _T("\t") + pFilter->DisplayName() + _T("\n");
			}
		}
	}
	if (PrntRptOptns.getFBPFuelPatches() == 1)
	{
		//FBP Fuel Type Grid
		resultString += "\nFuel Patches:\n"s;

		if (currScenario->GetFilterCount() != 0) { resultString += _T("\tLandscape Fuel Type Patch:\n"); }
		for (ULONG i = 0; i < currScenario->GetFilterCount(); i++)
		{
			auto pFilter = dynamic_cast<Project::ReplaceGridFilter*>(currScenario->FilterAtIndex(i));
			if (pFilter)
			{
				WTime tempTime(m_timeManager);

				resultString += _T("\t\tName: ") + ((pFilter->m_name.length()) ? pFilter->m_name : _T("No Name")) + _T("\n");

				ICWFGM_Fuel *from_fuel, *to_fuel;
				UCHAR from_index;
				if (SUCCEEDED(pFilter->GetRelationship(&from_fuel, &from_index, &to_fuel)))
				{
					std::string from_name, to_name;
					if (from_fuel == (ICWFGM_Fuel *)~0)
					{
						from_name = _T("--- All Combustible Fuels ---");
					}
					else if (from_fuel == (ICWFGM_Fuel *)(-2))
					{
						from_name = _T("--- NODATA ---");
					}
					else if (from_fuel)
					{
						std::string bname;
						HRESULT hr = from_fuel->get_Name(&bname);
						if (FAILED(hr))
							throw std::logic_error("Failure");
						from_name = strprintf(_T("%s"), bname.c_str());
					}
					else if (from_index != (UCHAR)-1)
					{
						long file_index = -1, export_index;
						fuelMap()->FuelAtIndex(from_index, &file_index, &export_index, &from_fuel);
						from_name = strprintf(_T("Index %d:"), file_index);
					}
					else
						from_name = _T("--- All Fuels ---");

					if (to_fuel)
					{
						std::string bname;
						HRESULT hr = to_fuel->get_Name(&bname);
						if (FAILED(hr))
							throw std::logic_error("Failure");
						to_name = strprintf(_T("%s"), bname.c_str());
					}
					else
						to_name = _T("No Fuel");

					resultString += _T("\t\t\tFrom: ") + from_name + _T("\n");
					resultString += _T("\t\t\tTo:   ") + to_name + _T("\n");

					resultString += _T("\t\t\tComments:\n");
					resultString += printableString(pFilter->m_comments, 4);
				}
			}
		}

		if (currScenario->GetFilterCount() != 0) { resultString += _T("\tPolygon Fuel Type Patch:\n"); }
		for (ULONG i = 0; i < currScenario->GetFilterCount(); i++)
		{
			auto pFilter = dynamic_cast<Project::PolyReplaceGridFilter*>(currScenario->FilterAtIndex(i));
			if (pFilter)
			{
				WTime tempTime(m_timeManager);

				resultString += "\t\tName: " + ((pFilter->m_name.length()) ? pFilter->m_name : "No Name") + _T("\n");

				ICWFGM_Fuel *from_fuel, *to_fuel;
				UCHAR from_index;
				if (SUCCEEDED(pFilter->GetRelationship(&from_fuel, &from_index, &to_fuel)))
				{
					std::string from_name, to_name;
					if (from_fuel == (ICWFGM_Fuel *)~0)
					{
						from_name = _T("--- All Combustible Fuels ---");
					}
					else if (from_fuel == (ICWFGM_Fuel *)(-2))
					{
						from_name = _T("--- NODATA ---");
					}
					else if (from_fuel)
					{
						std::string bname;
						HRESULT hr = from_fuel->get_Name(&bname);
						if (FAILED(hr))
							throw std::logic_error("Failure");
						from_name = strprintf(_T("%s"), bname.c_str());
					}
					else if (from_index != (UCHAR)-1)
					{
						long file_index = -1, export_index;
						fuelMap()->FuelAtIndex(from_index, &file_index, &export_index, &from_fuel);
						from_name = strprintf(_T("Index %d:"), file_index);
					}
					else
						from_name = _T("--- All Fuels ---");

					if (to_fuel)
					{
						std::string bname;
						HRESULT hr = to_fuel->get_Name(&bname);
						if (FAILED(hr))
							throw std::logic_error("Failure");
						to_name = strprintf(_T("%s"), bname.c_str());
					}
					else
						to_name = _T("No Fuel");

					resultString += _T("\t\t\tFrom: ") + from_name + _T("\n");
					resultString += _T("\t\t\tTo:   ") + to_name + _T("\n");
				}

				double area = fabs(pFilter->Area());
				std::string areaUnit = ::UnitConversion::UnitName(uc->AreaDisplay(), true);
				area = ::UnitConversion::ConvertUnit(area, static_cast<UnitConvert::STORAGE_UNIT>(STORAGE_FORMAT_M2), uc->AreaDisplay());
				std::string areaStr;
				areaStr = strprintf(_T("\t\t\tArea: (%.3f %s)"), area, areaUnit.c_str());
				resultString += areaStr + _T("\n");

				resultString += _T("\t\t\tComments:\n");
				resultString += printableString(pFilter->m_comments, 4);
			}
		}
	}

	if (PrntRptOptns.getFBPFuelTypes())
	{
		//Start with unmodded fuels and add on modded after
		resultString += "Active Fuel Types :\n";
		std::string moddedResult = "\nActive Fuel Types (modified):\n";

		UCHAR		count, unique_count, ii;
		long		ASCII_index, tmp, export_index;

		ICWFGM_Fuel	*fuel = NULL;

		const FuelCollection *fc = &m_fuelCollection;
		CCWFGM_FuelMap *fuelMap = fc->m_fuelMap.get();
		fuelMap->GetFuelCount(&count, &unique_count);

		UCHAR i;
		for (i = 0; i < unique_count; i++)
		{
			if (SUCCEEDED(fuelMap->FuelAtIndex(i, &ASCII_index, &export_index, &fuel)))
			{
				Fuel *f = Fuel::FromCOM(fuel);

				//Check if the fuel is active
				if (f->m_flags & Fuel::FLAG_USED)
				{
					//Check if there's differences
					if ((f->m_flags & (Fuel::FLAG_MODIFIED | Fuel::FLAG_MODIFIED_NAME | Fuel::FLAG_NONSTANDARD)) && (!f->IsNonFuel()))
						moddedResult += f->PrintReportChanges(&m_fuelCollection);

					else
					{
						bool ret = FALSE;
						fuel->IsC6FuelType(&ret);

						if (ret)
							resultString += f->PrintOptionalParams(&m_fuelCollection, true);
						else
							resultString += f->PrintOptionalParams(&m_fuelCollection, false);
					}
				}
			}
		}

		resultString += moddedResult;
	}

	if (PrntRptOptns.getWxStreams() == 1)
	{
		for (ULONG i = 0; i < currScenario->GetStreamCount(); i++)
		{
			WeatherStream *ws = currScenario->StreamAtIndex(i);
			std::string temp;
			WTime tempTime(m_timeManager);

			resultString += "\nWeather Station:\n";
			resultString += "\tName: " + ws->m_weatherStation()->m_name + "\n";

			if (!ws->m_weatherStation()->m_comments.empty())
			{
				resultString += _T("\tComments:\n");
				resultString += printableString(ws->m_weatherStation()->m_comments, 4);
			}

			std::string loc = FormatLatLon(RADIAN_TO_DEGREE(ws->m_weatherStation()->Latitude()), RADIAN_TO_DEGREE(ws->m_weatherStation()->Longitude()), uc, true);

			resultString += "\tCoordinates:  " + loc + _T("\n");
			temp = strprintf("%.1lf %s", uc->ConvertUnit(ws->m_weatherStation()->Elevation(), STORAGE_FORMAT_M, uc->SmallDistanceDisplay()), uc->UnitName(uc->SmallDistanceDisplay(), true).c_str());
			resultString += "\tElevation:    " + temp + _T("\n");

			resultString += "\tWeather Stream Name: " + ws->m_name + _T("\n");

			ws->GetStartTime(tempTime);
			resultString += "\t\tStart Date: ";
			resultString += tempTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_YEAR | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE);
			resultString += "\n";

			ws->GetEndTime(tempTime);
			resultString += "\t\tEnd Date:   ";
			resultString += tempTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_YEAR | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE);
			resultString += "\n";

			temp = strprintf("\t\tImported From File:                     %s\n", ws->IsAnyImportedFromFile() ? "True" : "False"); resultString += temp;
			temp = strprintf("\t\tImported From Ensemble:                 %s\n", ws->IsAnyImportedFromEnsemble() ? "True" : "False"); resultString += temp;
			temp = strprintf("\t\tAny Data Generated From Diurnal Curves: %s\n", ws->IsAnyDailyObservations() ? "True" : "False"); resultString += temp;
			temp = strprintf("\t\tAny Data User Modified:                 %s\n", ws->IsAnyModified() ? "True" : "False"); resultString += temp;

			if (!ws->m_comments.empty())
			{
				resultString += _T("Comments:\n");
				resultString += printableString(ws->m_comments, 3);
			}

			resultString += "\t\tYesterday's Daily Starting Codes:\n";
			temp = strprintf("\t\t\tFFMC: %*.1lf\n", 36, ws->Initial_FFMC());
			resultString += temp;
			temp = strprintf("\t\t\tDMC: %*.1lf\n", 37, ws->Initial_DMC());
			resultString += temp;
			temp = strprintf("\t\t\tDC: %*.1lf\n", 38, ws->Initial_DC());
			resultString += temp;
			const char *m_precipTime;
			if (DaylightSavings())
				m_precipTime = _T("Precipitation (13:00:01 - 23:59:59):");
			else
				m_precipTime = _T("Precipitation (12:00:01 - 23:59:59):");
			temp = strprintf("\t\t\t%s %*.1lf %s\n",
				m_precipTime,
				5,
				uc->ConvertUnit(ws->Initial_Rain(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()),
				uc->UnitName(uc->SmallMeasureDisplay(), true).c_str());
			resultString += temp;


			resultString += "\t\tToday's Hourly Starting Code:\n";
			temp = strprintf("\t\t\tFFMC: %*.1lf\n", 36, ws->Initial_HFFMC());
			resultString += temp;

			WeatherOptions wo = ws->GetWeatherOptions();

			if (wo.m_FFMC == 2)
				temp = strprintf("\t\t\t@ Hour: %34s\n", "N/A");
			else
				temp = strprintf("\t\t\t@ Hour: %*d\n", 34, ws->Initial_HFFMCTime().GetHours());

			resultString += temp;

			resultString += "\t\tFWI Values:\n";
			if (ws->UseSpecifiedFWI())
				resultString += "\t\t\tUsing specified FWI System values\n";
			else
				resultString += "\t\t\tUsing calculated FWI System values\n";

			resultString += "\t\tMethod of Hourly FFMC Calculation:\n";

			if (wo.m_FFMC == 0)
				resultString += "\t\t\tVan Wagner\n";
			else if (wo.m_FFMC == 2)
				resultString += "\t\t\tLawson\n";

			if (ws->IsAnyDailyObservations()) {
				resultString += "\t\tDiurnal Parameters: \n";
				resultString += "\t\t\tTemperature:\n";
				temp = strprintf("\t\t\t\tAlpha: %*.3lf\n", 7, wo.m_temp_alpha);
				resultString += temp;
				temp = strprintf("\t\t\t\tBeta: %*.3lf\n", 8, wo.m_temp_beta);
				resultString += temp;
				temp = strprintf("\t\t\t\tGamma: %*.3lf\n", 7, wo.m_temp_gamma);
				resultString += temp;

				resultString += "\t\t\tWind:\n";
				temp = strprintf("\t\t\t\tAlpha: %*.3lf\n", 7, wo.m_wind_alpha);
				resultString += temp;
				temp = strprintf("\t\t\t\tBeta: %*.3lf\n", 8, wo.m_wind_beta);
				resultString += temp;
				temp = strprintf("\t\t\t\tGamma: %*.3lf\n", 7, wo.m_wind_gamma);
				resultString += temp;
			}
		}
	}
	if (PrntRptOptns.getWxPatchesAndWxGrids() == 1)
	{
		//Weather Patches and Weather Grids in Scenario
		resultString += _T("\nWeather Patches:\n");
		for (ULONG i = 0; i < currScenario->GetFilterCount(); i++)
		{
			auto pFilter = dynamic_cast<CWeatherPolyFilter*>(currScenario->FilterAtIndex(i));
			if (pFilter)
			{
				WTime tempTime(m_timeManager);
				std::string temp;

				resultString += _T("\tName: ") + pFilter->m_name + _T("\n");

				pFilter->GetStartTime(tempTime);
				resultString += "\t\tStart Time: ";
				resultString += tempTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE | WTIME_FORMAT_YEAR);
				resultString += "\n";
				pFilter->GetEndTime(tempTime);
				resultString += "\t\tEnd Time:   ";
				resultString += tempTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE | WTIME_FORMAT_YEAR);
				resultString += "\n";

				resultString += "\t\tOperations:\n";
				USHORT op;
				if ((op = pFilter->GetTemperature_Operation()) != (std::uint16_t)-1) {
					switch (op) {
					case 0: temp = strprintf("\t\t\tTemperature = %.1lf" DEGREE_SYMBOL "\n", pFilter->GetTemperature()); break;
					case 1: temp = strprintf("\t\t\tTemperature += %.1lf" DEGREE_SYMBOL "\n", pFilter->GetTemperature()); break;
					case 2: temp = strprintf("\t\t\tTemperature -= %.1lf" DEGREE_SYMBOL "\n", pFilter->GetTemperature()); break;
					case 3: temp = strprintf("\t\t\tTemperature *= %.1lf" DEGREE_SYMBOL "\n", pFilter->GetTemperature()); break;
					case 4: temp = strprintf("\t\t\tTemperature /= %.1lf" DEGREE_SYMBOL "\n", pFilter->GetTemperature()); break;
					}
					resultString += temp;
				}

				if ((op = pFilter->GetRH_Operation()) != (std::uint16_t)-1) {
					switch (op) {
					case 0: temp = strprintf("\t\t\tRH = %.1lf%%\n", pFilter->GetRH() * 100.0); break;
					case 1: temp = strprintf("\t\t\tRH += %.1lf%%\n", pFilter->GetRH() * 100.0); break;
					case 2: temp = strprintf("\t\t\tRH -= %.1lf%%\n", pFilter->GetRH() * 100.0); break;
					case 3: temp = strprintf("\t\t\tRH *= %.1lf%%\n", pFilter->GetRH() * 100.0); break;
					case 4: temp = strprintf("\t\t\tRH /= %.1lf%%\n", pFilter->GetRH() * 100.0); break;
					}
					resultString += temp;
				}

				if ((op = pFilter->GetPrecipitation_Operation()) != (std::uint16_t)-1) {
					switch (op) {
					case 0: temp = strprintf("\t\t\tPrecipitation = %.1lf %s\n", uc->ConvertUnit(pFilter->GetPrecipitation(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()), uc->UnitName(uc->SmallMeasureDisplay(), true).c_str()); break;
					case 1: temp = strprintf("\t\t\tPrecipitation += %.1lf %s\n", uc->ConvertUnit(pFilter->GetPrecipitation(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()), uc->UnitName(uc->SmallMeasureDisplay(), true).c_str()); break;
					case 2: temp = strprintf("\t\t\tPrecipitation -= %.1lf %s\n", uc->ConvertUnit(pFilter->GetPrecipitation(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()), uc->UnitName(uc->SmallMeasureDisplay(), true).c_str()); break;
					case 3: temp = strprintf("\t\t\tPrecipitation *= %.1lf %s\n", uc->ConvertUnit(pFilter->GetPrecipitation(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()), uc->UnitName(uc->SmallMeasureDisplay(), true).c_str()); break;
					case 4: temp = strprintf("\t\t\tPrecipitation /= %.1lf %s\n", uc->ConvertUnit(pFilter->GetPrecipitation(), STORAGE_FORMAT_MM, uc->SmallMeasureDisplay()), uc->UnitName(uc->SmallMeasureDisplay(), true).c_str()); break;
					}
					resultString += temp;
				}

				if ((op = pFilter->GetWS_Operation()) != (std::uint16_t)-1) {
					switch (op) {
					case 0: temp = strprintf("\t\t\tWind Speed = %.1lf %s\n", uc->ConvertUnit(pFilter->GetWindSpeed(), STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay()), uc->UnitName(uc->VelocityDisplay(), true).c_str()); break;
					case 1: temp = strprintf("\t\t\tWind Speed += %.1lf %s\n", uc->ConvertUnit(pFilter->GetWindSpeed(), STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay()), uc->UnitName(uc->VelocityDisplay(), true).c_str()); break;
					case 2: temp = strprintf("\t\t\tWind Speed -= %.1lf %s\n", uc->ConvertUnit(pFilter->GetWindSpeed(), STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay()), uc->UnitName(uc->VelocityDisplay(), true).c_str()); break;
					case 3: temp = strprintf("\t\t\tWind Speed *= %.1lf %s\n", uc->ConvertUnit(pFilter->GetWindSpeed(), STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay()), uc->UnitName(uc->VelocityDisplay(), true).c_str()); break;
					case 4: temp = strprintf("\t\t\tWind Speed /= %.1lf %s\n", uc->ConvertUnit(pFilter->GetWindSpeed(), STORAGE_FORMAT_KM | STORAGE_FORMAT_HOUR, uc->VelocityDisplay()), uc->UnitName(uc->VelocityDisplay(), true).c_str()); break;
					}
					resultString += temp;
				}

				if ((op = pFilter->GetWD_Operation()) != (std::uint16_t)-1) {
					switch (op) {
					case 0: temp = strprintf("\t\t\tWind Direction = %.1lf" DEGREE_SYMBOL "\n", pFilter->GetWindDirection()); break;
					case 1: temp = strprintf("\t\t\tWind Direction += %.1lf" DEGREE_SYMBOL "\n", pFilter->GetWindDirection()); break;
					case 2: temp = strprintf("\t\t\tWind Direction -= %.1lf" DEGREE_SYMBOL "\n", pFilter->GetWindDirection()); break;
					}
					resultString += temp;
				}

				resultString += _T("\t\tComments:\n");
				resultString += printableString(pFilter->m_comments, 4);
			}
		}
		resultString += "\nWeather Grids:\n";
		for (ULONG i = 0; i < (int)currScenario->GetFilterCount(); i++)
		{
			auto pFilter = dynamic_cast<CWeatherGridFilter*>(currScenario->FilterAtIndex(i));
			if (pFilter)
			{
				WTime tempTime(m_timeManager);

				resultString += _T("\tName: ") + pFilter->m_name + _T("\n");

				pFilter->GetStartTime(tempTime);
				resultString += _T("\t\tStart Time: ");
				resultString += tempTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE | WTIME_FORMAT_YEAR);
				resultString += "\n";
				pFilter->GetEndTime(tempTime);
				resultString += _T("\t\tEnd Time:   ");
				resultString += tempTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE | WTIME_FORMAT_YEAR);
				resultString += "\n";

				resultString += _T("\t\t\tComments:\n");
				resultString += printableString(pFilter->m_comments, 4);
			}
			else
			{
				auto xFilter = dynamic_cast<CWeatherGridBase*>(currScenario->FilterAtIndex(i));
				if (xFilter)
				{
					WTime tempTime(m_timeManager);
					WTimeSpan tempTimeSpan;

					resultString += _T("\tName: ") + xFilter->m_name + _T("\n");

					xFilter->GetStartTime(tempTime);
					resultString += _T("\t\tStart Time:        ");
					resultString += tempTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE | WTIME_FORMAT_YEAR);
					resultString += _T("\n");
					xFilter->GetEndTime(tempTime);
					resultString += _T("\t\tEnd Time:          ");
					resultString += tempTime.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE | WTIME_FORMAT_YEAR);
					resultString += _T("\n");
					xFilter->GetStartTimeOfDay(tempTimeSpan);
					resultString += _T("\t\tStart Time of Day: ");
					resultString += tempTimeSpan.ToString(WTIME_FORMAT_TIME);
					resultString += _T("\n");
					xFilter->GetEndTimeOfDay(tempTimeSpan);
					resultString += _T("\t\tEnd Time of Day:   ");
					resultString += tempTimeSpan.ToString(WTIME_FORMAT_TIME);
					resultString += _T("\n");

					auto yFilter = dynamic_cast<CWindDirectionGrid*>(xFilter);
					if (yFilter) {
						resultString += _T("\t\tWeather Grid Type: Wind Direction\n\t\tFiles:\n");
						for (USHORT j = 0; j < yFilter->GetSectorCount(); j++)
						{
							for (USHORT k = 0; k < yFilter->GetRangeCount(j); k++)
							{
								resultString += _T("\t\t\t");
								resultString += yFilter->GetRangeString(j, k);
								resultString += _T("\n");
							}
						}
					} else {
						auto zFilter = dynamic_cast<CWindSpeedGrid*>(xFilter);
						if (zFilter) {
							resultString += _T("\t\tWeather Grid Type: Wind Speed\n\t\tFiles:\n");
							for (USHORT j = 0; j < zFilter->GetSectorCount(); j++)
							{
								for (USHORT k = 0; k < zFilter->GetRangeCount(j); k++)
								{
									resultString += _T("\t\t\t");
									resultString += zFilter->GetRangeString(j, k);
									resultString += _T("\n");
								}
							}
						}
					}

					resultString += _T("\t\tComments:\n");
					resultString += printableString(xFilter->m_comments, 4);
				}
			}
		}
	}
	if (PrntRptOptns.getIgnitions() == 1)
	{
		//Fires in Scenario
		resultString += "\nIgnitions:\n";
		for (ULONG i = 0; i < currScenario->GetFireCount(); i++)
		{
			Fire *f = currScenario->FireAtIndex(i);
			resultString += "\tName: " + f->m_name + "\n";
			resultString += "\t\tStart Time: ";
			resultString += f->IgnitionTime().ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_YEAR | WTIME_FORMAT_WITHDST | WTIME_FORMAT_TIME | WTIME_FORMAT_DATE);
			resultString += "\n";
			resultString += "\t\tIgnition Type: ";
			if (f->IgnitionType(0) == CWFGM_FIRE_IGNITION_POINT)
			{
				std::uint32_t size = 0;
				std::uint16_t type;
				XY_Point *pt = f->GetIgnition(0, &type, &size);
				double lat, lon;
				if (pt)
				{
					GetLLByAbsoluteCoord(pt->x, pt->y, &lat, &lon);
					std::string loc = FormatLatLon(lat, lon, uc, true);
					std::string temp;
					temp = strprintf("Point (%s)\n", loc.c_str());
					resultString += temp;
				}
				delete[] pt;
			}
			else if (f->IgnitionType(0) == CWFGM_FIRE_IGNITION_LINE)
				resultString += "Line\n";
			else
				resultString += "Polygon\n";

			resultString += _T("\t\tComments:\n");
			resultString += printableString(currScenario->FireAtIndex(i)->m_comments, 3);
		}
	}

	//close file and free any memory allocated
	_ftprintf(outputFile, _T("%s"), resultString.c_str());
	fclose(outputFile);

	return S_OK;
}

#include "PermissibleVectorReadDrivers.h"
const char *Project::CWFGMProject::m_permissibleVectorReadDrivers[] = GDAL_READ_DRIVERS;
std::vector<std::string> Project::CWFGMProject::m_permissibleVectorReadDriversSA;

class StaticInitializer
{
public:
	StaticInitializer()
	{
		for (int iii = 0; iii < GDAL_READ_DRIVERS_LENGTH; iii++)
		{
			std::string c(Project::CWFGMProject::m_permissibleVectorReadDrivers[iii]);
			Project::CWFGMProject::m_permissibleVectorReadDriversSA.push_back(c);
		}
	}
};
static StaticInitializer init;
