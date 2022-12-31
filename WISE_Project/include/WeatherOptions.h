/**
 * WISE_Project: WeatherOptions.h
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

#ifndef __WEATHER_OPTIONS_H
#define __WEATHER_OPTIONS_H

#include "cwfgmp_config.h"
#include "CWFGM_WeatherStream.h"
#include "hssconfig/config.h"

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(push, 8)
#endif

namespace Project
{
	class CWFGMPROJECT_EXPORT WeatherOptions
	{
	public:
		constexpr static std::uint16_t VAN_WAGNER = 0;
		constexpr static std::uint16_t LAWSON = 2;

	public:
		double	m_temp_alpha,
				m_temp_beta,
				m_temp_gamma,
				m_wind_alpha,
				m_wind_beta,
				m_wind_gamma;			// values for Judy Beck's diurnal weather model
		std::uint16_t	m_FFMC;				// 0 for VanWagner, and 2 for Lawson

	public:
		WeatherOptions();
		WeatherOptions(const TCHAR *group_name);
		WeatherOptions(const WeatherOptions &toCopy);
		WeatherOptions(class CCWFGM_WeatherStream *weather);

		void SaveToIniFile(const TCHAR *group_name) const;
		bool SaveToWeatherStream(CCWFGM_WeatherStream *weather) const;

		const WeatherOptions &operator=(const WeatherOptions &toCopy);
	};
};

#ifdef HSS_SHOULD_PRAGMA_PACK
#pragma pack(pop)
#endif

#endif
