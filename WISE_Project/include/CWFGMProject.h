/**
 * WISE_Project: CWFGMProject.h
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

#if !defined(AFX_CWFGMPROJECT_H__0B4298A6_0A6C_11D4_BCD5_00A0833B1640__INCLUDED_)
#define AFX_CWFGMPROJECT_H__0B4298A6_0A6C_11D4_BCD5_00A0833B1640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __INTEL_COMPILER
#pragma managed(push, off)
#endif

#include "cwfgmp_config.h"
#include <functional>
#include <vector>

#if _DLL
#include "cwinapp.h"
#else
#include "AfxIniSettings.h"
#endif

#include "ICWFGM_FBPFuel.h"
#include "GridCOM.h"
#include "GridCom_ext.h"
#include "convert.h"
#include "FGMOutputs.h"

#include "FuelCollection.h"
#include "GridFilterCollection.h"
#include "FireCollection.h"
#include "ScenarioCollection.h"
#include "WeatherCollection.h"
#include "LandscapeWarningOptions.h"
#include "vectorcollection.h"
#include "assetcollection.h"
#include "targetcollection.h"
#include "viewstorage.h"
#include "CoordinateConverter.h"
#include "WeatherGridManager.h"
#include "Dlgcnvt.h"
#include "ISerializeProto.h"
#include "Thread.h"
#include "TimestepSettings.h"
#include "CumulativeGridOutput.h"
#include "SummaryCollection.h"
#include "WTime.h"


namespace Project
{
	class BasicScenarioOptions;
};


#define METHOD_IDW					0			// Inverse distance weighting
#define METHOD_VAW					1			// Voronoi area weighting
#define METHOD_NEAREST				2			// just pick the closest vertex to the point of interest
#define METHOD_DISCRETIZE			3			// discretize the cell into small portions, then apply METHOD_NEAREST to each of the sub-cells

#define PARA_FI						10			// Fire intensity (vector stat derived from the differential equations)
#define PARA_FLAMELENGTH			11			// Flame Length
#define PARA_ROS					12			// Rate of spread (vector stat derived from the differential equations)
#define PARA_SFC					13			// Surface fuel consumption (vector stat derived from the differential equations)
#define PARA_CFC					14			// Crown fuel consumption (vector stat derived from the differential equations)
#define PARA_TFC					15			// Total fuel consumption (vector stat derived from the differential equations)
#define PARA_CFB					16			// crown fraction burned (vector stat derived from the differential equations)
#define PARA_RAZ					17			// direction the fire is burning (vector stat derived from the differential equations)
#define PARA_BURNT					18			// Is this grid cell burnt?
#define PARA_BURNT_MEAN				100			// mean times a cell was burnt
#define PARA_ARRIVAL_TIME			19			// time between when the cell burned, and when the ignition that burned it started
#define PARA_FBP_HROS				20			// direct FBP stat: HROS
#define PARA_FBP_HROS_MAP			-1			// direct FBP stat: HROS
#define PARA_FBP_FROS				21			// direct FBP stat: FROS
#define PARA_FBP_FROS_MAP			-2			// direct FBP stat: HROS
#define PARA_FBP_BROS				22			// direct FBP stat: BROS
#define PARA_FBP_BROS_MAP			-3			// direct FBP stat: HROS
#define PARA_FBP_RSS				23			// direct FBP stat: RSS
#define PARA_FBP_RSS_MAP			-4			// direct FBP stat: HROS
#define PARA_FBP_RAZ_MAP			-5			// direct FBP stat: HROS
#define PARA_FBP_FMC_MAP			-6			// direct FBP stat: FMC
#define PARA_FBP_CFB_MAP			-7			// direct FBP stat: CFB
#define PARA_FBP_CFC_MAP			-8			// direct FBP stat: CFC
#define PARA_FBP_SFC_MAP			-9			// direct FBP stat: SFC
#define PARA_FBP_TFC_MAP			-10			// direct FBP stat: TFC
#define PARA_FBP_FI_MAP				-11			// direct FBP stat: FI
#define PARA_FBP_FLAMELENGTH_MAP	-12			// direct stat: flame length
#define PARA_CURINGDEGREE_MAP		-13
#define PARA_GREENUP_MAP			-14
#define PARA_PC_MAP					-15
#define PARA_PDF_MAP				-16
#define PARA_CBH_MAP				-17
#define PARA_TREE_HEIGHT_MAP		-18
#define PARA_FUEL_LOAD_MAP			-19
#define PARA_CFL_MAP				-20
#define PARA_GRASSPHENOLOGY_MAP		-21
#define PARA_ROSVECTOR_MAP			-22
#define PARA_DIRVECTOR_MAP			-23
#define PARA_ACTIVE					24
#define PARA_BURNED					25
#define PARA_BURNED_CHANGE			26
#define PARA_MIN_ARRIVAL_TIME		27
#define PARA_MAX_ARRIVAL_TIME		28
#define PARA_TOTAL_FUEL_CONSUMED	29
#define PARA_SURFACE_FUEL_CONSUMED	30
#define PARA_CROWN_FUEL_CONSUMED	31
#define PARA_RADIATIVE_POWER		32
#define PARA_HFI					33
#define PARA_HCFB					34
#define PARA_CRITICAL_PATH			35
#define PARA_CRITICAL_PATH_MEAN		36
#define PARA_TEMP		0			// Temperature
#define PARA_DEW		1			// Dew Point Temperature
#define PARA_RH			2			// Relative Humidity
#define PARA_WD			3			// wind direction
#define PARA_WS			4			// wind speed
#define PARA_GUST		200			// wind gusts
#define PARA_PRECIP		5			// precipitation
#define PARA_FFMC		6			// hourly FFMC
#define PARA_ISI		7			// hourly ISI
#define PARA_FWI		8			// hourly FWI
#define PARA_SBUI		9			// hourly BUI

struct Project_XYStatOptions
{
	WTime* mintime;
	WTime* time;
	unsigned short method;
	unsigned short discretization;
};

/////////////////////////////////////////////////////////////////////////////
// CWFGMProject

typedef bool (__cdecl *export_setsize)(APTR aParm, ULONG cnt);			// returns true - continue, false - abort
typedef bool (__cdecl *export_iteration)(APTR aParm, ULONG cnt);		// returns true - continue, false - abort


#if _DLL
class CWFGMPROJECT_EXPORT CWinAppProject : public CWinAppExt
#else
class CWFGMPROJECT_EXPORT CWinAppProject : public IniSettings
#endif
{
public:
	virtual Project::FBPOptions* getFBPOptions() = 0;
	virtual Project::BasicScenarioOptions* getBasicScenarioOptions() = 0;
	virtual Project::CBurnPeriodOption* getBurnPeriodOptions() = 0;
	virtual Project::ViewCollection* getConfigurationOptions() = 0;
	virtual Project::CuringOptions* getCuringOptions() = 0;
#if _DLL
	virtual const CRuntimeClass* findClass(const char* name) = 0;
#else
	virtual const HSS_Types::RuntimeClass *findClass(const char *name) = 0;
#endif
};

#if _DLL
static inline CWinAppProject* AfxGetProjectApp() { return (CWinAppProject *)AfxGetApp(); }
#else
extern CWinAppProject* AfxGetProjectApp();
#endif

namespace Project
{
	constexpr int IMPORT_REGULAR = 0;
	constexpr int IMPORT_ZIP = 1;

	enum class LoadBalancingType
	{
		NONE = 0,
		EXTERNAL_COUNTER = 1,
		LOCAL_FILE = 2
	};


class CWFGMPROJECT_EXPORT CWFGMProject : public ISerializeProto
{
#if !defined(_DLL) || _DLL == 0
    typedef std::vector<std::byte> CByteArray;
#endif

	mutable double m_resolution;
	Project::LoadBalancingType m_loadBalancing;
	HRESULT getDimensions(USHORT *xdim, USHORT *ydim) const;
	HRESULT getResolution(double *resolution) const;

protected:
	boost::intrusive_ptr<CCWFGM_FuelMap>	m_fuelMap;
	boost::intrusive_ptr<CCWFGM_Grid>		m_grid;
	boost::intrusive_ptr<ICWFGM_GridEngine>	m_gridEngine;

	std::string printableString(const std::string &str, USHORT indent_cnt);

public:
	boost::intrusive_ptr<CCWFGM_LayerManager>	m_layerManager;
	static const char *m_permissibleVectorReadDrivers[];
	static std::vector<std::string> m_permissibleVectorReadDriversSA;
	static std::int32_t		m_managedMode;

	std::string				m_comments;
	std::string				m_csProjectionName;
	std::string				m_csGridName;
	std::string				m_csElevationName;
	std::string				m_loadWarnings;
	std::string				m_preExecutionCommand_Linux, m_postExecutionCommand_Linux;
	std::string				m_preExecutionCommand_Windows, m_postExecutionCommand_Windows;

	bool ExportParameterGrid(const TCHAR *szPath, const Scenario *scenario, const Asset *asset, const ULONG asset_index, const Project_XYStatOptions &options,
        std::int16_t ParaOption, bool explicitNodata, bool minimizeOutput, bool multilayer,
        export_setsize ess_fcn, export_iteration ei_fcn, APTR parm, GDALExporter::CompressionType compression, ::UnitConversion* const unitConversion);

	bool ExportParameterGrid(std::vector<GridOutput> &outputs, const Scenario *scenario, const Asset* asset, const ULONG asset_index, const Project_XYStatOptions&options,
		bool explicitNodata, bool minimizeOutput, bool multilayer, export_setsize ess_fcn,
		export_iteration ei_fcn, APTR parm, USHORT mode, GDALExporter::CompressionType compression, ::UnitConversion* const unitConversion);

    /**
     * Export a grid from the given scenario into a cumulative grid output. The output from the scenario
     * will be added onto the existing grid data.
     * @param output The cumulative grid output to add on to.
     * @param wxTime The simulation time to output grid data at.
     * @param scenario The scenario to output data for.
     * @param method The interpolation method to use on the grid.
     * @param ParaOption The type of grid to output.
     * @param ess_fcn
     * @param ei_fcn
     * @param parm
     * @tparam T The variable type the grid is storing.
     * @return True if the export was successful, false otherwise.
     */
private:
    template<typename T>
    bool exportParameterGridArray(std::vector<GDALExtras::CumulativeGridOutput<T> *> &outputs, const Scenario *scenario, const Asset *asset, const ULONG asset_index, const Project_XYStatOptions&options,
        std::vector<std::int16_t> &ParaOption, bool explicitNodata, export_setsize ess_fcn, export_iteration ei_fcn, APTR parm,
		::UnitConversion* const unitConversion);

public:
	bool exportParameterGridArray(std::vector<GDALExtras::CumulativeGridOutput<double> *> *outputs, const Scenario *scenario, const Asset* asset, const ULONG asset_index, const Project_XYStatOptions&options,
		std::vector<std::int16_t> &ParaOption, bool explicitNodata, export_setsize ess_fcn, export_iteration ei_fcn, APTR parm,
		::UnitConversion* const unitConversion);

	/**
	 * Get the type of load balancing that has been requested to be used for this project.
	 */
	Project::LoadBalancingType loadBalancing() { return m_loadBalancing; }
	void loadBalancing(Project::LoadBalancingType balancing) { this->m_loadBalancing = balancing; }

public:
	/**
     * Export a cumulative grid output to a file. This will normalize the output grid so further
     * additions to the grid should not be done.
     * @param szPath The path to the location to save the file to.
     * @param ParaOption The type of grid to output.
     * @param output The cumulative grid to save.
     * @param maximum An optional maximum value to normalize the output grid to.
     *                If specified the minimum value in the output file will always be 0 and the
     *                maximum will always be this value. If not specified the output grid will be
     *                averaged using the write counter.
     * @tparam T The variable type the grid is storing.
     * @return True if the export was successful, false otherwise.
     */
    template<typename T>
    bool ExportParameterGrid(const TCHAR *szPath, std::int16_t ParaOption, bool explicitNodata, bool minimizeOutput, bool multilayer, GDALExtras::CumulativeGridOutput<T> &output, GDALExporter::CompressionType compression, UnitConversion* const unitConversion, T maximum = -1);
	bool ExportParameterGrid(const TCHAR *szPath, std::int16_t ParaOption, bool explicitNodata, bool minimizeOutput, bool multilayer, GDALExtras::CumulativeGridOutput<double> *output, GDALExporter::CompressionType compression, UnitConversion* const unitConversion, double maximum = -1.0);

    /**
     * Start a thread that will do the grid export.
     */
    template<typename T>
    bool ExportParameterGrid(std::vector<T*> &arr, const Scenario *scenario, const Asset *asset, const ULONG asset_index, const Project_XYStatOptions &options, std::vector<std::int16_t> const &ParaOption, bool explicitNodata,
		USHORT &left, USHORT &right, USHORT &bottom, USHORT &top, export_setsize ess_fcn, export_iteration ei_fcn, APTR parm, ::UnitConversion* const unitConversion);

private:
    /**
     * Write grid export data to a file.
     */
    template<typename T>
    bool ExportParameterGrid(const TCHAR *szPath, T* const arr, USHORT xSize, USHORT ySize, std::int16_t ParaOption,
		bool explicitNodata, USHORT left, USHORT right, USHORT bottom, USHORT top, bool minimizeOutput, bool shouldFlip,
		GDALExporter::CompressionType compression, UnitConversion* const unitConversion);

	template<typename T>
	bool ExportParameterGrid(const TCHAR *szPath, T* const acc, T* const acc2, T* const _min, T* const _max, T* const stdm, T* const stds, uint64_t * const cnt, USHORT xSize, USHORT ySize, std::int16_t ParaOption,
		bool explicitNodata, USHORT left, USHORT right, USHORT bottom, USHORT top, bool minimizeOutput, bool shouldFlip, GDALExporter::CompressionType compression, UnitConversion* const unitConversion);

	const std::string statName(const std::int16_t ParaOption, UnitConversion* const unitConversion);

	HRESULT PrintReportTxt(const TCHAR *szPath, const class PrintReportOptions &PrntRptOptns, const TCHAR *VersionNumber, const TCHAR *ReleaseDate, const TCHAR *FGMName, Scenario *currScenario, const ::UnitConversion *uc);
	HRESULT PrintReportJson(const TCHAR *szPath, const Project::SummaryCollection* collection, const class PrintReportOptions &PrntRptOptns,
		const TCHAR *VersionNumber, const TCHAR *ReleaseDate, const TCHAR *FGMName, const std::vector<Scenario*> &scenarios,
		const ::UnitConversion *uc, bool addWeather);

	public:
	    HRESULT PrintReport(const TCHAR *szPath, const class PrintReportOptions &PrntRptOptns, const TCHAR *VersionNumber, const TCHAR *ReleaseDate, const TCHAR *FGMName, Project::Scenario *currScenario, const ::UnitConversion *uc, bool addWeather);
		HRESULT PrintReport(const Project::SummaryCollection* collection, const class PrintReportOptions& PrntRptOptns,
			const TCHAR* VersionNumber, const TCHAR* ReleaseDate, const TCHAR* FGMName, Scenario* currScenario,
			const ::UnitConversion* uc, bool addWeather);
		HRESULT ExportReportJson(const TCHAR* szPath, const Project::SummaryCollection* collection);
		double GetMeanElevation();
	    double GetMedianElevation();
	    bool ExportOperatingHistory(const TCHAR *szPath);
	    void InitializeCoorConverter();
	    bool DEMPresent();
	    double GetDefaultElevation();

	    bool GetXYByLL(const double Lat, const double Lon, double *X, double *Y);		// these 2 routines work in METRES origin lower left corner of plot
	    bool GetLLByCoord(const double x, const double y, double *Lat, double *Lon);
		bool GetLLByAbsoluteCoord(const double x, const double y, double* Lat, double* Lon);

	    std::string FormatXY(const double x, const double y, const ::UnitConversion *uc, bool include_units)			{ return FormatXY(x, y, uc->CoordinateDisplay(), uc, include_units); };
	    std::string FormatXY(const double x, const double y, UnitConvert::STORAGE_UNIT to_format, const ::UnitConversion *uc, bool include_units);
	    std::string FormatLatLon(const double latitude, const double longitude, const ::UnitConversion *uc, bool include_units)	{ return FormatLatLon(latitude, longitude, uc->CoordinateDisplay(), uc, include_units); }
	    std::string FormatLatLon(const double latitude, const double longitude, UnitConvert::STORAGE_UNIT to_format, const ::UnitConversion *uc, bool include_units);

	    void GetMinMaxElev(double *MinElev, double *MaxElev);
	    void GetMinMaxSlope(double *MinSlope, double *MaxSlope);
	    void GetMinMaxAzimuth(double *MinAzimuth, double *MaxAzimuth);
		bool ExportLookupTable(const std::string &szName);
		bool ExportGrid(const std::string& szName, GDALExporter::CompressionType compression = GDALExporter::CompressionType::NONE);
		bool ExportElevationGrid(const std::string &szPath);
		bool ExportSlopeGrid(const std::string &szPath);
		bool ExportAspectGrid(const std::string &szPath);

	CWFGMProject();							// used for dynamic creation
	CWFGMProject(CCWFGM_FuelMap *fuelMap, FuelCollection *fuelCollection, bool resetLUTName);
	virtual ~CWFGMProject();

	HSS_Time::WTimeManager		*m_timeManager;			// the one and only time manager for this doc
	WeatherStationCollection	m_weatherCollection;
	FuelCollection				m_fuelCollection;
	GridFilterCollection		m_gridFilterCollection;
	VectorCollection			m_vectorCollection;
	AssetCollection				m_assetCollection;
	TargetCollection			m_targetCollection;
	FireCollection				m_fireCollection;
	ScenarioCollection			m_scenarioCollection;
	ViewCollection				m_viewCollection;		// object that encapsulates the lists of views and their
									// settings - not necessary the current views, but what the
									// user last asked for/set
	FGMOutputs					m_outputs;
	Service::TimestepSettings   m_timestepSettings;
	CCoordinateConverter		m_CoorConverter;
	CCWFGM_FuelMap		*fuelMap() const		{ return m_fuelMap.get(); };
	CCWFGM_Grid		*grid() const			{ return m_grid.get(); };
	ICWFGM_GridEngine	*gridEngine() const		{ return m_gridEngine.get(); };

	void AssignNewGrid(CCWFGM_Grid *grid);

	HRESULT LockState()					{ return m_gridEngine->MT_Lock(nullptr, false, (USHORT)-1); };

	HRESULT GetDimensions(USHORT *xdim, USHORT *ydim) const;
	HRESULT GetResolution(double *resolution) const;
	HRESULT GetXLLCorner(double *xllCorner) const;
	HRESULT GetYLLCorner(double *yllCorner) const;
	HRESULT GetXURCorner(double *xurCorner) const;
	HRESULT GetYURCorner(double *yurCorner) const;
	HRESULT GetLLCorner(XY_Point *pt) const;
	HRESULT GetURCorner(XY_Point *pt) const;
	HRESULT GetExtents(XY_Point *ll, XY_Point *ur) const;
	USHORT convertX(double x) const;
	USHORT convertY(double y) const;
	HRESULT convert(XY_Point* pt) const;
	double invertX(double x) const;
	double invertY(double y) const;
	HRESULT invert(XY_Point* pt) const;
	
	void ExportHeader(FILE *fp, USHORT *xSize, USHORT *ySize) const;

	void GetLocation(double *latitude, double *longitude) const;
	bool InsideCanada() const;
	bool InsideNewZealand() const;
	bool InsideTasmania() const;
	bool InsideAustraliaMainland() const;
	HRESULT setLocation(double latitude, double longitude);

	const ::TimeZoneInfo *GuessTimeZone(SHORT set);
	HSS_Time::WTimeSpan GetTimeZone() const;
	std::uint32_t GetTimeZoneId() const;
	HRESULT SetTimeZoneTS(const HSS_Time::WTimeSpan &timezone);
	HRESULT SetTimeZone(std::uint32_t timezoneId);
	const TimeZoneInfo *GetTimeZoneStruct(SHORT i) const	{ return m_timeManager->m_worldLocation.CurrentTimeZone(i); };
	const WorldLocation& worldLocation()							{ return m_timeManager->m_worldLocation; }

	bool DaylightSavings(bool enable);
	bool DaylightSavings() const;

	double GetDefaultFMC(bool &active) const;
	void SetDefaultFMC(double fmc, bool active);

	void RecordUsedFuels();

	void CollectLoadWarnings();

	CWorkerThreadPool *GetPool();

public:
	HRESULT serialize(const std::string& filename, const std::uint32_t file_major_version, const std::function<void(google::protobuf::Message*)> updateCallback, const std::function<std::string(void)>& password_callback);
	HRESULT deserialize(const std::string& filename, std::uint32_t& file_major_version, const std::function<std::string(void)>& password_callback, google::protobuf::Message** result,
		std::shared_ptr<validation::validation_object> valid, const std::string& name);
	std::string serializeOutputs(const SerializeProtoOptions& options, bool binary);
	virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
	virtual WISE::ProjectProto::Project* serialize(const SerializeProtoOptions& options) override;
	virtual CWFGMProject *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
	std::optional<bool> isdirty(void) const noexcept override { return std::make_optional(mModified); }

	unsigned int RegisterOutputDataListener(const std::function<void(void)> &callback);
	void UnregisterOutputDataListener(unsigned int id);
	HRESULT GetGridFileData(std::string* filename, std::string* scenName, HSS_Time::WTime *startTime, HSS_Time::WTime* time, int* interpMethod, int* stat, int *discretize);
	HRESULT SetGridFileData(const std::string& filename, const std::string& scenName, const HSS_Time::WTime &startTime, const HSS_Time::WTime &time, int interpMethod, int stat, int discretize);
	HRESULT GetVectorFileData(std::string *filename, std::string* scenName, HSS_Time::WTime* time, int* exportAll, int* remIslands, int* mergeContact, int* onlyActive);
	HRESULT GetVectorFileMetadata(int* promVer, int* scenName, int* jobName, int* igName, int* simDate, int* fireSize, int* totalPerim, int* activePerim, int* wxValues, int* fwiValues, int *ignitionLocation, int *ignitionAttributes, int *maxBurnDistance, UnitConvert::STORAGE_UNIT* areaUnit, UnitConvert::STORAGE_UNIT* perimUnit);
	HRESULT SetVectorFileData(const std::string& filename, const std::string& scenName, const HSS_Time::WTime &time, int exportAll, int remIslands, int mergeContact, int onlyActive);
	HRESULT SetVectorFileMetadata(int promVer, int scenName, int jobName, int igName, int simDate, int fireSize, int totalPerim, int activePerim, int wxValues, int fwiValues, int ignitionLocation, int ignitionAttributes, int maxBurnDistance, UnitConvert::STORAGE_UNIT areaUnit, UnitConvert::STORAGE_UNIT perimUnit);

protected:
	CWorkerThreadPool *m_pool;
	CRWThreadSemaphore m_poolLock;

private:
	std::string GetAsciiGridHeader() const;
	bool _daylightSavings(bool enable, bool oldSerialize);
	void adjustProjectTimes(const HSS_Time::WTimeSpan &diff);

	void removeQuadrantWindGrids();

private:
	bool mModified = false;
};


struct _error_msg {
	HRESULT code;
	const char *msg;
};
};

CWFGMPROJECT_EXPORT const struct Project::_error_msg *error_msgs();

#ifndef __INTEL_COMPILER
#pragma managed(pop)
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CWFGMPROJECT_H__0B4298A6_0A6C_11D4_BCD5_00A0833B1640__INCLUDED_)
