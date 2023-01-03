/**
 * WISE_Project: vectorcollection.h
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

#ifndef __VECTORCOLLECTION_H
#define __VECTORCOLLECTION_H

#include "cwfgmp_config.h"
#include "ICWFGM_Fuel.h"
#include "ICWFGM_VectorEngine.h"
#include "linklist.h"
#include "poly.h"
#include "FontHelper.h"
#include "ISerializeProto.h"
#include "intrusive_ptr.h"

#include "projectGridFilter.pb.h"
#include "vectorCollection.pb.h"


#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 8)
#endif

namespace Project
{
	class CWFGMPROJECT_EXPORT Vector : public Node
	{
		friend class VectorCollection;	
		protected:
		std::string			m_comments;
		std::optional<COLORREF>			m_col;
		std::optional<COLORREF>			m_fillColor;
		std::optional<long>				m_LineWidth;
		std::optional<bool>				m_imported;
		std::optional<std::uint64_t>	m_symbol;

	public:
		virtual ~Vector();
		virtual Vector *Duplicate() const					{ weak_assert(false); return NULL; }		// this should always be overridden

		virtual void SetLineWidth(long Width)				{ m_LineWidth = Width; }
		virtual int GetLineWidth() const					{ if (m_LineWidth.has_value()) return m_LineWidth.value(); return 1; }

		virtual double GetPolyLineWidth()					{ return 1.0; }	// in grid units
		virtual void SetPolyLineWidth(double /*width*/)		{ }				// in grid units

		__INLINE Vector *LN_Succ() const					{ return (Vector *)Node::LN_Succ(); }
		__INLINE Vector *LN_Pred() const					{ return (Vector *)Node::LN_Pred(); }

		virtual void PostSerialize(class CWFGMProject* /*project*/) { }

		const TCHAR *Name(const TCHAR *name);
		__INLINE const TCHAR *Name() const					{ return ln_Name; }

		virtual COLORREF SetColor(COLORREF color)			{ m_col = color; return m_col.value(); }
		virtual COLORREF GetColor() const					{ if (m_col.has_value()) return m_col.value(); return RGB(0, 0, 0); };

		__INLINE COLORREF GetFillColor() const				{ if (m_fillColor.has_value()) return m_fillColor.value(); return RGB(0, 0, 0); }
		__INLINE void SetFillColor(COLORREF color)			{ m_fillColor = color; }

		__INLINE const std::string &Comments(const std::string &comments)	{ m_comments = comments; return m_comments; }
		__INLINE const std::string &Comments() const		{ return m_comments; }

		__INLINE bool IsImported() const					{ if (m_imported.has_value()) return m_imported.value(); return false; }
		__INLINE bool IsImported(bool imported)				{ m_imported = imported; return m_imported.value(); }

		__INLINE LOGFONT GetFont() const					{ return PolyEditGetWingdingsFont(); }

		__INLINE std::uint64_t GetSymbol() const			{ if (m_symbol.has_value()) return m_symbol.value(); return 0; }
		__INLINE std::uint64_t SetSymbol(std::uint64_t s)	{ m_symbol = s; return m_symbol.value(); }

		__INLINE HRESULT LockState()						{ if (m_filter) return m_filter->MT_Lock(false, (USHORT)-1); return S_OK; }

		ICWFGM_GridEngine *gridEngine(ICWFGM_GridEngine *gridEngine);
		std::string m_loadWarning;

	protected:
		Vector();

	public:
		boost::intrusive_ptr<ICWFGM_VectorEngine>	m_filter;		// the filter represented by this object
	};
};


namespace Project
{
	class CWFGMPROJECT_EXPORT StaticVector : public Vector, public ISerializeProto
	{
		friend class VectorCollection;
		friend class StaticVectorHelper;

	public:
		StaticVector();
		StaticVector(const StaticVector &toCopy);
		virtual ~StaticVector();
		virtual Vector *Duplicate() const;

		ULONG AddPolyLine(const XY_Point *points, ULONG num_points, bool is_polygon);
		ULONG SetPolyLine(ULONG index, const XY_Point *points, ULONG num_points, bool is_polygon);
		__INLINE ULONG AddPolyLine(const XY_PolyConst &poly, bool is_polygon)		{ return AddPolyLine(poly.GetPointsArray(), poly.NumPoints(), is_polygon); };
		__INLINE ULONG SetPolyline(ULONG index, const XY_PolyConst &poly, bool is_polygon)	{ return SetPolyLine(index, poly.GetPointsArray(), poly.NumPoints(), is_polygon); };
		bool ClearPolyLine(ULONG index);
		ULONG GetPolyLineMaxSize();
		ULONG GetPolyLineTotalSize();
		ULONG GetPolyLineCount();
		ULONG GetPolyLineSize(ULONG index);
		XY_Point *GetPolyLine(ULONG index, XY_Point *points, std::uint32_t *size, bool *is_polygon);
		bool ClearAllPolyLines();
		bool IsPolygon(ULONG index);

		virtual double GetPolyLineWidth();			// in grid units
		virtual void SetPolyLineWidth(double width);		// in grid units

		HRESULT ImportPolylines(const std::string &file_name, const std::vector<std::string> *pd);
		HRESULT ImportPolylinesWFS(const std::string &server, const std::string &layer, const std::string &uid, const std::string &pwd);
		HRESULT ExportPolylines(const std::string &driver_name, const std::string &export_projection, const std::string &file_name);

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		StaticVector *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};
};


namespace Project
{
	class CWFGMPROJECT_EXPORT VectorCollection : public ISerializeProto
	{
	protected:
		List m_vectorList;
		boost::intrusive_ptr<ICWFGM_GridEngine>	m_gridEngine;
		std::string				m_loadWarning;

	public:
		const class CWFGMProject *m_project;

		VectorCollection(const CWFGMProject *project) : m_project(project) { };
		~VectorCollection();

		void PostSerialize(class CWFGMProject *project);

		__INLINE void AddVector(Vector *vector)				{ m_vectorList.AddTail(vector); };
		__INLINE void InsertVector(Vector *vector, ULONG index)	{ m_vectorList.InsertIndex(vector, index); };
		__INLINE void RemoveVector(Vector *vector)			{ m_vectorList.Remove(vector); };
		__INLINE ULONG GetCount() const						{ return m_vectorList.GetCount(); };
		__INLINE Vector *FirstVector() const				{ return (Vector *)m_vectorList.LH_Head(); };
		__INLINE Vector *VectorAtIndex(ULONG index) const	{ return (Vector *)m_vectorList.IndexNode(index); };
		__INLINE ULONG IndexOf(Vector *vector) const		{ return m_vectorList.NodeIndex(vector); };
		__INLINE Vector *FindName(const TCHAR *name) const	{ return (Vector *)m_vectorList.FindName(name); };

		bool AssignNewGrid(ICWFGM_GridEngine *grid);
		void AdjustVectorWidths();

		std::string CollectLoadWarnings();

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		VectorCollection *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};
};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#endif
