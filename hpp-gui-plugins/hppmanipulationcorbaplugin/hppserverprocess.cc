#include "hppserverprocess.hh"

namespace hpp {
  namespace plot {
    HppServerProcess::HppServerProcess ( hpp::corbaServer::Server* basic,
        hpp::wholebodyStep::Server* wbs, hpp::manipulation::Server* manip)
      : basic_ (basic)
        , wbs_ (wbs)
        , manip_ (manip)
    {}

    HppServerProcess::~HppServerProcess()
    {
      delete basic_;
      delete wbs_;
      delete manip_;
    }

    void HppServerProcess::init()
    {
      basic_->startCorbaServer ();
      wbs_->startCorbaServer ("hpp", "corbaserver", "wholebodyStep", "problem");
      manip_->startCorbaServer ("hpp", "corbaserver", "manipulation");
      emit done ();
      ServerProcess::init();
    }

    void HppServerProcess::processRequest(bool loop)
    {
      basic_->processRequest (loop);
      emit done();
    }
  } // namespace plot
} // namespace hpp
