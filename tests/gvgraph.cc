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

#include "hpp/plot/gvgraph.h"
#include "hpp/plot/graph-widget.hh"

#include <QDebug>
#include <QApplication>
#include <QMainWindow>

int test_0 () {
  using hpp::plot::GVGraph;
  GVGraph graph ("test");

  graph.addNode ("n1");
  graph.addNode ("n2");
  graph.addEdge ("n1", "n2");
  graph.setRootNode ("n1");

  graph.applyLayout ();
  qDebug() << graph.boundingRect ();
  foreach (hpp::plot::GVNode n, graph.nodes ()) {
      qDebug() << n.name << ", " << n.centerPos << ", " << n.height << ", " << n.width;
    }
  return EXIT_SUCCESS;
}

int test_1 (int argc, char** argv)
{
  using namespace hpp::plot;
  QApplication a(argc, argv);
  QMainWindow window;

  GraphWidget w ("test");
  w.graph_.addNode ("n1");
  w.graph_.addNode ("n2");
  w.graph_.addEdge ("n1", "n2");
  w.graph_.setRootNode ("n1");
  w.updateGraph();

  window.setCentralWidget(&w);
  window.show();
  return a.exec();
}

int main (int argc, char** argv) {
//  return test_0();
  return test_1(argc, argv);
}
