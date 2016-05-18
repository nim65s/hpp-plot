#include "hppmonitoringplugin.hh"

#include <limits>

#include <gepetto/gui/mainwindow.hh>
#include <gepetto/gui/omniorb/url.hh>

using gepetto::gui::MainWindow;

namespace hpp {
  namespace plot {
    HppMonitoringPlugin::HppMonitoringPlugin() :
      cgWidget_ (NULL),
      manip_ (NULL),
      basic_ (NULL)
    {
    }

    HppMonitoringPlugin::~HppMonitoringPlugin()
    {
      MainWindow* main = MainWindow::instance();
      foreach (QDockWidget* dock, docks_) {
        main->removeDockWidget(dock);
        delete dock;
      }
      docks_.clear();
      closeConnection ();
    }

    void HppMonitoringPlugin::init()
    {
      openConnection ();

      MainWindow* main = MainWindow::instance ();
      QDockWidget* dock;

      // Constraint graph widget
      dock = new QDockWidget ("Constraint &Graph", main);
      cgWidget_ = new hpp::plot::HppManipulationGraphWidget (manip_, main);
      dock->setWidget(cgWidget_);
      main->insertDockWidget (dock, Qt::BottomDockWidgetArea, Qt::Horizontal);
      dock->toggleViewAction()->setShortcut(gepetto::gui::DockKeyShortcutBase + Qt::Key_G);

      hpp::plot::GraphAction* a = new hpp::plot::GraphAction (cgWidget_);
      a->setShortcut(Qt::Key_C);
      a->setText ("Generate from &current config");
      connect (a, SIGNAL (activated(hpp::ID)), SLOT (projectCurrentConfigOn(hpp::ID)));
      cgWidget_->addNodeContextMenuAction (a);
      cgWidget_->addAction(a);

      a = new hpp::plot::GraphAction (cgWidget_);
      a->setShortcut(Qt::Key_G);
      a->setText ("&Generate config");
      connect (a, SIGNAL (activated(hpp::ID)), SLOT (projectRandomConfigOn(hpp::ID)));
      cgWidget_->addNodeContextMenuAction (a);
      cgWidget_->addAction(a);

      a = new hpp::plot::GraphAction (cgWidget_);
      a->setShortcut(Qt::Key_E);
      a->setText ("&Extend current config");
      connect (a, SIGNAL (activated(hpp::ID)), SLOT (extendFromCurrentToCurrentConfigOn(hpp::ID)));
      cgWidget_->addEdgeContextMenuAction (a);
      cgWidget_->addAction(a);

      a = new hpp::plot::GraphAction (cgWidget_);
      a->setShortcut(Qt::Key_R);
      a->setText ("&Extend current config to random config");
      connect (a, SIGNAL (activated(hpp::ID)), SLOT (extendFromCurrentToRandomConfigOn(hpp::ID)));
      cgWidget_->addEdgeContextMenuAction (a);
      cgWidget_->addAction(a);

      connect (main, SIGNAL (refresh()), cgWidget_, SLOT (updateGraph()));
      connect (main, SIGNAL (applyCurrentConfiguration()),
          SLOT (applyCurrentConfiguration()));
    }

    QString HppMonitoringPlugin::name() const
    {
      return QString ("Monitoring for hpp-manipulation-corba");
    }

    static QString getIIOPurl ()
    {
      QString host = MainWindow::instance ()->settings_->getSetting
        ("hpp/host", QString ()).toString ();
      QString port = MainWindow::instance ()->settings_->getSetting
        ("hpp/port", QString ()).toString ();
      return gepetto::gui::omniOrb::IIOPurl (host, port);
    }

    void HppMonitoringPlugin::openConnection()
    {
      closeConnection ();
      basic_ = new hpp::corbaServer::Client (0,0);
      manip_ = new hpp::corbaServer::manipulation::Client (0,0);
      QByteArray iiop = getIIOPurl ().toAscii();
      basic_->connect (iiop.constData ());
      manip_->connect (iiop.constData ());
      if (cgWidget_) cgWidget_->client (manip_);
    }

    void HppMonitoringPlugin::closeConnection()
    {
      if (basic_) delete basic_;
      basic_ = NULL;
      if (manip_) delete manip_;
      manip_ = NULL;
    }

    bool HppMonitoringPlugin::corbaException(int jobId, const CORBA::Exception &excep) const
    {
      try {
        const hpp::Error& error = dynamic_cast <const hpp::Error&> (excep);
        MainWindow::instance ()->logJobFailed (jobId, QString (error.msg));
        return true;
      } catch (const std::exception& exp) {
        qDebug () << exp.what();
      }
      return false;
    }

    void HppMonitoringPlugin::projectRandomConfigOn(hpp::ID idNode)
    {
      QFutureWatcher <bool>* fw = new QFutureWatcher<bool>(this);
      QDialog* d = new QDialog (NULL,Qt::Dialog);
      QLabel* l = new QLabel ("Projecting...");
      d->setLayout(new QHBoxLayout);
      d->layout()->addWidget(l);
      connect (this,SIGNAL(projectionStatus(QString)), l, SLOT(setText(QString)));
      d->show();
      fw->setFuture(QtConcurrent::run (this, &HppMonitoringPlugin::projectRandomConfigOn_impl, idNode));
      connect (fw, SIGNAL (finished()), d, SLOT (deleteLater()));
      connect (fw, SIGNAL (finished()), fw, SLOT (deleteLater()));
    }

    bool HppMonitoringPlugin::projectRandomConfigOn_impl(hpp::ID idNode)
    {
      hpp::floatSeq_var qRand;
      hpp::floatSeq_var res;
      ::CORBA::Double error, minError = std::numeric_limits<double>::infinity();
      int i = 0;
      do {
          qRand = basic_->robot ()->shootRandomConfig ();
          i++;
          bool success = manip_->problem()->applyConstraints
              (idNode, qRand.in(), res.out(), error);
          if (success) {
              basic_->robot()->setCurrentConfig (res.in());
              MainWindow::instance ()->requestApplyCurrentConfiguration ();
              return true;
            }
          if (error < minError) {
              minError = error;
            }
          if (i >= 20) {
            qDebug () << "Projection failed after 20 trials.";
            break;
          }
          emit projectionStatus(QString ("Tried %1 times. Minimal residual error is %2").arg(i).arg(minError));
        } while (true);
      return false;
    }

    bool HppMonitoringPlugin::projectCurrentConfigOn(ID idNode)
    {
      hpp::floatSeq_var from = basic_->robot()->getCurrentConfig();
      return projectConfigOn (from.in(), idNode);
    }

    bool HppMonitoringPlugin::extendFromCurrentToCurrentConfigOn(hpp::ID idEdge)
    {
      hpp::floatSeq_var from = basic_->robot()->getCurrentConfig();
      bool success = extendConfigOn (from.in(), from.in(), idEdge);
      if (!success) {
        basic_->robot()->setCurrentConfig (from.in());
        MainWindow::instance ()->requestApplyCurrentConfiguration ();
      }
      return success;
    }

    bool HppMonitoringPlugin::extendFromCurrentToRandomConfigOn (hpp::ID idEdge)
    {
      hpp::floatSeq_var from = basic_->robot()->getCurrentConfig();
      hpp::floatSeq_var qRand = basic_->robot ()->shootRandomConfig ();
      bool success = extendConfigOn (from.in(), qRand.in(), idEdge);
      if (!success) {
        basic_->robot()->setCurrentConfig (from.in());
        MainWindow::instance ()->requestApplyCurrentConfiguration ();
      }
      return success;
    }

    bool HppMonitoringPlugin::projectConfigOn(hpp::floatSeq config, hpp::ID idNode)
    {
      hpp::floatSeq_var res;
      ::CORBA::Double error;
      bool success = manip_->problem()->applyConstraints(idNode, config, res.out(), error);
      if (success) {
        basic_->robot()->setCurrentConfig (res.in());
        MainWindow::instance ()->requestApplyCurrentConfiguration ();
      } else {
        MainWindow::instance ()->logError (QString ("Unable to project configuration. Residual error is %1").arg(error));
      }
      return success;
    }

    bool HppMonitoringPlugin::extendConfigOn(hpp::floatSeq from, hpp::floatSeq config, hpp::ID idEdge)
    {
      hpp::floatSeq_var res;
      ::CORBA::Double error;
      bool success = manip_->problem()->applyConstraintsWithOffset(idEdge, from, config, res.out(), error);
      if (success) {
        basic_->robot()->setCurrentConfig (res.in());
        MainWindow::instance ()->requestApplyCurrentConfiguration ();
      } else {
        MainWindow::instance ()->logError (QString ("Unable to project configuration. Residual error is %1").arg(error));
      }
      return success;
    }

    void HppMonitoringPlugin::applyCurrentConfiguration ()
    {
      hpp::floatSeq_var q = basic_->robot()->getCurrentConfig();
      cgWidget_->showNodeOfConfiguration (q.in());
    }
  } // namespace plot
} // namespace hpp

Q_EXPORT_PLUGIN2 (hppmonitoringplugin, hpp::plot::HppMonitoringPlugin)
