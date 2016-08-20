#include "hpp/plot/hpp-manipulation-graph.hh"

#include <limits>
#include <iostream>

#include <QGVNode.h>
#include <QGVEdge.h>
#include <QMessageBox>
#include <QMap>
#include <QTemporaryFile>
#include <QDir>
#include <QMenu>
#include <QPushButton>
#include <QInputDialog>
#include <QTimer>
#include <QLayout>
#include <QDebug>
#include <QtGui/qtextdocument.h>

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
        showWaypoints_ (new QPushButton (
              QIcon::fromTheme("view-refresh"), "&Show waypoints", buttonBox_)),
        statButton_ (new QPushButton (
              QIcon::fromTheme("view-refresh"), "&Statistics", buttonBox_)),
        updateStatsTimer_ (new QTimer (this)),
        currentId_ (-1),
        showNodeId_ (-1)
    {
      statButton_->setCheckable(true);
      showWaypoints_->setCheckable(true);
      showWaypoints_->setChecked(true);
      buttonBox_->layout()->addWidget(statButton_);
      buttonBox_->layout()->addWidget(showWaypoints_);
      updateStatsTimer_->setInterval(1000);
      updateStatsTimer_->setSingleShot(false);

      connect (updateStatsTimer_, SIGNAL (timeout()), SLOT (updateStatistics()));
      connect (statButton_, SIGNAL (clicked(bool)), SLOT (startStopUpdateStats(bool)));
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
      hpp::GraphComp_var graph = new hpp::GraphComp;
      hpp::GraphElements_var elmts = new hpp::GraphElements;
      try {
        manip_->graph()->getGraph(graph.out(), elmts.out());

        scene_->setGraphAttribute("label", QString (graph->name));

        scene_->setGraphAttribute("splines","spline");
        // scene_->setGraphAttribute("rankdir", "LR");
        scene_->setGraphAttribute("outputorder", "edgesfirst");
        scene_->setGraphAttribute("nodesep", "0.5");
        scene_->setGraphAttribute("esep","0.8");
        scene_->setGraphAttribute("sep","1");

        scene_->setNodeAttribute("shape", "circle");
        scene_->setNodeAttribute("style", "filled");
        scene_->setNodeAttribute("fillcolor", "white");
        // scene_->setNodeAttribute("height", "1.2");
        // scene_->setEdgeAttribute("minlen", "3");


        // Add the nodes
        nodes_.clear();
        bool hideW = !showWaypoints_->isChecked ();
        for (std::size_t i = 0; i < elmts->nodes.length(); ++i) {
	  if (elmts->nodes[i].id > graph->id) {
	    QGVNode* n = scene_->addNode (QString (elmts->nodes[i].name));
	    if (i == 0) scene_->setRootNode(n);
	    NodeInfo ni;
	    ni.id = elmts->nodes[i].id;
	    ni.node = n;
	    ni.constraintStr = getConstraints(ni.id);
	    ni.lockedStr = getLockedJoints(ni.id);
	    nodeInfos_[n] = ni;
	    n->setFlag (QGraphicsItem::ItemIsMovable, true);
	    n->setFlag (QGraphicsItem::ItemSendsGeometryChanges, true);
	    nodes_[elmts->nodes[i].id] = n;
	  }
        }
        for (std::size_t i = 0; i < elmts->edges.length(); ++i) {
	  if (elmts->edges[i].id > graph->id) {
	    EdgeInfo ei;
	    QGVEdge* e = scene_->addEdge (nodes_[elmts->edges[i].start],
					  nodes_[elmts->edges[i].end], "");
	    ei.name = QString::fromLocal8Bit(elmts->edges[i].name);
	    ei.id = elmts->edges[i].id;
            CORBA::String_var cnname = manip_->graph()->getContainingNode(ei.id);
            ei.containingNodeName = QString::fromLocal8Bit((char*)cnname);
	    ei.edge = e;
	    updateWeight (ei, true);
	    ei.constraintStr = getConstraints(ei.id);
	    ei.lockedStr = getLockedJoints(ei.id);
	    edgeInfos_[e] = ei;
	    if (ei.weight < 0) {
	      e->setAttribute("weight", "3");
	      if (elmts->edges[i].start >= elmts->edges[i].end)
		e->setAttribute("constraint", "false");
	      nodes_[elmts->edges[i].end]->setAttribute("shape", "hexagon");
	    }
	    if (hideW && ei.weight < 0) {
              e->setAttribute("weight", "0");
              e->setAttribute("style", "invisible");
              nodes_[elmts->edges[i].end]->setAttribute("style", "invisible");
            }
	  }
        }
      } catch (const hpp::Error& e) {
        qDebug () << e.msg;
      }
    }

    void HppManipulationGraphWidget::updateStatistics()
    {
      try {
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
            manip_->graph()->getEdgeStat
              (ei.id, ei.errors.out(), ei.freqs.out());
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
        selectionChanged ();
      } catch (const CORBA::Exception&) {
        updateStatsTimer_->stop();
        statButton_->setChecked(false);
        throw;
      }
    }

    void HppManipulationGraphWidget::showNodeOfConfiguration (const hpp::floatSeq& cfg)
    {
      if (showNodeId_ >= 0) {
        // Do unselect
        nodes_[showNodeId_]->setAttribute("fillcolor", "white");
        nodes_[showNodeId_]->updateLayout ();
      }
      try {
        manip_->graph()->getNode(cfg, showNodeId_);
        // Do select
        if (nodes_.contains(showNodeId_)) {
          nodes_[showNodeId_]->setAttribute("fillcolor", "green");
          nodes_[showNodeId_]->updateLayout ();
          scene_->update();
        } else {
          qDebug() << "Node" << showNodeId_ << "does not exist. Refer the graph may solve the issue.";
          showNodeId_ = -1;
        }
      } catch (const hpp::Error& e) {
        qDebug() << QString(e.msg);
      }
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

    void HppManipulationGraphWidget::edgeDoubleClick(QGVEdge *edge)
    {
      EdgeInfo& ei = edgeInfos_[edge];
      bool ok;
      ::CORBA::Long w = QInputDialog::getInt(
            this, "Update edge weight",
            tr("Edge %1 weight").arg(ei.name),
            ei.weight, 0, std::numeric_limits<int>::max(), 1,
            &ok);
      if (ok) {
          updateWeight(ei, w);
          edge->updateLayout();
        }
    }

    void HppManipulationGraphWidget::selectionChanged()
    {
      QList <QGraphicsItem*> items = scene_->selectedItems();
      currentId_ = -1;
      if (items.size() == 0) {
          elmtInfo_->setText ("No info");
        }
      QString weight;
      if (items.size() == 1) {
          QString type, name; ::hpp::ID id;
          QGVNode* node = dynamic_cast <QGVNode*> (items.first());
          QGVEdge* edge = dynamic_cast <QGVEdge*> (items.first());
          QString end;
	  QString constraints;
	  QString locked;
          if (node) {
              type = "Node";
              name = node->label();
              const NodeInfo& ni = nodeInfos_[node];
              id = ni.id;
              currentId_ = id;
	      constraints = ni.constraintStr;
	      locked = ni.lockedStr;
            } else if (edge) {
              type = "Edge";
              const EdgeInfo& ei = edgeInfos_[edge];
              name = ei.name;
              id = ei.id;
              currentId_ = id;
              weight = QString ("<li>Weight: %1</li>").arg(ei.weight);
              end = "<p>Extension results<ul>";
              for (std::size_t i = 0; i < std::min(ei.errors->length(),ei.freqs->length()); ++i) {
                end.append (
                    QString ("<li>%1: %2</li>")
                    .arg(QString(ei.errors.in()[i]))
                    .arg(        ei.freqs.in()[i]  ));
              }
              end.append("</ul></p>");
              end.append(QString("<p><h4>Containing node</h4>\n%1</p>").arg(ei.containingNodeName));
	      constraints = ei.constraintStr;
	      locked = ei.lockedStr;
            } else {
              return;
            }
          elmtInfo_->setText (
            QString ("<h4>%1 %2</h4><ul>"
              "<li>Id: %3</li>"
              "%4"
              "</ul>%5%6%7")
            .arg (type).arg (Qt::escape (name)).arg(id).arg (weight)
            .arg(end).arg(constraints).arg(locked));
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

    HppManipulationGraphWidget::EdgeInfo::EdgeInfo ()
      : id (-1), edge (NULL)
    {
      initConfigProjStat (configStat.out());
      initConfigProjStat (pathStat.out());
      errors = new Names_t();
      freqs = new intSeq();
    }

    void HppManipulationGraphWidget::updateWeight (EdgeInfo& ei, bool get)
    {
      if (get) ei.weight = manip_->graph()->getWeight(ei.id);
      if (ei.edge == NULL) return;
      if (ei.weight <= 0) {
          ei.edge->setAttribute("style", "dashed");
          ei.edge->setAttribute("penwidth", "1");
        } else {
          ei.edge->setAttribute("style", "filled");
          ei.edge->setAttribute("penwidth", QString::number(1 + (ei.weight - 1  / 5)));
        }
    }

    void HppManipulationGraphWidget::updateWeight (EdgeInfo& ei, const ::CORBA::Long w)
    {
      manip_->graph()->setWeight(ei.id, w);
      ei.weight = w;
      updateWeight (ei, false);
    }

    QString HppManipulationGraphWidget::getConstraints (hpp::ID id)
    {
      QString ret;
      hpp::Names_t_var c = new hpp::Names_t;
      manip_->graph()->getNumericalConstraints(id, c);
      ret.append("<p><h4>Applied constraints</h4>");
      if (c->length() > 0) {
	ret.append("<ul>");
	for (unsigned i = 0; i < c->length(); i++) {
	  ret.append(QString("<li>%1</li>").arg(c[i].in()));
	}
	ret.append("</ul></p>");
      }
      else
	ret.append("No constraints applied</p>");
      return ret;
    }

    QString HppManipulationGraphWidget::getLockedJoints (hpp::ID id)
    {
      QString ret;
      hpp::Names_t_var c = new hpp::Names_t;
      manip_->graph()->getLockedJoints(id, c);
      ret.append("<p><h4>Locked joints</h4>");
      if (c->length() > 0) {
	ret.append("<ul>");
	for (unsigned i = 0; i < c->length(); i++) {
	  ret.append(QString("<li>%1</li>").arg(c[i].in()));
	}
	ret.append("</ul></p>");
      }
      else
	ret.append("No locked joints</p>");
      return ret;
    }
  }
}
