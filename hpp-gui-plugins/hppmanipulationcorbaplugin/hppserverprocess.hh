#ifndef HPPSERVERPROCESS_HH
#define HPPSERVERPROCESS_HH

#include <hpp/corbaserver/server.hh>
#include <hpp/corbaserver/manipulation/server.hh>

#include <gepetto/gui/omniorb/omniorbthread.hh>

namespace hpp {
  namespace plot {
    class HppServerProcess : public gepetto::gui::ServerProcess
    {
      Q_OBJECT

      public:
        HppServerProcess ( hpp::corbaServer::Server* basic,
                           hpp::manipulation::Server* manip);

        ~HppServerProcess ();

        public slots:
          void init ();
        void processRequest (bool loop);

      private:
        hpp::corbaServer::Server*   basic_;
        hpp::manipulation::Server*  manip_;
    };
  } // namespace plot
} // namespace hpp

#endif // HPPSERVERPROCESS_HH
