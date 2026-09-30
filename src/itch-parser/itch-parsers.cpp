#include "itch-parsers.hpp"

namespace itch {
namespace parsers {
ParsedMessage parse_generic_message(const std::span<std::byte> message) {
	assert(message.size() > 0);
	MessageType message_type;
	copy_int_from_network<MESSAGE_TYPE_LENGTH>(
		&message_type,
		message,
		MESSAGE_TYPE_OFFSET
	);

	switch(message_type) {
		using enum MessageType;
		case System:
			return SystemMessage::parse(message);
		case StockDirectory:
			return StockDirectoryMessage::parse(message);
		case StockTradingAction:
			return StockTradingActionMessage::parse(message);
		case AddOrderNoMPID:
			return AddOrderMessage::parse_no_mpid(message);
		case AddOrderMPID:
			return AddOrderMessage::parse_w_mpid(message);
		case OrderExecuted:
			return OrderExecutedMessage::parse_no_price(message);
		case OrderExecutedPrice:
			return OrderExecutedMessage::parse_w_price(message);
		case OrderCancel:
			return OrderCancelMessage::parse(message);
		case OrderDelete:
			return OrderDeleteMessage::parse(message);
		case OrderReplace:
			return OrderReplaceMessage::parse(message);
		default:
			assert(false && "Unknown message type");
			std::unreachable();
	}
}

auto SystemMessage::parse(const std::span<std::byte> message) -> SystemMessage {
	using namespace spec::system_message;
	assert(message.size() == MESSAGE_SIZE);

	SystemMessage new_message;
	parse_timestamp(new_message, message);
	copy_int_from_network<EVENT_CODE_LENGTH>(
		&new_message.event_code,
		message,
		EVENT_CODE_OFFSET
	);
	return new_message;
}

auto StockDirectoryMessage::parse(
	const std::span<std::byte> message) -> StockDirectoryMessage {
	using namespace spec::stock_directory_message;
	assert(message.size() == MESSAGE_LENGTH);

	StockDirectoryMessage new_message;
	parse_locate(new_message, message);
	parse_timestamp(new_message, message);
	copy_alpha_from_network(
		&new_message.stock,
		message,
		spec::stock_message::STOCK_OFFSET,
		spec::stock_message::STOCK_LENGTH
	);
	copy_int_from_network<MARKET_CATEGORY_LENGTH>(
		&new_message.market_category,
		message,
		MARKET_CATEGORY_OFFSET
	);
	copy_int_from_network<FINANCIAL_STATUS_LENGTH>(
		&new_message.financial_status_indicator,
		message,
		FINANCIAL_STATUS_OFFSET
	);
	copy_int_from_network<ROUND_LOT_SIZE_LENGTH>(
		&new_message.round_lot_size,
		message,
		ROUND_LOT_SIZE_OFFSET
	);
	copy_int_from_network<ROUND_LOTS_ONLY_LENGTH>(
		&new_message.round_lots,
		message,
		ROUND_LOTS_ONLY_OFFSET
	);
	copy_alpha_from_network(
		&new_message.issue_classification,
		message,
		ISSUE_CLASSIFICATION_OFFSET,
		ISSUE_CLASSIFICATION_LENGTH
	);
	copy_alpha_from_network(
		&new_message.issue_sub_type,
		message,
		ISSUE_SUB_TYPE_OFFSET,
		ISSUE_SUB_TYPE_LENGTH
	);
	copy_int_from_network<AUTHENTICITY_LENGTH>(
		&new_message.authenticity,
		message,
		AUTHENTICITY_OFFSET
	);
	copy_int_from_network<SHORT_SALE_THRESHOLD_LENGTH>(
		&new_message.short_sale_threshold_indicator,
		message,
		SHORT_SALE_THRESHOLD_OFFSET
	);
	copy_int_from_network<IPO_FLAG_LENGTH>(
		&new_message.ipo_flag,
		message,
		IPO_FLAG_OFFSET
	);
	copy_int_from_network<LULD_TIER_LENGTH>(
		&new_message.luld_reference_price_tier,
		message,
		LULD_TIER_OFFSET
	);
	copy_int_from_network<ETP_FLAG_LENGTH>(
		&new_message.etp_flag,
		message,
		ETP_FLAG_OFFSET
	);
	copy_int_from_network<ETP_LEVERAGE_FACTOR_LENGTH>(
		&new_message.etp_leverage_factor,
		message,
		ETP_LEVERAGE_FACTOR_OFFSET
	);
	copy_int_from_network<INVERSE_INDICATOR_LENGTH>(
		&new_message.inverse_indicator,
		message,
		INVERSE_INDICATOR_OFFSET
	);
	return new_message;
}

auto StockTradingActionMessage::parse(
	const std::span<std::byte> message) -> StockTradingActionMessage {
	using namespace spec::stock_trading_action_message;
	assert(message.size() == MESSAGE_LENGTH);

	StockTradingActionMessage new_message;
	parse_locate(new_message, message);
	parse_timestamp(new_message, message);
	copy_alpha_from_network(
		&new_message.stock,
		message,
		spec::stock_message::STOCK_OFFSET,
		spec::stock_message::STOCK_LENGTH
	);
	copy_int_from_network<TRADING_STATE_LENGTH>(
		&new_message.trading_state,
		message,
		TRADING_STATE_OFFSET
	);
	copy_alpha_from_network(
		&new_message.reason,
		message,
		REASON_OFFSET,
		REASON_LENGTH
	);
	return new_message;
}

auto AddOrderMessage::parse_no_mpid(const std::span<std::byte> message) -> AddOrderMessage {
	using namespace spec::add_order_message;
	assert(message.size() == LENGTH_NO_MPID);

	AddOrderMessage new_message;
	parse_common(new_message, message);
	copy_int_from_network<SIDE_LENGTH>(
		&new_message.side,
		message,
		SIDE_OFFSET
	);
	copy_int_from_network<SHARE_LENGTH>(
		&new_message.shares,
		message,
		SHARE_OFFSET
	);
	copy_int_from_network<PRICE_LENGTH>(
		&new_message.price,
		message,
		PRICE_OFFSET
	);
	return new_message;
}

auto AddOrderMessage::parse_w_mpid(const std::span<std::byte> message) -> AddOrderMessage {
	using namespace spec::add_order_message;
	assert(message.size() == LENGTH_W_MPID);

	AddOrderMessage new_message = parse_no_mpid(message.subspan<0, LENGTH_NO_MPID>());
	Attribution attribution;
	copy_alpha_from_network(
		&attribution,
		message,
		ATTRIBUTION_OFFSET,
		ATTRIBUTION_LENGTH
	);
	new_message.attribution = attribution;
	return new_message;
}

auto OrderExecutedMessage::parse_no_price(
	const std::span<std::byte> message) -> OrderExecutedMessage {
	using namespace spec::order_execute_message;
	assert(message.size() == LENGTH_NO_PRICE);

	OrderExecutedMessage new_message;
	parse_common(new_message, message);
	copy_int_from_network<EXECUTED_SHARE_LENGTH>(
		&new_message.executed_shares,
		message,
		EXECUTED_SHARE_OFFSET
	);
	copy_int_from_network<MATCH_NUMBER_LENGTH>(
		&new_message.match_number,
		message,
		MATCH_NUMBER_OFFSET
	);
	return new_message;
}

auto OrderExecutedMessage::parse_w_price(const std::span<std::byte> message) -> OrderExecutedMessage {
	using namespace spec::order_execute_message;
	assert(message.size() == LENGTH_W_PRICE);

	OrderExecutedMessage new_message = parse_no_price(message.subspan<0, LENGTH_NO_PRICE>());
	Printable printable;
	Price execution_price;
	copy_int_from_network<PRINTABLE_LENGTH>(
		&printable,
		message,
		PRINTABLE_OFFSET
	);
	copy_int_from_network<EXECUTION_PRICE_LENGTH>(
		&execution_price,
		message,
		EXECUTION_PRICE_OFFSET
	);
	new_message.printable = printable;
	new_message.execution_price = execution_price;
	return new_message;
}

auto OrderCancelMessage::parse(const std::span<std::byte> message) -> OrderCancelMessage {
	using namespace spec::order_cancel_message;
	assert(message.size() == MESSAGE_LENGTH);

	OrderCancelMessage new_message;
	parse_common(new_message, message);
	copy_int_from_network<CANCELLED_SHARES_LENGTH>(
		&new_message.cancelled_shares,
		message,
		CANCELLED_SHARES_OFFSET
	);
	return new_message;
}

auto OrderDeleteMessage::parse(const std::span<std::byte> message) -> OrderDeleteMessage {
	using namespace spec::order_delete_message;
	assert(message.size() == MESSAGE_LENGTH);

	OrderDeleteMessage new_message;
	parse_common(new_message, message);
	return new_message;
}

auto OrderReplaceMessage::parse(const std::span<std::byte> message) -> OrderReplaceMessage {
	using namespace spec::order_replace_message;
	assert(message.size() == MESSAGE_LENGTH);

	OrderReplaceMessage new_message;
	parse_locate(new_message, message);
	parse_timestamp(new_message, message);
	copy_int_from_network<ORDER_REF_LENGTH>(
		&new_message.old_order_reference_number,
		message,
		ORDER_REF_OFFSET
	);
	copy_int_from_network<NEW_ORDER_REF_LENGTH>(
		&new_message.new_order_reference_number,
		message,
		NEW_ORDER_REF_OFFSET
	);
	copy_int_from_network<SHARES_LENGTH>(
		&new_message.share_quantity,
		message,
		SHARES_OFFSET
	);
	copy_int_from_network<PRICE_LENGTH>(
		&new_message.new_price,
		message,
		PRICE_OFFSET
	);
	return new_message;
}
}
}
