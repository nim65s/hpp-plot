#ifndef HPP_PLOT_GRAPHWIDGET_HH
#define HPP_PLOT_GRAPHWIDGET_HH

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QGVScene.h>

namespace hpp {
  namespace plot {
    class GraphWidget : public QWidget
    {
      Q_OBJECT

    public:
      GraphWidget (QString name = QString (), QWidget *parent = NULL);

      void initializeGraph ();

      ~GraphWidget ();

    public slots:
      void updateGraph ();
      void updateEdges ();

    private slots:
      void nodeContextMenu(QGVNode* node);
      void nodeDoubleClick(QGVNode* node);
      void wheelEvent(QWheelEvent* event);

    protected:
      virtual void fillScene ();
      QGVScene* scene_;

    private:
      QWidget* buttonBox_;
      QGraphicsView* view_;
    };
  }
}

#endif // HPP_PLOT_GRAPHWIDGET_HH
