#include "graph.h"
#include "network.h"
#include "mkm.h"
#include "pushrelabel.h"

int main()
{
  Graph myGraph;

  std::cin >> myGraph;

  Network myNetwork (myGraph);

  #ifdef PUSH_RELABEL_
    FlowInterface *flowInterface = new PushRelabel(&myNetwork);
    #else
      #ifdef MKM_
      FlowInterface *flowInterface = new MKM(&myNetwork);
      #else
        #error "You should define either PUSH_RELABEL_ or MKM_";
        return 1;
    #endif
  #endif

  try
  {
    flowInterface->run();
  }
  catch(...)
  {
    std::cout << "EXCEPTION HAPPENED" << std::endl;
    delete[] flowInterface;
    return 2;
  }

  std::cout << flowInterface->network_->getNetworkFlow() << std::endl;

  for (size_t i = 0; i < myGraph.getEdges().size(); i++)
  {
    Edge edge = myGraph.getEdges()[i];
    Capacity_t flow = myNetwork.getTubes()(edge.getOriginId(), edge.getDestinationId()).getFlow();
    Capacity_t ans = std::max(Capacity_t (0), std::min(flow, edge.getCapacity()));
    myNetwork.getTubes()(myGraph.getEdges()[i].getDestinationId(), myGraph.getEdges()[i].getOriginId()).push (ans);

    std::cout << ans << '\n';
  }

  delete flowInterface;

  return 0;
}