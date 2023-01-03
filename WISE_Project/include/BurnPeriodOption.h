/**
 * WISE_Project: BurnPeriodOption.h
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

#if !defined(AFX_BURNPERIODOPTION_H__006D8675_52DF_43F9_A56C_AE0BF194B380__INCLUDED_)
#define AFX_BURNPERIODOPTION_H__006D8675_52DF_43F9_A56C_AE0BF194B380__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cwfgmp_config.h"
#include "WTime.h"
#include "hssconfig/config.h"

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 8)
#endif

namespace Project
{
	class CWFGMPROJECT_EXPORT CBurnPeriodOption
	{
	public:
		double m_MinISI;
		double m_MinFWI;
		double m_MinRH;
		double m_MaxWS;
		HSS_Time::WTimeSpan m_Start, m_End;
		HSS_Time::WTime m_time;

		bool m_bEffective;
		bool	eff_MinISI, eff_MinFWI, eff_MinRH, eff_MaxWS, eff_Start, eff_End;

		CBurnPeriodOption(const TCHAR *szGroup);
		CBurnPeriodOption(const CBurnPeriodOption &toCopy);
		CBurnPeriodOption(const HSS_Time::WTimeManager *tm);

		void SaveToIniFile(const TCHAR *group_name) const;

		const CBurnPeriodOption &operator=(const CBurnPeriodOption &toCopy);
	};
};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#endif // !defined(AFX_BURNPERIODOPTION_H__006D8675_52DF_43F9_A56C_AE0BF194B380__INCLUDED_)
