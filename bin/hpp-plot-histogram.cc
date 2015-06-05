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
#include <QTimer>

#include "hpp/plot/histogram-widget.hh"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QMainWindow window;
  
  // setup customPlot as central widget of window:
  hpp::plot::HistogramWidget widget;
  window.setCentralWidget(&widget);

  widget.init();
  widget.connectCorba();

  QTimer dataTimer;
  QObject::connect(&dataTimer, SIGNAL(timeout()), &widget, SLOT(recomputeData()));
  dataTimer.start (100);
  
  window.setGeometry(100, 100, 500, 400);
  window.show();
  return a.exec();
}

