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