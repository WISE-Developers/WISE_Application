/**
 * WISE_Project: assetcollection.h
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

#ifndef __ASSETCOLLECTION_H
#define __ASSETCOLLECTION_H

#include "cwfgmp_config.h"
#include "ICWFGM_GridEngine.h"
#include "ICWFGM_Asset.h"
#include "linklist.h"
#include "poly.h"
#include "FontHelper.h"
#include "ISerializeProto.h"
#include "intrusive_ptr.h"

#include <gsl/util>

#include "projectGridFilter.pb.h"
#include "assetCollection.pb.h"

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 8)
#endif

namespace Project
{
	class CWFGMPROJECT_EXPORT Asset : public SNode, public ISerializeProto
	{
		friend class AssetCollection;

	protected:
		std::string		m_comments;
		std::optional<COLORREF>			m_col;
		std::optional<COLORREF>			m_fillColor;
		std::optional<long>				m_LineWidth;
		std::optional<bool>				m_imported;
		std::optional<std::uint64_t>	m_symbol;

	public:
		Asset();
		virtual ~Asset();
		virtual Asset* Duplicate() const;

		virtual void SetLineWidth(long Width)								{ m_LineWidth = Width; }
		virtual int GetLineWidth() const									{ if (m_LineWidth.has_value()) return m_LineWidth.value(); return 1; }

		Asset* LN_Succ() const										{ return (Asset*)SNode::LN_Succ(); }
		Asset* LN_Pred() const										{ return (Asset*)SNode::LN_Pred(); }

		const std::string& Name(const std::string& name)					{ ln_Name = name; return ln_Name; }
		const std::string& Name() const noexcept					{ return ln_Name; }

		virtual COLORREF SetColor(COLORREF color) noexcept					{ m_col = color; return m_col.value(); }
		virtual COLORREF GetColor() const noexcept							{ if (m_col.has_value()) return m_col.value(); return RGB(0, 0, 0); }

		COLORREF GetFillColor() const noexcept						{ if (m_fillColor.has_value()) return m_fillColor.value(); return RGB(0, 0, 0); }
		void SetFillColor(COLORREF color) noexcept					{ m_fillColor = color; }

		const std::string& Comments(const std::string& comments)	{ m_comments = comments; return m_comments; }
		const std::string& Comments(std::string&& comments)		{ m_comments = std::move(comments); return m_comments; }
		const std::string& Comments() const noexcept				{ return m_comments; }

		bool IsImported() const noexcept							{ if (m_imported.has_value()) return m_imported.value(); return false; }
		bool IsImported(bool imported) noexcept					{ m_imported = imported; return m_imported.value(); }

		LOGFONT GetFont() const									{ return PolyEditGetWingdingsFont(); }

		std::uint64_t GetSymbol() const noexcept					{ if (m_symbol.has_value()) return m_symbol.value(); return (std::uint64_t)0; }
		std::uint64_t SetSymbol(std::uint64_t s) noexcept			{ m_symbol = s; return m_symbol.value(); }

		HRESULT LockState()										{ if (m_filter) return m_filter->MT_Lock(false, gsl::narrow_cast<USHORT>(-1)); return S_OK; }

		ICWFGM_GridEngine* gridEngine(ICWFGM_GridEngine* gridEngine);
		std::string m_loadWarning;

		std::uint32_t AddPoly(const XY_Point* points, std::uint32_t num_points, std::uint16_t interpret);
		std::uint32_t SetPoly(std::uint32_t index, const XY_Point* points, std::uint32_t num_points, std::uint16_t interpret);
		ULONG AddPoly(const XY_PolyConst& poly, std::uint16_t interpret) { return AddPoly(poly.GetPointsArray(), poly.NumPoints(), interpret); };
		ULONG SetPoly(ULONG index, const XY_PolyConst& poly, std::uint16_t interpret) { return SetPoly(index, poly.GetPointsArray(), poly.NumPoints(), interpret); };
		bool ClearPoly(std::uint32_t index);
		std::uint32_t GetPolyMaxSize();
		std::uint32_t GetPolyTotalSize();
		std::uint32_t GetPolyCount();
		std::uint32_t GetPolySize(std::uint32_t index);
		XY_Point* GetPoly(std::uint32_t index, XY_Point* points, std::uint32_t* size, std::uint16_t* interpret);
		bool ClearAllPolys();
		bool IsPolygon(std::uint32_t index);

		virtual double GetPolyLineWidth();			// in grid units
		virtual void SetPolyLineWidth(double width);		// in grid units

		HRESULT ImportPolylines(const std::filesystem::path& file_name, const std::vector<std::string_view>& pd);
		HRESULT ExportPolylines(std::string_view driver_name, const std::string& export_projection, const std::filesystem::path& file_name);

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual Asset* deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	protected:
		virtual void Clone(boost::intrusive_ptr<ICWFGM_Asset>* /*asset*/) const;

	public:
		boost::intrusive_ptr<ICWFGM_Asset>	m_filter;		// the filter represented by this object
	};


	class CWFGMPROJECT_EXPORT AssetCollection : public ISerializeProto
	{
	protected:
		::SList m_assetList;
		std::string				m_loadWarning;

	public:
		const class CWFGMProject* m_project;

		AssetCollection(const CWFGMProject* project) : m_project(project) { }
		virtual ~AssetCollection();

		void AddAsset(Asset* asset) { m_assetList.AddTail(asset); }
		void InsertAsset(Asset* asset, std::uint32_t index) { m_assetList.InsertIndex(asset, index); }
		void RemoveAsset(Asset* asset) { m_assetList.Remove(asset); }
		std::uint32_t GetCount() const { return m_assetList.GetCount(); }
		Asset* FirstAsset() const { return (Asset*)m_assetList.LH_Head(); }
		Asset* AssetAtIndex(std::uint32_t index) const { return (Asset*)m_assetList.IndexNode(index); }
		std::uint32_t IndexOf(const Asset* vector) const { return m_assetList.NodeIndex(vector); }
		Asset* FindName(const std::string& name) const { return (Asset*)m_assetList.FindName(name); }

		bool AssignNewGrid(ICWFGM_GridEngine* grid);

		std::string CollectLoadWarnings();

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual AssetCollection* deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};

};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#endif