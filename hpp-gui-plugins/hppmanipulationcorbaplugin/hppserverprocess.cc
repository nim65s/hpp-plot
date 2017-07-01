#include "hppserverprocess.hh"

namespace hpp {
  namespace plot {
    HppServerProcess::HppServerProcess ( hpp::corbaServer::Server* basic,
                                         hpp::manipulation::Server* manip)
      : basic_ (basic)
        , manip_ (manip)
    {}

    HppServerProcess::~HppServerProcess()
    {
      delete basic_;
      delete manip_;
    }

    void HppServerProcess::init()
    {
      basic_->startCorbaServer ();
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
