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

SET (${PROJECT_NAME}_SOURCES_TEST
  main.cc
  )

ADD_EXECUTABLE(${PROJECT_NAME}-test
  ${${PROJECT_NAME}_SOURCES_TEST})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}-test ${QT_LIBRARIES} ${PROJECT_NAME})

SET (${PROJECT_NAME}_SOURCES_HIST
  hpp-plot-histogram.cc
  )

ADD_EXECUTABLE(${PROJECT_NAME}-histogram
  ${${PROJECT_NAME}_SOURCES_HIST})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}-histogram ${QT_LIBRARIES} ${PROJECT_NAME})

INSTALL(TARGETS ${PROJECT_NAME}-histogram DESTINATION bin)
