#ifndef _NETWORK_HEADER
#define _NETWORK_HEADER

#include "graph.h"

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