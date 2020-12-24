#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "shapelib::shp" for configuration "MinSizeRel"
set_property(TARGET shapelib::shp APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(shapelib::shp PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/shp.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/shp.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS shapelib::shp )
list(APPEND _IMPORT_CHECK_FILES_FOR_shapelib::shp "${_IMPORT_PREFIX}/lib/shp.lib" "${_IMPORT_PREFIX}/bin/shp.dll" )

# Import target "shapelib::shpcreate" for configuration "MinSizeRel"
set_property(TARGET shapelib::shpcreate APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(shapelib::shpcreate PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/shpcreate.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS shapelib::shpcreate )
list(APPEND _IMPORT_CHECK_FILES_FOR_shapelib::shpcreate "${_IMPORT_PREFIX}/bin/shpcreate.exe" )

# Import target "shapelib::shpadd" for configuration "MinSizeRel"
set_property(TARGET shapelib::shpadd APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(shapelib::shpadd PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/shpadd.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS shapelib::shpadd )
list(APPEND _IMPORT_CHECK_FILES_FOR_shapelib::shpadd "${_IMPORT_PREFIX}/bin/shpadd.exe" )

# Import target "shapelib::shpdump" for configuration "MinSizeRel"
set_property(TARGET shapelib::shpdump APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(shapelib::shpdump PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/shpdump.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS shapelib::shpdump )
list(APPEND _IMPORT_CHECK_FILES_FOR_shapelib::shpdump "${_IMPORT_PREFIX}/bin/shpdump.exe" )

# Import target "shapelib::shprewind" for configuration "MinSizeRel"
set_property(TARGET shapelib::shprewind APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(shapelib::shprewind PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/shprewind.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS shapelib::shprewind )
list(APPEND _IMPORT_CHECK_FILES_FOR_shapelib::shprewind "${_IMPORT_PREFIX}/bin/shprewind.exe" )

# Import target "shapelib::dbfcreate" for configuration "MinSizeRel"
set_property(TARGET shapelib::dbfcreate APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(shapelib::dbfcreate PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/dbfcreate.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS shapelib::dbfcreate )
list(APPEND _IMPORT_CHECK_FILES_FOR_shapelib::dbfcreate "${_IMPORT_PREFIX}/bin/dbfcreate.exe" )

# Import target "shapelib::dbfadd" for configuration "MinSizeRel"
set_property(TARGET shapelib::dbfadd APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(shapelib::dbfadd PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/dbfadd.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS shapelib::dbfadd )
list(APPEND _IMPORT_CHECK_FILES_FOR_shapelib::dbfadd "${_IMPORT_PREFIX}/bin/dbfadd.exe" )

# Import target "shapelib::dbfdump" for configuration "MinSizeRel"
set_property(TARGET shapelib::dbfdump APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(shapelib::dbfdump PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/dbfdump.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS shapelib::dbfdump )
list(APPEND _IMPORT_CHECK_FILES_FOR_shapelib::dbfdump "${_IMPORT_PREFIX}/bin/dbfdump.exe" )

# Import target "shapelib::shptreedump" for configuration "MinSizeRel"
set_property(TARGET shapelib::shptreedump APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(shapelib::shptreedump PROPERTIES
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/shptreedump.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS shapelib::shptreedump )
list(APPEND _IMPORT_CHECK_FILES_FOR_shapelib::shptreedump "${_IMPORT_PREFIX}/bin/shptreedump.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
