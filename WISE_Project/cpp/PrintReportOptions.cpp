/**
 * WISE_Project: PrintReportOptions.cpp
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

#include "PrintReportOptions.h"

Project::PrintReportOptions::PrintReportOptions() noexcept
{
	this->ApplicationSettings = 1;
	this->ScenarioSettings = 1;
	this->LandscapeProperties = 1;
	this->InputFiles = 1;
	this->FBPFuelPatches = 1;
	this->WxPatchesAndWxGrids = 1;
	this->GeoData = 1;
	this->AssetData = 0;
	this->Ignitions = 1;
	this->WxStreams = 1;
	this->ScenarioComments = 1;
	this->FBPFuelTypes = 1;
}


void Project::PrintReportOptions::setApplicationSettings(int _ApplicationSettings) noexcept { this->ApplicationSettings = _ApplicationSettings; }
void Project::PrintReportOptions::setScenarioSettings(int _ScenarioSettings) noexcept { this->ScenarioSettings = _ScenarioSettings; }
void Project::PrintReportOptions::setInputFiles(int _InputFiles) noexcept { this->InputFiles = _InputFiles; }
void Project::PrintReportOptions::setLandscapeProperties(int _LandscapeProperties) noexcept { this->LandscapeProperties = _LandscapeProperties; }
void Project::PrintReportOptions::setScenarioComments(int ProjectComments) noexcept { this->ScenarioComments = ProjectComments; }
void Project::PrintReportOptions::setFBPFuelPatches(int _FBPFuelPatches) noexcept { this->FBPFuelPatches = _FBPFuelPatches; }
void Project::PrintReportOptions::setWxPatchesAndWxGrids(int _WxPatchesAndWxGrids) noexcept { this->WxPatchesAndWxGrids = _WxPatchesAndWxGrids; }
void Project::PrintReportOptions::setGeoData(int _GeoData) noexcept { this->GeoData = _GeoData; }
void Project::PrintReportOptions::setAssetData(int _AssetData) noexcept { this->AssetData = _AssetData; }
void Project::PrintReportOptions::setIgnitions(int _Ignitions) noexcept { this->Ignitions = _Ignitions; }
void Project::PrintReportOptions::setWxStreams(int _WxStreams) noexcept { this->WxStreams = _WxStreams; }
void Project::PrintReportOptions::setFBPFuelTypes(int _FBPFuelTypes) noexcept { this->FBPFuelTypes = _FBPFuelTypes; }
void Project::PrintReportOptions::setPath(const std::string& _Path) { this->Path = _Path; }

int Project::PrintReportOptions::getApplicationSettings() const noexcept { return ApplicationSettings; }
int Project::PrintReportOptions::getScenarioSettings() const noexcept { return ScenarioSettings; }
int Project::PrintReportOptions::getInputFiles() const noexcept { return InputFiles; }
int Project::PrintReportOptions::getLandscapeProperties() const noexcept { return LandscapeProperties; }
int Project::PrintReportOptions::getScenarioComments() const noexcept { return ScenarioComments; }
int Project::PrintReportOptions::getFBPFuelPatches() const noexcept { return FBPFuelPatches; }
int Project::PrintReportOptions::getWxPatchesAndWxGrids() const noexcept { return WxPatchesAndWxGrids; }
int Project::PrintReportOptions::getGeoData() const noexcept { return GeoData; }
int Project::PrintReportOptions::getAssetData() const noexcept { return AssetData; }
int Project::PrintReportOptions::getIgnitions() const noexcept { return Ignitions; }
int Project::PrintReportOptions::getFBPFuelTypes() const noexcept { return FBPFuelTypes; }
int Project::PrintReportOptions::getWxStreams() const noexcept { return WxStreams; }
std::string Project::PrintReportOptions::getPath() const { return Path; }
