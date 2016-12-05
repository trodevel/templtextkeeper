/*

Text Template Keeper library.

Copyright (C) 2015 Sergey Kolevatov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

// $Revision: 5151 $ $Date:: 2016-12-05 #$ $Author: serge $

#include "templtextkeeper.h"            // self

#include "../utils/read_config_file.h"  // read_config_file
#include "../utils/tokenizer.h"         // tokenize_to_vector

#include <stdexcept>                    // std::invalid_argument

NAMESPACE_TEMPLTEXTKEEPER_START

TemplTextKeeper::TemplTextKeeper()
{
}

TemplTextKeeper::~TemplTextKeeper()
{
    for( auto & e : localized_templs_ )
        delete e.second;
}

bool TemplTextKeeper::init(
        const std::string & config_file )
{
    if( config_file.empty() )
        return false;

    try
    {
        std::vector<std::string> lines;

        read_config_file( config_file, lines );

        parse_lines( lines );
    }
    catch( std::exception & e )
    {
        throw e;
    }

    return true;
}

void TemplTextKeeper::process_line( const std::string & line )
{
    if( line.empty() )
        throw std::runtime_error( "parse_line: invalid entry - empty line" );

    if( line[0] == 'T' )
    {
        process_line_t( line );
    }
    else if( line[0] == 'L' )
    {
        process_line_l( line );
    }
    else
    {
        throw std::runtime_error( "parse_line: invalid entry " + line );
    }
}

void TemplTextKeeper::process_line_t( const std::string & line )
{
    auto e = to_general_templ( line );

    TemplateInfo info;

    info.name   = e.name;

    auto b = templs_.insert( MapIdToTemplateInfo::value_type( e.id, info ) ).second;

    if( b == false )
    {
        throw std::runtime_error( "duplicate general template id " + std::to_string( e.id ) );
    }

    b = templ_names_.insert( MapTemplNameToTemplId::value_type( e.name, e.id ) ).second;

    if( b == false )
    {
        throw std::runtime_error( "duplicate general template name '" + e.name + "', id " + std::to_string( e.id ) );
    }
}

void TemplTextKeeper::process_line_l( const std::string & line )
{
    auto e = to_localized_templ( line );

    auto it = templs_.find( e.parent_id );

    if( it == templs_.end() )
    {
        throw std::runtime_error( "cannot find parent general template id " + std::to_string( e.parent_id ) );
    }

    auto & info = it->second;

    LocalizedTemplateInfo loc_info;

    loc_info.id     = e.id;
    loc_info.name   = e.name;

    auto b = info.localized_templ_info.insert( MapLocaleToLocTemplInfo::value_type( e.locale, loc_info ) ).second;

    if( b == false )
    {
        throw std::runtime_error( "general template " + std::to_string( e.parent_id ) + " has already locale " + e.locale + ", cannot add localized template " + std::to_string( e.id ) );
    }

    Templ * t = new Templ( e.templ, e.name );

    b = localized_templs_.insert( MapIdToTempl::value_type( e.id, t ) ).second;

    if( b == false )
    {
        throw std::runtime_error( "duplicate localized template id " + std::to_string( e.id ) );
    }
}

TemplTextKeeper::GeneralTemplate TemplTextKeeper::to_general_templ( const std::string & l )
{
    // format: T;1;Say;
    GeneralTemplate res;

    std::vector< std::string > elems;
    tokenize_to_vector( elems, l, ";" );

    if( elems.size() < 3 )
        throw std::runtime_error( "not enough arguments (<3) in entry: " + l  );

    try
    {
        res.id          = std::stoi( elems[1] );
        res.name        = elems[2];
    }
    catch( std::exception & e )
    {
        throw std::runtime_error( "invalid entry: " + l );
    }

    return res;
}

TemplTextKeeper::LocalizedTemplate TemplTextKeeper::to_localized_templ( const std::string & l )
{
    // format: L;1;1;de;Sagen;%TEXT.
    LocalizedTemplate res;

    std::vector< std::string > elems;
    tokenize_to_vector( elems, l, ";" );

    if( elems.size() < 6 )
        throw std::runtime_error( "not enough arguments (<6) in entry: " + l  );

    try
    {
        res.id          = std::stoi( elems[1] );
        res.parent_id   = std::stoi( elems[2] );
        res.locale      = elems[3];
        res.name        = elems[4];
        res.templ       = elems[5];
    }
    catch( std::exception & e )
    {
        throw std::runtime_error( "invalid entry: " + l );
    }

    return res;
}

void TemplTextKeeper::parse_lines( const std::vector<std::string> & lines )
{
    for( auto & l : lines )
    {
        process_line( l );
    }
}

bool TemplTextKeeper::has_template( uint32_t id ) const
{
    return ( localized_templs_.count( id ) > 0 );
}

const TemplTextKeeper::Templ & TemplTextKeeper::get_template( uint32_t id ) const
{
    auto it = localized_templs_.find( id );

    if( it == localized_templs_.end() )
    {
        throw std::invalid_argument( "cannot find template '" + std::to_string( id ) + "'" );
    }

    return *it->second;
}

std::vector<TemplTextKeeper::TemplateInfo> TemplTextKeeper::get_template_list() const
{
    std::vector<TemplateInfo> res;

    for( auto & e : templs_ )
    {
        res.push_back( e.second );
    }

    return res;
}

NAMESPACE_TEMPLTEXTKEEPER_END
