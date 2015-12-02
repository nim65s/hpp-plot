#include "hpp/plot/hpp-manipulation-graph.hh"

#include <QGVNode.h>
#include <QGVEdge.h>
#include <QMessageBox>
#include <QMap>
#include <QTemporaryFile>
#include <QDir>
#include <QMenu>
#include <QPushButton>
#include <QTimer>
#include <QLayout>
#include <QDebug>

namespace hpp {
  namespace plot {
    namespace {
      void initConfigProjStat (::hpp::ConfigProjStat& p) {
        p.success = 0;
        p.error = 0;
        p.nbObs = 0;
      }
    }
    GraphAction::GraphAction(HppManipulationGraphWidget *parent) :
      QAction (parent),
      gw_ (parent)
    {
      connect (this, SIGNAL (triggered()), SLOT(transferSignal()));
    }

    void GraphAction::transferSignal()
    {
      hpp::ID id;
      if (gw_->selectionID(id))
        emit activated (id);
    }

    HppManipulationGraphWidget::HppManipulationGraphWidget (corbaServer::manipulation::Client* hpp_, QWidget *parent)
      : GraphWidget ("Manipulation graph", parent),
        manip_ (hpp_),
        updateStatsTimer_ (new QTimer (this)),
        currentId_ (-1)
    {
      QPushButton* stats = new QPushButton (
            QIcon::fromTheme("view-refresh"), "&Statistics", buttonBox_);
      stats->setCheckable(true);
      buttonBox_->layout()->addWidget(stats);
      updateStatsTimer_->setInterval(1000);
      updateStatsTimer_->setSingleShot(false);

      connect (updateStatsTimer_, SIGNAL (timeout()), SLOT (updateStatistics()));
      connect (stats, SIGNAL (clicked(bool)), SLOT (startStopUpdateStats(bool)));
      connect (scene_, SIGNAL (selectionChanged()), SLOT(selectionChanged()));
    }

    HppManipulationGraphWidget::~HppManipulationGraphWidget()
    {
      qDeleteAll (nodeContextMenuActions_);
      qDeleteAll (edgeContextMenuActions_);
      delete updateStatsTimer_;
    }

    void HppManipulationGraphWidget::addNodeContextMenuAction(GraphAction *action)
    {
      nodeContextMenuActions_.append (action);
    }

    void HppManipulationGraphWidget::addEdgeContextMenuAction(GraphAction *action)
    {
      edgeContextMenuActions_.append (action);
    }

    void HppManipulationGraphWidget::client (corbaServer::manipulation::Client* hpp)
    {
      manip_ = hpp;
    }

    bool hpp::plot::HppManipulationGraphWidget::selectionID(ID &id)
    {
      id = currentId_;
      return currentId_ != -1;
    }

    void HppManipulationGraphWidget::fillScene()
    {
      /*
      QTemporaryFile dotFile (QDir::tempPath () + QString ("/constraintGraph_XXXXXX.dot"));
      dotFile.setAutoRemove(false);
      qDebug() << dotFile.fileName();
      if (dotFile.open()) {
          manip_.graph()->display(dotFile.fileName().toLocal8Bit().constData());
          QByteArray content = dotFile.readAll();
          scene_->loadLayout(content.constData());
          qDebug() << dotFile.fileName();
        }
      //*/
//       /*
      hpp::GraphComp_var graph;
      hpp::GraphElements_var elmts;
      try {
        manip_->graph()->getGraph(graph.out(), elmts.out());

        scene_->setGraphAttribute("label", QString (graph->name));

        //      scene_->setGraphAttribute("splines", "ortho");
        scene_->setGraphAttribute("rankdir", "LR");
        //scene_->setGraphAttribute("concentrate", "true"); //Error !
        scene_->setGraphAttribute("nodesep", "0.4");

        scene_->setNodeAttribute("shape", "circle");
        scene_->setNodeAttribute("style", "filled");
        scene_->setNodeAttribute("fillcolor", "white");
        scene_->setNodeAttribute("height", "1.2");
        scene_->setEdgeAttribute("minlen", "3");
        //scene_->setEdgeAttribute("dir", "both");


        // Add the nodes
        QMap < ::CORBA::Long, QGVNode*> nodes;
        for (std::size_t i = 0; i < elmts->nodes.length(); ++i) {
          QGVNode* n = scene_->addNode (QString (elmts->nodes[i].name));
          NodeInfo ni;
          ni.id = elmts->nodes[i].id;
          nodeInfos_[n] = ni;
          n->setFlag (QGraphicsItem::ItemIsMovable, true);
          n->setFlag (QGraphicsItem::ItemSendsGeometryChanges, true);
          nodes[elmts->nodes[i].id] = n;
        }
        for (std::size_t i = 0; i < elmts->edges.length(); ++i) {
          QGVEdge* e = scene_->addEdge (nodes[elmts->edges[i].start],
              nodes[elmts->edges[i].end],
              QString (elmts->edges[i].name));
          EdgeInfo ei;
          ei.id = elmts->edges[i].id;
          edgeInfos_[e] = ei;
        }
      } catch (const hpp::Error& e) {
        qDebug () << e.msg;
      }
      //*/
    }

    void HppManipulationGraphWidget::updateStatistics()
    {
      foreach (QGraphicsItem* elmt, scene_->items()) {
          QGVNode* node = dynamic_cast <QGVNode*> (elmt);
          if (node) {
              NodeInfo& ni = nodeInfos_[node];
              manip_->graph()->getConfigProjectorStats
                (ni.id, ni.configStat.out(), ni.pathStat.out());
              float sr = (ni.configStat->nbObs > 0)
                ? (float)ni.configStat->success/(float)ni.configStat->nbObs
                : 0.f / 0.f;
              QString colorcode = (ni.configStat->nbObs > 0)
                ? QColor (255,(int)(sr*255),(int)(sr*255)).name()
                : "white";
              const QString& fillcolor = node->getAttribute("fillcolor");
              if (!(fillcolor == colorcode)) {
                  node->setAttribute("fillcolor", colorcode);
                  node->updateLayout();
                }
              continue;
            }
          QGVEdge* edge = dynamic_cast <QGVEdge*> (elmt);
          if (edge) {
              EdgeInfo& ei = edgeInfos_[edge];
              manip_->graph()->getConfigProjectorStats
                (ei.id, ei.configStat.out(), ei.pathStat.out());
              float sr = (ei.configStat->nbObs > 0)
                ? (float)ei.configStat->success/(float)ei.configStat->nbObs
                : 0.f / 0.f;
              QString colorcode = (ei.configStat->nbObs > 0)
                ? QColor (255 - (int)(sr*255),0,0).name()
                : "";
              const QString& color = edge->getAttribute("color");
              if (!(color == colorcode)) {
                  edge->setAttribute("color", colorcode);
                  edge->updateLayout();
                }
              continue;
            }
        }
      scene_->update();
    }

    void HppManipulationGraphWidget::nodeContextMenu(QGVNode *node)
    {
      const NodeInfo& ni = nodeInfos_[node];
      hpp::ID id = currentId_;
      currentId_ = ni.id;

      QMenu cm ("Node context menu", this);
      foreach (GraphAction* action, nodeContextMenuActions_) {
          cm.addAction (action);
      }
      cm.exec(QCursor::pos());

      currentId_ = id;
    }

    void HppManipulationGraphWidget::nodeDoubleClick(QGVNode *node)
    {
      QMessageBox::information(this, tr("Node double clicked"), tr("Node %1").arg(node->label()));
    }

    void HppManipulationGraphWidget::edgeContextMenu(QGVEdge *edge)
    {
      const EdgeInfo& ei = edgeInfos_[edge];
      hpp::ID id = currentId_;
      currentId_ = ei.id;

      QMenu cm ("Edge context menu", this);
      foreach (GraphAction* action, edgeContextMenuActions_) {
          cm.addAction (action);
      }
      cm.exec(QCursor::pos());

      currentId_ = id;
    }

    void HppManipulationGraphWidget::edgeDoubleClick(QGVNode *edge)
    {
      QMessageBox::information(this, tr("Node double clicked"), tr("Node %1").arg(edge->label()));
    }

    void HppManipulationGraphWidget::selectionChanged()
    {
      QList <QGraphicsItem*> items = scene_->selectedItems();
      currentId_ = -1;
      if (items.size() == 0) {
          elmtInfo_->setText ("No info");
        }
      if (items.size() == 1) {
          QString type, name; int success, error, nbObs; ::hpp::ID id;
          QGVNode* node = dynamic_cast <QGVNode*> (items.first());
          QGVEdge* edge = dynamic_cast <QGVEdge*> (items.first());
          if (node) {
              type = "Node";
              name = node->label();
              const NodeInfo& ni = nodeInfos_[node];
              id = ni.id;
              currentId_ = id;
              success = ni.configStat->success;
              error = ni.configStat->error;
              nbObs = ni.configStat->nbObs;
            } else if (edge) {
              type = "Edge";
              name = edge->label();
              const EdgeInfo& ei = edgeInfos_[edge];
              id = ei.id;
              currentId_ = id;
              success = ei.configStat->success;
              error = ei.configStat->error;
              nbObs = ei.configStat->nbObs;
            } else {
              return;
            }
          elmtInfo_->setText (
            QString ("<h4>%1 %2</h4><ul>"
              "<li>Id: %3</li>"
              "<li>Success: %4</li>"
              "<li>Error: %5</li>"
              "<li>Nb observations: %6</li>"
              "</ul>")
            .arg (type).arg (name).arg(id)
            .arg(success).arg(error).arg(nbObs));
        }
    }

    void HppManipulationGraphWidget::startStopUpdateStats(bool start)
    {
      if (start) updateStatsTimer_->start ();
      else       updateStatsTimer_->stop ();
    }

    HppManipulationGraphWidget::NodeInfo::NodeInfo () : id (-1)
    {
      initConfigProjStat (configStat.out());
      initConfigProjStat (pathStat.out());
    }

    HppManipulationGraphWidget::EdgeInfo::EdgeInfo () : id (-1)
    {
      initConfigProjStat (configStat.out());
      initConfigProjStat (pathStat.out());
    }
  }
}
