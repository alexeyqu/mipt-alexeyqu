#ifndef _UKKONEN_LIB__
#define _UKKONEN_LIB__

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include <vector>
#include <string>
#include <map>

#define ALPHABET_SIZE 26
#define EMPTY_INDEX 0
#define ROOT_INDEX 1

typedef uint32_t Index_t;

class SuffixTree
{
private:

  class Node
  {
  private:
    int go_[ALPHABET_SIZE];
    Index_t parent_, suffixLink_;
    Index_t indexNode_, length_;
    Index_t startPosition_; // index of first char in cur suffix
    Index_t curPrefixLength_;
    bool isLeaf_;

    void clearGoArray();

  public:
    explicit Node (Index_t lastPosition, Index_t parent, Index_t indexNode);
    explicit Node (Index_t startPosition, Index_t parent, Index_t indexNode, Index_t length);

    Index_t getNextNode (char c) const;
    void setNextNode (char c, Index_t node);

    Index_t getParent() const {return parent_;}
    void setParent (int parent) {parent_ = parent;}

    Index_t getSuffixLink() const {return suffixLink_;}
    void setSuffixLink (Index_t suffixLink) {suffixLink_ = suffixLink;}

    Index_t getLength() const {return length_;}
    void setLength (Index_t length) {length_ = length;}

    Index_t getStartPosition() const {return startPosition_;}
    void setStartPosition (Index_t startPosition) {startPosition_ = startPosition;}

    Index_t getIndexNode() const {return indexNode_;}

    void setCurPrefixLength (Index_t newLength) {curPrefixLength_ = newLength;}

    Index_t getDistance() const;
  };

  class Ukkonen
  {
  private:
    Index_t firstPosition_, curOffset_;
    Index_t curPoint_, curLinkPoint_, prevLinkPoint_;
    Index_t lastChar_;
    bool isEmptyPhase_;

    const std::string &str_;
    SuffixTree &suffTree_;

  public:
    explicit Ukkonen (const std::string &str, SuffixTree &suffTree);

    void appendChar (char c);
    void growTree();
    Index_t addLeaf();
    Index_t split();
    Index_t adjustOffset();

    void printSubtree (Node *node);
    std::string assembleEdge (const std::string &s, Index_t start, Index_t length);
  };

  std::vector <Node> tree_;
  Index_t lastPosition_;

public:
  Index_t root_, empty_;

  explicit SuffixTree() : lastPosition_ (0), root_ (0), empty_ (0) {};

  void fixNodeLengths (Index_t newLength);

  ~SuffixTree() {};

  long long calcSum();

  friend SuffixTree buildTree (const std::string &str);
};

SuffixTree buildTree (const std::string &str);

#endif