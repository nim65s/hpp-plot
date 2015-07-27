#ifndef HPP_PLOT_HPP_MANIPULATION_GRAPH_H
#define HPP_PLOT_HPP_MANIPULATION_GRAPH_H

#include <hpp/plot/graph-widget.hh>
#include <hpp/corbaserver/manipulation/client.hh>


namespace hpp {
  namespace plot {
    class HppManipulationGraphWidget : public GraphWidget
    {
    public:
      HppManipulationGraphWidget (QWidget* parent);

    protected:
      void fillScene ();

    private:
      corbaServer::manipulation::Client client_;
    };
  }
}

#endif // HPP_PLOT_HPP_MANIPULATION_GRAPH_H
