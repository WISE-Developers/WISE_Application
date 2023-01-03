/**
 * WISE_Project: WeatherOptions.cpp
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
#include "StdAfx.h"
#endif

#include "WeatherOptions.h"
#include "WeatherCom_ext.h"
#include "GridCom_ext.h"

#if _DLL
#include "cwinapp.h"
#define BOOL_CHECK ? true : false
#else
#include "AfxIniSettings.h"
#define BOOL_CHECK
#endif
#include "WTime.h"

#include <gsl/util>


Project::WeatherOptions::WeatherOptions()
{
	m_temp_alpha = -0.77;
	m_temp_beta = 2.80;	// temp alpha, beta defaults changed to Cdn averages 060619
	m_temp_gamma = -2.20;
	m_wind_alpha = 1.00;
	m_wind_beta = 1.24;
	m_wind_gamma = -3.59;	// Denver defaults, "Beck & Trevitt, Forecasting diurnal variations in meteorological parameters for
				// predicting fire behavior", 1989
	m_FFMC = VAN_WAGNER;		// default Van Wagner
}


Project::WeatherOptions::WeatherOptions(const TCHAR *group_name)
{
#if _DLL
	CWinAppExt* app = (CWinAppExt*)AfxGetApp();
#else
	auto app = AfxGetApp();
#endif
	m_temp_alpha = app->GetProfileDouble(group_name, _T("Temperature Alpha Value"), -0.77);
	m_temp_beta = app->GetProfileDouble(group_name, _T("Temperature Beta Value"), 2.80);
	m_temp_gamma = app->GetProfileDouble(group_name, _T("Temperature Gamma Value"), -2.20);
	m_wind_alpha = app->GetProfileDouble(group_name, _T("Wind Alpha Value"), 1.00);
	m_wind_beta = app->GetProfileDouble(group_name, _T("Wind Beta Value"), 1.24);
	m_wind_gamma = app->GetProfileDouble(group_name, _T("Wind Gamma Value"), -3.59);
	m_FFMC = (USHORT)app->GetProfileInt(group_name, _T("FFMC Calculation"), 0);

	int fixed = app->GetProfileInt(group_name, _T("Canadian Default Values"), 0);
	if (!fixed)
	{
		m_temp_alpha = -0.77;
		m_temp_beta = 2.80;
	}

	if (m_FFMC >= 3)
		m_FFMC = 0;				// reset the default
}


void Project::WeatherOptions::SaveToIniFile(const TCHAR *group_name) const
{
#if _DLL
	CWinAppExt* app = (CWinAppExt*)AfxGetApp();
#else
	auto app = AfxGetApp();
#endif
	app->WriteProfileDouble(group_name, _T("Temperature Alpha Value"), m_temp_alpha, _T("%.2lf"));
	app->WriteProfileDouble(group_name, _T("Temperature Beta Value"), m_temp_beta, _T("%.2lf"));
	app->WriteProfileDouble(group_name, _T("Temperature Gamma Value"), m_temp_gamma, _T("%.2lf"));
	app->WriteProfileDouble(group_name, _T("Wind Alpha Value"), m_wind_alpha, _T("%.2lf"));
	app->WriteProfileDouble(group_name, _T("Wind Beta Value"), m_wind_beta, _T("%.2lf"));
	app->WriteProfileDouble(group_name, _T("Wind Gamma Value"), m_wind_gamma, _T("%.2lf"));
	app->WriteProfileInt(group_name, _T("FFMC Calculation"), m_FFMC);
	app->WriteProfileInt(group_name, _T("Canadian Default Values"), 1);
}


Project::WeatherOptions::WeatherOptions(const WeatherOptions &toCopy)
{
	*this = toCopy;
}


auto Project::WeatherOptions::operator=(const WeatherOptions &toCopy) -> const WeatherOptions&
{
	if (&toCopy != this)
	{
		m_temp_alpha = toCopy.m_temp_alpha;
		m_temp_beta = toCopy.m_temp_beta;
		m_temp_gamma = toCopy.m_temp_gamma;
		m_wind_alpha = toCopy.m_wind_alpha;
		m_wind_beta = toCopy.m_wind_beta;
		m_wind_gamma = toCopy.m_wind_gamma;
		m_FFMC = toCopy.m_FFMC;
	}
	return *this;
}


Project::WeatherOptions::WeatherOptions(CCWFGM_WeatherStream *weather)
{
	PolymorphicAttribute dval;
	bool val{ false };

	/*POLYMORPHIC CHECK*/
	if (SUCCEEDED(weather->GetAttribute(CWFGM_WEATHER_OPTION_TEMP_ALPHA, &dval)))
		VariantToDouble_(dval, &m_temp_alpha);

	if (SUCCEEDED(weather->GetAttribute(CWFGM_WEATHER_OPTION_TEMP_BETA, &dval)))
		VariantToDouble_(dval, &m_temp_beta);

	if (SUCCEEDED(weather->GetAttribute(CWFGM_WEATHER_OPTION_TEMP_GAMMA, &dval)))
		VariantToDouble_(dval, &m_temp_gamma);

	if (SUCCEEDED(weather->GetAttribute(CWFGM_WEATHER_OPTION_WIND_ALPHA, &dval)))
		VariantToDouble_(dval, &m_wind_alpha);

	if (SUCCEEDED(weather->GetAttribute(CWFGM_WEATHER_OPTION_WIND_BETA, &dval)))
		VariantToDouble_(dval, &m_wind_beta);

	if (SUCCEEDED(weather->GetAttribute(CWFGM_WEATHER_OPTION_WIND_GAMMA, &dval)))
		VariantToDouble_(dval, &m_wind_gamma);

	m_FFMC = gsl::narrow_cast<USHORT>(-1);
	if (SUCCEEDED(weather->GetAttribute(CWFGM_WEATHER_OPTION_FFMC_VANWAGNER, &dval)))
	{
		VariantToBoolean_(dval, &val);
		if (val)		m_FFMC = 0;
	}

	if (m_FFMC == gsl::narrow_cast<std::uint16_t>(-1))
		if (SUCCEEDED(weather->GetAttribute(CWFGM_WEATHER_OPTION_FFMC_LAWSON, &dval)))
		{
			VariantToBoolean_(dval, &val);
			if (val)	m_FFMC = 2;
		}

	if (m_FFMC == gsl::narrow_cast<std::uint16_t>(-1))
		m_FFMC = 0;			//No FFMC Option was set before, so set it as Vanwagner.
}


bool Project::WeatherOptions::SaveToWeatherStream(CCWFGM_WeatherStream *weather) const
{
	PolymorphicAttribute v;
	bool bv{ false };

	if (FAILED(weather->SetAttribute(CWFGM_WEATHER_OPTION_TEMP_ALPHA, m_temp_alpha)))			return false;
	if (FAILED(weather->SetAttribute(CWFGM_WEATHER_OPTION_TEMP_BETA, m_temp_beta)))				return false;
	if (FAILED(weather->SetAttribute(CWFGM_WEATHER_OPTION_TEMP_GAMMA, m_temp_gamma)))			return false;
	if (FAILED(weather->SetAttribute(CWFGM_WEATHER_OPTION_WIND_ALPHA, m_wind_alpha)))			return false;
	if (FAILED(weather->SetAttribute(CWFGM_WEATHER_OPTION_WIND_BETA, m_wind_beta)))				return false;
	if (FAILED(weather->SetAttribute(CWFGM_WEATHER_OPTION_WIND_GAMMA, m_wind_gamma)))			return false;

	switch (m_FFMC)
	{
	case VAN_WAGNER:
		if (SUCCEEDED(weather->GetAttribute(CWFGM_WEATHER_OPTION_FFMC_VANWAGNER, &v)))
		{
			VariantToBoolean_(v, &bv);
			if (!bv)
			{
				v = true;
				if (FAILED(weather->SetAttribute(CWFGM_WEATHER_OPTION_FFMC_VANWAGNER, v)))		return false;
			}
		}
		else
		{
			v = true;
			if (FAILED(weather->SetAttribute(CWFGM_WEATHER_OPTION_FFMC_VANWAGNER, v)))		return false;
		}
		break;
	case LAWSON:
		if (SUCCEEDED(weather->GetAttribute(CWFGM_WEATHER_OPTION_FFMC_LAWSON, &v)))
		{
			VariantToBoolean_(v, &bv);
			if (!bv)
			{
				v = true;
				if (FAILED(weather->SetAttribute(CWFGM_WEATHER_OPTION_FFMC_LAWSON, v)))			return false;
			}
		}
		else
		{
			v = true;
			if (FAILED(weather->SetAttribute(CWFGM_WEATHER_OPTION_FFMC_LAWSON, v)))				return false;
		}
		break;
	default:				
		weak_assert(0);
		return false;
	}
    return true;
}
