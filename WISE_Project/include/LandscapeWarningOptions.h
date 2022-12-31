/**
 * WISE_Project: LandscapeWarningOptions.h
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

#ifndef __LANDSCAPEWARNINGOPTIONS_H
#define __LANDSCAPEWARNINGOPTIONS_H

#include "cwfgmp_config.h"
#include "types.h"
#include "hssconfig/config.h"


#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 4)
#endif

namespace Project
{
	// just a packager for some warning settings the user can set
	class CWFGMPROJECT_EXPORT LandscapeWarningOptions 
	{
	private:
		std::uint32_t m_fixLoading;

	public:
		bool	m_warnOnDelete;					// do we warn when a user adds a fire to a scenario, when the fire's start
																								// time is before the scenario's?
	public:
		LandscapeWarningOptions();
					// primitives to load, save settings from the INI file

		LandscapeWarningOptions(const TCHAR *group_name);	// gets what we need to know from the app's INI file

		void SaveToIniFile(const TCHAR *group_name) const;

		void AdjustLoading(std::uint32_t project_version);
	};
};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#endif
