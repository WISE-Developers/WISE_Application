/**
 * WISE_Project: AngleGridFilter.h
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

#ifndef _ANGLE_GRID_FILTER_H_
#define _ANGLE_GRID_FILTER_H_

#include "cwfgmp_config.h"
#include "AttributeGridFilter.h"


namespace Project
{
	class CWFGMPROJECT_EXPORT AngleGridFilter : public AttributeGridFilter {
	protected:
		AngleGridFilter();

	public:
		virtual COLORREF BoundaryColor() const noexcept override	{ return RGB(255, 255, 0); };
		virtual ULONG BoundaryWidth() const noexcept override		{ return 1; };

		virtual HRESULT Import(const class CWFGMProject* project, const std::string& prj_filename, const std::string& name) override;

	protected:
		virtual bool acceptNegatives() const { return true; }
	};


	class CWFGMPROJECT_EXPORT OVDGridFilter : public AngleGridFilter {
	public:
		OVDGridFilter();
		virtual void Reset() override;

		virtual std::string DisplayName() const override;
		virtual const std::string TypeString() const override;

	protected:
		virtual bool acceptNegatives() const override { return false; }
	};
};

#endif
