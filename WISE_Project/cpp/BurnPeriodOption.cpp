/**
 * WISE_Project: BurnPeriodOption.cpp
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
#include "CWinApp.h"
#define BOOL_CHECK ? true : false
#else
#include "AfxIniSettings.h"
#define BOOL_CHECK
#endif

#include "BurnPeriodOption.h"

#include <gsl/gsl>


Project::CBurnPeriodOption::CBurnPeriodOption(const TCHAR *szGroup)
	: m_time(0, nullptr)
{
#if _DLL
	CWinAppExt *app = (CWinAppExt *)AfxGetApp();
#else
    const gsl::not_null<IniSettings*> app = AfxGetApp();
#endif
	m_bEffective = true;
	m_Start = HSS_Time::WTimeSpan(app->GetProfileLong(szGroup, _T("Burn Start4"), 0));
	m_End = HSS_Time::WTimeSpan(app->GetProfileLong(szGroup, _T("Burn End4"), 24 * 60 * 60 - 1));
	m_MaxWS = app->GetProfileDouble(szGroup, _T("Extinguish Max WS"), 0.0);
	m_MinRH = app->GetProfileDouble(szGroup, _T("Extinguish Min RH"), 100.0);
	m_MinFWI = app->GetProfileDouble(szGroup, _T("Extinguish Min FWI"), 0.0);
	m_MinISI = app->GetProfileDouble(szGroup, _T("Extinguish Min ISI1"), 0.0);

	eff_Start = app->GetProfileBOOL(szGroup, _T("Burn Start3 Effective"), true) BOOL_CHECK;
	eff_End = app->GetProfileBOOL(szGroup, _T("Burn End3 Effective"), true) BOOL_CHECK;
	eff_MaxWS = app->GetProfileBOOL(szGroup, _T("Extinguish Max WS Effective"), true) BOOL_CHECK;
	eff_MinRH = app->GetProfileBOOL(szGroup, _T("Extinguish Min RH Effective"), true) BOOL_CHECK;
	eff_MinFWI = app->GetProfileBOOL(szGroup, _T("Extinguish Min FWI Effective"), true) BOOL_CHECK;
	eff_MinISI = app->GetProfileBOOL(szGroup, _T("Extinguish Min ISI1 Effective"), true) BOOL_CHECK;
}


Project::CBurnPeriodOption::CBurnPeriodOption(const CBurnPeriodOption &toCopy)
	: m_time(toCopy.m_time)
{
	if (&toCopy)
		*this = toCopy;
	else
	{
		m_bEffective = true;
		m_Start = HSS_Time::WTimeSpan(0ULL);
		m_End = HSS_Time::WTimeSpan(0, 23, 59, 59);
		m_MaxWS = 0.0;
		m_MinRH = 100.0;
		m_MinFWI = 0.0;
		m_MinISI = 0.0;

		eff_Start = eff_End = eff_MaxWS = eff_MinRH = eff_MinFWI = eff_MinISI = true;
	}
}


auto Project::CBurnPeriodOption::operator=(const CBurnPeriodOption &toCopy) -> const CBurnPeriodOption&
{
	m_time = toCopy.m_time;
	m_bEffective = toCopy.m_bEffective;
	m_Start = toCopy.m_Start;
	m_End = toCopy.m_End;
	m_MaxWS = toCopy.m_MaxWS;
	m_MinRH = toCopy.m_MinRH;
	m_MinFWI = toCopy.m_MinFWI;
	m_MinISI = toCopy.m_MinISI;
	eff_Start = toCopy.eff_Start;
	eff_End = toCopy.eff_End;
	eff_MaxWS = toCopy.eff_MaxWS;
	eff_MinRH = toCopy.eff_MinRH;
	eff_MinFWI = toCopy.eff_MinFWI;
	eff_MinISI = toCopy.eff_MinISI;
	return *this;
}


Project::CBurnPeriodOption::CBurnPeriodOption(const HSS_Time::WTimeManager *tm)
	: m_time(0, tm)
{
	m_bEffective = true;
	m_Start = HSS_Time::WTimeSpan(0ULL);
	m_End = HSS_Time::WTimeSpan(0, 23, 59, 59);
	m_MaxWS = 0.0;
	m_MinRH = 100.0;
	m_MinFWI = 0.0;
	m_MinISI = 0.0;

	eff_Start = eff_End = eff_MaxWS = eff_MinRH = eff_MinFWI = eff_MinISI = true;
}


void Project::CBurnPeriodOption::SaveToIniFile(const TCHAR *group_name) const
{
#if _DLL
	CWinAppExt *app = (CWinAppExt *)AfxGetApp();
#else
	const gsl::not_null<IniSettings*> app = AfxGetApp();
#endif
	app->WriteProfileLong(group_name, _T("Burn Start4"), gsl::narrow_cast<unsigned long>(m_Start.GetTotalSeconds()));
	app->WriteProfileLong(group_name, _T("Burn End4"), gsl::narrow_cast<unsigned long>(m_End.GetTotalSeconds()));
	app->WriteProfileDouble(group_name, _T("Extinguish Max WS"), m_MaxWS, _T("%.2f"));
	app->WriteProfileDouble(group_name, _T("Extinguish Min RH"), m_MinRH, _T("%.2f"));
	app->WriteProfileDouble(group_name, _T("Extinguish Min FWI"), m_MinFWI, _T("%.2f"));
	app->WriteProfileDouble(group_name, _T("Extinguish Min ISI1"), m_MinISI, _T("%.2f"));

	app->WriteProfileBOOL(group_name, _T("Burn Start3 Effective"), eff_Start ? true : false);
	app->WriteProfileBOOL(group_name, _T("Burn End3 Effective"), eff_End ? true : false);
	app->WriteProfileBOOL(group_name, _T("Extinguish Max WS Effective"), eff_MaxWS ? true : false);
	app->WriteProfileBOOL(group_name, _T("Extinguish Min RH Effective"), eff_MinRH ? true : false);
	app->WriteProfileBOOL(group_name, _T("Extinguish Min FWI Effective"), eff_MinFWI ? true : false);
	app->WriteProfileBOOL(group_name, _T("Extinguish Min ISI1 Effective"), eff_MinISI ? true : false);
}
