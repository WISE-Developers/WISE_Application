/**
 * WISE_Project: targetcollection.cpp
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
#include "targetcollection.h"
#include "GridCom_ext.h"
#include "CWFGM_Target.h"


Project::Target::Target() {
	m_target = boost::make_intrusive<CCWFGM_Target>();
	m_target->put_UserData((void*)this);
}


Project::Target::~Target() {
	gridEngine(nullptr);
}


ICWFGM_GridEngine * Project::Target::gridEngine(ICWFGM_GridEngine *gridEngine) {
	if (!m_target)
		return gridEngine;
	if (SUCCEEDED(m_target->put_GridEngine(gridEngine)))
		return gridEngine;
	return nullptr;
}


Project::Target * Project::Target::Duplicate() const {
	Target *gf = new Target();
	if (gf) {
		gf->Name(Name());
		gf->Comments(Comments());
		gf->m_col = m_col;
		gf->m_displaySize = m_displaySize;
		gf->m_symbol = m_symbol;
		Clone(&gf->m_target);

		if (gf->m_target)
			gf->m_target->put_UserData((void*)this);
	}
	return gf;
}


void Project::Target::Clone(boost::intrusive_ptr<ICWFGM_Target>* target) const
{
	boost::intrusive_ptr<ICWFGM_CommonBase> gf;
	m_target->Clone(&gf);
	*target = boost::dynamic_pointer_cast<CCWFGM_Target>(gf);

	if (*target)
		(*target)->put_UserData((void*)this);
}


HRESULT Project::Target::ImportPointSet(const std::string& file_name, const std::vector<std::string>& pd) {
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	return vf->ImportPointSet(file_name.c_str(), &pd);
}


HRESULT Project::Target::ExportPointSet(const std::string& driver_name, const std::string& export_projection, const std::string& file_name) {
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	return vf->ExportPointSet(driver_name, export_projection, file_name);
}


ULONG Project::Target::AddPointSet(const XY_Point *points, ULONG num_points) {
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	XY_PolyConst pc(points, num_points);
	std::uint32_t num;
	HRESULT r = vf->AddPointSet(pc, &num);
	if (SUCCEEDED(r))
		return num;
	return (ULONG)-1;
}


ULONG Project::Target::SetPointSet(ULONG index, const XY_Point *points, ULONG num_points) {
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	XY_PolyConst pc(points, num_points);
	HRESULT r = vf->SetPointSet(index, pc);
	if (SUCCEEDED(r))
		return index;
	return (ULONG)-1;
}


BOOL Project::Target::ClearPointSet(ULONG index) {
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	return SUCCEEDED(vf->ClearPointSet(index));
}


ULONG Project::Target::GetTargetMaxSize() {
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetTargetSetCount((ULONG)-1, &num)))
		return num;
	return 0;
}


ULONG Project::Target::GetTargetTotalSize() {
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetTargetSetCount((ULONG)-2, &num)))
		return num;
	return 0;
}


ULONG Project::Target::GetTargetCount() {
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetTargetCount(&num)))
		return num;
	return 0;
}


ULONG Project::Target::GetTargetSize(ULONG index) {
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	std::uint32_t num;
	if (SUCCEEDED(vf->GetTargetSetCount(index, &num)))
		return num;
	return 0;
}


XY_Point * Project::Target::GetTargetSet(ULONG index, XY_Point *points, std::uint32_t *size) {
	XY_Point *p;
	if (!points) {
		if (!(*size))
			if (FAILED(m_target->GetTargetSetCount(index, size)))
				return NULL;
		try
		{
			p = new XY_Point[*size];
		}
		catch (std::bad_alloc& e)
		{
			return NULL;
		}
	} else	p = points;

	XY_Poly pc(p, *size);
	HRESULT r = m_target->GetTargetSet(index, size, &pc);

	for (ULONG i = 0; i < (*size); i++)
		p[i] = pc.GetPoint(i);

	if (SUCCEEDED(r))
		return p;				// success so return either points (if provided) or the memory we had to allocate
	if (p != points)
		delete [] p;				// failure so conditional memory clean-up
	return NULL;
}


BOOL Project::Target::ClearPointSets() {
	auto vf = boost::dynamic_pointer_cast<CCWFGM_Target>(m_target);
	return SUCCEEDED(vf->ClearPointSet((ULONG)-1));
}

Project::TargetCollection::~TargetCollection() {
	Target *v;
	while (v = (Target *)m_targetList.RemHead())
		delete v;
}


BOOL Project::TargetCollection::AssignNewGrid(ICWFGM_GridEngine *grid) {
	Target *gf = FirstTarget();
	while (gf->LN_Succ()) {
		gf->gridEngine(grid);
		gf = gf->LN_Succ();
	}
	return TRUE;
}


ULONG Project::TargetCollection::IndexOf(ICWFGM_Target* target) const {
	Target *tt = FirstTarget();
	ULONG lidx = 0;
	while (tt->LN_Succ()) {
		if (tt->m_target == target)
			return lidx;
		tt = tt->LN_Succ();
		lidx++;
	}
	return (ULONG)-1;
}
