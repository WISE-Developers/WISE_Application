/**
 * WISE_Project: ReplaceGridFilter.cpp
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

#ifdef _MSC_VER
#include "stdafx.h"
#endif

#include <string>
#include "ReplaceGridFilter.h"
#include "CWFGM_ReplaceGridFilter.h"
#include "CWFGM_PolyReplaceGridFilter.h"
#include "ScenarioCollection.h"
#include "XY_PolyType.h"
#include "str_printf.h"
#ifndef _MSC_VER
#include "BaseObject.h"
#endif


using namespace std::string_literals;


Project::ReplaceGridBase::ReplaceGridBase() noexcept
{
}


Project::ReplaceGridFilter::ReplaceGridFilter()
{
	m_filter = boost::make_intrusive<CCWFGM_ReplaceGridFilter>();
	PolymorphicUserData v = (void*)this;
	m_filter->put_UserData(v);
}


Project::PolyReplaceGridFilter::PolyReplaceGridFilter()
{
	m_filter = boost::make_intrusive<CCWFGM_PolyReplaceGridFilter>();
	PolymorphicUserData v = (void*)this;
	m_filter->put_UserData(v);
}


Project::PolyReplaceGridFilter::~PolyReplaceGridFilter()
{
}


HRESULT Project::ReplaceGridFilter::SetRelationship(ICWFGM_Fuel *from_fuel, UCHAR from_index, ICWFGM_Fuel *to_fuel)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_ReplaceGridFilter>(m_filter);
	HRESULT hr{ S_OK };
	if (gf)
		hr = gf->SetRelationship(from_fuel, from_index, to_fuel);
	else
		hr = E_FAIL;
	return hr;
}


HRESULT Project::ReplaceGridFilter::GetRelationship(ICWFGM_Fuel **from_fuel, UCHAR *from_index, ICWFGM_Fuel **to_fuel) const
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_ReplaceGridFilter>(m_filter);
	if (gf)
		return gf->GetRelationship(from_fuel, from_index, to_fuel);
	return E_FAIL;
}


HRESULT Project::PolyReplaceGridFilter::SetRelationship(ICWFGM_Fuel *from_fuel, UCHAR from_index, ICWFGM_Fuel *to_fuel)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	HRESULT hr;
	if (gf)
		hr = gf->SetRelationship(from_fuel, from_index, to_fuel);
	else
		hr = E_FAIL;
	return hr;
}


HRESULT Project::PolyReplaceGridFilter::GetRelationship(ICWFGM_Fuel **from_fuel, UCHAR *from_index, ICWFGM_Fuel **to_fuel) const
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (gf)
		return gf->GetRelationship(from_fuel, from_index, to_fuel);
	return E_FAIL;
}


bool Project::ReplaceGridFilter::SetArea(USHORT x1, USHORT y1, USHORT x2, USHORT y2)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_ReplaceGridFilter>(m_filter);
	if (gf)
	{
		XY_Point pt1;
		pt1.x = x1;
		pt1.y = y1;
		XY_Point pt2;
		pt2.x = x2;
		pt2.y = y2;
		return SUCCEEDED(gf->SetArea(pt1, pt2));
	}
	return false;
}


bool Project::ReplaceGridFilter::GetArea(USHORT *x1, USHORT *y1, USHORT *x2, USHORT *y2) const
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_ReplaceGridFilter>(m_filter);
	if (gf)
	{
		XY_Point pt1;
		XY_Point pt2;
		const HRESULT hr = gf->GetArea(&pt1, &pt2);
		*x1 = static_cast<USHORT>(pt1.x);
		*y1 = static_cast<USHORT>(pt1.y);
		*x2 = static_cast<USHORT>(pt2.x);
		*y2 = static_cast<USHORT>(pt2.y);
		return SUCCEEDED(hr);
	}
	return false;
}


void Project::ReplaceGridFilter::Clone(boost::intrusive_ptr<ICWFGM_GridEngine> *grid) const
{
	boost::intrusive_ptr<ICWFGM_CommonBase> gf;
	m_filter->Clone(&gf);
	*grid = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(gf);
}


void Project::PolyReplaceGridFilter::Clone(boost::intrusive_ptr<ICWFGM_GridEngine> *grid) const
{
	boost::intrusive_ptr<ICWFGM_CommonBase> gf;
	m_filter->Clone(&gf);
	*grid = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(gf);
}


auto Project::ReplaceGridBase::Duplicate() const -> ReplaceGridBase*
{
	return Duplicate([] { return new ReplaceGridFilter(); });
}


auto Project::ReplaceGridBase::Duplicate(std::function<GridFilter*(void)> constructor) const -> ReplaceGridBase*
{
	auto gf = dynamic_cast<ReplaceGridFilter*>(GridFilter::Duplicate(constructor));
	if (gf)
	{
		ICWFGM_Fuel *from, *to;
		UCHAR from_i;
		GetRelationship(&from, &from_i, &to);
		gf->SetRelationship(from, from_i, to);
	}
	return gf;
}


auto Project::PolyReplaceGridFilter::Duplicate() const -> PolyReplaceGridFilter*
{
	return Duplicate([] { return new PolyReplaceGridFilter(); });
}


auto Project::PolyReplaceGridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> PolyReplaceGridFilter*
{
	auto gf = dynamic_cast<PolyReplaceGridFilter*>(ReplaceGridBase::Duplicate(constructor));
	if (gf)
	{
		gf->m_color = m_color;
		gf->m_lineWidth = m_lineWidth;
	}
	return gf;
}


std::string Project::ReplaceGridFilter::DisplayName() const
{
	std::string name;
	ICWFGM_Fuel *from_fuel, *to_fuel;
	UCHAR from_index;

	USHORT x1, y1, x2, y2;
	GetArea(&x1, &y1, &x2, &y2);

	if (SUCCEEDED(GetRelationship(&from_fuel, &from_index, &to_fuel)))
	{
		std::string from_name, to_name;
		if (from_fuel == (ICWFGM_Fuel *)~0)
		{
			from_name = "--- All Combustible Fuels ---"s;
		}
		else if (from_fuel == (ICWFGM_Fuel *)(-2))
		{
			from_name = "--- NODATA ---"s;
		}
		else if (from_fuel)
		{
			std::string bname;
			const HRESULT hr = from_fuel->get_Name(&bname);
			if (FAILED(hr))
				AfxThrowOleException(hr);
			from_name = strprintf(_T("%s"), bname.c_str());
		}
		else if ((from_index != (UCHAR)-1) && (m_fuelMap))
		{
			long file_index = -1, export_index;
			m_fuelMap->FuelAtIndex(from_index, &file_index, &export_index, &from_fuel);
			from_name = strprintf(_T("Index %d:"), file_index);
		}
		else
			from_name = "--- All Fuels ---"s;

		if (to_fuel)
		{
			std::string bname;
			const HRESULT hr = to_fuel->get_Name(&bname);
			if (FAILED(hr))
				AfxThrowOleException(hr);
			to_name = bname;
		}
		else
			to_name = "No Fuel"s;

		if (m_name.length())
			name = strprintf(_T("%s (%s to %s)"), m_name.c_str(), from_name.c_str(), to_name.c_str());
		else
			name = strprintf(_T("No Name (%s to %s)"), from_name.c_str(), to_name.c_str());
	}
	else
	{
		if (m_name.length())
			name = strprintf(_T("%s (Not Defined)"), m_name.c_str());
		else	
			name = "No Name (Not Defined)"s;
	}
	return name;
}


const std::string Project::ReplaceGridFilter::TypeString() const
{
	USHORT x1, y1, x2, y2;
	GetArea(&x1, &y1, &x2, &y2);
	if ((x1 == (USHORT)-1) && (y1 == (USHORT)-1) && (x2 == (USHORT)-1) && (y2 == (USHORT)-1))
		return "Landscape Patch"s;
	return "Polygon Patch"s;
}


std::string Project::PolyReplaceGridFilter::DisplayName() const
{
	std::string name;
	ICWFGM_Fuel *from_fuel, *to_fuel;
	UCHAR from_index;
	if (SUCCEEDED(GetRelationship(&from_fuel, &from_index, &to_fuel)))
	{
		std::string from_name, to_name;
		if (from_fuel == (ICWFGM_Fuel *)~0)
		{
			from_name = "--- All Combustible Fuels ---"s;
		}
		else if (from_fuel == (ICWFGM_Fuel *)(-2))
		{
			from_name = "--- NODATA ---"s;
		}
		else if (from_fuel)
		{
			std::string bname;
			const HRESULT hr = from_fuel->get_Name(&bname);
			if (FAILED(hr))
				AfxThrowOleException(hr);
			from_name = strprintf(_T("%s"), bname.c_str());
		}
		else if ((from_index != (UCHAR)-1) && (m_fuelMap))
		{
			long file_index = -1, export_index;
			m_fuelMap->FuelAtIndex(from_index, &file_index, &export_index, &from_fuel);
			from_name = strprintf(_T("Index %d:"), file_index);
		}
		else
			from_name = "--- All Fuels ---"s;

		if (to_fuel)
		{
			std::string bname;
			const HRESULT hr = to_fuel->get_Name(&bname);
			if (FAILED(hr))
				AfxThrowOleException(hr);
			to_name = strprintf(_T("%s"), bname.c_str());
		}
		else
			to_name = "No Fuel"s;

		if (m_name.length())
			name = strprintf(_T("%s (%s to %s)"), m_name.c_str(), from_name.c_str(), to_name.c_str());
		else
			name = strprintf(_T("No Name (%s to %s)"), from_name.c_str(), to_name.c_str());
	}
	else
	{
		if (m_name.length())
			name = strprintf(_T("%s (Not Defined)"), m_name.c_str());
		else
			name = "No Name (Not Defined)"s;
	}
	return name;
}


ULONG Project::PolyReplaceGridFilter::AddPolygon(const XY_Point &points, ULONG num_points)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
	{
		std::uint32_t num;
		XY_Poly *poly = NULL;
		try
		{
			poly = new XY_Poly(&points, num_points);
		}
		catch (std::bad_alloc&)
		{
			return (ULONG)-1;			// invalid index;
		}
		HRESULT r = vf->AddPolygon(*poly, &num);

		delete poly;

		if (SUCCEEDED(r))
			return num;
	}
	return (ULONG)-1;
}


bool Project::PolyReplaceGridFilter::ClearPolygon(ULONG index)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
		return SUCCEEDED(vf->ClearPolygon(index));
	return false;
}


double Project::ReplaceGridFilter::Area()
{
	USHORT x1, y1, x2, y2;
	GetArea(&x1, &y1, &x2, &y2);
	if (x1 != (USHORT)-1)
		return (double)(x2 - x1) * (double)(y2 - y1);
	return -1.0;
}


double Project::PolyReplaceGridFilter::Area()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
	{
		double area;
		vf->GetArea(&area);
		return area;
	}
	return 0.0;
}


ULONG Project::PolyReplaceGridFilter::GetPolygonMaxSize()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
	{
		std::uint32_t num;
		if (SUCCEEDED(vf->GetPolygonSize((std::uint32_t)-1, &num)))
			return num;
	}
	return 0;
}


ULONG Project::PolyReplaceGridFilter::GetPolygonCount()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
	{
		std::uint32_t num;
		if (SUCCEEDED(vf->GetPolygonCount(&num)))
			return num;
	}
	return 0;
}


ULONG Project::PolyReplaceGridFilter::GetPolygonSize(ULONG index)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
	{
		std::uint32_t num;
		if (SUCCEEDED(vf->GetPolygonSize(index, &num)))
			return num;
	}
	return 0;
}


XY_Point *Project::PolyReplaceGridFilter::GetPolygon(ULONG index, XY_Point *points, std::uint32_t *size)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
	{
		XY_Point *p = nullptr;
		if (!points)
		{
			if (!(*size))
				if (FAILED(vf->GetPolygonSize(index, size)))
					return nullptr;
			try
			{
				p = new XY_Point[*size];
			}
			catch (std::bad_alloc&)
			{
				return nullptr;
			}
		}
		else
			p = points;

		XY_Poly poly(p, XY_Poly::ALLOCSTYLE_NEWDELETE);
		const HRESULT r = vf->GetPolygon(index, &poly);
		p = poly.Detach();

		if (SUCCEEDED(r))
			return p;				// success so return either points (if provided) or the memory we had to allocate
		if (p != points)
			delete[] p;				// failure so conditional memory clean-up
	}
	return nullptr;
}


bool Project::PolyReplaceGridFilter::ClearAllPolygons()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
		return SUCCEEDED(vf->ClearPolygon((ULONG)-1));
	return false;
}


HRESULT Project::PolyReplaceGridFilter::ImportPolygons(const std::string &file_name, const std::vector<std::string> *pd)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
		return vf->ImportPolygons(file_name, pd);
	return E_FAIL;
}


HRESULT Project::PolyReplaceGridFilter::ImportPolygonsWFS(const std::string &server, const std::string &layer, const std::string &uid, const std::string &pwd)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
		return vf->ImportPolygonsWFS(server, layer, uid, pwd);
	return E_FAIL;
}


HRESULT Project::PolyReplaceGridFilter::ExportPolygons(const std::string &driver_name, const std::string &export_projection, const std::string &file_name)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_PolyReplaceGridFilter>(m_filter);
	if (vf)
		return vf->ExportPolygons(driver_name, export_projection, file_name);
	return E_FAIL;
}
