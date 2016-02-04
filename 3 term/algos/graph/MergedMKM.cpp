#define MKM_
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

#ifndef _MKM_HEADER_
#define _MKM_HEADER_

#include <queue>

enum Direction { Forward, Backward };

class BFS
{
public:
  Network *network_;
  std::vector<uint32_t> level_;
  std::vector<bool> visited_;

  explicit BFS (Network *network) : network_ (network), level_ (network->getNOfVertices()), visited_ (network->getNOfVertices()) {};

  void run();

  bool PathReachedSink() const {return visited_[network_->getSink()];}

  bool isGoodVertex (uint32_t vertex) const;

  bool isNextLevel (uint32_t from, uint32_t to) const;
};

class MKM : public FlowInterface
{
private:
  BFS bfs_;
  std::vector<bool> visitedVertex_;
  std::vector<Capacity_t> outCapacity_, inCapacity_;
  std::vector<uint32_t> avaliableVertex_[2];

  void initBlockingFlow();

  void pushPreflow (uint32_t vertex, Capacity_t deltaFlow, Direction direction);

  void findBlockingFlow();

  Capacity_t getPotential (uint32_t vertex);

  void deleteVertex (uint32_t vertex);

public:

  explicit MKM (Network *network);

  ~MKM() {}

  virtual void run();
};

#endif


bool BFS::isGoodVertex (uint32_t vertex) const
{
  return ((visited_[vertex] &&
      (level_[vertex] < level_[network_->getSink()] ||
      vertex == network_->getSink())));
}

bool BFS::isNextLevel (uint32_t from, uint32_t to) const
{
  return (level_[from] + 1) == level_[to];
}

void BFS::run()
{
  std::fill (visited_.begin(), visited_.end(), 0);

  std::queue<uint32_t> vertexQueue;
  vertexQueue.push (network_->getSource());
  level_[network_->getSource()] = 0;
  visited_[network_->getSource()] = true;

  uint32_t nOfVertices = network_->getNOfVertices();

  while (!vertexQueue.empty())
  {
    uint32_t curVertex = vertexQueue.front();
    vertexQueue.pop();

    for (uint32_t curDestination = 0; curDestination < nOfVertices; curDestination++)
    {
      if (visited_[curDestination] ||
          network_->getTubes()(curVertex, curDestination).isSaturated())
        continue;

      level_[curDestination] = level_[curVertex] + 1;
      visited_[curDestination] = true;
      vertexQueue.push (curDestination);
    }
  }
}

MKM::MKM (Network *network): FlowInterface (network), bfs_ (network), visitedVertex_ (network_->getNOfVertices()),
    outCapacity_ (network_->getNOfVertices()), inCapacity_ (network_->getNOfVertices())
{
  avaliableVertex_[0] = std::vector<uint32_t> (network_->getNOfVertices()); 
  avaliableVertex_[1] = std::vector<uint32_t> (network_->getNOfVertices());
}

void MKM::initBlockingFlow()
{
  fill (avaliableVertex_[0].begin(), avaliableVertex_[0].end(), 0);
  fill (avaliableVertex_[1].begin(), avaliableVertex_[1].end(), 0);
  fill (outCapacity_.begin(), outCapacity_.end(), 0);
  fill (inCapacity_.begin(), inCapacity_.end(), 0);

  uint32_t nOfVertices = network_->getNOfVertices();

  for (uint32_t vertex = 0; vertex < nOfVertices; vertex++)
  {
    visitedVertex_[vertex] = !bfs_.isGoodVertex (vertex);
  }

  for (uint32_t vertex = 0; vertex < nOfVertices; vertex++)
  {
    if (visitedVertex_[vertex])
      continue;

    for (uint32_t curDestination = 0; curDestination < nOfVertices; curDestination++)
    {
      if (bfs_.isNextLevel (vertex, curDestination) && !visitedVertex_[curDestination])
      {
        Capacity_t capacity = network_->getTubes()(vertex, curDestination).getResidualCapacity();
        outCapacity_[vertex] += capacity;
        inCapacity_[curDestination] += capacity;
      }
    }
  }
}

void MKM::deleteVertex (uint32_t vertex)
{
  uint32_t nOfVertices = network_->getNOfVertices();

  visitedVertex_[vertex] = true;

  for (uint32_t curDestination = 0; curDestination < nOfVertices; curDestination++)
  {
    if (visitedVertex_[curDestination])
      continue;

    if (bfs_.isNextLevel (vertex, curDestination))
      inCapacity_[curDestination] -= network_->getTubes()(vertex, curDestination).getResidualCapacity();

    if (bfs_.isNextLevel (curDestination, vertex))
      outCapacity_[curDestination] -= network_->getTubes()(curDestination, vertex).getResidualCapacity();
  }
}

void MKM::pushPreflow (uint32_t vertex, Capacity_t flow, Direction direction)
{
  if (flow == 0 ||
      (direction == Forward && vertex == network_->getSink()) ||
      (direction == Backward && vertex == network_->getSource()))
    return;

  uint32_t nOfVertices = network_->getNOfVertices();

  for (uint32_t &nextVertex = avaliableVertex_[direction][vertex]; nextVertex < nOfVertices; nextVertex++)
  {
    uint32_t from = vertex, to = nextVertex;

    if (direction == Backward) std::swap (from, to);

    if (visitedVertex_[nextVertex] ||
        !bfs_.isNextLevel (from, to) ||
        network_->getTubes()(from, to).isSaturated())
        continue;

    Capacity_t delta = std::min (flow, network_->getTubes()(from, to).getResidualCapacity());
    outCapacity_[from] -= delta;
    inCapacity_[to] -= delta;

    network_->getTubes()(from, to).push (delta);

    flow -= delta;

    pushPreflow (nextVertex, delta, direction);

    if (flow == 0) break;
  }

  if (flow != 0)
  {
    network_->dumpNetwork();
    throw;
  }
}

Capacity_t MKM::getPotential (uint32_t vertex)
{
  if (vertex == network_->getSource()) return outCapacity_[vertex];
  if (vertex == network_->getSink()) return inCapacity_[vertex];
  return std::min (outCapacity_[vertex], inCapacity_[vertex]);
}

void MKM::findBlockingFlow()
{
  initBlockingFlow();

  uint32_t nOfVertices = network_->getNOfVertices();

  while (1)
  {
    uint32_t curVertex = nOfVertices;

    for (uint32_t tryVertex = 0; tryVertex < nOfVertices; tryVertex++)
    {
      if (visitedVertex_[tryVertex]) continue;

      if (curVertex == nOfVertices || 
          getPotential (tryVertex) < getPotential (curVertex))
        curVertex = tryVertex;
    }

    if (curVertex == nOfVertices)
        break;

    Capacity_t deltaFlow = getPotential (curVertex);
    network_->addNetworkFlow (deltaFlow);

    pushPreflow (curVertex, deltaFlow, Forward);
    pushPreflow (curVertex, deltaFlow, Backward);

    deleteVertex (curVertex);
  }
}

void MKM::run()
{
  while (1)
  {
    // std::cout << "DNA1" << std::endl;
    bfs_.run();
    // std::cout << "DNA" << std::endl;

    if (!bfs_.PathReachedSink()) break;

    findBlockingFlow();
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

