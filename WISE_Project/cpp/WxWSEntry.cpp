/**
 * WISE_Project: WxWSEntry.cpp
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

#include "WxWSEntry.h"
#include "stdchar.h"
#include <boost/algorithm/string.hpp>

Project::WxWSEntry::WxWSEntry()
{
	m_windSpeed = 0;
	m_filename_path = "";
	m_windDirSelection = 0;
	m_filenameModified = false;
}

void Project::WxWSEntry::setWindSpeed(double windSpeed)
{
	m_windSpeed = windSpeed;
}

void Project::WxWSEntry::setFilenamePath(const std::string &filename_path)
{
	m_filename_path = filename_path;
}

void Project::WxWSEntry::setWindDirSelection(std::uint16_t windDirSelection)
{
	m_windDirSelection = windDirSelection;
}

void Project::WxWSEntry::setFilenameModified(bool modified)
{
	m_filenameModified = modified;
}

void Project::WxWSEntry::setWindDirection(double dir)
{
	if (dir >= 337.5 || dir < 22.5)
		m_windDirSelection = 0;
	else if (dir >= 22.5 && dir < 67.5)
		m_windDirSelection = 1;
	else if (dir >= 67.5 && dir < 112.5)
		m_windDirSelection = 2;
	else if (dir >= 112.5 && dir < 157.5)
		m_windDirSelection = 3;
	else if (dir >= 157.5 && dir < 202.5)
		m_windDirSelection = 4;
	else if (dir >= 202.5 && dir < 247.5)
		m_windDirSelection = 5;
	else if (dir >= 247.5 && dir < 292.5)
		m_windDirSelection = 6;
	else
		m_windDirSelection = 7;
}

const TCHAR* Project::WxWSEntry::getWindDirectionName(std::uint16_t index)
{
	if (index > 7)
		return _T("");
	switch(index)
	{
	case 0:
		return _T("North (337.5" DEGREE_SYMBOL " - 22.4" DEGREE_SYMBOL ")");
	case 1:
		return _T("Northeast (22.5" DEGREE_SYMBOL " - 67.4" DEGREE_SYMBOL ")");
	case 2:
		return _T("East (67.5" DEGREE_SYMBOL " - 112.4" DEGREE_SYMBOL ")");
	case 3:
		return _T("Southeast (112.5" DEGREE_SYMBOL " - 157.4" DEGREE_SYMBOL ")");
	case 4:
		return _T("South (157.5" DEGREE_SYMBOL " - 202.4" DEGREE_SYMBOL ")");
	case 5:
		return _T("Southwest (202.5" DEGREE_SYMBOL " - 247.4" DEGREE_SYMBOL ")");
	case 6:
		return _T("West (247.5" DEGREE_SYMBOL " - 292.4" DEGREE_SYMBOL ")");
	case 7:
		return _T("Northwest (292.5" DEGREE_SYMBOL " - 337.4" DEGREE_SYMBOL ")");
	default:
#ifdef _MSC_VER
		__assume(0);
#else
        __builtin_unreachable();
#endif
	}
}

std::uint16_t Project::WxWSEntry::getWindDirIndex(const std::string &name)
{
	if (boost::iequals(name, WxWSEntry::getWindDirectionName(0))) {
		return 0;
	}
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(1))) {
		return 1;
	}
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(2))) {
		return 2;
	}
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(3))) {
		return 3;
	}
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(4))) {
		return 4;
	}
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(5))) {
		return 5;
	}
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(6))) {
		return 6;
	}
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(7))) {
		return 7;
	}
	else return (std::uint16_t)-1;
	__assume(0);
}

bool Project::WxWSEntry::setWindDirName(const std::string &name)
{
	if (boost::iequals(name, WxWSEntry::getWindDirectionName(0)))
		m_windDirSelection = 0;
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(1)))
		m_windDirSelection = 1;
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(2)))
		m_windDirSelection = 2;
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(3)))
		m_windDirSelection = 3;
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(4)))
		m_windDirSelection = 4;
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(5)))
		m_windDirSelection = 5;
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(6)))
		m_windDirSelection = 6;
	else if (boost::iequals(name, WxWSEntry::getWindDirectionName(7)))
		m_windDirSelection = 7;
	else
		m_windDirSelection = 8;
	return (m_windDirSelection != 8);
}

const TCHAR *Project::WxWSEntry::getWindDirName() const
{
	return WxWSEntry::getWindDirectionName(m_windDirSelection);
}


bool Project::WxWSEntry::operator ==(const WxWSEntry &toCompare)
{
	return (m_windSpeed == toCompare.m_windSpeed) && (m_windDirSelection == toCompare.m_windDirSelection);
}


bool Project::WxWSEntry::operator !=(const WxWSEntry &toCompare)
{
	return (m_windSpeed != toCompare.m_windSpeed) || (m_windDirSelection != toCompare.m_windDirSelection);
}
