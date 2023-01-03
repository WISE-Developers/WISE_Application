/**
 * WISE_Project: GeoJSONHelper.cpp
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

#define _SILENCE_CXX20_IS_POD_DEPRECATION_WARNING

#if _DLL
#include "stdafx.h"
#endif
#include "GeoJSONHelper.h"
#include "poly.h"
#include "lines.h"


namespace HSS_MATH_HELPER
{
	using json = nlohmann::json;
	using namespace std::string_literals;

	template<class cls>
    typename std::enable_if<std::is_base_of<XY_Point, cls>::value, HSS_MATH_HELPER::json_type>::type
        convertToGeoJSON(const cls& hssObject, Project::CWFGMProject *project)
	{
		json retval;
		retval["type"s] = "Feature"s;
		retval["geometry"s]["type"s] = "Point"s;
		json coords = json::array();
		double lat, lon;
		project->GetLLByCoord(hssObject.x, hssObject.y, &lat, &lon);
		coords.push_back(lon);
		coords.push_back(lat);
		retval["geometry"s]["coordinates"s] = coords;

		return retval;
	}

	template<class cls>
    typename std::enable_if<std::is_base_of<XY_PolyConst, cls>::value, HSS_MATH_HELPER::json_type>::type
        convertToGeoJSON(const cls& hssObject, Project::CWFGMProject *project)
	{
		json retval;
		retval["type"s] = "Feature"s;
		retval["geometry"s]["type"s] = "Polygon"s;
		json coords = json::array();
		for (std::uint32_t i = 0; i < hssObject.NumPoints(); i++)
		{
			json coord = json::array();
			auto point = hssObject.GetPoint(i);
			double lat, lon;
			project->GetLLByCoord(point.x, point.y, &lat, &lon);
			coord.push_back(lon);
			coord.push_back(lat);
			coords.push_back(coord);
		}
		{
			json coord = json::array();
			auto point = hssObject.GetPoint(0);
			double lat, lon;
			project->GetLLByCoord(point.x, point.y, &lat, &lon);
			coord.push_back(lon);
			coord.push_back(lat);
			coords.push_back(coord);
		}
		retval["geometry"s]["coordinates"s] = json::array();
		retval["geometry"s]["coordinates"s].push_back(coords);

		return retval;
	}

	template<class cls>
    typename std::enable_if<std::is_base_of<XY_Line, cls>::value, HSS_MATH_HELPER::json_type>::type
        convertToGeoJSON(const cls& hssObject, Project::CWFGMProject *project)
	{
		json retval;
		retval["type"s] = "Feature"s;
		retval["geometry"s]["type"s] = "LineString"s;
		json coords = json::array();
		json coord1 = json::array();
		double lat, lon;
		project->GetLLByCoord(hssObject.p1.x, hssObject.p1.y, &lat, &lon);
		coord1.push_back(lon);
		coord1.push_back(lat);
		coords.push_back(coord1);
		json coord2 = json::array();
		project->GetLLByCoord(hssObject.p2.x, hssObject.p2.y, &lat, &lon);
		coord2.push_back(lon);
		coord2.push_back(lat);
		coords.push_back(coord2);
		retval["geometry"s]["coordinates"s] = coords;

		return retval;
	}

	auto convertToGeoJSON(XY_Point* hssObject, std::size_t count, FeatureType type, Project::CWFGMProject* project)->json_type
	{
		json retval;
		retval["type"s] = "Feature"s;
		json coords = json::array();

		if (type == FeatureType::LINE_STRING)
		{
			retval["geometry"s]["type"s] = "LineString"s;
			for (std::size_t i = 0; i < count; i++)
			{
				auto& pt = hssObject[i];
				json coords2 = json::array();
				double lat, lon;
				project->GetLLByCoord(pt.x, pt.y, &lat, &lon);
				coords2.push_back(lon);
				coords2.push_back(lat);
				coords.push_back(coords2);
			}
		}
		else if (type == FeatureType::POLYGON_IN || type == FeatureType::POLYGON_OUT)
		{
			retval["geometry"s]["type"s] = "Polygon"s;
			json coords2 = json::array();
			for (std::size_t i = 0; i < count; i++)
			{
				json coord = json::array();
				auto& point = hssObject[i];
				double lat, lon;
				project->GetLLByCoord(point.x, point.y, &lat, &lon);
				coord.push_back(lon);
				coord.push_back(lat);
				coords2.push_back(coord);
			}
			{
				json coord = json::array();
				auto& point = hssObject[0];
				double lat, lon;
				project->GetLLByCoord(point.x, point.y, &lat, &lon);
				coord.push_back(lon);
				coord.push_back(lat);
				coords2.push_back(coord);
			}
			coords.push_back(coords2);
		}
		else
		{
			if (count > 1)
			{
				retval["geometry"s]["type"s] = "MultiPoint"s;
				for (std::size_t i = 0; i < count; i++)
				{
					auto& pt = hssObject[i];
					json coords2 = json::array();
					double lat, lon;
					project->GetLLByCoord(pt.x, pt.y, &lat, &lon);
					coords2.push_back(lon);
					coords2.push_back(lat);
					coords.push_back(coords2);
				}
			}
			else
			{
				retval["geometry"s]["type"s] = "Point"s;
				double lat, lon;
				project->GetLLByCoord(hssObject->x, hssObject->y, &lat, &lon);
				coords.push_back(lon);
				coords.push_back(lat);
			}
		}

		retval["geometry"s]["coordinates"s] = coords;

		return retval;
	}
}
