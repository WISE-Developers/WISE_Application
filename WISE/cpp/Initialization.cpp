/**
 * WISE_Application: Initialization.cpp
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

#ifndef _NO_MFC
#define _NO_MFC
#endif

#if _DLL
#include "stdafx.h"
#else //_DLL
#include "types.h"
#endif //_DLL
#include "SPARCS.h"
#include "ReplaceGridFilter.h"
#include "WeatherGridFilter.h"
#include "FontHelper.h"

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

#include "filesystem.hpp"
#include "VectorExportOptions.h"
#include "cpoints.cpp"
#include "misc_native.h"


struct _error_msg
{
	HRESULT code;
	const char* msg;
};

const char* _internal_error_msg = "Internal Error - Please report this to the Prometheus support team.";
const struct _error_msg _error_msgs[] = {
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
	{ ERROR_INVALID_ACCESS | ERROR_SEVERITY_WARNING,			"There is no valid PrometheusCOM license registered on this machine." },
	{ ERROR_GRID_UNINITIALIZED,						"Fuel grid is uninitialized." },
	{ ERROR_GRID_INITIALIZED,						"Fuel grid has already been initialized." },
	{ ERROR_GRID_LOCATION_OUT_OF_RANGE,					"Requested for data for a location out of range." },
	{ ERROR_GRID_SIZE_INCORRECT,						"The grid file was the incorrect size." },
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

	{ (HRESULT)0,									"Undescribed Error - Please report this to the Prometheus support team." }
};


const char* ErrorMsg(HRESULT hr, const char* msg)
{
	static char _msg[8192];
	if (msg)
	{
		_tcsncpy_s(_msg, msg, 8191);
		_msg[8191] = '\0';
		return _msg;
	}

	const struct _error_msg* e = _error_msgs;
	while (e->code != S_OK)
	{
		if (e->code == hr)
			break;
		e++;
	}
	return e->msg;
}


#define RETURN_MSG(hr, msg)	{ last_error_msg = ErrorMsg(hr, msg); last_error = hr; return (hr); }


void SPARCS_P::initMe2()
{
	m_project = nullptr;
	m_mqtt_msg = nullptr;
	m_mqtt_msg_len = 0;
	m_stop_requested = false;
	m_client.m_mqtt_qos = 0;
	m_client.m_mqtt_max_inflight = 20;
	m_client.m_callback = this;
	m_proto = nullptr;
	m_remainingScenarioCnt = -1;

	Project::CWFGMProject::m_appMode = -2;
	Project::CWFGMProject::m_managedMode = 1;
	m_event = neosmart::CreateEvent();
	m_lock = new CRWThreadSemaphore();
}


SPARCS_P::SPARCS_P()
	: m_unitConversion(NULL)
{
	SPARCS_P::initMe2();
}


SPARCS_P::~SPARCS_P()
{
	deinitMQTT();
	cleanupProtoStatus();
	delete m_lock;
	if (m_event)
		neosmart::DestroyEvent(m_event);
	if (m_project)
		delete m_project;
	if (m_proto)
		delete m_proto;
	m_rootValidation.reset();

	if (Project::FuelCollection::m_fuelCollection_Canada)		{ delete Project::FuelCollection::m_fuelCollection_Canada; Project::FuelCollection::m_fuelCollection_Canada = NULL; }
	if (Project::FuelCollection::m_fuelCollection_NewZealand)	{ delete Project::FuelCollection::m_fuelCollection_NewZealand; Project::FuelCollection::m_fuelCollection_NewZealand = NULL; }
	if (Project::FuelCollection::m_fuelCollection_Tasmania)		{ delete Project::FuelCollection::m_fuelCollection_Tasmania; Project::FuelCollection::m_fuelCollection_Tasmania = NULL; }

	Project::FuelCollection::m_fuelMap_Canada = nullptr;
	Project::FuelCollection::m_fuelMap_NewZealand = nullptr;
	Project::FuelCollection::m_fuelMap_Tasmania = nullptr;

	if (m_sharedMemory)
	{
		m_sharedMemory->shutdown();
		m_sharedMemory.reset();
	}
}


HRESULT SPARCS_P::Initialize_SPARCS_P()
{
	HRESULT hr;

    try
    {
    	if (!Project::FuelCollection::m_fuelCollection_Canada)
		{
			if (FAILED(hr = (Project::FuelCollection::m_fuelMap_Canada = new CCWFGM_FuelMap()) ? S_OK : E_FAIL))
				RETURN_MSG(hr, "FuelCom.DLL is not registered or an invalid version for this version of Prometheus.");
			if (FAILED(hr = (Project::FuelCollection::m_fuelMap_NewZealand = new CCWFGM_FuelMap()) ? S_OK : E_FAIL))
				RETURN_MSG(hr, "FuelCom.DLL is not registered or an invalid version for this version of Prometheus.");
			if (FAILED(hr = (Project::FuelCollection::m_fuelMap_Tasmania = new CCWFGM_FuelMap()) ? S_OK : E_FAIL))
				RETURN_MSG(hr, "FuelCom.DLL is not registered or an invalid version for this version of Prometheus.");

			Project::FuelCollection::m_fuelCollection_Canada = new Project::FuelCollection(Project::FuelCollection::m_fuelMap_Canada.get(), "Fuel Defaults");
			Project::FuelCollection::m_fuelCollection_NewZealand = new Project::FuelCollection(Project::FuelCollection::m_fuelMap_NewZealand.get(), "NZ Fuel Defaults");
			Project::FuelCollection::m_fuelCollection_Tasmania = new Project::FuelCollection(Project::FuelCollection::m_fuelMap_Tasmania.get(), "TAZ Fuel Defaults");
		}
		hr = S_OK;

		m_project = new Project::CWFGMProject();

		if (!m_fwi)
			m_fwi = std::make_unique<CCWFGM_FWI>();
    }
    catch (std::bad_alloc&)
    {
        hr = E_OUTOFMEMORY;
    }
	RETURN_MSG(hr, NULL);
}
