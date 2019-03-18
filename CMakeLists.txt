cmake_minimum_required (VERSION 3.0.2)
project(IDATag)

include(common.cmake)
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

add_target(${PROJECT_NAME} ${PROJECT_NAME} "${root_dir}/src" OPTIONS shared warnings)
set_target_output_directory(${PROJECT_NAME} "")
target_include_directories(${PROJECT_NAME} PRIVATE ";${IDASDK}/include")
target_include_directories(${PROJECT_NAME} PRIVATE ";${QTHEADER}")

if("$ENV{IDATARGET}" STREQUAL "IDA64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
		__NT__
		__IDP__
		__X64__
		__EA64__
		USE_DANGEROUS_FUNCTIONS
		__QT__
		__UI__
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

elseif("$ENV{IDATARGET}" STREQUAL "IDA32")
	target_compile_definitions(${PROJECT_NAME} PRIVATE 
		__NT__
		__IDP__
		__X64__
		USE_DANGEROUS_FUNCTIONS
		__QT__
		__UI__
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
endif()