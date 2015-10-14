#include "hpp/plot/hpp-manipulation-graph.h"

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
    GraphAction::GraphAction(QWidget *parent) :
      QAction (parent)
    {
      connect (this, SIGNAL (activated()), SLOT(transferSignal()));
    }

    void GraphAction::transferSignal()
    {
      emit activated (id_);
    }

    const int HppManipulationGraphWidget::IdRole = Qt::UserRole;
    const int HppManipulationGraphWidget::SuccessRateRole = IdRole + 1;

    HppManipulationGraphWidget::HppManipulationGraphWidget (corbaServer::manipulation::Client* hpp_, QWidget *parent)
      : GraphWidget ("Manipulation graph", parent),
        manip_ (hpp_),
        updateStatsTimer_ (new QTimer (this))
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
          n->setData(IdRole, QVariant::fromValue < ::hpp::ID> (elmts->nodes[i].id));
          n->setData(SuccessRateRole, 0.f/0.f);
          n->setFlag (QGraphicsItem::ItemIsMovable, true);
          n->setFlag (QGraphicsItem::ItemSendsGeometryChanges, true);
          nodes[elmts->nodes[i].id] = n;
        }
        for (std::size_t i = 0; i < elmts->edges.length(); ++i) {
          QGVEdge* e = scene_->addEdge (nodes[elmts->edges[i].start],
              nodes[elmts->edges[i].end],
              QString (elmts->edges[i].name));
          e->setData(IdRole, QVariant::fromValue < ::hpp::ID> (elmts->edges[i].id));
          e->setData(SuccessRateRole, 0.f/0.f);
        }
      } catch (const hpp::Error& e) {
        qDebug () << e.msg;
      }
      //*/
    }

    void HppManipulationGraphWidget::updateStatistics()
    {
      hpp::ConfigProjStat_var config, path;
      foreach (QGraphicsItem* elmt, scene_->items()) {
          QGVNode* node = dynamic_cast <QGVNode*> (elmt);
          if (node) {
              manip_->graph()->getConfigProjectorStats (node->data(IdRole).value <hpp::ID>(),
                                                        config.out(), path.out());
              float sr = (config->nbObs > 0) ? (float)config->success/(float)config->nbObs : 0.f / 0.f;
              node->setData(SuccessRateRole, sr);
              QString colorcode = (config->nbObs > 0) ? QColor (255,(int)(sr*255),(int)(sr*255)).name() : "white";
              const QString& fillcolor = node->getAttribute("fillcolor");
              if (!(fillcolor == colorcode)) {
                  node->setAttribute("fillcolor", colorcode);
                  node->updateLayout();
                }
              continue;
            }
          QGVEdge* edge = dynamic_cast <QGVEdge*> (elmt);
          if (edge) {
              manip_->graph()->getConfigProjectorStats (edge->data(IdRole).value <hpp::ID>(),
                                                        config.out(), path.out());
              float sr = (config->nbObs > 0) ? (float)config->success/(float)config->nbObs : 0.f / 0.f;
              edge->setData(SuccessRateRole, sr);
              QString colorcode = (config->nbObs > 0) ? QColor (255 - (int)(sr*255),0,0).name() : "";
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
      ::hpp::ID id = node->data (IdRole).value < ::hpp::ID> ();

      QMenu cm ("Node context menu", this);
      foreach (GraphAction* action, nodeContextMenuActions_) {
          cm.addAction (action);
          action->id_ = id;
        }
      cm.exec(QCursor::pos());
    }

    void HppManipulationGraphWidget::nodeDoubleClick(QGVNode *node)
    {
      QMessageBox::information(this, tr("Node double clicked"), tr("Node %1").arg(node->label()));
    }

    void HppManipulationGraphWidget::edgeContextMenu(QGVEdge *edge)
    {
      ::hpp::ID id = edge->data (IdRole).value < ::hpp::ID> ();

      QMenu cm ("Edge context menu", this);
      foreach (GraphAction* action, edgeContextMenuActions_) {
          cm.addAction (action);
          action->id_ = id;
        }
      cm.exec(QCursor::pos());
    }

    void HppManipulationGraphWidget::edgeDoubleClick(QGVNode *edge)
    {
      QMessageBox::information(this, tr("Node double clicked"), tr("Node %1").arg(edge->label()));
    }

    void HppManipulationGraphWidget::selectionChanged()
    {
      QList <QGraphicsItem*> items = scene_->selectedItems();
      if (items.size() == 0) {
          elmtInfo_->setText ("No info");
        }
      if (items.size() == 1) {
          QString type, name; float sr; ::hpp::ID id;
          QGVNode* node = dynamic_cast <QGVNode*> (items.first());
          QGVEdge* edge = dynamic_cast <QGVEdge*> (items.first());
          if (node) {
              type = "Node";
              name = node->label();
            } else if (edge) {
              type = "Edge";
              name = edge->label();
            } else {
              return;
            }
          sr = items.first()->data(SuccessRateRole).toFloat();
          id = items.first()->data(IdRole).value < ::hpp::ID> ();
          elmtInfo_->setText (
            QString ("<h4>%1 %2</h4><ul>"
              "<li>Id: %3</li>"
              "<li>Success rate: %4</li>"
              "</ul>")
            .arg (type).arg (name).arg(id).arg(sr));
        }
    }

    void hpp::plot::HppManipulationGraphWidget::startStopUpdateStats(bool start)
    {
      if (start) updateStatsTimer_->start ();
      else       updateStatsTimer_->stop ();
    }
  }
}
