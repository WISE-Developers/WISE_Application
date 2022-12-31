/**
 * WISE_Project: statsentry.cpp
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
#include "cwinapp.h"

#define WRITE_STRING(str) str.c_str()
#else
#include "AfxIniSettings.h"
#define WRITE_STRING(str) str
#endif

#include "FireEngine_ext.h"
#include "statsentry.h"
#include "str_printf.h"

#include <boost/algorithm/string.hpp>

using namespace std::string_literals;


Project::CWFGMStatsEntry::CWFGMStatsEntry(const CWFGMStatsEntry& se, UnitConversion* u) :
	m_displaySucc(nullptr),
	m_attributeNameStr(se.m_attributeNameStr),
	m_attributeTitleStr(se.m_attributeTitleStr),
	m_attributeFormatStr(se.m_attributeFormatStr),
	m_operation(se.m_operation),
	m_resolution(se.m_resolution),
	m_threshold(se.m_threshold),
	m_attributeRuleStr(se.m_attributeRuleStr),
	m_attributeShortTextStr(se.m_attributeShortTextStr),
	m_attributeTipTextStr(se.m_attributeTipTextStr),
	m_unitConversion(u)
{ }


void Project::CWFGMStatsEntry::Initialize(const viewentry* ve)
{
	m_attributeNameStr = ve->attributeName;
	m_attributeTitleStr = ve->attributeTitle;
	m_attributeFormatStr = ve->attributeFormat;
	m_operation = ve->operation;
	m_resolution = ve->resolution;
	m_threshold = ve->range_threshold;
	m_attributeShortTextStr = ve->attributeShortText;
	m_attributeTipTextStr = ve->attributeTipText;
	if (ve->attributeRule)
		m_attributeRuleStr = ve->attributeRule;
	else
		m_attributeRuleStr = ""s;
}


Project::CWFGMStatsEntry::CWFGMStatsEntry(const char* group_name, UnitConversion * u) :
	m_displaySucc(nullptr),
	m_unitConversion(u)
{
#if _DLL
	CWinAppExt* app = (CWinAppExt*)AfxGetApp();
#else
	IniSettings* app = AfxGetApp();
#endif
	m_attributeNameStr = app->GetProfileString(group_name, _T("Attribute Name"), _T(""));
	m_attributeTitleStr = app->GetProfileString(group_name, _T("Attribute Title Format"), _T(""));
	m_attributeFormatStr = app->GetProfileString(group_name, _T("Attribute Data Format"), _T(""));
	m_operation = app->GetProfileLong(group_name, _T("Operation"), 0);
	m_resolution = app->GetProfileLong(group_name, _T("Resolution"), 0);
	if (m_resolution > 10)
		m_resolution = 1;	// dealing with obvious stupid things that once appeared on my laptop
	m_threshold = app->GetProfileDouble(group_name, _T("Threshold"), 0.0);
	m_attributeRuleStr = app->GetProfileString(group_name, _T("Secondary Rule"), _T(""));
	m_attributeShortTextStr = app->GetProfileString(group_name, _T("Attribute Short Name"), _T(""));
	m_attributeTipTextStr = app->GetProfileString(group_name, _T("Attribute Tooltip Text"), _T(""));
}


void Project::CWFGMStatsEntry::SaveToIniFile(const TCHAR* group_name) const
{
#if _DLL
	CWinAppExt* app = (CWinAppExt*)AfxGetApp();
#else
	IniSettings* app = AfxGetApp();
#endif
	app->WriteProfileString(group_name, _T("Attribute Name"), WRITE_STRING(m_attributeNameStr));
	app->WriteProfileString(group_name, _T("Attribute Title Format"), WRITE_STRING(m_attributeTitleStr));
	app->WriteProfileString(group_name, _T("Attribute Data Format"), WRITE_STRING(m_attributeFormatStr));
	app->WriteProfileLong(group_name, _T("Operation"), m_operation);
	app->WriteProfileLong(group_name, _T("Resolution"), m_resolution);
	app->WriteProfileDouble(group_name, _T("Threshold"), m_threshold, _T("%16lf"));
	app->WriteProfileString(group_name, _T("Secondary Rule"), WRITE_STRING(m_attributeRuleStr));
	app->WriteProfileString(group_name, _T("Attribute Short Name"), WRITE_STRING(m_attributeShortTextStr));
	app->WriteProfileString(group_name, _T("Attribute Tooltip Text"), WRITE_STRING(m_attributeTipTextStr));
}


std::string Project::CWFGMStatsEntry::getUnits() const
{
	std::string units;
	// then we've got units to deal with
	if (m_operation & 0x80000000)
	{
		if ((m_operation & 0x0000ffff) == OPERATION_STATS_FBP)
			units = "%";
		else {
			long op = m_operation & 0x0fffffff;
			op = op >> 16;
			switch (op) {
			case 18:/* dew pt */
			case 1: /* temperature */						units = m_unitConversion->UnitName(m_unitConversion->TempDisplay(), true); break;
			case 5: /* precip */							units = m_unitConversion->UnitName(m_unitConversion->SmallMeasureDisplay(), true); break;
			case 4: /* wind speed */						units = m_unitConversion->UnitName(m_unitConversion->VelocityDisplay(), true); break;
			case 3: /* wind direction */					units = m_unitConversion->UnitName(STORAGE_FORMAT_ANGLE | STORAGE_FORMAT_DEGREE, true); break;
			case 40: /* maximum ROS */						units = m_unitConversion->UnitName(m_unitConversion->AltVelocityDisplay(), true); break;
			case 41: /* maximum FI */						units = m_unitConversion->UnitName(m_unitConversion->IntensityDisplay(), true); break;
			case CWFGM_FIRE_STAT_ACTIVE_PERIMETER:
			case CWFGM_FIRE_STAT_TOTAL_PERIMETER:
			case CWFGM_FIRE_STAT_EXTERIOR_PERIMETER:		units = m_unitConversion->UnitName(m_unitConversion->AltDistanceDisplay(), true); break;
			case CWFGM_FIRE_STAT_ACTIVE_PERIMETER_GROWTH:
			case CWFGM_FIRE_STAT_TOTAL_PERIMETER_GROWTH:
			case CWFGM_FIRE_STAT_EXTERIOR_PERIMETER_GROWTH:	units = m_unitConversion->UnitName(m_unitConversion->AltDistanceDisplay() | STORAGE_FORMAT_HOUR, true); break;
			case CWFGM_FIRE_STAT_AREA:
			case CWFGM_FIRE_STAT_AREA_CHANGE:				units = m_unitConversion->UnitName(m_unitConversion->AreaDisplay(), true); break;
			case CWFGM_FIRE_STAT_BURNED_CHANGE:				units = m_unitConversion->UnitName(m_unitConversion->AreaDisplay(), true); break;
			case CWFGM_FIRE_STAT_BURNED:					units = m_unitConversion->UnitName(m_unitConversion->AreaDisplay(), true); break;
			case CWFGM_FIRE_STAT_AREA_GROWTH:				units = m_unitConversion->UnitName(m_unitConversion->AreaDisplay() | STORAGE_FORMAT_HOUR, true); break;
			case 43: /* maximum CFC */
			case 44: /* maximum SFC */
			case 45: /* maximum TFC */						units = m_unitConversion->UnitName(m_unitConversion->MassAreaDisplay(), true); break;
			case 46: /* maximum flame length */				units = m_unitConversion->UnitName(m_unitConversion->SmallDistanceDisplay(), true); break;
			case CWFGM_FIRE_STAT_TOTAL_FUEL_CONSUMED:				
			case CWFGM_FIRE_STAT_SURFACE_FUEL_CONSUMED:
			case CWFGM_FIRE_STAT_CROWN_FUEL_CONSUMED:
															units = m_unitConversion->UnitName(m_unitConversion->MassDisplay(), true); break;
			case CWFGM_FIRE_STAT_RADIATIVE_POWER:			units = m_unitConversion->UnitName(m_unitConversion->PowerDisplay(), true); break;
			}
		}
	}
	return units;
}


std::string Project::CWFGMStatsEntry::m_attributeName() const
{
	std::string units = getUnits();
	std::string title = m_attributeTitle();
	return strprintf(title.c_str(), m_attributeNameStr.c_str(), units.c_str());
}


std::string Project::CWFGMStatsEntry::m_attributeTipText() const
{
	std::string units = getUnits();
	return strprintf(m_attributeTipTextStr.c_str(), units.c_str());
}


Project::CWFGMStatsEntryCollection::~CWFGMStatsEntryCollection()
{
	CWFGMStatsEntry* se;
	while (se = m_statsList.RemHead())
		delete se;
}


std::uint32_t Project::CWFGMStatsEntryCollection::NumDisplayableStats() const
{
	std::uint32_t cnt = 0;
	CWFGMStatsEntry* se = m_displayFirst;
	while (se)
	{
		cnt++;
		se = se->m_displaySucc;
	}
	return cnt;
}


bool Project::CWFGMStatsEntryCollection::MakeDisplayable(CWFGMStatsEntry* stat, bool displayable, std::uint32_t index)
{
	bool retval = false;
	// first try to remove it from the list
	std::uint32_t cur_index = 0;
	CWFGMStatsEntry* se = m_displayFirst;
	if (se == stat)
	{
		m_displayFirst = se->m_displaySucc;
		se->m_displaySucc = NULL;
		retval = true;
	}
	else
	{
		while (se)
		{
			cur_index++;
			if (se->m_displaySucc == stat)
			{
				se->m_displaySucc = stat->m_displaySucc;
				stat->m_displaySucc = NULL;
				retval = true;
				break;
			}
			se = se->m_displaySucc;
		}
	}
	if (displayable)
	{
		if (!m_displayFirst)
			m_displayFirst = stat;
		else if (index == (std::uint32_t)-1)
		{
			se = m_displayFirst;
			while (se->m_displaySucc)
				se = se->m_displaySucc;
			se->m_displaySucc = stat;
		}
		else if (!index)
		{
			stat->m_displaySucc = m_displayFirst;
			m_displayFirst = stat;
		}
		else
		{
			se = m_displayFirst;
			index--;
			while ((index > 0) && se->m_displaySucc)
			{
				index--;
				se = se->m_displaySucc;
			}
			stat->m_displaySucc = se->m_displaySucc;
			se->m_displaySucc = stat;
		}
		retval = true;
	}
	return retval;
}


std::uint32_t Project::CWFGMStatsEntryCollection::AddStat(CWFGMStatsEntry* stat, std::uint32_t index)
{
	if (index == (std::uint32_t)-1)
	{
		m_statsList.AddTail(stat);
		return (std::uint32_t)(m_statsList.GetCount() - 1);
	}
	CWFGMStatsEntry* _where = m_statsList.LH_Head();
	std::uint32_t insert = 0;
	while ((_where->LN_Succ()) && (index > 0))
	{
		index--;
		insert++;
		_where = _where->LN_Succ();
	}
	m_statsList.Insert(stat, _where);
	return insert;
}


void Project::CWFGMStatsEntryCollection::RemoveStat(CWFGMStatsEntry* stat)
{
	CWFGMStatsEntry* se = m_displayFirst;
	if (se == stat)
		m_displayFirst = se->m_displaySucc;
	else
	{
		while (se)
		{
			if (se->m_displaySucc == stat)
			{
				se->m_displaySucc = stat->m_displaySucc;
				stat->m_displaySucc = NULL;
				break;
			}
			se = se->m_displaySucc;
		}
	}
	m_statsList.Remove(stat);
}


std::uint32_t Project::CWFGMStatsEntryCollection::DisplayIndexOf(CWFGMStatsEntry* stat) const
{
	std::uint32_t index = 0;
	CWFGMStatsEntry* se = m_displayFirst;
	while (se)
	{
		if (se == stat)
			return index;
		se = se->LN_Next();
		index++;
	}
	return (std::uint32_t)-1;
}


auto Project::CWFGMStatsEntryCollection::DisplayStatsAt(std::uint32_t index) const -> CWFGMStatsEntry*
{
	CWFGMStatsEntry* se = m_displayFirst;
	while ((index > 0) && (se))
	{
		index--;
		se = se->m_displaySucc;
	}
	return se;
}


auto Project::CWFGMStatsEntryCollection::FindStat(const TCHAR* attributeNameStr) const -> CWFGMStatsEntry*
{
	CWFGMStatsEntry* se = m_displayFirst;
	if (se)
		while (se->LN_Succ())
		{
			if (boost::iequals(se->m_attributeNameStr, attributeNameStr))
				return se;
			se = se->LN_Succ();
		}
	return NULL;
}


void Project::CWFGMStatsEntryCollection::SaveToIniFile(const TCHAR* group_name) const
{
#if _DLL
	CWinAppExt* app = (CWinAppExt*)AfxGetApp();
#else
	IniSettings* app = AfxGetApp();
#endif
	std::uint32_t	scnt = m_statsList.GetCount(),
		dcnt = NumDisplayableStats();

	app->WriteProfileInt(group_name, _T("Number Attributes"), scnt),
		app->WriteProfileInt(group_name, _T("Number Displayable Attributes"), dcnt);
	CWFGMStatsEntry* se = m_statsList.LH_Head();
	scnt = 0;
	while (se->LN_Succ())
	{
		std::string str = strprintf(_T("%s: Statistics Entry #%d"), group_name, (int)scnt);
		se->SaveToIniFile(str.c_str());
		se = se->LN_Succ();
		scnt++;
	}
	if (dcnt)
	{
		se = DisplayStatsAt(0);
		dcnt = 0;
		while (se)
		{
			std::string str = strprintf(_T("Display Stats Index #%d"), (int)dcnt);
			scnt = m_statsList.NodeIndex(se);
			app->WriteProfileInt(group_name, WRITE_STRING(str), scnt);
			se = se->LN_Next();
			dcnt++;
		}
	}
}


void Project::CWFGMStatsEntryCollection::Initialize(const TCHAR* group_name)
{
#if _DLL
	CWinAppExt* app = (CWinAppExt*)AfxGetApp();
#else
	IniSettings* app = AfxGetApp();
#endif
	m_displayFirst = NULL;
	ULONG i, scnt = (ULONG)app->GetProfileInt(group_name, _T("Number Attributes"), 0),
		dcnt = (ULONG)app->GetProfileInt(group_name, _T("Number Displayable Attributes"), 0);
	Project::CWFGMStatsEntry* se = nullptr;
	try
	{
		for (i = 0; i < scnt; i++)
		{
			std::string str = strprintf(_T("%s: Statistics Entry #%d"), group_name, (int)i);
			se = NULL;
			se = New(str.c_str());
			m_statsList.AddTail(se);
		}
		for (i = 0; i < dcnt; i++)
		{
			std::string str = strprintf(_T("Display Stats Index #%d"), (int)i);
			scnt = (ULONG)app->GetProfileInt(group_name, str.c_str(), -1);
			if (scnt != (ULONG)-1)
			{
				Project::CWFGMStatsEntry* se1 = m_statsList.IndexNode(scnt);
				if (se1)
					MakeDisplayable(se1, true);
			}
		}
	}
	catch (std::bad_alloc& cme)
	{
		if (se)
			delete se;
		while (se = m_statsList.RemHead())
			delete se;
		throw cme;
	}
}


#if _DLL
void Project::CWFGMStatsEntryCollection::Initialize(const viewentry* default_entries)
{
	if (default_entries)
	{
		const viewentry* ve = default_entries;
		Project::CWFGMStatsEntry* se{ nullptr };
		try
		{
			while (ve->attributeName)
			{
				se = nullptr;
				se = New();
				se->Initialize(ve);
				m_statsList.AddTail(se);
				MakeDisplayable(se, !ve->defaultHidden);
				ve++;
			}
		}
		catch(std::bad_alloc& cme)
		{
			if (se)
				delete se;
			while (se = m_statsList.RemHead())
				delete se;
			throw cme;
		}
	}
}
#endif


Project::CWFGMStatsEntryCollection& Project::CWFGMStatsEntryCollection::operator=(const CWFGMStatsEntryCollection& sec)
{
	if (&sec != this)
	{
		CWFGMStatsEntry* copy = nullptr, * se;
		while (se = m_statsList.RemHead())
			delete se;
		m_displayFirst = NULL;

		try
		{
			se = sec.FirstStat();
			while (se->LN_Succ()) {
				copy = NULL;
				copy = New(*se);
				m_statsList.AddTail(copy);
				se = se->LN_Succ();
			}
			se = sec.m_displayFirst;
			while (se) {
				std::uint32_t index = sec.m_statsList.NodeIndex(se);
				copy = m_statsList.IndexNode(index);

		#ifdef DEBUG
				weak_assert(copy);
		#endif

				MakeDisplayable(copy, true);
				se = se->LN_Next();
			}
		} 
		catch (std::bad_alloc& cme)
		{
			if (copy)
				delete copy;
			while (se = m_statsList.RemHead())
				delete se;
			throw cme;
		}
	}
	return *this;
}


Project::CWFGMStatsEntry* Project::CWFGMStatsEntryCollection::New() const
{
	return new CWFGMStatsEntry(m_unitConversion);
}


Project::CWFGMStatsEntry* Project::CWFGMStatsEntryCollection::New(const CWFGMStatsEntry& se) const
{
	return new CWFGMStatsEntry(se, m_unitConversion);
}


Project::CWFGMStatsEntry* Project::CWFGMStatsEntryCollection::New(const char* group_name) const
{
	return new CWFGMStatsEntry(group_name, m_unitConversion);
}


Project::CWFGMStatsEntryCollection* Project::CWFGMStatsEntryCollection::NewCollection()
{
	return new CWFGMStatsEntryCollection(m_unitConversion);
}
