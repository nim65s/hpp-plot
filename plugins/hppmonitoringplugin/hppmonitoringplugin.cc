// BSD 2-Clause License

// Copyright (c) 2015 - 2018, hpp-plot
// Authors: Heidy Dallard, Joseph Mirabel
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:

// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#include "hppmonitoringplugin.hh"

#include <limits>
#include <QDockWidget>
#include <QHBoxLayout>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
# include <QtConcurrent>
#else
# include <QtCore>
#endif

#include <gepetto/gui/mainwindow.hh>
#include <gepetto/gui/omniorb/url.hh>

using gepetto::gui::MainWindow;

namespace hpp {
  namespace plot {
    HppMonitoringPlugin::HppMonitoringPlugin() :
      cgWidget_ (NULL),
      manip_ (NULL),
      basic_ (NULL),
      hppPlugin_ (NULL)
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
      dock->setObjectName ("hppmonitoringplugin.constraintgraph");
      cgWidget_ = new hpp::plot::HppManipulationGraphWidget (manip_, main);
      dock->setWidget(cgWidget_);
      main->insertDockWidget (dock, Qt::RightDockWidgetArea, Qt::Horizontal);
      dock->toggleViewAction()->setShortcut(gepetto::gui::DockKeyShortcutBase + Qt::Key_G);
      main->registerShortcut(dock->windowTitle(), "Toggle view", dock->toggleViewAction());

      hpp::plot::GraphAction* a = new hpp::plot::GraphAction (cgWidget_);
      a->setShortcut(Qt::Key_C);
      a->setText ("Generate from &current config");
      connect (a, SIGNAL (activated(hpp::ID)), SLOT (projectCurrentConfigOn(hpp::ID)));
      cgWidget_->addNodeContextMenuAction (a);
      cgWidget_->addAction(a);
      main->registerShortcut(dock->windowTitle(), a);

      a = new hpp::plot::GraphAction (cgWidget_);
      a->setShortcut(Qt::Key_G);
      a->setText ("&Generate config");
      connect (a, SIGNAL (activated(hpp::ID)), SLOT (projectRandomConfigOn(hpp::ID)));
      cgWidget_->addNodeContextMenuAction (a);
      cgWidget_->addAction(a);
      main->registerShortcut(dock->windowTitle(), a);

      a = new hpp::plot::GraphAction (cgWidget_);
      a->setShortcut(Qt::Key_T);
      a->setText ("Set as &target state");
      connect (a, SIGNAL (activated(hpp::ID)), SLOT (setTargetState(hpp::ID)));
      cgWidget_->addNodeContextMenuAction (a);
      cgWidget_->addAction(a);
      main->registerShortcut(dock->windowTitle(), a);

      a = new hpp::plot::GraphAction (cgWidget_);
      // a->setShortcut(Qt::Key_C);
      a->setText ("Display &node constraints");
      cgWidget_->connect (a, SIGNAL (activated(hpp::ID)), SLOT (displayNodeConstraint(hpp::ID)));
      cgWidget_->addNodeContextMenuAction (a);
      // cgWidget_->addAction(a);
      // main->registerShortcut(dock->windowTitle(), a);

      a = new hpp::plot::GraphAction (cgWidget_);
      a->setShortcut(Qt::Key_E);
      a->setText ("&Extend current config");
      connect (a, SIGNAL (activated(hpp::ID)), SLOT (extendFromCurrentToCurrentConfigOn(hpp::ID)));
      cgWidget_->addEdgeContextMenuAction (a);
      cgWidget_->addAction(a);
      main->registerShortcut(dock->windowTitle(), a);

      a = new hpp::plot::GraphAction (cgWidget_);
      a->setShortcut(Qt::Key_R);
      a->setText ("&Extend current config to random config");
      connect (a, SIGNAL (activated(hpp::ID)), SLOT (extendFromCurrentToRandomConfigOn(hpp::ID)));
      cgWidget_->addEdgeContextMenuAction (a);
      cgWidget_->addAction(a);
      main->registerShortcut(dock->windowTitle(), a);

      a = new hpp::plot::GraphAction (cgWidget_);
      // a->setShortcut(Qt::Key_C);
      a->setText ("Display edge &constraints");
      cgWidget_->connect (a, SIGNAL (activated(hpp::ID)), SLOT (displayEdgeConstraint(hpp::ID)));
      cgWidget_->addEdgeContextMenuAction (a);
      cgWidget_->addAction(a);
      main->registerShortcut(dock->windowTitle(), a);

      a = new hpp::plot::GraphAction (cgWidget_);
      // a->setShortcut(Qt::Key_T);
      a->setText ("Display edge &target constraints");
      cgWidget_->connect (a, SIGNAL (activated(hpp::ID)), SLOT (displayEdgeTargetConstraint(hpp::ID)));
      cgWidget_->addEdgeContextMenuAction (a);
      // cgWidget_->addAction(a);
      // main->registerShortcut(dock->windowTitle(), a);

      connect (main, SIGNAL (refresh()), cgWidget_, SLOT (updateGraph()));
      connect (main, SIGNAL (applyCurrentConfiguration()),
          SLOT (applyCurrentConfiguration()));

      main->connectSignal (SIGNAL(appliedConfigAtParam(int,double)), 
                           SLOT(appliedConfigAtParam(int,double)),
                           this);
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
      QByteArray iiop = getIIOPurl ().toLatin1();
      try {
        basic_->connect (iiop.constData ());
        manip_->connect (iiop.constData ());
        hpp::Names_t_var for_deletion = manip_->problem()->getAvailable("type");
      } catch (const CORBA::Exception& e) {
        QString error ("Could not find the manipulation server. Is it running ?");
        error += "\n";
        error += e._name();
        error += " : ";
        error += e._rep_id();

        gepetto::gui::MainWindow* main = gepetto::gui::MainWindow::instance();
        if (main != NULL)
          main->logError(error);
        else
          qDebug () << error;

        closeConnection ();
        return;
      }
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
      } catch (const std::bad_cast&) {
        // dynamic_cast failed.
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
      if (manip_ == NULL) return false;
      hpp::floatSeq_var qRand;
      hpp::floatSeq_var res;
      ::CORBA::Double error, minError = std::numeric_limits<double>::infinity();
      int i = 0;
      try {
        do {
            qRand = basic_->robot ()->shootRandomConfig ();
            i++;
            bool success = manip_->problem()->applyConstraints
                (idNode, qRand.in(), res.out(), error);
            if (success) {
                setCurrentConfig (res.in());
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
      } catch (const hpp::Error& e) {
        MainWindow::instance ()->logError (e.msg.in());
      }
      return false;
    }

    bool HppMonitoringPlugin::projectCurrentConfigOn(ID idNode)
    {
      hpp::floatSeq from = getCurrentConfig();
      return projectConfigOn (from, idNode);
    }

    void HppMonitoringPlugin::setTargetState(ID idNode)
    {
      if (manip_ == NULL) return;
      manip_->problem()->setTargetState(idNode);
    }

    bool HppMonitoringPlugin::extendFromCurrentToCurrentConfigOn(hpp::ID idEdge)
    {
      hpp::floatSeq from = getCurrentConfig();
      return extendConfigOn (from, from, idEdge);
    }

    bool HppMonitoringPlugin::extendFromCurrentToRandomConfigOn (hpp::ID idEdge)
    {
      hpp::floatSeq from = getCurrentConfig();
      hpp::floatSeq_var qRand = basic_->robot ()->shootRandomConfig ();
      return extendConfigOn (from, qRand.in(), idEdge);
    }

    bool HppMonitoringPlugin::projectConfigOn(hpp::floatSeq config, hpp::ID idNode)
    {
      if (manip_ == NULL) return false;
      hpp::floatSeq_var res;
      ::CORBA::Double error;
      bool success = manip_->problem()->applyConstraints(idNode, config, res.out(), error);
      if (success) {
        setCurrentConfig (res.in());
      } else {
        MainWindow::instance ()->logError (QString ("Unable to project configuration. Residual error is %1").arg(error));
      }
      return success;
    }

    bool HppMonitoringPlugin::extendConfigOn(hpp::floatSeq from, hpp::floatSeq config, hpp::ID idEdge)
    {
      if (manip_ == NULL) return false;
      hpp::floatSeq_var res;
      ::CORBA::Double error;
      bool success = manip_->problem()->applyConstraintsWithOffset(idEdge, from, config, res.out(), error);
      if (success) {
        setCurrentConfig (res.in());
      } else {
        MainWindow::instance ()->logError (QString ("Unable to project configuration. Residual error is %1").arg(error));
      }
      return success;
    }

    void HppMonitoringPlugin::applyCurrentConfiguration ()
    {
      hpp::floatSeq q = getCurrentConfig();
      cgWidget_->showNodeOfConfiguration (q);
    }

    void HppMonitoringPlugin::appliedConfigAtParam (int pid, double param)
    {
      if (manip_ == NULL) return;
      CORBA::String_var graphName;
      hpp::ID id;
      try {
        id = manip_->problem()->edgeAtParam(pid, param, graphName.out());
      } catch (const hpp::Error& e) {
        return;
      }
      if (strcmp(graphName.in(), cgWidget_->graphName().c_str()) != 0)
        cgWidget_->showEdge (id);
    }

    hpp::floatSeq HppMonitoringPlugin::getCurrentConfig ()
    {
      QObject* plugin (hppPlugin());
      hpp::floatSeq const* config;
      bool ok = QMetaObject::invokeMethod (plugin, "getCurrentConfig",
          Qt::DirectConnection,
          Q_RETURN_ARG (hpp::floatSeq const*, config));
      if (!ok) {
        qDebug() << "HppMonitoringPlugin::getCurrentConfig failed";
      }
      return *config;
    }

    void HppMonitoringPlugin::setCurrentConfig (const hpp::floatSeq& q)
    {
      QObject* plugin (hppPlugin());
      bool ok = QMetaObject::invokeMethod (plugin, "setCurrentConfig",
          Qt::DirectConnection,
          Q_ARG (const hpp::floatSeq&, q));

      if (!ok) {
        qDebug() << "HppMonitoringPlugin::setCurrentConfig failed";
      }
    }

    QObject* HppMonitoringPlugin::hppPlugin ()
    {
      if (hppPlugin_ == NULL) {
        MainWindow* main = MainWindow::instance();
        hppPlugin_ = main->getFromSlot ("getCurrentConfig");
      }
      if (hppPlugin_ == NULL) {
        throw std::runtime_error ("unable to retrieve slot getCurrentConfig from HPP plugin");
      }
      return hppPlugin_;
    }
  } // namespace plot
} // namespace hpp

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
Q_EXPORT_PLUGIN2 (hppmonitoringplugin, hpp::plot::HppMonitoringPlugin)
#endif
