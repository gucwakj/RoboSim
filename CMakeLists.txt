#build RoboSim GUI
project (RoboSim)

# set minimum cmake version
cmake_minimum_required (VERSION 2.8)

# verbose output
set (CMAKE_VERBOSE_MAKEFILE off)

# set the build postfix extension
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set (CMAKE_DEBUG_POSTFIX "d")
endif ()

# suppress developer warnings
cmake_policy (SET CMP0015 NEW)

# set developer warnings
if (WIN32)
	add_definitions (-D_CRT_SECURE_NO_WARNINGS)
else ()
	set (CMAKE_CXX_FLAGS "-Wall")
endif ()

# include directory
include_directories (
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_BINARY_DIR}"
)

# set output directory for executables
set (OUTPUT_BINDIR "${PROJECT_BINARY_DIR}/bin")
foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
	string (TOUPPER "${CONF}" CONF)
	set ("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${OUTPUT_BINDIR}")
endforeach ()
make_directory (${OUTPUT_BINDIR})

# add Qt
cmake_policy (SET CMP0020 NEW)
set (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/Qt/5.4.1/5.4/msvc2013_64_opengl/lib/cmake")
find_package (Qt5Core REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5OpenGL REQUIRED)
find_package (Qt5Widgets REQUIRED)
set (QT_USE_QTOPENGL TRUE)
set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
add_definitions (${QT_DEFINITIONS})
if (WIN32)
	link_directories ("C:/Qt/5.4.1/5.4/msvc2013_64_opengl/lib")
	set (EXTRA_LIBS ${EXTRA_LIBS} optimized Qt5::Core debug Qt5::Cored)
	set (EXTRA_LIBS ${EXTRA_LIBS} optimized Qt5::Gui debug Qt5::Guid)
	set (EXTRA_LIBS ${EXTRA_LIBS} optimized Qt5::OpenGL debug Qt5::OpenGLd)
	set (EXTRA_LIBS ${EXTRA_LIBS} optimized Qt5::Widgets debug Qt5::Widgetsd)
else ()
	set (EXTRA_LIBS ${EXTRA_LIBS} QtCore)
	set (EXTRA_LIBS ${EXTRA_LIBS} QtGui)
	set (EXTRA_LIBS ${EXTRA_LIBS} QtOpenGL)
	set (EXTRA_LIBS ${EXTRA_LIBS} QtWidgets)
endif ()

# add OpenSceneGraph headers
if (WIN32)
	#include_directories ("C:/Users/kgucwa/projects/librs/deps/osg/build/include")
	#include_directories ("C:/Users/kgucwa/projects/librs/deps/osg/include")
	include_directories ("C:/Users/kgucwa/Downloads/OpenSceneGraph-3.2.1/include")
	include_directories ("C:/Users/kgucwa/Downloads/OpenSceneGraph-3.2.1/build/include")
	link_directories ("C:/Users/kgucwa/Downloads/OpenSceneGraph-3.2.1/build/lib")
else ()
	include_directories ("/home/kgucwa/projects/librs/deps/osg/build/include")
	include_directories ("/home/kgucwa/projects/librs/deps/osg/include")
	link_directories ("/home/kgucwa/projects/librs/deps/osg/build/lib")
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
	src/obstacleeditor.cpp
	src/obstaclemodel.cpp
	src/obstacleview.cpp
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
	include/obstacleeditor.h
	include/obstaclemodel.h
	include/obstacleview.h
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
set_target_properties (robosimwidget PROPERTIES COMPILE_DEFINITIONS "ROBOSIMWIDGET_DLL")

#
# demo app for project
#

# add project library into standalone app
include_directories ("${RoboSim_SOURCE_DIR}/include")
link_directories ("${RoboSim_SOURCE_DIR}/build/Release")
set (EXTRA_LIBS ${EXTRA_LIBS} robosimwidget)

# set gui form
qt5_wrap_ui (APP_FORMS_HEADERS forms/mainwindow.ui)

# create standalone app
add_executable (RoboSim
    src/main.cpp
    src/mainwindow.cpp
    include/mainwindow.h
    ${APP_FORMS_HEADERS}
)
set_target_properties (RoboSim PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries (RoboSim ${EXTRA_LIBS})

