#pragma once

#include "types.hpp"
namespace orderbook {

class Order {
public:
  Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
    : orderType_{ orderType }, orderId_{ orderId }, side_{ side }, 
      price_{ price }, initialQuantity_{ quantity }, remainingQuantity_{ quantity }
  {}

  OrderId getOrderId() const { return orderId_; }
  OrderType getOrderType const { orderType_; }
  Side getSide() const { return side_ };
  Price getPrice() const { return price_ };
  Quantity getInitialQuantity() const { return initialQuantity_; }
  Quantity getRemainingQuantity() const { return remainingQuantity_; }
  Quantity getFilledQuantity() const { return getInitialQuantity() - getRemainingQuantity(); }
  bool isFilled() { return getRemainingQuantity() == 0; }
  void fill(Quantity quantity); // TODO implementation in .cpp file

private: 
  OrderType orderType_;
  OrderId orderId_;
  Side side_;
  Price price_;
  Quantity initialQuantity_;
  Quantity remainingQuantity_;
};

}
