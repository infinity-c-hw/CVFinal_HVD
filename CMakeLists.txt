cmake_minimum_required(VERSION 2.6)
project(cvfinal_hd)

# configure library path
if(WIN32)
	# point to your wxWidgets root directory
	set(wxWidgets_ROOT_DIR "C:\DevUtils\Libs\wxWidgets")

	# FIXME: untested
	#set(OpenCV_DIR "C:\DevUtils\Libs\opencv")
endif(WIN32)

# configure wxWidgets
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

# configure OpenCV
find_package(OpenCV REQUIRED)

# specify output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# compiler flags setup
set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "-Wall")

# configure include paths
include_directories(lib/icv)
include_directories(lib/icutils)
include_directories(gui)
include_directories(src/hvdgui)

# compile libraries
add_subdirectory(lib/icv)
add_subdirectory(lib/icutils)

# compile main binary
unset(src)
set(src ${src} gui/cvpanel/cvpanel.cpp)
set(src ${src} gui/hvd.cpp)
set(src ${src} src/hvdgui/hvdframe.cpp)
set(src ${src} src/hvdgui/hvdlogger.cpp)
set(src ${src} src/hvdgui/hvdapp.cpp)
set(src ${src} src/hvdmain/hvdmain.cpp)
set(src ${src} src/hvdmain/vidprocessor.cpp)
set(src ${src} src/hvdmain/vidplayer.cpp)

add_executable(hvd ${src})
target_link_libraries(hvd ${wxWidgets_LIBRARIES} ${OpenCV_LIBS} icv icutils)

# setup icons
list(APPEND icon_set "ico_tool_camera.png")
list(APPEND icon_set "ico_tool_image.png")
list(APPEND icon_set "ico_tool_video.png")
list(APPEND icon_set "ico_log_allowed.png")
list(APPEND icon_set "ico_log_false.png")
list(APPEND icon_set "ico_log_pending.png")

set(ICON_OUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(ICON_SRC "${CMAKE_SOURCE_DIR}/gui/resources")

if(UNIX)
	set(CP cp)
elseif(WIN32)
	set(CP copy)
endif(UNIX)

foreach(icon IN LISTS icon_set)
	add_custom_command(
		OUTPUT ${ICON_OUT}/${icon}
		COMMAND ${CP} ${ICON_SRC}/${icon} ${ICON_OUT}/${icon}
		DEPENDS ${ICON_SRC}/${icon}
	)
	add_custom_target(
		${icon} ALL
		DEPENDS ${ICON_OUT}/${icon}
	)
endforeach(icon)
