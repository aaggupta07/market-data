#ifndef ITCH_SPEC_HPP
#define ITCH_SPEC_HPP

#include <cstddef>

namespace itch {
namespace spec {
	inline constexpr size_t STOCK_LOCATE_OFFSET = 1;
	inline constexpr size_t STOCK_LOCATE_LENGTH = 2;
	inline constexpr size_t TIMESTAMP_OFFSET = 5;
	inline constexpr size_t TIMESTAMP_LENGTH = 6;
	inline constexpr size_t ORDER_REF_OFFSET = 11;
	inline constexpr size_t ORDER_REF_LENGTH = 8;

	inline constexpr size_t MESSAGE_TYPE_OFFSET = 0;
	inline constexpr size_t MESSAGE_TYPE_LENGTH = 1;
	enum class MessageType: char {
		System = 'S',
		StockDirectory = 'R',
		StockTradingAction = 'H',
		AddOrderNoMPID = 'A',
		AddOrderMPID = 'F',
		OrderExecuted = 'E',
		OrderExecutedPrice = 'C',
		OrderCancel = 'X',
		OrderDelete = 'D',
		OrderReplace = 'U'
	};

	namespace system_message {
		inline constexpr size_t EVENT_CODE_OFFSET = 11;
		inline constexpr size_t EVENT_CODE_LENGTH = 1;

		enum class EventCode: char {
			MessageStart = 'O',
			SystemStart = 'S',
			MarketStart = 'Q',
			MarketEnd = 'M',
			SystemEnd = 'E',
			MessageEnd = 'C'
		};

		inline constexpr size_t MESSAGE_SIZE = 12;
	}

	namespace stock_message {
		inline constexpr size_t STOCK_OFFSET = 11;
		inline constexpr size_t STOCK_LENGTH = 8;

		enum class MarketCategory: char {
			NasdaqGlobalSelect = 'Q',
			NasdaqGlobal = 'G',
			NasdaqCapital = 'S',
			NYSE = 'N',
			NYSEAmerican = 'A',
			NYSEArca = 'P',
			BATSZExchange = 'Z',
			InvestorsExchange = 'V',
			NotAvailable = ' ',
		};

		enum class FinancialStatusIndicator: char {
			Deficient = 'D',
			Delinquent = 'E',
			Bankrupt = 'Q',
			Suspended = 'S',
			DeficientAndBankrupt = 'G',
			DeficientAndDelinquent = 'H',
			DelinquentAndBankrupt = 'J',
			DeficientDelinquentAndBankrupt = 'K',
			CreationsOrRedemptionsSuspended = 'C',
			Normal = 'N',
			NotAvailable = ' ',
		};

		enum class RoundLots: char {
			Only = 'Y',
			OddAndMixedAllowed = 'N',
		};

		enum class Authenticity: char {
			LiveProduction = 'P',
			Test = 'T',
		};

		enum class ShortSaleThresholdIndicator: char {
			Restricted = 'Y',
			NotRestricted = 'N',
			NotAvailable = ' ',
		};

		enum class IPOFlag: char {
			IPO = 'Y',
			NotIPO = 'N',
			NotAvailable = ' ',
		};

		enum class LULDReferencePriceTier: char {
			Tier1 = '1',
			Tier2 = '2',
			NotAvailable = ' ',
		};

		enum class ETPFlag: char {
			ETP = 'Y',
			NotETP = 'N',
			NotAvailable = ' ',
		};

		enum class InverseIndicator: char {
			Inverse = 'Y',
			NotInverse = 'N',
		};

		enum class TradingState: char {
			Halted = 'H',
			Paused = 'P',
			Quoted = 'Q',
			Trading = 'T',
		};
	}

	namespace stock_directory_message {
		inline constexpr size_t MARKET_CATEGORY_OFFSET = 19;
		inline constexpr size_t MARKET_CATEGORY_LENGTH = 1;
		inline constexpr size_t FINANCIAL_STATUS_OFFSET = 20;
		inline constexpr size_t FINANCIAL_STATUS_LENGTH = 1;
		inline constexpr size_t ROUND_LOT_SIZE_OFFSET = 21;
		inline constexpr size_t ROUND_LOT_SIZE_LENGTH = 4;
		inline constexpr size_t ROUND_LOTS_ONLY_OFFSET = 25;
		inline constexpr size_t ROUND_LOTS_ONLY_LENGTH = 1;
		inline constexpr size_t ISSUE_CLASSIFICATION_OFFSET = 26;
		inline constexpr size_t ISSUE_CLASSIFICATION_LENGTH = 1;
		inline constexpr size_t ISSUE_SUB_TYPE_OFFSET = 27;
		inline constexpr size_t ISSUE_SUB_TYPE_LENGTH = 2;
		inline constexpr size_t AUTHENTICITY_OFFSET = 29;
		inline constexpr size_t AUTHENTICITY_LENGTH = 1;
		inline constexpr size_t SHORT_SALE_THRESHOLD_OFFSET = 30;
		inline constexpr size_t SHORT_SALE_THRESHOLD_LENGTH = 1;
		inline constexpr size_t IPO_FLAG_OFFSET = 31;
		inline constexpr size_t IPO_FLAG_LENGTH = 1;
		inline constexpr size_t LULD_TIER_OFFSET = 32;
		inline constexpr size_t LULD_TIER_LENGTH = 1;
		inline constexpr size_t ETP_FLAG_OFFSET = 33;
		inline constexpr size_t ETP_FLAG_LENGTH = 1;
		inline constexpr size_t ETP_LEVERAGE_FACTOR_OFFSET = 34;
		inline constexpr size_t ETP_LEVERAGE_FACTOR_LENGTH = 4;
		inline constexpr size_t INVERSE_INDICATOR_OFFSET = 38;
		inline constexpr size_t INVERSE_INDICATOR_LENGTH = 1;

		inline constexpr size_t MESSAGE_LENGTH = 39;
	}

	namespace stock_trading_action_message {
		inline constexpr size_t TRADING_STATE_OFFSET = 19;
		inline constexpr size_t TRADING_STATE_LENGTH = 1;
		inline constexpr size_t REASON_OFFSET = 21;
		inline constexpr size_t REASON_LENGTH = 4;

		inline constexpr size_t MESSAGE_LENGTH = 25;
	}

	namespace add_order_message {
		inline constexpr size_t SIDE_OFFSET = 19;
		inline constexpr size_t SIDE_LENGTH = 1;
		inline constexpr size_t SHARE_OFFSET = 20;
		inline constexpr size_t SHARE_LENGTH = 4;
		inline constexpr size_t STOCK_OFFSET = 24;
		inline constexpr size_t STOCK_LENGTH = 8;
		inline constexpr size_t PRICE_OFFSET = 32;
		inline constexpr size_t PRICE_LENGTH = 4;
		inline constexpr size_t ATTRIBUTION_OFFSET = 36;
		inline constexpr size_t ATTRIBUTION_LENGTH = 4;

		enum class Side: char {
			Buy = 'B',
			Sell = 'S',
		};

		inline constexpr size_t LENGTH_NO_MPID = 36;
		inline constexpr size_t LENGTH_W_MPID = 40;
	}

	namespace order_execute_message {
		inline constexpr size_t EXECUTED_SHARE_OFFSET = 19;
		inline constexpr size_t EXECUTED_SHARE_LENGTH = 4;
		inline constexpr size_t MATCH_NUMBER_OFFSET = 23;
		inline constexpr size_t MATCH_NUMBER_LENGTH = 8;
		inline constexpr size_t PRINTABLE_OFFSET = 31;
		inline constexpr size_t PRINTABLE_LENGTH = 1;
		inline constexpr size_t EXECUTION_PRICE_OFFSET = 32;
		inline constexpr size_t EXECUTION_PRICE_LENGTH = 4;

		enum class Printable: char {
			NonPrintable = 'N',
			Printable = 'Y',
		};

		inline constexpr size_t LENGTH_NO_PRICE = 31;
		inline constexpr size_t LENGTH_W_PRICE = 36;
	}

	namespace order_cancel_message {
		inline constexpr size_t CANCELLED_SHARES_OFFSET = 19;
		inline constexpr size_t CANCELLED_SHARES_LENGTH = 4;

		inline constexpr size_t MESSAGE_LENGTH = 23;
	}

	namespace order_delete_message {
		inline constexpr size_t MESSAGE_LENGTH = 19;
	}

	namespace order_replace_message {
		inline constexpr size_t NEW_ORDER_REF_OFFSET = 19;
		inline constexpr size_t NEW_ORDER_REF_LENGTH = 8;
		inline constexpr size_t SHARES_OFFSET = 27;
		inline constexpr size_t SHARES_LENGTH = 4;
		inline constexpr size_t PRICE_OFFSET = 31;
		inline constexpr size_t PRICE_LENGTH = 4;

		inline constexpr size_t MESSAGE_LENGTH = 35;
	}
}
}
#endif
