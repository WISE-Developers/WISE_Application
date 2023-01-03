/**
 * WISE_Project: ReplaceGridFilter.h
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

#ifndef _REPLACE_GRID_FILTER_H_
#define _REPLACE_GRID_FILTER_H_

#include "cwfgmp_config.h"

#include "ICWFGM_Fuel.h"
#include "GridCOM.h"
#include "poly.h"

#include "GridFilter.h"
#include "ISerializeProto.h"

#include "projectGridFilter.pb.h"


namespace Project
{
	class ReplaceGridBaseData : public ISerializationData
	{
	public:
		const class CWFGMProject *m_project;
	};


	class CWFGMPROJECT_EXPORT ReplaceGridBase : public GridFilter, public ISerializeProto
	{
	public:
		ReplaceGridBase() noexcept;

		ReplaceGridBase* Duplicate() const override;

		virtual HRESULT SetRelationship(ICWFGM_Fuel *from_fuel, UCHAR from_index, ICWFGM_Fuel *to_fuel) = 0;
		virtual HRESULT GetRelationship(ICWFGM_Fuel **from_fuel, UCHAR *from_index, ICWFGM_Fuel **to_fuel) const = 0;

		void SetFuelMap(CCWFGM_FuelMap *fuelMap) override	{ m_fuelMap = fuelMap; };

		virtual double Area() = 0;

	protected:
		ReplaceGridBase* Duplicate(std::function<GridFilter*(void)> constructor) const override;

	protected:
		boost::intrusive_ptr<CCWFGM_FuelMap> m_fuelMap;

	public:
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual ReplaceGridBase *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		virtual ReplaceGridBase *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name, ISerializationData* data) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	protected:
		static constexpr int serialVersionUid = 1;
	};


	class CWFGMPROJECT_EXPORT ReplaceGridFilter : public ReplaceGridBase
	{
	public:
		ReplaceGridFilter();

		std::string DisplayName() const override;
		const std::string TypeString() const override;

		HRESULT SetRelationship(ICWFGM_Fuel *from_fuel, UCHAR from_index, ICWFGM_Fuel *to_fuel) override;
		HRESULT GetRelationship(ICWFGM_Fuel **from_fuel, UCHAR *from_index, ICWFGM_Fuel **to_fuel) const override;

		bool SetArea(USHORT x1, USHORT y1, USHORT x2, USHORT y2);
		bool GetArea(USHORT *x1, USHORT *y1, USHORT *x2, USHORT *y2) const;

		COLORREF BoundaryColor() const noexcept override	{ return gsl::narrow_cast<COLORREF>(-1); };	// no boundary color

		virtual double Area();

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual ReplaceGridFilter *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		virtual ReplaceGridFilter *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name, ISerializationData* data) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	protected:
		void Clone(boost::intrusive_ptr<ICWFGM_GridEngine> *grid) const override;
	};


	class CWFGMPROJECT_EXPORT PolyReplaceGridFilter : public ReplaceGridBase
	{
		std::optional<COLORREF> m_color;
		std::optional<ULONG>	m_lineWidth;

	public:
		PolyReplaceGridFilter();
		~PolyReplaceGridFilter();

		PolyReplaceGridFilter* Duplicate() const override;

		std::string DisplayName() const override;
		const std::string TypeString() const override		{ using namespace std::string_literals; return "Polygon Patch"s; }

		ULONG AddPolygon(const XY_Point &points, ULONG num_points);
		ULONG AddPolygon(const XY_PolyConst &poly)	{ return AddPolygon(*poly.GetPointsArray(), poly.NumPoints()); };
		bool ClearPolygon(ULONG index);
		ULONG GetPolygonMaxSize();
		ULONG GetPolygonCount();
		ULONG GetPolygonSize(ULONG index);
		XY_Point *GetPolygon(ULONG index, XY_Point *points, std::uint32_t *size);
		double Area() override;
		bool ClearAllPolygons();

		HRESULT ImportPolygons(const std::string &file_name, const std::vector<std::string> *pd);
		HRESULT ImportPolygonsWFS(const std::string &server, const std::string &layer, const std::string &uid, const std::string &pwd);
		HRESULT ExportPolygons(const std::string &driver_name, const std::string &export_projection, const std::string &file_name);

		HRESULT SetRelationship(ICWFGM_Fuel *from_fuel, UCHAR from_index, ICWFGM_Fuel *to_fuel) override;
		HRESULT GetRelationship(ICWFGM_Fuel **from_fuel, UCHAR *from_index, ICWFGM_Fuel **to_fuel) const override;

		void SetLineWidth(ULONG Width)	noexcept	{ m_lineWidth = Width; };
		ULONG GetLineWidth() noexcept				{ if (m_lineWidth.has_value()) return m_lineWidth.value(); return 1; };
		ULONG BoundaryWidth() const noexcept override		{ if (m_lineWidth.has_value()) return m_lineWidth.value(); return 1; };

		COLORREF BoundaryColor() const noexcept override	{ if (m_color.has_value()) return m_color.value(); return RGB(0, 0, 0); };
		COLORREF SetColor(COLORREF c) noexcept		{ m_color = c; return m_color.value(); };

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual PolyReplaceGridFilter *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		virtual PolyReplaceGridFilter *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name, ISerializationData* data) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	protected:
		void Clone(boost::intrusive_ptr<ICWFGM_GridEngine> *grid) const override;
		PolyReplaceGridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};
};

#endif
