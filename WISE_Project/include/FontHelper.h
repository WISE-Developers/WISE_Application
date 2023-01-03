/**
 * WISE_Project: FontHelper.h
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

#ifndef __FONT_HELPER_H__
#define __FONT_HELPER_H__

#include <cstdint>

#if _DLL
inline LOGFONT PolyEditGetWingdingsFont() { LOGFONT font; return font; }
#else
typedef std::uint32_t LOGFONT;
inline LOGFONT PolyEditGetWingdingsFont() { return 0; }
#endif

#endif //__FONT_HELPER_H__
