#
# Copyright (c) 2015 CNRS
# Authors: Joseph Mirabel
#
#
# This file is part of hpp-plot
# hpp-gui is free software: you can redistribute it
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

IF(USE_QT4)
  SET(QT "QT4")
ELSE(USE_QT4)
  SET(QT "")
ENDIF(USE_QT4)

GEPETTO_GUI_PLUGIN(hppmanipulationplugin
  ${QT}

  HEADERS
  hppmanipulationplugin.hh
  hppserverprocess.hh

  SOURCES
  hppmanipulationplugin.cc
  hppserverprocess.cc

  PKG_CONFIG_DEPENDENCIES
  "hpp-corbaserver"
  "hpp-manipulation-corba"
  )
