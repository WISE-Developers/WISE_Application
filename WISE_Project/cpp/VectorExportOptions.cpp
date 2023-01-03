/**
 * WISE_Project: VectorExportOptions.cpp
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

#include "VectorExportOptions.h"
#if _DLL
#include "cwinapp.h"
#define BOOL_CHECK ? true : false
#else
#include "AfxIniSettings.h"
#define BOOL_CHECK
#endif


Project::VectorExportOptions::VectorExportOptions()
{
	this->Perimeter = false;
	this->Enclave = false;
	this->Boundary = false;
	this->PrometheusVersion = true;
	this->FGMName = true;
	this->ScenarioName = true;
	this->IgnitionName = true;
	this->IgnitionAttributes = false;
	this->TotalPerimeter = true;
	this->ActivePerimeter = true;
	this->DateTime = true;
	this->FireSize = true;
	this->Comment1 = "";
	this->OutputAll = false;
	this->WxValues = false;
	this->FWIValues = false;
	this->IgnitionLocation = false;
	this->MaxBurnDistance = false;
	this->AssetFirstArrivalTime = false;
	this->AssetArrivalCount = false;
	this->IdentifyFinal = false;
	this->SimulationStatus = false;
}


Project::VectorExportOptions::VectorExportOptions(const VectorExportOptions *toCopy)
{
	this->Perimeter = toCopy->Perimeter;
	this->Enclave = toCopy->Enclave;
	this->Boundary = toCopy->Boundary;
	this->PrometheusVersion = toCopy->PrometheusVersion;
	this->FGMName = toCopy->FGMName;
	this->ScenarioName = toCopy->ScenarioName;
	this->IgnitionName = toCopy->IgnitionName;
	this->IgnitionAttributes = toCopy->IgnitionAttributes;
	this->TotalPerimeter = toCopy->TotalPerimeter;
	this->ActivePerimeter = toCopy->ActivePerimeter;
	this->DateTime = toCopy->DateTime;
	this->FireSize = toCopy->FireSize;
	this->Comment1 = toCopy->Comment1;
	this->OutputAll = toCopy->OutputAll;
	this->WxValues = toCopy->WxValues;
	this->FWIValues = toCopy->FWIValues;
	this->IgnitionLocation = toCopy->IgnitionLocation;
	this->MaxBurnDistance = toCopy->MaxBurnDistance;
	this->AssetFirstArrivalTime = toCopy->AssetFirstArrivalTime;
	this->AssetArrivalCount = toCopy->AssetArrivalCount;
	this->IdentifyFinal = toCopy->IdentifyFinal;
	this->SimulationStatus = toCopy->SimulationStatus;
}


Project::VectorExportOptions::VectorExportOptions(const char *group_name)
{
#if _DLL
	CWinAppExt* app = (CWinAppExt*)AfxGetApp();
#else
	auto app = AfxGetApp();
#endif
	Perimeter = app->GetProfileBOOL(group_name, "Active Portions", false) BOOL_CHECK;
	Enclave = app->GetProfileBOOL(group_name, "Retain Interiors", false) BOOL_CHECK;
	Boundary = app->GetProfileBOOL(group_name, "Combine All", false) BOOL_CHECK;
	PrometheusVersion = app->GetProfileBOOL(group_name, "Version", true) BOOL_CHECK;
	FGMName = app->GetProfileBOOL(group_name, "FGM Name", true) BOOL_CHECK;
	ScenarioName = app->GetProfileBOOL(group_name, "Scenario Name", true) BOOL_CHECK;
	IgnitionName = app->GetProfileBOOL(group_name, "Ignition Name", true) BOOL_CHECK;
	IgnitionAttributes = app->GetProfileBOOL(group_name, "Ignition Attributes", false) BOOL_CHECK;
	TotalPerimeter = app->GetProfileBOOL(group_name, "Total Perimeter", true) BOOL_CHECK;
	ActivePerimeter = app->GetProfileBOOL(group_name, "Active Perimeter", true) BOOL_CHECK;
	DateTime = app->GetProfileBOOL(group_name, "Date Time", true) BOOL_CHECK;
	FireSize = app->GetProfileBOOL(group_name, "Fire Size", true) BOOL_CHECK;
	OutputAll = app->GetProfileBOOL(group_name, "Output All", true) BOOL_CHECK;
	WxValues = app->GetProfileBOOL(group_name, "Wx Values", false) BOOL_CHECK;
	FWIValues = app->GetProfileBOOL(group_name, "FWI Values", false) BOOL_CHECK;
	IgnitionLocation = app->GetProfileBOOL(group_name, "Ignition Location", false) BOOL_CHECK;
	MaxBurnDistance = app->GetProfileBOOL(group_name, "Max Burn Distance", false) BOOL_CHECK;
	AssetFirstArrivalTime = app->GetProfileBOOL(group_name, "Arrival Time", false) BOOL_CHECK;
	AssetArrivalCount = app->GetProfileBOOL(group_name, "Arrival Count", false) BOOL_CHECK;
	IdentifyFinal = app->GetProfileBOOL(group_name, "Identify Final Perimeter", false) BOOL_CHECK;
	SimulationStatus = app->GetProfileBOOL(group_name, "Simulation Status", false) BOOL_CHECK;
}


void Project::VectorExportOptions::SaveToIniFile(const char *group_name) const
{
#if _DLL
	CWinAppExt *app = (CWinAppExt *)AfxGetApp();
#else
	auto app = AfxGetApp();
#endif
	app->WriteProfileBOOL(group_name, "Active Portions", Perimeter);
	app->WriteProfileBOOL(group_name, "Retain Interiors", Enclave);
	app->WriteProfileBOOL(group_name, "Combine All", Boundary);
	app->WriteProfileBOOL(group_name, "Version", PrometheusVersion);
	app->WriteProfileBOOL(group_name, "FGM Name", FGMName);
	app->WriteProfileBOOL(group_name, "Scenario Name", ScenarioName);
	app->WriteProfileBOOL(group_name, "Ignition Name", IgnitionName);
	app->WriteProfileBOOL(group_name, "Ignition Attributes", IgnitionAttributes);
	app->WriteProfileBOOL(group_name, "Total Perimeter", TotalPerimeter);
	app->WriteProfileBOOL(group_name, "Active Perimeter", ActivePerimeter);
	app->WriteProfileBOOL(group_name, "Date Time", DateTime);
	app->WriteProfileBOOL(group_name, "Fire Size", FireSize);
	app->WriteProfileBOOL(group_name, "Output All", OutputAll);
	app->WriteProfileBOOL(group_name, "Wx Values", WxValues);
	app->WriteProfileBOOL(group_name, "FWI Values", FWIValues);
	app->WriteProfileBOOL(group_name, "Ignition Location", IgnitionLocation);
	app->WriteProfileBOOL(group_name, "Max Burn Distance", MaxBurnDistance);
	app->WriteProfileBOOL(group_name, "Arrival Time", AssetFirstArrivalTime);
	app->WriteProfileBOOL(group_name, "Arrival Count", AssetArrivalCount);
	app->WriteProfileBOOL(group_name, "Identify Final Perimeter", IdentifyFinal);
	app->WriteProfileBOOL(group_name, "Simulation Status", SimulationStatus);
}
