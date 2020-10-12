#include <sx.uniswap/uniswap.hpp>
#include <sx.swap/swap.sx.hpp>
#include <sx.defibox/defibox.hpp>
#include <sx.dfs/dfs.hpp>
#include <sx.hamburger/hamburger.hpp>

#include "registry.sx.hpp"

[[eosio::action]]
void sx::registry::setswap( const name contract )
{
    // contract or registry is authorized
    if ( !has_auth( get_self() )) require_auth( contract );

    // tables
    sx::registry::swap_table _swap( get_self(), get_self().value );
    swapSx::tokens _tokens( contract, contract.value );

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
void sx::registry::clear()
{
    require_auth( get_self() );

    sx::registry::defibox_table _defibox( get_self(), get_self().value );
    sx::registry::dfs_table _dfs( get_self(), get_self().value );
    clear_table( _defibox );
    clear_table( _dfs );
}

template <typename T>
void sx::registry::clear_table( T& table )
{
    auto itr = table.begin();
    while ( itr != table.end() ) {
        itr = table.erase( itr );
    }
}


[[eosio::action]]
void sx::registry::sethamburger( const extended_asset requirement )
{
    require_auth( get_self() );

    hamburger::pairs _pairs( "hamburgerswp"_n, "hamburgerswp"_n.value );
    sx::registry::hamburger_table hamburger( get_self(), get_self().value );

    for ( const auto row : _pairs ) {
        if ( !is_requirement( row.token0.get_contract(), row.reserve0, requirement ) &&
             !is_requirement( row.token1.get_contract(), row.reserve1, requirement ) ) continue;

        // add both directions
        add_pair( hamburger, row.token0, row.token1, row.id );
        add_pair( hamburger, row.token1, row.token0, row.id );
    }
}

[[eosio::action]]
void sx::registry::setdefibox( const extended_asset requirement )
{
    require_auth( get_self() );

    defibox::pairs _pairs( "swap.defi"_n, "swap.defi"_n.value );
    sx::registry::defibox_table defibox( get_self(), get_self().value );

    for ( const auto row : _pairs ) {
        if ( !is_requirement( row.token0.contract, row.reserve0, requirement ) &&
             !is_requirement( row.token1.contract, row.reserve1, requirement ) ) continue;

        // add both directions
        const extended_symbol base = extended_symbol{ row.token0.symbol, row.token0.contract };
        const extended_symbol quote = extended_symbol{ row.token1.symbol, row.token1.contract };
        add_pair( defibox, base, quote, row.id );
        add_pair( defibox, quote, base, row.id );
    }
}

[[eosio::action]]
void sx::registry::setdfs( const extended_asset requirement )
{
    require_auth( get_self() );

    dfs::markets _markets( "defisswapcnt"_n, "defisswapcnt"_n.value );
    sx::registry::dfs_table dfs( get_self(), get_self().value );

    for ( const auto row : _markets ) {
        if ( !is_requirement( row.contract0, row.reserve0, requirement ) &&
             !is_requirement( row.contract1, row.reserve1, requirement ) ) continue;

        // add both directions
        const extended_symbol base = extended_symbol{ row.sym0, row.contract0 };
        const extended_symbol quote = extended_symbol{ row.sym1, row.contract1 };
        add_pair( dfs, base, quote, row.mid );
        add_pair( dfs, quote, base, row.mid );
    }
}

template <typename T>
void sx::registry::add_pair( T& table, const extended_symbol base, const extended_symbol quote, const uint64_t pair_id )
{
    const symbol_code base_symcode = base.get_symbol().code();
    const symbol_code quote_symcode = quote.get_symbol().code();

    // find
    auto itr = table.find( base_symcode.raw() );

    // does not exist - create
    if ( itr == table.end() ) {
        table.emplace( get_self(), [&]( auto& row ) {
            row.base = base;
            row.pair_ids[quote_symcode] = pair_id;
            row.contracts[quote_symcode] = quote.get_contract();
        });
    // if not modified - modify
    } else if ( itr->pair_ids.at(quote_symcode) != pair_id ) {
        table.modify( itr, get_self(), [&]( auto& row ) {
            row.pair_ids[quote_symcode] = pair_id;
            row.contracts[quote_symcode] = quote.get_contract();
        });
    }
}

/**
 * Must meet minimum requirement
 *
 * 1. must match symbol & contract
 * 2. reserve must exceed requirement
 */
bool sx::registry::is_requirement( const name contract, const asset reserve, const extended_asset requirement )
{
    if ( contract != requirement.contract ) return false;
    if ( reserve.symbol != requirement.quantity.symbol ) return false;
    if ( reserve >= requirement.quantity ) return true;
    return false;
}
