/**
 * WISE_Project: WxWSEntry.h
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

#pragma once

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
#pragma managed(push, off)
#endif

#include "types.h"
#include "cwfgmp_config.h"
#include "hssconfig/config.h"
#include <cstdint>
#include <string>
#ifdef _MSC_VER
#include <tchar.h>
#endif

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 4)
#endif


namespace Project {
class CWFGMPROJECT_EXPORT WxWSEntry {
private:
	double m_windSpeed;
	std::string m_filename_path;
	bool m_filenameModified;
	std::uint16_t m_windDirSelection;
public:
	WxWSEntry();
	void setWindSpeed(double windSpeed);
	void setFilenamePath(const std::string &filename_path);
	void setWindDirection(double dir);
	void setWindDirSelection(std::uint16_t windDirSelection);
	bool setWindDirName(const std::string &name);
	void setFilenameModified(bool modified);

	double getWindSpeed() const { return m_windSpeed; }
	std::string getFilenamePath() const { return m_filename_path; }
	const TCHAR *getWindDirName() const;
	std::uint16_t getWindDirSelection() const { return m_windDirSelection; }
	bool getFilenameModified() const { return m_filenameModified; }

	bool operator ==(const WxWSEntry &toCompare);
	bool operator !=(const WxWSEntry &toCompare);

public:
	static const TCHAR * getWindDirectionName(std::uint16_t index);
	static std::uint16_t getWindDirIndex(const std::string &name);
	static std::uint16_t getWindDirNameCount() { return 8; }
};
};

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
#pragma managed(pop)
#endif
#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif
