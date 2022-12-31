/**
 * WISE_Project: PercentConiferGridFilter.h
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

#ifndef _PERCENTCONIFER_GRID_FILTER_H_
#define _PERCENTCONIFER_GRID_FILTER_H_

#include "cwfgmp_config.h"
#include "AttributeGridFilter.h"


namespace Project
{
	class CWFGMPROJECT_EXPORT PercentGridFilter : public AttributeGridFilter
	{
	protected:
		PercentGridFilter();

	public:
		virtual COLORREF BoundaryColor() const noexcept override	{ return RGB(255, 255, 0); };
		virtual ULONG BoundaryWidth() const noexcept override		{ return 1; };

		virtual HRESULT Import(const CWFGMProject *project, const std::string& prj_filename, const std::string& name) override;

		PercentGridFilter* Duplicate() const override;

	protected:
		PercentGridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};


	class CWFGMPROJECT_EXPORT PercentConiferGridFilter : public PercentGridFilter 
	{
	public:
		PercentConiferGridFilter();
		virtual void Reset();

		std::string DisplayName() const override;
		const std::string TypeString() const override				{ using namespace std::string_literals; return "Percent Conifer Grid"s; }

		PercentConiferGridFilter* Duplicate() const override;

	protected:
		PercentConiferGridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};


	class CWFGMPROJECT_EXPORT PercentCureGrassGridFilter : public PercentGridFilter 
	{
	public:
		PercentCureGrassGridFilter();
		virtual void Reset();

		std::string DisplayName() const override;
		const std::string TypeString() const override				{ using namespace std::string_literals; return "Degree of curing (%) Grid"s; }

		PercentCureGrassGridFilter* Duplicate() const override;

	protected:
		PercentCureGrassGridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};


	class CWFGMPROJECT_EXPORT PercentDeadFirGridFilter : public PercentGridFilter 
	{
	public:
		PercentDeadFirGridFilter();
		virtual void Reset();

		std::string DisplayName() const override;
		const std::string TypeString() const override				{ using namespace std::string_literals; return "Percent Dead Fir Grid"s; }

		PercentDeadFirGridFilter* Duplicate() const override;

	protected:
		PercentDeadFirGridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};
};

#endif
