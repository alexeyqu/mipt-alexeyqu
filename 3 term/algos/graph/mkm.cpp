#include "mkm.h"

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