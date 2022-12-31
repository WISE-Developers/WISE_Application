/**
 * WISE_Project: BasicScenarioOptions.cpp
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
#include "stdafx.h"
#include "cwinapp.h"
#define BOOL_CHECK ? true : false
#else
#include "AfxIniSettings.h"
#define BOOL_CHECK
#include <boost/format.hpp>
#endif
#include "BasicScenarioOptions.h"
#include "propsysreplacement.h"
#include "FireEngine_ext.h"
#include "FuelCom_ext.h"
#include "Thread.h"
#include "results.h"


Project::BasicScenarioOptions::BasicScenarioOptions()
{
	m_perimeterResolution = 1.0;
	m_perimeterSpacing = 0.001;
	m_spatialThreshold = 1.0;
	m_temporalThreshold_Accel = WTimeSpan(0, 0, 2, 0);
	m_displayInterval = WTimeSpan(0, 1, 0, 0);

	m_bBreaching = true;
	m_bSpotting = false;
	m_bBoundaryStop = false;
	m_bWeatherTemporalInterpolate = false;
	m_bWeatherSpatialInterpolate = false;
	m_bPrecipitationInterpolation = true;
	m_bWindInterpolation = true;
	m_bWindInterpolationVector = true;
	m_bCalcSpatialFWI = true;
	m_bTempRHInterpolation = true;
	m_bHistoryInterpolation = true;
	m_bDynamicSpatialThreshold = true;
	m_bPurgeNonDisplayable = false;
	m_bSuppressConcaveTightFill = false;
	m_bCardinalDirection = false;
	m_bIndependentFires = false;
}


Project::BasicScenarioOptions::BasicScenarioOptions(const TCHAR *group_name)
{
#if _DLL
	CWinAppExt *app = (CWinAppExt *)AfxGetApp();
#else
    IniSettings *app = AfxGetApp();
#endif
	m_perimeterResolution = app->GetProfileDouble(group_name, _T("Distance Resolution"), 1.0);
	m_spatialThreshold = app->GetProfileDouble(group_name, _T("Spatial Threshold"), 1.0);
	m_temporalThreshold_Accel = WTimeSpan(app->GetProfileLongLong(group_name, _T("AccelerationTemporal Threshold"), 2 * 60));
	bool b = app->GetProfileBOOL(group_name, _T("Fixed Accel Threshold"), false) BOOL_CHECK;
	if (!b)
	{
		if (m_temporalThreshold_Accel == WTimeSpan(0, 0, 2, 0))
			m_temporalThreshold_Accel = WTimeSpan(0, 0, 2, 0);		// RWB: copy-paste bug found 080806 - quietly reset behind the scenes
		app->WriteProfileBOOL(group_name, _T("Fixed Accel Threshold"), true);
	}
	m_displayInterval = WTimeSpan(app->GetProfileLongLong(group_name, _T("Display Time Step"), 60 * 60));

	m_bBoundaryStop = app->GetProfileBOOL(group_name, _T("Boundary Stop"), false) BOOL_CHECK;
	m_bBreaching = app->GetProfileBOOL(group_name, _T("Breaching"), true) BOOL_CHECK;
	m_bSpotting = app->GetProfileBOOL(group_name, _T("Spotting"), false) BOOL_CHECK;
	m_bWeatherTemporalInterpolate = false;
	m_bWeatherSpatialInterpolate = app->GetProfileBOOL(group_name, _T("Spatially Interpolate Weather"), false) BOOL_CHECK;
	m_bPrecipitationInterpolation = app->GetProfileBOOL(group_name, _T("Spatially Interpolate Precipitation"), true) BOOL_CHECK;
	m_bWindInterpolation = app->GetProfileBOOL(group_name, _T("Spatially Interpolate Wind"), true) BOOL_CHECK;
	m_bWindInterpolationVector = app->GetProfileBOOL(group_name, _T("Vector Addition for Wind Interpolation"), true) BOOL_CHECK;
	m_bTempRHInterpolation = app->GetProfileBOOL(group_name, _T("Spatially Interpolate Temp, RH"), true) BOOL_CHECK;
	m_bCalcSpatialFWI = app->GetProfileBOOL(group_name, _T("Spatially Calculate FWI Values"), true) BOOL_CHECK;
	m_bHistoryInterpolation = app->GetProfileBOOL(group_name, _T("Spatially Interpolate with History"), true) BOOL_CHECK;
	m_bDynamicSpatialThreshold = app->GetProfileBOOL(group_name, _T("Dynamic Spatial Threshold"), true) BOOL_CHECK;
	m_bPurgeNonDisplayable = app->GetProfileBOOL(group_name, _T("Purge NonDisplayable Timesteps"), false) BOOL_CHECK;
	m_bCardinalDirection = app->GetProfileBOOL(group_name, _T("Furthest Travelled"), false) BOOL_CHECK;
	m_perimeterSpacing = 0.001;
	m_bSuppressConcaveTightFill = false;
	m_bIndependentFires = false;
}


void Project::BasicScenarioOptions::SaveToIniFile(const TCHAR *group_name) const
{
#if _DLL
	CWinAppExt *app = (CWinAppExt *)AfxGetApp();
#else
    IniSettings *app = AfxGetApp();
#endif

	app->WriteProfileDouble(group_name, _T("Distance Resolution"), m_perimeterResolution, _T("%.7lf"));
	app->WriteProfileDouble(group_name, _T("Spatial Threshold"), m_spatialThreshold, _T("%.7lf"));
	app->WriteProfileLongLong(group_name, _T("AccelerationTemporal Threshold"), m_temporalThreshold_Accel.GetTotalSeconds());
	app->WriteProfileLongLong(group_name, _T("Display Time Step"), m_displayInterval.GetTotalSeconds());

	app->WriteProfileBOOL(group_name, _T("Boundary Stop"), m_bBoundaryStop);
	app->WriteProfileBOOL(group_name, _T("Breaching"), m_bBreaching);
	app->WriteProfileBOOL(group_name, _T("Spotting"), m_bSpotting);
	app->WriteProfileBOOL(group_name, _T("Temporally Interpolate Weather"), m_bWeatherTemporalInterpolate);
	app->WriteProfileBOOL(group_name, _T("Spatially Interpolate Weather"), m_bWeatherSpatialInterpolate);
	app->WriteProfileBOOL(group_name, _T("Spatially Interpolate Precipitation"), m_bPrecipitationInterpolation);
	app->WriteProfileBOOL(group_name, _T("Spatially Interpolate Wind"), m_bWindInterpolation);
	app->WriteProfileBOOL(group_name, _T("Vector Addition for Wind Interpolation"), m_bWindInterpolationVector);
	app->WriteProfileBOOL(group_name, _T("Spatially Interpolate Temp, RH"), m_bTempRHInterpolation);
	app->WriteProfileBOOL(group_name, _T("Spatially Calculate FWI Values"), m_bCalcSpatialFWI);
	app->WriteProfileBOOL(group_name, _T("Spatially Interpolate with History"), m_bHistoryInterpolation);
	app->WriteProfileBOOL(group_name, _T("Dynamic Spatial Threshold"), m_bDynamicSpatialThreshold);
	app->WriteProfileBOOL(group_name, _T("Purge NonDisplayable Timesteps"), m_bPurgeNonDisplayable);
	app->WriteProfileBOOL(group_name, _T("Furthest Travelled"), m_bCardinalDirection);
}


Project::BasicScenarioOptions::BasicScenarioOptions(const BasicScenarioOptions &toCopy)
{
	if (&toCopy)
		*this = toCopy;
	else
	{
		m_perimeterResolution = 1.0;
		m_perimeterSpacing = 0.0;
		m_spatialThreshold = 1.0;
		m_temporalThreshold_Accel = WTimeSpan(0, 0, 2, 0);
		m_displayInterval = WTimeSpan(0, 1, 0, 0);

		m_bBreaching = true;
		m_bSpotting = false;
		m_bBoundaryStop = false;
		m_bWeatherTemporalInterpolate = false;
		m_bWeatherSpatialInterpolate = false;
		m_bPrecipitationInterpolation = true;
		m_bWindInterpolation = true;
		m_bWindInterpolationVector = true;
		m_bTempRHInterpolation = true;
		m_bCalcSpatialFWI = true;
		m_bHistoryInterpolation = true;
		m_bDynamicSpatialThreshold = true;
		m_bPurgeNonDisplayable = false;
		m_bSuppressConcaveTightFill = false;
		m_bCardinalDirection = false;
		m_bIndependentFires = false;
	}
}


Project::BasicScenarioOptions::BasicScenarioOptions(CCWFGM_Scenario *scenario)
{
	PolymorphicAttribute v;

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_PERIMETER_RESOLUTION, &v)))				VariantToDouble_(v, &m_perimeterResolution);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_PERIMETER_SPACING, &v)))					VariantToDouble_(v, &m_perimeterSpacing);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_SPATIAL_THRESHOLD, &v)))					VariantToDouble_(v, &m_spatialThreshold);

	scenario->GetAttribute(CWFGM_SCENARIO_OPTION_TEMPORAL_THRESHOLD_ACCEL, &v);							VariantToTimeSpan_(v, &m_temporalThreshold_Accel);

	scenario->GetAttribute(CWFGM_SCENARIO_OPTION_DISPLAY_INTERVAL, &v);									VariantToTimeSpan_(v, &m_displayInterval);

	scenario->GetAttribute(CWFGM_SCENARIO_OPTION_BOUNDARY_STOP, &v);									VariantToBoolean_(v, &m_bBoundaryStop);

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_BREACHING, &v)))							VariantToBoolean_(v, &m_bBreaching);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_SPOTTING, &v)))							VariantToBoolean_(v, &m_bSpotting);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_TEMPORAL, &v)))		VariantToBoolean_(v, &m_bWeatherTemporalInterpolate);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_SPATIAL, &v)))		VariantToBoolean_(v, &m_bWeatherSpatialInterpolate);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_PRECIP, &v)))		VariantToBoolean_(v, &m_bPrecipitationInterpolation);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_WIND, &v)))			VariantToBoolean_(v, &m_bWindInterpolation);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_WIND_VECTOR, &v)))	VariantToBoolean_(v, &m_bWindInterpolationVector);
	weak_assert(m_bWindInterpolationVector);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_TEMP_RH, &v)))		VariantToBoolean_(v, &m_bTempRHInterpolation);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_CALCFWI, &v)))		VariantToBoolean_(v, &m_bCalcSpatialFWI);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_HISTORY, &v)))		VariantToBoolean_(v, &m_bHistoryInterpolation);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_SPATIAL_THRESHOLD_DYNAMIC, &v)))			VariantToBoolean_(v, &m_bDynamicSpatialThreshold);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_PURGE_NONDISPLAYABLE, &v)))				VariantToBoolean_(v, &m_bPurgeNonDisplayable);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_SUPPRESS_TIGHT_CONCAVE_ADDPOINT, &v)))	VariantToBoolean_(v, &m_bSuppressConcaveTightFill);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_CARDINAL_ROS, &v)))						VariantToBoolean_(v, &m_bCardinalDirection);
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_INDEPENDENT_TIMESTEPS, &v)))				VariantToBoolean_(v, &m_bIndependentFires);
}


bool Project::BasicScenarioOptions::SaveToScenario(CCWFGM_Scenario *scenario) const
{
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_PERIMETER_RESOLUTION, m_perimeterResolution)))					return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_PERIMETER_SPACING, m_perimeterSpacing)))						return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_SPATIAL_THRESHOLD, m_spatialThreshold)))						return false;

	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_TEMPORAL_THRESHOLD_ACCEL, m_temporalThreshold_Accel)))			return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_DISPLAY_INTERVAL, m_displayInterval)))							return false;

	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_BOUNDARY_STOP, m_bBoundaryStop)))								return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_BREACHING, m_bBreaching)))										return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_SPOTTING, m_bSpotting)))										return false;

	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_TEMPORAL, m_bWeatherTemporalInterpolate)))	return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_SPATIAL, m_bWeatherSpatialInterpolate)))	return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_PRECIP, m_bPrecipitationInterpolation)))	return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_WIND, m_bWindInterpolation)))				return false;
	weak_assert(m_bWindInterpolationVector);
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_WIND_VECTOR, m_bWindInterpolationVector)))	return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_TEMP_RH, m_bTempRHInterpolation)))			return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_CALCFWI, m_bCalcSpatialFWI)))				return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_WEATHER_INTERPOLATE_HISTORY, m_bHistoryInterpolation)))			return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_SPATIAL_THRESHOLD_DYNAMIC, m_bDynamicSpatialThreshold)))		return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_PURGE_NONDISPLAYABLE, m_bPurgeNonDisplayable)))					return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_SUPPRESS_TIGHT_CONCAVE_ADDPOINT, m_bSuppressConcaveTightFill)))	return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_CARDINAL_ROS, m_bCardinalDirection)))							return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_INDEPENDENT_TIMESTEPS, m_bIndependentFires)))					return false;
	return true;
}


auto Project::BasicScenarioOptions::operator=(const BasicScenarioOptions &toCopy) -> const BasicScenarioOptions& {
	if (this == &toCopy)
		return *this;
	m_perimeterResolution = toCopy.m_perimeterResolution;
	m_perimeterSpacing = toCopy.m_perimeterSpacing;
	m_spatialThreshold = toCopy.m_spatialThreshold;
	m_temporalThreshold_Accel = toCopy.m_temporalThreshold_Accel;
	m_displayInterval = toCopy.m_displayInterval;
	m_bBoundaryStop = toCopy.m_bBoundaryStop;
	m_bBreaching = toCopy.m_bBreaching;
	m_bSpotting = toCopy.m_bSpotting;
	m_bWeatherTemporalInterpolate = toCopy.m_bWeatherTemporalInterpolate;
	m_bWeatherSpatialInterpolate = toCopy.m_bWeatherSpatialInterpolate;
	m_bPrecipitationInterpolation = toCopy.m_bPrecipitationInterpolation;
	m_bWindInterpolation = toCopy.m_bWindInterpolation;
	m_bWindInterpolationVector = toCopy.m_bWindInterpolationVector;
	m_bTempRHInterpolation = toCopy.m_bTempRHInterpolation;
	m_bCalcSpatialFWI = toCopy.m_bCalcSpatialFWI;
	m_bHistoryInterpolation = toCopy.m_bHistoryInterpolation;
	m_bDynamicSpatialThreshold = toCopy.m_bDynamicSpatialThreshold;
	m_bPurgeNonDisplayable = toCopy.m_bPurgeNonDisplayable;
	m_bSuppressConcaveTightFill = toCopy.m_bSuppressConcaveTightFill;
	m_bCardinalDirection = toCopy.m_bCardinalDirection;
	m_bIndependentFires = toCopy.m_bIndependentFires;
	return *this;
}


void Project::CuringOptions::initEmpty() { }


Project::CuringOptions::CuringOptions()
{
	initEmpty();
}


Project::CuringOptions::CuringOptions(const TCHAR *szGroup)
{
#if _DLL
	CWinAppExt *app = (CWinAppExt *)AfxGetApp();
#else
    IniSettings *app = AfxGetApp();
#endif
	ULONG numEntries = app->GetProfileLong(szGroup, _T("Curing Options 2"), 0);
	if (!numEntries)
	{
		initEmpty();
	}
	else
	{
		for (ULONG i = 0; i < numEntries; i++)
		{
			CuringNode *cn = new CuringNode();
#if _DLL
			CString str;
			str.Format("Curing Option #%d Date", (int)i);
#else
            std::string str = (boost::format("Curing Option #%d Date") % i).str();
#endif
#if _DLL
			cn->m_date = WTimeSpan((LPCSTR)app->GetProfileString(szGroup, str, "January 1"));
#else
#undef GetProfileString
			cn->m_date = WTimeSpan(app->GetProfileString(szGroup, str, "January 1"));
#endif
#if _DLL
			str.Format("Curing Option #%d Percentage", (int)i);
#else
            str = (boost::format("Curing Option #%d Percentage") % i).str();
#endif
			cn->m_curing = app->GetProfileDouble(szGroup, str, 60.0);
			curing.AddTail(cn);
		}
	}
}


Project::CuringOptions::CuringOptions(const CuringOptions &toCopy)
{
	*this = toCopy;
}


auto Project::CuringOptions::operator=(const CuringOptions &toCopy) -> const CuringOptions& {
	if (this == &toCopy)
		return *this;

	CuringNode *cn;
	while (cn = curing.RemHead())
		delete cn;
	cn = toCopy.curing.LH_Head();
	while (cn->LN_Succ())
	{
		CuringNode *ncn = new CuringNode(*cn);
		curing.AddTail(ncn);
		cn = cn->LN_Succ();
	}
	return *this;
}


Project::CuringOptions::CuringOptions(CCWFGM_TemporalAttributeFilter *filter)
{
	USHORT i, count;
	WTimeVariant time;
	PolymorphicAttribute val;
	bool val_valid;
	filter->Count(FUELCOM_ATTRIBUTE_CURINGDEGREE, &count);
	for (i = 0; i < count; i++)
	{
		if (FAILED(filter->TimeAtIndex(FUELCOM_ATTRIBUTE_CURINGDEGREE, i, &time)))
			return;
		if (FAILED(filter->GetOptionKey(FUELCOM_ATTRIBUTE_CURINGDEGREE, time, &val, &val_valid)))
			return;
		if (val_valid)
		{
			CuringNode *cn = new CuringNode();
			try
			{
				cn->m_date = std::get<WTimeSpan>(time);
			    VariantToDouble_(val, &cn->m_curing);
				curing.AddTail(cn);
			}
			catch (std::bad_variant_access &)
			{
				weak_assert(0);
				delete cn;
			}
		}
	}
}


Project::CuringOptions::~CuringOptions()
{
	CuringNode *cn;
	while (cn = curing.RemHead())
		delete cn;
}


bool Project::CuringOptions::SaveToScenario(CCWFGM_TemporalAttributeFilter *filter) const
{
	CuringOptions copy(*this);

	USHORT i, count;
	WTimeVariant time;
	PolymorphicAttribute val;
	bool val_valid;
	CuringNode *cn;
	filter->Count(FUELCOM_ATTRIBUTE_CURINGDEGREE, &count);
	for (i = count - 1; i < count; i--)
	{
		if (FAILED(filter->TimeAtIndex(FUELCOM_ATTRIBUTE_CURINGDEGREE, i, &time)))
			return FALSE;
		if (FAILED(filter->ClearOptionKey(FUELCOM_ATTRIBUTE_CURINGDEGREE, time)))
			return FALSE;
	}
	while (cn = copy.curing.RemHead())
	{
		val = cn->m_curing;
		if (FAILED(filter->SetOptionKey(FUELCOM_ATTRIBUTE_CURINGDEGREE, cn->m_date, val, true)))
			return FALSE;
		delete cn;
	}

	return TRUE;
}


void Project::CuringOptions::SaveToIniFile(const TCHAR *group_name) const
{
#if _DLL
	CWinAppExt *app = (CWinAppExt *)AfxGetApp();
#else
    IniSettings *app = AfxGetApp();
#endif
	app->WriteProfileLong(group_name, _T("Curing Options 2"), curing.GetCount());
	for (ULONG i = 0; i < curing.GetCount(); i++)
	{
		CuringNode *cn = curing.IndexNode(i);
#if _DLL
		CString str;
		str.Format("Curing Option #%d Date", (int)i);
#else
        std::string str = (boost::format("Curing Option #%d Date") % i).str();
#endif
#if _DLL
#else
#undef WriteProfileString
#endif
		app->WriteProfileString(group_name, str, cn->m_date.ToString(WTIME_FORMAT_DAY | WTIME_FORMAT_CONDITIONAL_TIME).c_str());
#if _DLL
		str.Format("Curing Option #%d Percentage", (int)i);
#else
        str = (boost::format("Curing Option #%d Percentage") % i).str();
#endif
		app->WriteProfileDouble(group_name, str, cn->m_curing, "%.1lf");
	}
}
