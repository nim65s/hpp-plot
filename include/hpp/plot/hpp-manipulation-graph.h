#ifndef HPP_PLOT_HPP_MANIPULATION_GRAPH_H
#define HPP_PLOT_HPP_MANIPULATION_GRAPH_H

#include <QAction>

#include <hpp/corbaserver/manipulation/client.hh>

#include <hpp/plot/graph-widget.hh>

namespace hpp {
  namespace corbaServer {
    namespace manipulation {
      class Client;
    }
  }
  namespace plot {
    class GraphAction : public QAction
    {
      Q_OBJECT

    public:
      GraphAction (QWidget* parent = NULL);
      hpp::ID id_;

    signals:
      void activated (hpp::ID id);

    private slots:
      void transferSignal ();
    };

    class HppManipulationGraphWidget : public GraphWidget
    {
      Q_OBJECT

    public:
      static const int IdRole;
      static const int SuccessRateRole;

      HppManipulationGraphWidget (corbaServer::manipulation::Client* hpp_, QWidget* parent);

      ~HppManipulationGraphWidget ();

      void addNodeContextMenuAction (GraphAction* action);
      void addEdgeContextMenuAction (GraphAction* action);

    protected:
      void fillScene ();

    public slots:
      void updateStatistics ();

    protected slots:
      virtual void nodeContextMenu(QGVNode* node);
      virtual void nodeDoubleClick(QGVNode* node);
      virtual void edgeContextMenu(QGVEdge* edge);
      virtual void edgeDoubleClick(QGVNode* edge);

      void selectionChanged ();

    private:
      corbaServer::manipulation::Client* manip_;

      QList <GraphAction*> nodeContextMenuActions_;
      QList <GraphAction*> edgeContextMenuActions_;
    };
  }
}

#endif // HPP_PLOT_HPP_MANIPULATION_GRAPH_H
