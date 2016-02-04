#include "graph.h"

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