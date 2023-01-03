/**
 * WISE_Project: WindDirectionGrid.cpp
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

#include "WindDirectionGrid.h"
#include "str_printf.h"

#include "filesystem.hpp"



Project::CWindDirectionGrid::CWindDirectionGrid(int sectors)
{
	m_filter = new CCWFGM_WindDirectionGrid();
	PolymorphicUserData v = this;
	m_filter->put_UserData(v);

	InitSectors(sectors);
}

std::string Project::CWindDirectionGrid::GetRangeString(USHORT sector, USHORT range) const
{
	USHORT cnt;
	WxWSEntry* entry = GetRanges(sector, &cnt);
	weak_assert(range < cnt);
	std::string string;
	if (entry)
	{
		std::string filename = fs::path(GetFilename(sector, range)).filename().string();
		string = strprintf(_T("[%s] %s (%.1lf km/h)"),
			directions_8l[sector],
			filename.c_str(),
			entry[range].getWindSpeed()
		);
		delete [] entry;
	}
	else
	{
		weak_assert(false);;
	}
	return string;
}

void Project::CWindDirectionGrid::InitSectors(int sectors)
{
	switch(sectors)
	{
		case 8: CreateOctants(); break;
		default: weak_assert(false); break;
	}
}

std::string Project::CWindDirectionGrid::DisplayName() const
{
	return _T("Wind Direction: ") + m_name;
}


auto Project::CWindDirectionGrid::Duplicate() const -> CWindDirectionGrid*
{
	return Duplicate([] { return new CWindDirectionGrid(); });
}


auto Project::CWindDirectionGrid::Duplicate(std::function<GridFilter*(void)> constructor) const -> CWindDirectionGrid*
{
	auto gf = dynamic_cast<CWindDirectionGrid*>(CWeatherGridBase::Duplicate(constructor));
	return gf;
}

void Project::CWindDirectionGrid::Clone(boost::intrusive_ptr<ICWFGM_GridEngine> *grid) const
{
	boost::intrusive_ptr<ICWFGM_CommonBase> gf;
	HRESULT hr = m_filter->Clone(&gf);
	if (FAILED(hr))
		throw std::logic_error("Failed");
	*grid = boost::dynamic_pointer_cast<ICWFGM_GridEngine>(gf);

	if (*grid)
	{
		PolymorphicUserData v = (void*)this;
		(*grid)->put_UserData(v);
	}
}


bool Project::CWindDirectionGrid::GetEnableGridDefault(HRESULT *hr)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	PolymorphicAttribute v;
	HRESULT h = gf->GetAttribute(CWFGM_WEATHER_GRID_APPLY_FILE_DEFAULT, &v);
	if (hr)
		*hr = h;
	bool b;
	VariantToBoolean_(v, &b);
	return b;
}


bool Project::CWindDirectionGrid::SetEnableGridDefault(bool enable, HRESULT *hr)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	PolymorphicAttribute v = enable;
	HRESULT h = gf->SetAttribute(CWFGM_WEATHER_GRID_APPLY_FILE_DEFAULT, v);
	if (hr)
		*hr = h;
	return GetEnableGridDefault();
}


bool Project::CWindDirectionGrid::GetEnableGridSectors(HRESULT *hr)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	PolymorphicAttribute v;
	HRESULT h = gf->GetAttribute(CWFGM_WEATHER_GRID_APPLY_FILE_SECTORS, &v);
	if (hr)
		*hr = h;
	bool b;
	VariantToBoolean_(v, &b);
	return b;
}


bool Project::CWindDirectionGrid::SetEnableGridSectors(bool enable, HRESULT *hr)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	PolymorphicAttribute v = enable;
	HRESULT h = gf->SetAttribute(CWFGM_WEATHER_GRID_APPLY_FILE_SECTORS, v);
	if (hr)
		*hr = h;
	return GetEnableGridSectors();
}


HRESULT Project::CWindDirectionGrid::Import(const USHORT sector, const double speed, const std::string &prj_filename, const std::string &name)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	HRESULT hr = gf->Import(sector, speed, prj_filename, name);
	return hr;
}


HRESULT Project::CWindDirectionGrid::Export(const USHORT sector, const double range, const std::string &prj_filename, const std::string &name)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	return gf->Export(sector, range, prj_filename, name);
}


HRESULT Project::CWindDirectionGrid::Remove(const USHORT sector, const double range)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	HRESULT hr = gf->Remove(sector, range);
	return hr;
}


HRESULT Project::CWindDirectionGrid::Remove(const USHORT sector, const USHORT range)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	USHORT cnt;
	HRESULT hr;
	WxWSEntry* entry = GetRanges(sector, &cnt);
	weak_assert(range < cnt);
	if (entry)
	{
		hr = gf->Remove(sector, entry[range].getWindSpeed());
		delete [] entry;
	}
	else
	{
		weak_assert(false);;
		hr = ERROR_SEVERITY_ERROR;
	}
	return hr;
}


HRESULT Project::CWindDirectionGrid::ModifySectorSet(const std::vector<WeatherGridSetModifier> &set_modifiers)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	HRESULT hr = gf->ModifySectorSet(set_modifiers);
	return hr;
}


std::string Project::CWindDirectionGrid::GetFilename(const USHORT sector, const USHORT range) const
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	USHORT count = 0;
	std::string cfile;
	gf->GetCount(sector, &count);
	if (count)
	{
		std::vector<std::string> safe_ig(count);
		gf->GetFilenames(sector, &safe_ig);
		cfile = safe_ig[range];
	}
	return cfile;
}

USHORT Project::CWindDirectionGrid::GetSectorCount() const
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	std::uint16_t count;
	gf->GetSectorCount(&count);
	return count;
}


USHORT Project::CWindDirectionGrid::GetRangeCount(const USHORT sector)
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	std::uint16_t count;
	gf->GetCount(sector, &count);
	return count;
}


auto Project::CWindDirectionGrid::GetRanges(const USHORT sector, USHORT *count) const -> WxWSEntry*
{
	auto gf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	std::uint16_t cnt;
	gf->GetCount(sector, &cnt);
	WxWSEntry *entry;
	if (cnt)
	{
		std::vector<double> safe_ig(cnt);
		std::vector<std::string> safe_file(cnt);
		gf->GetWindSpeeds(sector, &cnt, &safe_ig);
		gf->GetFilenames(sector, &safe_file);
		entry = new WxWSEntry[cnt];
		for (USHORT i = 0; i < cnt; i++)
		{
			std::string filename(safe_file[i]);
			entry[i].setFilenamePath(filename);
			entry[i].setWindSpeed(safe_ig[i]);
			entry[i].setWindDirSelection(sector);
		}
	}
	else
		entry = nullptr;
	*count = cnt;
	return entry;
}


HRESULT Project::CWindDirectionGrid::SetStartTime(const WTime &time)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	PolymorphicAttribute v = time;
	return vf->SetAttribute(CWFGM_WEATHER_OPTION_START_TIME, v);
}


HRESULT Project::CWindDirectionGrid::SetEndTime(const WTime &time)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	PolymorphicAttribute v = time;
	return vf->SetAttribute(CWFGM_WEATHER_OPTION_END_TIME, v);
}


HRESULT Project::CWindDirectionGrid::SetStartTimeOfDay(const WTimeSpan &time)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	PolymorphicAttribute v = time;
	return vf->SetAttribute(CWFGM_WEATHER_OPTION_START_TIMESPAN, v);
}


HRESULT Project::CWindDirectionGrid::SetEndTimeOfDay(const WTimeSpan &time)
{
	auto vf = boost::dynamic_pointer_cast<CCWFGM_WindDirectionGrid>(m_filter);
	PolymorphicAttribute v = time;
	return vf->SetAttribute(CWFGM_WEATHER_OPTION_END_TIMESPAN, v);
}
