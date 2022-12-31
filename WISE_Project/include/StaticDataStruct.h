/**
 * WISE_Project: StaticDataStruct.h
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

#ifndef _STATIC_FUEL_DATA_STRUCT_H
#define _STATIC_FUEL_DATA_STRUCT_H

#include "cwfgmp_config.h"
#include "types.h"
#include "hssconfig/config.h"


#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 2)
#endif

namespace Project {
struct StaticEquationData {
	std::uint16_t equationKey, equation;
};


struct StaticAttributeData {
	std::uint16_t mode;
	std::uint16_t attributeKey;
	double attribute;
};


struct StaticFuelData {
	const TCHAR *szMainAgencyName;
	const TCHAR *szFBPName;
	const TCHAR *szAlias[8];
	const GUID *clsid;
	long import_val, export_val;
	COLORREF color;
	const StaticEquationData *equation;
	const StaticAttributeData *attribute;
};
};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#endif
