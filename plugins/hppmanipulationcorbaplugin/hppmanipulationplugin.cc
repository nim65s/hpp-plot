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

#include "hppmanipulationplugin.hh"

#include <hpp/manipulation/problem-solver.hh>

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

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
Q_EXPORT_PLUGIN2 (hppmanipulationplugin, hpp::plot::HppManipulationPlugin)
#endif
