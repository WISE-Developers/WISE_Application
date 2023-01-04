/**
 * WISE_Project: viewstorage.h
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

#ifndef __VIEWSTORAGE_H
#define __VIEWSTORAGE_H

#include "cwfgmp_config.h"

#if _DLL
#include <afx.h>
#else
#include "types.h"
#include "BaseObject.h"
#ifdef _MSC_VER
#include <Windows.h>
#endif
#endif

#include "linklist.h"
#include "ISerializeProto.h"
#include "viewStorage.pb.h"
#include "hssconfig/config.h"

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 4)
#endif

namespace Project
{
	class CWFGMPROJECT_EXPORT ViewStateNode : public MinNode, public ISerializeProto
	{
	public:
		ViewStateNode();
		ViewStateNode(const ViewStateNode *toCopy)	{ m_pViewSpecific = NULL; *this = *toCopy; m_nID = 0; };
		ViewStateNode(const TCHAR *group_name);

		~ViewStateNode()				{ if (m_pViewSpecific) delete [] m_pViewSpecific; };

		ViewStateNode *LN_Succ() const		{ return (ViewStateNode *)MinNode::LN_Succ(); };
		ViewStateNode *LN_Pred() const		{ return (ViewStateNode *)MinNode::LN_Pred(); };

		void SaveToIniFile(const TCHAR *group_name) const;

		const ViewStateNode &operator=(const ViewStateNode &toCopy);

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual ViewStateNode *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		virtual std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	public:
#if _DLL
		const CRuntimeClass	*m_pViewClass;
#else
		const HSS_Types::RuntimeClass* m_pViewClass;
#endif
		std::string		m_viewClassName;
		WINDOWPLACEMENT	m_windowPlacement;
		unsigned char	*m_pViewSpecific;
		std::uint32_t	m_sViewSpecific;
		std::uint32_t	m_zOrderIndex;
		std::uint32_t	m_nID;
		bool			m_windowPlacementSet;
	};


	class CWFGMPROJECT_EXPORT ViewCollection : ISerializeProto
	{
	protected:
		MinListTempl<ViewStateNode>	m_viewList;

		public:
		ViewCollection() = default;
		ViewCollection(const ViewCollection *toCopy);
		ViewCollection(const TCHAR *group_name);

		~ViewCollection();

		void AddView(ViewStateNode *node);
		std::uint32_t GetViewCount() const	{ return m_viewList.GetCount(); };
		ViewStateNode *FirstView() const	{ return (ViewStateNode *)m_viewList.LH_Head(); };
		ViewStateNode *LastView() const	{ return (ViewStateNode *)m_viewList.LH_Tail(); };
		void RemoveViews()					{ ViewStateNode *vn; while ((vn = (ViewStateNode *)m_viewList.RemHead()) != NULL) delete vn; };

		void SaveToIniFile(const TCHAR *group_name) const;

		const ViewCollection &operator=(const ViewCollection &toCopy);

		std::string				m_loadWarning;

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual ViewCollection *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		virtual std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};
};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#endif
