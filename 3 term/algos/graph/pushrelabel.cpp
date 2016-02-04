#include "pushrelabel.h"

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