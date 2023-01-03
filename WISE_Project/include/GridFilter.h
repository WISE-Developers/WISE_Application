/**
 * WISE_Project: GridFilter.h
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

#ifndef _GRID_FILTER_H_
#define _GRID_FILTER_H_

#include "intrusive_ptr.h"
#include "cwfgmp_config.h"
#include "CWFGM_FuelMap.h"
#include "ICWFGM_GridEngine.h"
#include "GridCOM.h"
#include "points.h"
#include "linklist.h"

#include <functional>
#include <gsl/util>


namespace Project
{
	class Scenario;

	class CWFGMPROJECT_EXPORT GridFilter : public MinNode
	{
	public:
		virtual ~GridFilter();
		virtual GridFilter *Duplicate() const;

		__INLINE GridFilter *LN_Succ() const			{ return (GridFilter *)MinNode::LN_Succ(); }
		__INLINE GridFilter *LN_Pred() const			{ return (GridFilter *)MinNode::LN_Pred(); }

		virtual std::string DisplayName() const			{ using namespace std::string_literals; if (m_name.length()) return m_name; else return "(No Name)"s; }
		virtual const std::string TypeString() const    { using namespace std::string_literals; return ""s; }

		virtual COLORREF BoundaryColor() const noexcept { return gsl::narrow_cast<COLORREF>(-1); }
		virtual ULONG BoundaryWidth() const noexcept 	{ return gsl::narrow_cast<ULONG>(-1); }

		ICWFGM_GridEngine *gridEngine(const Project::Scenario *s) const;

		virtual void SetFuelMap(CCWFGM_FuelMap * /*fuelMap*/) { }

		__INLINE HRESULT LockState()					{ return m_filter->MT_Lock(nullptr, false, gsl::narrow_cast<USHORT>(-1)); }

		std::string	m_name,
					m_comments,
					m_loadWarning;
		boost::intrusive_ptr<ICWFGM_GridEngine>		m_filter;		// the filter represented by this object

	protected:
		virtual void Clone(boost::intrusive_ptr<ICWFGM_GridEngine>* /*grid*/) const	{ weak_assert(false);; }
		virtual GridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const;
	};
};

#endif
