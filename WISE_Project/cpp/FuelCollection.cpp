/**
 * WISE_Project: FuelCollection.cpp
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

#include "cwfgmp_config.h"
#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif
#if _DLL
#include "cwinapp.h"
#include <afxdisp.h>
#else
#include "AfxIniSettings.h"
#endif
#include <stdlib.h>
#include "propsysreplacement.h"
#include "FuelCollection.h"
#include "colors.h"
#include "FuelCom_ext.h"
#include "StaticData.h"
#include "results.h"
#include "misc.h"
#include <boost/algorithm/string.hpp>
#include "CWFGMProject.h"
#include "CWFGM_Fuel_Shared.h"
#include "str_printf.h"
#include "debug-trap.h"
#include <boost/algorithm/string.hpp>
#include "stdchar.h"

#ifdef DEBUG
#include <assert.h>
#endif

#include "filesystem.hpp"

using namespace std::string_literals;


#define FUELCOLLECTION_DEFAULT_VERSION	39


auto Project::Fuel::FromCOM(ICWFGM_Fuel *fuel) -> Fuel*
{
	if (!fuel)
		return nullptr;
	if (fuel == (ICWFGM_Fuel *)(~0))
		return nullptr;
	if (fuel == (ICWFGM_Fuel *)(-2))
		return nullptr;
	PolymorphicUserData llf;
	fuel->get_UserData(&llf);

	if (!llf.index())
		return nullptr;
	Fuel *f;

	/*POLYMORPHIC CHECK*/
	try { f = (Fuel *)std::get<void *>(llf); } catch (std::bad_variant_access &) { weak_assert(false); return nullptr; };
	return f;
}


Project::Fuel::Fuel(ICWFGM_Fuel *fuel, COLORREF color, const std::string &defaultFBP, const std::string &comments) : m_comments(comments)
{
	m_fuel.reset(fuel);

	if (m_fuel)
	{
		PolymorphicUserData d(this);
		m_fuel->put_UserData(d);
	}
	m_col = color;
	m_defaultFuelFBP = defaultFBP;
	m_cpen = nullptr;
	m_flags = 0;
}


Project::Fuel::Fuel(ICWFGM_Fuel *fuel, const Fuel *f)
{
	m_fuel.reset(fuel);

	if (m_fuel)
	{
		PolymorphicUserData d(this);
		m_fuel->put_UserData(d);
	}
	m_col = f->m_col;
	m_defaultFuelFBP = f->m_defaultFuelFBP; // copy constructor intentionally loses link to the original fuel
	if (auto loc = m_defaultFuelFBP.find('('); (loc != std::string::npos) && isdigit(m_defaultFuelFBP[loc + 1]))
	{
		m_defaultFuelFBP.erase(loc, std::string::npos);
		boost::trim(m_defaultFuelFBP);
	}
	m_comments = f->m_comments;
	m_cpen = nullptr;
	m_flags = 0;
}


Project::Fuel::~Fuel()
{
	if (m_cpen)
	{
#if _DLL
		CPen* pen = static_cast<CPen*>(m_cpen);
		delete pen;
		m_cpen = nullptr;
#elif defined(_DEBUG)
        psnip_dbg_assert(false);
#endif
	}
}


COLORREF Project::Fuel::Color(COLORREF color)
{
	if (m_col != color)
	{
		m_col = color;
		if (m_cpen)
		{
#if _DLL
			CPen* pen = static_cast<CPen*>(m_cpen);
			delete pen;
			m_cpen = nullptr;
#else
			throw std::logic_error("CPen was assigned but there is no way to clean it up.");
#endif
		}
	}
	return m_col.value();
}


std::string Project::Fuel::internalFBPName() const
{
	std::string name = "";
	if (m_fuel)
	{
		std::string bname;
		if (SUCCEEDED(m_fuel->get_Name(&bname)))
			name = bname;
	}
	return name;
}


bool Project::Fuel::internalFBPName(const std::string &name)
{
	if (!m_fuel)
		return false;
	std::string bname(name);
	auto fbpfuel = boost::dynamic_pointer_cast<ICWFGM_FBPFuel>(m_fuel);
	if (fbpfuel)
		return SUCCEEDED(fbpfuel->put_Name(bname));
	return false;
}


bool Project::Fuel::Name(const std::string &name)
{
	return internalFBPName(name);
}


std::string Project::Fuel::Name() const
{
	return internalFBPName();
}


bool Project::Fuel::IsModifiable()
{
	if (m_fuel)
	{
		std::string bname;
		m_fuel->get_Name(&bname);
		auto fbpfuel = boost::dynamic_pointer_cast<ICWFGM_FBPFuel>(m_fuel);
		if (fbpfuel)
			return SUCCEEDED(fbpfuel->put_Name(bname));
	}
	return false;
}


bool Project::Fuel::Equals(Fuel *f)
{
	HRESULT hr = m_fuel->Equals(f->m_fuel, false);
	return (hr == S_OK);
}


bool Project::Fuel::NeedsFMC()
{
	return NeedsFMC(m_fuel.get());
}


bool Project::Fuel::NeedsFMC(ICWFGM_Fuel *fuel)
{
	bool boolean;
	if (FAILED(fuel->IsNonFuel(&boolean)))
	{
		weak_assert(false);
		return false;
	}
	if (boolean)
		return false;
	if (FAILED(fuel->IsMixedFuelType(&boolean)))
	{
		weak_assert(false);
		return false;
	}
	if (boolean)
		return true;
	if (FAILED(fuel->IsMixedDeadFirFuelType(&boolean)))
	{
		weak_assert(false);
		return false;
	}
	if (boolean)
		return true;

	ICWFGM_FBPFuel *fbpfuel = dynamic_cast<ICWFGM_FBPFuel *>(fuel);
	if (!fbpfuel)
		return false;
	USHORT equation;
	if (FAILED(fbpfuel->GetEquation(FUELCOM_EQUATION_FMC, &equation)))
	{
		weak_assert(false);
		return false;
	}
	if (equation == FUELCOM_EQUATION_SELECTION_FMC_NOCALC)
		return false;
	return true;
}


Project::FuelCollection *Project::FuelCollection::m_fuelCollection_Canada = nullptr;
Project::FuelCollection *Project::FuelCollection::m_fuelCollection_NewZealand = nullptr;
Project::FuelCollection *Project::FuelCollection::m_fuelCollection_Tasmania = nullptr;
boost::intrusive_ptr<CCWFGM_FuelMap> Project::FuelCollection::m_fuelMap_Canada;
boost::intrusive_ptr<CCWFGM_FuelMap> Project::FuelCollection::m_fuelMap_NewZealand;
boost::intrusive_ptr<CCWFGM_FuelMap> Project::FuelCollection::m_fuelMap_Tasmania;
std::uint16_t Project::FuelCollection::m_defaultCountry = Project::FuelCollection::DEFAULTCOUNTRY_CANADA;


Project::FuelCollection::FuelCollection(CCWFGM_FuelMap *fuelMap, bool CWFGM_defaults)
{
	m_fuelMap = fuelMap;
	if ((m_fuelMap) && (CWFGM_defaults))
		getDefaultFuels(m_defaultCountry);
}


static std::string getFuelMapLocation(int nz)
{
	std::string retVal;
#if _DLL
	std::string str;
	HKEY hKey = nullptr;
	int retval = RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\CWFGM Project Steering Committee\\Prometheus"), 0, KEY_READ, &hKey);
	if (retval == S_OK)
	{
		DWORD dwSize = 0;
		LONG result;
		CString subkey;
		if (nz == 2)
			subkey = _T("TAZ Fuel Defaults");
		else if (nz == 1)
			subkey = _T("NZ Fuel Defaults");
		else
			subkey = _T("Fuel Defaults");

		result = RegGetValue(hKey, subkey, "V70 Defaults File", RRF_RT_REG_SZ, nullptr, nullptr, &dwSize);
		str.resize(dwSize);
		result = RegGetValue(hKey, subkey, "V70 Defaults File", RRF_RT_REG_SZ, nullptr, &str[0], &dwSize);
		if (result == ERROR_SUCCESS)
		{
			if (str.length() > 0)
				retVal = str;
		}
		RegCloseKey(hKey);
	}
#else
    std::string subkey;
    if (nz == 2)
        subkey = _T("TAZ Fuel Defaults");
    else if (nz == 1)
        subkey = _T("NZ Fuel Defaults");
    else
        subkey = _T("Fuel Defaults");
    retVal = AfxGetApp()->GetRegistryString(subkey, "V70 Defaults File", "");
#endif
	if (retVal.length() == 0)
	{
#if _DLL
		retVal = ((CWinAppExt*)AfxGetApp())->m_pszAppDataPath;
#else
        retVal = AfxGetApp()->GetPath() + "Prometheus.tmp";
#endif
		fs::path p(retVal.c_str());
		if (nz == 2)
			p = p.replace_filename(p.stem().string() + _T("_TAZ"));
		else if (nz == 1)
			p = p.replace_filename(p.stem().string() + _T("_NZ"));
		p = p.replace_extension("fuelmapx");
		retVal = p.string();
	}
	return retVal;
}


Project::FuelCollection::FuelCollection(CCWFGM_FuelMap *fuelMap, const TCHAR *group_name)
{
	m_fuelMap = fuelMap;
	bool need_initial_defaults = false;
	USHORT country;
	fs::path p1;
	if (!_tcscmp(group_name, _T("TAZ Fuel Defaults")))
	{
		p1 = fs::u8path(getFuelMapLocation(2));
		country = Project::FuelCollection::DEFAULTCOUNTRY_TASMANIA;
	}
	else if (!_tcscmp(group_name, _T("NZ Fuel Defaults")))
	{
		p1 = fs::u8path(getFuelMapLocation(1));
		country = Project::FuelCollection::DEFAULTCOUNTRY_NEWZEALAND;
	}
	else
	{
		p1 = fs::u8path(getFuelMapLocation(0));
		country = Project::FuelCollection::DEFAULTCOUNTRY_CANADA;
	}
	fs::path path_buffer = p1.replace_extension(".fuelmapj");

#if _DLL
#else
#undef GetProfileInt
#endif
	int revision = AfxGetApp()->GetProfileInt(group_name, _T("V70 Fuelmap Revision"), 0);

	if ((!_tcscmp(group_name, _T("TAZ Fuel Defaults"))) && (revision == 35))		// force a rewrite of the NZ but not TAZ or Cdn defaults
		revision = 36;
	else if ((_tcscmp(group_name, _T("NZ Fuel Defaults"))) && (revision == 35))
		revision = 36;

	//if the fuel map is old, this is PSaaS, or the folder that should contain the fuelmap doesn't exist load the defaults
	if ((revision < FUELCOLLECTION_DEFAULT_VERSION) || (p1.has_parent_path() && !fs::is_directory(p1.parent_path())))
	{
		need_initial_defaults = true;
#if _DLL
		AfxGetApp()->WriteProfileString(group_name, _T("V70 Defaults Fuelmap"), p1.string().c_str());
#else
#undef WriteProfileString
		AfxGetApp()->WriteProfileString(group_name, _T("V70 Defaults Fuelmap"), p1.string());
#endif
	}
	else
	{
		FILE *f = nullptr;
		_tfopen_s(&f, p1.string().c_str(), _T("r"));
		if (!(f))
 		{
			_tfopen_s(&f, path_buffer.string().c_str(),  _T("r"));
			if (!(f))
			{
#if !defined(_NO_GUI) && _DLL
				std::string ext22, filter;
				ext22 = _T("fuelmapj");
				filter = _T("Default Fuelmap File File (*.fuelmapj, *.fuelmapb) | *.fuelmapj; *.fuelmapb||");
				CFileDialog dlg(TRUE, (LPCTSTR)ext22.c_str(), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT, (LPCTSTR)filter.c_str());

				std::string title(_T("Please Select Application Default Fuelmap File"));
				title += group_name;
				dlg.m_ofn.lpstrTitle = title.c_str();
				if (dlg.DoModal() == IDOK)
				{
					p1 = fs::u8path((LPCTSTR)dlg.GetPathName());
					AfxGetApp()->WriteProfileString(group_name, _T("V70 Defaults Fuelmap"), p1.string().c_str());
				}
				else
				{
#endif
					AfxGetApp()->WriteProfileString(group_name, _T("V70 Defaults Fuelmap"), path_buffer.string().c_str());
					need_initial_defaults = true;
#if !defined(_NO_GUI) && _DLL
				}
#endif
			}
			else
			{
#if !defined(_NO_GUI) && _DLL
				std::string msg;
				msg = strprintf(_T("Could not find \"%s\".\nNow using \"%s\"."), p1.string().c_str(), path_buffer.string().c_str());
				AfxMessageBox(msg.c_str(), MB_OK | MB_ICONINFORMATION);
#endif
				fclose(f);
				p1 = path_buffer;
				AfxGetApp()->WriteProfileString(group_name, _T("V70 Defaults Fuelmap"), p1.string().c_str());
			}
		}
		else
			fclose(f);

		if (!need_initial_defaults)
		{
			if (!fs::exists(p1))
				need_initial_defaults = true;
			else if (boost::iequals(p1.extension().string(), ".fuelmapj") || boost::iequals(p1.extension().string(), ".fuelmapb"))
			{
#if _DLL
				CWaitCursor wait;
#endif
				try
				{
					need_initial_defaults = !LoadProtoFuelmap(p1.string().c_str());
				}
				catch (...)
				{
					need_initial_defaults = true;
				}
			}
			else
				need_initial_defaults = true;
		}
	}
	if (need_initial_defaults)
		getDefaultFuels(country);
}


Project::FuelCollection::~FuelCollection()
{
	if (m_fuelMap)
		Delete();
}


void Project::FuelCollection::Delete()
{
	long fi, export_i;
	ICWFGM_Fuel *f;
	Fuel *fuel;
	for (UCHAR i = 0; i < 255; i++)
	{
		if (SUCCEEDED(m_fuelMap->FuelAtIndex(i, &fi, &export_i, &f)))
		{
			fuel = Fuel::FromCOM(f);
			if (fuel)
			{
				delete fuel;
				void * llf = nullptr;
				PolymorphicUserData pud(llf);
				f->put_UserData(pud);
			}
		}
	}
}


void Project::FuelCollection::Clone(FuelCollection *into) const
{
	long fi1, fi2, export_i;
	ICWFGM_Fuel *f1, *f2;
	Fuel *f, *ff;
	for (UCHAR i = 0; i < 255; i++)
	{
		HRESULT hr = m_fuelMap->FuelAtIndex(i, &fi1, &export_i, &f1);
		UCHAR tmp = (UCHAR)-1;
		if (SUCCEEDED(hr))
			if (SUCCEEDED(m_fuelMap->IndexOfFuel(f1, &fi2, &export_i, &tmp)))
				if (fi2 != fi1)
					continue;			// already did the copy for this one
		if (SUCCEEDED(hr))
		{
			hr = into->m_fuelMap->FuelAtIndex(i, &fi2, &export_i, &f2);
			weak_assert(SUCCEEDED(hr));
			f = Fuel::FromCOM(f2);
			if (f)
				delete f;				// delete any old fuel for this ICWFGM_Fuel
			f = Fuel::FromCOM(f1);
			if (f)
				ff = NewFuel(f2, f->m_col.value(), f->m_defaultFuelFBP, f->m_comments);
		}
	}
	into->m_isImportedLUT = m_isImportedLUT;
	into->m_csLUTFileName = m_csLUTFileName;
	into->m_showUnusedFuels = m_showUnusedFuels;
}


auto Project::FuelCollection::AddFuel(const TCHAR *id) -> Fuel*
{
	const struct StaticFuelData *fd = FuelTable;
	while (fd->szMainAgencyName)
	{
		if (!_tcsicmp(fd->szFBPName, id))
			return AddFuel(fd);
		fd++;
	}
	return nullptr;
}


auto Project::FuelCollection::AddFuel(const struct StaticFuelData *fuel_desc) -> Fuel*
{
	boost::intrusive_ptr<ICWFGM_Fuel> fuel, fuelCopy;
	UCHAR index;
	Fuel *f = nullptr;
	HRESULT hr;

	if (SUCCEEDED(hr = CCWFGM_Fuel::FromCLSID(*fuel_desc->clsid, &fuel)))
	{
		if (SUCCEEDED(hr = fuel->Mutate(&fuelCopy)))
		{
			f = NewFuel(fuelCopy.get(), fuel_desc->color, "", "");
		}
		else
			return nullptr;
	}
	else
		return nullptr;

	ICWFGM_FBPFuel *fbpfuel2 = dynamic_cast<ICWFGM_FBPFuel *>(fuelCopy.get());

	if (fuel_desc->szMainAgencyName)
	{
		f->Name(fuel_desc->szMainAgencyName);
	}

	if (fuel_desc->szFBPName)
	{
		f->m_defaultFuelFBP = fuel_desc->szFBPName;
	}

	const StaticEquationData *equation = fuel_desc->equation;
	if (equation)
	{
		while (equation->equationKey)
		{
			hr = fbpfuel2->SetEquation(equation->equationKey, equation->equation);
			equation++;
		}
	}

	const StaticAttributeData *attribute = fuel_desc->attribute;
	if (attribute)
	{
		while (attribute->attributeKey)
		{
			boost::intrusive_ptr<ICWFGM_Fuel> c2, d1;
			ICWFGM_FBPFuel *fbp_c2, *fbp_d1;
			boost::intrusive_ptr<ICWFGM_Fuel> fuel_c5, fuelCopy_c5;
			PolymorphicAttribute v;
			switch (attribute->mode)
			{
			case 0:		v = attribute->attribute;
						hr = fbpfuel2->SetAttribute(attribute->attributeKey, v);
						break;
			case 1:		v = attribute->attribute;
						if (SUCCEEDED(hr = fbpfuel2->get_C2(&c2))) {
							fbp_c2 = dynamic_cast<ICWFGM_FBPFuel *>(c2.get());
							if (fbp_c2)
								hr = fbp_c2->SetAttribute(attribute->attributeKey, v);
						}
						break;
			case 2:		v = attribute->attribute;
						if (SUCCEEDED(hr = fbpfuel2->get_D1(&d1))) {
							fbp_d1 = dynamic_cast<ICWFGM_FBPFuel *>(d1.get());
							if (fbp_d1)
								hr = fbp_d1->SetAttribute(attribute->attributeKey, v);
						}
						break;
			case 3:		if (SUCCEEDED(hr = CCWFGM_Fuel::FromCLSID(CLSID_CWFGM_Fuel_C5, &fuel_c5)))
							if (SUCCEEDED(hr = fuel_c5->Mutate(&fuelCopy_c5)))
								hr = fbpfuel2->put_C2(fuelCopy_c5);
						break;
			}
			attribute++;
		}
	}

	if (FAILED(m_fuelMap->AddFuel(fuelCopy.get(), fuel_desc->import_val, fuel_desc->export_val, &index)))
	{
		delete f;
		f = nullptr;
	}
	return f;
}


void Project::FuelCollection::getDefaultFuels(USHORT country)
{
	const struct StaticFuelData *fd;
	switch (country)
	{
	case FuelCollection::DEFAULTCOUNTRY_TASMANIA:
		fd = TAZ_FuelTable;
		break;
	case FuelCollection::DEFAULTCOUNTRY_NEWZEALAND:
		fd = NZ_FuelTable;
		break;
	case FuelCollection::DEFAULTCOUNTRY_CANADA:
	default:
		fd = FuelTable;
		break;
	}
	while (fd->szMainAgencyName)
	{
		AddFuel(fd);
		fd++;
	}
}


auto Project::FuelCollection::FindName(const TCHAR *name) const -> Fuel*
{
	long fi, export_i;
	ICWFGM_Fuel *f;
	if (!m_fuelMap)
		return nullptr;
	for (UCHAR i = 0; i < 255; i++)
	{
		HRESULT hr = m_fuelMap->FuelAtIndex(i, &fi, &export_i, &f);
		if (SUCCEEDED(hr))
		{
			std::string str;
			hr = f->get_Name(&str);
			if (boost::iequals(str, name))
			{
				return Fuel::FromCOM(f);
			}
		}
	}
	return nullptr;
}


auto Project::FuelCollection::FindWithFBPCode(const TCHAR *name) const -> Fuel*
{
	long fi, export_i;
	ICWFGM_Fuel *f;
	Fuel *fuel;
	if (!m_fuelMap)
		return nullptr;
	for (UCHAR i = 0; i < 255; i++)
	{
		HRESULT hr = m_fuelMap->FuelAtIndex(i, &fi, &export_i, &f);
		if (SUCCEEDED(hr))
		{
			fuel = Fuel::FromCOM(f);
			weak_assert(fuel);

			if (!fuel)
				return nullptr;
			if (boost::iequals(fuel->m_defaultFuelFBP, name))
				return fuel;
		}
	}
	return nullptr;
}


HRESULT Project::FuelCollection::PCOM_FindFuel(const TCHAR *name, UCHAR *index, Fuel **fuel)
{
	if (!name)
		return E_INVALIDARG;
	*fuel = FindName(name);
	if (!(*fuel))
		return ERROR_FUELS_FUEL_UNKNOWN;
	long t1, t2;
	return m_fuelMap->IndexOfFuel((*fuel)->m_fuel.get(), &t1, &t2, index);
}


void Project::FuelCollection::SaveToIniFile(const TCHAR *group_name)
{
#if _DLL
	std::string path((LPCTSTR)AfxGetApp()->GetProfileString(group_name, _T("V70 Defaults Fuelmap"), _T("")));
#else
#undef GetProfileString
	std::string path = AfxGetApp()->GetProfileString(group_name, _T("V70 Defaults Fuelmap"), _T(""));
#endif

	std::uint16_t country;
	if (!_tcscmp(group_name, _T("TAZ Fuel Defaults")))
		country = Project::FuelCollection::DEFAULTCOUNTRY_TASMANIA;
	else if (!_tcscmp(group_name, _T("NZ Fuel Defaults")))
		country = Project::FuelCollection::DEFAULTCOUNTRY_NEWZEALAND;
	else
		country = Project::FuelCollection::DEFAULTCOUNTRY_CANADA;

#ifdef DEBUG
	weak_assert(path.length());
#endif

	AfxGetApp()->WriteProfileInt(group_name, _T("V70 Fuelmap Revision"), FUELCOLLECTION_DEFAULT_VERSION);

#if _DLL
	CWaitCursor wait;
#endif
	fs::path p1(path);
	fs::create_directories(p1.parent_path());
	SaveProtoFuelmap(path.c_str());
}


std::string Project::FuelCollection::MakeUniqueName(Fuel *fuel, const TCHAR *name) const
{
	std::string str(name);
	long dup_cnt = 1;
	bool done = false;
	long fi, fi2, export_i;
	ICWFGM_Fuel *f;
	while (!done)
	{
		done = true;
		for (std::uint8_t i = 0; i < 255; i++)
		{
			HRESULT hr = m_fuelMap->FuelAtIndex(i, &fi, &export_i, &f);
			if (SUCCEEDED(hr))
			{
				std::uint8_t b = gsl::narrow_cast<std::uint8_t>(-1);
				if (SUCCEEDED(m_fuelMap->IndexOfFuel(f, &fi2, &export_i, &b)))
					if (fi2 != fi)
						continue;
				Fuel *ff = Fuel::FromCOM(f);
				if ((ff) && (ff != fuel) && (boost::equals(str, ff->Name())))
				{
					dup_cnt++;
					str = strprintf(_T("%s (#%d)"), name, dup_cnt);
					done = false;
					break;
				}
			}
		}
	}
	return str;
}


auto Project::FuelCollection::getFuel(FuelCollection* /*docFuelCollection*/, const TCHAR *fbp_name, const TCHAR *agency_name,
	LONG import_num, LONG export_num, COLORREF color, bool try_to_add) -> Fuel*
{
	short copy = 0;

#ifdef DEBUG
	weak_assert(agency_name);
#endif

	Fuel *f = nullptr;
	f = FindName(agency_name);				// let's see if it's already mapped

	if (!f)
	{
		// can't find it anywhere with the agency name, so look into the app defaults for something with the same FBP template
		if ((fbp_name[0]))
			if (m_fuelCollection_Canada)
				f = m_fuelCollection_Canada->FindWithFBPCode(fbp_name);
		if ((!f) && (fbp_name[0]))
			if (m_fuelCollection_NewZealand)
				f = m_fuelCollection_NewZealand->FindWithFBPCode(fbp_name);
		if ((!f) && (fbp_name[0]))
			if (m_fuelCollection_Tasmania)
				f = m_fuelCollection_Tasmania->FindWithFBPCode(fbp_name);

		// we found it, so let's make a copy
		if ((f) && (try_to_add))
		{
			boost::intrusive_ptr<ICWFGM_Fuel> fuel;
			f->m_fuel->Mutate(&fuel);			// this will create a copy of it, not necessarily a read/write, but of the same type (the -> type would give us a read/
			Fuel *f2 = NewFuel(fuel.get(), f);
			f2->Name(agency_name);
			f2->m_defaultFuelFBP = fbp_name;
			f2->Color(color);
			f = f2;						// let it fall through to the AddFuel lines
			copy = 1;
		}
	}

	// IF we haven't found it (either in this fuel map or in the app defaults) AND IF it's a valid name AND IF we are trying
	// to add it, THEN force a creation of that fuel
	if ((!f) && (fbp_name[0]))
	{
		const struct StaticFuelData *fd = FuelTable, *valid = nullptr;
		struct StaticFuelData sfd;
		while (fd->szMainAgencyName)
		{
			if ((!_tcsicmp(fbp_name, fd->szFBPName)) ||
			    (!_tcsicmp(fbp_name, fd->szAlias[0])) ||
			    (!_tcsicmp(fbp_name, fd->szAlias[1])) ||
			    (!_tcsicmp(fbp_name, fd->szAlias[2])) ||
			    (!_tcsicmp(fbp_name, fd->szAlias[3])) ||
			    (!_tcsicmp(fbp_name, fd->szAlias[4])) ||
			    (!_tcsicmp(fbp_name, fd->szAlias[5])) ||
			    (!_tcsicmp(fbp_name, fd->szAlias[6])) ||
			    (!_tcsicmp(fbp_name, fd->szAlias[7])))
			{
				valid = fd;
				break;
			}
			fd++;
		}

		if (!valid)
		{
			fd = NZ_FuelTable;
			while (fd->szMainAgencyName)
			{
				if ((!_tcsicmp(fbp_name, fd->szFBPName)) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[0])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[1])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[2])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[3])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[4])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[5])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[6])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[7])))
				{
					valid = fd;
					break;
				}
				fd++;
			}
		}

		if (!valid)
		{
			fd = TAZ_FuelTable;
			while (fd->szMainAgencyName)
			{
				if ((!_tcsicmp(fbp_name, fd->szFBPName)) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[0])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[1])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[2])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[3])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[4])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[5])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[6])) ||
				    (!_tcsicmp(fbp_name, fd->szAlias[7])))
				{
					valid = fd;
					break;
				}
				fd++;
			}
		}

		if ((valid) && (try_to_add))
		{
			sfd = *valid;
			sfd.szMainAgencyName = agency_name;
			sfd.import_val = import_num;
			sfd.export_val = export_num;
			sfd.color = color;
			f = AddFuel(&sfd);
			return f;
		}
	}

	// with things like colors, names, etc. - we always and only use the first one encountered.  RWB: 011127: disabled
	if ((f) && (try_to_add))
	{
		UCHAR idx;
		m_fuelMap->AddFuel(f->m_fuel.get(), import_num, export_num, &idx);
	}
	return f;
}


auto Project::FuelCollection::FuelTypeCopy(ICWFGM_FBPFuel *fuel, LONG file_index, LONG fuel_index, bool warn) -> Fuel*
{
	UCHAR count = 0, unique_count = 0;
	m_fuelMap->GetFuelCount(&count, &unique_count);
	if ((count > 254)  && (warn))
	{
#if !defined(_NO_GUI) && _DLL
		AfxMessageBox(_T("There are too many fuel type / grid value entries to perform this operation"), MB_ICONSTOP | MB_OK);
#endif
		return nullptr;
	}

	if (!fuel)
		return nullptr;

	boost::intrusive_ptr<ICWFGM_Fuel> newfuel;
	fuel->Mutate(&newfuel);

	PolymorphicUserData data;
	fuel->get_UserData(&data);
	Fuel* f;
	if (data.index())
	{
		/*POLYMORPHIC CHECK*/
		try { f = NewFuel(newfuel.get(), static_cast<Fuel *>(std::get<void *>(data))); } catch (std::bad_variant_access &) { weak_assert(false); return nullptr; };
	}
	else
		f = NewFuel(newfuel.get(), nullptr);
	data = f;
	newfuel->put_UserData(data);

	{
		UCHAR b;
		if (FAILED(m_fuelMap->AddFuel(newfuel.get(), file_index, fuel_index, &b)))
		{
#if !defined(_NO_GUI) && _DLL
			if (warn)
				AfxMessageBox(_T("The fuel new/copy operation has failed.\n"), MB_ICONINFORMATION | MB_OK);
#endif
			delete f;
			return nullptr;
		}
		return f;
	}
}


auto Project::FuelCollection::FuelTypeCopy(ICWFGM_Fuel *fuel, LONG file_index, LONG fuel_index, bool warn) -> Fuel*
{
	Fuel *pFuel;
	ICWFGM_FBPFuel *newfbpfuel = dynamic_cast<ICWFGM_FBPFuel *>(fuel);
	if (newfbpfuel)
		pFuel = FuelTypeCopy(newfbpfuel, file_index, fuel_index, warn);
	else
		pFuel = nullptr;
	if (pFuel)
		return pFuel;
	return nullptr;
}


bool Project::Fuel::IsNonFuel()
{
	if(m_fuel)
	{
		bool val;
		m_fuel->IsNonFuel(&val);
		return (val) ? TRUE : FALSE;
	}
	return FALSE;
}


std::string Project::FuelCollection::CalculateModifiedFuelsBit(FuelCollection *canada, FuelCollection *nz, FuelCollection *taz)
{
	std::string warnings;
	long fi1, fi2, export_i;
	ICWFGM_Fuel *f1;
	Fuel *f;
	for (UCHAR i = 0; i < 255; i++)
	{
		HRESULT hr = m_fuelMap->FuelAtIndex(i, &fi1, &export_i, &f1);
		UCHAR tmp = (UCHAR)-1;
		if (SUCCEEDED(hr))
			if (SUCCEEDED(m_fuelMap->IndexOfFuel(f1, &fi2, &export_i, &tmp)))
				if (fi2 != fi1)
					continue;			// already did the copy for this one
		if (SUCCEEDED(hr))
		{
			f = Fuel::FromCOM(f1);
			weak_assert(f);

			if (f)
				warnings += f->SetModifiedBits(canada, nz, taz);
		}
	}
	return warnings;
}


std::string Project::Fuel::SetModifiedBits(FuelCollection *canada, FuelCollection *nz, FuelCollection *taz)
{
	std::string warnings;
	Fuel *f2;

	if (canada)
		f2 = canada->getFuel(nullptr, m_defaultFuelFBP.c_str(), Name().c_str(), -1, -1, COLORREF(0), false);
	else
		f2 = nullptr;

	if ((!f2) && (nz))
		f2 = nz->getFuel(nullptr, m_defaultFuelFBP.c_str(), Name().c_str(), -1, -1, COLORREF(0), false);

	if ((!f2) && (taz))
		f2 = taz->getFuel(nullptr, m_defaultFuelFBP.c_str(), Name().c_str(), -1, -1, COLORREF(0), false);

	if (f2)
	{
#ifdef DEBUG
		std::string fbp1 = Name();
		std::string fbp2 = f2->Name();
		GUID cls1, cls2;
		m_fuel->GetDefaultCLSID(&cls1);
		f2->m_fuel->GetDefaultCLSID(&cls2);
#endif

		if (!boost::iequals(Name(), f2->Name()))
			m_flags |= Fuel::FLAG_MODIFIED_NAME;
		else
			m_flags &= (~(Fuel::FLAG_MODIFIED_NAME));
		if (!Equals(f2))
			m_flags |= Fuel::FLAG_MODIFIED;
		else
			m_flags &= (~(Fuel::FLAG_MODIFIED));
	}
	else
	{
		std::string fbp1 = Name();
		warnings = "Fuel Type \"";
		warnings += fbp1;
		warnings += "\" is defined as standard but cannot be located in the set of standard/default fuel types.\n";
	}
	return warnings;
}


auto Project::Fuel::FindDefault(FuelCollection *canada, FuelCollection *nz, FuelCollection *taz) -> Fuel*
{
	Fuel *f2;

	if (canada)
		f2 = canada->getFuel(nullptr, m_defaultFuelFBP.c_str(), Name().c_str(), -1, -1, COLORREF(0), FALSE);
	else
		f2 = nullptr;

	if ((!f2) && (nz))
		f2 = nz->getFuel(nullptr, m_defaultFuelFBP.c_str(), Name().c_str(), -1, -1, COLORREF(0), FALSE);

	if ((!f2) && (taz))
		f2 = taz->getFuel(nullptr, m_defaultFuelFBP.c_str(), Name().c_str(), -1, -1, COLORREF(0), FALSE);

	return f2;
}


std::string Project::Fuel::DisplayName(CCWFGM_FuelMap *fuelMap)
{
	UCHAR ii = (UCHAR)-1;
	long ASCII_index, tmp, export_index;
	fuelMap->IndexOfFuel(m_fuel.get(), &tmp, &export_index, &ii);
	std::string cname = Name(), cname2, cname3;

	if (tmp !=-1)
	{
		cname2 = strprintf("%ld", tmp);
			
		while (!FAILED(fuelMap->IndexOfFuel(m_fuel.get(), &tmp, &export_index, &ii)))
		{
			if (tmp != -1)
			{
				cname3 = strprintf(", %ld" , tmp);
				cname2 = cname2 + cname3;
			}
		}		
		cname = cname2 + ": " + cname;
	}

	if (m_defaultFuelFBP.length())
	{
		cname = cname + " (" + m_defaultFuelFBP + ")";
	}
	return cname;
}

unsigned short compareKeys[] = {
	FUELCOM_ATTRIBUTE_M3M4_C2_A,
	FUELCOM_ATTRIBUTE_M3M4_C2_B,
	FUELCOM_ATTRIBUTE_M3M4_C2_C,
	FUELCOM_ATTRIBUTE_M4_D1_A,
	FUELCOM_ATTRIBUTE_A,
	FUELCOM_ATTRIBUTE_B,
	FUELCOM_ATTRIBUTE_C,
	FUELCOM_ATTRIBUTE_M4_D1_B,
	FUELCOM_ATTRIBUTE_M4_D1_C,
	FUELCOM_ATTRIBUTE_Q,
	FUELCOM_ATTRIBUTE_BUI0,
	FUELCOM_ATTRIBUTE_MAXBE,
	FUELCOM_ATTRIBUTE_CFL,
	FUELCOM_ATTRIBUTE_TREE_HEIGHT,
	FUELCOM_ATTRIBUTE_O1AB_MATTED_A,
	FUELCOM_ATTRIBUTE_O1AB_MATTED_B,
	FUELCOM_ATTRIBUTE_O1AB_MATTED_C,
	FUELCOM_ATTRIBUTE_O1AB_STANDING_A,
	FUELCOM_ATTRIBUTE_O1AB_STANDING_B,
	FUELCOM_ATTRIBUTE_O1AB_STANDING_C,
	FUELCOM_ATTRIBUTE_CBH,
	FUELCOM_ATTRIBUTE_PDF,
	FUELCOM_ATTRIBUTE_PC,
	FUELCOM_ATTRIBUTE_CURINGDEGREE,
	FUELCOM_ATTRIBUTE_GFL,
	0
};

unsigned short optionalKeys[] = {
	FUELCOM_ATTRIBUTE_CBH,
	FUELCOM_ATTRIBUTE_PDF,
	FUELCOM_ATTRIBUTE_PC,
	FUELCOM_ATTRIBUTE_CURINGDEGREE,
	FUELCOM_ATTRIBUTE_GFL,
	0
};

std::string Project::Fuel::PrintOptionalParams(FuelCollection* m_fuelCollection, BOOL IsC6)
{
	std::string ret = "";

	//Get the full fuel name
	Fuel *f = Fuel::FromCOM(m_fuel.get());

	std::string fname;
	if (f)
		fname = f->DisplayName(m_fuelCollection->m_fuelMap.get()); //Conifer Plantation for example

	std::string fuelName = "\t" + fname + " [";

	ret += fuelName;

	short j = 0;
	short optCount = 0;
	PolymorphicAttribute temp;
	unsigned short curKey = optionalKeys[j++];
	while (curKey != 0)
	{
		HRESULT curResult = f->m_fuel->GetAttribute(curKey, &temp);

		if (curResult == S_OK)
		{
			double fVal;
			VariantToDouble_(temp, &fVal);
			std::string val;

			if (curKey == FUELCOM_ATTRIBUTE_CURINGDEGREE || curKey == FUELCOM_ATTRIBUTE_PC || curKey == FUELCOM_ATTRIBUTE_PDF)
				fVal *= 100.0;

			val = strprintf("%.2f", fVal);
			std::string attr = GetOptionalAttrNameUnit(curKey, val);

			if (curKey == FUELCOM_ATTRIBUTE_CBH)
			{
				if (IsC6)
				{
					optCount++;
					ret += " " + attr + ";";
				}
			}
			else
			{
				optCount++;
				ret += " " + attr + ";";
			}
		}

		curKey = optionalKeys[j++];
	}

	if (optCount > 0)
		ret = ret.substr(0, ret.size() - 1);

	return ret + " ]\n";
}

std::string Project::Fuel::PrintReportChanges(FuelCollection* m_fuelCollection)
{
	std::string ret = "";
	Fuel* defaultFuel = nullptr;

	//Get the default fuel
	if (m_fuelCollection->m_csLUTFileName == "Canada"s)
		defaultFuel = FindDefault(m_fuelCollection->m_fuelCollection_Canada, nullptr, nullptr);

	else if (m_fuelCollection->m_csLUTFileName == "New Zealand"s)
		defaultFuel = FindDefault(nullptr, m_fuelCollection->m_fuelCollection_NewZealand, nullptr);

	else if (m_fuelCollection->m_csLUTFileName == "Tasmania"s)
		defaultFuel = FindDefault(nullptr, nullptr, m_fuelCollection->m_fuelCollection_Tasmania);

	if (!defaultFuel) // can happen if m_csLUTFileName is empty
		defaultFuel = FindDefault(m_fuelCollection->m_fuelCollection_Canada, m_fuelCollection->m_fuelCollection_NewZealand, m_fuelCollection->m_fuelCollection_Tasmania);

	//Get the full fuel name
	Fuel *f = Fuel::FromCOM(m_fuel.get());

	std::string fname;
	if (f)
		fname = f->DisplayName(m_fuelCollection->m_fuelMap.get()); //Conifer Plantation for example

	std::string fuelName = "\t" + fname + "\n";

	ret += fuelName;
	PolymorphicAttribute temp, dTemp;

	//Check for mixed fuel types
	boost::intrusive_ptr<ICWFGM_Fuel> pc2 = nullptr, pd1 = nullptr;
	auto fbpfuel = boost::dynamic_pointer_cast<ICWFGM_FBPFuel>(m_fuel);
	fbpfuel->get_C2(&pc2);
	fbpfuel->get_D1(&pd1);

	//Check each attribute key for a value
	short j = 0;
	unsigned short curKey = compareKeys[j++];
	while (curKey != 0)
	{
		HRESULT curResult = f->m_fuel->GetAttribute(curKey, &temp);
		HRESULT defResult;
		if (defaultFuel)
			defResult = defaultFuel->m_fuel->GetAttribute(curKey, &dTemp);
		else
			defResult = !curResult;	// this looks odd but done on purpose for the next if statement

		//Check if they both returned the same value
		if (curResult == defResult)
		{
			//Check if they both returned S_OK else that means they both failed meaning they're equal and no change to report
			if ((defaultFuel) && (curResult == S_OK))
			{
				//Need to compare the values
				double fVal;
				VariantToDouble_(temp, &fVal);
				double dVal;
				VariantToDouble_(dTemp, &dVal);

				if (fVal != dVal)
				{
					//Values are different, need to output it
					std::string val;

					if (curKey == FUELCOM_ATTRIBUTE_CURINGDEGREE || curKey == FUELCOM_ATTRIBUTE_PC || curKey == FUELCOM_ATTRIBUTE_PDF)
						fVal *= 100.0;
					else if (curKey == FUELCOM_ATTRIBUTE_CFL)
						fVal *= 10.0;

					val = strprintf("%.2f", fVal);
					std::string attr = GetAttributeName(curKey);
					ret += "\t\t" + attr + " = ";

					if (curKey == FUELCOM_ATTRIBUTE_CBH)
						ret += val + "m";
					else if (curKey == FUELCOM_ATTRIBUTE_CFL)
						ret += val + "kg/m^2";
					else
						ret += val;

					ret += "\n";
				}
			}
			else if (pc2 != nullptr || pd1 != nullptr)
			{
				//Try getting value from there
				if (pc2)
				{
					curResult = pc2->GetAttribute(curKey, &temp);
					if (curResult == S_OK)
					{
						std::string val;
						double fVal;
						VariantToDouble_(temp, &fVal);

						if (curKey == FUELCOM_ATTRIBUTE_CURINGDEGREE || curKey == FUELCOM_ATTRIBUTE_PC || curKey == FUELCOM_ATTRIBUTE_PDF)
							fVal *= 100.0;
						else if (curKey == FUELCOM_ATTRIBUTE_CFL)
							fVal *= 10.0;

						val = strprintf("%.2f", fVal);
						std::string attr = GetAttributeName(curKey);
						ret += "\t\t" + attr + " = ";

						if (curKey == FUELCOM_ATTRIBUTE_CBH)
							ret += val + "m";
						else if (curKey == FUELCOM_ATTRIBUTE_CFL)
							ret += val + "kg/m^2";
						else
							ret += val;

						ret += "\n";
					}
				}
				else if (pd1)
				{
					curResult = pd1->GetAttribute(curKey, &temp);
					if (curResult == S_OK)
					{
						std::string val;
						double fVal;
						VariantToDouble_(temp, &fVal);

						if (curKey == FUELCOM_ATTRIBUTE_CURINGDEGREE || curKey == FUELCOM_ATTRIBUTE_PC || curKey == FUELCOM_ATTRIBUTE_PDF)
							fVal *= 100.0;
						else if (curKey == FUELCOM_ATTRIBUTE_CFL)
							fVal *= 10.0;

						val = strprintf("%.2f", fVal);
						std::string attr = GetAttributeName(curKey);
						ret += "\t\t" + attr + " = ";

						if (curKey == FUELCOM_ATTRIBUTE_CBH)
							ret += val + "m";
						else if (curKey == FUELCOM_ATTRIBUTE_CFL)
							ret += val + "kg/m^2";
						else
							ret += val;

						ret += "\n";
					}
				}
				else
				{
					//Values are different, need to output it
					std::string attr = GetAttributeName(curKey);
					ret += attr + "= not present\n";
				}
			}
		}
		else
		{	//One failed and one didn't
			//Check that this isn't a mixed fuel type
			if (curResult == E_UNEXPECTED)
			{
				//Values are different, need to output it
				std::string attr = GetAttributeName(curKey);
				ret += attr + "= not present\n";
			}
			else
			{
				double fVal;
				VariantToDouble_(temp, &fVal);

				std::string val;

				if (curKey == FUELCOM_ATTRIBUTE_CURINGDEGREE || curKey == FUELCOM_ATTRIBUTE_PC || curKey == FUELCOM_ATTRIBUTE_PDF)
					fVal *= 100.0;
				else if (curKey == FUELCOM_ATTRIBUTE_CFL)
					fVal *= 10.0;

				val = strprintf("%.2f", fVal);
				std::string attr = GetAttributeName(curKey);
				ret += "\t\t" + attr + " = ";

				if (curKey == FUELCOM_ATTRIBUTE_CBH)
					ret += val + "m";
				else if (curKey == FUELCOM_ATTRIBUTE_CFL)
					ret += val + "kg/m^2";
				else
					ret += val;

				ret += "\n";
			}
		}

		curKey = compareKeys[j++];
	}

	return ret;
}

std::string Project::Fuel::GetOptionalAttrNameUnit(USHORT key, const std::string &val)
{
	std::string ret;
	switch (key)
	{
	case FUELCOM_ATTRIBUTE_CBH:
		ret = val + "m CBH";
		break;
	case FUELCOM_ATTRIBUTE_PDF:
		ret = val + " PDF";
		break;
	case FUELCOM_ATTRIBUTE_PC:
		ret = val + " PC";
		break;
	case FUELCOM_ATTRIBUTE_CURINGDEGREE:
		ret = val + "% Deg. of Curing";
		break;
	case FUELCOM_ATTRIBUTE_GFL:
		ret = val + "kg/m" POWER2_SYMBOL " Grass Fuel Load";
		break;
	}

	return ret;
}

std::string Project::Fuel::GetAttributeName(USHORT key)
{
	std::string ret;
	switch (key)
	{
	case FUELCOM_ATTRIBUTE_CURINGDEGREE:
		ret = "degree of curing (%)";
		break;
	case FUELCOM_ATTRIBUTE_GFL:
		ret = "gfl";
		break;
	case FUELCOM_ATTRIBUTE_PDF:
		ret = "pdf";
		break;
	case FUELCOM_ATTRIBUTE_PC:
		ret = "pc";
		break;
	case FUELCOM_ATTRIBUTE_Q:
		ret = "q";
		break;
	case FUELCOM_ATTRIBUTE_BUI0:
		ret = "BUI0";
		break;
	case FUELCOM_ATTRIBUTE_MAXBE:
		ret = "max be";
		break;
	case FUELCOM_ATTRIBUTE_CFL:
		ret = "CFL";
		break;
	case FUELCOM_ATTRIBUTE_CBH:
		ret = "CBH";
		break;
	case FUELCOM_ATTRIBUTE_TREE_HEIGHT:
		ret = "tree height";
		break;

	case FUELCOM_ATTRIBUTE_O1AB_MATTED_A:
		ret = "O1AB matted a";
		break;
	case FUELCOM_ATTRIBUTE_O1AB_STANDING_A:
		ret = "O1AB standing a";
		break;
	case FUELCOM_ATTRIBUTE_M3M4_C2_A:
		ret = "M3/M4 C2 a";
		break;
	case FUELCOM_ATTRIBUTE_M4_D1_A:
		ret = "M4 D1 a";
		break;
	case FUELCOM_ATTRIBUTE_A:
		ret = "a";
		break;

	case FUELCOM_ATTRIBUTE_O1AB_MATTED_B:
		ret = "O1AB matted b";
		break;
	case FUELCOM_ATTRIBUTE_O1AB_STANDING_B:
		ret = "O1AB standing b";
		break;
	case FUELCOM_ATTRIBUTE_M3M4_C2_B:
		ret = "M3/M4 C2 b";
		break;
	case FUELCOM_ATTRIBUTE_M4_D1_B:
		ret = "M4 D1 b";
		break;
	case FUELCOM_ATTRIBUTE_B:
		ret = "b";
		break;

	case FUELCOM_ATTRIBUTE_O1AB_MATTED_C:
		ret = "O1AB matted c";
		break;
	case FUELCOM_ATTRIBUTE_O1AB_STANDING_C:
		ret = "O1AB standing c";
		break;
	case FUELCOM_ATTRIBUTE_M3M4_C2_C:
		ret = "M3/M4 C2 c";
		break;
	case FUELCOM_ATTRIBUTE_M4_D1_C:
		ret = "M4 D1 c";
		break;
	case FUELCOM_ATTRIBUTE_C:
		ret = "c";
		break;
	}

	return ret;
}
