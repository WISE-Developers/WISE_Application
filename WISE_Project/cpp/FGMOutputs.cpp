/**
 * WISE_Project: FGMOutputs.cpp
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
#include "FGMOutputs.h"

#include <boost/algorithm/string.hpp>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "SummaryCollection.h"


Project::VectorOutput::VectorOutput()
	: startTime(0, nullptr),
	endTime(0, nullptr), 
	multiplePerimeters(false),
	removeIslands(false),
	mergeContacting(false),
	activePerimeters(false)
{
	m_mutex = new std::mutex();

	exported = 0;
	shouldStream = false;
}


// std::vector<>.push_back uses the copy constructor for the new element
Project::VectorOutput::VectorOutput(const VectorOutput &toCopy)
	: startTime(toCopy.startTime),
	endTime(toCopy.endTime)
{
	m_mutex = new std::mutex();

	scenarioName = toCopy.scenarioName;
	subScenarioNames = toCopy.subScenarioNames;
	filename = toCopy.filename;
	coverageName = toCopy.coverageName;
	multiplePerimeters = toCopy.multiplePerimeters;
	removeIslands = toCopy.removeIslands;
	mergeContacting = toCopy.mergeContacting;
	activePerimeters = toCopy.activePerimeters;
	applicationVersion = toCopy.applicationVersion;
	outputScenarioName = toCopy.outputScenarioName;
	jobName = toCopy.jobName;
	ignitionName = toCopy.ignitionName;
	ignitionLocation = toCopy.ignitionLocation;
	ignitionAttributes = toCopy.ignitionAttributes;
	maxBurnDistance = toCopy.maxBurnDistance;
	simulationDate = toCopy.simulationDate;
	fireSize = toCopy.fireSize;
	perimeterTotal = toCopy.perimeterTotal;
	perimeterActive = toCopy.perimeterActive;
	wxValues = toCopy.wxValues;
	fwiValues = toCopy.fwiValues;
	areaUnits = toCopy.areaUnits;
	perimeterUnits = toCopy.perimeterUnits;
	shouldStream = toCopy.shouldStream;
	exported = toCopy.exported;
	timeOverrides = toCopy.timeOverrides;
	assetArrivalTime = toCopy.assetArrivalTime;
	assetCount = toCopy.assetCount;
	identifyFinalPerimeter = toCopy.identifyFinalPerimeter;
	simulationStatus = toCopy.simulationStatus;
}


Project::VectorOutput::VectorOutput(VectorOutput &&toTransfer) :
	startTime(toTransfer.startTime),
	endTime(toTransfer.endTime),
	filename{ std::move(toTransfer.filename) },
	scenarioName{ std::move(toTransfer.scenarioName) },
	subScenarioNames{ std::move(toTransfer.subScenarioNames) },
	coverageName{ std::move(toTransfer.coverageName) } {
	m_mutex = toTransfer.m_mutex;
	toTransfer.m_mutex = nullptr;

	multiplePerimeters = toTransfer.multiplePerimeters;
	removeIslands = toTransfer.removeIslands;
	mergeContacting = toTransfer.mergeContacting;
	activePerimeters = toTransfer.activePerimeters;
	applicationVersion = toTransfer.applicationVersion;
	outputScenarioName = toTransfer.outputScenarioName;
	jobName = toTransfer.jobName;
	ignitionName = toTransfer.ignitionName;
	ignitionLocation = toTransfer.ignitionLocation;
	ignitionAttributes = toTransfer.ignitionAttributes;
	maxBurnDistance = toTransfer.maxBurnDistance;
	simulationDate = toTransfer.simulationDate;
	fireSize = toTransfer.fireSize;
	perimeterTotal = toTransfer.perimeterTotal;
	perimeterActive = toTransfer.perimeterActive;
	wxValues = toTransfer.wxValues;
	fwiValues = toTransfer.fwiValues;
	areaUnits = toTransfer.areaUnits;
	perimeterUnits = toTransfer.perimeterUnits;
	shouldStream = toTransfer.shouldStream;
	exported = toTransfer.exported;
	timeOverrides = toTransfer.timeOverrides;
	assetArrivalTime = toTransfer.assetArrivalTime;
	assetCount = toTransfer.assetCount;
	identifyFinalPerimeter = toTransfer.identifyFinalPerimeter;
	simulationStatus = toTransfer.simulationStatus;
}


Project::VectorOutput::~VectorOutput()
{
	if (m_mutex)
		delete m_mutex;
}


void Project::VectorOutput::lock()
{
	m_mutex->lock();
}


void Project::VectorOutput::unlock()
{
	m_mutex->unlock();
}


HSS_Time::WTime& Project::VectorOutput::getStartTime(const std::string& subName)
{
	auto val = std::find_if(timeOverrides.begin(), timeOverrides.end(), [&](const VectorOutputTimeOverride& time) -> bool { return boost::iequals(time.subName, subName); });
	if (val != timeOverrides.end())
		return val->startTime;
	return startTime;
}


HSS_Time::WTime& Project::VectorOutput::getEndTime(const std::string& subName)
{
	auto val = std::find_if(timeOverrides.begin(), timeOverrides.end(), [&](const VectorOutputTimeOverride& time) -> bool { return boost::iequals(time.subName, subName); });
	if (val != timeOverrides.end())
		return val->endTime;
	return endTime;
}


Project::FuelGridOutput::FuelGridOutput() {
}


Project::FuelGridOutput::FuelGridOutput(const FuelGridOutput& toCopy) : file_exported{ (bool)toCopy.file_exported } {
	scenarioName = toCopy.scenarioName;
	subScenarioName = toCopy.subScenarioName;
	filename = toCopy.filename;
	coverageName = toCopy.coverageName;
	compression = toCopy.compression;
	shouldStream = toCopy.shouldStream;
}


Project::FuelGridOutput::FuelGridOutput(FuelGridOutput&& toMove)
	: filename{ std::move(toMove.filename) },
	scenarioName{ std::move(toMove.scenarioName) },
	subScenarioName{ std::move(toMove.subScenarioName) },
	file_exported{ (bool)toMove.file_exported },
	coverageName{ std::move(toMove.coverageName) } {
	shouldStream = toMove.shouldStream;
	compression = toMove.compression;
}


Project::FuelGridOutput::~FuelGridOutput() {
}


Project::GridOutput::GridOutput()
{
	m_mutex = new std::mutex();
}


Project::GridOutput::GridOutput(GridOutput &&toMove)
	: filename{ std::move(toMove.filename) },
	coverageName{ std::move(toMove.coverageName) },
	assetName{ std::move(toMove.assetName) },
	assetIndex{ std::move(toMove.assetIndex) },
	statistic{ std::exchange(toMove.statistic, 0) },
	m_d{ toMove.m_d },
	m_darray{ toMove.m_darray },
	checkPaths{ (bool)toMove.checkPaths },
	lastOutputCount{ std::exchange(toMove.lastOutputCount, 0) },
	m_mutex{ toMove.m_mutex }
{
	toMove.m_d = nullptr;
	toMove.m_darray = nullptr;
	toMove.m_mutex = nullptr;
}


Project::GridOutput::~GridOutput()
{
	cleanup();
	if (m_mutex)
		delete m_mutex;
}


void Project::GridOutput::cleanup()
{
	if (m_d) {
		delete[] m_d;
		m_d = nullptr;
	}
	if (m_darray) {
		delete m_darray;
		m_darray = nullptr;
	}
}


void Project::GridOutput::lock()
{
	m_mutex->lock();
}


void Project::GridOutput::unlock()
{
	m_mutex->unlock();
}


Project::AssetOutput::AssetOutput()
	: m_mutex{ new std::mutex() },
	m_statBuilder{ nullptr }
{ }


Project::AssetOutput::AssetOutput(AssetOutput&& toMove)
	: scenarioName{ std::move(toMove.scenarioName) },
	filename{ std::move(toMove.filename) },
	criticalPathFilename{ std::move(toMove.criticalPathFilename) },
	fileType{ toMove.fileType },
	shouldStream{ toMove.shouldStream },
	criticalPath{ toMove.criticalPath },
	exportCount{ std::exchange(toMove.exportCount, 0) },
	file_exported{ (bool)toMove.file_exported },
	m_mutex{ toMove.m_mutex },
	m_statBuilder{ toMove.m_statBuilder }
{
	toMove.m_statBuilder = nullptr;
	toMove.m_mutex = nullptr;
}


Project::AssetOutput::~AssetOutput()
{
	cleanup();
	if (m_mutex)
		delete m_mutex;
}


void Project::AssetOutput::cleanup()
{
	if (m_statBuilder)
	{
		delete m_statBuilder;
		m_statBuilder = nullptr;
	}
}


void Project::AssetOutput::lock()
{
	m_mutex->lock();
}


void Project::AssetOutput::unlock()
{
	m_mutex->unlock();
}


HSS_Time::WTime& Project::GridOutputs::getStartExportTime(const std::string& subName)
{
	auto val = std::find_if(timeOverrides.begin(), timeOverrides.end(), [&](const GridOutputTimeOverride& time) -> bool { return boost::iequals(time._subName, subName); });
	if (val != timeOverrides.end())
		if ((val->_startTime.IsValid()) && (val->_startTime.GetTotalMicroSeconds()))
			return val->_startTime;
	return startExportTime;
}


HSS_Time::WTime& Project::GridOutputs::getExportTime(const std::string& subName)
{
	auto val = std::find_if(timeOverrides.begin(), timeOverrides.end(), [&](const GridOutputTimeOverride& time) -> bool { return boost::iequals(time._subName, subName); });
	if (val != timeOverrides.end())
		if ((val->_endTime.IsValid()) && (val->_endTime.GetTotalMicroSeconds()))
			return val->_endTime;
	return exportTime;
}


Project::StatsOutputs::StatsOutputs(StatsOutputs&& toMove)
	: scenarioName(std::move(toMove.scenarioName)),
	filename(std::move(toMove.filename)),
	fileType(toMove.fileType),
	entries(std::move(toMove.entries)),
	shouldStream(std::exchange(toMove.shouldStream, false)),
	file_exported(static_cast<bool>(toMove.file_exported))
{
}


Project::SummaryOutput::SummaryOutput(const SummaryOutput &toCopy)
{
	scenarioName = toCopy.scenarioName;
	filename = toCopy.filename;
	m_collection = toCopy.m_collection;

	if (toCopy.applicationName.has_value())
		applicationName = toCopy.applicationName.value();
	if (toCopy.geoData.has_value())
		geoData = toCopy.geoData.value();
	if (toCopy.assetData.has_value())
		assetData = toCopy.assetData.value();
	if (toCopy.outputScenarioName.has_value())
		outputScenarioName = toCopy.outputScenarioName.value();
	if (toCopy.scenarioComments.has_value())
		scenarioComments = toCopy.scenarioComments.value();
	if (toCopy.inputs.has_value())
		inputs = toCopy.inputs.value();
	if (toCopy.landscape.has_value())
		landscape = toCopy.landscape.value();
	if (toCopy.fbpPatches.has_value())
		fbpPatches = toCopy.fbpPatches.value();
	if (toCopy.wxPatches.has_value())
		wxPatches = toCopy.wxPatches.value();
	if (toCopy.ignitions.has_value())
		ignitions = toCopy.ignitions.value();
	if (toCopy.wxStreams.has_value())
		wxStreams = toCopy.wxStreams.value();
	if (toCopy.fbp.has_value())
		fbp = toCopy.fbp.value();
	if (toCopy.wxData.has_value())
		wxData = toCopy.wxData.value();

	shouldStream = toCopy.shouldStream;
	exported.store(toCopy.exported.load());
	m_mutex = new std::mutex();
}

Project::GridOutputs::GridOutputs(GridOutputs &&toMove)
	: scenarioName{ std::move(toMove.scenarioName) },
	startExportTime{ toMove.startExportTime },
	exportTime{ toMove.exportTime },
	subScenarioNames{ std::move(toMove.subScenarioNames) },
	interpolation{ std::exchange(toMove.interpolation, 0) },
	discretize{ toMove.discretize },
	shouldStream{ std::exchange(toMove.shouldStream, false) },
	outputNodata{ std::exchange(toMove.outputNodata, false) },
	minimizeOutput{ std::exchange(toMove.minimizeOutput, true) },
	file_exported{ (bool)(toMove.file_exported) },
	outputs{ std::move(toMove.outputs) },
	timeOverrides{ std::move(toMove.timeOverrides) },
	compression{ toMove.compression },

	gridOptionsSet{ std::exchange(toMove.gridOptionsSet, false) },
	originValid{ std::move(toMove.originValid) },
	origin{ toMove.origin },
	resolution{ toMove.resolution },
	outputOriginMode { toMove.outputOriginMode },
	scale{ toMove.scale },
	useFuelMap{ std::exchange(toMove.useFuelMap, true) }
{
}


Project::SummaryCollection::SummaryCollection()
	: m_collection(new WISE::ProjectProto::ScenarioReportGroup())
{
}


Project::SummaryCollection::~SummaryCollection()
{
	delete m_collection;
}


Project::SummaryOutput::SummaryOutput()
	: m_collection(nullptr)
{
	m_mutex = new std::mutex();
}


Project::SummaryOutput::~SummaryOutput()
{
	cleanup();
	if (m_mutex)
		delete m_mutex;
}


void Project::SummaryOutput::makeCumulativeOutput()
{
	if (!m_collection)
		m_collection = new SummaryCollection();
}


void Project::SummaryOutput::cleanup()
{
	if (m_collection) {
		delete m_collection;
		m_collection = nullptr;
	}
}


std::uint32_t Project::SummaryOutput::cumulativeSize()
{
	if (m_collection)
		return m_collection->m_collection->reports_size();
	return 0;
}


void Project::SummaryOutput::lock()
{
	m_mutex->lock();
}


void Project::SummaryOutput::unlock()
{
	m_mutex->unlock();
}
