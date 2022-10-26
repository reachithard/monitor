# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
Findlibuuid
-----------

Find the uuid library (libuuid).

IMPORTED Targets
^^^^^^^^^^^^^^^^

.. versionadded:: 3.12

The following :prop_tgt:`IMPORTED` targets may be defined:

``libuuid::libuuid``
  libuuid library.

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``libuuid_FOUND``
  true if libuuid headers and libraries were found
``libuuid_INCLUDE_DIR``
  the directory containing libuuid headers
``libuuid_INCLUDE_DIRS``
  list of the include directories needed to use libuuid
``libuuid_LIBRARIES``
  libuuid libraries to be linked

Cache variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``libuuid_INCLUDE_DIR``
  the directory containing libuuid headers
``libuuid_LIBRARY``
  path to the libuuid library
#]=======================================================================]

# use pkg-config to get the directories and then use these values
# in the find_path() and find_library() calls
find_package(PkgConfig QUIET)
set(SEARCH_PATH "${BASE_DIR}/output")
message("${SEARCH_PATH}")

find_path(libuuid_INCLUDE_DIR NAMES uuid/uuid.h
   HINTS
   ${SEARCH_PATH}/include
   PATH_SUFFIXES uuid
   )

# CMake 3.9 and below used 'libuuid_LIBRARIES' as the name of
# the cache entry storing the find_library result.  Use the
# value if it was set by the project or user.
if(DEFINED libuuid_LIBRARIES AND NOT DEFINED libuuid_LIBRARY)
  set(libuuid_LIBRARY ${libuuid_LIBRARIES})
endif()

find_library(libuuid_LIBRARY NAMES libuuid.a libuuid.so
   HINTS
   ${SEARCH_PATH}/lib
   ${SEARCH_PATH}/lib64
   )

set(libuuid_INCLUDE_DIRS ${libuuid_INCLUDE_DIR})
set(libuuid_LIBRARIES ${libuuid_LIBRARY})

find_package(PackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libuuid
                                  REQUIRED_VARS libuuid_LIBRARY libuuid_INCLUDE_DIR)

mark_as_advanced(libuuid_INCLUDE_DIR libuuid_LIBRARY)

if(libuuid_FOUND AND NOT TARGET libuuid::libuuid)
  add_library(libuuid::libuuid UNKNOWN IMPORTED)
  set_target_properties(libuuid::libuuid PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${libuuid_INCLUDE_DIRS}")
  set_target_properties(libuuid::libuuid PROPERTIES INTERFACE_COMPILE_OPTIONS "${libuuid_DEFINITIONS}")
  set_property(TARGET libuuid::libuuid APPEND PROPERTY IMPORTED_LOCATION "${libuuid_LIBRARY}")
endif()
