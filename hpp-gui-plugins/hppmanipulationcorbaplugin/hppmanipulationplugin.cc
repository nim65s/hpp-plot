#include "hppmanipulationplugin.hh"

#include <hpp/manipulation/problem-solver.hh>
#include <hpp/manipulation/manipulation-planner.hh>
#include <hpp/core/discretized-collision-checking.hh>
#include <hpp/manipulation/graph-path-validation.hh>

#include <hppserverprocess.hh>

using gepetto::gui::CorbaServer;

namespace hpp {
  namespace plot {
    HppManipulationPlugin::HppManipulationPlugin() :
      server_ (NULL)
    {
    }

    HppManipulationPlugin::~HppManipulationPlugin()
    {
      if (server_) {
        server_->wait();
        delete server_;
      }
    }

    void HppManipulationPlugin::init()
    {
      hpp::manipulation::ProblemSolverPtr_t ps = hpp::manipulation::ProblemSolver::create ();
      ps->addPathPlannerType ("M-RRT", hpp::manipulation::ManipulationPlanner::create);
      ps->addPathValidationType ("Graph-discretized", hpp::manipulation::
          GraphPathValidation::create <hpp::core::DiscretizedCollisionChecking>);
      ps->pathPlannerType ("M-RRT");
      ps->pathValidationType ("Graph-discretized", 0.05);

      hpp::corbaServer::Server* bs = new hpp::corbaServer::Server (ps, 0, NULL, true);
      hpp::manipulation::Server* ms = new hpp::manipulation::Server (0, NULL, true);
      ms->setProblemSolverMap (bs->problemSolverMap());

      server_ = new CorbaServer (new HppServerProcess (bs, ms));
      server_->start();
      server_->waitForInitDone();
    }

    QString HppManipulationPlugin::name() const
    {
      return QString ("hpp-manipulation-corba plugin");
    }
  } // namespace plot
} // namespace hpp

Q_EXPORT_PLUGIN2 (hppmanipulationplugin, hpp::plot::HppManipulationPlugin)
