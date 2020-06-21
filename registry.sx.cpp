#include <swap.sx/swap.sx.hpp>

#include "registry.sx.hpp"

[[eosio::action]]
void registrySx::setswap( const name contract )
{
    // contract or registry is authorized
    if ( !has_auth( get_self() )) require_auth( contract );

    // tables
    registrySx::swap_table _swap( get_self(), get_self().value );
    swapSx::tokens_table _tokens( contract, contract.value );

    // retrieve tokens from swap contract
    set<symbol_code> tokens;
    for ( const auto token : _tokens ) {
        tokens.insert( token.sym.code() );
    }

    // add/modify swap contract
    auto itr = _swap.find( contract.value );
    if ( itr == _swap.end() ) {
        check( tokens.size(), "contract has zero tokens");
        _swap.emplace( get_self(), [&]( auto& row ){
            row.contract = contract;
            row.tokens = tokens;
        });
    // delete if contract has zero tokens
    } else if ( !tokens.size() ) {
        _swap.erase( itr );
    } else {
        _swap.modify( itr, get_self(), [&]( auto& row ){
            row.tokens = tokens;
        });
    }
}