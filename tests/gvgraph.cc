// Copyright (c) 2015, Joseph Mirabel
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of hpp-plot.
// hpp-plot is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// hpp-plot is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// hpp-plot. If not, see <http://www.gnu.org/licenses/>.

#include "hpp/plot/hpp-manipulation-graph.h"

#include <QDebug>
#include <QApplication>
#include <QMainWindow>

#include <hpp/corbaserver/manipulation/client.hh>

int test_1 (int argc, char** argv)
{
  using namespace hpp::plot;
  QApplication a(argc, argv);
  QMainWindow window;

  hpp::corbaServer::manipulation::Client client (0,NULL);
  client.connect();
  hpp::plot::HppManipulationGraphWidget w (&client, NULL);
  w.initializeGraph();
  w.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  window.setCentralWidget(&w);
  window.show();
  return a.exec();
}

int main (int argc, char** argv) {
//  return test_0();
  return test_1(argc, argv);
}
