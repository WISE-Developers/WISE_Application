/**
 * WISE_Project: WeatherGridFilter.h
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

#if !defined(AFX_WEATHERGRIDFILTER_H__7E2F5933_2E4A_4485_8129_ACC2C4A925D6__INCLUDED_)
#define AFX_WEATHERGRIDFILTER_H__7E2F5933_2E4A_4485_8129_ACC2C4A925D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cwfgmp_config.h"

#include "ICWFGM_Fuel.h"
#include "GridCOM.h"
namespace Project
{
	class CWFGMProject;
};
#include "WTime.h"
#include "GridFilter.h"
#include "WeatherCom_ext.h"
#include "poly.h"
#include "ISerializeProto.h"

#include "projectGridFilter.pb.h"

using namespace HSS_Time;

#define WEATHER_FILTER_GRID_TYPE	0
#define WEATHER_FILTER_PATCH_TYPE	1

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 8)
#endif

namespace Project
{
	class CWFGMPROJECT_EXPORT CWeatherGridBase : public GridFilter, public ISerializeProto
	{
	public:
		CWeatherGridBase()					{ };

		CWeatherGridBase* Duplicate() const override;

		bool GetStartTime(WTime &start) { PolymorphicAttribute v; HRESULT hr; if (SUCCEEDED(hr = m_filter->GetAttribute(0, CWFGM_WEATHER_OPTION_START_TIME, &v))) { VariantToTime_(v, &start); } return SUCCEEDED(hr); };
		bool GetEndTime(WTime &EndTime) { PolymorphicAttribute v; HRESULT hr; if (SUCCEEDED(hr = m_filter->GetAttribute(0, CWFGM_WEATHER_OPTION_END_TIME, &v))) { VariantToTime_(v, &EndTime); } return SUCCEEDED(hr); };
		bool GetStartTimeOfDay(WTimeSpan &start)	{ PolymorphicAttribute v; HRESULT hr; if (SUCCEEDED(hr = m_filter->GetAttribute(0, CWFGM_WEATHER_OPTION_START_TIMESPAN, &v))) { VariantToTimeSpan_(v, &start); } return SUCCEEDED(hr); };
		bool GetEndTimeOfDay(WTimeSpan &EndTime)	{ PolymorphicAttribute v; HRESULT hr; if (SUCCEEDED(hr = m_filter->GetAttribute(0, CWFGM_WEATHER_OPTION_END_TIMESPAN, &v))) { VariantToTimeSpan_(v, &EndTime); } return SUCCEEDED(hr); };

		virtual HRESULT SetStartTime(const WTime &time);
		virtual HRESULT SetEndTime(const WTime &time);
		virtual HRESULT SetStartTimeOfDay(const WTimeSpan &time);
		virtual HRESULT SetEndTimeOfDay(const WTimeSpan &time);

	protected:
		void Clone(boost::intrusive_ptr<ICWFGM_GridEngine>* grid) const override;
		CWeatherGridBase* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};


	class CWFGMPROJECT_EXPORT CWeatherGridFilter : public CWeatherGridBase {
	public:
		CWeatherGridFilter();
		CWeatherGridFilter(class CWeatherPolyFilter *filter);
		virtual ~CWeatherGridFilter();

		std::string DisplayName() const override;
		const std::string TypeString() const override { using namespace std::string_literals; return "Weather Grid"s; }

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		CWeatherGridFilter *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};


	class CWFGMPROJECT_EXPORT CWeatherPolyFilter : public CWeatherGridBase
	{
	protected:
		COLORREF	m_color;
		ULONGLONG	m_lineWidth;
		ULONG		m_flags;

	public:
		long m_Type_Depreciated;		// 0 is global weather grid, 1 is patch.

		CWeatherPolyFilter();
		virtual ~CWeatherPolyFilter();

		CWeatherPolyFilter* Duplicate() const override;

		std::string DisplayName() const override { return m_name; }
		const std::string TypeString() const override { using namespace std::string_literals; return "Weather Patch"s; }

		std::uint32_t AddPolygon(const XY_Point &points, std::uint32_t num_points);
		std::uint32_t AddPolygon(const XY_PolyConst &poly)	{ return AddPolygon(*poly.GetPointsArray(), poly.NumPoints()); };
		void SetLandscapeWxFlag(bool newValue) { if(newValue){ m_flags |= 0x1; } else {m_flags &= (~(0x1));} };
		bool GetLandscapeWxFlag() const { return (m_flags & 0x1) ? true : false; }
		bool ClearPolygon(std::uint32_t index);
		std::uint32_t GetPolygonMaxSize();
		std::uint32_t GetPolygonCount();
		std::uint32_t GetPolygonSize(ULONG index);
		XY_Point *GetPolygon(std::uint32_t index, XY_Point *points, std::uint32_t *size);
		virtual double Area();
		bool ClearAllPolygons();

		double GetTemperature();
		double GetRH();
		double GetPrecipitation();
		double GetWindDirection();
		double GetWindSpeed();
		void SetTemperature(double temp);
		void SetRH(double rh);
		void SetPrecipitation(double precip);
		void SetWindDirection(double wd);
		void SetWindSpeed(double ws);

		std::uint16_t GetTemperature_Operation();
		std::uint16_t GetRH_Operation();
		std::uint16_t GetPrecipitation_Operation();
		std::uint16_t GetWD_Operation();
		std::uint16_t GetWS_Operation();
		void SetTemperature_Operation(std::uint16_t op);
		void SetRH_Operation(std::uint16_t op);
		void SetPrecipitation_Operation(std::uint16_t op);
		void SetWD_Operation(std::uint16_t op);
		void SetWS_Operation(std::uint16_t op);

		HRESULT ImportPolygons(const std::filesystem::path &file_name, const std::vector<std::string_view> &permissible_drivers);
		HRESULT ImportPolygonsWFS(const std::string &server, const std::string &layer, const std::string &uid, const std::string &pwd);
		HRESULT ExportPolygons(std::string_view driver_name, const std::string &export_projection, const std::filesystem::path &file_name);

		void SetLineWidth(ULONG Width)						{ m_lineWidth = Width; };
		ULONG GetLineWidth()								{ return m_lineWidth; };
		virtual ULONG BoundaryWidth() const noexcept override		{ return m_lineWidth; };

		virtual COLORREF BoundaryColor() const noexcept	override	{ return m_color; };
		COLORREF SetColor(COLORREF color)					{ m_color = color; return m_color; };

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual CWeatherPolyFilter *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	protected:
		CWeatherPolyFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};
};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#endif // !defined(AFX_WEATHERGRIDFILTER_H__7E2F5933_2E4A_4485_8129_ACC2C4A925D6__INCLUDED_)
