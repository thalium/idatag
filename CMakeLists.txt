cmake_minimum_required (VERSION 3.10.0)
project(IDATag)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

#set(QT_MOC_EXECUTABLE "$ENV{QTHEADER}/../bin/moc.exe")
set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.3.0\\5.3\\msvc2013_64")
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

get_filename_component(root_dir     ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
get_filename_component(bin_d_dir    ${root_dir}/bin/${PROJECT_NAME}_debug_${ARCH} ABSOLUTE)
get_filename_component(bin_dir      ${root_dir}/bin/${PROJECT_NAME}_${ARCH} ABSOLUTE)

if("$ENV{IDASDK}" STREQUAL "")
    message(FATAL_ERROR "missing IDASDK environment variable")
endif()
get_filename_component(IDASDK "$ENV{IDASDK}" ABSOLUTE)
message("-- Using IDASDK=${IDASDK}")


if("$ENV{IDATARGET}" STREQUAL "")
    message(FATAL_ERROR "missing IDATARGET environment variable")
endif()
message("-- Using IDATARGET=$ENV{IDATARGET}")

if("$ENV{QTHEADER}" STREQUAL "")
    message(FATAL_ERROR "missing QTHEADER environment variable")
endif()
get_filename_component(QTHEADER "$ENV{QTHEADER}" ABSOLUTE)
message("-- Using QTHEADER=$ENV{QTHEADER}")

add_library(${PROJECT_NAME}  SHARED
	src/idatag.hpp
	src/idatag.cpp
	src/json.hpp
	src/model.hpp
	src/model.cpp
	src/view.hpp
	src/view.cpp
	src/proxy.hpp
	src/proxy.cpp
	src/delegate.hpp
	src/delegate.cpp
	src/palette.hpp
	src/palette.cpp
	src/configuration.cpp
	src/configuration.hpp
	src/hooks_disas.cpp
	src/hooks_disas.hpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_BINARY_DIR}/bin"
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_BINARY_DIR}/bin"
	LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_BINARY_DIR}/bin"
	LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_BINARY_DIR}/bin"
   )

target_include_directories(${PROJECT_NAME} PRIVATE ";${IDASDK}/include")
target_include_directories(${PROJECT_NAME} PRIVATE ";${QTHEADER}")

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)


if("$ENV{IDATARGET}" STREQUAL "IDA64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
		__NT__
		__IDP__
		__X64__
		__EA64__
		USE_DANGEROUS_FUNCTIONS
		USE_STANDARD_FILE_FUNCTIONS
		__QT__
		QT_NAMESPACE=QT
		QT_DLL
		QT_GUI_LIB
		QT_CORE_LIB
		QT_THREAD_SUPPORT
	)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		"${IDASDK}/lib/x64_win_vc_64/ida.lib"
		"${IDASDK}/lib/x64_win_qt/Qt5Core.lib"
		"${IDASDK}/lib/x64_win_qt/Qt5Widgets.lib"
		"${IDASDK}/lib/x64_win_qt/Qt5Gui.lib"
	)
	set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "64.dll")
	# C4267 : IDA SDK => conversion size_t to int (pro.h)
	# C4244 : IDA SDK => conversion size_t to int (idp.hpp)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4267 /wd4244")

elseif("$ENV{IDATARGET}" STREQUAL "IDA32")
	target_compile_definitions(${PROJECT_NAME} PRIVATE 
		__NT__
		__IDP__
		__X64__
		USE_DANGEROUS_FUNCTIONS
		USE_STANDARD_FILE_FUNCTIONS
		__QT__
		QT_NAMESPACE=QT
		QT_DLL
		QT_GUI_LIB
		QT_CORE_LIB
		QT_THREAD_SUPPORT
	)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		"${IDASDK}/lib/x64_win_vc_32/ida.lib"
		"${IDASDK}/lib/x64_win_qt/Qt5Core.lib"
		"${IDASDK}/lib/x64_win_qt/Qt5Widgets.lib"
		"${IDASDK}/lib/x64_win_qt/Qt5Gui.lib"
	)
	# C4267 : IDA SDK => conversion size_t to int (pro.h)
	# C4244 : IDA SDK => conversion size_t to int (idp.hpp)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/wd4267 /wd4244")

endif()