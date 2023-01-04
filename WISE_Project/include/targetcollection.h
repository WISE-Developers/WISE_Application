/**
 * WISE_Project: targetcollection.h
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

#ifndef __TARGETCOLLECTION_H
#define __TARGETCOLLECTION_H

#include "cwfgmp_config.h"
#include "ICWFGM_GridEngine.h"
#include "ICWFGM_Target.h"
#include "linklist.h"
#include "poly.h"
#include "FontHelper.h"
#include "ISerializeProto.h"
#include "intrusive_ptr.h"

#include "projectGridFilter.pb.h"
#include "assetCollection.pb.h"

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 8)
#endif

namespace Project
{
	class CWFGMPROJECT_EXPORT Target : public SNode, public ISerializeProto {
		friend class TargetCollection;

	protected:
		std::string						m_comments;
		std::optional<COLORREF>			m_col;
		std::optional<long>				m_displaySize;
		std::optional<bool>				m_imported;
		std::optional<std::uint64_t>	m_symbol;

	public:
		Target();
		virtual ~Target();
		virtual Target* Duplicate() const;

		virtual void SetDisplaySize(long Width)				{ m_displaySize = Width; };
		virtual int GetDisplaySize() const					{ if (m_displaySize.has_value()) return m_displaySize.value(); return 1; };

		Target* LN_Succ() const								{ return (Target*)SNode::LN_Succ(); };
		Target* LN_Pred() const								{ return (Target*)SNode::LN_Pred(); };

		const std::string& Name(const std::string& name)	{ ln_Name = name; return ln_Name; }
		const std::string& Name() const noexcept	{ return ln_Name; }

		virtual COLORREF SetColor(COLORREF color)			{ m_col = color; return m_col.value(); };
		virtual COLORREF GetColor() const					{ if (m_col.has_value()) return m_col.value(); return RGB(0, 0, 0); };

		const std::string& Comments(const std::string& comments)	{ m_comments = comments; return m_comments; }
		const std::string& Comments(std::string&& comments)		{ m_comments = std::move(comments); return m_comments; }
		const std::string& Comments() const noexcept				{ return m_comments; }

		bool IsImported() const					{ if (m_imported.has_value()) return m_imported.value(); return false; };
		bool IsImported(bool imported)				{ m_imported = imported; return m_imported.value(); };

		LOGFONT GetFont() const					{ return PolyEditGetWingdingsFont(); }

		std::uint64_t GetSymbol() const			{ if (m_symbol.has_value()) return m_symbol.value(); return 0; }
		std::uint64_t SetSymbol(std::uint64_t s)	{ m_symbol = s; return m_symbol.value(); }

		HRESULT LockState() { if (m_target) return m_target->MT_Lock(false, (USHORT)-1); return S_OK; };

		ICWFGM_GridEngine* gridEngine(ICWFGM_GridEngine* gridEngine);
		std::string m_loadWarning;

		ULONG AddPointSet(const XY_Point* points, ULONG num_points);
		ULONG SetPointSet(ULONG index, const XY_Point* points, ULONG num_points);
		ULONG AddPointSet(const XY_PolyConst& poly) { return AddPointSet(poly.GetPointsArray(), poly.NumPoints()); };
		ULONG SetPointSet(ULONG index, const XY_PolyConst& poly) { return SetPointSet(index, poly.GetPointsArray(), poly.NumPoints()); };
		BOOL ClearPointSet(ULONG index);
		ULONG GetTargetMaxSize();
		ULONG GetTargetTotalSize();
		ULONG GetTargetCount();
		ULONG GetTargetSize(ULONG index);
		XY_Point* GetTargetSet(ULONG index, XY_Point* points, std::uint32_t* size);
		BOOL ClearPointSets();

		HRESULT ImportPointSet(const std::string& file_name, const std::vector<std::string>& pd);
		HRESULT ExportPointSet(const std::string& driver_name, const std::string& export_projection, const std::string& file_name);

	protected:
		virtual void Clone(boost::intrusive_ptr<ICWFGM_Target>* /*target*/) const;

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual Target* deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		virtual std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

		boost::intrusive_ptr<ICWFGM_Target>	m_target;		// the filter represented by this object
	};


	class CWFGMPROJECT_EXPORT TargetCollection : public ISerializeProto {
	protected:
		::SList				m_targetList;
		std::string			m_loadWarning;

	public:
		const class CWFGMProject* m_project;

		TargetCollection(const CWFGMProject* project) : m_project(project) { };
		~TargetCollection();

		void AddTarget(Target* target) { m_targetList.AddTail(target); };
		void InsertTarget(Target* target, ULONG index) { m_targetList.InsertIndex(target, index); };
		void RemoveTarget(Target* target) { m_targetList.Remove(target); };
		ULONG GetCount() const { return m_targetList.GetCount(); };
		Target* FirstTarget() const { return (Target*)m_targetList.LH_Head(); };
		Target* TargetAtIndex(ULONG index) const { return (Target*)m_targetList.IndexNode(index); };
		ULONG IndexOf(Target* vector) const { return m_targetList.NodeIndex(vector); };
		ULONG IndexOf(ICWFGM_Target* target) const;
		Target* FindName(const TCHAR* name) const { return (Target*)m_targetList.FindName(name); };

		BOOL AssignNewGrid(ICWFGM_GridEngine* grid);

		std::string CollectLoadWarnings();

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual TargetCollection* deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		virtual std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};

};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#endif
