/**
 * WISE_Project: TimestepSettings.h
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
#include <stdint.h>
#include <variant>

#include <gsl/util>

#include "ISerializeProto.h"


namespace Service
{
	typedef std::variant<double, std::uint32_t, std::uint64_t, std::string> StatisticType;

	enum class Statistic
	{
		BURNED_AREA,
		TOTAL_PERIMETER,
		EXTERIOR_PERIMETER,
		ACTIVE_PERIMETER,
		TOTAL_PERIMETER_CHANGE,
		TOTAL_PERIMETER_GROWTH,
		EXTERIOR_PERIMETER_CHANGE,
		EXTERIOR_PERIMETER_GROWTH,
		ACTIVE_PERIMETER_CHANGE,
		ACTIVE_PERIMETER_GROWTH,
		AREA_CHANGE,
		AREA_GROWTH,
		NUM_VERTICES,
		NUM_ACTIVE_POINTS,
		CUMULATIVE_NUM_ACTIVE_POINTS,
		CUMULATIVE_NUM_POINTS,
		NUM_FRONTS,
		NUM_ACTIVE_FRONTS,
		ROS,
		CFB,
		CFC,
		SFC,
		TFC,
		FI,
		HFI,
		HCFB,
		FLAMELENGTH,
		TIMESTEP_REALTIME,
		TIMESTEP_CUMULATIVE_REALTIME,
		TIMESTEP_CUMULATIVE_BURNING_SECS,
		SCENARIO_CURRENT_TIME,
		SCENARIO_NAME
	};

	class TimestepSettings : public ISerializeProto
	{
	private:
		std::vector<Statistic> outputStatistics;

	public:
		inline std::int32_t outputStatisticsCount()	noexcept	{ return gsl::narrow_cast<std::int32_t>(outputStatistics.size()); }
		inline Statistic outputStatistic(std::int32_t index)	{ return outputStatistics[index]; }
		std::uint16_t discretize = 10;

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		ISerializeProto *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;

		std::optional<bool> isdirty(void) const noexcept override { return std::make_optional(false); }
	};
}
