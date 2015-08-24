#ifndef HPPSERVERPROCESS_HH
#define HPPSERVERPROCESS_HH

#include <hpp/corbaserver/server.hh>
#include <hpp/corbaserver/wholebody-step/server.hh>
#include <hpp/corbaserver/manipulation/server.hh>

#include <hpp/gui/omniorb/omniorbthread.h>

class HppServerProcess : public ServerProcess
{
  Q_OBJECT

public:
  HppServerProcess ( hpp::corbaServer::Server* basic,
      hpp::wholebodyStep::Server* wbs, hpp::manipulation::Server* manip);

  ~HppServerProcess ();

public slots:
  void init ();
  void processRequest (bool loop);

private:
  hpp::corbaServer::Server*   basic_;
  hpp::wholebodyStep::Server* wbs_;
  hpp::manipulation::Server*  manip_;
};

#endif // HPPSERVERPROCESS_HH
