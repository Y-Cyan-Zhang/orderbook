#pragma once

#include <cstdint>

namespace orderbook {

using Price = std::int32_t;
using Quantity = std::int32_t;
using OrderId = std::uint64_t;

enum class OrderType {
  GoodTillCancel,
  ImmediateOrCancel,
  FillOrKill,
};

enum class Side {
  Buy,
  Sell,
};

}
