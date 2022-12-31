/**
 * WISE_Project: LandscapeWarningOptions.cpp
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
#define BOOL_CHECK ? true : false
#else
#include "AfxIniSettings.h"
#define BOOL_CHECK
#endif
#include "LandscapeWarningOptions.h"


Project::LandscapeWarningOptions::LandscapeWarningOptions()
{
	m_fixLoading = 0;
	m_warnOnDelete = true;
}


Project::LandscapeWarningOptions::LandscapeWarningOptions(const TCHAR *group_name)
{
	m_fixLoading = 0;
#if _DLL
	CWinAppExt *app = (CWinAppExt *)AfxGetApp();
#else
	auto app = AfxGetApp();
#endif
	m_warnOnDelete = app->GetProfileBOOL(group_name, _T("Warn on Delete"), true) BOOL_CHECK;
}


void Project::LandscapeWarningOptions::SaveToIniFile(const TCHAR *group_name) const
{
#if _DLL
	CWinAppExt* app = (CWinAppExt*)AfxGetApp();
#else
	auto app = AfxGetApp();
#endif
	app->WriteProfileBOOL(group_name, _T("Warn on Delete"), m_warnOnDelete);
}


void Project::LandscapeWarningOptions::AdjustLoading(std::uint32_t version)
{
	m_fixLoading = version;
}
