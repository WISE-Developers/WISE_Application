/**
 * WISE_Project: GridFilterCollection.h
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

#if !defined(AFX_GRIDFILTERCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
#define AFX_GRIDFILTERCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cwfgmp_config.h"

#include "ICWFGM_Fuel.h"
#include "GridCOM.h"
#include "linklist.h"
#include "points.h"
#include "ISerializeProto.h"


#include "GridFilter.h"
#include "gridCollection.pb.h"


namespace Project
{
	class CWFGMPROJECT_EXPORT GridFilterCollection : public ISerializeProto
	{
	public:
		const class CWFGMProject *m_project;

		GridFilterCollection(const CWFGMProject *project) : m_project(project) { }
		virtual ~GridFilterCollection();

		__INLINE void AddFilter(GridFilter *filter)		{ m_filterList.AddTail(filter); }
		__INLINE void RemoveFilter(GridFilter *filter)		{ m_filterList.Remove(filter); }

		template<typename T = std::uint32_t>
		std::uint32_t GetCount(bool wxpatch_islandscape) const;

		__INLINE GridFilter *FirstFilter() const		{ return m_filterList.LH_Head(); }
		__INLINE GridFilter *FilterAtIndex(ULONG index) const	{ return m_filterList.IndexNode(index); }
		__INLINE ULONG IndexOf(GridFilter *filter) const	{ return m_filterList.NodeIndex(filter); }

		GridFilter *FindName(const char *name) const;

		bool AssignNewGrid(CCWFGM_FuelMap *fuelMap, ICWFGM_GridEngine *grid);

		std::string CollectLoadWarnings();

		void UpdateDeprecatedWeatherFilters();

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual GridFilterCollection *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	protected:
		boost::intrusive_ptr<CCWFGM_FuelMap>		m_fuelMap;
		boost::intrusive_ptr<ICWFGM_GridEngine>	m_gridEngine;
		MinListTempl<GridFilter>	m_filterList;
		std::string				m_loadWarning;
	};
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDFILTERCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
