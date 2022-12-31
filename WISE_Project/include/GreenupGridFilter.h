/**
 * WISE_Project: GreenupGridFilter.h
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

#ifndef _GREENUP_GRID_FILTER_H_
#define _GREENUP_GRID_FILTER_H_

#include "cwfgmp_config.h"
#include "AttributeGridFilter.h"


namespace Project
{
	class CWFGMPROJECT_EXPORT GreenupGridFilter : public AttributeGridFilter
	{
	public:
		GreenupGridFilter();
		virtual void Reset();

		std::string DisplayName() const override;
		const std::string TypeString() const override				{ using namespace std::string_literals; return "Green-up Grid"s; }

		virtual COLORREF BoundaryColor() const noexcept override	{ return RGB(0, 255, 0); }
		virtual ULONG BoundaryWidth() const noexcept override		{ return 1; }

		GreenupGridFilter* Duplicate() const override;

	protected:
		GreenupGridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};


	class CWFGMPROJECT_EXPORT CBHGridFilter : public AttributeGridFilter
	{
	public:
		CBHGridFilter();
		virtual void Reset();

		std::string DisplayName() const override;
		const std::string TypeString() const override				{ using namespace std::string_literals; return "Crown Base Height Grid"s; }

		virtual COLORREF BoundaryColor() const noexcept override	{ return RGB(0, 255, 0); }
		virtual ULONG BoundaryWidth() const noexcept override		{ return 1; }

		CBHGridFilter* Duplicate() const override;

	protected:
		CBHGridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};


	class CWFGMPROJECT_EXPORT TreeHeightGridFilter : public AttributeGridFilter
	{
	public:
		TreeHeightGridFilter();
		virtual void Reset();

		std::string DisplayName() const override;
		const std::string TypeString() const override				{ using namespace std::string_literals; return "Tree Height Grid"s; }

		virtual COLORREF BoundaryColor() const noexcept override	{ return RGB(0, 255, 0); }
		virtual ULONG BoundaryWidth() const noexcept override		{ return 1; }

		TreeHeightGridFilter* Duplicate() const override;

	protected:
		TreeHeightGridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};


	class CWFGMPROJECT_EXPORT FuelLoadGridFilter : public AttributeGridFilter
	{
	public:
		FuelLoadGridFilter();
		virtual void Reset();

		std::string DisplayName() const override;
		const std::string TypeString() const override { using namespace std::string_literals; return "Fuel Load Grid"s; }

		virtual COLORREF BoundaryColor() const noexcept override	{ return RGB(0, 255, 0); }
		virtual ULONG BoundaryWidth() const noexcept override		{ return 1; }

		FuelLoadGridFilter* Duplicate() const override;

	protected:
		FuelLoadGridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};
};

#endif
