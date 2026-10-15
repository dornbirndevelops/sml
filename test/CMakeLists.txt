#
# Copyright (c) 2016-2020 Jean Davy
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
project(test LANGUAGES CXX)

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DCHECK_COMPILE_TIME -DBOOST_ALL_NO_LIB -DBUILD_SHARED_LIBS=OFF)

add_subdirectory(unit)

add_executable(mre mre.cpp)
target_link_libraries(mre PRIVATE sml::sml)

add_executable(scenario1 scenario1.cpp)
target_link_libraries(scenario1 PRIVATE sml::sml)

add_executable(scenario2 scenario2.cpp)
target_link_libraries(scenario2 PRIVATE sml::sml)

add_executable(scenario3 scenario3.cpp)
target_link_libraries(scenario3 PRIVATE sml::sml)

add_executable(scenario4 scenario4.cpp)
target_link_libraries(scenario4 PRIVATE sml::sml)

if(IS_COMPILER_OPTION_MSVC_LIKE)
  add_compile_options("-FIcommon/test.hpp")
elseif(IS_COMPILER_OPTION_GCC_LIKE)
  add_compile_options(-include common/test.hpp)
endif()

add_subdirectory(ft)
add_subdirectory(ut)
