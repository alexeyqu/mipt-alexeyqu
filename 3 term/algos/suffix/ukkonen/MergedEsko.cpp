#define _ESKO__
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

    void clearGo();

  public:
    explicit Node (Index_t lastPosition, int parent, Index_t indexNode);
    explicit Node (Index_t startPosition, int parent, Index_t indexNode, Index_t length);

    int getNextNode (char c);
    void setNextNode (char c, int node);

    int getParent() {return parent_;}
    void setParent (int parent) {parent_ = parent;}

    int getSuffixLink() {return suffixLink_;}
    void setSuffixLink (int suffixLink) {suffixLink_ = suffixLink;}

    Index_t getLength() {return length_;}
    void setLength (Index_t length) {length_ = length;}

    Index_t getStartPosition() {return startPosition_;}
    void setStartPosition (Index_t startPosition) {startPosition_ = startPosition;}

    Index_t getIndexNode() {return indexNode_;}

    void setCurPrefixLength (Index_t newLength) {curPrefixLength_ = newLength;}

    Index_t getDistance();
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

  friend SuffixTree buildTree (std::string &str);
};

SuffixTree buildTree (std::string &str);

#endif


//-----------------------------------------------------------------------------
//-----NODE--------------------------------------------------------------------
//-----------------------------------------------------------------------------

void SuffixTree::Node::clearGo()
{
  for (char i = 0; i < ALPHABET_SIZE; i++)
  {
    go_[i] = -1;
  }
}

SuffixTree::Node::Node (Index_t lastPosition, int parent, Index_t indexNode) : \
  startPosition_ (lastPosition - 1), curPrefixLength_ (lastPosition), parent_ (parent), indexNode_ (indexNode), isLeaf_ (true)
{
  // std::cout << "AAA " << startPosition_ << ' ' << curPrefixLength_ << ' ' << lastPosition << std::endl;
  clearGo();
}

SuffixTree::Node::Node (Index_t startPosition, int parent, Index_t indexNode, Index_t length) : \
  startPosition_ (startPosition), parent_ (parent), indexNode_ (indexNode), length_ (length), isLeaf_ (false)
{
  clearGo();
}

int SuffixTree::Node::getNextNode (char c)
{
  return go_[c];
}

void SuffixTree::Node::setNextNode (char c, int node)
{
  go_[c] = node;
}

Index_t SuffixTree::Node::getDistance()
{
  // std::cout << ']' << startPosition_ << ' ' << curPrefixLength_ << std::endl;
  return (isLeaf_? (curPrefixLength_ - startPosition_) : length_);
}

//-----------------------------------------------------------------------------
//-----UKKONEN-----------------------------------------------------------------
//-----------------------------------------------------------------------------

SuffixTree::Ukkonen::Ukkonen (const std::string &str, SuffixTree &suffTree) : \
  str_ (str), suffTree_ (suffTree)
{
  Node empty_ = Node (0, EMPTY_INDEX, EMPTY_INDEX, 0);
  Node root_ = Node (0, EMPTY_INDEX, ROOT_INDEX, 1);

  suffTree_.tree_.push_back (empty_);
  suffTree_.tree_.push_back (root_);

  for (Index_t c = 0; c < ALPHABET_SIZE; c++)
  {
    suffTree_.tree_[EMPTY_INDEX].setNextNode (c, ROOT_INDEX);
  }

  suffTree_.tree_[ROOT_INDEX].setSuffixLink (EMPTY_INDEX);

  curOffset_ = 1;
  curPoint_ = 1;
  prevLinkPoint_ = 1;
  suffTree_.lastPosition_ = 0;
}

void SuffixTree::Ukkonen::appendChar (char c)
{
  lastChar_ = c;
  suffTree_.lastPosition_++;

  suffTree_.fixNodeLengths (suffTree_.lastPosition_);

  isEmptyPhase_ = false;

  while (1)
  {
    curPoint_ = adjustOffset();

    growTree();

    if (isEmptyPhase_) break;

    if (curLinkPoint_ &&
      prevLinkPoint_ != 1)
    {
      suffTree_.tree_[prevLinkPoint_].setSuffixLink (curLinkPoint_);
      prevLinkPoint_ = 1;
    }

    prevLinkPoint_ = curLinkPoint_;
    curLinkPoint_ = 0;
    curPoint_ = suffTree_.tree_[suffTree_.tree_[curPoint_].getParent()].getSuffixLink();

    if (curOffset_)
    {
      curPoint_ = suffTree_.tree_[curPoint_].getNextNode (str_[firstPosition_]);
    }
    else
    {
      curOffset_ = suffTree_.tree_[curPoint_].getLength();
    }
  }
}

Index_t SuffixTree::Ukkonen::adjustOffset()
{
  while (curOffset_ > suffTree_.tree_[curPoint_].getDistance())
  {
    curOffset_ -= suffTree_.tree_[curPoint_].getDistance();
    firstPosition_ += suffTree_.tree_[curPoint_].getDistance();

    curPoint_ = suffTree_.tree_[curPoint_].getNextNode (str_[firstPosition_]);
  }

  return curPoint_;
}

void SuffixTree::Ukkonen::growTree()
{
  if (curOffset_ == suffTree_.tree_[curPoint_].getDistance())
  {
    if (suffTree_.tree_[curPoint_].getNextNode (lastChar_) == -1)
    {
      curPoint_ = addLeaf();
      curOffset_ = 0;
    }
    else
    {
      curPoint_ = suffTree_.tree_[curPoint_].getNextNode (lastChar_);
      isEmptyPhase_ = true;
      curOffset_ = 1;
    }

    curLinkPoint_ = suffTree_.tree_[curPoint_].getParent();
  }
  else
  {
    if (str_[suffTree_.tree_[curPoint_].getStartPosition() + curOffset_] != lastChar_)
    {
      curPoint_ = split();
    }
    else
    {
      curOffset_++;
      isEmptyPhase_ = true;
    }
  }
}

Index_t SuffixTree::Ukkonen::addLeaf()
{
  Node newLeaf = Node (suffTree_.lastPosition_, curPoint_, suffTree_.tree_.size());
  suffTree_.tree_.push_back (newLeaf);

  int newLeafIndex = (int) suffTree_.tree_.size() - 1;

  suffTree_.tree_[curPoint_].setNextNode (lastChar_, newLeafIndex);

  curPoint_ = newLeafIndex;
  firstPosition_ = suffTree_.tree_[curPoint_].getStartPosition();

  return curPoint_;
}

Index_t SuffixTree::Ukkonen::split()
{
  Node midVertex = Node (suffTree_.tree_[curPoint_].getStartPosition(), suffTree_.tree_[curPoint_].getParent(), suffTree_.tree_.size(), curOffset_);
  suffTree_.tree_.push_back (midVertex);

  int midVertexIndex = (int) suffTree_.tree_.size() - 1;

  suffTree_.tree_[curPoint_].setStartPosition (suffTree_.tree_[curPoint_].getStartPosition() + curOffset_);
  suffTree_.tree_[curPoint_].setLength (suffTree_.tree_[curPoint_].getLength() - curOffset_);
  suffTree_.tree_[curPoint_].setParent(midVertexIndex);

  Node newLeaf = Node (suffTree_.lastPosition_, midVertexIndex, suffTree_.tree_.size());
  suffTree_.tree_.push_back (newLeaf);

  suffTree_.tree_[suffTree_.tree_[midVertexIndex].getParent()].setNextNode (str_[suffTree_.tree_[midVertexIndex].getStartPosition()], midVertexIndex);

  suffTree_.tree_[midVertexIndex].setNextNode (str_[suffTree_.tree_[curPoint_].getStartPosition()], curPoint_);
  suffTree_.tree_[midVertexIndex].setNextNode (str_[newLeaf.getStartPosition()], (int) newLeaf.getIndexNode());

  curLinkPoint_ = midVertexIndex;
  curPoint_ = midVertexIndex;
  firstPosition_ = suffTree_.tree_[midVertexIndex].getStartPosition();

  return curPoint_;
}

void SuffixTree::Ukkonen::printSubtree (Node *node)
{
  std::cout << node->getIndexNode() << ": ";

  for (char c = 0; c < ALPHABET_SIZE; c++)
  {
    int nextNode = node->getNextNode (c);
          // std::cout << "DBG: " << (char) (c + 'a') << ' ' << node->getNextNode (c) << std::endl;
    if (nextNode >= 2)
    {
      std::string str = assembleEdge (str_, suffTree_.tree_[nextNode].getStartPosition(), suffTree_.tree_[nextNode].getDistance());
      std::cout << " -" << str << "->" << suffTree_.tree_[nextNode].getIndexNode() << ' ';
    }
  }

  // return;

  std::cout << std::endl;

  for (char c = 0; c < ALPHABET_SIZE; c++)
  {
    int nextNode = node->getNextNode (c);

    if (nextNode >= 2)
    {
      this->printSubtree (&suffTree_.tree_[nextNode]);
    }
  }
}

std::string SuffixTree::Ukkonen::assembleEdge (const std::string &s, Index_t start, Index_t length)
{
  std::string str;

  for (Index_t i = 0; i < length; i++)
  {
    str += s[start + i] + 'a';
  }

  return str;
}

void SuffixTree::fixNodeLengths (Index_t newLength)
{
  for (size_t i = 0; i < tree_.size(); i++)
  {
    tree_[i].setCurPrefixLength (newLength);
  }
}

//-----------------------------------------------------------------------------
//-----SUFFIXTREE--------------------------------------------------------------
//-----------------------------------------------------------------------------

SuffixTree buildTree (std::string &str)
{
  SuffixTree suffTree;
  SuffixTree::Ukkonen builder (str, suffTree);

  for (Index_t i = 0; i < str.size(); i++)
  {
    builder.appendChar (str[i]);

#ifdef NOJUDGE
    builder.printSubtree (&suffTree.tree_[ROOT_INDEX]);
#endif
  }

  return suffTree;
}

long long SuffixTree::calcSum()
{
  long long sum = 0;

  Node *cur = &tree_[ROOT_INDEX + 1];
  Node *last = &tree_.back() + 1;

  for ( ; cur != last; cur++)
  {
    sum += cur->getDistance();
  }

  return sum;
}


std::vector <long long> getNumOfNonEmptySubstrings (int lenSubstring, std::string str)
{
  int problemSize = str.size();

  for (size_t i = 0; i < problemSize; i++)
  {
    str[i] -= 'a';
  }

  str += str;

  std::vector <long long> ans;

  ans.resize (problemSize);

  for (size_t i = 0; i < problemSize; i++)
  {
    std::string substring = str.substr (i, lenSubstring);

    SuffixTree tree = buildTree (substring);

    ans[i] = tree.calcSum();
  }

  return ans;
}

int main() // this part was designed to solve acm.timus.ru #1706 problem
{
  int key = 0;
  std::string str;

  std::cin >> key >> str;

  std::vector <long long> ans = getNumOfNonEmptySubstrings (key, str);

  for (size_t i = 0; i < ans.size(); i++)
  {
    std::cout << ans[i] << ' ';
  }

  return 0;
}

