#include "hpp/plot/graph-widget.hh"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <QWheelEvent>
#include <QSplitter>
#include <QScrollBar>
#include <QGraphicsSceneDragDropEvent>
#include <qmath.h>

#include "QGVScene.h"
#include "QGVNode.h"
#include "QGVEdge.h"
#include "QGVSubGraph.h"

#include <QDebug>

namespace hpp {
  namespace plot {
    GraphView::GraphView(QWidget *parent)
      : QGraphicsView (parent)
    {
      setTransformationAnchor (AnchorUnderMouse);
      setDragMode (ScrollHandDrag);
      setBackgroundBrush (QBrush (Qt::white, Qt::SolidPattern));
    }

    void GraphView::wheelEvent(QWheelEvent *event)
    {
      if (event->modifiers () == Qt::ControlModifier) {
          qreal scaleFactor = qPow(2.0, event->delta() / 240.0); //How fast we zoom
          qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
          if(0.05 < factor && factor < 10) //Zoom factor limitation
            scale(scaleFactor, scaleFactor);
        }
    }

    GraphWidget::GraphWidget(QString name, QWidget* parent)
      : QWidget (parent),
        scene_ (new QGVScene(name, 0)),
        buttonBox_ (new QWidget ()),
        elmtInfo_ (new QTextEdit ()),
        loggingInfo_ (new QTextEdit ()),
        view_ (new GraphView (0)),
        layoutShouldBeFreed_ (false)
    {
      view_->setScene(scene_);

      elmtInfo_->setReadOnly (true);
      loggingInfo_->setReadOnly (true);
      elmtInfo_->setText ("No info");
      loggingInfo_->setText ("No info");
      elmtInfo_->setWordWrapMode(QTextOption::NoWrap);
      loggingInfo_->setWordWrapMode(QTextOption::NoWrap);

      view_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      view_->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
      view_->setRenderHint(QPainter::Antialiasing);
      view_->setRenderHint(QPainter::TextAntialiasing);

      QSplitter* splitter = new QSplitter ();
      splitter->setOrientation(Qt::Horizontal);
      QWidget* infoW = new QWidget ();
      QVBoxLayout* infoL = new QVBoxLayout ();
      infoL->addWidget(elmtInfo_);
      infoL->addWidget(loggingInfo_);
      infoW->setLayout(infoL);
      splitter->addWidget(infoW);
      splitter->addWidget(view_);
      QVBoxLayout* vl = new QVBoxLayout ();
      vl->addWidget(buttonBox_);
      vl->addWidget(splitter);
      splitter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      this->setLayout(vl);

      elmtInfo_->resize(elmtInfo_->minimumSize());
      loggingInfo_->resize(loggingInfo_->minimumSize());
      view_->resize(splitter->width() - elmtInfo_->width(), splitter->height());

      connect(scene_, SIGNAL(nodeContextMenu(QGVNode*)), SLOT(nodeContextMenu(QGVNode*)));
      connect(scene_, SIGNAL(nodeDoubleClick(QGVNode*)), SLOT(nodeDoubleClick(QGVNode*)));
      connect(scene_, SIGNAL(edgeContextMenu(QGVEdge*)), SLOT(edgeContextMenu(QGVEdge*)));
      connect(scene_, SIGNAL(edgeDoubleClick(QGVEdge*)), SLOT(edgeDoubleClick(QGVEdge*)));

      QHBoxLayout* hLayout = new QHBoxLayout (buttonBox_);
      hLayout->setMargin(0);
      algList_ = new QComboBox (this);
      algList_->addItems (QStringList () << "dot" << "neato" << "fdp" << "sfdp" << "twopi" << "circo" << "patchwork" << "osage");
      QPushButton* refresh = new QPushButton (
            QIcon::fromTheme("view-refresh"), "&Refresh", buttonBox_);
      QPushButton* update = new QPushButton (
            QIcon::fromTheme("view-refresh"), "&Update edges", buttonBox_);
      buttonBox_->setLayout(hLayout);
      buttonBox_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
      hLayout->setAlignment(buttonBox_, Qt::AlignRight);
      hLayout->addSpacerItem(new QSpacerItem (0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
      hLayout->addWidget(algList_);
      hLayout->addWidget(update);
      hLayout->addWidget(refresh);
      connect(refresh, SIGNAL (clicked ()), this, SLOT (updateGraph()));
      connect(update, SIGNAL (clicked ()), this, SLOT (updateEdges()));

      connect(scene_, SIGNAL (nodeMouseRelease(QGVNode*)), this, SLOT (updateEdges()));
    }

    GraphWidget::~GraphWidget()
    {
      delete scene_;
    }

    void GraphWidget::updateGraph()
    {
      //Layout scene
      if (layoutShouldBeFreed_) scene_->freeLayout ();
      scene_->clear();
      fillScene();
      scene_->applyLayout(algList_->currentText ());
      layoutShouldBeFreed_ = true;
      scene_->render("dot");

      //Fit in view
      view_->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
    }

    void GraphWidget::updateEdges()
    {
      //Layout scene
      if (layoutShouldBeFreed_) scene_->freeLayout ();
      scene_->applyLayout("nop");
      layoutShouldBeFreed_ = true;
      scene_->render("dot");
    }

    void GraphWidget::nodeContextMenu(QGVNode *node)
    {
      Q_UNUSED (node)
    }

    void GraphWidget::nodeDoubleClick(QGVNode *node)
    {
      Q_UNUSED (node)
    }

    void hpp::plot::GraphWidget::edgeDoubleClick(QGVEdge *edge)
    {
      Q_UNUSED (edge)
    }

    void hpp::plot::GraphWidget::edgeContextMenu(QGVEdge *edge)
    {
      Q_UNUSED (edge)
    }

    void GraphWidget::fillScene()
    {
      /*
      scene_->loadLayout("digraph test{node [style=filled,fillcolor=white];N1 -> N2;N2 -> N3;N3 -> N4;N4 -> N1;}");
      connect(scene_, SIGNAL(nodeContextMenu(QGVNode*)), SLOT(nodeContextMenu(QGVNode*)));
      connect(scene_, SIGNAL(nodeDoubleClick(QGVNode*)), SLOT(nodeDoubleClick(QGVNode*)));
      ui->graphicsView->setScene(scene_);
      return;
      */

      //Configure scene attributes
      scene_->setGraphAttribute("label", "DEMO");

      scene_->setGraphAttribute("splines", "ortho");
      scene_->setGraphAttribute("rankdir", "LR");
      //scene_->setGraphAttribute("concentrate", "true"); //Error !
      scene_->setGraphAttribute("nodesep", "0.4");

      scene_->setNodeAttribute("shape", "box");
      scene_->setNodeAttribute("style", "filled");
      scene_->setNodeAttribute("fillcolor", "white");
      scene_->setNodeAttribute("height", "1.2");
      scene_->setEdgeAttribute("minlen", "3");
      //scene_->setEdgeAttribute("dir", "both");

      //Add some nodes
      QGVNode *node1 = scene_->addNode("BOX");
      node1->setIcon(QIcon::fromTheme("view-refresh").pixmap(24,24).toImage());
      QGVNode *node2 = scene_->addNode("SERVER0");
      node2->setIcon(QImage(":/icons/Gnome-Network-Transmit-64.png"));
      node2->setFlag(QGraphicsItem::ItemIsMovable, true);
      node2->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
      QGVNode *node3 = scene_->addNode("SERVER1");
      node3->setIcon(QImage(":/icons/Gnome-Network-Transmit-64.png"));
      QGVNode *node4 = scene_->addNode("USER");
      node4->setIcon(QImage(":/icons/Gnome-Stock-Person-64.png"));
      QGVNode *node5 = scene_->addNode("SWITCH");
      node5->setIcon(QImage(":/icons/Gnome-Network-Server-64.png"));

      //Add some edges
      scene_->addEdge(node1, node2, "TTL")->setAttribute("color", "red");
      scene_->addEdge(node1, node2, "SERIAL");
      scene_->addEdge(node1, node3, "RAZ")->setAttribute("color", "blue");
      scene_->addEdge(node2, node3, "SECU");

      scene_->addEdge(node2, node4, "STATUS")->setAttribute("color", "red");

      scene_->addEdge(node4, node3, "ACK")->setAttribute("color", "red");

      scene_->addEdge(node4, node2, "TBIT");
      scene_->addEdge(node4, node2, "ETH");
      scene_->addEdge(node4, node2, "RS232");

      scene_->addEdge(node4, node5, "ETH1");
      scene_->addEdge(node2, node5, "ETH2");

      QGVSubGraph *sgraph = scene_->addSubGraph("SUB1");
      sgraph->setAttribute("label", "OFFICE");

      QGVNode *snode1 = sgraph->addNode("PC0152");
      QGVNode *snode2 = sgraph->addNode("PC0153");

      scene_->addEdge(snode1, snode2, "RT7");

      scene_->addEdge(node3, snode1, "GB8");
      scene_->addEdge(node3, snode2, "TS9");


      QGVSubGraph *ssgraph = sgraph->addSubGraph("SUB2");
      ssgraph->setAttribute("label", "DESK");
      scene_->addEdge(snode1, ssgraph->addNode("PC0155"), "S10");
    }
  }
}
