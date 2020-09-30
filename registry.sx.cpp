#include "../sx.swap/swap.sx.hpp"
#include "swap.defi.hpp"
#include "dfs.hpp"
#include "registry.sx.hpp"

[[eosio::action]]
void registrySx::setswap( const name contract )
{
    // contract or registry is authorized
    if ( !has_auth( get_self() )) require_auth( contract );

    // tables
    registrySx::swap_table _swap( get_self(), get_self().value );
    swapSx::tokens_table _tokens( contract, contract.value );

    // erase all rows
    if ( contract.value == 0 ) {
        auto row = _swap.begin();
        while ( row != _swap.end() ) {
            row = _swap.erase( row );
        }
        return;
    }

    // retrieve tokens from swap contract
    set<symbol_code> tokens;
    set<extended_symbol> ext_tokens;

    for ( const auto token : _tokens ) {
        tokens.insert( token.sym.code() );
        ext_tokens.insert( extended_symbol{ token.sym, token.contract } );
    }

    // add/modify swap contract
    auto itr = _swap.find( contract.value );
    if ( itr == _swap.end() ) {
        check( tokens.size(), "contract has zero tokens");
        _swap.emplace( get_self(), [&]( auto& row ){
            row.contract = contract;
            row.tokens = tokens;
            row.ext_tokens = ext_tokens;
        });
    // delete if contract has zero tokens
    } else if ( !tokens.size() ) {
        _swap.erase( itr );
    } else {
        _swap.modify( itr, get_self(), [&]( auto& row ){
            row.tokens = tokens;
            row.ext_tokens = ext_tokens;
        });
    }
}


[[eosio::action]]
void registrySx::clear()
{
    require_auth( get_self() );

    registrySx::defibox_table _defibox( get_self(), get_self().value );
    registrySx::dfs_table _dfs( get_self(), get_self().value );

    clear_table( _dfs );
    clear_table( _defibox );
}

template <typename T>
void registrySx::clear_table( T& table )
{
    auto itr = table.begin();
    while ( itr != table.end() ) {
        itr = table.erase( itr );
    }
}

[[eosio::action]]
void registrySx::setdefibox( const asset requirement )
{
    require_auth( get_self() );

    defibox::pairs _pairs( "swap.defi"_n, "swap.defi"_n.value );

    for ( const auto row : _pairs ) {
        if ( !is_requirement( row.reserve0, requirement ) && !is_requirement( row.reserve1, requirement ) ) continue;

        // add both directions
        registrySx::defibox_table defibox( get_self(), get_self().value );
        add_pair( defibox, row.reserve1.symbol.code(), row.reserve0.symbol.code(), row.id );
        add_pair( defibox, row.reserve0.symbol.code(), row.reserve1.symbol.code(), row.id );
    }
}

[[eosio::action]]
void registrySx::setdfs( const asset requirement )
{
    require_auth( get_self() );

    dfs::markets _markets( "defisswapcnt"_n, "defisswapcnt"_n.value );

    for ( const auto row : _markets ) {
        if ( !is_requirement( row.reserve0, requirement ) && !is_requirement( row.reserve1, requirement ) ) continue;

        // add both directions
        registrySx::dfs_table dfs( get_self(), get_self().value );
        add_pair( dfs, row.reserve1.symbol.code(), row.reserve0.symbol.code(), row.mid );
        add_pair( dfs, row.reserve0.symbol.code(), row.reserve1.symbol.code(), row.mid );
        add_token( row.sym0, row.contract0 );
        add_token( row.sym1, row.contract1 );
    }
}

template <typename T>
void registrySx::add_pair( T& table, const symbol_code base, const symbol_code quote, const uint64_t pair_id )
{
    // find
    auto itr = table.find( base.raw() );

    // does not exist - create
    if ( itr == table.end() ) {
        table.emplace( get_self(), [&]( auto& row ) {
            row.base = base;
            row.quotes[quote] = pair_id;
        });
    // if not modified - modify
    } else if ( itr->quotes.at(quote) != pair_id ) {
        table.modify( itr, get_self(), [&]( auto& row ) {
            row.quotes[quote] = pair_id;
        });
    }
}

void registrySx::add_token( const symbol sym, const name contract )
{
    // find
    registrySx::tokens_table table( get_self(), get_self().value );
    auto itr = table.find( sym.code().raw() );

    // does not exist - create
    if ( itr == table.end() ) {
        table.emplace( get_self(), [&]( auto& row ) {
            row.sym = sym;
            row.contract = contract;
        });
    }
}

/**
 * Must meet minimum requirement
 *
 * 1. must match symbol
 * 2. quantity must exceed requirement
 */
bool registrySx::is_requirement( const asset quantity, const asset requirement )
{
    if ( quantity.symbol == requirement.symbol && quantity >= requirement ) return true;
    return false;
}
