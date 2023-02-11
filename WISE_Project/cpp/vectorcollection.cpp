/**
 * WISE_Project: vectorcollection.cpp
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

#include "cwfgmp_config.h"

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

#if _DLL
#include <afxdisp.h>
#include <shlwapi.h>
#endif

#include "vectorcollection.h"
#include "cpoints.h"
#include "GridCom_ext.h"
#include "FontHelper.h"
#include "XY_PolyType.h"
#include "CWFGM_VectorFilter.h"


Project::Vector::Vector()
{
	ln_Name = nullptr;
}


Project::Vector::~Vector()
{
	gridEngine(nullptr);
	if (ln_Name)
		delete[] ln_Name;
}


const TCHAR *Project::Vector::Name(const TCHAR *name)
{
	TCHAR *copy;
	if ((name) && (name[0]))
	{
		try
		{
			copy = new TCHAR[_tcslen(name) + 1];
		}
		catch (std::bad_alloc&)
		{
			return ln_Name;
		}
#ifdef _MSC_VER
		_tcscpy_s(copy, _tcslen(name) + 1, name);
#else
		_tcscpy_s(copy, name);
#endif
	}
	else
		copy = nullptr;
	if (ln_Name)
		delete[] ln_Name;
	ln_Name = copy;
	return ln_Name;
}


ICWFGM_GridEngine *Project::Vector::gridEngine(ICWFGM_GridEngine *gridEngine)
{
	if (!m_filter)
		return gridEngine;
	if (SUCCEEDED(m_filter->put_GridEngine(gridEngine)))
		return gridEngine;
	return nullptr;
}


Project::StaticVector::StaticVector()
{
	m_filter = boost::make_intrusive<CCWFGM_VectorFilter>();
	PolymorphicUserData v((void*)this);
	m_filter->put_UserData(v);
}


Project::StaticVector::~StaticVector() { }


auto Project::StaticVector::Duplicate() const -> Vector*
{
	return new StaticVector(*this);
}


Project::StaticVector::StaticVector(const StaticVector &toCopy)
{
	Name(toCopy.Name());
	Comments(toCopy.Comments());
	m_col = toCopy.m_col;
	m_fillColor = toCopy.m_fillColor;
	m_LineWidth = toCopy.m_LineWidth;
	m_symbol = toCopy.m_symbol;

	boost::intrusive_ptr<ICWFGM_CommonBase> filter;
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(toCopy.m_filter);
	HRESULT hr = vf->Clone(&filter);
	if (FAILED(hr))
		throw std::logic_error("Failed");
	m_filter = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(filter);
	PolymorphicUserData v((void*)this);
	m_filter->put_UserData(v);
}


double Project::StaticVector::GetPolyLineWidth()
{
	double width;
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	vf->get_Width(&width);
	return width;
}


void Project::StaticVector::SetPolyLineWidth(double width)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	vf->put_Width(width);
}


HRESULT Project::StaticVector::ImportPolylines(const std::filesystem::path &file_name, const std::vector<std::string_view> &pd)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	return vf->ImportPolylines(file_name, pd);
}


HRESULT Project::StaticVector::ImportPolylinesWFS(const std::string &server, const std::string &layer, const std::string &uid, const std::string &pwd)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	return vf->ImportPolylinesWFS(server, layer, uid, pwd);
}


HRESULT Project::StaticVector::ExportPolylines(std::string_view driver_name, const std::string &export_projection, const std::filesystem::path &file_name)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	return vf->ExportPolylines(driver_name, export_projection, file_name);
}


ULONG Project::StaticVector::AddPolyLine(const XY_Point *points, ULONG num_points, bool is_polygon)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	std::uint32_t num;

	XY_PolyConst pc(points, num_points);
	HRESULT r = vf->AddPolyLine(pc, is_polygon ? XY_PolyLL_BaseTempl<double>::Flags::INTERPRET_POLYGON : XY_PolyLL_BaseTempl<double>::Flags::INTERPRET_POLYLINE, &num);
	if (SUCCEEDED(r))
		return num;
	return (ULONG)-1;
}


ULONG Project::StaticVector::SetPolyLine(ULONG index, const XY_Point *points, ULONG num_points, bool is_polygon)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	XY_PolyConst poly(points, num_points);
	HRESULT r = vf->SetPolyLine(index, poly, is_polygon ? XY_PolyLL_BaseTempl<double>::Flags::INTERPRET_POLYGON : XY_PolyLL_BaseTempl<double>::Flags::INTERPRET_POLYLINE);
	if (SUCCEEDED(r))
		return index;
	return (ULONG)-1;
}


bool Project::StaticVector::ClearPolyLine(ULONG index)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	return SUCCEEDED(vf->ClearPolyLine(index));
}


ULONG Project::StaticVector::GetPolyLineMaxSize()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolyLineSize((ULONG)-1, &num)))
		return num;
	return 0;
}


ULONG Project::StaticVector::GetPolyLineTotalSize()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolyLineSize((ULONG)-2, &num)))
		return num;
	return 0;
}


ULONG Project::StaticVector::GetPolyLineCount()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolyLineCount(&num)))
		return num;
	return 0;
}


ULONG Project::StaticVector::GetPolyLineSize(ULONG index)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolyLineSize(index, &num)))
		return num;
	return 0;
}


bool Project::StaticVector::IsPolygon(ULONG index)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	std::uint16_t type;
	if (SUCCEEDED(vf->GetPolyType(index, &type)))
		return (type == XY_PolyLL_BaseTempl<double>::Flags::INTERPRET_POLYGON) ? true : false;
	return false;
}


XY_Point *Project::StaticVector::GetPolyLine(ULONG index, XY_Point *points, std::uint32_t *size, bool *is_polygon)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	XY_Point *p;
	if (!points)
	{
		if (!(*size))
			if (FAILED(vf->GetPolyLineSize(index, size)))
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

	USHORT type;

	XY_Poly pc(p, *size);
	HRESULT r = vf->GetPolyLine(index, size, &pc, &type);

	for (ULONG i = 0; i < (*size); i++)
		p[i] = pc.GetPoint(i);

	*is_polygon = (type == XY_PolyLL_BaseTempl<double>::Flags::INTERPRET_POLYGON);
	if (SUCCEEDED(r))
		return p;				// success so return either points (if provided) or the memory we had to allocate
	if (p != points)
		delete [] p;				// failure so conditional memory clean-up
	return nullptr;
}


bool Project::StaticVector::ClearAllPolyLines()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(m_filter);
	return SUCCEEDED(vf->ClearPolyLine((ULONG)-1));
}


Project::VectorCollection::~VectorCollection()
{
	Vector *v;

	while (v = (Vector *)m_vectorList.RemHead())
		delete v;
}


bool Project::VectorCollection::AssignNewGrid(ICWFGM_GridEngine *grid)
{
	m_gridEngine = grid;

	ICWFGM_CommonData* data;
	if (FAILED(grid->GetCommonData(nullptr, &data)))
		return false;

	Vector *gf = FirstVector();
	while (gf->LN_Succ()) {
		gf->gridEngine(grid);
		gf->m_filter->put_CommonData(data);
		gf = gf->LN_Succ();
	}
	return true;
}


void Project::VectorCollection::AdjustVectorWidths()
{
	double resolution = 5.0;

	Project::Vector *gf = FirstVector();
	while (gf->LN_Succ())
	{
		if (gf->m_filter)
		{
			auto vf = boost::dynamic_pointer_cast<CCWFGM_VectorFilter>(gf->m_filter);
			vf->put_Width(resolution);
		}
		gf = gf->LN_Succ();
	}
}


void Project::VectorCollection::PostSerialize(CWFGMProject *project)
{
	Vector *gf = FirstVector();
	while (gf->LN_Succ())
	{
		gf->PostSerialize(project);
		gf = gf->LN_Succ();
	}
}
