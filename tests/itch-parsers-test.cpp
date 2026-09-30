#include "itch-parsers.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string_view>
#include <variant>
#include <vector>

namespace {
using namespace itch::parsers;
namespace spec = itch::spec;

inline constexpr uint16_t STOCK_LOCATE = 0x1234;
inline constexpr uint64_t TIMESTAMP = 0x010203040506;
inline constexpr uint64_t ORDER_REFERENCE = 0x0102030405060708;

int failures = 0;

#define CHECK(expression) \
	do { \
		if (!(expression)) { \
			std::cerr << "CHECK failed at line " << __LINE__ << ": " << #expression << '\n'; \
			++failures; \
		} \
	} while (false)

void put_integer(
	std::vector<std::byte>& message,
	size_t offset,
	size_t length,
	uint64_t value) {
	for (size_t i = 0; i < length; ++i) {
		message[offset + i] = static_cast<std::byte>(
			(value >> ((length - i - 1) * 8)) & 0xff);
	}
}

void put_alpha(
	std::vector<std::byte>& message,
	size_t offset,
	std::string_view value) {
	std::memcpy(message.data() + offset, value.data(), value.size());
}

auto make_message(char type, size_t length) -> std::vector<std::byte> {
	std::vector<std::byte> message(length, static_cast<std::byte>(' '));
	put_alpha(message, 0, std::string_view(&type, 1));
	put_integer(message, 1, 2, STOCK_LOCATE);
	put_integer(message, 3, 2, 0x5678);
	put_integer(message, 5, 6, TIMESTAMP);
	return message;
}

void put_order_reference(std::vector<std::byte>& message) {
	put_integer(message, 11, 8, ORDER_REFERENCE);
}

template<typename Message>
auto parse_as(std::vector<std::byte>& bytes) -> Message {
	ParsedMessage parsed = parse_generic_message(bytes);
	CHECK(std::holds_alternative<Message>(parsed));
	return std::get<Message>(parsed);
}

void test_system_message() {
	auto bytes = make_message('S', spec::system_message::MESSAGE_SIZE);
	put_alpha(bytes, 11, "Q");
	auto message = parse_as<SystemMessage>(bytes);
	CHECK(message.ns_since_midnight == TIMESTAMP);
	CHECK(message.event_code == spec::system_message::EventCode::MarketStart);
}

void test_stock_directory_message() {
	using namespace spec::stock_directory_message;
	auto bytes = make_message('R', MESSAGE_LENGTH);
	put_alpha(bytes, spec::stock_message::STOCK_OFFSET, "AAPL    ");
	put_alpha(bytes, MARKET_CATEGORY_OFFSET, "Q");
	put_alpha(bytes, FINANCIAL_STATUS_OFFSET, "N");
	put_integer(bytes, ROUND_LOT_SIZE_OFFSET, ROUND_LOT_SIZE_LENGTH, 100);
	put_alpha(bytes, ROUND_LOTS_ONLY_OFFSET, "N");
	put_alpha(bytes, ISSUE_CLASSIFICATION_OFFSET, "C");
	put_alpha(bytes, ISSUE_SUB_TYPE_OFFSET, "CS");
	put_alpha(bytes, AUTHENTICITY_OFFSET, "P");
	put_alpha(bytes, SHORT_SALE_THRESHOLD_OFFSET, "N");
	put_alpha(bytes, IPO_FLAG_OFFSET, "N");
	put_alpha(bytes, LULD_TIER_OFFSET, "1");
	put_alpha(bytes, ETP_FLAG_OFFSET, "Y");
	put_integer(bytes, ETP_LEVERAGE_FACTOR_OFFSET, ETP_LEVERAGE_FACTOR_LENGTH, 3);
	put_alpha(bytes, INVERSE_INDICATOR_OFFSET, "N");

	auto message = parse_as<StockDirectoryMessage>(bytes);
	CHECK(message.stock_locate == STOCK_LOCATE);
	CHECK(message.ns_since_midnight == TIMESTAMP);
	CHECK(std::memcmp(&message.stock, "AAPL    ", 8) == 0);
	CHECK(message.market_category == spec::stock_message::MarketCategory::NasdaqGlobalSelect);
	CHECK(message.financial_status_indicator ==
		spec::stock_message::FinancialStatusIndicator::Normal);
	CHECK(message.round_lot_size == 100);
	CHECK(message.round_lots == spec::stock_message::RoundLots::OddAndMixedAllowed);
	CHECK(message.issue_classification == 'C');
	CHECK(std::memcmp(&message.issue_sub_type, "CS", 2) == 0);
	CHECK(message.authenticity == spec::stock_message::Authenticity::LiveProduction);
	CHECK(message.short_sale_threshold_indicator ==
		spec::stock_message::ShortSaleThresholdIndicator::NotRestricted);
	CHECK(message.ipo_flag == spec::stock_message::IPOFlag::NotIPO);
	CHECK(message.luld_reference_price_tier ==
		spec::stock_message::LULDReferencePriceTier::Tier1);
	CHECK(message.etp_flag == spec::stock_message::ETPFlag::ETP);
	CHECK(message.etp_leverage_factor == 3);
	CHECK(message.inverse_indicator == spec::stock_message::InverseIndicator::NotInverse);
}

void test_stock_trading_action_message() {
	using namespace spec::stock_trading_action_message;
	auto bytes = make_message('H', MESSAGE_LENGTH);
	put_alpha(bytes, spec::stock_message::STOCK_OFFSET, "TSLA    ");
	put_alpha(bytes, TRADING_STATE_OFFSET, "H");
	put_alpha(bytes, REASON_OFFSET, "T1  ");

	auto message = parse_as<StockTradingActionMessage>(bytes);
	CHECK(message.stock_locate == STOCK_LOCATE);
	CHECK(message.ns_since_midnight == TIMESTAMP);
	CHECK(std::memcmp(&message.stock, "TSLA    ", 8) == 0);
	CHECK(message.trading_state == spec::stock_message::TradingState::Halted);
	CHECK(std::memcmp(&message.reason, "T1  ", 4) == 0);
}

void test_add_order_messages() {
	using namespace spec::add_order_message;
	auto bytes = make_message('A', LENGTH_NO_MPID);
	put_order_reference(bytes);
	put_alpha(bytes, SIDE_OFFSET, "B");
	put_integer(bytes, SHARE_OFFSET, SHARE_LENGTH, 1000);
	put_alpha(bytes, STOCK_OFFSET, "MSFT    ");
	put_integer(bytes, PRICE_OFFSET, PRICE_LENGTH, 4123400);

	auto message = parse_as<AddOrderMessage>(bytes);
	CHECK(message.stock_locate == STOCK_LOCATE);
	CHECK(message.ns_since_midnight == TIMESTAMP);
	CHECK(message.order_reference_number == ORDER_REFERENCE);
	CHECK(message.side == Side::Buy);
	CHECK(message.shares == 1000);
	CHECK(message.price == 4123400);
	CHECK(!message.attribution.has_value());

	auto attributed_bytes = make_message('F', LENGTH_W_MPID);
	std::copy(bytes.begin() + 1, bytes.end(), attributed_bytes.begin() + 1);
	put_alpha(attributed_bytes, ATTRIBUTION_OFFSET, "ABCD");
	auto attributed = parse_as<AddOrderMessage>(attributed_bytes);
	CHECK(attributed.attribution.has_value());
	CHECK(std::memcmp(&*attributed.attribution, "ABCD", 4) == 0);
}

void test_order_executed_messages() {
	using namespace spec::order_execute_message;
	auto bytes = make_message('E', LENGTH_NO_PRICE);
	put_order_reference(bytes);
	put_integer(bytes, EXECUTED_SHARE_OFFSET, EXECUTED_SHARE_LENGTH, 250);
	put_integer(bytes, MATCH_NUMBER_OFFSET, MATCH_NUMBER_LENGTH, 0x1112131415161718);

	auto message = parse_as<OrderExecutedMessage>(bytes);
	CHECK(message.order_reference_number == ORDER_REFERENCE);
	CHECK(message.executed_shares == 250);
	CHECK(message.match_number == 0x1112131415161718);
	CHECK(!message.printable.has_value());
	CHECK(!message.execution_price.has_value());

	auto priced_bytes = make_message('C', LENGTH_W_PRICE);
	std::copy(bytes.begin() + 1, bytes.end(), priced_bytes.begin() + 1);
	put_alpha(priced_bytes, PRINTABLE_OFFSET, "Y");
	put_integer(priced_bytes, EXECUTION_PRICE_OFFSET, EXECUTION_PRICE_LENGTH, 1234500);
	auto priced = parse_as<OrderExecutedMessage>(priced_bytes);
	CHECK(priced.printable == Printable::Printable);
	CHECK(priced.execution_price == 1234500);
}

void test_cancel_delete_replace_messages() {
	auto cancel_bytes = make_message('X', spec::order_cancel_message::MESSAGE_LENGTH);
	put_order_reference(cancel_bytes);
	put_integer(
		cancel_bytes,
		spec::order_cancel_message::CANCELLED_SHARES_OFFSET,
		spec::order_cancel_message::CANCELLED_SHARES_LENGTH,
		75);
	auto cancel = parse_as<OrderCancelMessage>(cancel_bytes);
	CHECK(cancel.order_reference_number == ORDER_REFERENCE);
	CHECK(cancel.cancelled_shares == 75);

	auto delete_bytes = make_message('D', spec::order_delete_message::MESSAGE_LENGTH);
	put_order_reference(delete_bytes);
	auto deletion = parse_as<OrderDeleteMessage>(delete_bytes);
	CHECK(deletion.order_reference_number == ORDER_REFERENCE);

	using namespace spec::order_replace_message;
	auto replace_bytes = make_message('U', MESSAGE_LENGTH);
	put_order_reference(replace_bytes);
	put_integer(replace_bytes, NEW_ORDER_REF_OFFSET, NEW_ORDER_REF_LENGTH, 0x2122232425262728);
	put_integer(replace_bytes, SHARES_OFFSET, SHARES_LENGTH, 500);
	put_integer(replace_bytes, PRICE_OFFSET, PRICE_LENGTH, 9876500);
	auto replacement = parse_as<OrderReplaceMessage>(replace_bytes);
	CHECK(replacement.old_order_reference_number == ORDER_REFERENCE);
	CHECK(replacement.new_order_reference_number == 0x2122232425262728);
	CHECK(replacement.share_quantity == 500);
	CHECK(replacement.new_price == 9876500);
}
}

int main() {
	test_system_message();
	test_stock_directory_message();
	test_stock_trading_action_message();
	test_add_order_messages();
	test_order_executed_messages();
	test_cancel_delete_replace_messages();

	if (failures != 0) {
		std::cerr << failures << " parser test(s) failed\n";
		return 1;
	}
	std::cout << "All parser tests passed\n";
}
