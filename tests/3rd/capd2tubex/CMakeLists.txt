# ==================================================================
#  tubex-lib / tests - cmake configuration file
# ==================================================================

  set(TESTS_NAME tubex-tests-3rd-capd)

  list(APPEND SRC_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/tests_capd2tubex.cpp
                        )

  # Looking for CAPD
  pkg_search_module(PKG_CAPD REQUIRED capd capd-gui mpcapd mpcapd-gui)

  add_executable(${TESTS_NAME} ${SRC_TESTS})
  # todo: find a clean way to access tubex header files?
  set(TUBEX_HEADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/../../include)
  target_include_directories(${TESTS_NAME} SYSTEM PUBLIC ${TUBEX_HEADERS_DIR}
                                                         ${CMAKE_CURRENT_SOURCE_DIR}/../../catch
                                                         ${PKG_CAPD_INCLUDE_DIRS})
  target_link_libraries(${TESTS_NAME} PUBLIC Ibex::ibex tubex tubex-capd ${PKG_CAPD_LDFLAGS})
  add_dependencies(check ${TESTS_NAME})
  add_test(NAME ${TESTS_NAME} COMMAND ${TESTS_NAME})