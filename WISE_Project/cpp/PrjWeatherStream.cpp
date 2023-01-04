/**
 * WISE_Project: PrjWeatherStream.cpp
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

#include "ICWFGM_Fuel.h"
#include "BurnPeriodOption.h"
#include "WeatherOptions.h"
#include "angles.h"
#include "CWFGMProject.h"
#include "WeatherCollection.h"
#include "macros.h"
#include "CWFGM_WeatherStream.h"
#include "str_printf.h"

#include <gsl/gsl>


Project::WeatherStream::WeatherStream()
{
	m_stream = boost::make_intrusive<CCWFGM_WeatherStream>();
	PolymorphicUserData v((void*)this);
	m_stream->put_UserData(v);
	m_station = nullptr;
}


Project::WeatherStream::WeatherStream(CCWFGM_WeatherStream *stream)
{
	m_stream = stream;
	PolymorphicUserData v((void*)this);
	m_stream->put_UserData(v);
	m_station = nullptr;
}


Project::WeatherStream::WeatherStream(const WeatherStream &stream)
{
	boost::intrusive_ptr<ICWFGM_CommonBase> ws;
	HRESULT hr = stream.m_stream->Clone(&ws);
	if (FAILED(hr))
		throw std::logic_error("Failed");
	m_stream = dynamic_cast<CCWFGM_WeatherStream *>(ws.get());
	PolymorphicUserData v((void*)this);
	m_stream->put_UserData(v);
	m_name = stream.m_name;
	m_comments = stream.m_comments;
	m_station = nullptr;
	SetWeatherOptions(stream.GetWeatherOptions());
}


HRESULT Project::WeatherStream::Import(const std::string &file_name, USHORT options)
{
	return m_stream->Import(file_name, options);
}


void Project::WeatherStream::ClearWeatherData()
{
	m_stream->ClearWeatherData(); 
}


HRESULT Project::WeatherStream::GetValidTimeRange(WTime &start, WTimeSpan &duration)
{
	const WTimeManager *tm = start.GetTimeManager();
	HRESULT hr = m_stream->GetValidTimeRange(&start, &duration);
	if (SUCCEEDED(hr))
	{
		start.SetTimeManager(tm);
	}
	return hr;
}


HRESULT Project::WeatherStream::Export(ICWFGM_FWI *FWI, const TCHAR *szPath, const ExportOptions *exportOptions, SHORT export_hourly, bool include_fwi, bool need_head)
{
	FILE *fp = nullptr;
	const int err = _tfopen_s(&fp, szPath, _T("w"));
	if (fp == nullptr)
		return E_FAIL;

	WTime nextTime((ULONGLONG)0, m_station->m_collection->m_project->m_timeManager);
	WTimeSpan duration;
	const WTimeSpan oneDay(1, 0, 0, 0);

	GetValidTimeRange(nextTime, duration);
	const std::int64_t totalHours = duration.GetTotalHours();

	std::string str;
	int i{ 0 };

	if (need_head)
	{
		switch (export_hourly)
		{
		case 0:
			if (exportOptions->ExportColumnAligned())
			{
				if (include_fwi)
					str = strprintf(_T("Hourly     Hour    Temp      RH       WS     WD  Precip     FFMC     DMC       DC     ISI     BUI      FWI     HFFMC    HISI     HFWI\n"));
				else
					str = strprintf(_T("Hourly     Hour    Temp      RH       WS     WD  Precip\n"));
			}
			else
			{
				const TCHAR *delimiter = exportOptions->GetExportDelimiter();
				if (include_fwi)
					str = strprintf(_T("Hourly%sHour%sTemp%sRH%sWS%sWD%sPrecip%sFFMC%sDMC%sDC%sISI%sBUI%sFWI%sHFFMC%sHISI%sHFWI\n"),delimiter,delimiter,delimiter,delimiter,delimiter,delimiter,delimiter,delimiter,delimiter,delimiter,delimiter,delimiter, delimiter, delimiter, delimiter);
				else
					str = strprintf(_T("Hourly%sHour%sTemp%sRH%sWS%sWD%sPrecip\n"),delimiter,delimiter,delimiter,delimiter,delimiter,delimiter);
			}
			break;
		case 1:
			if (exportOptions->ExportColumnAligned())
			{
				if (include_fwi)
					str = strprintf(_T("Daily       Min_Temp  Max_Temp  Min_RH   Min_WS   Max_WS    WD   Precip    FFMC     DMC      DC    ISI     BUI    FWI\n"));
				else
					str = strprintf(_T("Daily       Min_Temp  Max_Temp  Min_RH   Min_WS   Max_WS    WD   Precip\n"));
			}
			else
			{
				const TCHAR *delimiter = exportOptions->GetExportDelimiter();
				if (include_fwi)
					str = strprintf(_T("Daily%sMin_Temp%sMax_Temp%sMin_RH%sMin_WS%sMax_WS%sWD%sPrecip%sFFMC%sDMC%sDC%sISI%sBUI%sFWI\n"), delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
				else
					str = strprintf(_T("Daily%sMin_Temp%sMax_Temp%sMin_RH%sMin_WS%sMax_WS%sWD%sPrecip\n"), delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
			}
			break;
		case 2:
			if (exportOptions->ExportColumnAligned())
			{
				if (include_fwi)
					str = strprintf(_T("Date         Temp     RH     WS      WD  Precip    FFMC     DMC       DC    ISI     BUI      FWI\n"));
				else
					str = strprintf(_T("Date         Temp     RH     WS      WD  Precip\n"));
			}
			else
			{
				const TCHAR *delimiter = exportOptions->GetExportDelimiter();
				if (include_fwi)
					str = strprintf(_T("Date%sTemp%sRH%sWS%sWD%sPrecip%sFFMC%sDMC%sDC%sISI%sBUI%sFWI\n"), delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter, delimiter);
				else
					str = strprintf(_T("Date%sTemp%sRH%sWS%sWD%sPrecip\n"), delimiter, delimiter, delimiter, delimiter, delimiter);
			}
			break;
		}
		_fputts(str.c_str(),fp);
	}
	for(i = 0; i < totalHours; i+=24)
	{
		FillOneDayStream(FWI, nextTime, fp,exportOptions, export_hourly, include_fwi);
		nextTime+=oneDay;
	}

	fclose(fp);
	return S_OK;
}

std::string Project::WeatherStream::FormStringLine(const WTime &lt, double temp, double ws, double rh, double precip, double wd, double ffmc, double hffmc, double dmc, double dc, double bui,
    double isi, double hisi, double fwi, double hfwi, const ExportOptions *exportOptions, SHORT include_fwi)
{
	std::string returnStr;
	if (exportOptions->ExportColumnAligned())
	{
		if (include_fwi)
			returnStr = strprintf(_T("%02d/%02d/%04d  %02d    %5.2lf    %5.2lf    %5.2lf   %3d   %5.2lf    %5.2lf   %5.2lf   %5.2lf   %5.2lf   %5.2lf    %5.2lf     %5.2lf   %5.2lf    %5.2lf"),
			    lt.GetDay(0), lt.GetMonth(0), lt.GetYear(0), lt.GetHour(0), temp, rh, ws, gsl::narrow_cast<int>(ROUND(wd)), precip,
			    ffmc, dmc, dc, isi, bui, fwi, hffmc, hisi, hfwi);
		else
			returnStr = strprintf(_T("%02d/%02d/%04d  %02d    %5.2lf    %5.2lf    %5.2lf   %3d   %5.2lf"),
			    lt.GetDay(0), lt.GetMonth(0), lt.GetYear(0), lt.GetHour(0), temp, rh, ws, gsl::narrow_cast<int>(ROUND(wd)), precip);
	}
	else
	{
		std::string tempStr;
		const TCHAR *delimiter = exportOptions->GetExportDelimiter();
		tempStr = strprintf(_T("%d/%d/%d%s%d%s"),lt.GetDay(0), lt.GetMonth(0), lt.GetYear(0), delimiter, lt.GetHour(0), delimiter);
		returnStr+=tempStr;
		tempStr = strprintf(_T("%.2lf%s%.2lf%s"),temp,delimiter,rh,delimiter);
		returnStr+=tempStr;

		tempStr = strprintf(_T("%.2lf%s%d%s%.2lf"), ws, delimiter, gsl::narrow_cast<int>(ROUND(wd)), delimiter, precip);
		returnStr+=tempStr;
		if (include_fwi)
		{
			tempStr = strprintf(_T("%s%.2f%s%.2lf%s%.2lf%s%.2lf%s%.2lf%s%.2lf%s%.2lf%s%.2lf%s%.2lf"),
				delimiter, ffmc, delimiter, dmc, delimiter, dc, delimiter, isi, delimiter,
				bui, delimiter, fwi, delimiter, hffmc, delimiter, hisi, delimiter, hfwi);
			returnStr+=tempStr;
		}
	}
	return returnStr;
}

void Project::WeatherStream::FillOneDayStream(ICWFGM_FWI *FWI, const WTime &newTime, FILE *fp,const ExportOptions *exportOptions,
	SHORT export_hourly, SHORT include_fwi)
{
	switch (export_hourly)
	{
		case 0:
		{
			WTime nextTime(newTime);
			const WTimeSpan oneHour(0, 1, 0, 0);
			double hourly_temp[24],hourly_rh[24],
				hourly_precip[24],hourly_ws[24],
				hourly_wd[24],hourly_ffmc[24],
				hourly_dc[24],hourly_dmc[24],
				hourly_isi[24],hourly_bui[24],
				hourly_fwi[24], hourly_daily_ffmc[24],
				hourly_daily_fwi[24], hourly_daily_isi[24];

			double min_temp,rh,_rh,precip,min_ws,wd,hffmc,dffmc,dmc,dc,dew;
			double bui,d_isi,d_fwi,hisi,hfwi;
			std::string str;
			USHORT i;
			for(i=0;i<24;i++)
			{
				IWXData wx;
				IFWIData ifwi;
				DFWIData dfwi;
				GetInstantaneousValues(nextTime, 0, &wx, &ifwi, &dfwi);
				min_temp = wx.Temperature;
				rh = wx.RH;
				precip = wx.Precipitation;
				min_ws = std::max(0.0, min(200.0, wx.WindSpeed));
				wd = wx.WindDirection;
				dew = wx.DewPointTemperature;

				rh = rh*100;
				hourly_temp[i] = min_temp;
				hourly_rh[i] = rh;
				hourly_precip[i] = precip;
				hffmc = ifwi.FFMC;
				dffmc = dfwi.dFFMC;
				d_isi = dfwi.dISI;
				d_fwi = dfwi.dFWI;
				dc = dfwi.dDC;
				dmc = dfwi.dDMC;
				bui = dfwi.dBUI;
				hisi = ifwi.ISI;
				hfwi = ifwi.FWI;
				min_ws = std::max(0.0, min(200.0, wx.WindSpeed));
				wd = wx.WindDirection;
				_rh = wx.RH;

				if(wd==-1.0)
				{
					wd = 0.0; 
				}
				else
				{
					wd = CARTESIAN_TO_COMPASS_DEGREE( RADIAN_TO_DEGREE( wd ) );
					if ((wd <= 0.0000001) && (min_ws != 0.0))
						wd = 360.0;
				}
				hourly_ffmc[i] = hffmc;
				hourly_dc[i] = dc;
				hourly_dmc[i] = dmc;
				hourly_wd[i] = wd;
				hourly_ws[i] = min_ws;
				hourly_bui[i] = bui;
				hourly_isi[i] = hisi;
				hourly_fwi[i] = hfwi;
				hourly_daily_ffmc[i] = dffmc;
				hourly_daily_fwi[i] = d_fwi;
				hourly_daily_isi[i] = d_isi;

				nextTime = nextTime + oneHour;
			}
			nextTime=newTime;
			for (i = 0; i < 24; i++)
			{
				min_temp = hourly_temp[i];
				min_ws = hourly_ws[i];
				rh = hourly_rh[i];
				precip = hourly_precip[i];
				wd = hourly_wd[i];
				hffmc = hourly_ffmc[i];
				dmc = hourly_dmc[i];
				dc = hourly_dc[i];
				bui = hourly_bui[i];
				hisi = hourly_isi[i];
				hfwi = hourly_fwi[i];
				dffmc = hourly_daily_ffmc[i];
				d_fwi = hourly_daily_fwi[i];
				d_isi = hourly_daily_isi[i];
				WTime lt(nextTime, WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST, 1);
				str=FormStringLine(lt,min_temp,min_ws,rh,precip,wd,dffmc,hffmc,dmc,dc,bui,d_isi,hisi,d_fwi,hfwi,exportOptions,include_fwi);

				str+="\n";
				_fputts(str.c_str(),fp);
				nextTime = nextTime + oneHour;
			}
			break;
		}
		case 1:
		{
			WTime ldt(newTime, WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST, -1), 
				lst(newTime, WTIME_FORMAT_AS_LOCAL, -1),
				noonStdTime(newTime);

			if (lst != ldt)
				noonStdTime += newTime.GetTimeManager()->m_worldLocation.m_amtDST();
			noonStdTime += WTimeSpan(0, 12, 0, 0);

			double min_temp, max_temp, min_ws, max_ws, min_gust, max_gust, min_rh, precip, wd;
			GetDailyValues(newTime, &min_temp, &max_temp, &min_ws, &max_ws, &min_gust, &max_gust, &min_rh, &precip, &wd);

			IWXData wx;
			IFWIData ifwi;
			DFWIData dfwi;
			if (include_fwi)
				GetInstantaneousValues(noonStdTime, 0, &wx, &ifwi, &dfwi);

			if(wd==-1.0) {
				wd = 0.0;
			}
			else
			{
				wd = CARTESIAN_TO_COMPASS_DEGREE(RADIAN_TO_DEGREE(wd));
				if ((wd <= 0.0000001) && (min_ws != 0.0))
					wd = 360.0;
			}
			min_rh *= 100.0;
			
			double isi, fwi;
			if (include_fwi)
			{
				FWI->ISI_FBP(dfwi.dFFMC, wx.WindSpeed, 60 * 60, &isi);
				FWI->FWI(isi, dfwi.dBUI, &fwi);
			}

			std::string str;
			if (exportOptions->ExportColumnAligned())
			{
				if (include_fwi)
					str = strprintf(_T("%02d/%02d/%04d  %6.2lf   %8.2lf   %6.2lf  %7.2lf  %7.2lf  %4d   %6.2lf   %5.2lf   %5.2lf  %5.2lf  %5.2lf   %5.2lf  %5.2lf\n"),
					    newTime.GetDay(0), newTime.GetMonth(0), newTime.GetYear(0),
					    min_temp, max_temp, min_rh, min_ws, max_ws, (int)ROUND(wd), precip,
					    dfwi.dFFMC,dfwi.dDMC,dfwi.dDC, isi, dfwi.dBUI, fwi);
				else
					str = strprintf(_T("%02d/%02d/%04d  %6.2lf   %8.2lf   %6.2lf  %7.2lf  %7.2lf  %4d   %6.2lf\n"),
					    newTime.GetDay(0), newTime.GetMonth(0), newTime.GetYear(0),
					    min_temp, max_temp, min_rh, min_ws, max_ws, (int)ROUND(wd), precip);
			}
			else
			{
				const TCHAR *delimiter = exportOptions->GetExportDelimiter();
				if (include_fwi)
					str = strprintf(_T("%02d/%02d/%04d%s"
						"%.2lf%s%.2lf%s%.2lf%s%.2lf%s%.2lf%s%d%s%.2lf%s%.2lf%s%.2lf%s%.2lf%s%.2lf%s%.2lf%s%.2lf\n"),
					    newTime.GetDay(0), newTime.GetMonth(0), newTime.GetYear(0), delimiter,
					    min_temp, delimiter, max_temp, delimiter, min_rh, delimiter, min_ws, delimiter, max_ws, delimiter, (int)ROUND(wd), delimiter, precip, delimiter,
					    dfwi.dFFMC,delimiter,dfwi.dDMC,delimiter,dfwi.dDC,delimiter,isi,delimiter,dfwi.dBUI, delimiter, fwi);
				else
					str = strprintf(_T("%02d/%02d/%04d%s"
						"%.2lf%s%.2lf%s%.2lf%s%.2lf%s%.2lf%s%d%s%.2lf\n"),
					    newTime.GetDay(0), newTime.GetMonth(0), newTime.GetYear(0), delimiter,
					    min_temp, delimiter, max_temp, delimiter, min_rh, delimiter, min_ws, delimiter, max_ws, delimiter, (int)ROUND(wd), delimiter, precip);
			}
			_fputts(str.c_str(), fp);
			break;
		}
		case 2:
		{
			WTime ldt(newTime, WTIME_FORMAT_AS_LOCAL | WTIME_FORMAT_WITHDST, -1), 
				lst(newTime, WTIME_FORMAT_AS_LOCAL, -1),
				noonStdTime(newTime);

			if (lst != ldt)
				noonStdTime += newTime.GetTimeManager()->m_worldLocation.m_amtDST();
			noonStdTime += WTimeSpan(0, 12, 0, 0);

			IWXData wx;
			IFWIData ifwi;
			DFWIData dfwi;
			GetInstantaneousValues(noonStdTime, 0, &wx, &ifwi, &dfwi);

			WTime start(newTime);
			GetStartTime(start);
			for (LONG i = 1; i < 24; i++)
			{
				IWXData wx2;
				WTime time(noonStdTime);
				time -= WTimeSpan(0, i, 0, 0);
				if (time >= start)
				{
					if (GetInstantaneousValues(time, 0, &wx2, nullptr, nullptr))
						wx.Precipitation += wx2.Precipitation;
					else
						break;
				}
				else
				{
					wx.Precipitation += Initial_Rain();
					break;
				}
			}

			if(wx.WindDirection==-1.0)
			{
				wx.WindDirection = 0.0; 
			}
			else
			{
				wx.WindDirection = CARTESIAN_TO_COMPASS_DEGREE(RADIAN_TO_DEGREE(wx.WindDirection));
				if ((wx.WindDirection <= 0.0000001) && (wx.WindSpeed != 0.0))
					wx.WindDirection = 360.0;
			}
			wx.RH *= 100.0;

			std::string str;
			if (exportOptions->ExportColumnAligned())
			{
				if (include_fwi)
					str = strprintf(_T("%02d/%02d/%04d  %5.2lf   %5.2lf  %5.2lf  %5.0lf  %5.2lf    %5.2lf   %5.2lf   %5.2lf  %5.2lf   %5.2lf    %5.2lf"),
					    newTime.GetDay(0), newTime.GetMonth(0), newTime.GetYear(0), wx.Temperature, wx.RH, wx.WindSpeed, wx.WindDirection, wx.Precipitation,
					    dfwi.dFFMC, dfwi.dDMC, dfwi.dDC, dfwi.dISI, dfwi.dBUI, dfwi.dFWI);
				else
					str = strprintf(_T("%02d/%02d/%04d  %5.2lf   %5.2lf  %5.2lf  %5.0lf  %5.2lf"),
					    newTime.GetDay(0), newTime.GetMonth(0), newTime.GetYear(0), wx.Temperature, wx.RH, wx.WindSpeed, wx.WindDirection, wx.Precipitation);
			}
			else
			{
				std::string tempStr;
				const TCHAR *delimiter = exportOptions->GetExportDelimiter();
				tempStr = strprintf(_T("%d/%d/%d%s"),newTime.GetDay(0), newTime.GetMonth(0), newTime.GetYear(0), delimiter);
				str+=tempStr;
				tempStr = strprintf(_T("%.2lf%s%.2lf%s"),wx.Temperature,delimiter,wx.RH,delimiter);
				str+=tempStr;
				tempStr = strprintf(_T("%.2lf%s%.0lf%s%.2lf"),wx.WindSpeed,delimiter,wx.WindDirection,delimiter, wx.Precipitation);
				str+=tempStr;
				if (include_fwi)
				{
					tempStr = strprintf(_T("%s%.2lf%s%.2lf%s%.2lf%s%.2lf%s%.2f%s%.2lf"),delimiter,dfwi.dFFMC,delimiter,dfwi.dDMC,delimiter,dfwi.dDC,delimiter,dfwi.dISI,delimiter,dfwi.dBUI,delimiter,dfwi.dFWI);
					str+=tempStr;
				}
			}
			_fputts(str.c_str(), fp);
			_fputts(_T("\n"), fp);
			break;
		}
	}
}


bool Project::WeatherStream::IsImportedFromFile()
{
	return SUCCEEDED(m_stream->IsImportedFromFile(WTime(0, nullptr)));
}
