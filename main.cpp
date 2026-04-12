#include <vector>


enum class OrderType {
  GoodTillCancel,
  FillAndKill,
};

enum class Side {
  Buy,
  Sell
};

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

struct LevelInfo {
  Price price_;
  Quantity quantity_;
}

using LevelInfos = std::vector<LevelInfo>;

class OrderBookLevelInfos {
public:
  OrderBookLevelInfos(const LevelInfos &bids, const LevelInfos &asks) 
    : bids_{ bids }, asks_{ asks } 
  {}


  const LevelInfos& getBids() const { return bids_; }
  const LevelInfos& getAsks() const { return asks_; }

private:
  LevelInfos bids_;
  LevelInfos asks_;
};

class Order {
public:
  Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity) 
    : OrderType_ { orderType }, orderId_{ orderId }, side_{ side }, 
      price_{ price }, initialQuantity_{ quantity }, remainingQuantity_{ quantity } 
  {}

  OrderId getOrderId() const { return orderId_; }
  Side getSide() const { return side_; }
  Price getPrice() const { return price_; }
  Quantity getInitialQuantity() const { return initialQuantity_; }
  Quantity getRemainingQuantity() const { return remainingQuantity_; }
  Quantity getFilledQuantity() const { return getInitialQuantity() - getRemainingQuantity(); }
  void fill(Quantity quantity) {
    if (quantity > getRemainingQuantity()) {
      throw std::logic_error(std::format("Order ({}) cannot be filled for more than its remaining quantity,", getOrderId());
    }
    remainingQuantity_ -= quantity;
  }

private:
  OrderType orderType_;
  OrderId orderId_;
  Side side_;
  Price price_;
  Quantity initialQuantity_;
  Quantity remainingQuantity_;
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>; // using a list gives an iterator that won't be invalidated as the list grows very large
                                               // TODO: optimize this away with a better data structure, i.e. vector, once the project is understood

int main() {

  return 0;
}
