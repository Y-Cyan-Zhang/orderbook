#pragma once

#include <cstdint>

namespace orderbook {

enum class OrderType {
  GoodTillCancel,
  FillOrKill,
  ImmediateOrCancel,
};

enum class Side {
  Buy,
  Sell
};

}
