/**
 * WISE_Project: ElevStruct.h
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

#ifndef ELEV_STRUCT_H
#define ELEV_STRUCT_H
#include "cwfgmp_config.h"

namespace Project {
struct ELEV_STRUCT
{
	int MinValue;	// This is the lowlimit of the category.
	int MaxValue;	// This is the top limit of the category.
	COLORREF color;
	ELEV_STRUCT() { MinValue = 0; MaxValue = 10000; color = 0; }
};
};

#endif
