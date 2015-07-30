#ifndef HPP_PLOT_GRAPHWIDGET_HH
#define HPP_PLOT_GRAPHWIDGET_HH

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QComboBox>
#include <QLabel>

#include <QGVScene.h>

namespace hpp {
  namespace plot {
    class GraphView : public QGraphicsView
    {
    public:
      GraphView (QWidget* parent = NULL);

      // QWidget interface
    protected:
      void wheelEvent(QWheelEvent *);
    };

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

    protected:
      virtual void fillScene ();
      QGVScene* scene_;
      QWidget* buttonBox_;
      QLabel* elmtInfo_;
      QLabel* loggingInfo_;

    private:
      GraphView* view_;
      QComboBox* algList_;
      bool layoutShouldBeFreed_;
    };
  }
}

#endif // HPP_PLOT_GRAPHWIDGET_HH
