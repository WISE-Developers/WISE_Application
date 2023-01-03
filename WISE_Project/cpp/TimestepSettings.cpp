/**
 * WISE_Project: TimestepSettings.cpp
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

#if _DLL
#include "stdafx.h"
#endif
#include "types.h"
#include "TimestepSettings.h"
#include "results.h"

#include <map>
#include <algorithm>

#define BOOST_BIMAP_DISABLE_SERIALIZATION
#include <boost/bimap.hpp>
#include <boost/assign/list_of.hpp>

#include "cwfgmProject.pb.h"


template<typename L, typename R>
constexpr boost::bimap<L, R> makeBimap(std::initializer_list<typename boost::bimap<L, R>::value_type> list) noexcept
{
	return boost::bimap<L, R>(list.begin(), list.end());
}


template<typename L, typename R>
bool bimapContains(const boost::bimap<L, R>& map, const L& item)
{
	const auto it = map.left.find(item);
	return it != map.left.end();
}


template<typename L, typename R>
bool mapContains(const std::map<L, R>& map, const L& item)
{
	const auto it = map.find(item);
	return it != map.end();
}


const boost::bimap<WISE::ProjectProto::GlobalStatistics, Service::Statistic> timestepNewStatMap =
boost::assign::list_of<boost::bimap<WISE::ProjectProto::GlobalStatistics, Service::Statistic>::relation>
	( WISE::ProjectProto::GlobalStatistics::TOTAL_BURN_AREA, Service::Statistic::BURNED_AREA )
	( WISE::ProjectProto::GlobalStatistics::TOTAL_PERIMETER, Service::Statistic::TOTAL_PERIMETER )
	( WISE::ProjectProto::GlobalStatistics::EXTERIOR_PERIMETER, Service::Statistic::EXTERIOR_PERIMETER )
	( WISE::ProjectProto::GlobalStatistics::ACTIVE_PERIMETER, Service::Statistic::ACTIVE_PERIMETER )
	( WISE::ProjectProto::GlobalStatistics::TOTAL_PERIMETER_CHANGE, Service::Statistic::TOTAL_PERIMETER_CHANGE )
	( WISE::ProjectProto::GlobalStatistics::TOTAL_PERIMETER_GROWTH_RATE, Service::Statistic::TOTAL_PERIMETER_GROWTH )
	( WISE::ProjectProto::GlobalStatistics::EXTERIOR_PERIMETER_CHANGE, Service::Statistic::EXTERIOR_PERIMETER_CHANGE )
	( WISE::ProjectProto::GlobalStatistics::EXTERIOR_PERIMETER_GROWTH_RATE, Service::Statistic::EXTERIOR_PERIMETER_GROWTH )
	( WISE::ProjectProto::GlobalStatistics::ACTIVE_PERIMETER_CHANGE, Service::Statistic::ACTIVE_PERIMETER_CHANGE )
	( WISE::ProjectProto::GlobalStatistics::ACTIVE_PERIMETER_GROWTH_RATE, Service::Statistic::ACTIVE_PERIMETER_GROWTH )
	( WISE::ProjectProto::GlobalStatistics::AREA_CHANGE, Service::Statistic::AREA_CHANGE )
	( WISE::ProjectProto::GlobalStatistics::AREA_GROWTH_RATE, Service::Statistic::AREA_GROWTH )
	( WISE::ProjectProto::GlobalStatistics::NUM_VERTICES, Service::Statistic::NUM_VERTICES )
	( WISE::ProjectProto::GlobalStatistics::NUM_ACTIVE_VERTICES, Service::Statistic::NUM_ACTIVE_POINTS )
	( WISE::ProjectProto::GlobalStatistics::CUMULATIVE_ACTIVE_VERTICES, Service::Statistic::CUMULATIVE_NUM_ACTIVE_POINTS )
	( WISE::ProjectProto::GlobalStatistics::CUMULATIVE_VERTICES, Service::Statistic::CUMULATIVE_NUM_POINTS )
	( WISE::ProjectProto::GlobalStatistics::NUM_FRONTS, Service::Statistic::NUM_FRONTS )
	( WISE::ProjectProto::GlobalStatistics::NUM_ACTIVE_FRONTS, Service::Statistic::NUM_ACTIVE_FRONTS )
	( WISE::ProjectProto::GlobalStatistics::MAX_ROS, Service::Statistic::ROS )
	( WISE::ProjectProto::GlobalStatistics::MAX_CFB, Service::Statistic::CFB )
	( WISE::ProjectProto::GlobalStatistics::MAX_CFC, Service::Statistic::CFC )
	( WISE::ProjectProto::GlobalStatistics::MAX_SFC, Service::Statistic::SFC )
	( WISE::ProjectProto::GlobalStatistics::MAX_TFC, Service::Statistic::TFC )
	( WISE::ProjectProto::GlobalStatistics::MAX_FI, Service::Statistic::FI )
	( WISE::ProjectProto::GlobalStatistics::MAX_FL, Service::Statistic::FLAMELENGTH )
	( WISE::ProjectProto::GlobalStatistics::TICKS, Service::Statistic::TIMESTEP_REALTIME )
	( WISE::ProjectProto::GlobalStatistics::PROCESSING_TIME, Service::Statistic::TIMESTEP_CUMULATIVE_REALTIME )
	( WISE::ProjectProto::GlobalStatistics::GROWTH_TIME, Service::Statistic::TIMESTEP_CUMULATIVE_BURNING_SECS )
	( WISE::ProjectProto::GlobalStatistics::DATE_TIME, Service::Statistic::SCENARIO_CURRENT_TIME )
	( WISE::ProjectProto::GlobalStatistics::SCENARIO_NAME, Service::Statistic::SCENARIO_NAME )
	( WISE::ProjectProto::GlobalStatistics::HFI, Service::Statistic::HFI )
	( WISE::ProjectProto::GlobalStatistics::HCFB, Service::Statistic::HCFB );

const std::map<WISE::ProjectProto::Project_TimestepSettings_Statistics, Service::Statistic> timestepOldStatMap =
{
	{ WISE::ProjectProto::Project_TimestepSettings::BurnArea, Service::Statistic::BURNED_AREA },
	{ WISE::ProjectProto::Project_TimestepSettings::TotalPerimeter, Service::Statistic::TOTAL_PERIMETER },
	{ WISE::ProjectProto::Project_TimestepSettings::ExteriorPerimeter, Service::Statistic::EXTERIOR_PERIMETER },
	{ WISE::ProjectProto::Project_TimestepSettings::ActivePerimeter, Service::Statistic::ACTIVE_PERIMETER },
	{ WISE::ProjectProto::Project_TimestepSettings::TotalPerimeterChange, Service::Statistic::TOTAL_PERIMETER_CHANGE },
	{ WISE::ProjectProto::Project_TimestepSettings::TotalPerimeterGrowth, Service::Statistic::TOTAL_PERIMETER_GROWTH },
	{ WISE::ProjectProto::Project_TimestepSettings::ExteriorPerimeterChange, Service::Statistic::EXTERIOR_PERIMETER_CHANGE },
	{ WISE::ProjectProto::Project_TimestepSettings::ExteriorPerimeterGrowth, Service::Statistic::EXTERIOR_PERIMETER_GROWTH },
	{ WISE::ProjectProto::Project_TimestepSettings::ActivePerimeterChange, Service::Statistic::ACTIVE_PERIMETER_CHANGE },
	{ WISE::ProjectProto::Project_TimestepSettings::ActivePerimeterGrowth, Service::Statistic::ACTIVE_PERIMETER_GROWTH },
	{ WISE::ProjectProto::Project_TimestepSettings::AreaChange, Service::Statistic::AREA_CHANGE },
	{ WISE::ProjectProto::Project_TimestepSettings::AreaGrowth, Service::Statistic::AREA_GROWTH },
	{ WISE::ProjectProto::Project_TimestepSettings::NumberVertices, Service::Statistic::NUM_VERTICES },
	{ WISE::ProjectProto::Project_TimestepSettings::NumberActivePoints, Service::Statistic::NUM_ACTIVE_POINTS },
	{ WISE::ProjectProto::Project_TimestepSettings::CumulativeNumberActivePoints, Service::Statistic::CUMULATIVE_NUM_ACTIVE_POINTS },
	{ WISE::ProjectProto::Project_TimestepSettings::CumulativeNumberPoints, Service::Statistic::CUMULATIVE_NUM_POINTS },
	{ WISE::ProjectProto::Project_TimestepSettings::NumberFronts, Service::Statistic::NUM_FRONTS },
	{ WISE::ProjectProto::Project_TimestepSettings::NumberActiveFronts, Service::Statistic::NUM_ACTIVE_FRONTS },
	{ WISE::ProjectProto::Project_TimestepSettings::ROS, Service::Statistic::ROS },
	{ WISE::ProjectProto::Project_TimestepSettings::CFB, Service::Statistic::CFB },
	{ WISE::ProjectProto::Project_TimestepSettings::CFC, Service::Statistic::CFC },
	{ WISE::ProjectProto::Project_TimestepSettings::SFC, Service::Statistic::SFC },
	{ WISE::ProjectProto::Project_TimestepSettings::TFC, Service::Statistic::TFC },
	{ WISE::ProjectProto::Project_TimestepSettings::FI, Service::Statistic::FI },
	{ WISE::ProjectProto::Project_TimestepSettings::FlameLength, Service::Statistic::FLAMELENGTH },
	{ WISE::ProjectProto::Project_TimestepSettings::TimestepRealtime, Service::Statistic::TIMESTEP_REALTIME },
	{ WISE::ProjectProto::Project_TimestepSettings::TimestepCumulativeRealtime, Service::Statistic::TIMESTEP_CUMULATIVE_REALTIME },
	{ WISE::ProjectProto::Project_TimestepSettings::TimestepCumulativeBurningSeconds, Service::Statistic::TIMESTEP_CUMULATIVE_BURNING_SECS },
	{ WISE::ProjectProto::Project_TimestepSettings::CurrentTime, Service::Statistic::SCENARIO_CURRENT_TIME },
	{ WISE::ProjectProto::Project_TimestepSettings::ScenarioName, Service::Statistic::SCENARIO_NAME },
	{ WISE::ProjectProto::Project_TimestepSettings::HFI, Service::Statistic::HFI },
	{ WISE::ProjectProto::Project_TimestepSettings::HCFB, Service::Statistic::HCFB }
	};


std::int32_t Service::TimestepSettings::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return 1;
}


google::protobuf::Message* Service::TimestepSettings::serialize(const SerializeProtoOptions& options)
{
	const auto message = new WISE::ProjectProto::Project_TimestepSettings();
	for (auto& entry : outputStatistics)
	{
		message->add_messageglobaloutputs(timestepNewStatMap.right.at(entry));
	}

	auto options2 = new ::WISE::ProjectProto::Project_DiscretizedStatsOptions();
	options2->set_version(serialVersionUid(options));
	options2->set_discretize(discretize);
	message->set_allocated_discretizedoptions(options2);

	return message;
}

ISerializeProto *Service::TimestepSettings::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name)
{
	auto message = dynamic_cast<const WISE::ProjectProto::Project_TimestepSettings*>(&proto);

	if (!message)
	{
		if (valid)
			/// <summary>
			/// The object passed as a timestep setting is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.Project.TimestepSettings", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(false);
		throw ISerializeProto::DeserializeError("TimestepSettings: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}

	/// <summary>
	/// Child validations for timestep settings.
	/// </summary>
	auto vt = validation::conditional_make_object(valid, "WISE.ProjectProto.Project.TimestepSettings", name);
	auto myValid = vt.lock();

HSS_PRAGMA_WARNING_PUSH
HSS_PRAGMA_GCC(GCC diagnostic ignored "-Wdeprecated-declarations")
HSS_PRAGMA_SUPPRESS_MSVC_WARNING(4996)
	for (auto& entry : message->messageoutputs())
	{
		const auto jj = static_cast<WISE::ProjectProto::Project_TimestepSettings_Statistics>(entry);
		if (mapContains(timestepOldStatMap, jj))
		{
			auto ii = timestepOldStatMap.at(jj);
			if (std::find(std::begin(outputStatistics), std::end(outputStatistics), ii) == std::end(outputStatistics))
				outputStatistics.push_back(ii);
		}
		else if (myValid)
			/// <summary>
			/// A requested message output is invalid or not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			myValid->add_child_validation("WISE.ProjectProto.Project.TimestepSettings", "messageOutputs", validation::error_level::WARNING, validation::id::object_invalid, std::to_string(jj));
	}
HSS_PRAGMA_WARNING_POP
	for (auto& entry : message->messageglobaloutputs())
	{
		const auto jj = static_cast<WISE::ProjectProto::GlobalStatistics>(entry);
		if (bimapContains(timestepNewStatMap, jj))
		{
			auto ii = timestepNewStatMap.left.at(jj);
			if (std::find(std::begin(outputStatistics), std::end(outputStatistics), ii) == std::end(outputStatistics))
				outputStatistics.push_back(ii);
		}
		else if (myValid)
			/// <summary>
			/// A requested message output is invalid or not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			myValid->add_child_validation("WISE.ProjectProto.Project.TimestepSettings", "messageGlobalOutputs", validation::error_level::WARNING, validation::id::object_invalid, std::to_string(jj));
	}

	if (message->has_discretizedoptions()) {
		auto dopt = message->discretizedoptions();
		if (dopt.version() != 1) {
			if (myValid)
				/// <summary>
				/// The object version is not supported. The grid output is not supported by this version of Prometheus.
				/// </summary>
				/// <type>user</type>
				myValid->add_child_validation("WISE.ProjectProto.Project.Outputs.DiscretizedStatsOptions", "discretizedOptions", validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(dopt.version()));
			weak_assert(false);
			throw std::invalid_argument("DiscretizedStatsOptions: Unknown version");
		}
		discretize = (std::uint16_t)dopt.discretize();
		if ((discretize < 1) || (discretize > 1000))
		{
			//				m_loadWarning = "Warning: WISE.ProjectProto.Project.Outputs.GridOutput: Invalid specified discretization value";
			weak_assert(false);
			if (myValid) {
				/// <summary>
				/// The scenario's override FMC value is invalid.
				/// </summary>
				/// <type>user</type>
				auto vt2 = validation::conditional_make_object(myValid, "WISE.ProjectProto.Project.Outputs.DiscretizedStatsOptions", "discretizedOptions");
				auto dValid = vt2.lock();

				dValid->add_child_validation("uint32", "discretize", validation::error_level::WARNING, validation::id::value_invalid, std::to_string(discretize), { false, 0.0 }, { true, 1000.0 });
			}
			discretize = 1000;
		}
	}


	return this;
}
