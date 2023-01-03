/**
 * WISE_Project: WindSpeedGrid.h
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

#pragma once

#include "cwfgmp_config.h"

#include "ICWFGM_Fuel.h"
#include "WeatherGridFilter.h"
#include "WindGrid.h"
#include "WxWSEntry.h"
#include "CWFGM_WindSpeedGrid.h"
#include "ISerializeProto.h"

#include "projectWindGrid.pb.h"


namespace Project
{
	class CWFGMPROJECT_EXPORT CWindSpeedGrid : public CWeatherGridBase
	{
	public:
		CWindSpeedGrid(int sectors = 8);
		virtual void InitSectors(int sectors);
		virtual std::string DisplayName() const;
		virtual const std::string TypeString() const { using namespace std::string_literals; return "Wind Speed Grid"s; }
		virtual void Clone(boost::intrusive_ptr<ICWFGM_GridEngine> *grid) const;
		std::string GetRangeString(USHORT sector, USHORT count) const;
		virtual CWindSpeedGrid *Duplicate() const;

		virtual HRESULT SetStartTime(const WTime &time);
		virtual HRESULT SetEndTime(const WTime &time);
		virtual HRESULT SetStartTimeOfDay(const WTimeSpan &time);
		virtual HRESULT SetEndTimeOfDay(const WTimeSpan &time);

		HRESULT Import(const USHORT sector, const double speed, const std::string &prj_filename, const std::string&name);
		HRESULT Export(const USHORT sector, const double speed, const std::string &, const std::string &name);
		HRESULT Remove(const USHORT sector, const double speed);
		HRESULT Remove(const USHORT sector, const USHORT range);
		HRESULT ModifySectorSet(const std::vector<WeatherGridSetModifier> &set_modifiers);
		std::string GetFilename(const USHORT sector, const USHORT range) const;
		WxWSEntry* GetRanges(const USHORT sector, USHORT *count) const;
		USHORT GetSectorCount() const;
		USHORT GetRangeCount(const USHORT sector);

		bool GetEnableGridDefault(HRESULT *hr = NULL);
		bool SetEnableGridDefault(bool enable, HRESULT *hr = NULL);

		bool GetEnableGridSectors(HRESULT *hr = NULL);
		bool SetEnableGridSectors(bool enable, HRESULT *hr = NULL);

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		CWindSpeedGrid *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }

	protected:
		CWindSpeedGrid* Duplicate(std::function<GridFilter*(void)> constructor) const override;

		void CreateOctants()
		{
			for (int i=0; i<8; i++)
			{
				std::string d(directions_8[i]);
				double min = ((i - 0.5) * 45.0);
				double max = ((i + 0.5) * 45.0);
				USHORT index = 0;

				auto ws = boost::dynamic_pointer_cast<CCWFGM_WindSpeedGrid>(m_filter);
				HRESULT hr = ws->AddSector(d, &min, &max, &index);
			}
		}
	};
};
