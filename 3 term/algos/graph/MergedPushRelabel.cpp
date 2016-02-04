#define PUSH_RELABEL_
#ifndef _GRAPH_HEADER
#define _GRAPH_HEADER

#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <assert.h>

typedef int64_t Capacity_t;

class Edge
{
private:
  uint32_t originId_, destinationId_;
  Capacity_t capacity_;

public:
  explicit Edge (uint32_t origin, uint32_t destinationId, Capacity_t capacity);

  uint32_t getOriginId() const {return originId_;}
  uint32_t getDestinationId() const {return destinationId_;}

  Capacity_t getCapacity() const {return capacity_;}
};

class Graph
{
private:
  std::vector<Edge> edges_;

protected:
  uint32_t nOfVertices_, nOfEdges_;

public:
  explicit Graph();

  uint32_t getNOfVertices() const {return nOfVertices_;}
  uint32_t getNOfEdges() const {return nOfEdges_;}
  const std::vector<Edge> &getEdges() const {return edges_;}

  void addEdge (uint32_t originId, uint32_t destinationId, Capacity_t capacity);
  friend std::istream &operator>> (std::istream &in, Graph &graph);
  friend std::ostream &operator<< (std::ostream &out, Graph &graph);
  // void dotPrint (std::ostream &out); TODO .dot graph print
};

#endif


Edge::Edge (uint32_t originId, uint32_t destinationId, Capacity_t capacity)
{
  originId_ = originId;
  destinationId_ = destinationId;
  capacity_ = capacity;
}

Graph::Graph()
{
  nOfVertices_ = 0;
  nOfEdges_ = 0;
  edges_.clear();
}

void Graph::addEdge (uint32_t originId, uint32_t destinationId, Capacity_t capacity)
{
  nOfVertices_ = std::max (std::max (originId, destinationId), nOfVertices_);
  nOfEdges_++;
  Edge new_edge (originId - 1, destinationId - 1, capacity);
  edges_.push_back (new_edge);
}

std::ostream &operator<< (std::ostream &out, Graph &graph)
{
  out << "Graph: " << graph.nOfVertices_ << " vertices, " << graph.nOfEdges_ << " edges.\n";

  for (uint32_t i = 0; i < graph.edges_.size(); i++)
  {
    out << graph.edges_[i].getOriginId() << " -> " << graph.edges_[i].getDestinationId() << " : ";
    out << graph.edges_[i].getCapacity() << '\n';
  }

  return out;
}

std::istream &operator>> (std::istream &in, Graph &graph)
{
  uint32_t nOfEdges = 0;

  in >> graph.nOfVertices_ >> nOfEdges;

  for (uint32_t i = 0; i < nOfEdges; i++)
  {
    uint32_t origin, destination, capacity;
    in >> origin >> destination >> capacity;

    graph.addEdge (origin, destination, capacity);
  }

  return in;
}

// void Graph::dotPrint (std::ostream &out) TODO dot graph print, why not.
// {
//   out << "digraph graph\n{\n";

//   for (uint32_t i = 0; i < nOfVertices_; i++)
//   {
//     out << i << "[label = " << i << "]\n";
//   }

//   for (uint32_t i = 0; i < nOfEdges_; i++)
//   {
//     out << edges_[i].getOriginId() << " -> " << edges_[i].getDestinationId() << '\n';
//   }

//   out << "}\n";
// }

#ifndef _NETWORK_HEADER
#define _NETWORK_HEADER


class Network;

class Tube
{
private:
  Capacity_t capacity_;
  Tube *backwardTube_;
  Capacity_t flow_;

public:

  explicit Tube() {flow_ = 0; capacity_ = 0; backwardTube_ = NULL;}

  Capacity_t getFlow() const {return flow_;}
  Capacity_t getCapacity() const {return capacity_;}
  Tube *getBackwardTube() const {return backwardTube_;}

  void setFlow (Capacity_t flow) {flow_ = flow;}
  void setCapacity (Capacity_t capacity) {capacity_ = capacity;}
  void setBackwardTube (Tube *backwardTube);

  Capacity_t getResidualCapacity() const;
  bool isSaturated() const;
  void push (Capacity_t deltaFlow);
};

class TubesMatrix
{
private:
  uint32_t nOfVertices_;
  Tube *tubes_;

public:
  explicit TubesMatrix (uint32_t size);

  void DeleteMatrix();

  Tube& operator() (int row, int column) const;
};

class Network
{
private:
  uint32_t source_, sink_;
  uint32_t nOfVertices_;
  Capacity_t networkFlow_;
  // std::vector <std::vector <Tube> > tubes_;
  // Tube **tubes_;
  TubesMatrix tubes_;

public:
  explicit Network (Graph &graph);

  ~Network();

  Capacity_t getNetworkFlow() const {return networkFlow_;}
  uint32_t getNOfVertices() const {return nOfVertices_;}
  uint32_t getSource() const {return source_;}
  uint32_t getSink() const {return sink_;}
  TubesMatrix getTubes() const {return tubes_;}
  // std::vector <std::vector <Tube> > getTubes() const {return tubes_;}
  // Tube **getTubes() const {return tubes_;}

  void setNetworkFlow (Capacity_t networkFlow) {networkFlow_ = networkFlow;}
  void addNetworkFlow (Capacity_t deltaFlow) {networkFlow_ += deltaFlow;}

  void exportEdge (Edge edge);
  void push (uint32_t from, uint32_t to, Capacity_t deltaFlow);

  friend std::ostream &operator<< (std::ostream &out, Network &network); 

  void dumpNetwork();
};

class FlowInterface
{
public:
  Network *network_;

  FlowInterface (Network *network): network_(network) {};

  virtual ~FlowInterface() {};

  virtual void run() = 0;
};

#endif


void Tube::setBackwardTube (Tube *backwardTube)
{
  if (backwardTube == NULL) throw;
  backwardTube_ = backwardTube;
}

Capacity_t Tube::getResidualCapacity() const
{
  return (capacity_ - flow_);
}

bool Tube::isSaturated() const
{
  return (capacity_ == flow_);
}

void Tube::push (Capacity_t deltaFlow)
{
  backwardTube_->flow_ -= deltaFlow;
  flow_ += deltaFlow;
  assert (flow_ <= capacity_);
}

TubesMatrix::TubesMatrix (uint32_t nOfVertices)
{
  nOfVertices_ = nOfVertices;

  tubes_ = new Tube[nOfVertices * nOfVertices];
}

void TubesMatrix::DeleteMatrix()
{
  delete[] tubes_;
}

Tube& TubesMatrix::operator() (int row, int column) const
{
  return tubes_[row * nOfVertices_ + column];
}

Network::Network (Graph &graph) : tubes_ (TubesMatrix (graph.getNOfVertices()))
{
  networkFlow_ = 0;
  nOfVertices_ = graph.getNOfVertices();
  source_ = 0;
  sink_ = graph.getNOfVertices() - 1;

  for (uint32_t i = 0; i < graph.getEdges().size(); i++)
  {
    exportEdge (graph.getEdges()[i]);
  }

  for (uint32_t i = 0; i < nOfVertices_; i++)
  {
    for (uint32_t j = 0; j < nOfVertices_; j++)
    {
      tubes_(i, j).setBackwardTube(&tubes_(j, i));
    }
  }
}

Network::~Network()
{
  tubes_.DeleteMatrix();
}

void Network::exportEdge (Edge edge)
{
  Tube& tube = tubes_(edge.getOriginId(), edge.getDestinationId());
  tube.setCapacity(edge.getCapacity() + tube.getCapacity());
}

void Network::push (uint32_t from, uint32_t to, Capacity_t deltaFlow)
{
  tubes_(from, to).push (deltaFlow);
}

std::ostream &operator<< (std::ostream &out, Network &network)
{
  out << "Network: " << network.nOfVertices_ << " vertices\n";

  for (uint32_t i = 0; i < network.nOfVertices_; i++)
  {
    for (uint32_t j = 0; j < network.nOfVertices_; j++)
    {
      out << i + 1 << " -> " << j + 1 << " : " << network.tubes_(i, j).getCapacity() << ' ' << network.tubes_(i, j).getFlow() << '\n';
    }
  }

  return out;
}

void Network::dumpNetwork()
{
  std::cout << "Network dump\n";

  std::cout << source_ << " -> " << sink_ << '\n';

  std::cout << nOfVertices_ << '\n';

  std::cout << networkFlow_ << '\n';

  for (uint32_t i = 0; i < nOfVertices_; i++)
  {
    for (uint32_t j = 0; j < nOfVertices_; j++)
    {
      // out << network.tubes_[i][j].getOriginId() << " -> " network.tubes_[i][j].getDestinationId() << " : ";
      std::cout << i << " -> " << j << " : " << tubes_(i, j).getCapacity() << ' ' << tubes_(i, j).getFlow() << '\n';
      // std::cout << tubes_[i][j].backwardTube_ << ' ' << tubes_[j][i].backwardTube_ << "\n\n";
    }
  }
}

#ifndef _PUSHRELABEL_HEADER
#define _PUSHRELABEL_HEADER


class PushRelabel : public FlowInterface
{
private:
  std::vector<uint32_t> height_;
  std::vector<uint32_t> usedHeight_;
  std::vector<Capacity_t> excess_;
  std::vector<uint32_t> pointer_;

  void initPreflow();

  bool canPush (uint32_t from, uint32_t to);

  void push (uint32_t from, uint32_t to, Capacity_t deltaFlow);

  bool isSaturated (uint32_t vertex);

  void relabel (uint32_t vertex);

  void discharge (uint32_t vertex);

  void gap (uint32_t vertex);

public:
  explicit PushRelabel (Network *network);

  ~PushRelabel() {};

  virtual void run();
};

#endif


PushRelabel::PushRelabel (Network *network) : FlowInterface (network), \
  height_ (network_->getNOfVertices()), usedHeight_ (network_->getNOfVertices()), \
  excess_ (network_->getNOfVertices()), pointer_ (network_->getNOfVertices()) {}

void PushRelabel::initPreflow()
{
  uint32_t source = network_->getSource();
  uint32_t nOfVertices = network_->getNOfVertices();

  height_[source] = nOfVertices;

  usedHeight_[nOfVertices] = 1;
  usedHeight_[0] = nOfVertices - 1;

  for (uint32_t curVertex = 0; curVertex < nOfVertices; curVertex++)
  {
    Capacity_t deltaFlow = network_->getTubes()(source, curVertex).getResidualCapacity();
    push (source, curVertex, deltaFlow);
  }
}

bool PushRelabel::canPush (uint32_t from, uint32_t to)
{
  return height_[from] == height_[to] + 1;
}

void PushRelabel::push (uint32_t from, uint32_t to, Capacity_t deltaFlow)
{
  excess_[to] += deltaFlow;
  excess_[from] -= deltaFlow;
  network_->push (from, to, deltaFlow);
}

bool PushRelabel::isSaturated (uint32_t vertex)
{
  return ((vertex != network_->getSource() &&
      vertex != network_->getSink() &&
      excess_[vertex] > 0));
}

void PushRelabel::relabel (uint32_t vertex)
{
  if (vertex == network_->getSource() ||
      excess_[vertex] == 0) return;

  uint32_t nOfVertices = network_->getNOfVertices();

  usedHeight_[height_[vertex]]--;

  uint32_t minHeight = 2 * nOfVertices; // maximum height possible (@see Cormen)
  uint32_t oldHeight = height_[vertex];

  for (uint32_t curVertex = 0; curVertex < nOfVertices; curVertex++)
  {
    if (network_->getTubes()(vertex, curVertex).isSaturated()) continue;

    minHeight = std::min (minHeight, height_[curVertex] + 1);
  }

  height_[vertex] = minHeight;
  usedHeight_[height_[vertex]]++;

  if (usedHeight_[oldHeight] == 0) gap (oldHeight);
}

void PushRelabel::gap (uint32_t gapHeight)
{
  uint32_t nOfVertices = network_->getNOfVertices();

  for (uint32_t curVertex = 0; curVertex < nOfVertices; curVertex++)
  {
    if (height_[curVertex] > gapHeight &&
        height_[curVertex] < nOfVertices) 
      height_[curVertex] = nOfVertices;
  }
}

void PushRelabel::discharge (uint32_t vertex)
{
  while (excess_[vertex] > 0)
  {
    uint32_t &toVertex = pointer_[vertex];

    if (toVertex == network_->getNOfVertices())
    {
      relabel (vertex);
      toVertex = 0;
      continue;
    }

    if (canPush (vertex, toVertex))
    {
      uint32_t deltaFlow = std::min (excess_[vertex], network_->getTubes()(vertex, toVertex).getResidualCapacity());
      push (vertex, toVertex, deltaFlow);
    }

    toVertex++;
  }
}

void PushRelabel::run()
{
  initPreflow();
  bool discharged = true;

  uint32_t nOfVertices = network_->getNOfVertices();
  uint32_t source = network_->getSource();

  while (discharged)
  {
    discharged = false;

    for (uint32_t curVertex = 0; curVertex < nOfVertices; curVertex++)
    {
      if (isSaturated (curVertex))
      {
        discharge (curVertex);
        discharged = true;
      }
    }
  }

  for (uint32_t curVertex = 0; curVertex < nOfVertices; curVertex++)
  {
    if (curVertex != source)
    {
      network_->addNetworkFlow (network_->getTubes()(source, curVertex).getFlow());
    }
  }
}


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

