/**
 * WISE_Project: FireCollection.h
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

#if !defined(AFX_FIRECOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
#define AFX_FIRECOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cwfgmp_config.h"
#include "StdFireCollection.h"
#include "FontHelper.h"
#include "ISerializeProto.h"
#include "projectIgnition.pb.h"

#include <gsl/util>

namespace Project
{
	class FireCollection;
	class CWFGMPROJECT_EXPORT Fire : public StdFire, public ISerializeProto
	{
		friend FireCollection;
		std::optional<COLORREF>			m_color;
		std::optional<COLORREF>			m_fillColor;
		std::optional<std::uint32_t>	m_lineWidth;
		std::optional<bool>				m_imported;
		std::optional<std::uint64_t>	m_symbol;

	public:
		Fire(const FireCollection &fireCollection);
		Fire(const FireCollection &fireCollection, const Fire &fire);
		Fire(const FireCollection &fireCollection, boost::intrusive_ptr<CCWFGM_Ignition> &fire);
		virtual ~Fire();

		__INLINE Fire *LN_Succ() const								{ return (Fire *)StdFire::LN_Succ(); };
		__INLINE Fire *LN_Pred() const								{ return (Fire *)StdFire::LN_Pred(); };

		bool operator==(const Fire &toCompare) const				{ if (&toCompare == this) return true; return m_fire == toCompare.m_fire; };

		__INLINE void SetLineWidth(std::uint32_t Width) noexcept	{ m_lineWidth = Width; };
		__INLINE std::uint32_t GetLineWidth() const noexcept		{ if (m_lineWidth.has_value()) return m_lineWidth.value(); return 3; };

		__INLINE COLORREF GetColor() const noexcept					{ if (m_color.has_value()) return m_color.value(); return RGB(0xff, 0, 0); };
		__INLINE COLORREF SetColor(COLORREF c) noexcept				{ if (m_color != c) { m_color = c; } return m_color.value(); };

		__INLINE COLORREF GetFillColor() const noexcept				{ if (m_fillColor.has_value()) return m_fillColor.value(); return RGB(0xff, 0, 0); }
		__INLINE void SetFillColor(COLORREF color) noexcept			{ m_fillColor = color; }

		__INLINE bool IsImported() const noexcept					{ if (m_imported.has_value()) return m_imported.value(); return false; };
		__INLINE bool IsImported(bool imported) noexcept			{ m_imported = imported; return m_imported.value(); };

		__INLINE LOGFONT GetFont() const							{ return PolyEditGetWingdingsFont(); };

		__INLINE std::uint64_t GetSymbol() const noexcept			{ if (m_symbol.has_value()) return m_symbol.value(); return 0; }
		__INLINE std::uint64_t SetSymbol(std::uint64_t s) noexcept	{ m_symbol = s; return m_symbol.value(); }

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual Fire *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};
};


namespace Project
{
	class CWFGMPROJECT_EXPORT FireCollection : public StdFireCollection, public ISerializeProto
	{
	public:
		FireCollection(const class CWFGMProject *project) : StdFireCollection(project)
															{ };
		virtual ~FireCollection()							{ };

		__INLINE Fire *FirstFire() const					{ return (Fire *)m_fireList.LH_Head(); };
		__INLINE Fire *LastFire() const						{ return (Fire *)m_fireList.LH_Tail(); };
		__INLINE Fire *FireAtIndex(ULONG index) const		{ return (Fire *)m_fireList.IndexNode(index); };

		__INLINE Fire *FindName(const std::string &name) const		{ return dynamic_cast<Fire *>(StdFireCollection::FindName(name)); };

		std::string CollectLoadWarnings();

	private:
		std::string m_loadWarnings;

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual FireCollection *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIRECOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
