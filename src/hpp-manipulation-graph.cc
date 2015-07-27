#include "hpp/plot/hpp-manipulation-graph.h"

#include <QGVNode.h>
#include <QGVEdge.h>
#include <QMap>

namespace hpp {
  namespace plot {
    HppManipulationGraphWidget::HppManipulationGraphWidget (QWidget *parent)
      : GraphWidget ("Manipulation graph", parent),
        client_ (0, NULL)
    {
      client_.connect();
    }

    void HppManipulationGraphWidget::fillScene()
    {
//    /*
      hpp::GraphComp_var graph;
      hpp::GraphElements_var elmts;
      client_.graph()->getGraph(graph.out(), elmts.out());

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
          nodes[elmts->nodes[i].id] = scene_->addNode (QString (elmts->nodes[i].name));
          nodes[elmts->nodes[i].id]->setFlag (QGraphicsItem::ItemIsMovable, true);
          nodes[elmts->nodes[i].id]->setFlag (QGraphicsItem::ItemSendsGeometryChanges, true);
        }
      for (std::size_t i = 0; i < elmts->edges.length(); ++i) {
          scene_->addEdge (nodes[elmts->edges[i].start],
              nodes[elmts->edges[i].end],
              QString (elmts->edges[i].name));
        }
      //*/
    }
  }
}
