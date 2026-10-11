#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Use cases of the dependency pool (dependency injection), one translation unit
# each:
#   accept/<name>.cpp - must compile and run successfully
#                       tests: pool_accept_<name>_build, pool_accept_<name>
#   reject/<name>.cpp - must NOT compile; the same TU compiled with
#                       SML_POOL_CONTROL (the closest valid usage) must compile,
#                       so a reject test cannot pass for an unrelated reason
#                       tests: pool_reject_<name>_control, pool_reject_<name>
#
# Every target is EXCLUDE_FROM_ALL and built from within ctest, so a use case
# that is not supported yet shows up as a failing test instead of breaking the
# build of everything else.

function(sml_pool_build_test test_name target)
  add_test(NAME ${test_name}
           COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${target} --config $<CONFIG>)
  # concurrent builds in one build tree are not safe
  set_tests_properties(${test_name} PROPERTIES RESOURCE_LOCK sml_build_tree LABELS "pool;${ARGN}")
endfunction()

# TUs using std::expected need C++23. Without it (old CMake or compiler) they
# fail to build, which shows up as a failing test (a reject test via its
# control), never as a false pass.
function(sml_pool_language_standard target name)
  if(name MATCHES "expected" AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.20)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 23)
  endif()
endfunction()

file(GLOB accept_sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/accept/*.cpp)
foreach(src ${accept_sources})
  get_filename_component(name ${src} NAME_WE)
  set(target pool_accept_${name})
  add_executable(${target} EXCLUDE_FROM_ALL ${src})
  sml_pool_language_standard(${target} ${name})
  sml_pool_build_test(${target}_build ${target} accept)
  set_tests_properties(${target}_build PROPERTIES FIXTURES_SETUP ${target})
  add_test(NAME ${target} COMMAND ${target})
  set_tests_properties(${target} PROPERTIES FIXTURES_REQUIRED ${target} LABELS "pool;accept")
endforeach()

file(GLOB reject_sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/reject/*.cpp)
foreach(src ${reject_sources})
  get_filename_component(name ${src} NAME_WE)
  set(target pool_reject_${name})

  add_library(${target}_control OBJECT EXCLUDE_FROM_ALL ${src})
  target_compile_definitions(${target}_control PRIVATE SML_POOL_CONTROL)
  sml_pool_language_standard(${target}_control ${name})
  sml_pool_build_test(${target}_control ${target}_control reject)
  set_tests_properties(${target}_control PROPERTIES FIXTURES_SETUP ${target})

  add_library(${target} OBJECT EXCLUDE_FROM_ALL ${src})
  sml_pool_language_standard(${target} ${name})
  sml_pool_build_test(${target} ${target} reject)
  # a broken control skips (= fails) the reject test instead of letting it pass
  set_tests_properties(${target} PROPERTIES WILL_FAIL TRUE FIXTURES_REQUIRED ${target})
endforeach()
