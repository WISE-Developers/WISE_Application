/**
 * WISE_Project: FuelCollection.Serialize.cpp
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

#if _DLL
#include "StdAfx.h"
#include "cwinapp.h"
#endif
#include "types.h"
#include <stdlib.h>
#include "FuelCollection.h"
#include "colors.h"
#include "FuelCom_ext.h"
#include "StaticData.h"
#include "results.h"
#include "misc.h"

#include "CWFGMProject.h"

#include <map>

#ifdef DEBUG
#include <assert.h>
#endif

#include <string>
#include <algorithm>

#include <boost/algorithm/string.hpp>


USHORT Project::FuelCollection::ExportFuelMap(const TCHAR *file_name) const
{
	FILE *f = NULL;
	_tfopen_s(&f, file_name, _T("w"));
	if (!f)
		return (USHORT)-1;

	UCHAR		count, unique_count, i, c;
	long		ASCII_index, export_index;
	ICWFGM_Fuel	*cfuel;
	Fuel		*fuel;
	std::string		cname, agency;
	WORD		r, g, b, h, s, l;

	m_fuelMap->GetFuelCount(&count, &unique_count);
	c = count;
	_ftprintf(f, _T("grid_value,export_value,descriptive_name,fuel_type,r,g,b,h,s,l\n"));
	for (i = 0; i < count; i++)
	{
		if (FAILED(m_fuelMap->FuelAtIndex(i, &ASCII_index, &export_index, &cfuel)))
		{
			count++;
			continue;
		}

		fuel = Fuel::FromCOM(cfuel);

		cname = fuel->m_defaultFuelFBP;

		agency = fuel->Name();

		std::replace(cname.begin(), cname.end(), ',', ';');		// commas are the delimiters in the text file
		std::replace(agency.begin(), agency.end(), ',', ';');

		r = GetRValue(fuel->Color());
		g = GetGValue(fuel->Color());
		b = GetBValue(fuel->Color());
		RGBtoHLS(fuel->Color(), &h, &l, &s);

		_ftprintf(f, _T("%ld,%ld,%s,%s,%hd,%hd,%hd,%hd,%hd,%hd\n"), ASCII_index, export_index,  agency.c_str(), cname.c_str(), r, g, b, h, s, l);
	}
	fclose(f);
	return c;
}


#define I_GRID_VALUE 0
#define E_GRID_VALUE 1
#define A_FUEL_TYPE 2
#define F_FUEL_TYPE 3
#define COLOUR_R 4
#define COLOUR_G 5
#define COLOUR_B 6
#define COLOUR_H 7
#define COLOUR_S 8
#define COLOUR_L 9

short getPosition(const TCHAR* value, const TCHAR* one, const TCHAR* two, const TCHAR* three, const TCHAR* four, const TCHAR* five, const TCHAR* six, const TCHAR* seven, const TCHAR* eight, const TCHAR* nine, const TCHAR* ten)
{
	std::string v(value ? value : ""), unu(one ? one : ""), du(two ? two : ""), tri(three ? three : ""), kvar(four ? four : ""), kvin(five ? five : ""), ses(six ? six : ""), sep(seven ? seven : ""), ok(eight ? eight : ""), nau(nine ? nine : ""), dek(ten ? ten : "");
    if (boost::iequals(v, boost::trim_copy(unu)))
		return 0;
    else if (boost::iequals(v, boost::trim_copy(du)))
		return 1;
    else if (boost::iequals(v, boost::trim_copy(tri)))
		return 2;
    else if (boost::iequals(v, boost::trim_copy(kvar)))
		return 3;
    else if (boost::iequals(v, boost::trim_copy(kvin)))
		return 4;
    else if (boost::iequals(v, boost::trim_copy(ses)))
		return 5;
    else if (boost::iequals(v, boost::trim_copy(sep)))
		return 6;
    else if (boost::iequals(v, boost::trim_copy(ok)))
		return 7;
    else if (boost::iequals(v, boost::trim_copy(nau)))
		return 8;
    else if (boost::iequals(v, boost::trim_copy(dek)))
		return 9;
	else
		return -1;
}


USHORT Project::FuelCollection::ImportFuelMap(const TCHAR *file_name, FuelCollection *docFuelCollection, unknownFuelFcn fcn1, badLineFcn fcn2, APTR parm)
{
#define HEADERS h_one, h_two, h_three, h_four, h_five, h_six, h_seven, h_eight, h_nine, h_ten
#define HANDLE_ERROR if (fcn2)\
			{\
				if (!fcn2(parm, i + 1))\
				{\
					fclose(f);\
					return (USHORT)-1;\
				}\
				else\
				{\
					i++;\
					continue;\
				}\
			}\
			else\
			{\
				fclose(f);\
				return (USHORT)-1;\
			}
	Fuel *fuel;
	FILE *f = NULL;
	_tfopen_s(&f, file_name, _T("r"));
	if (!f)
		return (USHORT)-1;					// failure, can't open the file
	USHORT i = 0;
	TCHAR buf[256];
	BOOL hasRGB = TRUE, hasHSL = TRUE;
HSS_PRAGMA_WARNING_PUSH
HSS_PRAGMA_GCC(GCC diagnostic ignored "-Wunused-result")
	_fgetts(buf, 255, f);						// get the header line
HSS_PRAGMA_WARNING_POP
	TCHAR	*context;
	TCHAR *h_one, *h_two, *h_three, *h_four, *h_five, *h_six, *h_seven, *h_eight, *h_nine, *h_ten;
	int numEntries = 10;
	h_one = strtok_se(buf, _T(","), &context);
	h_two = strtok_se(NULL, _T(","), &context);
	h_three = strtok_se(NULL, _T(","), &context);
	h_four = strtok_se(NULL, _T(","), &context);
	h_five = strtok_se(NULL, _T(","), &context);
	h_six = strtok_se(NULL, _T(","), &context);
	h_seven = strtok_se(NULL, _T(","), &context);
	h_eight = strtok_se(NULL, _T(","), &context);
	h_nine = strtok_se(NULL, _T(","), &context);
	h_ten = strtok_se(NULL, _T(",\r\n"), &context);

	std::map<int, int> positionMap;

	int index = getPosition(_T("grid_value"), HEADERS);
	if (index < 0)
		index = getPosition(_T("grid value"), HEADERS);
	weak_assert(index >= 0);
	if (index < 0)
		return (USHORT)-2;
	positionMap.insert({ I_GRID_VALUE, index });

	index = getPosition(_T("export_value"), HEADERS);
	if (index < 0)
		index = getPosition(_T("export value"), HEADERS);
	if (index < 0)
		index = getPosition(_T("export grid_value"), HEADERS);
	if (index < 0)
		index = getPosition(_T("export_grid_value"), HEADERS);

	positionMap.insert({ E_GRID_VALUE, index });
	if (index < 0)
		numEntries--;

	index = getPosition(_T("descriptive_name"), HEADERS);
	if (index < 0)
		index = getPosition(_T("descriptive name"), HEADERS);
	if (index < 0)
		index = getPosition(_T("agency fuel type"), HEADERS);
	if (index < 0)
		index = getPosition(_T("agency_fuel_type"), HEADERS);
	weak_assert(index >= 0);
	if (index < 0)
		return (USHORT)-2;
	positionMap.insert({ A_FUEL_TYPE, index });

	index = getPosition(_T("fuel_type"), HEADERS);
	if (index < 0)
		index = getPosition(_T("fuel type"), HEADERS);
	if (index < 0)
		index = getPosition(_T("FBP fuel type"), HEADERS);
	if (index < 0)
		index = getPosition(_T("FBP_fuel_type"), HEADERS);
	weak_assert(index >= 0);
	if (index < 0)
		return (USHORT)-2;
	positionMap.insert({ F_FUEL_TYPE, index });

	index = getPosition(_T("R"), HEADERS);
	if (index < 0)
		index = getPosition(_T("Red"), HEADERS);
	if (index < 0)
	{
		hasRGB = FALSE;
		numEntries--;
	}
	else
		positionMap.insert({ COLOUR_R, index });

	index = getPosition(_T("G"), HEADERS);
	if (index < 0)
		index = getPosition(_T("Green"), HEADERS);
	if (index < 0)
	{
		hasRGB = FALSE;
		numEntries--;
	}
	else
		positionMap.insert({ COLOUR_G, index });

	index = getPosition(_T("B"), HEADERS);
	if (index < 0)
		index = getPosition(_T("Blue"), HEADERS);
	if (index < 0)
	{
		hasRGB = FALSE;
		numEntries--;
	}
	else
		positionMap.insert({ COLOUR_B, index });

	index = getPosition(_T("hue"), HEADERS);
	if (index < 0)
		index = getPosition(_T("H"), HEADERS);
	if (index < 0)
	{
		hasHSL = FALSE;
		numEntries--;
	}
	else
		positionMap.insert({ COLOUR_H, index });

	index = getPosition(_T("sat"), HEADERS);
	if (index < 0)
		index = getPosition(_T("saturation"), HEADERS);
	if (index < 0)
		index = getPosition(_T("S"), HEADERS);
	if (index < 0)
	{
		hasHSL = FALSE;
		numEntries--;
	}
	else
		positionMap.insert({ COLOUR_S, index });

	index = getPosition(_T("lum"), HEADERS);
	if (index < 0)
		index = getPosition(_T("luminence"), HEADERS);
	if (index < 0)
		index = getPosition(_T("L"), HEADERS);
	if (index < 0)
	{
		hasHSL = FALSE;
		numEntries--;
	}
	else
		positionMap.insert({ COLOUR_L, index });

	if (positionMap.at(E_GRID_VALUE) >= 0)
	{
		if (numEntries != 7 && numEntries != 10)
		{
			fclose(f);
			return (USHORT)-2;
		}
	}
	else
	{
		if (numEntries != 6 && numEntries != 9)
		{
			fclose(f);
			return (USHORT)-2;
		}
	}
	if (!hasRGB && !hasHSL)
	{
		fclose(f);
		return (USHORT)-2;
	}

	TCHAR* values[10];

	while (_fgetts(buf, 255, f))
	{
		if (!(buf[0]))
			continue;					// blank line
		if (!_tcscmp(buf, _T("\n")))
			continue;					// also a blank line
		values[0] = strtok_se(buf, _T(","), &context);
		values[1] = strtok_se(NULL, _T(","), &context);
		values[2] = strtok_se(NULL, _T(","), &context);
		values[3] = strtok_se(NULL, _T(","), &context);
		values[4] = strtok_se(NULL, _T(","), &context);
		values[5] = strtok_se(NULL, _T(","), &context);
		if (hasRGB && hasHSL)
		{
			values[6] = strtok_se(NULL, _T(","), &context);
			values[7] = strtok_se(NULL, _T(","), &context);
			values[8] = strtok_se(NULL, _T(","), &context);
			values[9] = strtok_se(NULL, _T(",\r\n"), &context);
		}
		else
			values[6] = strtok_se(NULL, _T(",\r\n"), &context);

		// didn't get everything we thought we should on the line
		if (!values[numEntries-1])
		{
			HANDLE_ERROR
		}
		long import_i, export_i, r_i, g_i, b_i, h_i, s_i, l_i;
		COLORREF c;
		// didn't get integers for the import, export codes
		if (_stscanf_s(values[positionMap.at(I_GRID_VALUE)], _T("%" SCNdLONG), &import_i) < 1)
		{
			HANDLE_ERROR
		}
		if (positionMap.at(E_GRID_VALUE) >= 0)
		{
			if (_stscanf_s(values[positionMap.at(E_GRID_VALUE)], _T("%" SCNdLONG), &export_i) < 1)
			{
				HANDLE_ERROR
			}
		}
		else
		{
			export_i = import_i;
		}

		if (hasRGB)
		{
			if ((_stscanf_s(values[positionMap.at(COLOUR_R)], _T("%" SCNdLONG), &r_i) > 0) &&
				(_stscanf_s(values[positionMap.at(COLOUR_G)], _T("%" SCNdLONG), &g_i) > 0) &&
				(_stscanf_s(values[positionMap.at(COLOUR_B)], _T("%" SCNdLONG), &b_i) > 0))
				c = RGB(r_i, g_i, b_i);
			else if (hasHSL)
			{
				if ((_stscanf_s(values[positionMap.at(COLOUR_H)], _T("%" SCNdLONG), &h_i) > 0) &&
					(_stscanf_s(values[positionMap.at(COLOUR_S)], _T("%" SCNdLONG), &s_i) > 0) &&
					(_stscanf_s(values[positionMap.at(COLOUR_L)], _T("%" SCNdLONG), &l_i) > 0))
					c = HLStoRGB((WORD)h_i, (WORD)l_i, (WORD)s_i);
				else
				{
					HANDLE_ERROR
				}
			}
			else
			{
				HANDLE_ERROR
			}
		}
		else
		{
			if ((_stscanf_s(values[positionMap.at(COLOUR_H)], _T("%" SCNdLONG), &h_i) > 0) &&
				(_stscanf_s(values[positionMap.at(COLOUR_S)], _T("%" SCNdLONG), &s_i) > 0) &&
				(_stscanf_s(values[positionMap.at(COLOUR_L)], _T("%" SCNdLONG), &l_i) > 0))
				c = HLStoRGB((WORD)h_i, (WORD)l_i, (WORD)s_i);
			else
			{
				HANDLE_ERROR
			}
		}

		std::string _fbp(values[positionMap.at(F_FUEL_TYPE)]), _agency(values[positionMap.at(A_FUEL_TYPE)]);
        boost::trim(_fbp);
        boost::trim(_agency);
		fuel = getFuel(docFuelCollection, _fbp.c_str(), _agency.c_str(), import_i, export_i, c, true);
		// couldn't match this fuel to an existing one, or create it based on
		// the standard fuel types provided to us back in spring of 2000
		if (!fuel)
		{
			if (fcn1)
			{
				if (fuel = fcn1(parm, i + 1, _fbp.c_str(), _agency.c_str(), import_i, export_i, c))
				{
					if (fuel != (Fuel *)-1)
					{
						UCHAR junk;
						m_fuelMap->AddFuel(fuel->m_fuel.get(), import_i, export_i, &junk);
					}
				}
				else
				{
					fclose(f);
					return (USHORT)-1;
				}
			}
			else
			{
				fclose(f);
				return (USHORT)-1;
			}
		}

		i++;
	}
	fclose(f);

	m_isImportedLUT = true;
	m_csLUTFileName = file_name;

	return i;
}
