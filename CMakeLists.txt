# build RoboSim GUI
project (LIBROBOSIMGUI)

# set minimum cmake version
cmake_minimum_required (VERSION 2.8)

# verbose ouptut
set (CMAKE_VERBOSE_MAKEFILE off)

# set build type
set (CMAKE_BUILD_TYPE "Release")

# set the build postfix extension
if (CMAKE_BUILD_TYPE MATCHES "Release")
    set (CMAKE_BUILD_POSTFIX "")
elseif (CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
    set (CMAKE_BUILD_POSTFIX "s")
elseif (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    set (CMAKE_BUILD_POSTFIX "rd")
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
    set (CMAKE_BUILD_POSTFIX "d")
else()
    set (CMAKE_BUILD_POSTFIX "")
endif()

# supress developer warnings
cmake_policy (SET CMP0015 NEW)

# set developer warnings
if (MSVC)
	add_definitions (-D_CRT_SECURE_NO_WARNINGS)
else (MSVC)
	set (CMAKE_CXX_FLAGS "-Wno-write-strings")
endif (MSVC)

# include directory
include_directories (
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_BINARY_DIR}"
)

# set output directory for executables
set (OUTPUT_BINDIR "${PROJECT_BINARY_DIR}/bin")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BINDIR})
make_directory (${OUTPUT_BINDIR})

# add Qt
find_package (Qt4 COMPONENTS QtCore QtGui QtOpenGL QtXml REQUIRED)
set(QT_USE_QTOPENGL TRUE)
include (${QT_USE_FILE})
add_definitions (${QT_DEFINITIONS})
set (EXTRA_LIBS ${EXTRA_LIBS} QtCore)
set (EXTRA_LIBS ${EXTRA_LIBS} QtGui)
set (EXTRA_LIBS ${EXTRA_LIBS} QtOpenGL)
set (EXTRA_LIBS ${EXTRA_LIBS} QtXml)

# add OpenSceneGraph library
include_directories ("/home/kgucwa/projects/librs/deps/osg/build/include")
include_directories ("/home/kgucwa/projects/librs/deps/osg/include")
link_directories ("/home/kgucwa/projects/librs/deps/osg/build/lib")
set (EXTRA_LIBS ${EXTRA_LIBS} optimized OpenThreads debug OpenThreadsd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osg debug osgd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgDB debug osgDBd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgFX debug osgFXd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgGA debug osgGAd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgQt debug osgQtd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgShadow debug osgShadowd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgText debug osgTextd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgUtil debug osgUtild)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized osgViewer debug osgViewerd)

# add openGL library
find_package (OpenGL REQUIRED)
if (OPENGL_FOUND)
	include_directories (${OPENGL_INCLUDE_DIR})
	set (EXTRA_LIBS ${EXTRA_LIBS} ${OPENGL_LIBRARIES})
endif (OPENGL_FOUND)

# add librs modules
include_directories ("/home/kgucwa/projects/librs/include")
link_directories ("/home/kgucwa/projects/librs/build/lib")
set (EXTRA_LIBS ${EXTRA_LIBS} optimized rsRobots debug rsRobotsd)
set (EXTRA_LIBS ${EXTRA_LIBS} optimized rsScene debug rsScened)

# add source files
set (SRCS ${SRCS}
	src/main.cpp
	src/mainwindow.cpp
	src/xmlreader.cpp
	src/xmldom.cpp
	src/robotmodel.cpp
	src/robotview.cpp
	src/qosgwidget.cpp
	src/roboteditor.cpp
)

# add headers
set (HEADERS
	include/mainwindow.h
	include/xmlreader.h
	include/xmldom.h
	include/robotmodel.h
	include/robotview.h
	include/qosgwidget.h
	include/roboteditor.h
)
qt4_wrap_cpp (HEADERS_MOC ${HEADERS})

# set gui form
set (FORMS forms/mainwindow.ui)
qt4_wrap_ui (FORMS_HEADERS ${FORMS})

# add executable
add_executable (RoboSim ${SRCS} ${HEADERS_MOC} ${FORMS_HEADERS})

# link against dependencies
target_link_libraries (RoboSim ${EXTRA_LIBS})

