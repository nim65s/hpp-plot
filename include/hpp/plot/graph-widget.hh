#ifndef HPP_PLOT_GRAPHWIDGET_HH
#define HPP_PLOT_GRAPHWIDGET_HH

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <hpp/plot/gvgraph.h>

namespace hpp {
  namespace plot {
    class GraphWidget;

    class GraphScene : public QGraphicsScene
    {
      Q_OBJECT

    public:
      GraphScene (GraphWidget* parent = NULL);

    protected:
      virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
      virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
      virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    private:
      GraphWidget* parent_;

      QGraphicsItem* dragging_;
      QPointF lastDraggingPos_;
    };

    class GraphWidget : public QWidget
    {
      Q_OBJECT

    public:
      GraphWidget (QString name = QString (), QWidget *parent = NULL);

      GVGraph graph_;

    public slots:
      void updateGraph ();

    private:
      QWidget* buttonBox_;
      GraphScene* scene_;
      QGraphicsView* view_;
    };
  }
}

#endif // HPP_PLOT_GRAPHWIDGET_HH

