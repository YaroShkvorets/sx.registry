# `registry.sx`

> SX Registry Information

## Quickstart

```bash
cleos get table registry.sx registry.sx swap
```

## Table of Content

- [TABLE `swap`](#table-swap)
- [ACTION `setswap`](#action-setswap)

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
