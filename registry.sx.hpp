#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("registry.sx")]] registrySx : public contract {
public:
    using contract::contract;

    /**
     * ## TABLE `swap`
     *
     * - `{name} contract` - swap contract account name
     * - `{set<symbol_code>} tokens` - supported tokens
     * - `{set<extended_symbol>} ext_tokens` - supported extended symbols
     *
     * ### example
     *
     * ```json
     * {
     *     "contract": "eosdt.sx",
     *     "tokens": ["EOS", "EOSDT", "USDT"],
     *     "ext_tokens": [
     *         { "sym": "4,EOS", "contract": "eosio.token" },
     *         { "sym": "4,USDT", "contract": "tethertether" },
     *         { "sym": "9,EOSDT", "contract": "eosdtsttoken" }
     *     ]
     * }
     * ```
     */
    struct [[eosio::table("swap")]] swap_row {
        name                    contract;
        set<symbol_code>        tokens;
        set<extended_symbol>    ext_tokens;

        uint64_t primary_key() const { return contract.value; }
    };
    typedef eosio::multi_index< "swap"_n, swap_row > swap_table;

    /**
     * ## ACTION `setswap`
     *
     * Set swap contract
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{name} contract` - swap contract account name
     *
     * ### example
     *
     * ```bash
     * cleos push action registry.sx setswap '["eosdt.sx"]' -p registry.sx
     * ```
     */
    [[eosio::action]]
    void setswap( const name contract );

    // action wrappers
    using setswap_action = eosio::action_wrapper<"setswap"_n, &registrySx::setswap>;
};
