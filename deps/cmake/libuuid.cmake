include(ExternalProject)
message("uuid dir:${CMAKE_CURRENT_BINARY_DIR}/libuuid")
SET(LIBUUID_CONFIGURE_COMMAND cd ${SRC_DIR}/libuuid && ./configure --prefix=${CMAKE_INSTALL_PREFIX})
SET(LIBUUID_BUILD_COMMAND cd ${SRC_DIR}/libuuid && make)
SET(LIBUUID_INSTALL_COMMAND cd ${SRC_DIR}/libuuid && make install)

ExternalProject_Add(libuuid
  # INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  SOURCE_DIR  ${SRC_DIR}/libuuid
  # URL https://udomain.dl.sourceforge.net/project/libuuid/libuuid-1.0.3.tar.gz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libuuid
  CONFIGURE_COMMAND ${LIBUUID_CONFIGURE_COMMAND}
  BUILD_COMMAND ${LIBUUID_BUILD_COMMAND}
  INSTALL_COMMAND ${LIBUUID_INSTALL_COMMAND}
  )