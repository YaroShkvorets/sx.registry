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

### example

```json
{
    "contract": "eosdt.sx",
    "tokens": ["EOS", "EOSDT", "USDT"]
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
