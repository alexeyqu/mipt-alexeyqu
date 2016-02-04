#ifndef _PUSHRELABEL_HEADER
#define _PUSHRELABEL_HEADER

#include "network.h"

class PushRelabel : public FlowInterface
{
private:
  std::vector<uint32_t> height_;
  std::vector<uint32_t> usedHeight_;
  std::vector<Capacity_t> excess_;
  std::vector<uint32_t> pointer_;

  void initPreflow();

  bool canPush (uint32_t from, uint32_t to);

  void push (uint32_t from, uint32_t to, Capacity_t deltaFlow);

  bool isSaturated (uint32_t vertex);

  void relabel (uint32_t vertex);

  void discharge (uint32_t vertex);

  void gap (uint32_t vertex);

public:
  explicit PushRelabel (Network *network);

  ~PushRelabel() {};

  virtual void run();
};

#endif