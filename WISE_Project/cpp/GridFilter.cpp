/**
 * WISE_Project: GridFilter.cpp
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
#if _DLL
#include "stdafx.h"
#endif
#include "GridFilter.h"
#include "ScenarioCollection.h"


Project::GridFilter::~GridFilter()
{
	if (m_filter)
		m_filter->PutGridEngine(nullptr, nullptr);
}


auto Project::GridFilter::Duplicate() const -> GridFilter*
{
	return Duplicate([] { return new GridFilter(); });
}


auto Project::GridFilter::Duplicate(std::function<GridFilter*(void)> constructor) const -> GridFilter*
{
	auto gf = constructor();
	if (gf)
	{
		gf->m_name = m_name;
		gf->m_comments = m_comments;
		gf->m_filter = NULL;
		Clone(&gf->m_filter);

		if (gf->m_filter)
		{
			PolymorphicUserData v = gf;
			gf->m_filter->put_UserData(v);
		}
	}
	return gf;
}


ICWFGM_GridEngine *Project::GridFilter::gridEngine(const Project::Scenario *s) const
{
	boost::intrusive_ptr<ICWFGM_GridEngine> ge;
	Layer *layerThread;
	if (s)
		layerThread = s->layerThread();
	else
		layerThread = nullptr;
	if (SUCCEEDED(m_filter->GetGridEngine(layerThread, &ge)))
		return ge.get();
	return nullptr;
}
