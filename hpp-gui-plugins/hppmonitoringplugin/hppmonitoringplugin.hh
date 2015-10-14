#ifndef HPPWIDGETSPLUGIN_HH
#define HPPWIDGETSPLUGIN_HH

#include <hpp/gui/plugin-interface.h>
#include <hpp/plot/hpp-manipulation-graph.h>

#include <hpp/corbaserver/manipulation/client.hh>
#undef __robot_hh__
#undef __problem_hh__
#include <hpp/corbaserver/client.hh>

class HppMonitoringPlugin : public QObject, public PluginInterface,
  public CorbaInterface
{
  Q_OBJECT
  Q_INTERFACES (PluginInterface CorbaInterface)

public:
  explicit HppMonitoringPlugin ();

  virtual ~HppMonitoringPlugin ();

  // PluginInterface interface
public:
  void init();
  QString name() const;

  // CorbaInterface
public:
  virtual void openConnection ();
  virtual void closeConnection();
  virtual bool corbaException (int jobId, const CORBA::Exception &excep) const;

public slots:
  void projectRandomConfigOn(hpp::ID idNode);
  void extendCurrentConfigOn(hpp::ID idEdge);

private:
  void projectConfigOn(hpp::floatSeq config, hpp::ID idNode);
  void extendConfigOn(hpp::floatSeq from, hpp::floatSeq config, hpp::ID idEdge);

  hpp::plot::HppManipulationGraphWidget* cgWidget_;
  QList <QDockWidget*> docks_;

  hpp::corbaServer::manipulation::Client* manip_;
  hpp::corbaServer::Client* basic_;
};

#endif // HPPWIDGETSPLUGIN_HH
