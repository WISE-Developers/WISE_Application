/**
 * WISE_Project: VectorExportOptions.h
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

#ifndef __VECTOR_EXPORT_OPTIONS_H__
#define __VECTOR_EXPORT_OPTIONS_H__

#include "cwfgmp_config.h"
#include "hssconfig/config.h"
#include <string>

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 1)
#endif


namespace Project
{
	class CWFGMPROJECT_EXPORT VectorExportOptions
	{
	public:
		std::string Comment1;
		bool Perimeter, Enclave, Boundary;
		bool PrometheusVersion;
		bool FGMName;
		bool ScenarioName;
		bool IgnitionName;
		bool IgnitionAttributes;
		bool TotalPerimeter;
		bool ActivePerimeter;
		bool DateTime;
		bool FireSize;
		bool OutputAll;
		bool WxValues;
		bool FWIValues;
		bool IgnitionLocation;
		bool MaxBurnDistance;
		bool AssetFirstArrivalTime;			// delta time since greater of simulation start time, and ignition start time
		bool AssetArrivalCount;				// how many assets the simulated fire reached on this timestep (different from # of intersections)
		bool IdentifyFinal;					// whether to add a field (boolean) if this is the final perimeter for a simulation or not
		bool SimulationStatus;				// enumerated type for status of the simulation - running, or stopped for a variety of reasons

	public:
		VectorExportOptions();
		VectorExportOptions(const VectorExportOptions *toCopy);
		VectorExportOptions(const char *group_name);
		void SaveToIniFile(const char *group_name) const;
	};
};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#endif //__VECTOR_EXPORT_OPTIONS_H__
