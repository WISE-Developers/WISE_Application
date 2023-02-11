/**
 * WISE_Project: StdFireCollection.h
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

#if !defined(AFX_STDFIRECOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
#define AFX_STDFIRECOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "intrusive_ptr.h"
#include "cwfgmp_config.h"
#include "ICWFGM_Fuel.h"
#include "CWFGM_Fire.h"
#include "FireEngine_ext.h"
#include "linklist.h"
#include "points.h"
#include "WTime.h"


namespace Project
{
	class StdFireCollection;
	class CWFGMPROJECT_EXPORT StdFire : public MinNode
	{
		friend class StdFireCollection;
	public:
		StdFire(const StdFireCollection &fireCollection);
		StdFire(const StdFireCollection &fireCollection, const StdFire &fire);
		StdFire(const StdFireCollection &fireCollection, boost::intrusive_ptr<CCWFGM_Ignition> &fire);
		virtual ~StdFire();

		StdFire *LN_Succ() const			{ return (StdFire *)MinNode::LN_Succ(); };
		StdFire *LN_Pred() const			{ return (StdFire *)MinNode::LN_Pred(); };

		bool operator==(const StdFire &toCompare) const									{ if (&toCompare == this) return true; return m_fire == toCompare.m_fire; };
		bool operator==(const boost::intrusive_ptr<CCWFGM_Ignition> &toCompare) const	{ return m_fire == toCompare; };

		HSS_Time::WTime IgnitionTime();			// Return the ignition time of this fire
		HRESULT IgnitionTime(const HSS_Time::WTime &time)	{ return HIgnitionTime(time); };	// Set the ignition time of this fire;

		HRESULT LockState()				{ return m_fire->MT_Lock(false, (USHORT)-1); };

		std::uint32_t GetIgnitionCount()	{ std::uint32_t i; if (SUCCEEDED(m_fire->GetIgnitionCount(&i))) return i; return 0; };
		void ClearAllIgnitions()			{ m_fire->ClearIgnition((ULONG)-1); };

		HRESULT SetIgnition(std::uint32_t index, std::uint16_t ignition_type, std::uint32_t ignition_size, XY_Point *points);

		std::uint32_t GetIgnitionSize(std::uint32_t index);
		std::uint32_t GetIgnitionMaxSize();
		XY_Point *GetIgnition(std::uint32_t index, std::uint16_t *ignition_type, std::uint32_t *ignition_size) const;
								// Set the ignition location for this fire...ignition_type is a valid define
								// which can be found in "../FireEngine/FireEngine_ext.h" - it's passed
								// to the COM object (if it exists)
		USHORT IgnitionType(ULONG index)	// Return the type of ignition that this fire object represents
									{ USHORT i; if (SUCCEEDED(m_fire->GetIgnitionType(index, &i))) return i;
									  return CWFGM_FIRE_IGNITION_UNDEFINED;
									};
		static USHORT IgnitionType(ULONG index, CCWFGM_Ignition *fire)
								// Return the type of ignition that this fire object represents
									{ USHORT i; if (SUCCEEDED(fire->GetIgnitionType(index, &i))) return i;
									  return CWFGM_FIRE_IGNITION_UNDEFINED;
									};

		HRESULT ImportIgnition(const std::filesystem::path &file_name, const std::vector<std::string_view> &permissible_drivers);
		HRESULT ImportIgnitionWFS(const std::string &server, const std::string &layer, const std::string &uid, const std::string &pwd);
		HRESULT ExportIgnition(std::string_view driver_name, const std::string &export_projection, const std::filesystem::path &file_name);
		void ExportBasicInfo(FILE *fp);

		std::string	m_name,			// name for the CWFGM fire object
					m_comments;		// comments for this object
		boost::intrusive_ptr<CCWFGM_Ignition> m_fire;		// m_fuel's UserData should point at this

	protected:
		const StdFireCollection	&m_fireCollection;

		HRESULT HIgnitionTime(const HSS_Time::WTime &time);
	};


	class CWFGMPROJECT_EXPORT StdFireCollection
	{
	protected:
		MinListTempl<StdFire>	m_fireList;

	public:
		StdFireCollection(const class CWFGMProject *project);
		virtual ~StdFireCollection();

		void AddFire(StdFire *fire)			{ m_fireList.AddTail(fire); };
		void RemoveFire(StdFire *fire)			{ m_fireList.Remove(fire); };
		ULONG GetCount() const					{ return m_fireList.GetCount(); };
		StdFire *FirstFire() const				{ return m_fireList.LH_Head(); };
		StdFire *LastFire() const				{ return m_fireList.LH_Tail(); };
		StdFire *FireAtIndex(ULONG index) const{ return m_fireList.IndexNode(index); };
		ULONG IndexOf(StdFire *fire) const		{ return m_fireList.NodeIndex(fire); };

		virtual StdFire *FindName(const std::string& name) const;

		bool AssignNewGrid(ICWFGM_GridEngine *grid);

		const class CWFGMProject *m_project;
	};
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDFIRECOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
