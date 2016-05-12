#ifndef HPPMANIPULATIONPLUGIN_HH
#define HPPMANIPULATIONPLUGIN_HH

#include <gepetto/gui/plugin-interface.hh>
#include <gepetto/gui/omniorb/omniorbthread.hh>

namespace hpp {
  namespace plot {
    class HppManipulationPlugin : public QObject, public gepetto::gui::PluginInterface
    {
      Q_OBJECT
        Q_INTERFACES (gepetto::gui::PluginInterface)

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
          gepetto::gui::CorbaServer* server_;
    };
  } // namespace plot
} // namespace hpp

#endif // HPPMANIPULATIONPLUGIN_HH
