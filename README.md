# `registry.sx`

> SX Registry Information

## Quickstart

```bash
cleos get table registry.sx registry.sx swap
```

## Table of Content

- [TABLE `defibox`](#table-defibox)
- [TABLE `swap`](#table-swap)
- [ACTION `setswap`](#action-setswap)
- [ACTION `setdefibox`](#action-setdefibox)

## TABLE `defibox`

- `{extended_symbol} base` - base symbols
- `{map<symbol_code, uint64_t>} pair_ids` - pair ids
- `{map<symbol_code, extended_symbol>} contracts` - contracts

### example

```json
{
    "base": {"contract":"eosio.token", "symbol": "4,EOS"},
    "pair_ids": [
        {"key": "USDT", "value": 12}
    ],
    "contracts": [
        {"key": "USDT", "value": "tethertether"}
    ]
}
```

## TABLE `swap`

- `{name} contract` - swap contract account name
- `{set<symbol_code>} tokens` - supported tokens
- `{set<extended_symbol>} ext_tokens` - supported extended symbols

### example

```json
{
    "contract": "swap.sx",
    "tokens": ["EOS", "EOSDT", "USDT"],
    "ext_tokens": [
        { "sym": "4,EOS", "contract": "eosio.token" },
        { "sym": "4,USDT", "contract": "tethertether" },
        { "sym": "9,EOSDT", "contract": "eosdtsttoken" }
    ]
}
```

## ACTION `setswap`

Set swap contract

- **authority**: `get_self()`

### params

- `{name} contract` - swap contract account name

### example

```bash
cleos push action registry.sx setswap '["eosdt.sx"]' -p registry.sx
```

## ACTION `setdefibox`

Set defibox pairs

- **authority**: `get_self()`

### params

- `{extended_asset} requirement` - minimum requirements

### example

```bash
cleos push action registry.sx setdefibox '[{"contract": "eosio.token", "quantity": "1000.0000 EOS"}]' -p registry.sx
```