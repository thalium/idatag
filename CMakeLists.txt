cmake_minimum_required (VERSION 3.10.0)
project(IDATag)

if("$ENV{IDASDK}" STREQUAL "")
    message(FATAL_ERROR "missing IDASDK environment variable")
endif()
get_filename_component(IDASDK "$ENV{IDASDK}" ABSOLUTE)
message("-- Using IDASDK=${IDASDK}")

if("$ENV{IDATARGET}" STREQUAL "")
    message(FATAL_ERROR "missing IDATARGET environment variable")
endif()
message("-- Using IDATARGET=$ENV{IDATARGET}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if("$ENV{IDATARGET}" STREQUAL "IDA64WIN" OR "$ENV{IDATARGET}" STREQUAL "IDA32WIN")
	set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.6.3\\5.6.3\\msvc2015_64")
elseif("$ENV{IDATARGET}" STREQUAL "IDA64NUX" OR "$ENV{IDATARGET}" STREQUAL "IDA32NUX")
	set(CMAKE_PREFIX_PATH "/opt/Qt/5.6.3/gcc_64")
	set(CMAKE_C_COMPILER /usr/bin/gcc-8)
	set(CMAKE_CXX_COMPILER /usr/bin/g++-8)
endif()

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

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
target_include_directories(${PROJECT_NAME} PRIVATE ";${CMAKE_PREFIX_PATH}/include")

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)

if("$ENV{IDATARGET}" STREQUAL "IDA64WIN")
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

elseif("$ENV{IDATARGET}" STREQUAL "IDA32WIN")
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


elseif("$ENV{IDATARGET}" STREQUAL "IDA32NUX")
	target_compile_definitions(${PROJECT_NAME} PRIVATE 
		__LINUX__
		__IDP__
		__X64__
		USE_DANGEROUS_FUNCTIONS
		USE_STANDARD_FILE_FUNCTIONS
		__QT__
		QT_NAMESPACE=QT
	)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		"${IDASDK}/lib/x64_linux_gcc_32/libida.so"
		"${IDASDK}/../libQt5Core.so.5"
		"${IDASDK}/../libQt5Widgets.so.5"
		"${IDASDK}/../libQt5Gui.so.5"
	)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		stdc++fs
	)

elseif("$ENV{IDATARGET}" STREQUAL "IDA64NUX")
	target_compile_definitions(${PROJECT_NAME} PRIVATE 
		__LINUX__
		__IDP__
		__X64__
		__EA64__
		USE_DANGEROUS_FUNCTIONS
		USE_STANDARD_FILE_FUNCTIONS
		__QT__
		QT_NAMESPACE=QT
	)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		"${IDASDK}/lib/x64_linux_gcc_64/libida64.so"
		"${IDASDK}/../libQt5Core.so.5"
		"${IDASDK}/../libQt5Widgets.so.5"
		"${IDASDK}/../libQt5Gui.so.5"
	)
	set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "64.so")
	target_link_libraries(${PROJECT_NAME} PRIVATE
		stdc++fs
	)

endif()