#pragma once
#include "types.hpp"
namespace orderbook {

class Order {
public:
  Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
    : orderType_{ orderType }, orderId_{ orderId }, side_{ side }, 
      price_{ price }, initialQuantity_{ quantity }, remainingQuantity_{ quantity }
  {}

private: 
  OrderType orderType_;
  OrderId orderId_;
  Side side_;
  Price price_;
  Quantity initialQuantity_;
  Quantity remainingQuantity_;
};

}
