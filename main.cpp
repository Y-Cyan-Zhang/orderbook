#include <iostream>
#include <numeric>
#include <vector>
#include <format>
#include <memory>
#include <list>
#include <map>
#include <unordered_map>


enum class OrderType {
  GoodTillCancel,
  FillOrKill
};

enum class Side {
  Buy,
  Sell
};

using Price = std::int32_t;
using Quantity = std::int32_t;
using OrderId = std::uint64_t;

struct LevelInfo {
  Price price_;
  Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderbookLevelInfos {
public:
  OrderbookLevelInfos(const LevelInfos& bids, const LevelInfos& asks)
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
  void Fill(Quantity quantity) {
    if (quantity > getRemainingQuantity())
      throw std::logic_error(
        std::format("Order ({}) cannot be filled for more than its remaining quantity.", getOrderId())
      );

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

using OrderPointer = std::shared_ptr<Order>;  // using to allow reference semantics, 
                                              // since on Order object can be stored in multiple data strcutures
                                              // (e.g. in Orders dict, and bid/ask dict)

using OrderPointers = std::list<OrderPointer>; // using a std::list since list iterators don't get invalidated as you insert/remove
                                               // will be inserting/removing a lot
                                               // TODO: can optimize this with a more efficient data structure, e.g. maybe a vector, more cache friendly

class OrderModify {
/* 
 * Abstraction for an Order to be modified. 
 * A modify will just be an order and cancel order.
 */
public:
  OrderModify(OrderId orderId, Side side, Price price, Quantity quantity)
    : orderId_{ orderId }, side_{ side }, price_{ price }, quantity_{ quantity }
  {}

  OrderId getOrderId() const { return orderId_; }
  Price getPrice() const { return price_; }
  Side getSide() const { return side_; }
  Quantity getQuantity() const { return quantity_; }

  OrderPointer toOrderPointer(OrderType orderType) const {
    return std::make_shared<Order>(orderType, getOrderId(), getSide(), getPrice(), getQuantity());
  }
private:
  OrderId orderId_;
  Price price_;
  Side side_;
  Quantity quantity_;
};

struct TradeInfo {
  OrderId orderId_;
  Price price_;
  Quantity quantity_;
};

class Trade {
public:
  Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade)
    : bidTrade_{ bidTrade }, askTrade_{ askTrade }
  {}

  const TradeInfo& getBidTrade() const { return bidTrade_; }
  const TradeInfo& getAskTrade() const { return askTrade_; }

private:
  TradeInfo bidTrade_;
  TradeInfo askTrade_;
};

using Trades = std::vector<Trade>;

class Orderbook {
public:
  Trades addOrder(OrderPointer order) {
    if (orders_.contains(order->getOrderId()))
      return { };

    if (order->getOrderType() == OrderType::FillOrKill && !canMatch(order->getSide(), order->getPrice()))
      return { };

    OrderPointers::iterator iterator;
    
    if (order->getSide() == Side::Buy) {
      auto& orders = bids_[order->getPrice()];
      orders.push_back(order);
      iterator = std::next(orders.begin(), orders.size() - 1);
    } else {
      auto& orders = asks_[order->getPrice()];
      orders.push_back(order);
      iterator = std::next(orders.begin(), orders.size() - 1);
    }

    orders_.insert({ order->getOrderId(), OrderEntry{ order, iterator }});
    return MatchOrders();
  }

  void cancelOrder(OrderId orderId) {
    if (!orders_.contains(orderId))
      return;

    const auto [order, orderIterator] = orders_.at(orderId);
    orders_.erase(orderId);

    if (order->getSide() == Side::Sell) {
      auto price = order->getPrice();
      auto& orders = asks_.at(price); // NOTE: this might be an issue with erase of the reference
      orders.erase(orderIterator);
      if (orders.empty())
        asks_.erase(price);
    } else {
      auto price = order->getPrice();
      auto& orders = bids_.at(price);
      if (orders.empty())
        bids_.erase(price);
    }
  }

  Trades matchOrder(OrderModify order) {
    if (!orders_.contains(order.getOrderId()))
      return { };

    const auto& [existingOrder, _] = orders_.at(order.getOrderId());
    cancelOrder(order.getOrderId());
    return addOrder(order.toOrderPointer(existingOrder->getOrderType()));
  }

  std::size_t size() const { return orders_.size(); }

  OrderbookLevelInfos getOrderInfos() const {
    LevelInfos bidInfos, askInfos;
    bidInfos.reserve(orders_.size());
    askInfos.reserve(orders_.size());

    auto createLevelInfos = [](Price price, const OrderPointers& orders){
      return LevelInfo{ price, 
        std::accumulate(orders.begin(), orders.end(), (Quantity)0,
          [](std::size_t runningSum, const OrderPointer& order){
            return runningSum + order->getRemainingQuantity();
          }
        )
      };
    };

    for (const auto& [price, orders] :bids_) 
      bidInfos.push_back(createLevelInfos(price, orders));

    for (const auto& [price, orders] : asks_)
      askInfos.push_back(createLevelInfos(price, orders));

    return OrderbookLevelInfos{ bidInfos, askInfos };
  }

private:
  struct OrderEntry {
    OrderPointer order_ { nullptr };
    OrderPointers::iterator location_;
  };

  std::map<Price, OrderPointers, std::greater<Price>> bids_;
  std::map<Price, OrderPointers, std::less<Price>> asks_;
  std::unordered_map<OrderId, OrderEntry> orders_;

  bool canMatch(Side side, Price price) const {
    if (side == Side::Buy) {
      if (asks_.empty())
        return false;

      const auto& [bestAsk, _] = *asks_.begin();
      return price >= bestAsk;
    } else {
      if (bids_.empty())
        return false;

      const auto& [bestBid, _] = *bids_.begin();
      return price <= bestBid;
    }
  }

  Trades MatchOrders() {
    Trades trades;
    trades.reserve(orders_.size());

    while (true) {
      if (bids_.empty() || asks_.empty())
        break;

      auto& [bidPrice, bids] = *bids_.begin();
      auto& [askPrice, asks] = *asks_.begin();

      if (bidPrice < askPrice)
        break;

      while (bids.size() && asks.size()) {
        auto& bid = bids.front();
        auto& ask = asks.front();

        Quantity quantity = std::min(bid->getRemainingQuantity(), ask->getRemainingQuantity());

        bid->Fill(quantity);
        ask->Fill(quantity);

        if (bid->isFilled()) {
          bids.pop_front();
          orders_.erase(bid->getOrderId());
        }

        if (ask->isFilled()) {
          asks.pop_front();
          orders_.erase(ask->getOrderId());
        }

        if (bids.empty())
          bids_.erase(bidPrice);

        if (asks.empty())
          asks_.erase(bidPrice);

        trades.push_back(Trade{
          TradeInfo{ bid->getOrderId(), bid->getPrice(), quantity },
          TradeInfo{ ask->getOrderId(), ask->getPrice(), quantity }
        });
      }
    }

    if (!bids_.empty()) {
      auto& [_, bids] = *bids_.begin();
      auto& order = bids.front();
      if (order->getOrderType() == OrderType::FillOrKill)
        cancelOrder(order->getOrderId());
    }

    if (!asks_.empty()) {
      auto& [_, asks] = *asks_.begin();
      auto& order = asks.front();
      if (order->getOrderType() == OrderType::FillOrKill)
        cancelOrder(order->getOrderId());
    }
    return trades;
  }
};

int main() {
  
  Orderbook orderbook;
  const OrderId orderId = 1;
  orderbook.addOrder(std::make_shared<Order>(OrderType::GoodTillCancel, orderId, Side::Buy, 100, 100));
  std::cout << "Orderbook size after adding order: " << orderbook.size() << std::endl;
  orderbook.cancelOrder(orderId);
  std::cout << "Orderbook size after canceling order: " << orderbook.size() << std::endl;

  return 0;
}
