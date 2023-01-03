/**
 * WISE_Project: FuelCollection.h
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

#if !defined(AFX_FUELCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
#define AFX_FUELCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cwfgmp_config.h"

#include "ICWFGM_FBPFuel.h"
#include "CWFGM_FuelMap.h"
#include "GridCOM.h"
#include "types.h"
#include "StaticDataStruct.h"
#include "ISerializeProto.h"
#include "fuelCollection.pb.h"

#include <gsl/util>

//	We kinda fake class inheritance off a ICWFGM_Fuel object by attaching our own data to the end of it, via the UserData field.
//	That attached data is this Fuel object, it maintains the color, comments, etc. associated with a given ICWFGM_Fuel.  It makes
//	things a bit more messy for clean-up and serialization, but is (IMHO) the best solution for performance in the long run.
namespace Project
{
	class FuelCollection;
	class CWFGMPROJECT_EXPORT Fuel : public ISerializeProto
	{
		friend FuelCollection;
	public:
		typedef void CPenHolder;

	protected:
		std::optional<COLORREF>	m_col;

		CPenHolder* m_cpen{ nullptr };

		Fuel(ICWFGM_Fuel *fuel, COLORREF color, const std::string &defaultFBP, const std::string &comments);
		Fuel(ICWFGM_Fuel *fuel, const Fuel *f);

	public:
		virtual ~Fuel();
		bool operator==(const Fuel &toCompare) const		{ if (&toCompare == this) return true; return m_fuel == toCompare.m_fuel; }
		bool operator==(const ICWFGM_Fuel *toCompare) const	{ return m_fuel.get() == toCompare; }

		std::string Name() const;					// this is the name used and displayed everywhere, used in LUT, display, and is user modifiable
		bool Name(const std::string &name);

		bool IsModifiable();

		COLORREF Color() const noexcept			{ if (m_col.has_value()) return m_col.value(); return RGB(0, 0, 0); };
		COLORREF Color(COLORREF color);

		CPenHolder *Pen() const noexcept			{ return m_cpen; }
		void Pen(CPenHolder* pen) noexcept			{ m_cpen = pen; }

		boost::intrusive_ptr<ICWFGM_Fuel>	m_fuel;				// m_fuel's UserData should point at this
		HRESULT LockState()						{ return m_fuel->MT_Lock(false, gsl::narrow_cast<USHORT>(-1)); };

	private:
		std::string internalFBPName() const;
		bool internalFBPName(const std::string &name);		// these hold/returns the FBP (short) name - it's only used to determine the fuel "template" and for exporting the LUT, and for resetting to defaults

	public:
		std::string			m_comments;
		std::int32_t		m_flags;			// whether it was user-created, etc.
		std::string			m_defaultFuelFBP;		// how to identify what our original/default fuel is

		constexpr static std::uint32_t FLAG_NONSTANDARD = 0x00000001;	// if it was user created
		constexpr static std::uint32_t FLAG_MODIFIED = 0x00000002;	// if it's a standard fuel type and it was modified
		constexpr static std::uint32_t FLAG_MODIFIED_NAME = 0x00000008;	// if it's a standard fuel that is unmodified 'cept for its name
		constexpr static std::uint32_t FLAG_USED = 0x00000004;	// whether it's used in the grid or not

		bool IsNonFuel();
		bool Equals(Fuel *f);
		std::string SetModifiedBits(FuelCollection *canada, FuelCollection *nz, FuelCollection *taz);	// sets the appropriate bits (above) based on its default fuel
		Fuel *FindDefault(FuelCollection *canada, FuelCollection *nz, FuelCollection *taz);		// returns the handle (if any) for the default fuel for this one
		static Fuel *FromCOM(ICWFGM_Fuel *fuel);

		bool NeedsFMC();

		static bool NeedsFMC(ICWFGM_Fuel *fuel);

		std::string DisplayName(CCWFGM_FuelMap *fuelMap);

		std::string PrintOptionalParams(FuelCollection * m_fuelCollection, BOOL IsC6);

		std::string PrintReportChanges(FuelCollection * m_fuelCollection);
		void PrintReportChanges(FuelCollection* m_fuelCollection, void* m_data);

		std::string GetOptionalAttrNameUnit(USHORT key, const std::string &val);

		std::string GetAttributeName(USHORT key);

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options, ISerializationData* userData) override;
		virtual Fuel *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};
};


typedef Project::Fuel *(*unknownFuelFcn)(APTR parameter, USHORT line_num, const TCHAR *name, const TCHAR *agency_name, LONG import_val, LONG export_val, COLORREF color);
typedef BOOL (*badLineFcn)(APTR parameter, USHORT line_num);


class FuelSerializationData : public ISerializationData
{
public:
	WISE::GridProto::CwfgmFuelMap_CwfgmFuelData* fuel = nullptr;
};


class FuelCollectionSerializationData : public ISerializationData
{
public:
	WISE::GridProto::CwfgmFuelMap* fuelMap = nullptr;
};


namespace Project
{
	class CWFGMPROJECT_EXPORT FuelCollection : public ISerializeProto
	{
		friend Fuel;

	public:
		static constexpr std::uint16_t DEFAULTCOUNTRY_CANADA = 0;
		static constexpr std::uint16_t DEFAULTCOUNTRY_NEWZEALAND = 1;
		static constexpr std::uint16_t DEFAULTCOUNTRY_TASMANIA = 2;
		static constexpr std::uint16_t DEFAULTCOUNTRY_NONE = ((std::uint16_t)-1);

		static boost::intrusive_ptr<CCWFGM_FuelMap>	m_fuelMap_Canada;		// application-wide default fuels
		static boost::intrusive_ptr<CCWFGM_FuelMap>	m_fuelMap_NewZealand;
		static boost::intrusive_ptr<CCWFGM_FuelMap>	m_fuelMap_Tasmania;
		static FuelCollection			*m_fuelCollection_Canada;	// for CWFGM-specified app-wide default fuel info
		static FuelCollection			*m_fuelCollection_NewZealand;
		static FuelCollection			*m_fuelCollection_Tasmania;
		static std::uint16_t			m_defaultCountry;
		std::optional<std::string>		m_csLUTFileName;
		std::string					m_loadWarnings;
		std::optional<bool>				m_showUnusedFuels;

		FuelCollection(CCWFGM_FuelMap *fuelMap = nullptr, bool CWFGM_defaults = false);
		FuelCollection(CCWFGM_FuelMap *fuelMap, const TCHAR *group_name);

		virtual ~FuelCollection();

		void SaveToIniFile(const TCHAR *group_name);
		void Clone(FuelCollection *into) const;
		void Delete();				// we did a clone, but now we're going to load so we have to undo the work of the clone,
							// also called by the destructor
		std::string CalculateModifiedFuelsBit(FuelCollection *canada, FuelCollection *nz, FuelCollection *taz);

		Fuel *FindWithFBPCode(const TCHAR *fbp) const;
		Fuel *FindName(const TCHAR *name) const;					// usually used
		std::string MakeUniqueName(Fuel *fuel, const TCHAR *name) const;

		HRESULT PCOM_FindFuel(const TCHAR *name, UCHAR *index, Fuel **fuel);

		std::uint16_t ImportFuelMap(const TCHAR *file_name, FuelCollection *docFuelCollection, unknownFuelFcn fcn1, badLineFcn fcn2, APTR parm);
		std::uint16_t ExportFuelMap(const TCHAR *file_name) const;

		boost::intrusive_ptr<CCWFGM_FuelMap>	m_fuelMap;

		bool IsImported() const				{ if (m_isImportedLUT.has_value()) return m_isImportedLUT.value(); return false; };

		virtual Fuel *NewFuel(ICWFGM_Fuel *fuel, COLORREF color, const std::string &defaultFBP, const std::string &comments) const
									{ return new Fuel(fuel, color, defaultFBP, comments); };
		virtual Fuel *NewFuel(ICWFGM_Fuel *fuel, const Fuel *f) const
									{ return new Fuel(fuel, f); };

		Fuel* FuelTypeCopy(ICWFGM_Fuel *fuel, LONG file_index, LONG fuel_index, bool warn);

    private:
		std::optional<bool> m_isImportedLUT;

		protected:
		Fuel* FuelTypeCopy(ICWFGM_FBPFuel *fuel, LONG file_index, LONG fuel_index, bool warn);

		Fuel *AddFuel(const TCHAR *id);
		Fuel *AddFuel(const struct StaticFuelData *fuel_desc);
							// add a fuel to the collection.  fuel type tells what
							// set of attributes to apply, color is the color on the mapview.
							// count is the file index, flag tells if it;s a special case, like water.

		bool LoadProtoFuelmap(const char *filename);
		bool SaveProtoFuelmap(const char *filename);

		Fuel *getFuel(FuelCollection *docFuelCollection, const TCHAR *fbp_name, const TCHAR *agency_name, LONG import_num, LONG export_num, COLORREF color, bool try_to_add);
		void getDefaultFuels(USHORT country);

	public:
		virtual std::int32_t serialVersionUid(const SerializeProtoOptions& options) const noexcept override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options) override;
		virtual google::protobuf::Message* serialize(const SerializeProtoOptions& options, ISerializationData* userData) override;
		virtual FuelCollection *deserialize(const google::protobuf::Message& proto, std::shared_ptr<validation::validation_object> valid, const std::string& name) override;
		std::optional<bool> isdirty(void) const noexcept override { return std::nullopt; }
	};
};

/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FUELCOLLECTION_H__354D4244_16E0_11D4_BCD7_00A0833B1640__INCLUDED_)
