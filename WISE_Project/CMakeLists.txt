cmake_minimum_required (VERSION 3.13)
if (MSVC)
set (CMAKE_SYSTEM_VERSION 10.0.22000.0)
endif (MSVC)
project (Project VERSION ${PROMETHEUS_VERSION} DESCRIPTION "Fire Engine Library")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

SET(LOWLEVEL_INCLUDE_DIR "error" CACHE STRING "The path to the includes from HSS LowLevel")
SET(WTIME_INCLUDE_DIR "error" CACHE STRING "The path to the WTime libraries includes")
SET(MULTITHREAD_INCLUDE_DIR "error" CACHE STRING "The path to the includes from HSS Multithread")
SET(MATH_INCLUDE_DIR "error" CACHE STRING "The path to HSS_Math include files")
SET(GEOGRAPHY_INCLUDE_DIR "error" CACHE STRING "The path to HSS_Math include files")
SET(THIRD_PARTY_INCLUDE_DIR "error" CACHE STRING "The path to third party include files")
SET(ERROR_CALC_INCLUDE_DIR "error" CACHE STRING "The path to the error calc include files")
SET(FWI_INCLUDE_DIR "error" CACHE STRING "The path to the FWI include files")
SET(WEATHER_INCLUDE_DIR "error" CACHE STRING "The path to the weather include files")
SET(FUEL_INCLUDE_DIR "error" CACHE STRING "The path to the fuel include files")
SET(GRID_INCLUDE_DIR "error" CACHE STRING "The path to the grid include files")
SET(FIREENGINE_INCLUDE_DIR "error" CACHE STRING "The path to the fire engine include files")
SET(REDAPP_INCLUDE_DIR "error" CACHE STRING "The path to the REDappWrapper include files")
SET(GDAL_INCLUDE_DIR "error" CACHE STRING "The path to the GDAL include files")
SET(GSL_INCLUDE_DIR "error" CACHE STRING "The path to the GSL include files")
SET(KMLHELPER_INCLUDE_DIR "error" CACHE STRING "The path to the GSL include files")
SET(PROTOBUF_INCLUDE_DIR "error" CACHE STRING "The path to the protobuf include files")
SET(BOOST_INCLUDE_DIR "error" CACHE STRING "The path to the boost include files")

find_library(FOUND_MULTITHREAD_LIBRARY_PATH NAMES Multithread REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_LOWLEVEL_LIBRARY_PATH NAMES LowLevel REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_WTIME_LIBRARY_PATH NAMES WTime REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_MATH_LIBRARY_PATH NAMES hssmath REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_GEOGRAPHY_LIBRARY_PATH NAMES hssgeography REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_PROTOBUF_LIBRARY_PATH NAMES protobuf libprotobuf REQUIRED PATHS ${PROTOBUF_LIBRARY_DIR})
find_library(FOUND_ERROR_CALC_LIBRARY_PATH NAMES ErrorCalc REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_FWI_LIBRARY_PATH NAMES fwi REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_FUEL_LIBRARY_PATH NAMES fuel REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_GRID_LIBRARY_PATH NAMES grid REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_REDAPP_LIBRARY_PATH NAMES REDappWrapper REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_WEATHER_LIBRARY_PATH NAMES weather REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_FIREENGINE_LIBRARY_PATH NAMES fireengine REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_KMLHELPER_LIBRARY_PATH NAMES kmllib REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_GDAL_LIBRARY_PATH NAMES gdal gdal_i REQUIRED PATHS ${GDAL_LIBRARY_DIR})
find_library(FOUND_MINIZIP_LIBRARY_PATH NAMES minizip REQUIRED PATHS ${GDAL_LIBRARY_DIR})

if (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -DPROTOBUF_USE_DLLS -D_NO_GUI -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK -D_CRT_SECURE_NO_WARNINGS /Zc:__cplusplus")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD -D_AFXDLL")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd -D_AFXDLL /W4")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS atomic chrono system iostreams REQUIRED PATHS ${BOOST_LIBRARY_DIR})
else ()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

set(Boost_LIBRARIES
    -L${BOOST_LIBRARY_DIR}/lib
    -lboost_system
    -lboost_iostreams)
endif (MSVC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_NO_MFC")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

add_library(project SHARED
    cpp/AngleGridFilter.cpp
    include/cwfgmp_config.h
    cpp/assetcollection.cpp
    cpp/assetcollection.Serialize.cpp
    cpp/AssetStats.cpp
    cpp/AttributeGridFilter.cpp
    cpp/BasicScenarioOptions.cpp
    cpp/BurnPeriodOption.cpp
    cpp/CWFGMProject.cpp
    cpp/CWFGMProject.proto.cpp
    cpp/CWFGMProject.Serialize.cpp
    cpp/FBPOptions.cpp
    cpp/FGMOutputs.cpp
    cpp/Fire.cpp
    cpp/FireCollection.cpp
    cpp/FireCollection.proto.cpp
    cpp/FireCollection.Serialize.cpp
    cpp/FuelCollection.cpp
    cpp/FuelCollection.proto.cpp
    cpp/FuelCollection.Serialize.cpp
    cpp/FuelGridFilter.cpp
    cpp/GeoJSONHelper.cpp
    cpp/GISOptions.cpp
    cpp/GreenupGridFilter.cpp
    cpp/GridFilter.cpp
    cpp/GridFilter.proto.cpp
    cpp/GridFilterCollection.cpp
    cpp/GridFilterCollection.Serialize.cpp
    cpp/LandscapeWarningOptions.cpp
    cpp/minizip_stream.cpp
    cpp/PercentConiferGridFilter.cpp
    cpp/PrintReportOptions.cpp
    cpp/PrjWeatherStation.cpp
    cpp/PrjWeatherStream.cpp
    cpp/ReplaceGridFilter.cpp
    cpp/Scenario.cpp
    cpp/Scenario.proto.cpp
    cpp/ScenarioCollection.cpp
    cpp/ScenarioCollection.Serialize.cpp
    cpp/StaticData.cpp
    cpp/StatsBuilder.cpp
    cpp/statsentry.cpp
    cpp/targetcollection.cpp
    cpp/targetcollection.Serialize.cpp
    cpp/TimestepSettings.cpp
    cpp/vectorcollection.cpp
    cpp/vectorcollection.Serialize.cpp
    cpp/VectorExportOptions.cpp
    cpp/viewstorage.cpp
    cpp/WeatherCollection.cpp
    cpp/WeatherCollection.proto.cpp
    cpp/WeatherCollection.Serialize.cpp
    cpp/WeatherGridFilter.cpp
    cpp/WeatherGridManager.cpp
    cpp/WeatherOptions.cpp
    cpp/WindDirectionGrid.cpp
    cpp/WindSpeedGrid.cpp
    cpp/WxWSEntry.cpp
    cpp/assetCollection.pb.cc
    cpp/cwfgmProject.pb.cc
    cpp/fuelCollection.pb.cc
    cpp/gridCollection.pb.cc
    cpp/projectGridFilter.pb.cc
    cpp/projectIgnition.pb.cc
    cpp/projectScenario.pb.cc
    cpp/projectWindGrid.pb.cc
    cpp/staticPoints.pb.cc
    cpp/summary.pb.cc
    cpp/vectorCollection.pb.cc
    cpp/viewStorage.pb.cc
    cpp/weatherStation.pb.cc
)

if (MSVC)
target_sources(project
    PRIVATE cpp/cwinapp.cpp
)
endif()

target_include_directories(project
    PUBLIC ${MATH_INCLUDE_DIR}
    PUBLIC ${FWI_INCLUDE_DIR}
    PUBLIC ${LOWLEVEL_INCLUDE_DIR}
    PUBLIC ${MULTITHREAD_INCLUDE_DIR}
    PUBLIC ${WTIME_INCLUDE_DIR}
    PUBLIC ${PROTOBUF_INCLUDE_DIR}
    PUBLIC ${FUEL_INCLUDE_DIR}
    PUBLIC ${GEOGRAPHY_INCLUDE_DIR}
    PUBLIC ${ERROR_CALC_INCLUDE_DIR}
    PUBLIC ${WEATHER_INCLUDE_DIR}
    PUBLIC ${GDAL_INCLUDE_DIR}
    PUBLIC ${GRID_INCLUDE_DIR}
    PUBLIC ${REDAPP_INCLUDE_DIR}
    PUBLIC ${THIRD_PARTY_INCLUDE_DIR}
    PUBLIC ${BOOST_INCLUDE_DIR}
    PUBLIC ${GSL_INCLUDE_DIR}
    PUBLIC ${FIREENGINE_INCLUDE_DIR}
    PUBLIC ${KMLHELPER_INCLUDE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(project PROPERTIES VERSION ${CMAKE_PROJECT_VERSION})
set_target_properties(project PROPERTIES SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR})
set_target_properties(project PROPERTIES DEFINE_SYMBOL "CWFGMPROJECT_DLL")

set_target_properties(project PROPERTIES
    PUBLIC_HEADER AngleGridFilter.h
    PUBLIC_HEADER assetcollection.h
    PUBLIC_HEADER AssetStats.h
    PUBLIC_HEADER AttributeGridFilter.h
    PUBLIC_HEADER BasicScenarioOptions.h
    PUBLIC_HEADER BurnPeriodOption.h
    PUBLIC_HEADER CWFGMProject.h
    PUBLIC_HEADER ElevStruct.h
    PUBLIC_HEADER FBPOptions.h
    PUBLIC_HEADER FGMOutputs.h
    PUBLIC_HEADER FireCollection.h
    PUBLIC_HEADER FuelCollection.h
    PUBLIC_HEADER FuelGridFilter.h
    PUBLIC_HEADER GeoJSONHelper.h
    PUBLIC_HEADER GISOptions.h
    PUBLIC_HEADER GreenupGridFilter.h
    PUBLIC_HEADER GridFilter.h
    PUBLIC_HEADER GridFilterCollection.h
    PUBLIC_HEADER IJsonConvert.h
    PUBLIC_HEADER LandscapeWarningOptions.h
    PUBLIC_HEADER minizip_stream.h
    PUBLIC_HEADER PercentConiferGridFilter.h
    PUBLIC_HEADER PermissibleVectorReadDrivers.h
    PUBLIC_HEADER PrintReportOptions.h
    PUBLIC_HEADER ReplaceGridFilter.h
    PUBLIC_HEADER ScenarioCollection.h
    PUBLIC_HEADER StaticData.h
    PUBLIC_HEADER StaticDataStruct.h
    PUBLIC_HEADER StaticUpgradeData.h
    PUBLIC_HEADER StatsBuilder.h
    PUBLIC_HEADER statsentry.h
    PUBLIC_HEADER StdFireCollection.h
    PUBLIC_HEADER SummaryCollection.h
    PUBLIC_HEADER targetcollection.h
    PUBLIC_HEADER TimestepSettings.h
    PUBLIC_HEADER vectorcollection.h
    PUBLIC_HEADER viewstorage.h
    PUBLIC_HEADER WeatherCollection.h
    PUBLIC_HEADER WeatherGridFilter.h
    PUBLIC_HEADER WeatherGridManager.h
    PUBLIC_HEADER WeatherOptions.h
    PUBLIC_HEADER WindDirectionGrid.h
    PUBLIC_HEADER WindSpeedGrid.h
    PUBLIC_HEADER WxWSEntry.h
)

target_link_libraries(project ${FOUND_GDAL_LIBRARY_PATH} ${Boost_LIBRARIES} ${FOUND_PROTOBUF_LIBRARY_PATH} ${FOUND_MINIZIP_LIBRARY_PATH})
target_link_libraries(project ${FOUND_FWI_LIBRARY_PATH} ${FOUND_FUEL_LIBRARY_PATH} ${FOUND_GRID_LIBRARY_PATH} ${FOUND_WEATHER_LIBRARY_PATH} ${FOUND_FIREENGINE_LIBRARY_PATH})
target_link_libraries(project ${FOUND_KMLHELPER_LIBRARY_PATH} ${FOUND_REDAPP_LIBRARY_PATH})
target_link_libraries(project ${FOUND_WTIME_LIBRARY_PATH} ${FOUND_LOWLEVEL_LIBRARY_PATH} ${FOUND_MULTITHREAD_LIBRARY_PATH} ${FOUND_ERROR_CALC_LIBRARY_PATH} ${FOUND_MATH_LIBRARY_PATH} ${FOUND_GEOGRAPHY_LIBRARY_PATH})
if (MSVC)
else ()
target_link_libraries(project -lstdc++fs)
endif (MSVC)
