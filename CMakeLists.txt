#build RoboSim GUI
project (RoboSim)

# set minimum cmake version
cmake_minimum_required (VERSION 2.8)

# verbose output
set (CMAKE_VERBOSE_MAKEFILE off)

# default to release build
set (CMAKE_BUILD_TYPE Release)

# set the build postfix extension
set (CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add build postfix type")
set (CMAKE_RELEASE_POSTFIX "" CACHE STRING "add build postfix type")
set (CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add build postfix type")
set (CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add build postfix type")
if (CMAKE_BUILD_TYPE MATCHES "Release")
	set (CMAKE_BUILD_POSTFIX "")
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
	set (CMAKE_BUILD_POSTFIX "d")
elseif (CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
	set (CMAKE_BUILD_POSTFIX "s")
elseif (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
	set (CMAKE_BUILD_POSTFIX "rd")
endif ()

# suppress developer warnings
cmake_policy (SET CMP0015 NEW)

# set developer warnings
if (WIN32)
	add_definitions (-D_CRT_SECURE_NO_WARNINGS)
else ()
	set (CMAKE_CXX_FLAGS "-fPIC -Wall")
endif ()

# include directory
include_directories (
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_BINARY_DIR}"
)

# set output directory for shared objects
set (OUTPUT_BINDIR "${RoboSim_BINARY_DIR}/bin")
foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
	string (TOUPPER "${CONF}" CONF)
	set ("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
	set ("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
	set ("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
endforeach ()
set ("CMAKE_RUNTIME_OUTPUT_DIRECTORY" "${OUTPUT_BINDIR}")
set ("CMAKE_LIBRARY_OUTPUT_DIRECTORY" "${OUTPUT_BINDIR}")
make_directory (${OUTPUT_BINDIR})

# add Qt
cmake_policy (SET CMP0020 NEW)
if ("$ENV{PROCESSOR_ARCHITEW6432}" STREQUAL "")
	set (QT_PREFIX "C:/Qt/5.6/msvc2013")
else ()
	set (QT_PREFIX "C:/Qt/5.5.1/5.5/msvc2013_64")
endif ()
if (WIN32)
	set (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${QT_PREFIX}/lib/cmake")
endif ()
find_package (Qt5Core REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5OpenGL REQUIRED)
find_package (Qt5Widgets REQUIRED)
set (QT_USE_QTOPENGL TRUE)
set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
add_definitions (${QT_DEFINITIONS})
include_directories (${Qt5Core_INCLUDE_DIRS})
include_directories (${Qt5Gui_INCLUDE_DIRS})
include_directories (${Qt5OpenGL_INCLUDE_DIRS})
include_directories (${Qt5Widgets_INCLUDE_DIRS})
if (WIN32)
	link_directories ("${QT_PREFIX}/lib")
	set (EXTRA_LIBS ${EXTRA_LIBS} optimized Qt5Core debug Qt5Cored)
	set (EXTRA_LIBS ${EXTRA_LIBS} optimized Qt5Gui debug Qt5Guid)
	set (EXTRA_LIBS ${EXTRA_LIBS} optimized Qt5OpenGL debug Qt5OpenGLd)
	set (EXTRA_LIBS ${EXTRA_LIBS} optimized Qt5Widgets debug Qt5Widgetsd)
else ()
	set (EXTRA_LIBS ${EXTRA_LIBS} Qt5::Core)
	set (EXTRA_LIBS ${EXTRA_LIBS} Qt5::Gui)
	set (EXTRA_LIBS ${EXTRA_LIBS} Qt5::OpenGL)
	set (EXTRA_LIBS ${EXTRA_LIBS} Qt5::Widgets)
endif ()

# add OpenSceneGraph headers
if (WIN32)
	include_directories ("C:/Users/kgucwa/projects/librs/deps/osg3.4.0/build/include")
	include_directories ("C:/Users/kgucwa/projects/librs/deps/osg3.4.0/include")
	link_directories ("C:/Users/kgucwa/projects/librs/deps/osg3.4.0/build/lib")
else ()
	include_directories ("/home/kgucwa/projects/librs/deps/osg3.4.0/build/include")
	include_directories ("/home/kgucwa/projects/librs/deps/osg3.4.0/include")
	link_directories ("/home/kgucwa/projects/librs/deps/osg3.4.0/build/lib")
endif()
set (EXTRA_LIBS ${EXTRA_LIBS} optimized OpenThreads debug OpenThreadsd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osg debug osgd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgFX debug osgFXd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgGA debug osgGAd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgQt debug osgQtd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgViewer debug osgViewerd)

# add tinyxml2 library
if (WIN32)
	include_directories ("C:/Users/kgucwa/projects/librs/deps/tinyxml2")
	link_directories ("C:/Users/kgucwa/projects/librs/deps/tinyxml2/build/lib")
else ()
	include_directories ("/home/kgucwa/projects/librs/deps/tinyxml2")
	link_directories ("/home/kgucwa/projects/librs/deps/tinyxml2/build/lib")
endif ()
set (EXTRA_LIBS ${EXTRA_LIBS} optimized tinyxml2 debug tinyxml2d)

# add libRS modules
if (WIN32)
	include_directories ("C:/Users/kgucwa/projects/librs/include")
	link_directories ("C:/Users/kgucwa/projects/librs/build/lib")
else ()
	include_directories ("/home/kgucwa/projects/librs/include")
	link_directories ("/home/kgucwa/projects/librs/build/lib")
endif ()
set (EXTRA_LIBS ${EXTRA_LIBS} optimized rs debug rsd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized rsRobots debug rsRobotsd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized rsScene debug rsScened)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized rsXML debug rsXMLd)

# add source files
set (SRCS ${SRCS}
	src/objecteditor.cpp
	src/objectmodel.cpp
	src/objectview.cpp
	src/platformselector.cpp
	src/robosimwidget.cpp
	src/roboteditor.cpp
	src/robotmodel.cpp
	src/robotview.cpp
	src/qosgwidget.cpp
	src/xmlparser.cpp
)

# add headers
set (HEADERS
	include/objecteditor.h
	include/objectmodel.h
	include/objectview.h
	include/platformselector.h
	include/robosimwidget.h
	include/roboteditor.h
	include/robotmodel.h
	include/robotview.h
	include/qosgwidget.h
	include/xmlparser.h
)

# set gui form
qt5_wrap_ui (WIDGET_FORMS_HEADERS forms/robosimwidget.ui)

# create library
add_library (robosimwidget SHARED ${SRCS} ${HEADERS} ${WIDGET_FORMS_HEADERS})
target_link_libraries (robosimwidget ${EXTRA_LIBS})
set_target_properties (robosimwidget PROPERTIES DEBUG_POSTFIX "d")
set_target_properties (robosimwidget PROPERTIES COMPILE_DEFINITIONS "ROBOSIMWIDGET_DLL")

#
# demo app for project
#

# add project library into standalone app
include_directories ("${RoboSim_SOURCE_DIR}/include")
link_directories ("${RoboSim_BINARY_DIR}/bin")
set (EXTRA_LIBS ${EXTRA_LIBS} optimized robosimwidget debug robosimwidgetd)

# set gui form
qt5_wrap_ui (APP_FORMS_HEADERS forms/mainwindow.ui)

# create standalone app
if (WIN32)
	set (RC_FILES robosim.rc)
endif ()
add_executable (RoboSim WIN32
	src/main.cpp
	src/mainwindow.cpp
	include/mainwindow.h
	${APP_FORMS_HEADERS}
	${RC_FILES}
)
set_target_properties (RoboSim PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries (RoboSim ${EXTRA_LIBS})
if (WIN32)
	target_link_libraries (RoboSim Qt5::WinMain)
endif ()

