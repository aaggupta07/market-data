#ifndef ITCH_PARSERS_HPP
#define ITCH_PARSERS_HPP

#include <arpa/inet.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <variant>

#include "itch-spec.hpp"

namespace itch {
namespace parsers {
using namespace spec;

using TimePoint = uint64_t; // Avoid std::chrono in low-latency path
using Price = uint32_t;

struct SystemMessage;
struct StockDirectoryMessage;
struct StockTradingActionMessage;
struct AddOrderMessage;
struct OrderExecutedMessage;
struct OrderCancelMessage;
struct OrderDeleteMessage;
struct OrderReplaceMessage;

using ParsedMessage =
std::variant<
	SystemMessage,
	StockDirectoryMessage,
	StockTradingActionMessage,
	AddOrderMessage,
	OrderExecutedMessage,
	OrderCancelMessage,
	OrderDeleteMessage,
	OrderReplaceMessage
>;

ParsedMessage parse_generic_message(const std::span<std::byte> message);

template<size_t length>
void copy_int_from_network(void* field, const std::span<std::byte> message, size_t offset) {
	if constexpr(length == 1) {
		std::memcpy(field, message.data() + offset, length);
	}
	else if constexpr(length == 2) {
		std::memcpy(field, message.data() + offset, length);
		uint16_t* cast_field = static_cast<uint16_t*>(field);
		*cast_field = ntohs(*cast_field);
	}
	else if constexpr(length == 4) {
		std::memcpy(field, message.data() + offset, length);
		uint32_t* cast_field = static_cast<uint32_t*>(field);
		*cast_field = ntohl(*cast_field);
	}
	else if constexpr(length == 6) {
		const uint8_t* bytes = reinterpret_cast<const uint8_t*>(message.data() + offset);
		uint64_t* cast_field = static_cast<uint64_t*>(field);
		*cast_field =
			(static_cast<uint64_t>(bytes[0]) << 40) |
			(static_cast<uint64_t>(bytes[1]) << 32) |
			(static_cast<uint64_t>(bytes[2]) << 24) |
			(static_cast<uint64_t>(bytes[3]) << 16) |
			(static_cast<uint64_t>(bytes[4]) << 8) |
			static_cast<uint64_t>(bytes[5]);
	}
	else if constexpr(length == 8) {
		std::memcpy(field, message.data() + offset, length);
		uint64_t* cast_field = static_cast<uint64_t*>(field);
		*cast_field = ntohll(*cast_field);
	}
	else {
		static_assert(
			length == 1 || length == 2 || length == 4 || length == 6 || length == 8,
			"Unsupported integer length");
	}
}

inline void copy_alpha_from_network(void* field, const std::span<std::byte> message, size_t offset, size_t length) {
	std::memcpy(field, message.data() + offset, length);
}

inline void parse_locate(auto& new_message, const std::span<std::byte> message) {
	copy_int_from_network<STOCK_LOCATE_LENGTH>(&new_message.stock_locate, message, STOCK_LOCATE_OFFSET);
}

inline void parse_timestamp(auto& new_message, const std::span<std::byte> message) {
	copy_int_from_network<TIMESTAMP_LENGTH>(&new_message.ns_since_midnight, message, TIMESTAMP_OFFSET);
}

inline void parse_order_reference(auto& new_message, const std::span<std::byte> message) {
	copy_int_from_network<ORDER_REF_LENGTH>(&new_message.order_reference_number, message, ORDER_REF_OFFSET);
}

inline void parse_common(auto& new_message, const std::span<std::byte> message) {
	parse_locate(new_message, message);
	parse_timestamp(new_message, message);
	parse_order_reference(new_message, message);
}

struct SystemMessage {
	TimePoint ns_since_midnight;
	spec::system_message::EventCode event_code;

	static SystemMessage parse(const std::span<std::byte> message);
};

struct StockDirectoryMessage {
	using Stock = uint64_t;
	using IssueSubType = uint16_t;

	uint16_t stock_locate;
	TimePoint ns_since_midnight;
	Stock stock;
	spec::stock_message::MarketCategory market_category;
	spec::stock_message::FinancialStatusIndicator financial_status_indicator;
	uint32_t round_lot_size;
	spec::stock_message::RoundLots round_lots;
	char issue_classification;
	IssueSubType issue_sub_type;
	spec::stock_message::Authenticity authenticity;
	spec::stock_message::ShortSaleThresholdIndicator short_sale_threshold_indicator;
	spec::stock_message::IPOFlag ipo_flag;
	spec::stock_message::LULDReferencePriceTier luld_reference_price_tier;
	spec::stock_message::ETPFlag etp_flag;
	uint32_t etp_leverage_factor;
	spec::stock_message::InverseIndicator inverse_indicator;

	static StockDirectoryMessage parse(const std::span<std::byte> message);
};

struct StockTradingActionMessage {
	using Stock = uint64_t;
	using Reason = uint32_t;

	uint16_t stock_locate;
	TimePoint ns_since_midnight;
	Stock stock;
	spec::stock_message::TradingState trading_state;
	Reason reason;

	static StockTradingActionMessage parse(const std::span<std::byte> message);
};

struct AddOrderMessage {
	using Attribution = uint32_t;
	using Side = spec::add_order_message::Side;

	uint16_t stock_locate;
	TimePoint ns_since_midnight;
	uint64_t order_reference_number;
	Side side;
	uint32_t shares;
	Price price;
	std::optional<Attribution> attribution;

	static AddOrderMessage parse_no_mpid(const std::span<std::byte> message);
	static AddOrderMessage parse_w_mpid(const std::span<std::byte> message);
};

struct OrderExecutedMessage {
	using Printable = spec::order_execute_message::Printable;

	uint16_t stock_locate;
	TimePoint ns_since_midnight;
	uint64_t order_reference_number;
	uint32_t executed_shares;
	uint64_t match_number;
	std::optional<Printable> printable;
	std::optional<Price> execution_price;

	static OrderExecutedMessage parse_no_price(const std::span<std::byte> message);
	static OrderExecutedMessage parse_w_price(const std::span<std::byte> message);
};

struct OrderCancelMessage {
	uint16_t stock_locate;
	TimePoint ns_since_midnight;
	uint64_t order_reference_number;
	uint32_t cancelled_shares;

	static OrderCancelMessage parse(const std::span<std::byte> message);
};

struct OrderDeleteMessage {
	uint16_t stock_locate;
	TimePoint ns_since_midnight;
	uint64_t order_reference_number;

	static OrderDeleteMessage parse(const std::span<std::byte> message);
};

struct OrderReplaceMessage {
	uint16_t stock_locate;
	TimePoint ns_since_midnight;
	uint64_t old_order_reference_number;
	uint64_t new_order_reference_number;
	uint32_t share_quantity;
	Price new_price;

	static OrderReplaceMessage parse(const std::span<std::byte> message);
};
}
}
#endif
