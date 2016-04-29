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

#include "hpp/plot/histogram-widget.hh"

#include <QPushButton>

namespace hpp {
  namespace plot {
    QVector <QPen> HistogramWidget::colors_ =
        QVector <QPen> () << QPen (Qt::blue) << QPen (Qt::red);

    HistogramWidget::HistogramWidget (QWidget* parent)
      : QWidget (parent),
        buttonBox_ (new QWidget (this)),
        plot_ (new QCustomPlot (this)),
        client_ (0, NULL)
    {
      QVBoxLayout* vLayout = new QVBoxLayout (this);
      vLayout->setSpacing (0);
      vLayout->setContentsMargins (0, 0, 0, 0);
      vLayout->addWidget (buttonBox_);
      vLayout->addWidget (plot_);
      plot_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      this->setLayout(vLayout);

      QHBoxLayout* hLayout = new QHBoxLayout (buttonBox_);
      QPushButton* refresh = new QPushButton (
            QIcon::fromTheme("view-refresh"), "&Refresh", buttonBox_);
      QPushButton* zoomauto = new QPushButton (
            QIcon::fromTheme("zoom-fit-best"), "&Zoom auto", buttonBox_);
      buttonBox_->setLayout(hLayout);
      hLayout->setAlignment(buttonBox_, Qt::AlignRight);
      hLayout->addSpacerItem(new QSpacerItem (0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
      hLayout->addWidget(zoomauto);
      hLayout->addWidget(refresh);
      connect(zoomauto, SIGNAL (clicked ()), this, SLOT (zoomAuto()));
      connect(refresh, SIGNAL (clicked ()), this, SLOT (recomputeSources()));
    }

    void HistogramWidget::init ()
    {
      plot_->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom
                             |QCP::iSelectLegend
                             |QCP::iMultiSelect);
      plot_->axisRect()->setupFullAxesBox(true);
      plot_->xAxis->setLabel("x");
      plot_->yAxis->setLabel("y");

      plot_->legend->setVisible(true);
    }

    void HistogramWidget::addPlottable(QString name) {
      // set up the QCPColorMap:
      QCPCurve *curve = new QCPCurve(plot_->xAxis, plot_->yAxis);
      plot_->addPlottable(curve);
      curve->setName(name);
      curve->addToLegend();

      curve->setPen(colors_[curves_.size()%colors_.size()]);
      curve->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus));
      curve->setLineStyle(QCPCurve::lsNone);

      curves_.push_back (curve);
      connect (plot_->legend->itemWithPlottable(curve), SIGNAL (selectionChanged(bool)),
               curve, SLOT (setVisible(bool)));
      plot_->legend->itemWithPlottable(curve)->setSelected(true);
    }

    void HistogramWidget::connectCorba (const char* iiop)
    {
      client_.connect (iiop);
    }

    void HistogramWidget::recomputeSources()
    {
      sources_.clear();
      curves_.clear();
      plot_->clearPlottables();

      hpp::GraphComp_var graph;
      hpp::GraphElements_var components;
      client_.graph()->getGraph (graph.out(), components.out());
      for (std::size_t i = 0; i < components->edges.length(); ++i) {
          try {
            hpp::floatSeq_var freq;
            hpp::floatSeqSeq_var values;
            client_.graph()->getHistogramValue (components->edges[i].id,
                                                freq.out(), values.out());
            sources_.push_back(components->edges[i]);
            addPlottable(QString (components->edges[i].name));
          } catch (hpp::Error& err) {
            continue;
          }
        }
    }

    void HistogramWidget::recomputeData ()
    {
      QList <QCPCurve*>::iterator itMap = curves_.begin();
      foreach (hpp::GraphComp comp, sources_) {
          (*itMap)->clearData ();
          hpp::floatSeq_var freq;
          hpp::floatSeqSeq_var values;
          client_.graph()->getHistogramValue (comp.id, freq.out(), values.out());
          for (std::size_t i = 0; i < freq->length(); ++i) {
              (*itMap)->addData (values[i][0], values[i][1]);
            }

          itMap++;
        }
      plot_->replot();
    }

    void HistogramWidget::zoomAuto()
    {
      plot_->rescaleAxes(true);
    }
  } // namespace plot
} // namespace hpp
