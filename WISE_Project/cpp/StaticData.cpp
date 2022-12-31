/**
 * WISE_Project: StaticData.cpp
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

#ifdef _MSC_VER
#include "stdafx.h"
#include <winerror.h>
#endif
#include "types.h"
#include "guid.h"
#include "StaticData.h"
#include "ICWFGM_FBPFuel.h"
#include "FuelCom_ext.h"


static const struct Project::StaticAttributeData M_fueltype[][2] = {
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.50}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.05}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.10}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.15}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.20}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.25}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.30}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.35}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.40}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.45}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.50}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.55}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.60}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.65}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.70}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.75}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.80}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.85}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.90}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PC, 0.95}, { 0, 0, 0. } }
};


static const struct Project::StaticAttributeData MD_fueltype[][2] = {
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.50}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.05}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.10}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.15}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.20}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.25}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.30}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.35}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.40}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.45}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.50}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.55}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.60}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.65}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.70}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.75}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.80}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.85}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.90}, { 0, 0, 0. } },
	{ { 0, FUELCOM_ATTRIBUTE_PDF, 0.95}, { 0, 0, 0. } }
};


const struct Project::StaticFuelData FuelTable[] = {
	 /* main/agency name						fbp name		aliases										COM class ID		import, export	color	equation	attribute*/
	{ _T("Spruce-Lichen Woodland"),					_T("C-1"),		{ _T("C1"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },						&CLSID_CWFGM_Fuel_C1,	1, 1,	RGB(209, 255, 115),	NULL,	NULL },
	{ _T("Boreal Spruce"),						_T("C-2"),		{ _T("C2"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },						&CLSID_CWFGM_Fuel_C2,	2, 2,	RGB(34, 102, 51),	NULL,	NULL },
	{ _T("Mature Jack or Lodgepole Pine"),				_T("C-3"),		{ _T("C3"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },						&CLSID_CWFGM_Fuel_C3,	3, 3,	RGB(131, 199, 149),	NULL,	NULL },
	{ _T("Immature Jack or Lodgepole Pine"),			_T("C-4"),		{ _T("C4"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },						&CLSID_CWFGM_Fuel_C4,	4, 4,	RGB(112, 168, 0),	NULL,	NULL },
	{ _T("Red and White Pine"),					_T("C-5"),		{ _T("C5"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },						&CLSID_CWFGM_Fuel_C5,	5, 5,	RGB(223, 184, 230),	NULL,	NULL },
	{ _T("Conifer Plantation"),					_T("C-6"),		{ _T("C6"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },						&CLSID_CWFGM_Fuel_C6,	6, 6,	RGB(172, 102, 237),	NULL,	NULL },
	{ _T("Ponderosa Pine - Douglas-Fir"),					_T("C-7"),			{ _T("C7"), _T("C-7 Ponderosa Pine / Douglas Fir"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },&CLSID_CWFGM_Fuel_C7,	7, 7,	RGB(112, 12, 242),	NULL,	NULL },

	{ _T("Leafless Aspen"),						_T("D-1"),		{ _T("D1"), _T("D-1a"), _T("D1a"), _T(""), _T(""), _T(""), _T(""), _T("") },					&CLSID_CWFGM_Fuel_D1,	11, 11,	RGB(196, 189, 151),	NULL,	NULL },
	{ _T("Green Aspen (with BUI Thresholding)"),			_T("D-2"),		{ _T("D2"), _T("D-2a"), _T("D-1b"), _T("D1b"), _T(""), _T(""), _T(""), _T("") },					&CLSID_CWFGM_Fuel_D2,	12, 12,	RGB(137, 112, 68),	NULL,	NULL },
	{ _T("Aspen"),							_T("D-1/D-2"),		{ _T("D-1/D-2 (green-up grid)"), _T(""), _T("D1/D2"), _T("D1/D2 (green-up grid)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_D1D2, 13, 13, RGB(196, 189, 151),	NULL,	NULL },

	{ _T("Jack or Lodgepole Pine Slash"),				_T("S-1"),		{ _T("S1"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },							&CLSID_CWFGM_Fuel_S1,	21, 21,	RGB(251, 190, 185),	NULL,	NULL },
	{ _T("White Spruce - Balsam Slash"),					_T("S-2"),			{ _T("S2"), _T("S-2 White Spruce / Balsam Slash"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },&CLSID_CWFGM_Fuel_S2,	22, 22,	RGB(247, 104, 161),	NULL,	NULL },
	{ _T("Coastal Cedar - Hemlock - Douglas-Fir Slash"),		_T("S-3"),		{ _T("S3"), _T("S-3 Coastal Cedar / Hemlock / Douglas-Fir Slash"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_S3,	23, 23,	RGB(174, 1, 126),	NULL,	NULL },

	{ _T("Matted Grass"),						_T("O-1a"),		{ _T("O-1"), _T("O1"), _T("O1a"), _T(""), _T(""), _T(""), _T(""), _T("") },						&CLSID_CWFGM_Fuel_O1a,	31, 31,	RGB(255, 255, 190),	NULL,	NULL },
	{ _T("Standing Grass"),						_T("O-1b"),		{ _T("O1b"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },							&CLSID_CWFGM_Fuel_O1b,	32, 32,	RGB(230, 230, 0),	NULL,	NULL },
	{ _T("Grass"),											_T("O-1a/O-1b"),	{ _T("O1a/O1b"), _T("O1a/b"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },						&CLSID_CWFGM_Fuel_O1ab,	33, 33,	RGB(255, 255, 190),	NULL,	NULL },

	{ _T("Boreal Mixedwood - Leafless"),				_T("M-1"),		{ _T("M-1a"), _T("M1a"), _T("M1 (pc-grid)"), _T("M-1 (green-up grid/pc-grid)"), _T("M1 (green-up grid/pc-grid)"), _T("M-1 (pc-grid)"), _T("M1"), _T("") }, &CLSID_CWFGM_Fuel_M1,	40, 40,	RGB(255, 211, 127),	NULL,	M_fueltype[0] },
	{ _T("Boreal Mixedwood - Green"),				_T("M-2"),		{ _T("M-2a"), _T("M2a"), _T("M2 (pc-grid)"), _T("M-2 (green-up grid/pc-grid)"), _T("M2 (green-up grid/pc-grid)"), _T("M-2 (pc-grid)"), _T("M2"), _T("") }, &CLSID_CWFGM_Fuel_M2,	50, 50,	RGB(255, 170, 0), 		NULL,	M_fueltype[0] },
	{ _T("Boreal Mixedwood"),					_T("M-1/M-2"),		{ _T("M-1/M-2a"), _T("M1/M2a"), _T("M-1/M-2 (pc-grid)"), _T("M1/M2 (pc-grid)"), _T("M1/M2 (green-up grid/pc-grid)"), _T("M-1/M-2 (green-up grid/pc-grid)"), _T("M1/M2"), _T("") }, &CLSID_CWFGM_Fuel_M1M2,	60, 60,	RGB(255, 211, 127),	NULL,	M_fueltype[0] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless"),			_T("M-3"),		{ _T("M-3a"), _T("M3a"), _T("M3 (pdf-grid)"), _T("M-3 (green-up grid/pdf-grid)"), _T("M3 (green-up grid/pdf-grid)"), _T("M-3 (pdf-grid)"), _T("M3"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless") }, &CLSID_CWFGM_Fuel_M3,	70, 70,	RGB(99, 0, 0),	NULL,	MD_fueltype[0] },
	{ _T("Dead Balsam Fir Mixedwood - Green"),			_T("M-4"),		{ _T("M-4a"), _T("M4a"), _T("M4 (pdf-grid)"), _T("M-4 (green-up grid/pdf-grid)"), _T("M4 (green-up grid/pdf-grid)"), _T("M-4 (pdf-grid)"), _T("M4"), _T("M-4 Dead Balsam Fir / Mixedwood - Green") }, &CLSID_CWFGM_Fuel_M4,	80, 80,	RGB(170, 0, 0),	NULL,	MD_fueltype[0] },
	{ _T("Dead Balsam Fir Mixedwood"),				_T("M-3/M-4"),		{ _T("M-3/M-4a"), _T("M3/M4a"), _T("M3/M4 (pdf-grid)"), _T("M-3/M-4 (pdf-grid)"), _T("M3/M4 (green-up grid/pdf-grid)"), _T("M-3/M-4 (green-up grid/pdf-grid)"), _T("M3/M4"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood") }, &CLSID_CWFGM_Fuel_M3M4,	90, 90,	RGB(99, 0, 0),	NULL,	MD_fueltype[0] },

	{ _T("Not Available"),						_T("Non-fuel"),		{ _T("N/A"), _T("NA"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },				&CLSID_CWFGM_Fuel_Non,	100,100,RGB(255, 255, 255),	NULL,	NULL },
	{ _T("Non-fuel"),						_T("Non-fuel"),		{ _T("Non fuel"), _T("NF"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },				&CLSID_CWFGM_Fuel_Non,	101,101,RGB(130, 130, 130),	NULL,	NULL },
	{ _T("Water"),							_T("Non-fuel"),		{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },					&CLSID_CWFGM_Fuel_Non,	102,102,RGB(115, 223, 255),	NULL,	NULL },
	{ _T("Unknown"),						_T("Non-fuel"),		{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },					&CLSID_CWFGM_Fuel_Non,	103,103,RGB(0, 0, 0),		NULL,	NULL },
	{ _T("Unclassified"),						_T("Non-fuel"),		{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },					&CLSID_CWFGM_Fuel_Non,	104,104,RGB(166, 166, 166),	NULL,	NULL },
	{ _T("Vegetated Non-Fuel"),					_T("Non-fuel"),		{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },					&CLSID_CWFGM_Fuel_Non,	105,105,RGB(204, 204, 204),	NULL,	NULL },

	{ _T("Boreal Mixedwood - Leafless (05% Conifer)"),		_T("M-1 (05 PC)"),	{ _T("M-1b"), _T("M1b"), _T("M1 (05%)"), _T("M-1 (5%)"), _T("M1 (5%)"), _T("M-1 (05%)"), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1, 	405,405,RGB(255, 211, 127),	NULL,	M_fueltype[1] },
	{ _T("Boreal Mixedwood - Leafless (10% Conifer)"),		_T("M-1 (10 PC)"),	{ _T("M-1c"), _T("M1c"), _T("M1 (10%)"), _T("M-1 (10%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	410,410,RGB(255, 211, 127),	NULL,	M_fueltype[2] },
	{ _T("Boreal Mixedwood - Leafless (15% Conifer)"),		_T("M-1 (15 PC)"),	{ _T("M-1d"), _T("M1d"), _T("M1 (15%)"), _T("M-1 (15%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	415,415,RGB(255, 211, 127),	NULL,	M_fueltype[3] },
	{ _T("Boreal Mixedwood - Leafless (20% Conifer)"),		_T("M-1 (20 PC)"),	{ _T("M-1e"), _T("M1e"), _T("M1 (20%)"), _T("M-1 (20%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	420,420,RGB(255, 211, 127),	NULL,	M_fueltype[4] },
	{ _T("Boreal Mixedwood - Leafless (25% Conifer)"),		_T("M-1 (25 PC)"),	{ _T("M-1f"), _T("M1f"), _T("M1 (25%)"), _T("M-1 (25%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	425,425,RGB(255, 211, 127),	NULL,	M_fueltype[5] },
	{ _T("Boreal Mixedwood - Leafless (30% Conifer)"),		_T("M-1 (30 PC)"),	{ _T("M-1g"), _T("M1g"), _T("M1 (30%)"), _T("M-1 (30%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	430,430,RGB(255, 211, 127),	NULL,	M_fueltype[6] },
	{ _T("Boreal Mixedwood - Leafless (35% Conifer)"),		_T("M-1 (35 PC)"),	{ _T("M-1h"), _T("M1h"), _T("M1 (35%)"), _T("M-1 (35%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	435,435,RGB(255, 211, 127),	NULL,	M_fueltype[7] },
	{ _T("Boreal Mixedwood - Leafless (40% Conifer)"),		_T("M-1 (40 PC)"),	{ _T("M-1i"), _T("M1i"), _T("M1 (40%)"), _T("M-1 (40%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	440,440,RGB(255, 211, 127),	NULL,	M_fueltype[8] },
	{ _T("Boreal Mixedwood - Leafless (45% Conifer)"),		_T("M-1 (45 PC)"),	{ _T("M-1j"), _T("M1j"), _T("M1 (45%)"), _T("M-1 (45%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	445,445,RGB(255, 211, 127),	NULL,	M_fueltype[9] },
	{ _T("Boreal Mixedwood - Leafless (50% Conifer)"),		_T("M-1 (50 PC)"),	{ _T("M-1k"), _T("M1k"), _T("M1 (50%)"), _T("M-1 (50%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	450,450,RGB(255, 211, 127),	NULL,	M_fueltype[10] },
	{ _T("Boreal Mixedwood - Leafless (55% Conifer)"),		_T("M-1 (55 PC)"),	{ _T("M-1l"), _T("M1l"), _T("M1 (55%)"), _T("M-1 (55%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	455,455,RGB(255, 211, 127),	NULL,	M_fueltype[11] },
	{ _T("Boreal Mixedwood - Leafless (60% Conifer)"),		_T("M-1 (60 PC)"),	{ _T("M-1m"), _T("M1m"), _T("M1 (60%)"), _T("M-1 (60%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	460,460,RGB(255, 211, 127),	NULL,	M_fueltype[12] },
	{ _T("Boreal Mixedwood - Leafless (65% Conifer)"),		_T("M-1 (65 PC)"),	{ _T("M-1n"), _T("M1n"), _T("M1 (65%)"), _T("M-1 (65%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	465,465,RGB(255, 211, 127),	NULL,	M_fueltype[13] },
	{ _T("Boreal Mixedwood - Leafless (70% Conifer)"),		_T("M-1 (70 PC)"),	{ _T("M-1o"), _T("M1o"), _T("M1 (70%)"), _T("M-1 (70%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	470,470,RGB(255, 211, 127),	NULL,	M_fueltype[14] },
	{ _T("Boreal Mixedwood - Leafless (75% Conifer)"),		_T("M-1 (75 PC)"),	{ _T("M-1p"), _T("M1p"), _T("M1 (75%)"), _T("M-1 (75%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	475,475,RGB(255, 211, 127),	NULL,	M_fueltype[15] },
	{ _T("Boreal Mixedwood - Leafless (80% Conifer)"),		_T("M-1 (80 PC)"),	{ _T("M-1q"), _T("M1q"), _T("M1 (80%)"), _T("M-1 (80%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	480,480,RGB(255, 211, 127),	NULL,	M_fueltype[16] },
	{ _T("Boreal Mixedwood - Leafless (85% Conifer)"),		_T("M-1 (85 PC)"),	{ _T("M-1r"), _T("M1r"), _T("M1 (85%)"), _T("M-1 (85%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	485,485,RGB(255, 211, 127),	NULL,	M_fueltype[17] },
	{ _T("Boreal Mixedwood - Leafless (90% Conifer)"),		_T("M-1 (90 PC)"),	{ _T("M-1s"), _T("M1s"), _T("M1 (90%)"), _T("M-1 (90%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	490,490,RGB(255, 211, 127),	NULL,	M_fueltype[18] },
	{ _T("Boreal Mixedwood - Leafless (95% Conifer)"),		_T("M-1 (95 PC)"),	{ _T("M-1t"), _T("M1t"), _T("M1 (95%)"), _T("M-1 (95%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M1,	495,495,RGB(255, 211, 127),	NULL,	M_fueltype[19] },

	{ _T("Boreal Mixedwood - Green (05% Conifer)"),			_T("M-2 (05 PC)"),	{ _T("M-2b"), _T("M2b"), _T("M2 (05%)"), _T("M-2 (5%)"), _T("M2 (5%)"), _T("M-2 (05%)"), _T(""), _T("") }, &CLSID_CWFGM_Fuel_M2,	505,505,RGB(255, 170, 0), 	NULL,	M_fueltype[1] },
	{ _T("Boreal Mixedwood - Green (10% Conifer)"),			_T("M-2 (10 PC)"),	{ _T("M-2c"), _T("M2c"), _T("M2 (10%)"), _T("M-2 (10%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	510,510,RGB(255, 170, 0),	NULL,	M_fueltype[2] },
	{ _T("Boreal Mixedwood - Green (15% Conifer)"),			_T("M-2 (15 PC)"),	{ _T("M-2d"), _T("M2d"), _T("M2 (15%)"), _T("M-2 (15%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	515,515,RGB(255, 170, 0), 	NULL,	M_fueltype[3] },
	{ _T("Boreal Mixedwood - Green (20% Conifer)"),			_T("M-2 (20 PC)"),	{ _T("M-2e"), _T("M2e"), _T("M2 (20%)"), _T("M-2 (20%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	520,520,RGB(255, 170, 0), 	NULL,	M_fueltype[4] },
	{ _T("Boreal Mixedwood - Green (25% Conifer)"),			_T("M-2 (25 PC)"),	{ _T("M-2f"), _T("M2f"), _T("M2 (25%)"), _T("M-2 (25%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	525,525,RGB(255, 170, 0), 	NULL,	M_fueltype[5] },
	{ _T("Boreal Mixedwood - Green (30% Conifer)"),			_T("M-2 (30 PC)"),	{ _T("M-2g"), _T("M2g"), _T("M2 (30%)"), _T("M-2 (30%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	530,530,RGB(255, 170, 0), 	NULL,	M_fueltype[6] },
	{ _T("Boreal Mixedwood - Green (35% Conifer)"),			_T("M-2 (35 PC)"),	{ _T("M-2h"), _T("M2h"), _T("M2 (35%)"), _T("M-2 (35%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	535,535,RGB(255, 170, 0), 	NULL,	M_fueltype[7] },
	{ _T("Boreal Mixedwood - Green (40% Conifer)"),			_T("M-2 (40 PC)"),	{ _T("M-2i"), _T("M2i"), _T("M2 (40%)"), _T("M-2 (40%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	540,540,RGB(255, 170, 0),	NULL,	M_fueltype[8] },
	{ _T("Boreal Mixedwood - Green (45% Conifer)"),			_T("M-2 (45 PC)"),	{ _T("M-2j"), _T("M2j"), _T("M2 (45%)"), _T("M-2 (45%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	545,545,RGB(255, 170, 0), 	NULL,	M_fueltype[9] },
	{ _T("Boreal Mixedwood - Green (50% Conifer)"),			_T("M-2 (50 PC)"),	{ _T("M-2k"), _T("M2k"), _T("M2 (50%)"), _T("M-2 (50%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	550,550,RGB(255, 170, 0),	NULL,	M_fueltype[10] },
	{ _T("Boreal Mixedwood - Green (55% Conifer)"),			_T("M-2 (55 PC)"),	{ _T("M-2l"), _T("M2l"), _T("M2 (55%)"), _T("M-2 (55%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	555,555,RGB(255, 170, 0), 	NULL,	M_fueltype[11] },
	{ _T("Boreal Mixedwood - Green (60% Conifer)"),			_T("M-2 (60 PC)"),	{ _T("M-2m"), _T("M2m"), _T("M2 (60%)"), _T("M-2 (60%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	560,560,RGB(255, 170, 0), 	NULL,	M_fueltype[12] },
	{ _T("Boreal Mixedwood - Green (65% Conifer)"),			_T("M-2 (65 PC)"),	{ _T("M-2n"), _T("M2n"), _T("M2 (65%)"), _T("M-2 (65%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	565,565,RGB(255, 170, 0), 	NULL,	M_fueltype[13] },
	{ _T("Boreal Mixedwood - Green (70% Conifer)"),			_T("M-2 (70 PC)"),	{ _T("M-2o"), _T("M2o"), _T("M2 (70%)"), _T("M-2 (70%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	570,570,RGB(255, 170, 0), 	NULL,	M_fueltype[14] },
	{ _T("Boreal Mixedwood - Green (75% Conifer)"),			_T("M-2 (75 PC)"),	{ _T("M-2p"), _T("M2p"), _T("M2 (75%)"), _T("M-2 (75%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	575,575,RGB(255, 170, 0), 	NULL,	M_fueltype[15] },
	{ _T("Boreal Mixedwood - Green (80% Conifer)"),			_T("M-2 (80 PC)"),	{ _T("M-2q"), _T("M2q"), _T("M2 (80%)"), _T("M-2 (80%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	580,580,RGB(255, 170, 0), 	NULL,	M_fueltype[16] },
	{ _T("Boreal Mixedwood - Green (85% Conifer)"),			_T("M-2 (85 PC)"),	{ _T("M-2r"), _T("M2r"), _T("M2 (85%)"), _T("M-2 (85%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	585,585,RGB(255, 170, 0), 	NULL,	M_fueltype[17] },
	{ _T("Boreal Mixedwood - Green (90% Conifer)"),			_T("M-2 (90 PC)"),	{ _T("M-2s"), _T("M2s"), _T("M2 (90%)"), _T("M-2 (90%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	590,590,RGB(255, 170, 0), 	NULL,	M_fueltype[18] },
	{ _T("Boreal Mixedwood - Green (95% Conifer)"),			_T("M-2 (95 PC)"),	{ _T("M-2t"), _T("M2t"), _T("M2 (95%)"), _T("M-2 (95%)"), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	595,595,RGB(255, 170, 0), 	NULL,	M_fueltype[19] },

	{ _T("Boreal Mixedwood (05% Conifer)"),				_T("M-1/M-2 (05 PC)"),	{ _T("M-1/M-2b"), _T("M1/M2b"), _T("M1/M2 (05%)"), _T("M-1/M-2 (5%)"), _T("M1/M2 (5%)"), _T("M-1/M-2 (05%)"), _T(""), _T("") }, &CLSID_CWFGM_Fuel_M1M2,605,605,RGB(255, 211, 127),	NULL,	M_fueltype[1] },
	{ _T("Boreal Mixedwood (10% Conifer)"),				_T("M-1/M-2 (10 PC)"),	{ _T("M-1/M-2c"), _T("M1/M2c"), _T("M1/M2 (10%)"), _T("M-1/M-2 (10%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	610,610,RGB(255, 211, 127),	NULL,	M_fueltype[2] },
	{ _T("Boreal Mixedwood (15% Conifer)"),				_T("M-1/M-2 (15 PC)"),	{ _T("M-1/M-2d"), _T("M1/M2d"), _T("M1/M2 (15%)"), _T("M-1/M-2 (15%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	615,615,RGB(255, 211, 127),	NULL,	M_fueltype[3] },
	{ _T("Boreal Mixedwood (20% Conifer)"),				_T("M-1/M-2 (20 PC)"),	{ _T("M-1/M-2e"), _T("M1/M2e"), _T("M1/M2 (20%)"), _T("M-1/M-2 (20%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	620,620,RGB(255, 211, 127),	NULL,	M_fueltype[4] },
	{ _T("Boreal Mixedwood (25% Conifer)"),				_T("M-1/M-2 (25 PC)"),	{ _T("M-1/M-2f"), _T("M1/M2f"), _T("M1/M2 (25%)"), _T("M-1/M-2 (25%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	625,625,RGB(255, 211, 127),	NULL,	M_fueltype[5] },
	{ _T("Boreal Mixedwood (30% Conifer)"),				_T("M-1/M-2 (30 PC)"),	{ _T("M-1/M-2g"), _T("M1/M2g"), _T("M1/M2 (30%)"), _T("M-1/M-2 (30%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	630,630,RGB(255, 211, 127),	NULL,	M_fueltype[6] },
	{ _T("Boreal Mixedwood (35% Conifer)"),				_T("M-1/M-2 (35 PC)"),	{ _T("M-1/M-2h"), _T("M1/M2h"), _T("M1/M2 (35%)"), _T("M-1/M-2 (35%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	635,635,RGB(255, 211, 127),	NULL,	M_fueltype[7] },
	{ _T("Boreal Mixedwood (40% Conifer)"),				_T("M-1/M-2 (40 PC)"),	{ _T("M-1/M-2i"), _T("M1/M2i"), _T("M1/M2 (40%)"), _T("M-1/M-2 (40%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	640,640,RGB(255, 211, 127),	NULL,	M_fueltype[8] },
	{ _T("Boreal Mixedwood (45% Conifer)"),				_T("M-1/M-2 (45 PC)"),	{ _T("M-1/M-2j"), _T("M1/M2j"), _T("M1/M2 (45%)"), _T("M-1/M-2 (45%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	645,645,RGB(255, 211, 127),	NULL,	M_fueltype[9] },
	{ _T("Boreal Mixedwood (50% Conifer)"),				_T("M-1/M-2 (50 PC)"),	{ _T("M-1/M-2k"), _T("M1/M2k"), _T("M1/M2 (50%)"), _T("M-1/M-2 (50%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	650,650,RGB(255, 211, 127),	NULL,	M_fueltype[10] },
	{ _T("Boreal Mixedwood (55% Conifer)"),				_T("M-1/M-2 (55 PC)"),	{ _T("M-1/M-2l"), _T("M1/M2l"), _T("M1/M2 (55%)"), _T("M-1/M-2 (55%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	655,655,RGB(255, 211, 127),	NULL,	M_fueltype[11] },
	{ _T("Boreal Mixedwood (60% Conifer)"),				_T("M-1/M-2 (60 PC)"),	{ _T("M-1/M-2m"), _T("M1/M2m"), _T("M1/M2 (60%)"), _T("M-1/M-2 (60%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	660,660,RGB(255, 211, 127),	NULL,	M_fueltype[12] },
	{ _T("Boreal Mixedwood (65% Conifer)"),				_T("M-1/M-2 (65 PC)"),	{ _T("M-1/M-2n"), _T("M1/M2n"), _T("M1/M2 (65%)"), _T("M-1/M-2 (65%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	665,665,RGB(255, 211, 127),	NULL,	M_fueltype[13] },
	{ _T("Boreal Mixedwood (70% Conifer)"),				_T("M-1/M-2 (70 PC)"),	{ _T("M-1/M-2o"), _T("M1/M2o"), _T("M1/M2 (70%)"), _T("M-1/M-2 (70%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	670,670,RGB(255, 211, 127),	NULL,	M_fueltype[14] },
	{ _T("Boreal Mixedwood (75% Conifer)"),				_T("M-1/M-2 (75 PC)"),	{ _T("M-1/M-2p"), _T("M1/M2p"), _T("M1/M2 (75%)"), _T("M-1/M-2 (75%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	675,675,RGB(255, 211, 127),	NULL,	M_fueltype[15] },
	{ _T("Boreal Mixedwood (80% Conifer)"),				_T("M-1/M-2 (80 PC)"),	{ _T("M-1/M-2q"), _T("M1/M2q"), _T("M1/M2 (80%)"), _T("M-1/M-2 (80%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	680,680,RGB(255, 211, 127),	NULL,	M_fueltype[16] },
	{ _T("Boreal Mixedwood (85% Conifer)"),				_T("M-1/M-2 (85 PC)"),	{ _T("M-1/M-2r"), _T("M1/M2r"), _T("M1/M2 (85%)"), _T("M-1/M-2 (85%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	685,685,RGB(255, 211, 127),	NULL,	M_fueltype[17] },
	{ _T("Boreal Mixedwood (90% Conifer)"),				_T("M-1/M-2 (90 PC)"),	{ _T("M-1/M-2s"), _T("M1/M2s"), _T("M1/M2 (90%)"), _T("M-1/M-2 (90%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	690,690,RGB(255, 211, 127),	NULL,	M_fueltype[18] },
	{ _T("Boreal Mixedwood (95% Conifer)"),				_T("M-1/M-2 (95 PC)"),	{ _T("M-1/M-2t"), _T("M1/M2t"), _T("M1/M2 (95%)"), _T("M-1/M-2 (95%)"), _T(""), _T(""), _T(""), _T("") },	&CLSID_CWFGM_Fuel_M1M2,	695,695,RGB(255, 211, 127),	NULL,	M_fueltype[19] },

	{ _T("Dead Balsam Fir Mixedwood - Leafless (05% Dead Fir)"),	_T("M-3 (05 PDF)"),	{ _T("M-3b"), _T("M3b"), _T("M-3 (5% pdf)"), _T("M3 (5% pdf)"), _T("M-3 (05%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (05% pdf)"), _T("M-3 (05% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	705,705,RGB(99, 0, 0),	NULL,	MD_fueltype[1] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (10% Dead Fir)"),	_T("M-3 (10 PDF)"),	{ _T("M-3c"), _T("M3c"), _T("M-3 (10%)"), _T("M3 (10% pdf)"), _T("M3 (10%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (10% pdf)"), _T("M-3 (10% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	710,710,RGB(99, 0, 0),	NULL,	MD_fueltype[2] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (15% Dead Fir)"),	_T("M-3 (15 PDF)"),	{ _T("M-3d"), _T("M3d"), _T("M-3 (15%)"), _T("M3 (15% pdf)"), _T("M3 (15%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (15% pdf)"), _T("M-3 (15% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	715,715,RGB(99, 0, 0),	NULL,	MD_fueltype[3] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (20% Dead Fir)"),	_T("M-3 (20 PDF)"),	{ _T("M-3e"), _T("M3e"), _T("M-3 (20%)"), _T("M3 (20% pdf)"), _T("M3 (20%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (20% pdf)"), _T("M-3 (20% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	720,720,RGB(99, 0, 0),	NULL,	MD_fueltype[4] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (25% Dead Fir)"),	_T("M-3 (25 PDF)"),	{ _T("M-3f"), _T("M3f"), _T("M-3 (25%)"), _T("M3 (25% pdf)"), _T("M3 (25%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (25% pdf)"), _T("M-3 (25% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	725,725,RGB(99, 0, 0),	NULL,	MD_fueltype[5] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (30% Dead Fir)"),	_T("M-3 (30 PDF)"),	{ _T("M-3g"), _T("M3g"), _T("M-3 (30%)"), _T("M3 (30% pdf)"), _T("M3 (30%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (30% pdf)"), _T("M-3 (30% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	730,730,RGB(99, 0, 0),	NULL,	MD_fueltype[6] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (35% Dead Fir)"),	_T("M-3 (35 PDF)"),	{ _T("M-3h"), _T("M3h"), _T("M-3 (35%)"), _T("M3 (35% pdf)"), _T("M3 (35%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (35% pdf)"), _T("M-3 (35% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	735,735,RGB(99, 0, 0),	NULL,	MD_fueltype[7] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (40% Dead Fir)"),	_T("M-3 (40 PDF)"),	{ _T("M-3i"), _T("M3i"), _T("M-3 (40%)"), _T("M3 (40% pdf)"), _T("M3 (40%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (40% pdf)"), _T("M-3 (40% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	740,740,RGB(99, 0, 0),	NULL,	MD_fueltype[8] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (45% Dead Fir)"),	_T("M-3 (45 PDF)"),	{ _T("M-3j"), _T("M3j"), _T("M-3 (45%)"), _T("M3 (45% pdf)"), _T("M3 (45%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (45% pdf)"), _T("M-3 (45% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	745,745,RGB(99, 0, 0),	NULL,	MD_fueltype[9] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (50% Dead Fir)"),	_T("M-3 (50 PDF)"),	{ _T("M-3k"), _T("M3k"), _T("M-3 (50%)"), _T("M3 (50% pdf)"), _T("M3 (50%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (50% pdf)"), _T("M-3 (50% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	750,750,RGB(99, 0, 0),	NULL,	MD_fueltype[10] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (55% Dead Fir)"),	_T("M-3 (55 PDF)"),	{ _T("M-3l"), _T("M3l"), _T("M-3 (55%)"), _T("M3 (55% pdf)"), _T("M3 (55%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (55% pdf)"), _T("M-3 (55% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	755,755,RGB(99, 0, 0),	NULL,	MD_fueltype[11] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (60% Dead Fir)"),	_T("M-3 (60 PDF)"),	{ _T("M-3m"), _T("M3m"), _T("M-3 (60%)"), _T("M3 (60% pdf)"), _T("M3 (60%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (60% pdf)"), _T("M-3 (60% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	760,760,RGB(99, 0, 0),	NULL,	MD_fueltype[12] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (65% Dead Fir)"),	_T("M-3 (65 PDF)"),	{ _T("M-3n"), _T("M3n"), _T("M-3 (65%)"), _T("M3 (65% pdf)"), _T("M3 (65%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (65% pdf)"), _T("M-3 (65% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	765,765,RGB(99, 0, 0),	NULL,	MD_fueltype[13] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (70% Dead Fir)"),	_T("M-3 (70 PDF)"),	{ _T("M-3o"), _T("M3o"), _T("M-3 (70%)"), _T("M3 (70% pdf)"), _T("M3 (70%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (70% pdf)"), _T("M-3 (70% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	770,770,RGB(99, 0, 0),	NULL,	MD_fueltype[14] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (75% Dead Fir)"),	_T("M-3 (75 PDF)"),	{ _T("M-3p"), _T("M3p"), _T("M-3 (75%)"), _T("M3 (75% pdf)"), _T("M3 (75%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (75% pdf)"), _T("M-3 (75% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	775,775,RGB(99, 0, 0),	NULL,	MD_fueltype[15] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (80% Dead Fir)"),	_T("M-3 (80 PDF)"),	{ _T("M-3q"), _T("M3q"), _T("M-3 (80%)"), _T("M3 (80% pdf)"), _T("M3 (80%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (80% pdf)"), _T("M-3 (80% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	780,780,RGB(99, 0, 0),	NULL,	MD_fueltype[16] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (85% Dead Fir)"),	_T("M-3 (85 PDF)"),	{ _T("M-3r"), _T("M3r"), _T("M-3 (85%)"), _T("M3 (85% pdf)"), _T("M3 (85%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (85% pdf)"), _T("M-3 (85% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	785,785,RGB(99, 0, 0),	NULL,	MD_fueltype[17] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (90% Dead Fir)"),	_T("M-3 (90 PDF)"),	{ _T("M-3s"), _T("M3s"), _T("M-3 (90%)"), _T("M3 (90% pdf)"), _T("M3 (90%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (90% pdf)"), _T("M-3 (90% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	790,790,RGB(99, 0, 0),	NULL,	MD_fueltype[18] },
	{ _T("Dead Balsam Fir Mixedwood - Leafless (95% Dead Fir)"),	_T("M-3 (95 PDF)"),	{ _T("M-3t"), _T("M3t"), _T("M-3 (95%)"), _T("M3 (95% pdf)"), _T("M3 (95%)"), _T("M-3 Dead Balsam Fir / Mixedwood - Leafless (95% pdf)"), _T("M-3 (95% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3,	795,795,RGB(99, 0, 0),	NULL,	MD_fueltype[19] },

	{ _T("Dead Balsam Fir Mixedwood - Green (05% Dead Fir)"),	_T("M-4 (05 PDF)"),	{ _T("M-4b"), _T("M4b"), _T("M-4 (5% pdf)"), _T("M4 (5%)"), _T("M-4 (05%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (05% pdf)"), _T("M-4 (05% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	805,805,RGB(170, 0, 0),	NULL,	MD_fueltype[1] },
	{ _T("Dead Balsam Fir Mixedwood - Green (10% Dead Fir)"),	_T("M-4 (10 PDF)"),	{ _T("M-4c"), _T("M4c"), _T("M-4 (10%)"), _T("M4 (10% pdf)"), _T("M4 (10%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (10% pdf)"), _T("M-4 (10% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	810,810,RGB(170, 0, 0),	NULL,	MD_fueltype[2] },
	{ _T("Dead Balsam Fir Mixedwood - Green (15% Dead Fir)"),	_T("M-4 (15 PDF)"),	{ _T("M-4d"), _T("M4d"), _T("M-4 (15%)"), _T("M4 (15% pdf)"), _T("M4 (15%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (15% pdf)"), _T("M-4 (15% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	815,815,RGB(170, 0, 0),	NULL,	MD_fueltype[3] },
	{ _T("Dead Balsam Fir Mixedwood - Green (20% Dead Fir)"),	_T("M-4 (20 PDF)"),	{ _T("M-4e"), _T("M4e"), _T("M-4 (20%)"), _T("M4 (20% pdf)"), _T("M4 (20%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (20% pdf)"), _T("M-4 (20% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	820,820,RGB(170, 0, 0),	NULL,	MD_fueltype[4] },
	{ _T("Dead Balsam Fir Mixedwood - Green (25% Dead Fir)"),	_T("M-4 (25 PDF)"),	{ _T("M-4f"), _T("M4f"), _T("M-4 (25%)"), _T("M4 (25% pdf)"), _T("M4 (25%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (25% pdf)"), _T("M-4 (25% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	825,825,RGB(170, 0, 0),	NULL,	MD_fueltype[5] },
	{ _T("Dead Balsam Fir Mixedwood - Green (30% Dead Fir)"),	_T("M-4 (30 PDF)"),	{ _T("M-4g"), _T("M4g"), _T("M-4 (30%)"), _T("M4 (30% pdf)"), _T("M4 (30%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (30% pdf)"), _T("M-4 (30% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	830,830,RGB(170, 0, 0),	NULL,	MD_fueltype[6] },
	{ _T("Dead Balsam Fir Mixedwood - Green (35% Dead Fir)"),	_T("M-4 (35 PDF)"),	{ _T("M-4h"), _T("M4h"), _T("M-4 (35%)"), _T("M4 (35% pdf)"), _T("M4 (35%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (35% pdf)"), _T("M-4 (35% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	835,835,RGB(170, 0, 0),	NULL,	MD_fueltype[7] },
	{ _T("Dead Balsam Fir Mixedwood - Green (40% Dead Fir)"),	_T("M-4 (40 PDF)"),	{ _T("M-4i"), _T("M4i"), _T("M-4 (40%)"), _T("M4 (40% pdf)"), _T("M4 (40%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (40% pdf)"), _T("M-4 (40% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	840,840,RGB(170, 0, 0),	NULL,	MD_fueltype[8] },
	{ _T("Dead Balsam Fir Mixedwood - Green (45% Dead Fir)"),	_T("M-4 (45 PDF)"),	{ _T("M-4j"), _T("M4j"), _T("M-4 (45%)"), _T("M4 (45% pdf)"), _T("M4 (45%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (45% pdf)"), _T("M-4 (45% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	845,845,RGB(170, 0, 0),	NULL,	MD_fueltype[9] },
	{ _T("Dead Balsam Fir Mixedwood - Green (50% Dead Fir)"),	_T("M-4 (50 PDF)"),	{ _T("M-4k"), _T("M4k"), _T("M-4 (50%)"), _T("M4 (50% pdf)"), _T("M4 (50%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (50% pdf)"), _T("M-4 (50% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	850,850,RGB(170, 0, 0),	NULL,	MD_fueltype[10] },
	{ _T("Dead Balsam Fir Mixedwood - Green (55% Dead Fir)"),	_T("M-4 (55 PDF)"),	{ _T("M-4l"), _T("M4l"), _T("M-4 (55%)"), _T("M4 (55% pdf)"), _T("M4 (55%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (55% pdf)"), _T("M-4 (55% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	855,855,RGB(170, 0, 0),	NULL,	MD_fueltype[11] },
	{ _T("Dead Balsam Fir Mixedwood - Green (60% Dead Fir)"),	_T("M-4 (60 PDF)"),	{ _T("M-4m"), _T("M4m"), _T("M-4 (60%)"), _T("M4 (60% pdf)"), _T("M4 (60%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (60% pdf)"), _T("M-4 (60% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	860,860,RGB(170, 0, 0),	NULL,	MD_fueltype[12] },
	{ _T("Dead Balsam Fir Mixedwood - Green (65% Dead Fir)"),	_T("M-4 (65 PDF)"),	{ _T("M-4n"), _T("M4n"), _T("M-4 (65%)"), _T("M4 (65% pdf)"), _T("M4 (65%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (65% pdf)"), _T("M-4 (65% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	865,865,RGB(170, 0, 0),	NULL,	MD_fueltype[13] },
	{ _T("Dead Balsam Fir Mixedwood - Green (70% Dead Fir)"),	_T("M-4 (70 PDF)"),	{ _T("M-4o"), _T("M4o"), _T("M-4 (70%)"), _T("M4 (70% pdf)"), _T("M4 (70%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (70% pdf)"), _T("M-4 (70% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	870,870,RGB(170, 0, 0),	NULL,	MD_fueltype[14] },
	{ _T("Dead Balsam Fir Mixedwood - Green (75% Dead Fir)"),	_T("M-4 (75 PDF)"),	{ _T("M-4p"), _T("M4p"), _T("M-4 (75%)"), _T("M4 (75% pdf)"), _T("M4 (75%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (75% pdf)"), _T("M-4 (75% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	875,875,RGB(170, 0, 0),	NULL,	MD_fueltype[15] },
	{ _T("Dead Balsam Fir Mixedwood - Green (80% Dead Fir)"),	_T("M-4 (80 PDF)"),	{ _T("M-4q"), _T("M4q"), _T("M-4 (80%)"), _T("M4 (80% pdf)"), _T("M4 (80%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (80% pdf)"), _T("M-4 (80% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	880,880,RGB(170, 0, 0),	NULL,	MD_fueltype[16] },
	{ _T("Dead Balsam Fir Mixedwood - Green (85% Dead Fir)"),	_T("M-4 (85 PDF)"),	{ _T("M-4r"), _T("M4r"), _T("M-4 (85%)"), _T("M4 (85% pdf)"), _T("M4 (85%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (85% pdf)"), _T("M-4 (85% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	885,885,RGB(170, 0, 0),	NULL,	MD_fueltype[17] },
	{ _T("Dead Balsam Fir Mixedwood - Green (90% Dead Fir)"),	_T("M-4 (90 PDF)"),	{ _T("M-4s"), _T("M4s"), _T("M-4 (90%)"), _T("M4 (90% pdf)"), _T("M4 (90%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (90% pdf)"), _T("M-4 (90% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	890,890,RGB(170, 0, 0),	NULL,	MD_fueltype[18] },
	{ _T("Dead Balsam Fir Mixedwood - Green (95% Dead Fir)"),	_T("M-4 (95 PDF)"),	{ _T("M-4t"), _T("M4t"), _T("M-4 (95%)"), _T("M4 (95% pdf)"), _T("M4 (95%)"), _T("M-4 Dead Balsam Fir / Mixedwood - Green (95% pdf)"), _T("M-4 (95% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M4,	895,895,RGB(170, 0, 0),	NULL,	MD_fueltype[19] },

	{ _T("Dead Balsam Fir Mixedwood (05% Dead Fir)"),		_T("M-3/M-4 (05 PDF)"),	{ _T("M-3/M-4b"), _T("M3/M4b"), _T("M-3/M-4 (05%)"), _T("M-3/M-4 (5%)"), _T("M3/M4 (05% pdf)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (05% pdf)"), _T("M-3/M-4 (05% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	905,905,RGB(99, 0, 0),	NULL,	MD_fueltype[1] },
	{ _T("Dead Balsam Fir Mixedwood (10% Dead Fir)"),		_T("M-3/M-4 (10 PDF)"),	{ _T("M-3/M-4c"), _T("M3/M4c"), _T("M-3/M-4 (10%)"), _T("M3/M4 (10% pdf)"), _T("M3/M4 (10%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (10% pdf)"), _T("M-3/M-4 (10% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	910,910,RGB(99, 0, 0),	NULL,	MD_fueltype[2] },
	{ _T("Dead Balsam Fir Mixedwood (15% Dead Fir)"),		_T("M-3/M-4 (15 PDF)"),	{ _T("M-3/M-4d"), _T("M3/M4d"), _T("M-3/M-4 (15%)"), _T("M3/M4 (15% pdf)"), _T("M3/M4 (15%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (15% pdf)"), _T("M-3/M-4 (15% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	915,915,RGB(99, 0, 0),	NULL,	MD_fueltype[3] },
	{ _T("Dead Balsam Fir Mixedwood (20% Dead Fir)"),		_T("M-3/M-4 (20 PDF)"),	{ _T("M-3/M-4e"), _T("M3/M4e"), _T("M-3/M-4 (20%)"), _T("M3/M4 (20% pdf)"), _T("M3/M4 (20%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (20% pdf)"), _T("M-3/M-4 (20% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	920,920,RGB(99, 0, 0),	NULL,	MD_fueltype[4] },
	{ _T("Dead Balsam Fir Mixedwood (25% Dead Fir)"),		_T("M-3/M-4 (25 PDF)"),	{ _T("M-3/M-4f"), _T("M3/M4f"), _T("M-3/M-4 (25%)"), _T("M3/M4 (25% pdf)"), _T("M3/M4 (25%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (25% pdf)"), _T("M-3/M-4 (25% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	925,925,RGB(99, 0, 0),	NULL,	MD_fueltype[5] },
	{ _T("Dead Balsam Fir Mixedwood (30% Dead Fir)"),		_T("M-3/M-4 (30 PDF)"),	{ _T("M-3/M-4g"), _T("M3/M4g"), _T("M-3/M-4 (30%)"), _T("M3/M4 (30% pdf)"), _T("M3/M4 (30%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (30% pdf)"), _T("M-3/M-4 (30% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	930,930,RGB(99, 0, 0),	NULL,	MD_fueltype[6] },
	{ _T("Dead Balsam Fir Mixedwood (35% Dead Fir)"),		_T("M-3/M-4 (35 PDF)"),	{ _T("M-3/M-4h"), _T("M3/M4h"), _T("M-3/M-4 (35%)"), _T("M3/M4 (35% pdf)"), _T("M3/M4 (35%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (35% pdf)"), _T("M-3/M-4 (35% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	935,935,RGB(99, 0, 0),	NULL,	MD_fueltype[7] },
	{ _T("Dead Balsam Fir Mixedwood (40% Dead Fir)"),		_T("M-3/M-4 (40 PDF)"),	{ _T("M-3/M-4i"), _T("M3/M4i"), _T("M-3/M-4 (40%)"), _T("M3/M4 (40% pdf)"), _T("M3/M4 (40%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (40% pdf)"), _T("M-3/M-4 (40% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	940,940,RGB(99, 0, 0),	NULL,	MD_fueltype[8] },
	{ _T("Dead Balsam Fir Mixedwood (45% Dead Fir)"),		_T("M-3/M-4 (45 PDF)"),	{ _T("M-3/M-4j"), _T("M3/M4j"), _T("M-3/M-4 (45%)"), _T("M3/M4 (45% pdf)"), _T("M3/M4 (45%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (45% pdf)"), _T("M-3/M-4 (45% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	945,945,RGB(99, 0, 0),	NULL,	MD_fueltype[9] },
	{ _T("Dead Balsam Fir Mixedwood (50% Dead Fir)"),		_T("M-3/M-4 (50 PDF)"),	{ _T("M-3/M-4k"), _T("M3/M4k"), _T("M-3/M-4 (50%)"), _T("M3/M4 (50% pdf)"), _T("M3/M4 (50%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (50% pdf)"), _T("M-3/M-4 (50% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	950,950,RGB(99, 0, 0),	NULL,	MD_fueltype[10] },
	{ _T("Dead Balsam Fir Mixedwood (55% Dead Fir)"),		_T("M-3/M-4 (55 PDF)"),	{ _T("M-3/M-4l"), _T("M3/M4l"), _T("M-3/M-4 (55%)"), _T("M3/M4 (55% pdf)"), _T("M3/M4 (55%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (55% pdf)"), _T("M-3/M-4 (55% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	955,955,RGB(99, 0, 0),	NULL,	MD_fueltype[11] },
	{ _T("Dead Balsam Fir Mixedwood (60% Dead Fir)"),		_T("M-3/M-4 (60 PDF)"),	{ _T("M-3/M-4m"), _T("M3/M4m"), _T("M-3/M-4 (60%)"), _T("M3/M4 (60% pdf)"), _T("M3/M4 (60%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (60% pdf)"), _T("M-3/M-4 (60% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	960,960,RGB(99, 0, 0),	NULL,	MD_fueltype[12] },
	{ _T("Dead Balsam Fir Mixedwood (65% Dead Fir)"),		_T("M-3/M-4 (65 PDF)"),	{ _T("M-3/M-4n"), _T("M3/M4n"), _T("M-3/M-4 (65%)"), _T("M3/M4 (65% pdf)"), _T("M3/M4 (65%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (65% pdf)"), _T("M-3/M-4 (65% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	965,965,RGB(99, 0, 0),	NULL,	MD_fueltype[13] },
	{ _T("Dead Balsam Fir Mixedwood (70% Dead Fir)"),		_T("M-3/M-4 (70 PDF)"),	{ _T("M-3/M-4o"), _T("M3/M4o"), _T("M-3/M-4 (70%)"), _T("M3/M4 (70% pdf)"), _T("M3/M4 (70%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (70% pdf)"), _T("M-3/M-4 (70% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	970,970,RGB(99, 0, 0),	NULL,	MD_fueltype[14] },
	{ _T("Dead Balsam Fir Mixedwood (75% Dead Fir)"),		_T("M-3/M-4 (75 PDF)"),	{ _T("M-3/M-4p"), _T("M3/M4p"), _T("M-3/M-4 (75%)"), _T("M3/M4 (75% pdf)"), _T("M3/M4 (75%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (75% pdf)"), _T("M-3/M-4 (75% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	975,975,RGB(99, 0, 0),	NULL,	MD_fueltype[15] },
	{ _T("Dead Balsam Fir Mixedwood (80% Dead Fir)"),		_T("M-3/M-4 (80 PDF)"),	{ _T("M-3/M-4q"), _T("M3/M4q"), _T("M-3/M-4 (80%)"), _T("M3/M4 (80% pdf)"), _T("M3/M4 (80%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (80% pdf)"), _T("M-3/M-4 (80% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	980,980,RGB(99, 0, 0),	NULL,	MD_fueltype[16] },
	{ _T("Dead Balsam Fir Mixedwood (85% Dead Fir)"),		_T("M-3/M-4 (85 PDF)"),	{ _T("M-3/M-4r"), _T("M3/M4r"), _T("M-3/M-4 (85%)"), _T("M3/M4 (85% pdf)"), _T("M3/M4 (85%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (85% pdf)"), _T("M-3/M-4 (85% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	985,985,RGB(99, 0, 0),	NULL,	MD_fueltype[17] },
	{ _T("Dead Balsam Fir Mixedwood (90% Dead Fir)"),		_T("M-3/M-4 (90 PDF)"),	{ _T("M-3/M-4s"), _T("M3/M4s"), _T("M-3/M-4 (90%)"), _T("M3/M4 (90% pdf)"), _T("M3/M4 (90%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (90% pdf)"), _T("M-3/M-4 (90% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	990,990,RGB(99, 0, 0),	NULL,	MD_fueltype[18] },
	{ _T("Dead Balsam Fir Mixedwood (95% Dead Fir)"),		_T("M-3/M-4 (95 PDF)"),	{ _T("M-3/M-4t"), _T("M3/M4t"), _T("M-3/M-4 (95%)"), _T("M3/M4 (95% pdf)"), _T("M3/M4 (95%)"), _T("M-3/M-4 Dead Balsam Fir / Mixedwood (95% pdf)"), _T("M-3/M-4 (95% pdf)"), _T("") }, &CLSID_CWFGM_Fuel_M3M4,	995,995,RGB(99, 0, 0),	NULL,	MD_fueltype[19] },

	{ NULL,								NULL,			{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },							NULL,			0, 0,	RGB(0, 0, 0),		NULL,	NULL }
};


static const struct Project::StaticEquationData NZ_fuelequation[][3] = {
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { 0, 0 } },			// for NZ-45
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { 0, 0 } },			// for NZ-47
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { 0, 0 } },			// for NZ-51
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { 0, 0 } },	/* [3] */	// for NZ-52
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_C2 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_C2 }, { 0, 0 } },			// for NZ-54
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { 0, 0 } },			// for NZ-55
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { 0, 0 } },	/* [6] */	// for NZ-56
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_O1 }, { 0, 0 } },			// for NZ-57
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_S1 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_S1 }, { 0, 0 } },			// for NZ-63
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_S1 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_S1 }, { 0, 0 } },			// for NZ-65
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_S1 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_S1 }, { 0, 0 } },	/* [10] */	// for NZ-66
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_C2 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_C2 }, { 0, 0 } },			// for NZ-67
	{ { FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_C2 }, { FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_C2 }, { 0, 0 } }			// for NZ-69
};


static const struct Project::StaticEquationData NZ_fuelequation1[] = {
	{ FUELCOM_EQUATION_RSI, FUELCOM_EQUATION_SELECTION_RSI_CONSTANT },
	{ FUELCOM_EQUATION_RSI_GREENUP, FUELCOM_EQUATION_SELECTION_RSI_CONSTANT },
	{ FUELCOM_EQUATION_SFC, FUELCOM_EQUATION_SELECTION_SFC_O1 },
	{ FUELCOM_EQUATION_SFC_GREENUP, FUELCOM_EQUATION_SELECTION_SFC_O1 },
	{ 0, 0 }																			// for NZ-70
};


static const struct Project::StaticAttributeData NZ_fueltype[][2] = {
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.2 }, { 0, 0, 0.0 } },														// for NZ-2, NZ-15
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.8 }, { 0, 0, 0.0 } },														// for NZ-30
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.2 }, { 0, 0, 0.0 } },														// for NZ-31
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.2 }, { 0, 0, 0.0 } },	/* [3] */												// for NZ-32
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.4 }, { 0, 0, 0.0 } },														// for NZ-40
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.3 }, { 0, 0, 0.0 } },														// for NZ-41
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 2.0 }, { 0, 0, 0.0 } },														// for NZ-43
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.2 }, { 0, 0, 0.0 } },	/* [7] */												// for NZ-44
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.8 }, { 0, 0, 0.0 } },														// for NZ-46
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 1.0 }, { 0, 0, 0.0 } },														// for NZ-50
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.8 }, { 0, 0, 0.0 } },	/* [10] */												// for NZ-53
	{ { 0, FUELCOM_ATTRIBUTE_SFC_MULTIPLIER, 4.0 }, { 0, 0, 0.0 } },												// for NZ-60
	{ { 0, FUELCOM_ATTRIBUTE_CFL, 1.2 }, { 0, 0, 0.0 } },														// for NZ-61
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.35 }, { 0, 0, 0.0 } },														// for NZ-62
};


static const struct Project::StaticAttributeData NZ_fueltype1[][5] = {
	{ { 0, FUELCOM_ATTRIBUTE_SFC_S1_DEFAULTS, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_SFC_S1_DEFAULTS | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_FFC_MULTIPLIER, 4.5 },
	  { 0, FUELCOM_ATTRIBUTE_WFC_MULTIPLIER, 3.0 },
	  { 0, 0, 0.0 }
	}																				// for NZ-64
};


static const struct Project::StaticAttributeData NZ_fueltype2[][9] = {
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.8 },
	  { 0, FUELCOM_ATTRIBUTE_GFL | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.8 },
	  { 0, FUELCOM_ATTRIBUTE_A, 82.0 },
	  { 0, FUELCOM_ATTRIBUTE_B, 0.1 },
	  { 0, FUELCOM_ATTRIBUTE_C, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_Q, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_BUI0, 1.0 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 }
	},																				// for NZ-45
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_GFL | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_A, 82.0 },
	  { 0, FUELCOM_ATTRIBUTE_B, 0.1 },
	  { 0, FUELCOM_ATTRIBUTE_C, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_Q, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_BUI0, 1.0 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 }
	},																				// for NZ-47
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 3.0 },
	  { 0, FUELCOM_ATTRIBUTE_GFL | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 3.0 },
	  { 0, FUELCOM_ATTRIBUTE_A, 82.0 },
	  { 0, FUELCOM_ATTRIBUTE_B, 0.1 },
	  { 0, FUELCOM_ATTRIBUTE_C, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_Q, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_BUI0, 1.0 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 }
	},																				// for NZ-51
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 2.5 },
	  { 0, FUELCOM_ATTRIBUTE_GFL | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 2.5 },
	  { 0, FUELCOM_ATTRIBUTE_A, 82.0 },
	  { 0, FUELCOM_ATTRIBUTE_B, 0.1 },
	  { 0, FUELCOM_ATTRIBUTE_C, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_Q, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_BUI0, 1.0 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 } /* [3] */
	},																				// for NZ-52
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_GFL | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_A, 82.0 },
	  { 0, FUELCOM_ATTRIBUTE_B, 0.1 },
	  { 0, FUELCOM_ATTRIBUTE_C, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_Q, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_BUI0, 1.0 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 } /* [3] */
	},																				// for NZ-55
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_GFL | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_A, 74.0 },
	  { 0, FUELCOM_ATTRIBUTE_B, 0.1 },
	  { 0, FUELCOM_ATTRIBUTE_C, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_Q, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_BUI0, 1.0 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 } /* [3] */
	},																				// for NZ-56
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_GFL | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_A, 41.0 },
	  { 0, FUELCOM_ATTRIBUTE_B, 0.1 },
	  { 0, FUELCOM_ATTRIBUTE_C, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_Q, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_BUI0, 1.0 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 } /* [3] */
	},																				// for NZ-57
	{ { 0, FUELCOM_ATTRIBUTE_SFC_M4_DEFAULTS, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_SFC_M4_DEFAULTS | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_A, 30.0 },
	  { 0, FUELCOM_ATTRIBUTE_B, 0.08 },
	  { 0, FUELCOM_ATTRIBUTE_C, 3.0 },
	  { 0, FUELCOM_ATTRIBUTE_Q, 1.0 },
	  { 0, FUELCOM_ATTRIBUTE_BUI0, 62.0 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 } /* [3] */
	}																				// for NZ-67
};


static const struct Project::StaticAttributeData NZ_fueltype4[][18] = {
	{ { 0, FUELCOM_ATTRIBUTE_SFC_C5_DEFAULTS, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_SFC_C5_DEFAULTS | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_Q, 0.8 },
	  { 0, FUELCOM_ATTRIBUTE_BUI0, 50.0 },	// above is for the root of the mixed species
	  { 3, 1, 0.0 },			// special code to change to C5
	  { 1, FUELCOM_ATTRIBUTE_A, 30.0 },	// this next set is for C5 (1st fueltype)
	  { 1, FUELCOM_ATTRIBUTE_B, 0.0697 },
	  { 1, FUELCOM_ATTRIBUTE_C, 4.0 },
	  { 1, FUELCOM_ATTRIBUTE_Q, 0.8 },
	  { 1, FUELCOM_ATTRIBUTE_BUI0, 50.0 },
	  { 2, FUELCOM_ATTRIBUTE_A, 30.0 },	// this next set is for D2 (2nd fueltype)
	  { 2, FUELCOM_ATTRIBUTE_B, 0.0232 },
	  { 2, FUELCOM_ATTRIBUTE_C, 1.6 },
	  { 0, FUELCOM_ATTRIBUTE_PC, 0.50 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 }	
	},																				// for NZ-54
	{ { 0, FUELCOM_ATTRIBUTE_SFC_C5_DEFAULTS, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_SFC_C5_DEFAULTS | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_Q, 0.8 },
	  { 0, FUELCOM_ATTRIBUTE_BUI0, 50.0 },	// above is for the root of the mixed species
	  { 3, 1, 0.0 },				// special code to change to C5
	  { 1, FUELCOM_ATTRIBUTE_A, 30.0 },	// this next set is for C5 (1st fueltype)
	  { 1, FUELCOM_ATTRIBUTE_B, 0.0697 },
	  { 1, FUELCOM_ATTRIBUTE_C, 4.0 },
	  { 1, FUELCOM_ATTRIBUTE_Q, 0.8 },
	  { 1, FUELCOM_ATTRIBUTE_BUI0, 50.0 },
	  { 2, FUELCOM_ATTRIBUTE_A, 30.0 },	// this next set is for D2 (2nd fueltype)
	  { 2, FUELCOM_ATTRIBUTE_B, 0.0232 },
	  { 2, FUELCOM_ATTRIBUTE_C, 1.6 },
	  { 0, FUELCOM_ATTRIBUTE_PC, 0.60 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 }	
	}																				// for NZ-69, DAC
};


static const struct Project::StaticAttributeData NZ_fueltype7[][8] = {
	{ { 0, FUELCOM_ATTRIBUTE_SFC_S1_DEFAULTS, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_SFC_S1_DEFAULTS | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_FFC_MULTIPLIER, 3.0 },
	  { 0, FUELCOM_ATTRIBUTE_FFC_MULTIPLIER | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 3.0 },
	  { 0, FUELCOM_ATTRIBUTE_WFC_MULTIPLIER, 2.5 },
	  { 0, FUELCOM_ATTRIBUTE_WFC_MULTIPLIER | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 2.5 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 }
	},																				// for NZ-63
	{ { 0, FUELCOM_ATTRIBUTE_SFC_S1_DEFAULTS, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_SFC_S1_DEFAULTS | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_FFC_MULTIPLIER, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_FFC_MULTIPLIER | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 1.5 },
	  { 0, FUELCOM_ATTRIBUTE_WFC_MULTIPLIER, 1.2 },
	  { 0, FUELCOM_ATTRIBUTE_WFC_MULTIPLIER | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 1.2 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 }
	}																				// for NZ-66
};


static const struct Project::StaticAttributeData NZ_fueltype5[][5] = {
	{ { 0, FUELCOM_ATTRIBUTE_GFL, 0.5 },
	  { 0, FUELCOM_ATTRIBUTE_GFL | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.5 },
	  { 0, FUELCOM_ATTRIBUTE_RSI_CONSTANT_RSI, 5.0 / 60.0 },
	  { 0, FUELCOM_ATTRIBUTE_RSI_CONSTANT_RSI | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 5.0 / 60.0 },
	  { 0, 0, 0.0 }
	}																				// for NZ-70
};


static const struct Project::StaticAttributeData NZ_fueltype6[][12] = {
	{ { 0, FUELCOM_ATTRIBUTE_SFC_S1_DEFAULTS, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_SFC_S1_DEFAULTS | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.0 },
	  { 0, FUELCOM_ATTRIBUTE_FFC_MULTIPLIER, 2.0 },
	  { 0, FUELCOM_ATTRIBUTE_FFC_MULTIPLIER | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 2.0 },
	  { 0, FUELCOM_ATTRIBUTE_WFC_MULTIPLIER, 2.0 },
	  { 0, FUELCOM_ATTRIBUTE_WFC_MULTIPLIER | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 2.0 },
	  { 0, FUELCOM_ATTRIBUTE_FFC_BUI_MULTIPLIER, 0.015 },
	  { 0, FUELCOM_ATTRIBUTE_FFC_BUI_MULTIPLIER | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.015 },
	  { 0, FUELCOM_ATTRIBUTE_WFC_BUI_MULTIPLIER, 0.035 },
	  { 0, FUELCOM_ATTRIBUTE_WFC_BUI_MULTIPLIER | FUELCOM_ATTRIBUTE_GREENUP_MODIFIER, 0.035 },
	  { 0, FUELCOM_FUEL_MODIFIED, 0.0 },
	  { 0, 0, 0.0 }
	}																				// for NZ-65
};


const struct Project::StaticFuelData NZ_FuelTable[] = {
	 /* main/agency name				fbp name	aliases							COM class ID		import, export	color	equation			attribute*/
	{ _T("Built-up Area (settlement)"),			_T("Non-fuel"),	{ _T("NZ1"), _T("NZ-1"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	1, 1,	RGB(156, 156, 156), NULL, NULL }, //	NULL,			NULL },
	{ _T("Urban Parkland/Open Space"),			_T("NZ-2"),		{ _T("NZ2"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ2,	2, 2,	RGB(104, 133, 120), NULL, NULL }, //	NULL,			NZ_fueltype[0] },
	{ _T("Surface Mine"),						_T("Non-fuel"),	{ _T("NZ3"), _T("NZ-3"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	3, 3,	RGB(112, 68, 137), NULL, NULL }, //	NULL,			NULL },
	{ _T("Dump"),								_T("Non-fuel"),	{ _T("NZ4"), _T("NZ-4"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	4, 4,	RGB(112, 68, 137), NULL, NULL }, //	NULL,			NULL },
	{ _T("Transport Infrastructure"),			_T("Non-fuel"),	{ _T("NZ5"), _T("NZ-5"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	5, 5,	RGB(168, 0, 0), NULL, NULL }, //		NULL,			NULL },
	{ _T("Surface Mines and Dump"),				_T("Non-fuel"),	{ _T("NZ6"), _T("NZ-6"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_Non,	6, 6,	RGB(112, 68, 137), NULL, NULL }, //	NULL,			NULL },

	{ _T("Sand and Gravel"),					_T("Non-fuel"),	{ _T("NZ10"), _T("NZ-10"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	10, 10,	RGB(255, 255, 115), NULL, NULL }, //	NULL,			NULL },
	{ _T("River and Lakeshore Gravel and Rock"),_T("Non-fuel"),	{ _T("NZ11"), _T("NZ-11"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	11, 11,	RGB(156, 186, 156), NULL, NULL }, //	NULL,			NULL },
	{ _T("Landslide"),							_T("Non-fuel"),	{ _T("NZ12"), _T("NZ-12"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	12, 12,	RGB(202, 122, 245), NULL, NULL }, //	NULL,			NULL },
	{ _T("Alpine Gravel and Rock"),				_T("Non-fuel"),	{ _T("NZ13"), _T("NZ-13"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	13, 13,	RGB(156, 186, 156), NULL, NULL }, //	NULL,			NULL },
	{ _T("Permanent Snow and Ice"),				_T("Non-fuel"),	{ _T("NZ14"), _T("NZ-14"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	14, 14,	RGB(219, 212, 255), NULL, NULL }, //	NULL,			NULL },
	{ _T("Alpine Grass/Herbfield"),				_T("NZ-15"),	{ _T("NZ15"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ15,	15, 15,	RGB(171, 205, 102), NULL, NULL }, //	NULL,			NZ_fueltype[0] },
	{ _T("Gravel and Rock"),					_T("Non-fuel"),	{ _T("NZ16"), _T("NZ-16"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	16, 16,	RGB(156, 186, 156), NULL, NULL }, //	NULL,			NULL },

	{ _T("Lake and Pond"),						_T("Non-fuel"),	{ _T("NZ20"), _T("NZ-20"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	20, 20,	RGB(190, 232, 255), NULL, NULL }, //	NULL,			NULL },
	{ _T("River"),								_T("Non-fuel"),	{ _T("NZ21"), _T("NZ-21"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	21, 21,	RGB(190, 232, 255), NULL, NULL }, //	NULL,			NULL },
	{ _T("Estuarine Open Water"),				_T("Non-fuel"),	{ _T("NZ22"), _T("NZ-22"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },		&CLSID_CWFGM_Fuel_Non,	22, 22,	RGB(214, 245, 232), NULL, NULL }, //	NULL,			NULL },

	{ _T("Short-rotation Cropland"),			_T("NZ-30"),	{ _T("NZ30"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ30,	30, 30,	RGB(255, 211, 127), NULL, NULL }, //	NULL,			NZ_fueltype[1] },
	{ _T("Vineyard"),							_T("NZ-31"),	{ _T("NZ31"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ31,	31, 31,	RGB(230, 152, 0), NULL, NULL }, //	NULL,			NZ_fueltype[2] },
	{ _T("Orchard and Other Perennial Crops"),	_T("NZ-32"),	{ _T("NZ32"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ32,	32, 32,	RGB(230, 152, 0), NULL, NULL }, //	NULL,			NZ_fueltype[3] },
	{ _T("Orchard Vineyard and Other Perennial Crops"),_T("NZ-33"),	{ _T("NZ33"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ33,	33, 33,	RGB(230, 152, 0), NULL, NULL }, //	NULL,			NZ_fueltype[3] },

	{ _T("High Producing Exotic Grassland"),	_T("NZ-40"),	{ _T("NZ40"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ40,	40, 40,	RGB(190, 255, 140), NULL, NULL }, //	NULL,			NZ_fueltype[4] },
	{ _T("Low Producing Grassland"),			_T("NZ-41"),	{ _T("NZ41"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ41,	41, 41,	RGB(163, 212, 0), NULL, NULL }, //	NULL,			NZ_fueltype[5] },
	{ _T("Tall Tussock Grassland"),				_T("NZ-43"),	{ _T("NZ43"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ43,	43, 43,	RGB(230, 230, 140), NULL, NULL }, //	NULL,			NZ_fueltype[6] },
	{ _T("Depleted Grassland"),					_T("NZ-44"),	{ _T("NZ44"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ44,	44, 44,	RGB(210, 210, 90), NULL, NULL }, //	NULL,			NZ_fueltype[7] },
	{ _T("Herbaceous Freshwater Vegetation"),	_T("NZ-45"),	{ _T("NZ45"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ45,	45, 45,	RGB(194, 255, 214), NULL, NULL }, //	NZ_fuelequation[0],	NZ_fueltype2[0] },
	{ _T("Herbaceous Saline Vegetation"),		_T("NZ-46"),	{ _T("NZ46"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ46,	46, 46,	RGB(222, 245, 222), NULL, NULL }, //	NULL,			NZ_fueltype[8] },
	{ _T("Flaxland"),							_T("NZ-47"),	{ _T("NZ47"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ47,	47, 47,	RGB(122, 245, 202), NULL, NULL }, //	NZ_fuelequation[1],	NZ_fueltype2[1] },

	{ _T("Fernland"),							_T("NZ-50"),	{ _T("NZ50"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ50,	50, 50,	RGB(112, 92, 0), NULL, NULL }, //	NULL,			NZ_fueltype[9] },
	{ _T("Gorse and/or Broom"),					_T("NZ-51"),	{ _T("NZ51"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ51,	51, 51,	RGB(125, 105, 15), NULL, NULL }, //	NZ_fuelequation[2],	NZ_fueltype2[2] },
	{ _T("Manuka and/or Kanuka"),				_T("NZ-52"),	{ _T("NZ52"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ52,	52, 52,	RGB(140, 121, 34), NULL, NULL }, //	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("Matagouri"),							_T("NZ-53"),	{ _T("NZ53"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ53,	53, 53,	RGB(212, 205, 174), NULL, NULL }, //	NULL,			NZ_fueltype[10] },
	{ _T("Broadleafed Indigenous Hardwoods"),	_T("NZ-54"),	{ _T("NZ54"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ54,	54, 54, RGB(168, 153, 79), NULL, NULL }, //	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("Sub Alpine Shrubland"),				_T("NZ-55"),	{ _T("NZ55"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ55,	55, 55,	RGB(184, 171, 106), NULL, NULL }, //	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("Mixed Exotic Shrubland"),				_T("NZ-56"),	{ _T("NZ56"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ56,	56, 56,	RGB(196, 187, 137), NULL, NULL }, //	NZ_fuelequation[6],	NZ_fueltype2[5] },
	{ _T("Grey Scrub"),							_T("NZ-57"),	{ _T("NZ57"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ57,	57, 57,	RGB(212, 205, 174), NULL, NULL }, //	NZ_fuelequation[7],	NZ_fueltype2[6] },
	{ _T("Matagouri or Grey Scrub"),			_T("NZ-58"),	{ _T("NZ58"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ58,	58, 58,	RGB(212, 205, 174), NULL, NULL }, //	NZ_fuelequation[7],	NZ_fueltype2[6] },

	{ _T("Minor Shelterbelts"),					_T("NZ-60"),	{ _T("NZ60"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ60,	60, 60,	RGB(56, 168, 0), NULL, NULL }, //	NULL,			NZ_fueltype[11] },
	{ _T("Major Shelterbelts"),					_T("NZ-61"),	{ _T("NZ61"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ61,	61, 61,	RGB(56, 168, 0), NULL, NULL }, //	NULL,			NZ_fueltype[12] },
	{ _T("Afforestation (not imaged)"),			_T("NZ-62"),	{ _T("NZ62"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ62,	62, 62,	RGB(56, 168, 0), NULL, NULL }, //	NULL,			NZ_fueltype[13] },
	{ _T("Afforestation (imaged post LCDB1)"),	_T("NZ-63"),	{ _T("NZ63"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ63,	63, 63,	RGB(56, 168, 0), NULL, NULL }, //	NZ_fuelequation[8],	NZ_fueltype7[0] },
	{ _T("Forest - Harvested"),					_T("NZ-64"),	{ _T("NZ64"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ64,	64, 64,	RGB(161, 173, 97), NULL, NULL }, //	NULL,			NZ_fueltype1[0] },
	{ _T("Pine Forest - Open Canopy"),			_T("NZ-65"),	{ _T("NZ65"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ65,	65, 65,	RGB(56, 168, 0), NULL, NULL }, //	NZ_fuelequation[9],	NZ_fueltype6[0] },
	{ _T("Pine Forest - Closed Canopy"),		_T("NZ-66"),	{ _T("NZ66"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ66,	66, 66,	RGB(56, 168, 0), NULL, NULL }, //	NZ_fuelequation[10],	NZ_fueltype7[1] },
	{ _T("Other Exotic Forest"),				_T("NZ-67"),	{ _T("NZ67"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ67,	67, 67,	RGB(56, 168, 0), NULL, NULL }, //	NZ_fuelequation[11],	NZ_fueltype2[7] },
	{ _T("Deciduous Hardwoods"),				_T("NZ-68"),	{ _T("NZ68"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ68,	68, 68,	RGB(71, 127, 0), NULL, NULL }, //	NULL,			NULL },
	{ _T("Indigenous Forest"),					_T("NZ-69"),	{ _T("NZ69"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ69,	69, 69, RGB(40, 70, 0), NULL, NULL }, //		NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("Mangrove"),							_T("NZ-70"),	{ _T("NZ70"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ70,	70, 70,	RGB(68, 137, 137), NULL, NULL }, //	NZ_fuelequation1,	NZ_fueltype5[0] },
	{ _T("Exotic Forest"),						_T("NZ-71"),	{ _T("NZ71"), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_NZ71,	71, 71,	RGB(56, 168, 0), NULL, NULL }, //	NZ_fuelequation[10],	NZ_fueltype7[1] },

	{ _T("Non-fuel"),							_T("Non-fuel"),	{ _T("Non fuel"), _T("NF"), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },				&CLSID_CWFGM_Fuel_Non,	101,101,RGB(130, 130, 130),	NULL,	NULL },

	{ NULL,						NULL,		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },					NULL,			0, 0,	RGB(0, 0, 0),		NULL,			NULL }
};


const struct Project::StaticFuelData TAZ_FuelTable[] = {
	 /* main/agency name									fbp name	aliases											COM class ID		import, export	color		equation		attribute*/
	{ _T("AHF - Fresh water aquatic herbland"),												_T("AHF"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	1, 1,	RGB(43, 221, 185),	NZ_fuelequation[0],	NZ_fueltype2[0] },
	{ _T("AHL - lacustrine herbland"),														_T("AHL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	2, 2,	RGB(21, 174, 143),	NULL,			NZ_fueltype[8] },
	{ _T("AHS - saline aquatic herbland"),													_T("AHS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	3, 3,	RGB(21, 174, 143),	NULL,			NZ_fueltype[8] },
	{ _T("ARS - Saline grassland"),															_T("ARS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	4, 4,	RGB(21, 174, 143),	NULL,			NZ_fueltype[8] },
	{ _T("ASF - Fresh water aquatic sedgeland and rushland"),								_T("ASF"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	5, 5,	RGB(43, 221, 185),	NZ_fuelequation[0],	NZ_fueltype2[0] },
	{ _T("ASS - Succulent saline herbland"),												_T("ASS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	6, 6,	RGB(21, 174, 143),	NULL,			NZ_fueltype[8] },
	{ _T("AUS - Saltmarsh (undifferntiated)"),												_T("AUS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	7, 7,	RGB(21, 174, 143),	NULL,			NZ_fueltype[8] },
	{ _T("AWU - wetland (undifferentiated)"),												_T("AWU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	8, 8,	RGB(43, 221, 185),	NZ_fuelequation[0],	NZ_fueltype2[0] },
	{ _T("DAC - Eucalyptus amygdalina coastal forest and woodland"),						_T("DAC"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	9, 9,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },

	{ _T("DAD - Eucalyptus amygdalina forest and woodland on dolerite"),					_T("DAD"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	10, 10,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DAI - Eucalyptus amygdalina inland forest and woodland (undifferentiated)"),		_T("DAI"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	11, 11,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DAM - Eucalyptus amygdalina forest and woodland on mudstone"),					_T("DAM"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	12, 12,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DAS - Eucalyptus amygdalina forest and woodland on sandstone"),					_T("DAS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	13, 13,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DAZ - Eucalyptus amygdalina inland forest and woodland on Cainozoic deposits"),	_T("DAZ"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	14, 14,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DBA - Eucalyptus barberi forest and woodland"),									_T("DBA"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	15, 15,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DCO - Eucalyptus coccifera forest and woodland"),									_T("DCO"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	16, 16,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DCR - Eucalyptus cordata forest"),												_T("DCR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	17, 17,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DDE - Eucalyptus delegatensis dry forest and woodland"),							_T("DDE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	18, 18,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DDP - Eucalyptus dalrympleana - Eucalyptus pauciflora forest and woodland"),		_T("DDP"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	19, 19,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },

	{ _T("DGL - Eucalyptus globulus dry forest and woodland"),								_T("DGL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	20, 20,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DGW - Eucalyptus gunnii woodland"),												_T("DGW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	21, 21,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DKW - King Island Eucalypt woodland"),											_T("DKW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	22, 22,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DMO - Eucalyptus morrisbyi forest and woodland"),									_T("DMO"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	23, 23,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DMW - Midlands woodland complex"),												_T("DMW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	24, 24,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DNF - Eucalyptus nitida Furneaux forest"),										_T("DNF"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	25, 25,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DNI - Eucalyptus nitida dry forest and woodland"),								_T("DNI"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	26, 26,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DOB - Eucalyptus obliqua dry forest and woodland"),								_T("DOB"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	27, 27,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DOV - Eucalyptus ovata forest and woodland"),										_T("DOV"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	28, 28,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DOW - Eucalyptus ovata heathy woodland"),											_T("DOW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	29, 29,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },

	{ _T("DPD - Eucalyptus pauciflora forest and woodland on dolerite"),					_T("DPD"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	30, 30,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DPE - Eucalyptus perriniana forest and woodland"),								_T("DPE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	31, 31,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DPO - Eucalyptus pauciflora forest and woodland not on dolerite substrates"),		_T("DPO"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	32, 32,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DPU - Eucalyptus pulchella forest and woodland"),									_T("DPU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	33, 33,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DRI - Eucalyptus risdonii forest and woodland"),									_T("DRI"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	34, 34,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DRO - Eucalyptus rodwayi forest and woodland"),									_T("DRO"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	35, 35,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DSC - Eucalyptus amygdalina - Eucalyptus obliqua damp sclerophyll forest"),		_T("DSC"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	36, 36,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DSG - Eucalyptus sieberi forest and woodland on granite"),						_T("DSG"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	37, 37,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DSO - Eucalyptus sieberi forest and woodland not on granite substrates"),			_T("DSO"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	38, 38,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DTD - Eucalyptus tenuiramis forest and woodland on dolerite"),					_T("DTD"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	39, 39,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },

	{ _T("DTG - Eucalyptus tenuiramis forest and woodland on granite"),						_T("DTG"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	40, 40,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DTO - Eucalyptus tenuiramis forest and woodland on sediments"),					_T("DTO"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	41, 41,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DVC - Eucalyptus viminalis - Eucalyptus globulus coastal forest and woodland"),	_T("DVC"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	42, 42,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DVF - Eucalyptus viminalis Furneaux forest and woodland"),						_T("DVF"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	43, 43,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DVG - Eucalyptus viminalis grassy forest and woodland"),							_T("DVG"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	44, 44,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("DVS - Eucalyptus viminalis shrubby/heathy woodland"),								_T("DVS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	45, 45,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },

	{ _T("FAG - Agricultural land"),														_T("FAG"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1a,	46, 46,	RGB(154, 227, 45),	NULL,			NZ_fueltype[5] },
	{ _T("FMG - Marram grassland"),															_T("FMG"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	47, 47,	RGB(104, 143, 14),	NZ_fuelequation[2],	NZ_fueltype2[2] },
	{ _T("FPE - Permanent easements"),														_T("FPE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_Non,	48, 48,	RGB(92, 92, 92),	NULL,			NULL },
	{ _T("FPF - Pteridium esculentum fernland"),											_T("FPF"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	49, 49,	RGB(147, 189, 2),	NULL,			NZ_fueltype[9] },

	{ _T("FPL - Plantations Softwood"),														_T("FPL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C6,	50, 50,	RGB(0, 79, 0),		NZ_fuelequation[10],	NZ_fueltype7[1] },
	{ _T("FPL-5a1 - Plantations Softwood"),													_T("FPL-5a1"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	51, 51,	RGB(209, 218, 173),	NULL,			NZ_fueltype[13] },
	{ _T("FPL-5a2 - Plantations Softwood"),													_T("FPL-5a2"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_S1,	52, 52,	RGB(128, 64, 0),	NULL,			NZ_fueltype1[0] },
	{ _T("FPL-5a3 - Plantations Softwood"),													_T("FPL-5a3"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	53, 53,	RGB(226, 231, 199),	NZ_fuelequation[8],	NZ_fueltype7[0] },
	{ _T("FPL-5a4 - Plantations Softwood"),													_T("FPL-5a4"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	54, 54,	RGB(46, 148, 31),	NZ_fuelequation[9],	NZ_fueltype6[0] },
	{ _T("FPL-5a5 - Plantations Softwood"),													_T("FPL-5a5"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C6,	55, 55,	RGB(0, 79, 0),		NZ_fuelequation[10],	NZ_fueltype7[1] },
	{ _T("FPL-5a6 - Plantations Softwood"),													_T("FPL-5a6"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C6,	56, 56,	RGB(0, 79, 0),		NZ_fuelequation[10],	NZ_fueltype7[1] },
	{ _T("FPL-5a7 - Plantations Softwood"),													_T("FPL-5a7"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C6,	57, 57,	RGB(0, 79, 0),		NZ_fuelequation[10],	NZ_fueltype7[1] },
	{ _T("FPL-5b1 - Plantations Hardwood"),													_T("FPL-5b1"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_S1,	58, 58,	RGB(128, 64, 0),	NULL,			NZ_fueltype1[0] },
	{ _T("FPL-5b2 - Plantations Hardwood"),													_T("FPL-5b2"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C6,	59, 59,	RGB(0, 79, 0),		NZ_fuelequation[10],	NZ_fueltype7[1] },

	{ _T("FPL-5b3 - Plantations Hardwood"),													_T("FPL-5b3"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	60, 60,	RGB(46, 148, 31),	NZ_fuelequation[9],	NZ_fueltype6[0] },
	{ _T("FPL-5b4 - Plantations Hardwood"),													_T("FPL-5a4"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	61, 61,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("FPU - Unverified plantations for silviculture"),									_T("FPU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	62, 62,	RGB(209, 218, 173),	NULL,			NZ_fueltype[13] },
	{ _T("FRG - Regenerating cleared land"),												_T("FRG"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	63, 63,	RGB(209, 218, 173),	NULL,			NZ_fueltype[13] },
	{ _T("FSM - Spartina marshland"),														_T("FSM"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	64, 64,	RGB(43, 221, 185),	NZ_fuelequation[0],	NZ_fueltype2[0] },
	{ _T("FUM - Extra-urban miscellaneous"),												_T("FUM"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_Non,	65, 65,	RGB(232, 100, 109),	NULL,			NULL },
	{ _T("FUR - Urban areas"),																_T("FUR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1a,	66, 66,	RGB(172, 255, 89),	NULL,			NZ_fueltype[0] },
	{ _T("FWU - Weed infestation"),															_T("FWU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	67, 67,	RGB(104, 143, 14),	NZ_fuelequation[2],	NZ_fueltype2[2] },
	{ _T("GCL - Lowland grassland complex"),												_T("GCL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1a,	68, 68,	RGB(154, 227, 45),	NULL,			NZ_fueltype[5] },
	{ _T("GHC - Coastal grass and herbfield"),												_T("GHC"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1a,	69, 69,	RGB(154, 227, 45),	NULL,			NZ_fueltype[5] },

	{ _T("GPH - Highland Poa grassland"),													_T("GPH"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1a,	70, 70,	RGB(154, 227, 45),	NULL,			NZ_fueltype[5] },
	{ _T("GPL - Lowland Poa labillardierei grassland"),										_T("GPL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1a,	71, 71,	RGB(154, 227, 45),	NULL,			NZ_fueltype[5] },
	{ _T("GRP - Rockplate grassland"),														_T("GRP"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1a,	72, 72,	RGB(154, 227, 45),	NULL,			NZ_fueltype[5] },
	{ _T("GSL - Lowland sedgy grassland"),													_T("GSL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1a,	73, 73,	RGB(154, 227, 45),	NULL,			NZ_fueltype[5] },
	{ _T("GTL - Lowland Themeda grassland"),												_T("GTL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1a,	74, 74,	RGB(154, 227, 45),	NULL,			NZ_fueltype[5] },

	{ _T("HCH - Alpine coniferous heathland"),												_T("HCH"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	75, 75,	RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("HCM - Cushion moorland"),															_T("HCM"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	76, 76,	RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("HHE - Eastern alpine heathland"),													_T("HHE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	77, 77,	RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("HHW - Western alpine heathland"),													_T("HHW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	78, 78,	RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("HSE - Eastern alpine sedgeland"),													_T("HSE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	79, 79,	RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("HSW - Western alpine sedgeland/herbland"),										_T("HSW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	80, 80,	RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("HUE - Eastern alpine vegetation (undifferentiated)"),								_T("HUE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	81, 81, RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },

	{ _T("MAP - Alkaline pans"),															_T("MAP"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	82, 82,	RGB(230, 231, 169),	NULL,			NZ_fueltype[7] },
	{ _T("MBE - Eastern buttongrass moorland"),												_T("MBE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	83, 83,	RGB(213, 213, 0),	NULL,			NZ_fueltype[6] },
	{ _T("MBP - Pure buttongrass moorland"),												_T("MBP"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	84, 84,	RGB(213, 213, 0),	NULL,			NZ_fueltype[6] },
	{ _T("MBR - Sparse buttongrass moorland on slopes"),									_T("MBR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	85, 85,	RGB(230, 231, 169),	NULL,			NZ_fueltype[7] },
	{ _T("MBS - Buttongrass moorland with emergent shrubs"),								_T("MBS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	86, 86,	RGB(213, 213, 0),	NULL,			NZ_fueltype[6] },
	{ _T("MBU - Buttongrass moorland (undifferentiated)"),									_T("MBU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	87, 87,	RGB(213, 213, 0),	NULL,			NZ_fueltype[6] },
	{ _T("MBW - Western buttongrass moorland"),												_T("MBW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	88, 88,	RGB(213, 213, 0),	NULL,			NZ_fueltype[6] },
	{ _T("MDS - Subalpine Diplarrena latifolia rushland"),									_T("MDS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	89, 89,	RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("MGH - Highland grassy sedgeland"),												_T("MGH"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	90, 90,	RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },

	{ _T("MRR - Restionaceae rushland"),													_T("MRR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	91, 91,	RGB(213, 213, 0),	NULL,			NZ_fueltype[6] },
	{ _T("MSP - Sphagnum peatland"),														_T("MSP"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	92, 92,	RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },

	{ _T("MSW - Western lowland sedgeland"),												_T("MSW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	93, 93,	RGB(230, 231, 169),	NULL,			NZ_fueltype[7] },
	{ _T("NAD - Acacia dealbata forest"),													_T("NAD"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	94, 94, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("NAF - Acacia melanoxylon swamp forest"),											_T("NAF"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	95, 95, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },

	{ _T("NAL - Allocasuarina littoralis forest"),											_T("NAL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	96, 96,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("NAR - Acacia melanoxylon on rises"),												_T("NAR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	97, 97, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("NAV - Allocasuarina verticillata forest"),										_T("NAV"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	98, 98,	RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("NBA - Bursaria - Acacia woodland and scrub"),										_T("NBA"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	99, 99,	RGB(0, 128, 0),		NZ_fuelequation[3],	NZ_fueltype2[3] },

	{ _T("NBS - Banksia serrata woodland"),													_T("NBS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	100, 100, RGB(0, 128, 0),		NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("NCR - Callitris rhomboidea forest"),												_T("NCR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	101, 101, RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("NLA - Leptospermum scoparium - Acacia mucronata forest"),							_T("NLA"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	102, 102, RGB(0, 128, 64),	NULL,			NZ_fueltype[10] },
	{ _T("NLE - Leptospermum forest"),														_T("NLE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	103, 103, RGB(0, 128, 64),	NULL,			NZ_fueltype[10] },
	{ _T("NLM - Leptospermum lanigerum - Melaleuca squarrosa swamp forest"),				_T("NLM"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	104, 104, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },

	{ _T("NLN - Subalpine Leptospermum nitidum woodland"),									_T("NLN"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	105, 105, RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("NME - Melaleuca ericifolia swamp forest"),										_T("NNE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	106, 106, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("NNP - Notelaea - Pomaderris - Beyeria forest"),									_T("NNP"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	107, 107, RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },
	{ _T("OAQ - Water- sea"),																_T("OAQ"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_Non,	108, 108, RGB(0, 0, 255),	NULL,			NULL },
	{ _T("ORO - Rock (cryptogamic lithosere )"),											_T("ORO"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_Non,	109, 109, RGB(182, 182, 182),	NULL,			NULL },
	{ _T("OSM - Sand- mud"),																_T("OSM"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_Non,	110, 110, RGB(181, 181, 181),	NULL,			NULL },

	{ _T("RCO - Coastal rainforest"),														_T("RCO"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	111, 111, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("RFE - Rainforest fernland"),														_T("RFE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	112, 112, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("RFS - Nothofagus gunnii rainforest and scrub"),									_T("RFS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	113, 113, RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("RHP - Lagarostrobos franklinii rainforest and scrub"),							_T("RHP"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	114, 114, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("RKF - Athrotaxis selaginoides - Nothofagus gunnii short rainforest"),				_T("RKF"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	115, 115, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("RKP - Athrotaxis selaginoides rainforest"),										_T("RKP"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	116, 116, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("RKS - Athrotaxis selaginoides subalpine scrub"),									_T("RSK"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	117, 117, RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },

	{ _T("RKX - Highland rainforest scrub with dead Athrotaxis selaginoides"),				_T("RKX"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	118, 118, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("RLS - Leptospermum with rainforest scrub"),										_T("RLS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	119, 119, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("RML - Nothofagus - Leptospermum short rainforest"),								_T("RML"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	120, 120, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("RMS - Nothofagus / Phyllocladus short rainforest"),								_T("RMS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	121, 121, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("RMT - Nothofagus - Atherosperma rainforest"),										_T("RMT"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	122, 122, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("RMU - Nothofagus rainforest undifferentiated"),									_T("RMU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	123, 123, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },

	{ _T("RPF - Athrotaxis cupressoides/Nothofagus gunnii short rainforest"),				_T("RPF"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	124, 124, RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("RPP - Athrotaxis cupressoides rainforest"),										_T("RPP"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	125, 125, RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("RPW - Athrotaxis cupressoides open woodland"),									_T("RPW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	126, 126, RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("RSH - Highland low rainforest and scrub"),										_T("RSH"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	127, 127, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },

	{ _T("SAC - Acacia longifolia coastal scrub"),											_T("SAC"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	128, 128, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SBM - Banksia marginata wet scrub"),												_T("SBM"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	129, 129, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SBR - Broadleaf scrub"),															_T("SBR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	130, 130, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SCA - Coastal scrub on alkaline sands"),											_T("SCA"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	131, 131, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SCH - Coastal heathland"),														_T("SCH"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	132, 132, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SCK - Coastal complex on King Island"),											_T("SCK"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	133, 133, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SCW - Heathland scrub complex at Wingaroo"),										_T("SCW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	134, 134, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SDU - Dry scrub"),																_T("SDU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	135, 135, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SHC - Heathland on calcarenite"),													_T("SHC"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	136, 136, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SHF - Heathland scrub mosaic on Flinders Island"),								_T("SHF"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	137, 137, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SHG - Heathland on granite"),														_T("SHG"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	138, 138, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SHL - Lowland sedgy heathland"),													_T("SHL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	139, 139, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },

	{ _T("SHS - Subalpine heathland"),														_T("SHS"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	140, 140, RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("SHU - Inland Heathland (undifferentiated)"),										_T("SHU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	141, 141, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SHW - Wet heathland"),															_T("SHW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	142, 142, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SLA-10a - Slash Unburned"),														_T("SLA-10a"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_S1,	144, 144, RGB(128, 64, 0),	NULL,			NZ_fueltype1[0] },
	{ _T("SLA-10b - Slash Burned"),															_T("SLA-10b"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_O1b,	145, 145, RGB(209, 218, 173),	NULL,			NZ_fueltype[13] },

	{ _T("SLW - Leptospermum scrub"),														_T("SLW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	146, 146, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SMM - Melaleuca squamea heathland"),												_T("SMM"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	147, 147, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SMP - Melaleuca pustulata scrub"),												_T("SMP"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	148, 148, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SMR - Melaleuca squarrosa scrub"),												_T("SMR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	149, 149, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SQR - Queenstown regrowth mosaic"),												_T("SQR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	150, 150, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SRC - Seabird rookery complex"),													_T("SRC"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	151, 151, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SRI - Riparian scrub"),															_T("SRI"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	152, 152, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SSC - Coastal Scrub"),															_T("SSC"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	153, 153, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },
	{ _T("SSK - Scrub complex on King Island"),												_T("SSK"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	154, 154, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },

	{ _T("SSW - Western subalpine scrub"),													_T("SSW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	155, 155, RGB(181, 238, 185),	NZ_fuelequation[5],	NZ_fueltype2[4] },
	{ _T("SWW - Western wet scrub"),														_T("SWW"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_C4,	156, 156, RGB(0, 128, 0),	NZ_fuelequation[3],	NZ_fueltype2[3] },

	{ _T("WBR - Eucalyptus brookeriana wet forest"),										_T("WBR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	157, 157, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WDA - Eucalyptus dalrympleana forest"),											_T("WDA"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	158, 158, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WDB - Eucalyptus delegatensis forest with broadleaf shrubs"),						_T("WDB"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	159, 159, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WDL - Eucalyptus delegatensis forest over Leptospermum"),							_T("WDL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	160, 160, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WDR - Eucalyptus delegatensis over rainforest"),									_T("WDR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	161, 161, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WDU - Eucalyptus delegatensis wet forest (undifferentiated)"),					_T("WDU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	162, 162, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },

	{ _T("WGK - Eucalyptus globulus King Island forest"),									_T("WGK"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	163, 163, RGB(0, 196, 98),	NZ_fuelequation[12],	NZ_fueltype4[1] },

	{ _T("WGL - Eucalyptus globulus wet forest"),											_T("WGL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	164, 164, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WNL - Eucalyptus nitida forest over Leptospermum"),								_T("WNL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	165, 165, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WNR - Eucalyptus nitida over rainforest"),										_T("WNR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	166, 166, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WNU - Eucalyptus nitida wet forest (undifferentiated)"),							_T("WNU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	167, 167, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WOB - Eucalyptus obliqua forest with broadleaf shrubs"),							_T("WOB"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	168, 168, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WOL - Eucalyptus obliqua forest over Leptospermum"),								_T("WOL"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	169, 169, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WOR - Eucalyptus obliqua forest over rainforest"),								_T("WOR"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	170, 170, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WOU - Eucalyptus obliqua wet forest (undifferentiated)"),							_T("WOU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	171, 171, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WRE - Eucalyptus regnans forest"),												_T("WRE"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	172, 172, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WSU - Eucalyptus subcrenulata forest and woodland"),								_T("WSU"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	173, 173, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },
	{ _T("WVI - Eucalyptus viminalis wet forest"),											_T("WVI"),	{ _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T("") },			&CLSID_CWFGM_Fuel_M2,	174, 174, RGB(2, 130, 60),	NZ_fuelequation[4],	NZ_fueltype4[0] },

	{ NULL,											NULL,		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },					NULL,			0, 0,	RGB(0, 0, 0),		NULL,			NULL }
};
