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
     *     "contract": "swap.sx",
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
     * ## TABLE `defibox`
     *
     * - `{symbol_code} base` - base symbol code
     * - `{map<symbol_code, uint64_t>} quotes` - supported quote pairs
     *
     * ### example
     *
     * ```json
     * {
     *     "base": "EOS",
     *     "quotes": {
     *         { "key": "USDT", "value": 12 }
     *     ]
     * }
     * ```
     */
    struct [[eosio::table("defibox")]] defibox_row {
        symbol_code                             base;
        map<symbol_code, uint64_t>              quotes;

        uint64_t primary_key() const { return base.raw(); }
    };
    typedef eosio::multi_index< "defibox"_n, defibox_row > defibox_table;

    struct [[eosio::table("dfs")]] dfs_row {
        symbol_code                         base;
        map<symbol_code, uint64_t>          quotes;

        uint64_t primary_key() const { return base.raw(); }
    };
    typedef eosio::multi_index< "dfs"_n, dfs_row > dfs_table;

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

    [[eosio::action]]
    void clear();

    /**
     * ## ACTION `setdefibox`
     *
     * Set defibox pairs
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{extended_asset} requirement` - minimum requirements
     *
     * ### example
     *
     * ```bash
     * cleos push action registry.sx setdefibox '[{"contract": "eosio.token", "quantity": "1000.0000 EOS"}]' -p registry.sx
     * ```
     */
    [[eosio::action]]
    void setdefibox( const asset requirement );

    [[eosio::action]]
    void setdfs( const asset requirement );

    // action wrappers
    using setswap_action = eosio::action_wrapper<"setswap"_n, &registrySx::setswap>;
    using setdefibox_action = eosio::action_wrapper<"setdefibox"_n, &registrySx::setdefibox>;

private:
    bool is_requirement( const asset quantity, const asset requirement );

    template <typename T>
    void add_pair( T& table, const symbol_code source, const symbol_code target, const uint64_t pair_id );

    template <typename T>
    void clear_table( T& table );
};
