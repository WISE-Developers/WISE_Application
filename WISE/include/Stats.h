/**
 * WISE_Application: Stats.h
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

#include <vector>
#include "FGMOutputs.h"
#include "objectcache_mt.h"
#include "intrusive_ptr.h"


struct OutputFileDetails
{
	std::string outputFile;
	//1 -> summary file, 2 -> vector file, 3 -> grid file, 4 -> csv/stats file
	std::uint8_t outputFileType;
	bool exists{ false };
	std::string coverageName;
};


struct ScenarioStepInfo
{
	class Project::Scenario* scenario;
	std::uint32_t s_index;
	HRESULT retval;
	bool finished;
	std::uint64_t timestamp;
	std::string status_msg;
	HSS_Time::WTimeSpan duration;
	std::uint64_t ticks;
	std::vector<std::tuple<std::string, Service::StatisticType>> statistics;
	std::vector<OutputFileDetails> outputFiles;
};


class SimMessage : public MinNode
{
public:
	ScenarioStepInfo stepInfo;

	DECLARE_OBJECT_CACHE_MT(SimMessage, SimMessage)
};


HRESULT exportStats(UnitConversion& unitConversion, Project::CWFGMProject *project, const ICWFGM_FWI* fwi, const Project::StatsOutputs& stats, Project::Scenario *s, SimMessage *sm);
HRESULT exportAssetStats(UnitConversion& unitConversion, Project::CWFGMProject* project, SPARCS_P *sp, const ICWFGM_FWI* fwi, Project::AssetOutput& stats, Project::Scenario* s, SimMessage* sm, bool write);
