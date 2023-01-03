/**
 * WISE_Project: StatsBuilder.cpp
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

#include "StatsBuilder.h"
#include "json.hpp"
#include "filesystem.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <boost/algorithm/string.hpp>


using json = nlohmann::json;
using namespace std::string_literals;


struct StatsBuilderEntry
{
	json compiled;
};


StatsFileBuilder::StatsFileBuilder(const std::string& scenarioName)
	: scenarioName(scenarioName)
{
}


StatsFileBuilder::~StatsFileBuilder()
{
	for (auto stat : stats)
		delete stat;
	stats.clear();
}

StatsBuilder* StatsFileBuilder::NewStatsBuilder()
{
	auto builder = new StatsBuilder(scenarioName);
	stats.push_back(builder);
	return builder;
}


StatsBuilder* StatsFileBuilder::NewStatsBuilder(std::int32_t index)
{
	auto builder = new StatsBuilder(scenarioName);
	builder->SetStream(index);
	stats.push_back(builder);
	return builder;
}


StatsBuilder* StatsFileBuilder::NewStatsBuilder(const std::string& name)
{
	auto builder = new StatsBuilder(scenarioName);
	builder->SetStream(name);
	stats.push_back(builder);
	return builder;
}


StatsBuilder* StatsFileBuilder::NewStatsBuilder(double latitude, double longitude)
{
	auto builder = new StatsBuilder(scenarioName);
	builder->SetStream(latitude, longitude);
	stats.push_back(builder);
	return builder;
}


void StatsFileBuilder::AddColumn(const std::string& fieldName, const std::string& desc, const std::string& units)
{
	for (auto stat : stats)
		stat->AddColumn(fieldName, desc, units);
}


void StatsFileBuilder::SaveJSON(const std::string& file, StatsFileFormat format, bool pretty)
{
	//ensure that the path to the file exists
	fs::path path(file);
	path = fs::absolute(path).parent_path();
	fs::create_directories(path);

	//open the file first so if it fails we don't have to do any of the other work
	std::ofstream writeFile;
	writeFile.open(file);

	std::vector<const StatsBuilderEntry*> values;
	json arr = json::array();

	//build each stat entry
	for (auto stat : stats)
	{
		auto value = stat->BuildFile(format);
		values.push_back(value);
		arr.push_back(value->compiled);
	}

	writeFile << arr.dump(pretty ? 4 : -1, ' ', false, json::error_handler_t::ignore);

	//delete the returned values
	for (auto value : values)
		delete value;

	writeFile.close();
}


StatsBuilder::StatsBuilder(const std::string& scenarioName)
	: scenarioName(scenarioName)
{
}


void StatsBuilder::AddColumn(const std::string& fieldName, const std::string& desc, const std::string& units)
{
	//make sure no rows have been added
	if (rowCount > 0)
		throw std::domain_error("Columns must only be added before rows have been added.");
	//make sure this is a unique field
	if (std::find_if(columns.begin(), columns.end(),
		[&fieldName](const std::tuple<std::string, std::string, std::string>& s) { return boost::iequals(fieldName, std::get<0>(s)); }) != columns.end())
		throw std::invalid_argument("This column already exists");

	columns.emplace_back(std::make_tuple(fieldName, desc, units));
}


void StatsBuilder::AddRow(const std::initializer_list<StatsType> &cols)
{
	if (cols.size() != columns.size())
		throw std::domain_error("Incorrect number of columns.");

	rows.push_back(cols);

	rowCount++;
}


void StatsBuilder::AddRow(const std::vector<StatsType> &cols)
{
	if (cols.size() != columns.size())
		throw std::domain_error("Incorrect number of columns.");

	rows.push_back(cols);

	rowCount++;
}


void StatsBuilder::AddDescription(const std::string& name, const StatsType& value)
{
	//make sure this is a unique field
	if (std::find_if(descriptions.begin(), descriptions.end(),
		[&name](const std::tuple<std::string, StatsType>& s) { return boost::iequals(name, std::get<0>(s)); }) != descriptions.end() ||
		boost::iequals(name, "headers"s) || boost::iequals(name, "weather_source"s) || boost::iequals(name, "version"s) ||
		boost::iequals(name, "scenario_name"s) || boost::iequals(name, "stats"s))
	{
		throw std::invalid_argument("This column already exists");
	}
	descriptions.emplace_back(std::make_tuple(name, value));
}


void setValue(json::value_type& js, const StatsBuilder::StatsFlatType& value)
{
	if (std::holds_alternative<std::string>(value))
		js = std::get<std::string>(value);
	else if (std::holds_alternative<double>(value))
		js = std::get<double>(value);
	else if (std::holds_alternative<std::int64_t>(value))
		js = std::get<std::int64_t>(value);
	else if (std::holds_alternative<bool>(value))
		js = std::get<bool>(value);
	else if (std::holds_alternative<std::monostate>(value))
		js = nullptr;
}


void setValue(json::value_type& js, const StatsBuilder::StatsObjectType& value)
{
	if (std::holds_alternative<std::string>(value))
		js = std::get<std::string>(value);
	else if (std::holds_alternative<double>(value))
		js = std::get<double>(value);
	else if (std::holds_alternative<std::int64_t>(value))
		js = std::get<std::int64_t>(value);
	else if (std::holds_alternative<bool>(value))
		js = std::get<bool>(value);
	else if (std::holds_alternative<std::monostate>(value))
		js = nullptr;
	else if (std::holds_alternative<StatsBuilder::StatsObject>(value))
	{
		json obj = json::object();
		for (auto& val : std::get<StatsBuilder::StatsObject>(value))
		{
			setValue(obj[std::get<0>(val)], std::get<1>(val));
		}
		js = obj;
	}
}


void setValue(json::value_type &js, const StatsBuilder::StatsType &value)
{
	if (std::holds_alternative<std::string>(value))
		js = std::get<std::string>(value);
	else if (std::holds_alternative<double>(value))
		js = std::get<double>(value);
	else if (std::holds_alternative<std::int64_t>(value))
		js = std::get<std::int64_t>(value);
	else if (std::holds_alternative<bool>(value))
		js = std::get<bool>(value);
	else if (std::holds_alternative<std::monostate>(value))
		js = nullptr;
	else if (std::holds_alternative<StatsBuilder::StatsArray>(value))
	{
		json arr = json::array();
		int index = 0;
		for (auto& val : std::get<StatsBuilder::StatsArray>(value))
		{
			setValue(arr[index], val);
			index++;
		}
		js = arr;
	}
	else if (std::holds_alternative<StatsBuilder::StatsObject>(value))
	{
		json obj = json::object();
		for (auto& val : std::get<StatsBuilder::StatsObject>(value))
		{
			setValue(obj[std::get<0>(val)], std::get<1>(val));
		}
		js = obj;
	}
}


const StatsBuilderEntry* StatsBuilder::BuildFile(StatsFileFormat format)
{
	StatsBuilderEntry* retval = new StatsBuilderEntry();

	//default fields that will always be present
	retval->compiled = {
		{ "version", 1 },
		{ "scenario_name", scenarioName }
	};

	//if a weather stream is specified
	if (streamIndex >= 0)
		retval->compiled["weather_source"]["stream_index"] = streamIndex;
	else if (streamName.size())
		retval->compiled["weather_source"]["stream_name"] = streamName;
	else if (hasLocation)
	{
		retval->compiled["weather_source"]["stream_location"]["latitude"] = latitude;
		retval->compiled["weather_source"]["stream_location"]["longitude"] = longitude;
	}

	for (auto& description : descriptions)
	{
		setValue(retval->compiled[std::get<0>(description)], std::get<1>(description));
	}

	//output the header definitions
	retval->compiled["headers"] = json::array();
	for (auto& header : columns)
	{
		if (std::get<2>(header).size())
			retval->compiled["headers"].push_back({
					{ "field_name", std::get<0>(header) },
					{ "name", std::get<1>(header) },
					{ "units", std::get<2>(header) }
				});
		else
			retval->compiled["headers"].push_back({
					{ "field_name", std::get<0>(header) },
					{ "name", std::get<1>(header) }
				});
	}

	//write the stats data
	if (format == StatsFileFormat::JSON_ROW)
	{
		retval->compiled["stats"] = json::array();
		for (std::size_t i = 0; i < rowCount; i++)
		{
			json obj = json::object();

			for (std::size_t j = 0; j < columns.size(); j++)
			{
				auto& value = rows[i][j];
				setValue(obj[std::get<0>(columns[j])], value);
			}

			retval->compiled["stats"].push_back(obj);
		}
	}
	else
	{
		for (std::size_t j = 0; j < columns.size(); j++)
		{
			const std::string& name = std::get<0>(columns[j]);
			retval->compiled["stats"][name] = json::array();
			
			for (std::size_t i = 0; i < rowCount; i++)
			{
				auto& value = rows[i][j];
				setValue(retval->compiled["stats"][name][i], value);
			}
		}
	}

	return retval;
}
