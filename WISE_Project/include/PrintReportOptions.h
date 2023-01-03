/**
 * WISE_Project: PrintReportOptions.h
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

#if _DLL
#include "stdafx.h"
#endif
#include "cwfgmp_config.h"
#include <string>


namespace Project
{
	class CWFGMPROJECT_EXPORT PrintReportOptions
	{
	private:
		int ApplicationSettings;
		int ScenarioSettings;
		int LandscapeProperties;
		int InputFiles;
		int FBPFuelPatches;
		int WxPatchesAndWxGrids;
		int GeoData;
		int AssetData;
		int Ignitions;
		int WxStreams;
		int FBPFuelTypes;
		int ScenarioComments;
		std::string Path;

	public:
		PrintReportOptions() noexcept;
		void setApplicationSettings(int _ApplicationSettings) noexcept;
		void setScenarioSettings(int _ScenarioSettings) noexcept;
		void setInputFiles(int _InputFiles) noexcept;
		void setLandscapeProperties(int _LandscapeProperties) noexcept;
		void setScenarioComments(int _ScenarioComments) noexcept;
		void setFBPFuelPatches(int _FBPFuelPatches) noexcept;
		void setWxPatchesAndWxGrids(int _WxPatchesAndWxGrids) noexcept;
		void setGeoData(int _GeoData) noexcept;
		void setAssetData(int _AssetData) noexcept;
		void setIgnitions(int _Ignitions) noexcept;
		void setWxStreams(int _WxStreams) noexcept;
		void setFBPFuelTypes(int _FBPFuelTypes) noexcept;
		void setPath(const std::string &_Path);

		int getApplicationSettings() const noexcept;
		int getScenarioSettings() const noexcept;
		int getInputFiles() const noexcept;
		int getLandscapeProperties() const noexcept;
		int getScenarioComments() const noexcept;
		int getFBPFuelPatches() const noexcept;
		int getWxPatchesAndWxGrids() const noexcept;
		int getGeoData() const noexcept;
		int getAssetData() const noexcept;
		int getIgnitions() const noexcept;
		int getWxStreams() const noexcept;
		int getFBPFuelTypes() const noexcept;
		std::string getPath() const;
	};
};
