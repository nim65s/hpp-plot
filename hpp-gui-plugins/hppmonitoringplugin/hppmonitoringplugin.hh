#ifndef HPP_PLOT_HPPWIDGETSPLUGIN_HH
#define HPP_PLOT_HPPWIDGETSPLUGIN_HH

#include <hpp/gui/plugin-interface.hh>
#include <hpp/plot/hpp-manipulation-graph.hh>

#include <hpp/corbaserver/manipulation/client.hh>
#undef __robot_hh__
#undef __problem_hh__
#include <hpp/corbaserver/client.hh>

namespace hpp {
  namespace plot {
    class HppMonitoringPlugin : public QObject, public gui::PluginInterface,
    public gui::CorbaInterface
    {
      Q_OBJECT
        Q_INTERFACES (hpp::gui::PluginInterface hpp::gui::CorbaInterface)

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
          bool projectCurrentConfigOn(hpp::ID idNode);
          bool extendFromCurrentToCurrentConfigOn(hpp::ID idEdge);
          bool extendFromCurrentToRandomConfigOn (hpp::ID idEdge);

    signals:
        void projectionStatus (QString status);

      private:
        bool projectConfigOn(hpp::floatSeq config, hpp::ID idNode);
        bool extendConfigOn(hpp::floatSeq from, hpp::floatSeq config, hpp::ID idEdge);


        bool projectRandomConfigOn_impl(hpp::ID idNode);

        hpp::plot::HppManipulationGraphWidget* cgWidget_;
        QList <QDockWidget*> docks_;

        hpp::corbaServer::manipulation::Client* manip_;
        hpp::corbaServer::Client* basic_;
    };
  } // namespace plot
} // namespace hpp

#endif // HPP_PLOT_HPPWIDGETSPLUGIN_HH
