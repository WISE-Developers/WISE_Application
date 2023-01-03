/**
 * WISE_Project: WeatherCollection.h
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

#if !defined(AFX_WEATHERCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
#define AFX_WEATHERCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cwfgmp_config.h"
#include "ICWFGM_Fuel.h"
#include "CWFGM_WeatherStream.h"
#include "WeatherCom_ext.h"
#include "GridCom_ext.h"
#include "linklist.h"
#include "WTime.h"
#include "WeatherOptions.h"
#include "Exprtopt.h"
#include "FwiCom.h"
#include "FontHelper.h"
#include "CWFGM_Grid.h"
#include "intrusive_ptr.h"

#include "ISerializeProto.h"

#include "weatherStation.pb.h"


namespace Project
{
	class CBurnPeriodOption;
	class WeatherStation;

	class CWFGMPROJECT_EXPORT WeatherStream : public MinNode, public ISerializeProto
	{
		friend class WeatherStation;

	protected:
		WeatherStream(CCWFGM_WeatherStream *stream);

		WeatherStation		*m_station;

	public:
		void ExportBasicInfo(FILE *fp);
		bool IsImportedFromFile();
		bool IsImportedFromUrl();
		bool IsDateImportedFromFile(const HSS_Time::WTime &time);
		HRESULT Export(ICWFGM_FWI *FWI, const TCHAR *szPath, const ExportOptions *exportOptions, SHORT export_hourly, bool include_fwi, bool need_head);
		HRESULT Import(const std::string &fileName, USHORT options);
		void ClearWeatherData();

		HRESULT SetFFMCMethod(short method);
		HRESULT SetWindGamma(double gamma);
		HRESULT SetWindBeta(double beta);
		HRESULT SetWindAlpha(double alpha);
		HRESULT SetTempGamma(double gamma);
		HRESULT SetTempBeta(double beta);
		HRESULT SetTempAlpha(double alpha);

	protected:
		WeatherStream();
		WeatherStream(const WeatherStream &stream);

	public:
		virtual ~WeatherStream()				{ };

		__INLINE WeatherStream *LN_Succ() const			{ return (WeatherStream *)MinNode::LN_Succ(); };
		__INLINE WeatherStream *LN_Pred() const			{ return (WeatherStream *)MinNode::LN_Pred(); };

		__INLINE WeatherStation *m_weatherStation() const	{ return m_station; };
		HRESULT GetValidTimeRange(HSS_Time::WTime &start, HSS_Time::WTimeSpan &duration);
		__INLINE HRESULT SetStartTime(const HSS_Time::WTime &start)	{ PolymorphicAttribute v = start; return m_stream->SetAttribute(CWFGM_WEATHER_OPTION_START_TIME, v); };
		__INLINE HRESULT SetEndTime(const HSS_Time::WTime &end)		{ PolymorphicAttribute v = end; return m_stream->SetAttribute(CWFGM_WEATHER_OPTION_END_TIME, v); };
		__INLINE bool GetStartTime(HSS_Time::WTime &start) { PolymorphicAttribute v; HRESULT hr; if (SUCCEEDED(hr = m_stream->GetAttribute(CWFGM_WEATHER_OPTION_START_TIME, &v))) { VariantToTime_(v, &start); } return SUCCEEDED(hr); };
		__INLINE bool GetEndTime(HSS_Time::WTime &EndTime) { PolymorphicAttribute v; HRESULT hr; if (SUCCEEDED(hr = m_stream->GetAttribute(CWFGM_WEATHER_OPTION_END_TIME, &v))) { VariantToTime_(v, &EndTime); } return SUCCEEDED(hr); };
		__INLINE bool GetFirstHourOfDay(const HSS_Time::WTime& day, int& hour) { UCHAR h; if (SUCCEEDED(m_stream->GetFirstHourOfDay(day, &h))) { hour = h; return true; } hour = -1; return false; }
		__INLINE bool GetLastHourOfDay(const HSS_Time::WTime& day, int& hour) { UCHAR h; if (SUCCEEDED(m_stream->GetLastHourOfDay(day, &h))) { hour = h; return true; } hour = -1; return false; }
		__INLINE HRESULT SetLocation(double latitude, double longitude)
									{ HRESULT hr; PolymorphicAttribute v = latitude; if (SUCCEEDED(hr = m_stream->SetAttribute(CWFGM_GRID_ATTRIBUTE_LATITUDE, v))) {
										v = longitude; hr = m_stream->SetAttribute(CWFGM_GRID_ATTRIBUTE_LONGITUDE, v); } return hr; };

		__INLINE bool put_CommonData(ICWFGM_CommonData* data) { return SUCCEEDED(m_stream->put_CommonData(data)); }

		__INLINE HRESULT Initial_FFMC(double ffmc)		{ PolymorphicAttribute v = ffmc; return m_stream->SetAttribute(CWFGM_WEATHER_OPTION_INITIAL_FFMC, v); };
		__INLINE HRESULT Initial_HFFMC(double ffmc)		{ PolymorphicAttribute v = ffmc; return m_stream->SetAttribute(CWFGM_WEATHER_OPTION_INITIAL_HFFMC, v); };
		__INLINE HRESULT Initial_HFFMCTime(const HSS_Time::WTimeSpan ts){ PolymorphicAttribute v = ts; return m_stream->SetAttribute(CWFGM_WEATHER_OPTION_INITIAL_HFFMCTIME, v); };
		__INLINE HRESULT Initial_DC(double dc)			{ PolymorphicAttribute v = dc; return m_stream->SetAttribute(CWFGM_WEATHER_OPTION_INITIAL_DC, v); };
		__INLINE HRESULT Initial_DMC(double dmc)		{ PolymorphicAttribute v = dmc; return m_stream->SetAttribute(CWFGM_WEATHER_OPTION_INITIAL_DMC, v); };
		__INLINE HRESULT Initial_Rain(double rain)		{ PolymorphicAttribute v = rain; return m_stream->SetAttribute(CWFGM_WEATHER_OPTION_INITIAL_RAIN, v); };
		__INLINE double Initial_FFMC()					{ PolymorphicAttribute v; double d; if (SUCCEEDED(m_stream->GetAttribute(CWFGM_WEATHER_OPTION_INITIAL_FFMC, &v))) { VariantToDouble_(v, &d); return d; } return -1.0; };
		__INLINE double Initial_HFFMC()					{ PolymorphicAttribute v; double d; if (SUCCEEDED(m_stream->GetAttribute(CWFGM_WEATHER_OPTION_INITIAL_HFFMC, &v))) { VariantToDouble_(v, &d); return d; } return -1.0; };
		__INLINE double Initial_DC()					{ PolymorphicAttribute v; double d; if (SUCCEEDED(m_stream->GetAttribute(CWFGM_WEATHER_OPTION_INITIAL_DC, &v))) { VariantToDouble_(v, &d); return d; } return -1.0; };
		__INLINE double Initial_DMC()					{ PolymorphicAttribute v; double d; if (SUCCEEDED(m_stream->GetAttribute(CWFGM_WEATHER_OPTION_INITIAL_DMC, &v))) { VariantToDouble_(v, &d); return d; } return -1.0; };
		__INLINE double Initial_Rain()					{ PolymorphicAttribute v; double d; if (SUCCEEDED(m_stream->GetAttribute(CWFGM_WEATHER_OPTION_INITIAL_RAIN, &v))) { VariantToDouble_(v, &d); return d; } return -1.0; };
		__INLINE HSS_Time::WTimeSpan Initial_HFFMCTime()			{ PolymorphicAttribute v; HSS_Time::WTimeSpan d; if (SUCCEEDED(m_stream->GetAttribute(CWFGM_WEATHER_OPTION_INITIAL_HFFMCTIME, &v))) { VariantToTimeSpan_(v, &d); return d; } return HSS_Time::WTimeSpan(-1); };

		__INLINE bool WarnOnSunriseCalc()				{ PolymorphicAttribute v; bool b; if (SUCCEEDED(m_stream->GetAttribute(CWFGM_WEATHER_OPTION_WARNONSUNRISE, &v)))	{ VariantToBoolean_(v, &b); return b; } return false; };
		__INLINE bool WarnOnSunsetCalc()				{ PolymorphicAttribute v; bool b; if (SUCCEEDED(m_stream->GetAttribute(CWFGM_WEATHER_OPTION_WARNONSUNSET, &v)))		{ VariantToBoolean_(v, &b); return b; } return false; };

		__INLINE WeatherOptions GetWeatherOptions() const	{ return WeatherOptions(m_stream.get()); };
		__INLINE bool SetWeatherOptions(const WeatherOptions &wo){return wo.SaveToWeatherStream(m_stream.get()); };

		__INLINE bool IsHourlyObservations(const HSS_Time::WTime &t)	{ return FAILED(m_stream->IsDailyObservations(t)); };
		__INLINE bool IsDailyObservations(const HSS_Time::WTime &t)	{ return SUCCEEDED(m_stream->IsDailyObservations(t)); };
		__INLINE bool IsAnyDailyObservations() const { return SUCCEEDED(m_stream->IsAnyDailyObservations()); };
		__INLINE bool IsAnyImportedFromFile() const { HSS_Time::WTime t(0, nullptr); return SUCCEEDED(m_stream->IsImportedFromFile(t)); };
		__INLINE bool IsAnyImportedFromEnsemble() const { HSS_Time::WTime t(0, nullptr); return SUCCEEDED(m_stream->IsImportedFromEnsemble(t)); };
		__INLINE bool IsAnyModified() const { return SUCCEEDED(m_stream->IsAnyModified()); };
		__INLINE HRESULT MakeHourlyObservations(const HSS_Time::WTime &time){return m_stream->MakeHourlyObservations(time); };
		__INLINE HRESULT MakeDailyObservations(const HSS_Time::WTime &time){ return m_stream->MakeDailyObservations(time); };

		__INLINE HRESULT GetDailyValues(const HSS_Time::WTime &time, double *min_temp, double *max_temp, double *min_ws, double *max_ws, double* min_gust, double* max_gust, double *rh, double *precip, double *wd)
	 								{ return m_stream->GetDailyValues(time, min_temp, max_temp, min_ws, max_ws, min_gust, max_gust, rh, precip, wd); };
		__INLINE HRESULT SetDailyValues(const HSS_Time::WTime &time, double min_temp, double max_temp, double min_ws, double max_ws, double min_gust, double max_gust, double rh, double precip, double wd)
									{ return m_stream->SetDailyValues(time, min_temp, max_temp, min_ws, max_ws, min_gust, max_gust, rh, precip, wd); };
		__INLINE bool GetDailyStandardFFMC(const HSS_Time::WTime &time, double *ffmc)
									{ return SUCCEEDED(m_stream->DailyStandardFFMC(time, ffmc)); };
		__INLINE bool GetInstantaneousValues(const HSS_Time::WTime &time, ULONG interpolation_method, IWXData *wx, IFWIData *ifwi, DFWIData *dfwi)
									{ return SUCCEEDED(m_stream->GetInstantaneousValues(time, interpolation_method, wx, ifwi, dfwi)); };
		__INLINE HRESULT SetInstantaneousValues(const HSS_Time::WTime &time, IWXData *wx)
									{ return m_stream->SetInstantaneousValues(time, wx); };
		__INLINE bool UseSpecifiedFWI() { PolymorphicAttribute v; if (SUCCEEDED(m_stream->GetAttribute(CWFGM_WEATHER_OPTION_FWI_USE_SPECIFIED, &v))) { bool d;  VariantToBoolean_(v, &d); return d; } return false; };

		std::string		m_name,		// name for the CWFGM weather stream object
						m_comments,	// comments for this object
						m_loadWarning;
		boost::intrusive_ptr<CCWFGM_WeatherStream>	m_stream;	// m_fuel's UserData should point at this

		__INLINE HRESULT LockState()				{ return m_stream->MT_Lock(false, (USHORT)-1); };

	private:
		void FillOneDayStream(ICWFGM_FWI *FWI, const HSS_Time::WTime &newTime, FILE *fp,const ExportOptions *exportOptions, SHORT export_hourly, SHORT include_fwi);
		std::string FormStringLine(const HSS_Time::WTime &lt, double temp, double ws, double rh, double precip, double wd, double ffmc, double hffmc, double dmc, double dc, double bui, double isi, double hisi,
			double fwi, double hfwi, const ExportOptions *exportOptions, SHORT include_fwi);

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		WeatherStream *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};


	class WeatherStationCollection;
	class CWFGMPROJECT_EXPORT WeatherStation : public MinNode, public ISerializeProto
	{
		friend WeatherStationCollection;
	protected:
		WeatherStation(const WeatherStationCollection *collection);
		WeatherStation(const WeatherStation &station);

	public:
		~WeatherStation();

		__INLINE WeatherStation *LN_Succ() const					{ return (WeatherStation *)MinNode::LN_Succ(); };
		__INLINE WeatherStation *LN_Pred() const					{ return (WeatherStation *)MinNode::LN_Pred(); };

		virtual WeatherStream *New() const							{ return new WeatherStream(); };
		virtual WeatherStream *New(const WeatherStream &s) const	{ return new WeatherStream(s); };
		virtual WeatherStream *New(CCWFGM_WeatherStream *s) const	{ return new WeatherStream(s); };

		bool AddStream(WeatherStream *stream);
		bool AddStream(WeatherStream *stream, ULONG index);
		HRESULT RemoveStream(WeatherStream *stream);
		__INLINE ULONG GetCount() const								{ return m_streamList.GetCount(); };
		__INLINE WeatherStream *FirstStream() const					{ return m_streamList.LH_Head(); };
		__INLINE WeatherStream *StreamAtIndex(ULONG index) const	{ return m_streamList.IndexNode(index); };
		__INLINE ULONG IndexOf(WeatherStream *stream) const			{ return m_streamList.NodeIndex(stream); };

		double Latitude() const;
		double Longitude() const;
		bool GridLocation(double *x, double *y) const;
		double Elevation() const;

		bool GridLocation(double x, double y);
		HRESULT Elevation(double Elevation);

		__INLINE void SetDisplaySize(ULONG Width)			{ m_displaySize = Width; };
		__INLINE ULONG GetDisplaySize() const				{ if (m_displaySize.has_value()) return m_displaySize.value(); return 3; };

		__INLINE COLORREF GetColor() const					{ if (m_color.has_value()) return m_color.value(); return RGB(0, 0, 0xff); };
		__INLINE COLORREF SetColor(COLORREF c)				{ if (m_color != c) { m_color = c; } return m_color.value(); };

		__INLINE LOGFONT GetFont() const					{ return PolyEditGetWingdingsFont(); }

		__INLINE std::uint64_t GetSymbol() const			{ if (m_symbol.has_value()) return m_symbol.value(); return 0; }
		__INLINE std::uint64_t SetSymbol(std::uint64_t s)	{ m_symbol = s; return m_symbol.value(); }

		WeatherStream *FindName(const TCHAR *name) const;
	
	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		WeatherStation *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

    public:
		std::string		m_name,		// name for the CWFGM fire object
						m_comments;	// comments for this object

		__INLINE HRESULT LockState()					{ return m_station->MT_Lock(false, (USHORT)-1); };

		std::string CollectLoadWarnings();

		const WeatherStationCollection	*m_collection;

		CCWFGM_WeatherStation* station()				{ return m_station.get(); }

    protected:
		boost::intrusive_ptr<CCWFGM_WeatherStation>	m_station;	// m_fuel's UserData should point at this
		MinListTempl<WeatherStream>	m_streamList;
		std::string				m_loadWarning;

		std::optional<COLORREF>			m_color;
		std::optional<ULONG>			m_displaySize;
		std::optional<std::uint64_t>	m_symbol;
	};


	class CWFGMPROJECT_EXPORT WeatherStationCollection : public ISerializeProto
	{
	public:
		const class CWFGMProject *m_project;

		WeatherStationCollection(const CWFGMProject *project);
		WeatherStationCollection();
		virtual ~WeatherStationCollection();

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		WISE::ProjectProto::StationCollection* serialize(const SerializeProtoOptions& options) override;
		WeatherStationCollection *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	public:
		virtual WeatherStation *New(const HSS_Time::WTimeManager *timeManager) const;
		virtual WeatherStation *New(const WeatherStation &s) const;

		HRESULT PCOM_CreateWeatherStation(const char *name, WeatherStation **station);
		HRESULT PCOM_SetWeatherStation(WeatherStation *station, CCWFGM_Grid *grid, double Latitude, double Longitude, double Elevation);

		__INLINE void AddStation(WeatherStation *station)				{ m_stationList.AddTail(station); };
		__INLINE void RemoveStation(WeatherStation *station)			{ m_stationList.Remove(station); };
		__INLINE ULONG GetCount() const									{ return m_stationList.GetCount(); };
		__INLINE WeatherStation *FirstStation() const					{ return m_stationList.LH_Head(); };
		__INLINE WeatherStation *StationAtIndex(ULONG index) const		{ return m_stationList.IndexNode(index); };
		__INLINE ULONG IndexOf(WeatherStation *station) const			{ return m_stationList.NodeIndex(station); };

		WeatherStation *FindName(const TCHAR *name) const;

		bool AssignNewGrid(ICWFGM_GridEngine *grid);

		std::string CollectLoadWarnings();

	protected:
		HRESULT PCOM_ImportStationStream(CCWFGM_Grid *grid, FILE *f);

		MinListTempl<WeatherStation>	m_stationList;
		std::string						m_loadWarning;
	};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEATHERCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
