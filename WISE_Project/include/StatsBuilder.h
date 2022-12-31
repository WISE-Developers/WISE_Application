/**
 * WISE_Project: StatsBuilder.h
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

#include "cwfgmp_config.h"

#include <variant>
#include <string>
#include <vector>


///<summary>
///The format to save the stats data in.
///</summary>
enum class StatsFileFormat
{
	JSON_ROW,
	JSON_COLUMN
};


///<summary>
///Compile the outputs required to build a stats JSON file. Can hold data
///from  multiple weather streams.
///</summary>
class CWFGMPROJECT_EXPORT StatsFileBuilder
{
private:
	std::vector<class StatsBuilder*> stats;
	std::string scenarioName;

public:
	///<summary>
	///Construct a new stats file builder.
	///</summary>
	///<param name="scenarioName">The name of the scenario that the stats are for.</param>
	StatsFileBuilder(const std::string& scenarioName);

	virtual ~StatsFileBuilder();

	///<summary>
	///Add a new stats builder. Each weather stream (or lack of weather stream) should be
	///added to a different builder. This method is for no weather stream.
	///</summary>
	StatsBuilder* NewStatsBuilder();

	///<summary>
	///Add a new stats builder. Each weather stream (or lack of weather stream) should be
	///added to a different builder.
	///</summary>
	///<param name="index">The index of the weather stream being used to generate the stats data.</param>
	StatsBuilder* NewStatsBuilder(std::int32_t index);

	///<summary>
	///Add a new stats builder. Each weather stream (or lack of weather stream) should be
	///added to a different builder.
	///</summary>
	///<param name="name">The name of the weather stream being used to generate the stats data.</param>
	StatsBuilder* NewStatsBuilder(const std::string& name);

	///<summary>
	///Add a new stats builder. Each weather stream (or lack of weather stream) should be
	///added to a different builder.
	///</summary>
	///<param name="latitude">Latitude of the location of the weather stream being used to generate the stats data.</param>
	///<param name="longitude">Longiutde of the location of the weather stream being used to generate the stats data.</param>
	StatsBuilder* NewStatsBuilder(double latitude, double longitude);

	///<summary>
	///Retrieve a StatsBuilder that has already been created.
	///</summary>
	///<param name="index">The index of the stats builder. Builders maintain the order they were created in.</param>
	inline StatsBuilder* GetBuilder(std::size_t index) const noexcept { return stats[index]; }

	///<summary>
	///Add a new column to every stats builder. Must be called before adding any rows.
	///</summary>
	///<param name="fieldName">A constant name that will be used to refer to this column.</param>
	///<param name="desc">A brief, human readable description of the column.</param>
	///<exceptions cref="std::domain_error">Thrown if columns are being added after rows have already been added.</exceptions>
	///<exceptions cref="std::invalid_argument">Thrown if the field name has already been added.</exceptions>
	inline void AddColumn(const std::string& fieldName, const std::string& desc) { AddColumn(fieldName, desc, {}); }

	///<summary>
	///Add a new column to every stats builder. Must be called before adding any rows.
	///</summary>
	///<param name="fieldName">A constant name that will be used to refer to this column.</param>
	///<param name="desc">A brief, human readable description of the column.</param>
	///<param name="units">The units that the column is stored in.</param>
	///<exceptions cref="std::domain_error">Thrown if columns are being added after rows have already been added.</exceptions>
	///<exceptions cref="std::invalid_argument">Thrown if the field name has already been added.</exceptions>
	void AddColumn(const std::string& fieldName, const std::string& desc, const std::string& units);

	///<summary>
	///Save the stats data to a JSON file.
	///</summary>
	///<param name="file">The location of the file to save the stats data to.</param>
	///<param name="format">The format to output the file in.</param>
	///<param name="pretty">Should the output file be pretty printed with newlines and tabs?</param>
	void SaveJSON(const std::string& file, StatsFileFormat format, bool pretty = false);
};


///<summary>
///Compile the outputs required to build a stats JSON file.
///</summary>
class CWFGMPROJECT_EXPORT StatsBuilder
{
	friend class StatsFileBuilder;
public:
	typedef std::variant<std::string, double, std::int64_t, bool, std::monostate> StatsFlatType;
	typedef std::vector<std::tuple<std::string, StatsFlatType>> StatsObject;
	typedef std::variant<std::string, double, std::int64_t, bool, std::monostate, StatsObject> StatsObjectType;
	typedef std::vector<StatsObjectType> StatsArray;
	typedef std::variant<std::string, double, std::int64_t, bool, std::monostate, StatsArray, StatsObject> StatsType;

private:
	std::size_t rowCount{ 0 };
	std::vector<std::tuple<std::string, std::string, std::string>> columns;
	std::vector<std::vector<StatsBuilder::StatsType>> rows;
	std::vector<std::tuple<std::string, StatsType>> descriptions;
	std::string scenarioName;
	std::string streamName;
	std::int32_t streamIndex{ -1 };
	double latitude{ 0 }, longitude{ 0 };
	bool hasLocation{ false };

public:
	///<summary>
	///Construct a new stats file builder.
	///</summary>
	///<param name="scenarioName">The name of the scenario that the stats are for.</param>
	StatsBuilder(const std::string& scenarioName);

	///<summary>
	///Retrieve the number of columns in the stats file.
	///</summary>
	inline std::size_t ColumnCount() const noexcept { return columns.size(); }

	///<summary>
	///Retrieve the number of rows in the stats file.
	///</summary>
	inline std::size_t RowCount() const noexcept { return rowCount; }

	///<summary>
	///Add a new column to the stats file. Must be called before adding any rows.
	///</summary>
	///<param name="fieldName">A constant name that will be used to refer to this column.</param>
	///<param name="desc">A brief, human readable description of the column.</param>
	///<exceptions cref="std::domain_error">Thrown if columns are being added after rows have already been added.</exceptions>
	///<exceptions cref="std::invalid_argument">Thrown if the field name has already been added.</exceptions>
	inline void AddColumn(const std::string& fieldName, const std::string& desc) { AddColumn(fieldName, desc, {}); }

	///<summary>
	///Add a new column to the stats file. Must be called before adding any rows.
	///</summary>
	///<param name="fieldName">A constant name that will be used to refer to this column.</param>
	///<param name="desc">A brief, human readable description of the column.</param>
	///<param name="units">The units that the column is stored in.</param>
	///<exceptions cref="std::domain_error">Thrown if columns are being added after rows have already been added.</exceptions>
	///<exceptions cref="std::invalid_argument">Thrown if the field name has already been added.</exceptions>
	void AddColumn(const std::string& fieldName, const std::string& desc, const std::string& units);

	///<summary>
	///Add a row to the stats file.
	///</summary>
	///<param name="cols">The data to add to the row. The number of values must match the number of
	///columns in the stats file. The order of the parameters must also match.</param>
	///<exceptions cref="std::domain_error">Thrown if the number of values doesn't match the number of columns.</exceptions>
	void AddRow(const std::initializer_list<StatsType> &cols);

	///<summary>
	///Add a row to the stats file.
	///</summary>
	///<param name="cols">The data to add to the row. The number of values must match the number of
	///columns in the stats file. The order of the parameters must also match.</param>
	///<exceptions cref="std::domain_error">Thrown if the number of values doesn't match the number of columns.</exceptions>
	void AddRow(const std::vector<StatsType> &cols);

	///<summary>
	///Add a descriptive item to the stats file. Descriptions will be inserted at the root level and will
	///not have entries in the headers object.
	///</summary>
	///<param name="name">The name of the desciption item.</param>
	///<param name="value">The value to display for the item.</param>
	///<exceptions cref="std::invalid_argument">Thrown if the name already exists.</exceptions>
	void AddDescription(const std::string& name, const StatsType& value);

private:
	///<summary>
	///Set the index of the weather stream being used to generate the stats data.
	///</summary>
	inline void SetStream(std::int32_t index) { streamName.clear(); hasLocation = false; streamIndex = index; }

	///<summary>
	///Set the name of the weather stream being used to generate the stats data.
	///</summary>
	inline void SetStream(const std::string& name) { streamIndex = -1; hasLocation = false; streamName = name; }

	///<summary>
	///Set the location of the weather stream being used to generate the stats data.
	///</summary>
	inline void SetStream(double _latitude, double _longitude) { hasLocation = true;  this->latitude = _latitude; this->longitude = _longitude; streamName.clear(); streamIndex = -1; }

	const struct StatsBuilderEntry* BuildFile(StatsFileFormat format);
};
