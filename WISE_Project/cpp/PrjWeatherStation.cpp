/**
 * WISE_Project: PrjWeatherStation.cpp
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

#if __has_include(<mathimf.h>)
#include <mathimf.h>
#else
#include <cmath>
#endif

#if _DLL
#include "stdafx.h"
#endif

#include "WeatherCollection.h"
#include "BurnPeriodOption.h"
#include "CWFGM_WeatherStation.h"
#include <boost/algorithm/string.hpp>
#include <gsl/util>

using namespace std::string_literals;
using namespace HSS_Time;


Project::WeatherStation::WeatherStation(const WeatherStationCollection *collection)
    : m_collection(collection)
{
	m_station = boost::make_intrusive<CCWFGM_WeatherStation>();
	PolymorphicUserData v = (void*)this;
	m_station->put_UserData(v);
}


Project::WeatherStation::WeatherStation(const WeatherStation &station) :
	m_collection(station.m_collection)
{
	boost::intrusive_ptr<ICWFGM_CommonBase> ws;
	HRESULT hr = station.m_station->Clone(&ws);
	m_station = dynamic_cast<CCWFGM_WeatherStation *>(ws.get());
	PolymorphicUserData v = (void*)this;
	m_station->put_UserData(v);
	m_name = station.m_name;
	m_comments = station.m_comments;

	m_displaySize = station.m_displaySize;
	m_color = station.m_color;
	m_symbol = station.m_symbol;

	std::uint32_t i, count;
	hr = m_station->GetStreamCount(&count);
	if (SUCCEEDED(hr))
	{
		for (i = 0; i < count; i++)
		{
			boost::intrusive_ptr<CCWFGM_WeatherStream> _ws;
			hr = m_station->StreamAtIndex(i, &_ws);
			if (SUCCEEDED(hr)) {
				WeatherStream *w, *wss = new WeatherStream(_ws.get());
				w = station.StreamAtIndex(i);
				wss->m_name = w->m_name;
				wss->m_comments = w->m_comments;
				m_streamList.AddTail(wss);
				wss->m_station = this;
			}
		}
	}
}


Project::WeatherStation::~WeatherStation()
{
	while (!m_streamList.IsEmpty())
	{
		WeatherStream *stream = m_streamList.LH_Head();
		RemoveStream(stream);
		delete stream;
	}
}


bool Project::WeatherStation::AddStream(WeatherStream *stream)
{
	if (stream->m_station)
		return false;
	if (SUCCEEDED(m_station->AddStream(stream->m_stream.get(), (ULONG)-1)))
	{
		if (IndexOf(stream) != gsl::narrow_cast<ULONG>(-1))
		{
			m_station->RemoveStream(stream->m_stream.get());
			return false;
		}
		m_streamList.AddTail(stream);
		stream->m_station = this;
		return true;
	}
	return false;
}


bool Project::WeatherStation::AddStream(WeatherStream *stream, ULONG index)
{
	if (stream->m_station)
		return false;
	if (SUCCEEDED(m_station->AddStream(stream->m_stream.get(), index)))
	{
		if (IndexOf(stream) != gsl::narrow_cast<ULONG>(-1))
		{
			m_station->RemoveStream(stream->m_stream.get());
			return false;
		}
		if (!index)
		{
			m_streamList.AddHead(stream);
		}
		else
		{
			MinNode *mn = m_streamList.IndexNode(index - 1);
			m_streamList.Insert(stream, mn);
		}
		stream->m_station = this;
		return true;
	}
	return false;
}


HRESULT Project::WeatherStation::RemoveStream(WeatherStream *stream)
{
	if (!stream->m_station)
		return E_UNEXPECTED;
	if (IndexOf(stream) == gsl::narrow_cast<ULONG>(-1))
		return E_UNEXPECTED;
	HRESULT hr;
	if (SUCCEEDED(hr = m_station->RemoveStream(stream->m_stream.get())))
	{
		m_streamList.Remove(stream);
		stream->m_station = nullptr;
	}
	return hr;
}


auto Project::WeatherStation::FindName(const TCHAR *name) const -> WeatherStream*
{
	WeatherStream *ws = m_streamList.LH_Head();
	while (ws->LN_Succ())
	{
		if (boost::equals(ws->m_name, name))
			return ws;
		ws = ws->LN_Succ();
	}
	ws = m_streamList.LH_Head();
	while (ws->LN_Succ())
	{
		if (boost::iequals(ws->m_name, name))
			return ws;
		ws = ws->LN_Succ();
	}
	return nullptr;
}
