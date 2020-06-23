#!/bin/bash

# compile
eosio-cpp registry.sx.cpp -I include

# unlock wallet
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)

# create account
cleos create account eosio registry.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# deploy
cleos set contract registry.sx . registry.sx.wasm registry.sx.abi

# set swap
cleos push action registry.sx setswap '["swap.sx"]' -p registry.sx