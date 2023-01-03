/**
 * WISE_Project: ScenarioCollection.cpp
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

#ifdef _MSC_VER
#include <afxdisp.h>
#include <comutil.h>
#include <afx.h>
#endif

#include "WTime.h"

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

#include <stdlib.h>
#include <ctime>
#include "ScenarioCollection.h"
#include "XY_PolyType.h"
#include <assert.h>

#include "CWFGMProject.h"
#include "GDALExporter.h"

#include "angles.h"
#include "WeatherGridFilter.h"
#include "str_printf.h"

#include <gsl/gsl>

using namespace std::string_literals;
using namespace HSS_Time;


#if defined(_DEBUG) && defined(_MSC_VER)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



Project::ScenarioCollection::ScenarioCollection(CWFGMProject *project,  const FBPOptions *fbpo, const BasicScenarioOptions *bso, const CBurnPeriodOption *bo, const CuringOptions *co) 
	: m_project(project),
    m_fbpOptions(fbpo),
	m_bsOptions(bso),
	m_burnOptions(bo),
	m_curingOptions(co)
{
	m_project->m_layerManager = new CCWFGM_LayerManager();

	Project::Scenario *scenario = new Project::Scenario(this, m_project->m_timeManager, nullptr);
	scenario->m_name="Scenario 1";
    WTime stm = WTime::Now(m_project->m_timeManager, WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_EXCLUDE_SECONDS);

#ifdef _DEBUG
	std::string time = stm.ToString(WTIME_FORMAT_STRING_ISO8601);
#endif

	scenario->Simulation_StartTime(stm);
	scenario->Simulation_EndTime(stm+WTimeSpan(0, 0, 1, 0));
	if (m_burnOptions)
		scenario->CorrectBurnConditionRange(*m_burnOptions, nullptr, "");

							// we always want at least one scenario, so we create one by default all
	AddScenario(scenario);				// the time...and attach it to ourselves
}


Project::ScenarioCollection::~ScenarioCollection()
{
	Project::Scenario *scenario;
	while (scenario = m_scenarioList.RemHead())
		delete scenario;
}


auto Project::ScenarioCollection::FindName(const char *name) const -> Scenario*
{
	Project::Scenario *s = m_scenarioList.LH_Head();
	while (s->LN_Succ())
	{
		if (s->m_name == name)
			return s;
		s = s->LN_Succ();
	}
	return NULL;
}


ULONG Project::ScenarioCollection::GetCountUsing(Fire *fire)
{
	ULONG cnt = 0;
	Project::Scenario *s = m_scenarioList.LH_Head();
	while (s->LN_Succ())
	{
		if (s->IndexOfFire(fire) != (ULONG)-1)
			cnt++;
		s = s->LN_Succ();
	}
	return cnt;
}


ULONG Project::ScenarioCollection::GetCountUsing(WeatherStream *stream)
{
	ULONG cnt = 0;
	Project::Scenario *s = m_scenarioList.LH_Head();
	while (s->LN_Succ())
	{
		if (s->IndexOfStream(stream) != (ULONG)-1)
			cnt++;
		s = s->LN_Succ();
	}
	return cnt;
}


ULONG Project::ScenarioCollection::GetCountUsing(WeatherStation *station)
{
	ULONG cnt = 0;
	Project::Scenario *s = m_scenarioList.LH_Head();
	while (s->LN_Succ())
	{
		WeatherStream *stream = station->FirstStream();
		while (stream->LN_Succ())
		{
			if (s->IndexOfStream(stream) != gsl::narrow_cast<ULONG>(-1))
			{
				cnt++;
				break;
			}
			stream = stream->LN_Succ();
		}
		s = s->LN_Succ();
	}
	return cnt;
}


ULONG Project::ScenarioCollection::GetCountUsing(GridFilter *filter)
{
	ULONG cnt = 0;
	Project::Scenario *s = m_scenarioList.LH_Head();
	while (s->LN_Succ())
	{
		if (s->IndexOfFilter(filter) != gsl::narrow_cast<ULONG>(-1))
			cnt++;
		s = s->LN_Succ();
	}
	return cnt;
}


ULONG Project::ScenarioCollection::GetCountUsing(Vector *vector)
{
	ULONG cnt = 0;
	Project::Scenario *s = m_scenarioList.LH_Head();
	while (s->LN_Succ())
	{
		if (s->IndexOfVector(vector) != gsl::narrow_cast<ULONG>(-1))
			cnt++;
		s = s->LN_Succ();
	}
	return cnt;
}


ULONG Project::ScenarioCollection::GetCountUsing(Asset* asset)
{
	ULONG cnt = 0;
	Scenario* s = m_scenarioList.LH_Head();
	while (s->LN_Succ())
	{
		if (s->IndexOfAsset(asset) != gsl::narrow_cast<ULONG>(-1))
			cnt++;
		s = s->LN_Succ();
	}
	return cnt;
}


bool Project::ScenarioCollection::AssignNewGrid(ICWFGM_GridEngine *oldgrid, ICWFGM_GridEngine *newgrid)
{
	Project::Scenario *s = m_scenarioList.LH_Head();

	ICWFGM_CommonData* data;
	if (FAILED(newgrid->GetCommonData(nullptr, &data)))
		return false;

	while (s->LN_Succ())
	{
		s->AssignNewGrid(oldgrid, newgrid);
		s->m_scenario->PutCommonData(s->m_layerThread, data);
		s = s->LN_Succ();
	}
	return true;
}


ULONG Project::Scenario::GetNumberFires(WTime &time)
{
	if (!m_childArray.size())
	{
		WTime t(time);
		std::uint32_t cnt;
		bool retval = SUCCEEDED(m_scenario->GetNumberFires(&t, &cnt));
		time = t; time.SetTimeManager(m_scenarioCollection->m_project->m_timeManager);
		if (retval)
			return cnt;
	}
	else
	{
		WTime t(time);
		std::uint32_t cnt, total = 0;
		bool r = false;
		for (auto s : m_childArray)
		{
			t = time;
			bool retval = SUCCEEDED(s->m_scenario->GetNumberFires(&t, &cnt));
			if (retval)
				total += cnt;
			r |= retval;
		}
		time = WTime(t, time.GetTimeManager());
		if (r)
			return total;
	}
	return 0;
}


bool Project::Scenario::GetStepsArray(std::uint32_t *size, std::vector<WTime> *times)
{
	WTime zero(0ULL, m_scenarioCollection->m_project->m_timeManager);
	std::vector<HSS_Time::WTime> safe_ig(*size, zero);
	bool retval;
	std::uint32_t _size = *size;
	HRESULT hr;
	if (m_childArray.size())
		hr = m_childArray[0]->m_scenario->GetStepsArray(&_size, &safe_ig);
	else
		hr = m_scenario->GetStepsArray(&_size, &safe_ig);
	if (SUCCEEDED(hr))
	{
		*size = _size;
		std::uint32_t i = 0;
		for (i = 0; i < (*size); i++)
			(*times)[i] = WTime(safe_ig[i].GetTotalSeconds(), m_scenarioCollection->m_project->m_timeManager);
		retval = true;
	}
	else
	{
		*size = _size;
		retval = false;
	}
	return retval;
}


CCWFGM_Scenario* Project::Scenario::getScenario(const WTime& time, std::uint32_t& f) const
{
	if (!m_childArray.size())
		return m_scenario.get();

	for (auto s : m_childArray)
	{
		WTime t(time);
		std::uint32_t cnt;
		if (SUCCEEDED(s->m_scenario->GetNumberFires(&t, &cnt)))
		{
			if (f < cnt)
				return s->m_scenario.get();
			f -= cnt;
		}
	}
	return m_scenario.get();		// this can happen before any fires exist, and we're passed 0
}


WTime Project::Scenario::Simulation_CurrentTime() const
{
	PolymorphicAttribute v;
	WTime time(0, m_scenarioCollection->m_project->m_timeManager);
	CCWFGM_Scenario* scenario;
	if (!m_childArray.size())
		scenario = m_scenario.get();
	else
		scenario = m_childArray[0]->m_scenario.get();
	scenario->GetAttribute(CWFGM_SCENARIO_OPTION_CURRENT_TIME, &v);
	VariantToTime_(v, &time);
	return WTime(time, m_timeManager);
}


// return the number of calculation intervals that we have a fire state
std::uint32_t Project::Scenario::GetNumberSteps()
{
	std::uint32_t steps;
	CCWFGM_Scenario* scenario;
	if (!m_childArray.size())
		scenario = m_scenario.get();
	else
		scenario = m_childArray[0]->m_scenario.get();
	if (SUCCEEDED(scenario->GetNumberSteps(&steps)))
		return steps;
	return 0;
}


XY_Point * Project::Scenario::GetVectorArray(std::uint32_t f, WTime &time, std::uint32_t *size)
{
	HRESULT hr;
	XY_Point *pt = NULL;
	std::uint32_t temp;
	CCWFGM_Scenario* scenario = getScenario(time, f);
	WTime t(time);
	if (!(*size)) 
	{
		hr = scenario->GetVectorSize(f, &t, &temp);

		if (FAILED(hr))
			return pt;
		*size = temp;
		if (!(*size))
			return pt;
	}

	XY_Poly poly(0, XY_Poly::ALLOCSTYLE_NEWDELETE);
	try
	{
		poly.SetNumPoints(*size);
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}

	hr = scenario->GetVectorArray(f, &t, size, &poly);
	if (SUCCEEDED(hr))
	{
		time = WTime(t.GetTotalSeconds(), m_scenarioCollection->m_project->m_timeManager);
	}
	else
	{
		delete [] pt;
		pt = nullptr;
	}
	return poly.Detach();
}


bool Project::Scenario::GetStatsArray(std::uint32_t f, WTime &time, SHORT stat, std::uint32_t *size, double *stats)
{
	CCWFGM_Scenario* scenario = getScenario(time, f);
	WTime t(time);
	std::vector<double> safe_stats;
	safe_stats.resize(*size);
	bool retval = SUCCEEDED(scenario->GetStatsArray(f, &t, stat, size, &safe_stats));
	if (retval)
	{
		for (ULONG i = 0; i < (*size); i++)
			stats[i] = safe_stats[i];
		time = WTime(t.GetTotalSeconds(), time.GetTimeManager()); 
	}
	return retval; 
}


// calculates and returns a generic stat for the entire fire
HRESULT Project::Scenario::GetStats(std::uint32_t fire, WTime& time, SHORT stat, std::uint16_t discretization, PolymorphicAttribute* stats)
{
	HRESULT hr;
	if ((fire != (std::uint32_t)-1) || (!m_childArray.size()))
	{
		CCWFGM_Scenario* scenario = getScenario(time, fire);
		WTime t(time);
		hr = scenario->GetStats(fire, nullptr, &t, stat, discretization, stats);
		time = t;
	}
	else
	{
		for (auto s : m_childArray)
		{
			WTime t(time);
			hr = s->m_scenario->GetStats(fire, nullptr, &t, stat, discretization, stats);
			time = t;
		}
	}
	return hr;
}


// calculates and returns a generic stat for the entire fire
HRESULT Project::Scenario::GetXYStats(const XY_Point& min_pt, const XY_Point &max_pt, WTime& mintime, WTime& time, std::uint16_t stat, std::uint32_t interp_method,
	std::uint16_t discretization, NumericVariant* stats) const
{
	XYStatOptions o;
	std::uint32_t f = 0;
	CCWFGM_Scenario* scenario = getScenario(time, f);
	o.mintime = mintime.GetTime(0);
	o.time = time.GetTime(0);
	o.interp_method = interp_method;
	o.discretization = discretization;
	HRESULT hr = scenario->GetXYStats(min_pt, max_pt, &o, stat, stats);
	time = WTime(o.time, time.GetTimeManager());
	return hr;
}


HRESULT Project::Scenario::GetXYStatsSet(const XY_Point& min_pt, const XY_Point& max_pt, WTime& mintime, WTime& time, std::uint32_t interp_method,
	USHORT discretization, std::vector<XYStat>& stats) const
{
	XYStatOptions o;
	std::uint32_t f = 0;
	CCWFGM_Scenario* scenario = getScenario(time, f);
	o.mintime = mintime.GetTime(0);
	o.time = time.GetTime(0);
	o.interp_method = interp_method;
	o.discretization = discretization;
	HRESULT hr = scenario->GetXYStatsSet(min_pt, max_pt, &o, &stats);
	time = WTime(o.time, time.GetTimeManager());
	return hr;
}


// calculates and returns a generic stat for the entire fire
HRESULT Project::Scenario::GetStatsPercentage(ULONG fire, WTime& time, SHORT stat, double greater_equal, double less_than, double* stats)
{
	HRESULT hr;
	if ((fire != (ULONG)-1) || !(m_childArray.size()))
	{
		WTime t(time);
		hr = m_scenario->GetStatsPercentage(fire, &t, stat, greater_equal, less_than, stats);
		time = t;
	}
	else
	{
		for (auto s : m_childArray)
		{
			WTime t(time);
			hr = s->m_scenario->GetStatsPercentage(fire, &t, stat, greater_equal, less_than, stats);
			time = t;
			if (FAILED(hr))
				break;
		}
	}
	return hr;
}


bool Project::ScenarioCollection::ExportScenarioInfo(FILE *fp)
{
	BOOL result;
	Project::Scenario *se = m_scenarioList.LH_Head();
	while(se->LN_Succ())
	{
		_fputts(_T("\n"),fp);
		result = se->ExportScenarioInfo(fp);
		if(!result)
			return false;
		se = se->LN_Succ();
	}
	return true;
}

bool Project::Scenario::ExportScenarioInfo(FILE *fp)
{
	std::string csBuf;
	WTime timeToExport(0ULL, m_timeManager);
// export scenario name
	_fputts((m_name + _T("\n")).c_str(), fp);
// export start time
	timeToExport=Simulation_StartTime();
	csBuf = timeToExport.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_ABBREV | WTIME_FORMAT_DATE | WTIME_FORMAT_TIME | WTIME_FORMAT_EXCLUDE_SECONDS);
	csBuf = _T("\tStart Time:\t") + csBuf;
	_fputts((csBuf + _T("\n")).c_str(), fp);
// export end time
	timeToExport = Simulation_EndTime();
	csBuf = timeToExport.ToString(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST | WTIME_FORMAT_ABBREV | WTIME_FORMAT_DATE | WTIME_FORMAT_TIME | WTIME_FORMAT_EXCLUDE_SECONDS);
	csBuf = _T("\tEnd Time:\t") + csBuf;
	_fputts((csBuf + _T("\n")).c_str(), fp);
	ExportScenarioFireInfo(fp);
	ExportScenarioWxStreamInfo(fp);
	ExportScenarioParameters(fp);
	return true;
}

void Project::Scenario::ExportScenarioFireInfo(FILE *fp)
{
	ULONG count=m_fireList.GetCount();
	for(ULONG i=0;i<count;i++)
	{
		Fire *f=FireAtIndex(i);
		f->ExportBasicInfo(fp);
	}
}

void Project::Scenario::ExportScenarioWxStreamInfo(FILE *fp)
{
	ULONG count=m_streamList.GetCount();
	for(ULONG i=0;i<count;i++)
	{
		WeatherStream *pStream=StreamAtIndex(i);
		pStream->ExportBasicInfo(fp);
	}
}

void Project::Scenario::ExportScenarioParameters(FILE *fp)
{
	BasicScenarioOptions m_basicScenarioOptions=GetBasicOptions();
	FBPOptions m_FBPOptions=GetFBPOptions();
	FBPOptions *m_fo=&m_FBPOptions;

	int m_secondsStep = (int)m_basicScenarioOptions.m_displayInterval.GetTotalSeconds();

	double m_perimeterResolution = m_basicScenarioOptions.m_perimeterResolution;

	BOOL m_topography = m_fo->m_topography;
	BOOL m_extinguishment = m_fo->m_extinguishment;

	BOOL m_bBoundaryStop = m_basicScenarioOptions.m_bBoundaryStop;

	_fputts(_T("\tParameters:\n"),fp);
	std::string csBuf;
	csBuf = strprintf(_T("\t\tDisplay interval:\t%d seconds\n"), m_secondsStep);
	_fputts(csBuf.c_str(),fp);
	csBuf = strprintf(_T("\t\tBoundary Stop:\t%s\n"), m_bBoundaryStop ? _T("YES") : _T("NO"));
	_fputts(csBuf.c_str(),fp);
	csBuf = strprintf(_T("\t\tPerimeter Resolution:\t%.2lf\n"), m_perimeterResolution);
	_fputts(csBuf.c_str(),fp);
	csBuf = strprintf(_T("\t\tTerrain effective:\t%s\n"), m_topography ? _T("YES") : _T("NO"));
	_fputts(csBuf.c_str(),fp);
	csBuf = strprintf(_T("\t\tExtinguishment:\t%s\n"), m_extinguishment ? _T("ON") : _T("OFF"));
	_fputts(csBuf.c_str(),fp);
}


HRESULT Project::Scenario::IsXYBurned(const XY_Point& pt, const WTime &time, bool &burned) const
{
	bool value, ivalue;
	HRESULT hr;
	if (!m_childArray.size())
	{
		if (SUCCEEDED(hr = m_scenario->IsXYBurned(pt, time, &value))) {
			burned = value ? true : false;
		}
		else {
			burned = false;
		}
	}
	else
	{
		burned = false;
		for (auto s : m_childArray)
		{
			WTime t(time);
			if (SUCCEEDED(hr = s->m_scenario->IsXYBurned(pt, time, &value)))
			{
				burned |= value ? true : false;
				if (burned)
					break;
			}
		}
	}
	return hr;
}


HRESULT Project::Scenario::GetBurnedBox(const WTime &time, XY_Rectangle *utm) const
{
	HRESULT hr;
	if (!m_childArray.size())
		hr = m_scenario->GetBurnedBox(time, utm);
	else
	{
		XY_Rectangle sr, cr;
		for (auto s : m_childArray)
		{
			if (SUCCEEDED(hr = s->m_scenario->GetBurnedBox(time, &sr)))
			{
				if (s == m_childArray[0])
					cr = sr;
				else
					cr.EncompassRectangle(sr);
			}
		}
		*utm = cr;
	}

	return hr;
}


bool Project::Scenario::ExportFilteredFuelGrid(const std::string &grid_file_name, CCWFGM_FuelMap *fuelMap, std::int32_t compression)
{
	if (grid_file_name.length() == 0)
		return false;

	std::uint8_t fuel_value;
	bool fuel_valid;
	ICWFGM_Fuel *fuel_pointer = NULL;
	ICWFGM_GridEngine *ge = gridEngine();
	std::uint16_t xsize, ysize;
	ge->GetDimensions(0, &xsize, &ysize);
	
	std::int32_t* l_array = new std::int32_t[xsize * ysize];
	std::int32_t* l_pointer = l_array;
	XY_Point pt;
	for(std::uint16_t i = ysize - 1; i < ysize; i--)
	{
		for(std::uint16_t j = 0; j < xsize; j++)
		{
			pt.x = m_scenarioCollection->m_project->invertX(((double)j) + 0.5);
			pt.y = m_scenarioCollection->m_project->invertY(((double)i) + 0.5);
			HRESULT hr = ge->GetFuelIndexData(layerThread(), pt, WTime(0, m_scenarioCollection->m_project->m_timeManager), &fuel_value, &fuel_valid, nullptr);
			hr = ge->GetFuelData(layerThread(), pt, WTime(0, m_scenarioCollection->m_project->m_timeManager), &fuel_pointer, &fuel_valid, nullptr);
			long file_index, export_index;
			if (hr == ERROR_FUELS_FUEL_UNKNOWN)
				export_index = -9999;
			else
			{
				ICWFGM_Fuel *fuel;
				fuelMap->FuelAtIndex(fuel_value, &file_index, &export_index, &fuel);
				if (fuel != fuel_pointer)
				{
					fuel_value = (std::uint8_t)-1;
					fuelMap->IndexOfFuel(fuel_pointer, &file_index, &export_index, &fuel_value);
				}
			}
			*l_pointer = export_index;
			l_pointer++;
		}
	}

	GDALExporter exporter;
	exporter.AddTag("TIFFTAG_SOFTWARE", "Prometheus");
	exporter.AddTag("TIFFTAG_GDAL_NODATA", "-9999");
	char mbstr[100];
	struct tm newtime;

#ifdef _MSC_VER
	__time64_t long_time;
	_time64(&long_time);
	_localtime64_s(&newtime, &long_time);
#else
    time_t t = std::time(0);
    localtime_r(&t, &newtime);
#endif

	std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S %Z", &newtime);
	exporter.AddTag("TIFFTAG_DATETIME", mbstr);
	double xllcorner,yllcorner;
	double resolution;
	m_scenarioCollection->m_project->GetResolution(&resolution);
	m_scenarioCollection->m_project->GetXLLCorner(&xllcorner);
	m_scenarioCollection->m_project->GetYLLCorner(&yllcorner);
	PolymorphicAttribute v;
	m_scenarioCollection->m_project->grid()->GetAttribute(CWFGM_GRID_ATTRIBUTE_SPATIALREFERENCE, &v);
	std::string ref;

	/*POLYMORPHIC CHECK*/
	try { ref = std::get<std::string>(v); } catch (std::bad_variant_access &) { weak_assert(0); return false; };

	const char* cref = ref.c_str();
	exporter.setProjection(cref);
	exporter.setExportCompress((GDALExporter::CompressionType)compression);
	exporter.setSize(xsize, ysize);
	exporter.setPixelResolution(resolution, resolution);
	exporter.setLowerLeft(xllcorner, yllcorner);
	GDALExporter::ExportResult res = exporter.Export(l_array, grid_file_name.c_str(), "Scenario Fuel Grid");
	
	if (l_array)
		delete [] l_array;
	
	if (res == GDALExporter::ExportResult::ERROR_ACCESS)
		return false;

	return true;
}


bool Project::Scenario::ExportHeader(FILE *fp)
{
	USHORT xSize, ySize;
	m_scenarioCollection->m_project->ExportHeader(fp, &xSize, &ySize);

	return true;
}


HRESULT Project::Scenario::GetBurnConditionDays(USHORT *days)
{
	HRESULT hr = m_temporalFilter->Count(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START, days);
	return hr;
}


HRESULT Project::Scenario::GetIndexBurnCondition(USHORT index, CBurnPeriodOption *option)
{
	WTimeVariant ulltime;
	m_temporalFilter->TimeAtIndex(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START, index, &ulltime);

	/*POLYMORPHIC CHECK*/
	try 
	{ 
		WTime wt(std::get<WTime>(ulltime)); 
		wt.SetTimeManager(m_scenarioCollection->m_project->m_timeManager);

		return GetDayBurnCondition(wt, option);
	}
	catch (std::bad_variant_access &)
	{ 
		weak_assert(0); return E_FAIL; 
	}
}


HRESULT Project::Scenario::ClearBurnConditions()
{
	USHORT days;
	HRESULT hr = m_temporalFilter->Count(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START, &days);
	if (SUCCEEDED(hr))
	{
		USHORT i = days;
		do
		{
			i--;
			m_temporalFilter->DeleteIndex(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START, i);
		} while (i > 0);
	}
	return hr;
}


HRESULT Project::Scenario::GetDayBurnCondition(const WTime &tday, CBurnPeriodOption *option)
{
	PolymorphicAttribute value;
	bool value_valid = false;
	LONGLONG lvalue;
	HRESULT hr;
	option->m_time = tday;
	if (SUCCEEDED(hr = m_temporalFilter->GetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_MIN_RH, tday, &value, &value_valid)))
	{
		VariantToDouble_(value, &option->m_MinRH);
		option->m_MinRH *= 100.0;
		option->eff_MinRH = value_valid ? true : false;
	}
	if (SUCCEEDED(hr = m_temporalFilter->GetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_MIN_FWI, tday, &value, &value_valid)))
	{
		VariantToDouble_(value, &option->m_MinFWI);
		option->eff_MinFWI = value_valid ? true : false;
	}
	if (SUCCEEDED(hr = m_temporalFilter->GetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_MIN_ISI, tday, &value, &value_valid)))
	{
		VariantToDouble_(value, &option->m_MinISI);
		option->eff_MinISI = value_valid ? true : false;
	}
	if (SUCCEEDED(hr = m_temporalFilter->GetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_MAX_WS, tday, &value, &value_valid)))
	{
		VariantToDouble_(value, &option->m_MaxWS);
		option->eff_MaxWS = value_valid ? true : false;
	}
	if (SUCCEEDED(hr = m_temporalFilter->GetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START_COMPUTED, tday, &value, &value_valid)))
	{
		/*POLYMORPHIC CHECK*/
		WTimeSpan w;
		VariantToTimeSpan_(value, &w);

		option->m_Start = w;
		option->eff_Start = value_valid ? true : false;
	}
	if (SUCCEEDED(hr = m_temporalFilter->GetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_END_COMPUTED, tday, &value, &value_valid)))
	{
		/*POLYMORPHIC CHECK*/
		WTimeSpan w;
		VariantToTimeSpan_(value, &w);

		option->m_End = w;
		option->eff_End = value_valid ? true : false;
	}
	return hr;
}


HRESULT Project::Scenario::SetDayBurnCondition(const WTime &tday, const CBurnPeriodOption &option)
{
	PolymorphicAttribute value;
	HRESULT hr;
	value = option.m_MinRH * 0.01;		hr = m_temporalFilter->SetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_MIN_RH, tday, value, option.eff_MinRH ? true : false);
	value = option.m_MinFWI;			hr = m_temporalFilter->SetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_MIN_FWI, tday, value, option.eff_MinFWI ? true : false);
	value = option.m_MinISI;			hr = m_temporalFilter->SetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_MIN_ISI, tday, value, option.eff_MinISI ? true : false);
	value = option.m_MaxWS;				hr = m_temporalFilter->SetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_MAX_WS, tday, value, option.eff_MaxWS ? true : false);
	value = option.m_Start;				hr = m_temporalFilter->SetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START, tday, value, option.eff_Start ? true : false);
	value = option.m_End;				hr = m_temporalFilter->SetOptionKey(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_END, tday, value, option.eff_End ? true : false);
	return hr;
}


bool Project::Scenario::CorrectBurnConditionRange(const CBurnPeriodOption &defaultoption, std::shared_ptr<validation::validation_object> valid, const std::string& name)
{
	bool changed = false;
	USHORT days, i;
	WTimeVariant ulltime;
	if (FAILED(m_temporalFilter->Count(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START, &days)))
		return false;

	WTime start = Simulation_StartTime();
	WTime end = Simulation_EndTime();
	start.PurgeToDay(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);
	end.PurgeToDay(WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST);

	for (i = days - 1; i < days; i--)
	{
		m_temporalFilter->TimeAtIndex(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START, i, &ulltime);
		HSS_Time::WTime time = std::get<WTime>(ulltime);
		time.SetTimeManager(start.GetTimeManager());
		if ((time < start) || (time > end))
		{
			if (valid)
				/// <summary>
				/// A burn condition doesn't overlap with the scenario simulation time, delete it.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.BurnCondition", name, validation::error_level::WARNING, "Condition.Correct:Delete", std::to_string(i));
			m_temporalFilter->DeleteIndex(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START, i);
			changed = true;
		}
	}

	while (start <= end)
	{
		bool found = false;
		m_temporalFilter->Count(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START, &days);
		for (i = 0; i < days; i++)
		{
			m_temporalFilter->TimeAtIndex(CWFGM_GRID_ATTRIBUTE_BURNINGCONDITION_PERIOD_START, i, &ulltime);
			HSS_Time::WTime time = std::get<WTime>(ulltime);
			time.SetTimeManager(start.GetTimeManager());
			if (time == start)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			if (valid)
				/// <summary>
				/// The burn conditions for a simulated day were not set, the default was used insted.
				/// </summary>
				/// <type>user</type>
				valid->add_child_validation("WISE.ProjectProto.BurnCondition", name, validation::error_level::WARNING, "Condition.Correct:Missing", "default");
			SetDayBurnCondition(start, defaultoption);
			changed = true;
		}
		start += WTimeSpan(1, 0, 0, 0);
	}
	return changed;
}
