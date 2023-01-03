/**
 * WISE_Project: cwinapp.h
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

#ifndef __MFC_CWINAPP_H
#define __MFC_CWINAPP_H

#include "cwfgmp_config.h"
#include <afxwin.h>
#include "afxwinappex.h"

#if !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
#pragma managed(push, off)
#endif

#pragma pack(push, 4)

class CWFGMPROJECT_EXPORT CWinAppExt : public CWinAppEx {
	DECLARE_DYNAMIC(CWinAppExt)
private:
	LPCTSTR m_origProfileName;
	CString m_path;

	virtual const TCHAR *GetViewType() const		{ return NULL; };

private:
	CString appDataPath;

public:
	__declspec(property(get=getAppDataPath)) LPCTSTR m_pszAppDataPath;
	LPCTSTR getAppDataPath();

public:
	CWinAppExt();
	~CWinAppExt();

	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void Exit();

	BOOL GetProfileBOOL(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL nDefault);
	BOOL WriteProfileBOOL(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL nValue);

	LONG GetProfileLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONG nDefault);
	BOOL WriteProfileLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONG nValue);

	SHORT GetProfileShort(LPCTSTR lpszSection, LPCTSTR lpszEntry, SHORT nDefault);
	BOOL WriteProfileShort(LPCTSTR lpszSection, LPCTSTR lpszEntry, SHORT nValue);

	LONGLONG GetProfileLongLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONGLONG nDefault);
	BOOL WriteProfileLongLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, LONGLONG nValue);

	ULONGLONG GetProfileULongLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, ULONGLONG nDefault);
	BOOL WriteProfileULongLong(LPCTSTR lpszSection, LPCTSTR lpszEntry, ULONGLONG nValue);

	float GetProfileFloat(LPCTSTR lpszSection, LPCTSTR lpszEntry, float nDefault);
	BOOL WriteProfileFloat(LPCTSTR lpszSection, LPCTSTR lpszEntry, float nValue, LPCTSTR lpszFormat);

	double GetProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double nDefault);
	BOOL WriteProfileDouble(LPCTSTR lpszSection, LPCTSTR lpszEntry, double nValue, LPCTSTR lpszFormat);

protected:
	//{{AFX_MSG(CWinAppExt)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CWinAppExt *AfxGetAppExt();

#pragma pack(pop)

#if !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
#pragma managed(pop)
#endif

#endif
