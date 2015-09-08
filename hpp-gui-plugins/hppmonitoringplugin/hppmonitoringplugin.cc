#include "hppmonitoringplugin.hh"

#include <hpp/gui/mainwindow.h>

HppMonitoringPlugin::HppMonitoringPlugin() :
  cgWidget_ (NULL),
  manip_ (new hpp::corbaServer::manipulation::Client (0,NULL)),
  basic_ (new hpp::corbaServer::Client (0,NULL))
{
  manip_->connect();
  basic_->connect();
}

HppMonitoringPlugin::~HppMonitoringPlugin()
{
  MainWindow* main = MainWindow::instance();
  foreach (QDockWidget* dock, docks_) {
      main->removeDockWidget(dock);
      delete dock;
    }
  docks_.clear();
  delete manip_;
  delete basic_;
}

void HppMonitoringPlugin::init()
{
  MainWindow* main = MainWindow::instance ();
  QDockWidget* dock;

  // Constraint graph widget
  dock = new QDockWidget ("Constraint Graph", main);
  cgWidget_ = new hpp::plot::HppManipulationGraphWidget (manip_, main);
  dock->setWidget(cgWidget_);
  main->insertDockWidget (dock, Qt::BottomDockWidgetArea, Qt::Horizontal);

  hpp::plot::GraphAction* a = new hpp::plot::GraphAction (cgWidget_);
  a->setText ("Generate config");
  connect (a, SIGNAL (activated(hpp::ID)), SLOT (projectRandomConfigOn(hpp::ID)));
  cgWidget_->addNodeContextMenuAction (a);

  a = new hpp::plot::GraphAction (cgWidget_);
  a->setText ("Extend current config");
  connect (a, SIGNAL (activated(hpp::ID)), SLOT (extendCurrentConfigOn(hpp::ID)));
  cgWidget_->addEdgeContextMenuAction (a);

  connect (main, SIGNAL (refresh()), cgWidget_, SLOT (updateGraph()));
}

QString HppMonitoringPlugin::name() const
{
  return QString ("Monitoring for hpp-manipulation-corba");
}

void HppMonitoringPlugin::projectRandomConfigOn(hpp::ID idNode)
{
  hpp::floatSeq_var qRand = basic_->robot ()->shootRandomConfig ();
  projectConfigOn (qRand.in(), idNode);
}

void HppMonitoringPlugin::extendCurrentConfigOn(hpp::ID idEdge)
{
  hpp::floatSeq_var from = basic_->robot()->getCurrentConfig();
  hpp::floatSeq_var qRand = basic_->robot ()->shootRandomConfig ();
  extendConfigOn (from.in(), qRand.in(), idEdge);
}

void HppMonitoringPlugin::projectConfigOn(hpp::floatSeq config, hpp::ID idNode)
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
}

void HppMonitoringPlugin::extendConfigOn(hpp::floatSeq from, hpp::floatSeq config, hpp::ID idEdge)
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
}

Q_EXPORT_PLUGIN2 (hppmonitoringplugin, HppMonitoringPlugin)
