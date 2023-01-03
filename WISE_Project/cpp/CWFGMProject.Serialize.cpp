/**
 * WISE_Project: CWFGMProject.Serialize.cpp
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
#include <winerror.h>
#endif
#if _DLL
#include "stdafx.h"
#endif //_MSC_VER

#include "CWFGMProject.h"
#include "ReplaceGridFilter.h"
#include "WeatherGridFilter.h"
#include "WindDirectionGrid.h"
#include "WindSpeedGrid.h"
#include "filesystem.hpp"

#include <gsl/util>

using namespace std::string_literals;


struct ComparableCallback
{
	std::function<void(void)> callback;
	unsigned int id;

	ComparableCallback(std::function<void(void)> _callback) { this->callback = _callback; id = cid; cid++; }
	ComparableCallback(const ComparableCallback& copy) { this->callback = copy.callback; id = copy.id; }

	ComparableCallback& operator=(const ComparableCallback& copy) { if (&copy != this) { this->callback = copy.callback; id = copy.id; } return *this; }

	void operator()() { callback(); }

	bool operator==(const ComparableCallback& other)
	{
		return id == other.id;
	}

private:
	static int cid;
};

int ComparableCallback::cid = 1;
std::vector<ComparableCallback> s_callbacks;

struct Caller
{
	template<typename F> void operator()(F f)
	{
		f();
	}
};

static void triggerCallbacks()
{
	for_each(s_callbacks.begin(), s_callbacks.end(), Caller());
}


const char *_internal_error_msg = "Internal Error - Please report this to the Prometheus support team.";
const struct Project::_error_msg _error_msgs[] = {
	{ E_OUTOFMEMORY,							"Insufficient memory." },
	{ ERROR_ACCESS_DENIED | ERROR_SEVERITY_WARNING,				"Cannot open file - access denied." },
	{ ERROR_INVALID_HANDLE | ERROR_SEVERITY_WARNING,			"Cannot open file - invalid handle." },
	{ ERROR_FILE_EXISTS | ERROR_SEVERITY_WARNING,				"Cannot open file - file already exists." },
	{ ERROR_INVALID_PARAMETER | ERROR_SEVERITY_WARNING,			"Invalid parameter arguement." },
	{ ERROR_TOO_MANY_OPEN_FILES | ERROR_SEVERITY_WARNING,			"Cannot open file - too many open files." },
	{ ERROR_FILE_NOT_FOUND | ERROR_SEVERITY_WARNING,			"Cannot open file - file not found." },
	{ ERROR_HANDLE_DISK_FULL | ERROR_SEVERITY_WARNING,			"Cannot open file - disk full." },
	{ ERROR_FILE_FORMAT_INVALID,						"Invalid/unknown file format." },
	{ E_POINTER,								"Invalid pointer." },
	{ E_INVALIDARG,								"Invalid parameter arguement." },
#if _DLL //TODO add more error codes
	{ ERROR_INVALID_ACCESS | ERROR_SEVERITY_WARNING,			"There is no valid PrometheusCOM license registered on this machine." },
#endif
	{ ERROR_GRID_UNINITIALIZED,						"Fuel grid is uninitialized." },
	{ ERROR_GRID_INITIALIZED,						"Fuel grid has already been initialized." },
	{ ERROR_GRID_LOCATION_OUT_OF_RANGE,					"Requested for data for a location out of range." },
	{ ERROR_GRID_SIZE_INCORRECT,						"The grid file was the incorrect size." },
	{ ERROR_GRID_UNSUPPORTED_RESOLUTION,						"The grid file was in an unsupported resolution." },
	{ ERROR_GRID_TIME_OUT_OF_RANGE,						"Requested data for an invalid time." },
	{ ERROR_GRID_WEATHER_INVALID_DATES,					"Weather data times will not work with the scenario start and end times." },
	{ ERROR_FUELS_GRIDVALUE_ALREADY_ASSIGNED,				"The specified grid value already exists for another fuel." },
	{ ERROR_FUELS_FUEL_UNKNOWN,						"Unknown Fuel Name." },
	{ ERROR_FIREBREAK_NOT_FOUND,						"Unknown Fire Break Name." },
	{ ERROR_SCENARIO_FIRE_UNKNOWN,						"Unknown Ignition Name." },
	{ ERROR_SCENARIO_VECTORENGINE_UNKNOWN,					"Unknown Vector Fire Break Name." },
	{ ERROR_SCENARIO_WEATHERGRID_UNKNOWN,					"Unknown Weather Grid Filter Name." },
	{ ERROR_WEATHER_STREAM_UNKNOWN,						"Unknown Weather Stream Name." },
	{ ERROR_WEATHER_STATION_UNKNOWN,					"Unknown Weather Station Name." },
	{ ERROR_WEATHER_STREAM_NOT_ASSIGNED,					"An attempt to work with a weather stream which isn't assigned to a weather station." },
	{ ERROR_WEATHER_STREAM_ALREADY_ASSIGNED,				"An attempt to assign a given weather stream to more than one weather station." },
	{ ERROR_WEATHER_STATION_ALREADY_PRESENT,				"An attempt to assign a given weather stream to the same weather station more than once." },
	{ ERROR_SCENARIO_FIRE_ALREADY_ADDED,					"An attempt to add an ignition to a scenario more than once." },
	{ ERROR_SCENARIO_VECTORENGINE_ALREADY_ADDED,				"An attempt to add a vector firebreak to a scenario more than once." },
	{ ERROR_SCENARIO_NO_FIRES,						"An attempt to reset a scenario with no fires added." },
	{ ERROR_SCENARIO_BAD_TIMES,						"Bad start or end times for the scenario." },
	{ ERROR_SCENARIO_BAD_TIMESTEPS,						"Bad/invalid calculation and output interval time steps." },
	{ ERROR_SCENARIO_INVALID_BURNCONDITIONS,				"Invalid burn condition settings (burning periods between days overlap)." },
	{ ERROR_SCENARIO_BAD_STATE,						"Simulation_Simulate was called before a Simulation_Restart." },
	{ ERROR_FIRE_INVALID_TIME,						"Request for statistics for an invalid time." },
	{ ERROR_FIRE_STAT_UNKNOWN,						"Request for an unknown statistics value." },
	{ ERROR_POINT_NOT_IN_FIRE,						"Request for a data point inside the fire perimeter, which actually is not." },
	{ E_NOTIMPL,								"Internal E_NOTIMPL Error - please report this to the Prometheus support team.", },
	{ E_UNEXPECTED,								"Internal E_UNEXPECTED Error - please report this to the Prometheus support team." },
	{ E_FAIL,								"Internal E_FAIL Error - Please report this to the Prometheus support team." },
	{ ERROR_NETWORK_UNREACHABLE | ERROR_SEVERITY_WARNING,			 "Unable to connect to a provided network URL." },
	{ E_NOINTERFACE,							"Internal E_NOINTERFACE Error - Please report this to the Prometheus support team." },
	{ ERROR_GRID_WEATHER_NOT_IMPLEMENTED,					_internal_error_msg },
	{ ERROR_GRID_WEATHER_NO_DATA,						_internal_error_msg },
	{ ERROR_FIRE_IGNITION_TYPE_UNKNOWN,					_internal_error_msg },
	{ ERROR_SCENARIO_UNKNOWN,						_internal_error_msg },
	{ ERROR_SCENARIO_OPTION_INVALID,					_internal_error_msg },
	{ ERROR_WEATHER_OPTION_INVALID,						_internal_error_msg },
	{ ERROR_SCENARIO_SIMULATION_RUNNING,					"Cannot perform the requested operation while a simulation is running." },
	{ ERROR_GRID_NO_DATA,							"No data at the point specified." },

	{ ERROR_GRID_WEATHER_NOT_IMPLEMENTED,					"An object associated with a simulation which can handle weather data is missing." },
	{ ERROR_GRID_WEATHER_INVALID_DATES,					"If the times for the weather don't work with the scenario start and end times." },
	{ ERROR_PROJECTION_UNKNOWN,						"Cannot parse the provided projection file." },

	{ ERROR_GRID_PRIMARY_STREAM_UNSPECIFIED,				"Multiple weather streams exist, but none have been identified as the primary weather stream." },
	{ ERROR_GRID_WEATHERSTATIONS_TOO_CLOSE,					"At least 2 weather stations are within 100m of each other." },
	{ ERROR_GRID_WEATHERSTREAM_TIME_OVERLAPS,				"One or more weather streams specify overlapping time intervals in the same weather station." },
	{ ERROR_GRID_WEATHER_STATION_ALREADY_PRESENT,				"Muliple weather streams from the same weather station exists; invalid." },

	{ ERROR_SCENARIO_FIRE_KNOWN,						"There is already an existing ignition with this name." },
	{ ERROR_SCENARIO_VECTORENGINE_KNOWN,					"A vector object with this name already exists." },
	{ ERROR_SCENARIO_KNOWN,							"A scenario with that name already exists." },

	{ ERROR_SCENARIO_MULTIPLE_WS_GRIDS,					"One or more wind speed grids apply at the same time." },
	{ ERROR_SCENARIO_MULTIPLE_WD_GRIDS,					"One or more wind direction grids apply at the same time." },

	{ ERROR_STATE_OBJECT_LOCKED,						_internal_error_msg },

	{ ERROR_GRIDFILTER_UNKNOWN,						"There is no grid filter at this index, or with the provided name." },
	{ ERROR_GRIDFILTER_KNOWN,						"An object with that name already exists." },

	{ ERROR_WEATHER_STREAM_ATTEMPT_PREPEND,					"Cannot prepend data to an existing weather stream." },
	{ ERROR_WEATHER_STREAM_ATTEMPT_APPEND,					"Attempt to append data to an existing weather stream failed." },
	{ ERROR_WEATHER_STREAM_ATTEMPT_OVERWRITE,				"Attempt to overwrite existing data in the weather stream failed." },
	{ WARNING_WEATHER_STREAM_INTERPOLATE,			"There were missing hours in the imported data that were filled using interpolation." },
	{ WARNING_WEATHER_STREAM_INTERPOLATE_BEFORE_INVALID_DATA, "There were missing hours in the imported data that were filled using interpolation, before encountering invalid data." },

	{ ERROR_ATTEMPT_EXPORT_EMPTY_FIRE,				"Cannot export empty fire perimeter." },

	{ ERROR_DATA_NOT_UNIQUE,						_internal_error_msg },
	{ ERROR_NAME_NOT_UNIQUE,						_internal_error_msg },
	{ ERROR_SECTOR_INVALID_INDEX,						"The sector index specified is out of range." },
	{ ERROR_SECTOR_TOO_SMALL,						"The sector angle specified span an angle less than MINIMUM_SECTOR_ANGLE." },
	{ ERROR_SPEED_OUT_OF_RANGE,						"The wind speed specified is out of range." },

	{ ERROR_PROTOBUF_OBJECT_INVALID,				"The protobuf object that we're encountering isn't what it should be." },
	{ ERROR_PROTOBUF_OBJECT_VERSION_INVALID,		"The protobuf object that we're encountering isn't an expected version." },


	{ (HRESULT)NULL,									"Undescribed Error - Please report this to the Prometheus support team." }
};

const struct Project::_error_msg *error_msgs() { return _error_msgs; };


HRESULT Project::CWFGMProject::SetGridFileData(const std::string& filename, const std::string& scenName, const HSS_Time::WTime &startTime,
	const HSS_Time::WTime &time, int interpMethod, int stat, int discretize)
{
	Project::GridOutputs grid;
	Project::GridOutput gridOutput;
	grid.startExportTime = startTime;
	grid.exportTime = time;
	gridOutput.filename = filename;
	grid.scenarioName = scenName;
	grid.interpolation = interpMethod;
	gridOutput.statistic = stat;
	grid.outputs.push_back(std::move(gridOutput));
	m_outputs.grids.push_back(std::move(grid));
	triggerCallbacks();
	return S_OK;
}


HRESULT Project::CWFGMProject::GetGridFileData(std::string* filename, std::string* scenName, HSS_Time::WTime *starttime,
	HSS_Time::WTime* time, int* interpMethod, int* stat, int *discretize)
{
	if (m_outputs.grids.size() > 0)
	{
		auto& grid = m_outputs.grids[0];
		if (grid.outputs.size() > 0)
		{
			auto& gridOutput = grid.outputs[0];

			if (filename)
				*filename = gridOutput.filename;
			if (scenName)
				*scenName = grid.scenarioName;

			if (starttime)
				*starttime = grid.startExportTime;
			if (time)
				*time = grid.exportTime;
			if (interpMethod)
				*interpMethod = grid.interpolation;
			if (stat)
				*stat = gridOutput.statistic;
			*discretize = 10;
			return S_OK;
		}
	}
	return (HRESULT)(-1);
}


HRESULT Project::CWFGMProject::GetVectorFileData(std::string *filename, std::string* scenName, HSS_Time::WTime* time,
	int* exportAll, int* remIslands, int* mergeContact, int* onlyActive)
{
	if (m_outputs.vectors.size() > 0)
	{
		auto& vector = m_outputs.vectors[0];
		if (filename)
			*filename = vector.filename;
		if (scenName)
			*scenName = vector.scenarioName;
		if (time)
			*time = vector.startTime;
		if (exportAll)
			*exportAll = vector.multiplePerimeters ? 1 : 0;
		if (remIslands)
			*remIslands = vector.removeIslands ? 1 : 0;
		if (mergeContact)
			*mergeContact = vector.mergeContacting ? 1 : 0;
		if (onlyActive)
			*onlyActive = vector.perimeterActive ? 1: 0;
		return S_OK;
	}
	return (HRESULT)(-1);
}

HRESULT Project::CWFGMProject::SetVectorFileData(const std::string& filename, const std::string& scenName,
	const HSS_Time::WTime &time, int exportAll, int remIslands, int mergeContact, int onlyActive)
{
	Project::VectorOutput file;
	file.filename = filename;
	file.scenarioName = scenName;
	file.startTime = time;
	file.endTime = time;
	file.multiplePerimeters = exportAll;
	file.removeIslands = remIslands;
	file.mergeContacting = mergeContact;
	file.perimeterActive = onlyActive;
	m_outputs.vectors.push_back(std::move(file));
	triggerCallbacks();
	return S_OK;
}

HRESULT Project::CWFGMProject::GetVectorFileMetadata(int* promVer, int* scenName, int* jobName, int* igName, int* simDate, int* fireSize, int* totalPerim, int* activePerim, int* wxValues, int* fwiValues,
	int *ignitionLocation, int *ignitionAttributes, int *maxBurnDistance, UnitConvert::STORAGE_UNIT* areaUnit, UnitConvert::STORAGE_UNIT* perimUnit)
{
	if (m_outputs.vectors.size() > 0)
	{
		auto& file = m_outputs.vectors[0];
		if (promVer)
			*promVer = (file.applicationVersion.has_value() && file.applicationVersion.value()) ? 1 : 0;
		if (scenName)
			*scenName = (file.outputScenarioName.has_value() && file.outputScenarioName.value()) ? 1 : 0;
		if (jobName)
			*jobName = (file.jobName.has_value() && file.jobName.value()) ? 1 : 0;
		if (igName)
			*igName = (file.ignitionName.has_value() && file.ignitionName.value()) ? 1 : 0;
		if (simDate)
			*simDate = (file.simulationDate.has_value() && file.simulationDate.value()) ? 1 : 0;
		if (fireSize)
			*fireSize = (file.fireSize.has_value() && file.fireSize.value()) ? 1 : 0;
		if (totalPerim)
			*totalPerim = (file.perimeterTotal.has_value() && file.perimeterTotal.value()) ? 1 : 0;
		if (activePerim)
			*activePerim = (file.perimeterActive.has_value() && file.perimeterActive.value()) ? 1 : 0;
		if (wxValues)
			*wxValues = (file.wxValues.has_value() && file.wxValues.value()) ? 1 : 0;
		if (fwiValues)
			*fwiValues = (file.fwiValues.has_value() && file.fwiValues.value()) ? 1 : 0;
		if (ignitionLocation)
			*ignitionLocation = (file.ignitionLocation.has_value() && file.ignitionLocation.value()) ? 1 : 0;
		if (ignitionAttributes)
			*ignitionAttributes = (file.ignitionAttributes.has_value() && file.ignitionAttributes.value()) ? 1 : 0;
		if (maxBurnDistance)
			*maxBurnDistance = (file.maxBurnDistance.has_value() && file.maxBurnDistance.value()) ? 1 : 0;
		if (areaUnit)
			*areaUnit = file.areaUnits.has_value() ? file.areaUnits.value() : -1;
		if (perimUnit)
			*perimUnit = file.perimeterUnits.has_value() ? file.perimeterUnits.value() : -1;
		return S_OK;
	}
	return (HRESULT)(-1);
}


HRESULT Project::CWFGMProject::SetVectorFileMetadata(int promVer, int scenName, int jobName, int igName, int simDate, int fireSize, int totalPerim, int activePerim, int wxValues, int fwiValues,
	int ignitionLocation, int ignitionAttributes, int maxBurnDistance, UnitConvert::STORAGE_UNIT areaUnit, UnitConvert::STORAGE_UNIT perimUnit)
{
	if (m_outputs.vectors.size() > 0)
	{
		auto& file = m_outputs.vectors[0];
		file.applicationVersion = promVer == 0 ? false : true;
		file.outputScenarioName = scenName == 0 ? false : true;
		file.jobName = jobName == 0 ? false : true;
		file.ignitionName = igName == 0 ? false : true;
		file.simulationDate = simDate == 0 ? false : true;
		file.fireSize = fireSize == 0 ? false : true;
		file.perimeterTotal = totalPerim == 0 ? false : true;
		file.perimeterActive = activePerim == 0 ? false : true;
		file.wxValues = wxValues == 0 ? false : true;
		file.fwiValues = fwiValues == 0 ? false : true;
		file.ignitionLocation = ignitionLocation == 0 ? false : true;
		file.ignitionAttributes = ignitionAttributes == 0 ? false : true;
		file.maxBurnDistance = maxBurnDistance == 0 ? false : true;
		if (areaUnit > 0)
			file.areaUnits = gsl::narrow_cast<std::uint8_t>(areaUnit);
		else
			file.areaUnits = std::nullopt;
		if (perimUnit > 0)
			file.perimeterUnits = gsl::narrow_cast<std::uint8_t>(perimUnit);
		else
			file.perimeterUnits = std::nullopt;
		triggerCallbacks();
		return S_OK;
	}
	return (HRESULT)(-1);
}


unsigned int Project::CWFGMProject::RegisterOutputDataListener(const std::function<void(void)> &callback)
{
	ComparableCallback back(callback);
	s_callbacks.push_back(back);
	return back.id;
}

void Project::CWFGMProject::UnregisterOutputDataListener(unsigned int id)
{
	for (std::vector<ComparableCallback>::iterator it = s_callbacks.begin(); it != s_callbacks.end(); ++it)
	{
		if (it->id == id)
		{
			s_callbacks.erase(it);
			break;
		}
	}
}


void Project::CWFGMProject::removeQuadrantWindGrids()
{
	GridFilter *gf = m_gridFilterCollection.FirstFilter();
	while (gf->LN_Succ())
	{
		auto df = dynamic_cast<CWindDirectionGrid*>(gf);
		auto sf = dynamic_cast<CWindSpeedGrid*>(gf);
		if ((df && df->GetSectorCount() == 4) || (sf && sf->GetSectorCount() == 4))
		{
			Project::Scenario *s = m_scenarioCollection.FirstScenario();
			while (s->LN_Succ())
			{
				s->RemoveFilter(gf);
				s = s->LN_Succ();
			}
			GridFilter *next = gf->LN_Succ();
			m_gridFilterCollection.RemoveFilter(gf);
			delete gf;
			gf = next;
		}
		else
			gf = gf->LN_Succ();
	}
}


void Project::CWFGMProject::CollectLoadWarnings()
{
	PolymorphicAttribute v;

	if (SUCCEEDED(m_fuelMap->GetAttribute(CWFGM_ATTRIBUTE_LOAD_WARNING, &v)))
	{
		std::string w1 = GetPolymorphicAttributeData(v, ""s);

		if (w1.length())
			m_loadWarnings += w1;
	}
	m_loadWarnings += m_fuelCollection.m_loadWarnings;

	if (SUCCEEDED(m_grid->GetAttribute(CWFGM_ATTRIBUTE_LOAD_WARNING, &v)))
	{
		std::string w2 = GetPolymorphicAttributeData(v, ""s);

		if (w2.length())
			m_loadWarnings += w2;
	}

	m_loadWarnings += m_fireCollection.CollectLoadWarnings();
	m_loadWarnings += m_weatherCollection.CollectLoadWarnings();
	m_loadWarnings += m_gridFilterCollection.CollectLoadWarnings();
	m_loadWarnings += m_vectorCollection.CollectLoadWarnings();
	m_loadWarnings += m_assetCollection.CollectLoadWarnings();
	m_loadWarnings += m_targetCollection.CollectLoadWarnings();
	m_loadWarnings += m_scenarioCollection.CollectLoadWarnings();
	m_loadWarnings += m_viewCollection.m_loadWarning;
}
