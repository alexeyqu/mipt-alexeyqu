#ifndef _MKM_HEADER_
#define _MKM_HEADER_

#include <queue>
#include "network.h"

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