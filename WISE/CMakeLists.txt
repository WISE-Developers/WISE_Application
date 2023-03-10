cmake_minimum_required (VERSION 3.13)
if (MSVC)
set (CMAKE_SYSTEM_VERSION 10.0.22000.0)
endif (MSVC)
project (WISE VERSION ${PROMETHEUS_VERSION} DESCRIPTION "Wildfire Intelligence and Simulation Engine")

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
SET(DEFAULTS_INCLUDE_DIR "error" CACHE STRING "The path to the defaults include files")
SET(COMMS_INCLUDE_DIR "error" CACHE STRING "The path to the server comms include files")
SET(STATUS_INCLUDE_DIR "error" CACHE STRING "The path to the job status include files")
SET(PAHO_INSTALL_PATH "error" CACHE STRING "The path to the Paho install")
SET(PAHOPP_INSTALL_PATH "error" CACHE STRING "The path to the Paho C++ install")

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
find_library(FOUND_PROJECT_LIBRARY_PATH NAMES project REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_GDAL_LIBRARY_PATH NAMES gdal gdal_i REQUIRED PATHS ${GDAL_LIBRARY_DIR})
find_library(FOUND_MINIZIP_LIBRARY_PATH NAMES minizip REQUIRED PATHS ${GDAL_LIBRARY_DIR})
find_library(FOUND_DEFAULTS_LIBRARY_PATH NAMES defaultsfile REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_STATUS_LIBRARY_PATH NAMES protostatus REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_COMMS_LIBRARY_PATH NAMES servercomms REQUIRED PATHS ${LOCAL_LIBRARY_DIR})
find_library(FOUND_PAHO_LIBRARY_PATH NAMES paho-mqtt3a REQUIRED PATHS ${PAHO_INSTALL_PATH}/lib)
find_library(FOUND_PAHOPP_LIBRARY_PATH NAMES paho-mqttpp3 REQUIRED PATHS ${PAHOPP_INSTALL_PATH}/lib)

if (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -DPROTOBUF_USE_DLLS -D_NO_GUI -D_SERIALIZE_PROTO -DBOOST_ALL_NO_LIB -DBOOST_ALL_DYN_LINK -D_CRT_SECURE_NO_WARNINGS /Zc:__cplusplus /verbosity:detailed")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD -D_AFXDLL")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd -D_AFXDLL /W4")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS atomic chrono date_time system iostreams program_options filesystem REQUIRED PATHS ${BOOST_LIBRARY_DIR})
else ()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

set(Boost_LIBRARIES
        -lboost_iostreams
        -lboost_program_options
        -lboost_filesystem
        -lboost_system
        -lboost_date_time)
endif (MSVC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_NO_MFC")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")

find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

add_executable(wise
    include/misc_native.h
    include/mqtt.h
    include/Stats.h
    cpp/Initialization.cpp
    cpp/Initialization.proto.cpp
    cpp/misc_native.cpp
    cpp/mqtt.cpp
    cpp/Simulation.cpp
    cpp/WISE_P.cpp
    cpp/Stats.cpp
    cpp/status.cpp
)

target_include_directories(wise
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
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC ${GSL_INCLUDE_DIR}
    PUBLIC ${FIREENGINE_INCLUDE_DIR}
    PUBLIC ${KMLHELPER_INCLUDE_DIR}
    PUBLIC ${DEFAULTS_INCLUDE_DIR}
    PUBLIC ${COMMS_INCLUDE_DIR}
    PUBLIC ${STATUS_INCLUDE_DIR}
    PUBLIC ${PAHO_INSTALL_PATH}/include
    PUBLIC ${STATUS_INCLUDE_DIR}/include
    PUBLIC ${PAHOPP_INSTALL_PATH}/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../WISE_Project/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(wise PROPERTIES VERSION ${CMAKE_PROJECT_VERSION})
set_target_properties(wise PROPERTIES SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR})
set_target_properties(wise PROPERTIES DEFINE_SYMBOL "")

set_target_properties(wise PROPERTIES
    PUBLIC_HEADER include/WISE.h
    PUBLIC_HEADER include/WISE_P.h
)

target_link_libraries(wise ${FOUND_FWI_LIBRARY_PATH} ${FOUND_FUEL_LIBRARY_PATH} ${FOUND_GRID_LIBRARY_PATH} ${FOUND_WEATHER_LIBRARY_PATH} ${FOUND_FIREENGINE_LIBRARY_PATH} ${FOUND_PROJECT_LIBRARY_PATH})
target_link_libraries(wise ${FOUND_KMLHELPER_LIBRARY_PATH} ${FOUND_REDAPP_LIBRARY_PATH} ${FOUND_DEFAULTS_LIBRARY_PATH} ${FOUND_STATUS_LIBRARY_PATH} ${FOUND_COMMS_LIBRARY_PATH})
target_link_libraries(wise ${FOUND_WTIME_LIBRARY_PATH} ${FOUND_LOWLEVEL_LIBRARY_PATH} ${FOUND_MULTITHREAD_LIBRARY_PATH} ${FOUND_ERROR_CALC_LIBRARY_PATH} ${FOUND_MATH_LIBRARY_PATH} ${FOUND_GEOGRAPHY_LIBRARY_PATH})
target_link_libraries(wise ${FOUND_PAHO_LIBRARY_PATH} ${FOUND_PAHOPP_LIBRARY_PATH})
target_link_libraries(wise ${FOUND_GDAL_LIBRARY_PATH} ${FOUND_PROTOBUF_LIBRARY_PATH} ${FOUND_MINIZIP_LIBRARY_PATH} ${Boost_LIBRARIES})
if (MSVC)
add_library(ZLib STATIC IMPORTED)
set(ZLIB_FILE_PATH "${LOCAL_LIBRARY_DIR}/zlib.lib")
set_target_properties(ZLib PROPERTIES IMPORTED_LOCATION ${ZLIB_FILE_PATH})
target_link_libraries(wise ZLib)

target_link_directories(wise PUBLIC ${LOCAL_LIBRARY_DIR})
else ()
target_link_libraries(wise -lstdc++fs -lrt -lquadmath)
target_link_directories(wise PUBLIC ${BOOST_LIBRARY_DIR}/lib)
endif (MSVC)
