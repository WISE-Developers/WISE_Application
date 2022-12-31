/**
 * WISE_Project: StaticUpgradeData.h
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

#ifndef _STATIC_UPGRADE_DATA_H
#define _STATIC_UPGRADE_DATA_H

#ifndef __INTEL_COMPILER
#pragma managed(push, off)
#endif

#include "cwfgmp_config.h"
#include "comptr.h"

extern const struct CWFGMPROJECT_EXPORT CGUIDUpgrade g_upgradeTable[];

#ifndef __INTEL_COMPILER
#pragma managed(pop)
#endif

#endif
