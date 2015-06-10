#include "hpp/plot/graph-widget.hh"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGraphicsTextItem>
#include <QTextDocument>
#include <QGraphicsSceneDragDropEvent>

#include <QDebug>

namespace hpp {
  namespace plot {
    GraphScene::GraphScene (GraphWidget *parent)
      : QGraphicsScene (parent),
        parent_ (parent),
        dragging_ (0)
    {}

    void GraphScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
    {
      qDebug() << "Start taratata";
    }

    void GraphScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
    {
      if (dragging_ == 0) {
          QPointF pos = event->scenePos();
          QRectF rect (sceneRect().topLeft() + (pos - lastDraggingPos_),
                       sceneRect().bottomRight() + (pos - lastDraggingPos_));
//          setSceneRect(rect);
          lastDraggingPos_ = pos;
        }
    }

    void GraphScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
    {
      qDebug() << "Stop dragging...";
    }

    void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
      dragging_ = itemAt(event->scenePos());
      lastDraggingPos_ = event->scenePos();
      if (dragging_ == 0) {
          qDebug() << "Start dragging";
      }
    }

    void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
      if (dragging_ == 0 && event->buttons() == Qt::LeftButton ) {
          QPointF pos = event->scenePos();
          QPointF dl = pos - lastDraggingPos_;
          qDebug() << "dragging of " << dl;
//          parent_->graph_.translate(dl);
//          parent_->updateGraph();
          lastDraggingPos_ = pos;
          event->accept();
      }
    }

    void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
      qDebug() << "Releasing...";
    }

    GraphWidget::GraphWidget(QString name, QWidget* parent)
      : QWidget (parent),
        graph_ (name, QFont (), 100),
        buttonBox_ (new QWidget (this)),
        scene_ (new GraphScene (this)),
        view_ (new QGraphicsView (scene_))
    {
      QVBoxLayout* vLayout = new QVBoxLayout (this);
      vLayout->setSpacing (0);
      vLayout->setContentsMargins (0, 0, 0, 0);
      vLayout->addWidget (buttonBox_);
      vLayout->addWidget (view_);
      view_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      view_->setAcceptDrops(true);
      view_->setAlignment(0);
      this->setLayout(vLayout);

      QHBoxLayout* hLayout = new QHBoxLayout (buttonBox_);
      QPushButton* refresh = new QPushButton (
            QIcon::fromTheme("view-refresh"), "&Refresh", buttonBox_);
      buttonBox_->setLayout(hLayout);
      hLayout->setAlignment(buttonBox_, Qt::AlignRight);
      hLayout->addSpacerItem(new QSpacerItem (0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
      hLayout->addWidget(refresh);
      connect(refresh, SIGNAL (clicked ()), this, SLOT (updateGraph()));
    }

    void GraphWidget::updateGraph()
    {
      graph_.applyLayout();
      foreach (const GVNode& n, graph_.nodes()) {
          scene_->addEllipse(n.centerPos.x() - n.width / 2,
                             n.centerPos.y() - n.height / 2,
                             n.width, n.height);
          QGraphicsTextItem *text = scene_->addText(n.name);
          QFont f = text->font(); f.setPointSize(30); text->setFont(f);
          text->setPos(n.centerPos.x() - text->document()->size().width() / 2,
                       n.centerPos.y() - text->document()->size().height() / 2);
        }
      foreach (const GVEdge& e, graph_.edges()) {
          scene_->addPath(e.path);
        }
      view_->setSceneRect(graph_.boundingRect());
    }
  }
}

