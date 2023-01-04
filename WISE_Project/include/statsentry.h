/**
 * WISE_Project: statsentry.h
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

#pragma once
#include "StatsConfig.h"
#if _DLL
#undef OPERATION_MIN
#undef OPERATION_MAX
#undef OPERATION_MEAN
#undef OPERATION_MEDIAN
#undef OPERATION_TOTAL
#undef OPERATION_TOTAL_PER_HA
#undef OPERATION_SUM
#undef OPERATION_SUM_PER_HA
#undef OPERATION_RANGE_COUNT
#undef OPERATION_UNDEFINED
#undef OPERATION_DOUBLE
#undef OPERATION_INT
#undef OPERATION_STRING
#undef OPERATION_4CHAR
#undef OPERATION_BOOL
#undef OPERATION_DATETIME
#undef OPERATION_TIMESPAN
#undef OPERATION_STEPCOLOR
#undef OPERATION_STATS_FBP
#undef OPERATION_DOUBLEPERCENT
#undef OPERATION_ULONGLONG
#undef OPERATION_SPECIAL
#endif

#include "cwfgmp_config.h"
#include "convert.h"
#include "Dlgcnvt.h"
#include "linklist.h"

namespace Project
{
	class CWFGMPROJECT_EXPORT CWFGMStatsEntry : public MinNode
	{
		friend class CWFGMStatsEntryCollection;

	private:
		CWFGMStatsEntry* m_displaySucc;			// for the list for all entries that are selected to be displayed

		UnitConversion* m_unitConversion;

	protected:
		std::string		m_attributeShortTextStr,
						m_attributeNameStr,		// name of the attribute of interest
						m_attributeTitleStr,	// format string for title for display purposes
						m_attributeFormatStr,	// format string for data for display purposes
						m_attributeTipTextStr;

	public:
		std::int32_t	m_operation,
						m_resolution;

		double			m_threshold;			// if we're doing any thresholding on this stats entry (for counts)
		std::string		m_attributeRuleStr;		// if this is set, then the threshold applies to this attribute instead of the
												// one being reported on

		constexpr static std::int16_t OPERATION_MIN = 1;
		constexpr static std::int16_t OPERATION_MAX = 2;
		constexpr static std::int16_t OPERATION_MEAN = 3;		// min, max, average are for calculating values for floating point attributes
		constexpr static std::int16_t OPERATION_MEDIAN = 4;
		constexpr static std::int16_t OPERATION_TOTAL = 5;		// for counts for boolean attributes
		constexpr static std::int16_t OPERATION_TOTAL_PER_HA = 6;		// same as OPERATION_TOTAL only per ha
		constexpr static std::int16_t OPERATION_SUM = 7;		// sums of floating point attributes
		constexpr static std::int16_t OPERATION_SUM_PER_HA = 8;	// same as OPERATION_SUM only per ha
		constexpr static std::int16_t OPERATION_RANGE_COUNT = 9;		// counts for floating point attributes (whether a value fits in a range)
		constexpr static std::int16_t OPERATION_UNDEFINED = 0;		// shouldn't be used
		constexpr static std::int16_t OPERATION_DOUBLE = 100;
		constexpr static std::int16_t OPERATION_INT = 101;
		constexpr static std::int16_t OPERATION_STRING = 102;
		constexpr static std::int16_t OPERATION_4CHAR = 103;
		constexpr static std::int16_t OPERATION_BOOL = 104;
		constexpr static std::int16_t OPERATION_DATETIME = 105;
		constexpr static std::int16_t OPERATION_TIMESPAN = 106;
		constexpr static std::int16_t OPERATION_STEPCOLOR = 107;
		constexpr static std::int16_t OPERATION_STATS_FBP = 108;
		constexpr static std::int16_t OPERATION_DOUBLEPERCENT = 109;
		constexpr static std::int16_t OPERATION_ULONGLONG = 110;
		constexpr static std::int16_t OPERATION_SPECIAL = -1;	// to flag something that is unusual

	protected:
		CWFGMStatsEntry() = delete;
		CWFGMStatsEntry(UnitConversion* u) : m_displaySucc(nullptr), m_operation(OPERATION_UNDEFINED), m_unitConversion(u) { }
		CWFGMStatsEntry(const CWFGMStatsEntry& se, UnitConversion* u);
		CWFGMStatsEntry(const char* group_name, UnitConversion* u);

	public:
		CWFGMStatsEntry* LN_Succ() const { return (CWFGMStatsEntry*)MinNode::LN_Succ(); }
		CWFGMStatsEntry* LN_Pred() const { return (CWFGMStatsEntry*)MinNode::LN_Pred(); }
		CWFGMStatsEntry* LN_Next() const { return m_displaySucc; }

		virtual std::string m_attributeRule() const { return m_attributeRuleStr; }
		virtual std::string m_attributeTitle() const { return m_attributeTitleStr; }
		virtual std::string m_attributeFormat() const { return m_attributeFormatStr; }
		virtual std::string m_attributeShortText() const { return m_attributeShortTextStr; }

		std::string getUnits() const;

		void Initialize(const viewentry* ve);

	public:
		virtual std::string m_attributeName() const;
		virtual std::string m_attributeTipText() const;

		void SaveToIniFile(const TCHAR* group_name) const;
	};


	class CWFGMPROJECT_EXPORT CWFGMStatsEntryCollection
	{
	protected:
		MinListTempl<CWFGMStatsEntry>	m_statsList;	// list for all possible stat's
		CWFGMStatsEntry* m_displayFirst{ nullptr };
		UnitConversion* m_unitConversion;

	public:
		CWFGMStatsEntryCollection() = delete;
		CWFGMStatsEntryCollection(UnitConversion* u) : m_unitConversion(u) { };
		virtual ~CWFGMStatsEntryCollection();

		CWFGMStatsEntryCollection& operator=(const CWFGMStatsEntryCollection& sec);

		void Initialize(const TCHAR* group_name);
#if _DLL
		void Initialize(const viewentry* ve);
#endif

		std::uint32_t NumStats() const { return m_statsList.GetCount(); }
		std::uint32_t NumDisplayableStats() const;
		bool MakeDisplayable(CWFGMStatsEntry* stat, bool displayable, std::uint32_t index = (std::uint32_t)-1);
		// if (displayable) then stat is added/moved to the list at (index)
		// it's added before the displayable entry 'index', to become the new
		// 'index' entry, or if index is -1, added to the end of the list.
		std::uint32_t AddStat(CWFGMStatsEntry* stat, std::uint32_t index = (std::uint32_t)-1);
		// adds a completely new stat to the set, returns its index in the main list
		void RemoveStat(CWFGMStatsEntry* stat);		// removes the stat from both lists

		std::uint32_t IndexOf(CWFGMStatsEntry* stat) const { return m_statsList.NodeIndex(stat); }
		std::uint32_t DisplayIndexOf(CWFGMStatsEntry* stat) const;

		virtual CWFGMStatsEntry* New() const;
		virtual CWFGMStatsEntry* New(const CWFGMStatsEntry& se) const;
		virtual CWFGMStatsEntry* New(const char* group_name) const;

		virtual CWFGMStatsEntryCollection* NewCollection();

		CWFGMStatsEntry* FirstStat() const { return m_statsList.LH_Head(); }
		CWFGMStatsEntry* StatsAt(std::uint32_t index) const { return m_statsList.IndexNode(index); }
		CWFGMStatsEntry* FirstDisplayStat() const { return m_displayFirst; }

		CWFGMStatsEntry* DisplayStatsAt(std::uint32_t index) const;
		CWFGMStatsEntry* FindStat(const TCHAR* attributeNameStr) const;

		void SaveToIniFile(const TCHAR* group_name) const;
	};
};
