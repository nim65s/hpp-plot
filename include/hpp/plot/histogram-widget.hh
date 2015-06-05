#ifndef HPP_PLOT_HISTOGRAMWIDGET_HH
#define HPP_PLOT_HISTOGRAMWIDGET_HH

#include <QWidget>
#include <QVBoxLayout>

#include <hpp/plot/qcustomplot.h>

#include <hpp/corbaserver/manipulation/client.hh>

namespace hpp {
  namespace plot {
    class HistogramWidget : public QWidget
    {
      Q_OBJECT

      public:
        HistogramWidget (QWidget* parent = NULL);

        void init ();

        void connectCorba (const char* iiop = "corbaloc:rir:/NameService");

      public slots:
        void recomputeSources ();
        void recomputeData ();
        void zoomAuto ();

      private:
        void addPlottable (QString name);


        QWidget* buttonBox_;
        QCustomPlot* plot_;

        QList <QCPCurve*> curves_;
        QList <hpp::GraphComp> sources_;

        corbaServer::manipulation::Client client_;

        static QVector <QPen> colors_;
    };
  } // namespace plot
} // namespace hpp

#endif // HPP_PLOT_HISTOGRAMWIDGET_HH
