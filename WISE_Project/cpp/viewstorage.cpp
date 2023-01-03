/**
 * WISE_Project: viewstorage.cpp
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

#include "viewstorage.h"
#include "str_printf.h"
#if _DLL
#include "CWFGMProject.h"
#else
#undef GetProfileInt
#undef WriteProfileString
#undef GetClassName
#include "AfxIniSettings.h"
#endif

#include <gsl/util>

#define VIEWCOLLECTION_BINARY_VERSION	1


Project::ViewStateNode::ViewStateNode()
{
	m_pViewClass = NULL;
	m_windowPlacement.length = sizeof(WINDOWPLACEMENT);
	m_windowPlacementSet = false;
	m_sViewSpecific = 0;
	m_pViewSpecific = NULL;
	m_zOrderIndex = (unsigned int)-1;
}


Project::ViewStateNode::ViewStateNode(const TCHAR *group_name)
{
#if _DLL
	CString str = AfxGetApp()->GetProfileString(group_name, _T("View Class"), _T(""));
#else
	std::string str = AfxGetApp()->GetProfileString(group_name, _T("View Class"), _T(""));
#endif

#if _DLL
	if (Project::CWFGMProject::m_appMode > 0)
		m_pViewClass = ((CWinAppProject *)AfxGetApp())->findClass(str);
	else
#endif
		m_pViewClass = nullptr;

	m_viewClassName = str;

	RECT rect;
	rect.left = AfxGetApp()->GetProfileInt(group_name, _T("WindowPlacement.Left"), 100);
	rect.top = AfxGetApp()->GetProfileInt(group_name, _T("WindowPlacement.Top"), 100);
	rect.right = AfxGetApp()->GetProfileInt(group_name, _T("WindowPlacement.Right"), 500);
	rect.bottom = AfxGetApp()->GetProfileInt(group_name, _T("WindowPlacement.Bottom"), 400);
	m_windowPlacement.rcNormalPosition = rect;

	m_windowPlacement.flags = AfxGetApp()->GetProfileInt(group_name, _T("WindowPlacement.Flag"), 0);
	m_windowPlacement.showCmd = AfxGetApp()->GetProfileInt(group_name, _T("WindowPlacement.ShowCmd"), SW_SHOW);

	m_zOrderIndex = AfxGetApp()->GetProfileInt(group_name, _T("WindowPlacement.ZOrder"), -1);

	m_nID = AfxGetApp()->GetProfileInt(group_name, _T("ID"), 0);

	m_windowPlacement.ptMinPosition = { 0, 0 };
	m_windowPlacement.ptMaxPosition = { -GetSystemMetrics(SM_CXBORDER), -GetSystemMetrics(SM_CYBORDER) };

	AfxGetApp()->GetProfileBinary(group_name, _T("Binary Settings"), &m_pViewSpecific, &m_sViewSpecific);

	m_windowPlacementSet = true;
}


void Project::ViewStateNode::SaveToIniFile(const TCHAR *group_name) const
{
	if (m_pViewClass)
#if _DLL
		AfxGetApp()->WriteProfileString(group_name, _T("View Class"), m_pViewClass->m_lpszClassName);
#else
        AfxGetApp()->WriteProfileString(group_name, _T("View Class"), m_pViewClass->GetClassName());
#endif

	AfxGetApp()->WriteProfileInt(group_name, _T("WindowPlacement.Flag"), m_windowPlacement.flags);
	AfxGetApp()->WriteProfileInt(group_name, _T("WindowPlacement.ShowCmd"), m_windowPlacement.showCmd);

	AfxGetApp()->WriteProfileInt(group_name, _T("WindowPlacement.Left"), m_windowPlacement.rcNormalPosition.left);
	AfxGetApp()->WriteProfileInt(group_name, _T("WindowPlacement.Top"), m_windowPlacement.rcNormalPosition.top);
	AfxGetApp()->WriteProfileInt(group_name, _T("WindowPlacement.Right"), m_windowPlacement.rcNormalPosition.right);
	AfxGetApp()->WriteProfileInt(group_name, _T("WindowPlacement.Bottom"), m_windowPlacement.rcNormalPosition.bottom);

	AfxGetApp()->WriteProfileInt(group_name, _T("WindowPlacement.ZOrder"), m_zOrderIndex);

	AfxGetApp()->WriteProfileInt(group_name, _T("ID"), m_nID);

	if (m_pViewSpecific)
		AfxGetApp()->WriteProfileBinary(group_name, _T("Binary Settings"), m_pViewSpecific, m_sViewSpecific);
}


auto Project::ViewStateNode::operator=(const ViewStateNode &toCopy) -> const ViewStateNode&
{
	if (&toCopy != this)
	{
		m_pViewClass = toCopy.m_pViewClass;
		m_windowPlacement = toCopy.m_windowPlacement;
		m_windowPlacementSet = toCopy.m_windowPlacementSet;
		if (m_pViewSpecific)
			delete[] m_pViewSpecific;
		if (toCopy.m_pViewSpecific)
		{
			m_pViewSpecific = new unsigned char[toCopy.m_sViewSpecific];
			memcpy(m_pViewSpecific, toCopy.m_pViewSpecific, toCopy.m_sViewSpecific);
			m_sViewSpecific = toCopy.m_sViewSpecific;
		}
		else
		{
			m_pViewSpecific = NULL;
			m_sViewSpecific = 0;
		}
	}
	return *this;
}


Project::ViewCollection::ViewCollection(const ViewCollection *toCopy)
{
	if (toCopy)
		*this = *toCopy;
}


auto Project::ViewCollection::operator=(const ViewCollection &toCopy) -> const ViewCollection&
{
	if (&toCopy != this)
	{
		RemoveViews();
		ViewStateNode *vn = toCopy.FirstView();
		while (vn->LN_Succ())
		{
			ViewStateNode *new_vn = new ViewStateNode(*vn);
			m_viewList.AddTail(new_vn);
			vn = vn->LN_Succ();
		}
	}
	return *this;
}


Project::ViewCollection::ViewCollection(const TCHAR *group_name)
{
	if (group_name)
	{
		int i, cnt = AfxGetApp()->GetProfileInt(group_name, _T("Number Of Views"), 0);
		for (i = 0; i < cnt; i++)
		{
			std::string str;
			str = strprintf(_T("View #%d"), i);
			ViewStateNode *new_vn = new ViewStateNode(str.c_str());
			m_viewList.AddTail(new_vn);
		}
	}
}


Project::ViewCollection::~ViewCollection()
{
	ViewStateNode *vn;
	while (vn = m_viewList.RemHead())
		delete vn;
}


void Project::ViewCollection::AddView(ViewStateNode *node)
{
	ViewStateNode *vn = FirstView();
	while (vn->LN_Succ())
	{
		if (vn->m_zOrderIndex > node->m_zOrderIndex)
			break;
		vn = vn->LN_Succ();
	}
	this->m_viewList.Insert(node, vn->LN_Pred());
}


void Project::ViewCollection::SaveToIniFile(const TCHAR *group_name) const
{
	AfxGetApp()->WriteProfileInt(group_name, _T("Number Of Views"), GetViewCount());
	ViewStateNode *vn = FirstView();
	int i = 0;
	while (vn->LN_Succ())
	{
		std::string str;
		str = strprintf(_T("View #%d"), i);
		vn->SaveToIniFile(str.c_str());
		vn = vn->LN_Succ();
		i++;
	}
}


std::int32_t Project::ViewStateNode::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return 1;
}


google::protobuf::Message* Project::ViewStateNode::serialize(const SerializeProtoOptions& options) {
	auto view = new WISE::ProjectProto::ViewStorage();
	view->set_version(serialVersionUid(options));

	std::string type;
	if (m_pViewClass)
#if _DLL
		type = m_pViewClass->m_lpszClassName;
#else
		type = m_pViewClass->GetClassName();
#endif

	else
		type = m_viewClassName;

	view->set_viewtype(type);
	view->set_id(m_nID);
	if (m_pViewSpecific)
		view->set_viewdata(m_pViewSpecific, m_sViewSpecific);
	
	auto val = new google::protobuf::Int32Value();
	val->set_value(gsl::narrow_cast<std::int32_t>(m_zOrderIndex));
	view->set_allocated_zorder(val);
	if (m_windowPlacementSet)
	{
		auto placement = new WISE::ProjectProto::ViewStorage_WindowPlacement();
		placement->set_length(m_windowPlacement.length);
		placement->set_flags(m_windowPlacement.flags);
		placement->set_showcmd(m_windowPlacement.showCmd);
		auto min = new WISE::ProjectProto::ViewStorage_WindowPlacement_Point();
		min->set_x(m_windowPlacement.ptMinPosition.x);
		min->set_y(m_windowPlacement.ptMinPosition.y);
		placement->set_allocated_minposition(min);
		auto max = new WISE::ProjectProto::ViewStorage_WindowPlacement_Point();
		max->set_x(m_windowPlacement.ptMaxPosition.x);
		max->set_y(m_windowPlacement.ptMaxPosition.y);
		placement->set_allocated_maxposition(max);
		auto position = new WISE::ProjectProto::ViewStorage_WindowPlacement_Rect();
		position->set_left(m_windowPlacement.rcNormalPosition.left);
		position->set_top(m_windowPlacement.rcNormalPosition.top);
		position->set_right(m_windowPlacement.rcNormalPosition.right);
		position->set_bottom(m_windowPlacement.rcNormalPosition.bottom);
		placement->set_allocated_normalposition(position);
		view->set_allocated_placement(placement);
	}
	return view;
}

auto Project::ViewStateNode::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> ViewStateNode*
{
	auto view = dynamic_cast<const WISE::ProjectProto::ViewStorage*>(&proto);

	if (!view)
	{
		if (valid)
			/// <summary>
			/// The object passed as a view storage is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.ViewStorage", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("ViewStateNode: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if (view->version() != 1)
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The polygon is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.ViewStorage", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(view->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("ViewStateNode: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

#if _DLL
	if (CWFGMProject::m_appMode > 0)
		m_pViewClass = ((CWinAppProject*)AfxGetApp())->findClass(CString(view->viewtype().c_str()));
	else
#endif
		m_pViewClass = nullptr;

	m_viewClassName = view->viewtype();

	m_nID = view->id();

	if (m_pViewSpecific)
		delete[] m_pViewSpecific;
	if (view->viewdata().size() > 0)
	{
		m_pViewSpecific = new unsigned char[view->viewdata().size()];
		std::copy(view->viewdata().begin(), view->viewdata().end(), m_pViewSpecific);
		m_sViewSpecific = view->viewdata().size();
	}

	if (view->has_zorder())
		m_zOrderIndex = view->zorder().value();
	if (view->has_placement())
	{
		auto placement = view->placement();
		m_windowPlacement.length = placement.length();
		m_windowPlacement.flags = placement.flags();
		m_windowPlacement.showCmd = placement.showcmd();
		m_windowPlacement.ptMinPosition.x = placement.minposition().x();
		m_windowPlacement.ptMinPosition.y = placement.minposition().y();
		m_windowPlacement.ptMaxPosition.x = placement.maxposition().x();
		m_windowPlacement.ptMaxPosition.y = placement.maxposition().y();
		m_windowPlacement.rcNormalPosition.left = placement.normalposition().left();
		m_windowPlacement.rcNormalPosition.top = placement.normalposition().top();
		m_windowPlacement.rcNormalPosition.right = placement.normalposition().right();
		m_windowPlacement.rcNormalPosition.bottom = placement.normalposition().bottom();
		m_windowPlacementSet = true;
	}
	else
		m_windowPlacementSet = false;

	return this;
}


std::int32_t Project::ViewCollection::serialVersionUid(const SerializeProtoOptions& options) const noexcept {
	return 1;
}


google::protobuf::Message* Project::ViewCollection::serialize(const SerializeProtoOptions& options)
{
	auto collection = new WISE::ProjectProto::ViewCollection();
	collection->set_version(serialVersionUid(options));

	auto vn = FirstView();
	while (vn->LN_Succ())
	{
		auto view = collection->add_views();
		auto v = dynamic_cast<WISE::ProjectProto::ViewStorage*>(vn->serialize(options));
		view->Swap(v);
		delete v;

		vn = vn->LN_Succ();
	}

	return collection;
}

auto Project::ViewCollection::deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) -> ViewCollection*
{
	auto collection = dynamic_cast<const WISE::ProjectProto::ViewCollection*>(&proto);

	if (!collection)
	{
		if (valid)
			/// <summary>
			/// The object passed as a view collection is invalid. An incorrect object type was passed to the parser.
			/// </summary>
			/// <type>internal</type>
			valid->add_child_validation("WISE.ProjectProto.ViewCollection", name, validation::error_level::SEVERE, validation::id::object_invalid, proto.GetDescriptor()->name());
		weak_assert(0);
		throw ISerializeProto::DeserializeError("ViewCollection: Protobuf object invalid", ERROR_PROTOBUF_OBJECT_INVALID);
	}
	if (collection->version() != 1)
	{
		if (valid)
			/// <summary>
			/// The object version is not supported. The grid filter is not supported by this version of Prometheus.
			/// </summary>
			/// <type>user</type>
			valid->add_child_validation("WISE.ProjectProto.ViewCollection", name, validation::error_level::SEVERE, validation::id::version_mismatch, std::to_string(collection->version()));
		weak_assert(0);
		throw ISerializeProto::DeserializeError("ViewCollection: Version is invalid", ERROR_PROTOBUF_OBJECT_VERSION_INVALID);
	}

	bool warned = false;
	for (int i = 0; i < collection->views_size(); i++)
	{
		auto view = collection->views(i);
		auto vn = new ViewStateNode();
		if (!vn->deserialize(view, valid, name)) {
			weak_assert(0);
			delete vn;
			return nullptr;
		}
		if ((!vn->m_pViewClass) && (!warned) && (vn->m_windowPlacement.length))
		{
			m_loadWarning = "This FGM contained retired views which will be re-opened.\r\n";
			warned = true;
		}
		AddView(vn);
	}

	return this;
}
