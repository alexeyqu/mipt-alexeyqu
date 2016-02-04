#include "esko.h"

//-----------------------------------------------------------------------------
//-----NODE--------------------------------------------------------------------
//-----------------------------------------------------------------------------

void SuffixTree::Node::clearGoArray()
{
  for (char i = 0; i < ALPHABET_SIZE; i++)
  {
    go_[i] = -1;
  }
}

SuffixTree::Node::Node (Index_t lastPosition, Index_t parent, Index_t indexNode) : \
  startPosition_ (lastPosition - 1), curPrefixLength_ (lastPosition), parent_ (parent), indexNode_ (indexNode), isLeaf_ (true)
{
  clearGoArray();
}

SuffixTree::Node::Node (Index_t startPosition, Index_t parent, Index_t indexNode, Index_t length) : \
  startPosition_ (startPosition), parent_ (parent), indexNode_ (indexNode), length_ (length), isLeaf_ (false)
{
  clearGoArray();
}

int SuffixTree::Node::getNextNode (char c) const
{
  return go_[c];
}

void SuffixTree::Node::setNextNode (char c, int node)
{
  go_[c] = node;
}

Index_t SuffixTree::Node::getDistance() const
{
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

SuffixTree buildTree (const std::string &str)
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