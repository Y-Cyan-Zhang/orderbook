#pragma once

#include <stdexcept>
#include <format>
#include "types.hpp"

namespace orderbook {

class Order {
public:
  Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
    : orderType_{ orderType }, orderId_{ orderId }, side_{ side }, price_{ price }, 
      initialQuantity_{ quantity }, remainingQuantity_{ quantity }
  {}

  OrderId getOrderId() const { return orderId_; }
  Side getSide() const { return side_; }
  Price getPrice() const { return price_; }
  OrderType getOrderType() const { return orderType_; }
  Quantity getInitialQuantity() const { return initialQuantity_; }
  Quantity getRemainingQuantity() const { return remainingQuantity_; }
  Quantity getFilledQuantity() const { return getInitialQuantity() - getRemainingQuantity(); }
  bool isFilled() const { return getRemainingQuantity() == 0; }
  void fill(Quantity quantity);

private:
  OrderType orderType_;
  OrderId orderId_;
  Side side_;
  Price price_;
  Quantity initialQuantity_;
  Quantity remainingQuantity_;
};

/* Using shared_ptr to allow reference semnatics.
 * Need reference semantics since Order objects can be stored in
 */
using OrderPointer = std::shared_ptr<Order>; // using for reference semantics
                                             // Order objects can be stored in
                                             // Order objects can be stored in
                                             // Order objects can be stored in

}
