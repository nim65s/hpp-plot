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

#include <QApplication>
#include <QMainWindow>

#include "hpp/plot/qcustomplot.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QMainWindow window;
  
  // setup customPlot as central widget of window:
  QCustomPlot customPlot;
  window.setCentralWidget(&customPlot);
  
  // create plot (from quadratic plot example):
  QVector<double> x(101), y(101);
  for (int i=0; i<101; ++i)
  {
    x[i] = i/50.0 - 1;
    y[i] = x[i]*x[i];
  }
  customPlot.addGraph();
  customPlot.graph(0)->setData(x, y);
  customPlot.xAxis->setLabel("x");
  customPlot.yAxis->setLabel("y");
  customPlot.rescaleAxes();
  
  window.setGeometry(100, 100, 500, 400);
  window.show();
  return a.exec();
}

