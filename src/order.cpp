#include <format>
#include <stdexcept>
#include "order.hpp"

namespace orderbook {

void Order::fill(Quantity quantity) {
  if (quantity > getRemainingQuantity())
    throw std::logic_error(
      std::format("Order ({}) cannot be filled for more than its remaining quantity.", getOrderId())
    ); 

  remainingQuantity_ -= quantity;
}

}
