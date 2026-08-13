#pragma once

#include <cstdint>

enum class OrderType {
  GoodTillCancel,
  FillOrKill,
  ImmediateOrCancel,
};

enum class Side {
  Buy,
  Sell
};


class Order {
public:
  Order(OrderType orderType, uint32_t orderId, Side side, float price, float quantity);
private:
};
