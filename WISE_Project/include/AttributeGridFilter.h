/**
 * WISE_Project: AttributeGridFilter.h
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

#ifndef _ATTRIBUTE_GRID_FILTER_H_
#define _ATTRIBUTE_GRID_FILTER_H_

#include "cwfgmp_config.h"
#include "ICWFGM_Fuel.h"
#include "GridCOM.h"
#include "linklist.h"
#include "lines.h"

#include "GridFilter.h"
#include "ISerializeProto.h"

#include "projectGridFilter.pb.h"

#include <gsl/util>


namespace Project
{
	class CWFGMPROJECT_EXPORT AttributeGridFilter : public GridFilter, public ISerializeProto
	{
	public:
		AttributeGridFilter();
		~AttributeGridFilter();

		AttributeGridFilter* Duplicate() const override;

		std::string DisplayName() const override;
		const std::string TypeString() const override		{ using namespace std::string_literals; return "Attribute"s; }

		COLORREF BoundaryColor() const noexcept override	{ return gsl::narrow_cast<COLORREF>(-1); };	// no boundary color

		bool SetAttributePoint(const XY_Point &pt, const NumericVariant &attribute);
		bool SetAttributeLine(const XY_Point &pt1, const XY_Point &pt2, const NumericVariant &attribute);
		NumericVariant GetAttribute(const XY_Point &pt) const;
		bool ResetAttribute(const NumericVariant &attribute);

		std::uint16_t AttributeType() const;
		std::uint16_t AttributeType(std::uint16_t type);

		USHORT Key() const;
		USHORT Key(USHORT key);

		virtual HRESULT Import(const class CWFGMProject *project, const std::string &prj_filename, const std::string &name);
		virtual HRESULT Export(const std::string &prj_filename, const std::string &name);

		virtual void Reset() { }

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		AttributeGridFilter *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	protected:
		void Clone(boost::intrusive_ptr<ICWFGM_GridEngine> *grid) const override;
		AttributeGridFilter* Duplicate(std::function<GridFilter*(void)> constructor) const override;
	};
};

#endif
