#ifndef HPPMANIPULATIONPLUGIN_HH
#define HPPMANIPULATIONPLUGIN_HH

#include <hpp/gui/plugin-interface.hh>
#include <hpp/gui/omniorb/omniorbthread.hh>

namespace hpp {
  namespace plot {
    class HppManipulationPlugin : public QObject, public gui::PluginInterface
    {
      Q_OBJECT
        Q_INTERFACES (hpp::gui::PluginInterface)

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
          gui::CorbaServer* server_;
    };
  } // namespace plot
} // namespace hpp

#endif // HPPMANIPULATIONPLUGIN_HH
