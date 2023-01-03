/**
 * WISE_Project: cwinapp.cpp
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

#include <stdlib.h>
#include "cwinapp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinAppExt
                  
IMPLEMENT_DYNAMIC(CWinAppExt, CWinAppEx)              
BEGIN_MESSAGE_MAP(CWinAppExt, CWinAppEx)
	//{{AFX_MSG_MAP(CWinAppExt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CWinAppExt::CWinAppExt() {
	m_origProfileName = NULL;
}


CWinAppExt::~CWinAppExt() {
	if (m_origProfileName) {
		m_pszProfileName = m_origProfileName;
	}
}


BOOL CWinAppExt::InitInstance() {
	return CWinAppEx::InitInstance();
}


int CWinAppExt::ExitInstance() {
	LPCTSTR p;
	if (m_origProfileName) {
		p = m_pszProfileName;
		m_pszProfileName = m_origProfileName;
	} else	p = NULL;
	int retval = CWinAppEx::ExitInstance();
	if (p)
		m_pszProfileName = p;
	return retval;
}


void CWinAppExt::Exit() {
	// Same as double-clicking on main window close box.
	ASSERT(AfxGetMainWnd() != NULL);
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}


BOOL CWinAppExt::GetProfileBOOL(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL nDefault) {
	CString str = GetProfileString(lpszSection, lpszEntry);
	if (str.GetLength()) {
		if (!str.CompareNoCase(_T("TRUE")))
			return TRUE;
		return FALSE;
	}
	return nDefault;
}


BOOL CWinAppExt::WriteProfileBOOL(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL nValue) {
	if (nValue)
		return WriteProfileString(lpszSection, lpszEntry, _T("TRUE"));
	return WriteProfileString(lpszSection, lpszEntry, _T("FALSE"));
}


SHORT CWinAppExt::GetProfileShort(LPCTSTR lpszSection, LPCTSTR lpszEntry, SHORT nDefault) {
	CString str = GetProfileString(lpszSection, lpszEntry);
	if (str.GetLength())	
		return (SHORT)_tstoi(str);
	return nDefault;
}


LONG CWinAppExt::GetProfileLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONG nDefault) {
	CString str = GetProfileString(lpszSection, lpszEntry);
	if (str.GetLength())	
		return _tstol(str);
	return nDefault;
}


LONGLONG CWinAppExt::GetProfileLongLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONGLONG nDefault) {
	CString str = GetProfileString(lpszSection, lpszEntry);
	if (str.GetLength())	
		return _tstoi64(str);
	return nDefault;
}


ULONGLONG CWinAppExt::GetProfileULongLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, ULONGLONG nDefault) {
	CString str = GetProfileString(lpszSection, lpszEntry);
	if (str.GetLength())	
		return (ULONGLONG)_tstoi64(str);
	return nDefault;
}


BOOL CWinAppExt::WriteProfileShort(LPCTSTR lpszSection, LPCTSTR lpszEntry, SHORT nValue) {
	CString buf;
	buf.Format(_T("%hd"), nValue);
	return WriteProfileString(lpszSection, lpszEntry, buf);
}


BOOL CWinAppExt::WriteProfileLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONG nValue) {
	CString buf;
	buf.Format(_T("%ld"), nValue);
	return WriteProfileString(lpszSection, lpszEntry, buf);
}


BOOL CWinAppExt::WriteProfileLongLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONGLONG nValue) {
	CString buf;
	buf.Format(_T("%I64d"), nValue);
	return WriteProfileString(lpszSection, lpszEntry, buf);
}


BOOL CWinAppExt::WriteProfileULongLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, ULONGLONG nValue) {
	CString buf;
	buf.Format(_T("%I64d"), (LONGLONG)nValue);
	return WriteProfileString(lpszSection, lpszEntry, buf);
}


float CWinAppExt::GetProfileFloat(LPCTSTR lpszSection, LPCTSTR lpszEntry, float nDefault) {
	CString str = GetProfileString(lpszSection, lpszEntry);
	if (str.GetLength())	
		return (float)_tstof(str);
	return nDefault;
}


BOOL CWinAppExt::WriteProfileFloat(LPCTSTR lpszSection, LPCTSTR lpszEntry, float nValue, LPCTSTR lpszFormat) {
	CString buf;
	buf.Format(lpszFormat, nValue);
	return WriteProfileString(lpszSection, lpszEntry, buf);
}


double CWinAppExt::GetProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double nDefault) {
	CString str = GetProfileString(lpszSection, lpszEntry);
	if (str.GetLength())	
		return _tstof(str);
	return nDefault;
}


BOOL CWinAppExt::WriteProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double nValue, LPCTSTR lpszFormat) {
	CString buf;
	buf.Format(lpszFormat, nValue);
	return WriteProfileString(lpszSection, lpszEntry, buf);
}


LPCTSTR CWinAppExt::getAppDataPath()
{
	if (appDataPath.GetLength() == 0)
	{
		appDataPath.GetEnvironmentVariable("ALLUSERSPROFILE");
		appDataPath += _T("\\CWFGM\\Prometheus\\Prometheus.tmp");
	}
	return appDataPath;
}


CWinAppExt *AfxGetAppExt() {
	return (CWinAppExt *)AfxGetApp();
}
