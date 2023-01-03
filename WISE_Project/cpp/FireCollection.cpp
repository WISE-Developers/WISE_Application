/**
 * WISE_Project: FireCollection.cpp
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

#include "types.h"
#include "FireCollection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


Project::Fire::Fire(const FireCollection &fireCollection)
	: StdFire(fireCollection)
{
}


Project::Fire::Fire(const FireCollection &fireCollection, const Fire &fire)
	: StdFire(fireCollection, fire)
{
	m_color = fire.m_color;
	m_fillColor = m_color;
	m_lineWidth = fire.m_lineWidth;
	m_imported = fire.m_imported;
	m_symbol = fire.m_symbol;
}


Project::Fire::Fire(const FireCollection &fireCollection, boost::intrusive_ptr<CCWFGM_Ignition> &fire)
	: StdFire(fireCollection, fire)
{
}


Project::Fire::~Fire() { }
