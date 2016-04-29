#ifndef HPP_PLOT_HPP_MANIPULATION_GRAPH_HH
#define HPP_PLOT_HPP_MANIPULATION_GRAPH_HH

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
    class HppManipulationGraphWidget;

    class GraphAction : public QAction
    {
      Q_OBJECT

    public:
      GraphAction (HppManipulationGraphWidget* parent);

    signals:
      void activated (hpp::ID id);

    private slots:
      void transferSignal ();

    private:
      HppManipulationGraphWidget* gw_;
    };

    class HppManipulationGraphWidget : public GraphWidget
    {
      Q_OBJECT

    public:
      HppManipulationGraphWidget (corbaServer::manipulation::Client* hpp_, QWidget* parent);

      ~HppManipulationGraphWidget ();

      void addNodeContextMenuAction (GraphAction* action);
      void addEdgeContextMenuAction (GraphAction* action);

      void client (corbaServer::manipulation::Client* hpp);

      bool selectionID (hpp::ID& id);

    protected:
      void fillScene ();

    public slots:
      void updateStatistics ();

    protected slots:
      virtual void nodeContextMenu(QGVNode* node);
      virtual void nodeDoubleClick(QGVNode* node);
      virtual void edgeContextMenu(QGVEdge* edge);
      virtual void edgeDoubleClick(QGVEdge* edge);

      void selectionChanged ();

    private slots:
      void startStopUpdateStats (bool start);

    private:
      corbaServer::manipulation::Client* manip_;

      struct NodeInfo {
        ::hpp::ID id;
        QString constraintStr;

        ::hpp::ConfigProjStat_var configStat, pathStat;
        NodeInfo ();
      };
      struct EdgeInfo {
        ::hpp::ID id;
        QString name;
        ::CORBA::Long weight;
        QString constraintStr;
        QGVEdge* edge;

        ::hpp::ConfigProjStat_var configStat, pathStat;
        ::hpp::Names_t_var errors;
        ::hpp::intSeq_var freqs;

        EdgeInfo ();
      };

      void updateWeight (EdgeInfo& ei, bool get = true);
      void updateWeight (EdgeInfo& ei, const ::CORBA::Long w);

      QList <GraphAction*> nodeContextMenuActions_;
      QList <GraphAction*> edgeContextMenuActions_;
      QMap <QGVNode*, NodeInfo> nodeInfos_;
      QMap <QGVEdge*, EdgeInfo> edgeInfos_;

      QPushButton* *statButton_;
      QTimer* updateStatsTimer_;

      hpp::ID currentId_;
    };
  }
}

#endif // HPP_PLOT_HPP_MANIPULATION_GRAPH_HH
