/**
 * WISE_Project: FBPOptions.cpp
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
#include <afxdisp.h>
#define BOOL_CHECK ? true : false
#else
#include "AfxIniSettings.h"
#define BOOL_CHECK
#endif

#include "FBPOptions.h"
#include "FireEngine_ext.h"
#include "propsysreplacement.h"

#include <gsl/gsl>


Project::FBPOptions::FBPOptions()
{
	m_topography = true;
	m_extinguishment = false;
	m_wind = true;
	m_specifiedFMC = 120.0;
	m_specifiedFMCActive = false;
	m_defaultElevation = -1.0;
	m_growthPercentileEnabled = false;
	m_growthPercentile = 50.0;
	m_greenupStart = HSS_Time::WTimeSpan(0LL);
	m_greenupEnd = HSS_Time::WTimeSpan(365, 23, 59, 59);
	m_standingStart = HSS_Time::WTimeSpan(0LL);
	m_standingEnd = HSS_Time::WTimeSpan(365, 23, 59, 59);
}


Project::FBPOptions::FBPOptions(const TCHAR *group_name)
{
#if _DLL
	CWinAppExt *app = (CWinAppExt *)AfxGetApp();
#else
	const gsl::not_null<IniSettings*> app = AfxGetApp();
#endif
	m_topography = app->GetProfileBOOL(group_name, _T("Topography"), TRUE) BOOL_CHECK;
	m_extinguishment = app->GetProfileBOOL(group_name, _T("Extinguishment"), FALSE) BOOL_CHECK;
	m_wind = app->GetProfileBOOL(group_name, _T("Wind"), TRUE) BOOL_CHECK;
	m_growthPercentileEnabled = app->GetProfileBOOL(group_name, _T("Growth Percentile Enabled"), FALSE) BOOL_CHECK;
	m_specifiedFMC = 120.0;
	m_specifiedFMCActive = false;
	m_defaultElevation = -1.0;
	m_growthPercentile = app->GetProfileDouble(group_name, _T("Growth Percentile 2"), 50.0);
	m_greenupStart = HSS_Time::WTimeSpan(app->GetProfileLongLong(group_name, _T("Green-up Start"), 0));
	m_greenupEnd = HSS_Time::WTimeSpan(app->GetProfileLongLong(group_name, _T("Green-up End"), 366 * 24 * 60 * 60 - 1));
	m_standingStart = HSS_Time::WTimeSpan(app->GetProfileLongLong(group_name, _T("Standing Start"), 0));
	m_standingEnd = HSS_Time::WTimeSpan(app->GetProfileLongLong(group_name, _T("Standing End"), 366 * 24 * 60 * 60 - 1));
}


void Project::FBPOptions::SaveToIniFile(const TCHAR *group_name) const
{
#if _DLL
	CWinAppExt *app = (CWinAppExt *)AfxGetApp();
#else
	const gsl::not_null<IniSettings*> app = AfxGetApp();
#endif
	app->WriteProfileBOOL(group_name, _T("Topography"), m_topography);
	app->WriteProfileBOOL(group_name, _T("Extinguishment"), m_extinguishment);
	app->WriteProfileBOOL(group_name, _T("Wind"), m_wind);
	app->WriteProfileBOOL(group_name, _T("Growth Percentile Enabled"), m_growthPercentileEnabled);
	app->WriteProfileDouble(group_name, _T("Growth Percentile"), m_growthPercentile, "%.2lf");
	app->WriteProfileLongLong(group_name, _T("Green-up Start"), m_greenupStart.GetTotalSeconds());
	app->WriteProfileLongLong(group_name, _T("Green-up End"), m_greenupEnd.GetTotalSeconds());
	app->WriteProfileLongLong(group_name, _T("Standing Start"), m_standingStart.GetTotalSeconds());
	app->WriteProfileLongLong(group_name, _T("Standing End"), m_standingEnd.GetTotalSeconds());
}


Project::FBPOptions::FBPOptions(const FBPOptions &toCopy)
{
	*this = toCopy;
}


auto Project::FBPOptions::operator=(const FBPOptions &toCopy) -> const FBPOptions&
{
	if (&toCopy != this)
	{
		m_topography = toCopy.m_topography;
		m_extinguishment = toCopy.m_extinguishment;
		m_wind = toCopy.m_wind;
		m_growthPercentileEnabled = toCopy.m_growthPercentileEnabled;
		m_specifiedFMC = toCopy.m_specifiedFMC;
		m_specifiedFMCActive = toCopy.m_specifiedFMCActive;
		m_defaultElevation = toCopy.m_defaultElevation;
		m_growthPercentile = toCopy.m_growthPercentile;
		m_greenupStart = toCopy.m_greenupStart;
		m_greenupEnd = toCopy.m_greenupEnd;
		m_standingStart = toCopy.m_standingStart;
		m_standingEnd = toCopy.m_standingEnd;
	}
	return *this;
}


Project::FBPOptions::FBPOptions(CCWFGM_Scenario *scenario)
{
	PolymorphicAttribute v;

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_TOPOGRAPHY, &v)))		{ VariantToBoolean_(v, &m_topography); }
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_EXTINGUISHMENT, &v)))	{ VariantToBoolean_(v, &m_extinguishment); }
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_WIND, &v)))			    { VariantToBoolean_(v, &m_wind); }
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_PERCENTILE_ENABLE, &v))) { VariantToBoolean_(v, &m_growthPercentileEnabled); }	

	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_SPECIFIED_FMC, &v)))		{ VariantToDouble_(v, &m_specifiedFMC); }
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_SPECIFIED_FMC_ACTIVE, &v))) { VariantToBoolean_(v, &m_specifiedFMCActive); }
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_DEFAULT_ELEVATION, &v)))	{ VariantToDouble_(v, &m_defaultElevation); }
	if (SUCCEEDED(scenario->GetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_PERCENTILE, &v))) { VariantToDouble_(v, &m_growthPercentile); }
}


bool Project::FBPOptions::SaveToScenario(CCWFGM_Scenario *scenario) const
{
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_TOPOGRAPHY, m_topography)))									return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_EXTINGUISHMENT, m_extinguishment)))							return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_WIND, m_wind)))												return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_PERCENTILE_ENABLE, m_growthPercentileEnabled)))	return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_SPECIFIED_FMC, m_specifiedFMC)))							return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_SPECIFIED_FMC_ACTIVE, m_specifiedFMCActive)))				return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_DEFAULT_ELEVATION, m_defaultElevation)))					return false;
	if (FAILED(scenario->SetAttribute(CWFGM_SCENARIO_OPTION_IGNITIONS_PERCENTILE, m_growthPercentile)))					return false;
    return true;
}
