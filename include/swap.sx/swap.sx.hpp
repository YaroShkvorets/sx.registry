#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

class [[eosio::contract("swap.sx")]] swapSx : public contract {
public:
    using contract::contract;

    /**
     * ## TABLE `tokens`
     *
     * - `{symbol} sym` -  token symbol
     * - `{contract} contract` - token contract account name
     * - `{asset} balance` - current balance
     * - `{asset} depth` - liquidity depth
     *
     * ### example
     *
     * ```json
     * {
     *     "sym": "4,USDT",
     *     "contract": "tethertether",
     *     "balance": "10000.0000 USDT",
     *     "depth": "10000.0000 USDT"
     * }
     * ```
     */
    struct [[eosio::table("tokens")]] tokens_row {
        symbol                      sym;
        name                        contract;
        asset                       balance;
        asset                       depth;

        uint64_t primary_key() const { return sym.code().raw(); }
    };
    typedef eosio::multi_index< "tokens"_n, tokens_row > tokens_table;
};
