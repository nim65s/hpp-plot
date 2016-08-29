# Copyright (c) 2015 CNRS
# Authors: Joseph Mirabel
#
#
# This file is part of hpp-plot
# hpp-plot is free software: you can redistribute it
# and/or modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation, either version
# 3 of the License, or (at your option) any later version.
#
# hpp-plot is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Lesser Public License for more details.  You should have
# received a copy of the GNU Lesser General Public License along with
# hpp-plot  If not, see
# <http://www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CXX_DISABLE_WERROR TRUE)

INCLUDE(cmake/base.cmake)

SET(PROJECT_NAME "hpp-plot")
SET(PROJECT_URL "https://github.com/jmirabel/hpp-plot")
SET(PROJECT_DESCRIPTION "Plotting tools for HPP")

SETUP_PROJECT()

OPTION(HPP_MANIPULATION_CORBA_CLIENT_ONLY "Disable functionnalities requiring the corba servers of HPP" OFF)

FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui)
ADD_OPTIONAL_DEPENDENCY("gepetto-viewer-corba")
ADD_REQUIRED_DEPENDENCY("hpp-corbaserver")
ADD_REQUIRED_DEPENDENCY("hpp-manipulation-corba")
ADD_REQUIRED_DEPENDENCY("hpp-wholebody-step-corba")
ADD_REQUIRED_DEPENDENCY("QGVCore")


# Activate hpp-util logging if requested
SET (HPP_DEBUG FALSE CACHE BOOL "trigger hpp-util debug output")
IF (HPP_DEBUG)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHPP_DEBUG")
ENDIF()

SET (${PROJECT_NAME}_HEADERS
  include/hpp/plot/qcustomplot.h
  include/hpp/plot/histogram-widget.hh
  include/hpp/plot/graph-widget.hh
  include/hpp/plot/hpp-manipulation-graph.hh
  )
SET (${PROJECT_NAME}_HEADERS_NOMOC
)

SET(${PROJECT_NAME}_FORMS
  )

SET(${PROJECT_NAME}_RESOURCES
  )

QT4_WRAP_CPP(${PROJECT_NAME}_HEADERS_MOC ${${PROJECT_NAME}_HEADERS})
QT4_WRAP_UI(${PROJECT_NAME}_FORMS_HEADERS ${${PROJECT_NAME}_FORMS})
QT4_ADD_RESOURCES(${PROJECT_NAME}_RESOURCES_RCC ${${PROJECT_NAME}_RESOURCES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET (${PROJECT_NAME}_SOURCES
  src/qcustomplot.cpp
  src/histogram-widget.cc
  src/graph-widget.cc
  src/hpp-manipulation-graph.cc
  )

ADD_LIBRARY (${PROJECT_NAME}
  SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_HEADERS_NOMOC}
  ${${PROJECT_NAME}_HEADERS_MOC}
  ${${PROJECT_NAME}_FORMS_HEADERS}
  ${${PROJECT_NAME}_RESOURCES_RCC}
  )

PKG_CONFIG_USE_DEPENDENCY(${PROJECT_NAME} hpp-corbaserver)
PKG_CONFIG_USE_DEPENDENCY(${PROJECT_NAME} hpp-manipulation-corba)
PKG_CONFIG_USE_DEPENDENCY(${PROJECT_NAME} QGVCore)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib)

ADD_SUBDIRECTORY (bin)
ADD_SUBDIRECTORY (hpp-gui-plugins)

SETUP_PROJECT_FINALIZE()
