/**
 * WISE_Project: GeoJSONHelper.h
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

#include "CWFGMProject.h"
#include "json.hpp"
#include "points.h"


namespace HSS_MATH_HELPER
{
	typedef nlohmann::json json_type;

	enum class FeatureType
	{
		POINT = 100,
		LINE_STRING = 200,
		POLYGON_IN = 300,
		POLYGON_OUT = 400
	};

	template<class cls>
	json_type convertToGeoJSON(const cls& hssObject, Project::CWFGMProject *project);

	json_type convertToGeoJSON(XY_Point* hssObject, std::size_t count, FeatureType type, Project::CWFGMProject* project);
}
