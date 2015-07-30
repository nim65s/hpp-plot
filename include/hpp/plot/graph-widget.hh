#ifndef HPP_PLOT_GRAPHWIDGET_HH
#define HPP_PLOT_GRAPHWIDGET_HH

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>

#include <QGVScene.h>

namespace hpp {
  namespace plot {
    class GraphWidget : public QWidget
    {
      Q_OBJECT

    public:
      GraphWidget (QString name = QString (), QWidget *parent = NULL);

      ~GraphWidget ();

    public slots:
      void updateGraph ();
      void updateEdges ();

    protected slots:
      virtual void nodeContextMenu(QGVNode* node);
      virtual void nodeDoubleClick(QGVNode* node);
      virtual void edgeContextMenu(QGVEdge* edge);
      virtual void edgeDoubleClick(QGVEdge* edge);
      virtual void wheelEvent(QWheelEvent* event);

    protected:
      virtual void fillScene ();
      QGVScene* scene_;
      QWidget* buttonBox_;
      QLabel* elmtInfo_;
      QLabel* loggingInfo_;

    private:
      QGraphicsView* view_;
      bool layoutShouldBeFreed_;
    };
  }
}

#endif // HPP_PLOT_GRAPHWIDGET_HH
