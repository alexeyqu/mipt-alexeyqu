#include "network.h"

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