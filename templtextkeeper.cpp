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

// $Revision: 8742 $ $Date:: 2018-03-12 #$ $Author: serge $

#include "templtextkeeper.h"            // self

#include "utils/read_config_file.h"     // utils::read_config_file
#include "utils/tokenizer.h"            // tokenize_to_vector
#include "utils/match_filter.h"         // utils::match_filter()
#include "lang_tools/parser.h"          // lang_tools::to_lang_iso
#include "lang_tools/str_helper.h"      // lang_tools::to_string_iso

#include <stdexcept>                    // std::invalid_argument

NAMESPACE_TEMPLTEXTKEEPER_START

TemplTextKeeper::TemplTextKeeper()
{
}

TemplTextKeeper::~TemplTextKeeper()
{
    for( auto & e : templs_ )
    {
        for( auto & t : e.second.localized_templ_info )
            delete t.second.t;
    }
}

bool TemplTextKeeper::init(
        const std::string & config_file )
{
    if( config_file.empty() )
        return false;

    try
    {
        std::vector<std::string> lines;

        utils::read_config_file( config_file, lines );

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

    info.category_id    = e.category_id;
    info.name           = e.name;

    auto b = templs_.insert( MapIdToTemplateInfo::value_type( e.id, info ) ).second;

    if( b == false )
    {
        throw std::runtime_error( "duplicate template id " + std::to_string( e.id ) );
    }

    b = templ_names_.insert( MapTemplNameToTemplId::value_type( e.name, e.id ) ).second;

    if( b == false )
    {
        throw std::runtime_error( "duplicate template name '" + e.name + "', id " + std::to_string( e.id ) );
    }
}

void TemplTextKeeper::process_line_l( const std::string & line )
{
    auto e = to_localized_templ( line );

    auto it = templs_.find( e.id );

    if( it == templs_.end() )
    {
        throw std::runtime_error( "cannot find template id " + std::to_string( e.id ) );
    }

    auto & info = it->second;

    LocalizedTemplateInfo loc_info;

    loc_info.name   = e.name;
    loc_info.templ  = e.templ;
    loc_info.t      = new Templ( e.templ, e.name );

    auto b = info.localized_templ_info.insert( MapLocaleToLocTemplInfo::value_type( e.locale, loc_info ) ).second;

    if( b == false )
    {
        throw std::runtime_error( "template " + std::to_string( e.id ) + " has already locale " + lang_tools::to_string_iso( e.locale ) );
    }
}

TemplTextKeeper::GeneralTemplate TemplTextKeeper::to_general_templ( const std::string & l )
{
    // format: T;1;17;Say;
    GeneralTemplate res;

    std::vector< std::string > elems;
    tokenize_to_vector( elems, l, ";" );

    if( elems.size() < 4 )
        throw std::runtime_error( "not enough arguments (<4) in entry: " + l  );

    try
    {
        res.id          = std::stoi( elems[1] );
        res.category_id = std::stoi( elems[2] );
        res.name        = elems[3];
    }
    catch( std::exception & e )
    {
        throw std::runtime_error( "invalid entry: " + l );
    }

    return res;
}

TemplTextKeeper::LocalizedTemplate TemplTextKeeper::to_localized_templ( const std::string & l )
{
    // format: L;1;de;Sagen;%TEXT.
    LocalizedTemplate res;

    std::vector< std::string > elems;
    tokenize_to_vector( elems, l, ";" );

    if( elems.size() < 5 )
        throw std::runtime_error( "not enough arguments (<5) in entry: " + l  );

    try
    {
        res.id          = std::stoi( elems[1] );
        res.locale      = lang_tools::to_lang_iso( elems[2] );
        res.name        = elems[3];
        res.templ       = elems[4];
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

bool TemplTextKeeper::has_template( id_t id, lang_tools::lang_e locale ) const
{
    auto it = templs_.find( id );

    if( it == templs_.end() )
        return false;

    auto & loc = it->second.localized_templ_info;

    auto it_2 = loc.find( locale );

    if( it_2 == loc.end() )
        return false;

    return true;
}

const TemplTextKeeper::Templ * TemplTextKeeper::get_template( id_t id, lang_tools::lang_e locale ) const
{
    auto it = templs_.find( id );

    if( it == templs_.end() )
        return nullptr;

    auto it2 = it->second.localized_templ_info.find( locale );

    if( it2 == it->second.localized_templ_info.end() )
        return nullptr;

    return it2->second.t;
}

const id_t TemplTextKeeper::find_template_id_by_name( const std::string & name ) const
{
    auto it = templ_names_.find( name );

    if( it == templ_names_.end() )
        return 0;

    return it->second;
}

TemplTextKeeper::Records TemplTextKeeper::find_templates(
        uint32_t            * total_size,
        category_id_t       category_id,
        const std::string   & filter,
        lang_tools::lang_e  locale,
        uint32_t            page_size,
        uint32_t            page_num ) const
{
    TemplTextKeeper::Records res;

    * total_size = 0;

    auto offset     = page_size * page_num;
    auto offset_end = offset + page_size;

    unsigned i = 0;

    for( auto & t : templs_ )
    {
        auto & loc = t.second.localized_templ_info;

        if( is_match( t.second, category_id ) == false )
            continue;

        for( auto & l : loc )
        {
            if( is_match( l, filter, locale ) )
            {
                // return only those elements, which belong to the desired page
                if( i >= offset && i < offset_end )
                {
                    Record r;

                    r.id                = t.first;
                    r.category_id       = t.second.category_id;
                    r.name              = t.second.name;

                    r.locale            = l.first;
                    r.localized_name    = l.second.name;
                    r.templ             = l.second.templ;

                    res.push_back( r );
                }

                i++;
            }
        }
    }

    * total_size  = i;

    return res;
}

bool TemplTextKeeper::is_match( const TemplateInfo & c, category_id_t category_id )
{
    if( category_id != 0 && category_id != c.category_id )
        return false;

    return true;
}

bool TemplTextKeeper::is_match( const MapLocaleToLocTemplInfo::value_type & c, const std::string & name_filter, lang_tools::lang_e lang )
{
    if( lang != lang_tools::lang_e::UNDEF && lang != c.first )
        return false;

    if( utils::match_filter( c.second.name, name_filter, true ) )
        return true;

    return false;
}

NAMESPACE_TEMPLTEXTKEEPER_END
