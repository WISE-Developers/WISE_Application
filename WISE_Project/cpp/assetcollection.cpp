/**
 * WISE_Project: assetcollection.cpp
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
#include "assetcollection.h"
#include "cpoints.h"
#include "GridCom_ext.h"
#include "FontHelper.h"
#include "CWFGM_Asset.h"


Project::Asset::Asset()
{
	m_filter = boost::make_intrusive<CCWFGM_Asset>();
	m_filter->put_UserData((void*)this);
}


Project::Asset::~Asset()
{
	gridEngine(nullptr);
}


ICWFGM_GridEngine * Project::Asset::gridEngine(ICWFGM_GridEngine *gridEngine)
{
	if (!m_filter)
		return gridEngine;
	if (SUCCEEDED(m_filter->put_GridEngine(gridEngine)))
		return gridEngine;
	return nullptr;
}


auto Project::Asset::Duplicate() const -> Asset*
{
	Asset* gf = new Asset();
	if (gf)
	{
		gf->Name(Name());
		gf->Comments(Comments());
		gf->m_col = m_col;
		gf->m_fillColor = m_fillColor;
		gf->m_LineWidth = m_LineWidth;
		gf->m_symbol = m_symbol;
		gf->m_imported = m_imported;
		Clone(&gf->m_filter);

		if (gf->m_filter)
			gf->m_filter->put_UserData((void*)this);
	}
	return gf;
}


void Project::Asset::Clone(boost::intrusive_ptr<ICWFGM_Asset>* asset) const
{
	boost::intrusive_ptr<ICWFGM_CommonBase> gf;
	m_filter->Clone(&gf);
	*asset = boost::dynamic_pointer_cast<CCWFGM_Asset>(gf);

	if (*asset)
		(*asset)->put_UserData((void*)this);
}


double Project::Asset::GetPolyLineWidth()
{
	double width;
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	if (vf)
		vf->get_Width(&width);
	else
		width = 0;
	return width;
}


void Project::Asset::SetPolyLineWidth(double width)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	if (vf)
		vf->put_Width(width);
}


HRESULT Project::Asset::ImportPolylines(const std::string& file_name, const std::vector<std::string>& pd)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	if (vf)
		return vf->ImportPolylines(file_name, &pd);
	return ERROR_INVALID_STATE;
}


HRESULT Project::Asset::ExportPolylines(const std::string& driver_name, const std::string& export_projection, const std::string& file_name)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	if (vf)
		return vf->ExportPolylines(driver_name, export_projection, file_name);
	return ERROR_INVALID_STATE;
}


std::uint32_t Project::Asset::AddPoly(const XY_Point *points, std::uint32_t num_points, std::uint16_t interpret)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	std::uint32_t num;
	XY_PolyConst pc(points, num_points);
	HRESULT r = vf->AddPolyLine(pc, interpret, &num);
	if (SUCCEEDED(r))
		return num;
	return (std::uint32_t)-1;
}


std::uint32_t Project::Asset::SetPoly(std::uint32_t index, const XY_Point *points, std::uint32_t num_points, std::uint16_t interpret)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	XY_PolyConst pc(points, num_points);
	HRESULT r = vf->SetPolyLine(index, pc, interpret);
	if (SUCCEEDED(r))
		return index;
	return (std::uint32_t)-1;
}


bool Project::Asset::ClearPoly(std::uint32_t index)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	return SUCCEEDED(vf->ClearPolyLine(index));
}


std::uint32_t Project::Asset::GetPolyMaxSize()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolyLineSize((std::uint32_t)-1, &num)))
		return num;
	return 0;
}


std::uint32_t Project::Asset::GetPolyTotalSize()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolyLineSize((std::uint32_t)-2, &num)))
		return num;
	return 0;
}


std::uint32_t Project::Asset::GetPolyCount()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolyLineCount(&num)))
		return num;
	return 0;
}


std::uint32_t Project::Asset::GetPolySize(std::uint32_t index)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetPolyLineSize(index, &num)))
		return num;
	return 0;
}


bool Project::Asset::IsPolygon(std::uint32_t index)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	USHORT type;
	if (SUCCEEDED(vf->GetPolyType(index, &type)))
		return (type == XY_PolyLL_BaseTempl<double>::Flags::INTERPRET_POLYGON) ? true : false;
	return FALSE;
}


XY_Point * Project::Asset::GetPoly(std::uint32_t index, XY_Point *points, std::uint32_t *size, std::uint16_t *interpret)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	XY_Point *p;
	if (!points)
	{
		if (!(*size))
			if (FAILED(vf->GetPolyLineSize(index, size)))
				return NULL;
		try
		{
			p = new XY_Point[*size];
		}
		catch (std::bad_alloc & e)
		{
			return NULL;
		}
	}
	else
		p = points;

	USHORT type;

	XY_Poly pc(p, *size);
	HRESULT r = vf->GetPolyLine(index, size, &pc, &type);

	for (std::uint32_t i = 0; i < (*size); i++)
		p[i] = pc.GetPoint(i);

	*interpret = type;
	if (SUCCEEDED(r))
		return p;				// success so return either points (if provided) or the memory we had to allocate
	if (p != points)
		delete [] p;				// failure so conditional memory clean-up
	return NULL;
}


bool Project::Asset::ClearAllPolys()
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Asset>(m_filter);
	return SUCCEEDED(vf->ClearPolyLine((std::uint32_t)-1));
}

Project::AssetCollection::~AssetCollection()
{
	Asset *v;

	while (v = (Asset *)m_assetList.RemHead())
		delete v;
}


bool Project::AssetCollection::AssignNewGrid(ICWFGM_GridEngine *grid)
{
	Asset *gf = FirstAsset();
	while (gf->LN_Succ()) {
		gf->gridEngine(grid);
		gf = gf->LN_Succ();
	}
	return true;
}
