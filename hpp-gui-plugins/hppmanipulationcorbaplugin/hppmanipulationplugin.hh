#ifndef HPPMANIPULATIONPLUGIN_HH
#define HPPMANIPULATIONPLUGIN_HH

#include <hpp/gui/plugin-interface.h>
#include <hpp/gui/omniorb/omniorbthread.h>

class HppManipulationPlugin : public QObject, public PluginInterface
{
  Q_OBJECT
  Q_INTERFACES (PluginInterface)

public:
  explicit HppManipulationPlugin ();

  virtual ~HppManipulationPlugin ();

signals:

public slots:

  // PluginInterface interface
public:
  void init();
  QString name() const;

private:
  CorbaServer* server_;
};

#endif // HPPMANIPULATIONPLUGIN_HH
