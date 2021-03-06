/*

Example.

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

// $Revision: 8373 $ $Date:: 2017-11-15 #$ $Author: serge $

#include <cstdio>
#include <sstream>                          // std::stringstream
#include <iostream>                         // std::cout

#include "templtextkeeper.h"                // TemplTextKeeper

#include "../lang_tools/str_helper.h"       // lang_tools::to_string_iso

std::string show_placeholders( const std::set<std::string> & ph )
{
    std::stringstream os;

    os << ph.size() << ": ";

    for( auto & s : ph )
        os << s << " ";

    return os.str();
}

void print( const templtextkeeper::TemplTextKeeper::Record & e )
{
    std::cout << e.id << ";" << e.category_id << ";" << e.name << ";" << lang_tools::to_string_iso( e.locale ) << ";" << e.localized_name << ";" << std::endl;
}

void print( uint32_t total_size, const templtextkeeper::TemplTextKeeper::Records & info )
{
    std::cout << "templates (" << total_size << "):\n" << std::endl;

    for( auto & e : info )
    {
        print( e );
    }

    std::cout << std::endl;
}

void test_01( const templtextkeeper::TemplTextKeeper & ttk )
{
    uint32_t total_size;

    templtextkeeper::TemplTextKeeper::Records info = ttk.find_templates( & total_size, 0, "", lang_tools::lang_e::UNDEF, 1000, 0 );

    print( total_size, info );
}

void test_01_b( const templtextkeeper::TemplTextKeeper & ttk )
{
    std::cout << "\ntests:\n" << std::endl;

    std::vector<lang_tools::lang_e> langs   = { lang_tools::lang_e::EN, lang_tools::lang_e::DE, lang_tools::lang_e::RU };

    for( unsigned int i = 1; i <= 7; ++i )
    {
        for( auto l : langs )
        {
            if( ttk.has_template( i, l ) )
            {
                const templtext::Templ & t = * ttk.get_template( i, l );
                std::cout << "templ " << i << ":" << lang_tools::to_string( l ) << " - " << t.get_template() << " " << show_placeholders( t.get_placeholders() ) << std::endl;
            }
        }
    }
}

void test_02( const templtext::Templ & t )
{
    std::cout << "TEST 02" << std::endl;

    templtext::Templ::MapKeyValue tokens  =
    {
            { "SALUTATION", "Mr." },
            { "NAME", "John Doe" },
    };

    std::string missing_token;

    if( t.validate_tokens( tokens, missing_token ) == false )
    {
        std::cout << "missing token '" << missing_token << "' in template " << t.get_name() << std::endl;
    }
}

void test_03( const templtext::Templ & t )
{
    std::cout << "TEST 03" << std::endl;

    templtext::Templ::MapKeyValue tokens  =
    {
            { "SALUTATION", "Mr." },
            { "NAME", "John Doe" },
            { "TEXT", "Hello World" }
    };

    std::string missing_token;

    if( t.validate_tokens( tokens, missing_token ) )
    {
        std::cout << "all tokens are present in template " << t.get_name() << std::endl;
    }
}

void test_04( const templtext::Templ & t )
{
    std::cout << "TEST 04" << std::endl;

    templtext::Templ::MapKeyValue tokens  =
    {
            { "SALUTATION", "Mr." },
            { "NAME", "John Doe" },
    };

    try
    {
        std::string res = t.format( tokens );
    }
    catch ( std::exception & e )
    {
        std::cout << "got exception '" << e.what() << "'" << std::endl;
    }
}

void test_05( const templtext::Templ & t )
{
    std::cout << "TEST 05" << std::endl;

    templtext::Templ::MapKeyValue tokens  =
    {
            { "SALUTATION", "Mr." },
            { "NAME", "John Doe" },
    };

    try
    {
        std::string res = t.format( tokens, false );

        std::cout << "formatted string is '" << res << "'" << std::endl;
    }
    catch ( std::exception & e )
    {
        std::cout << "got exception '" << e.what() << "'" << std::endl;
    }
}


void test_06( const templtext::Templ & t )
{
    std::cout << "TEST 06" << std::endl;

    templtext::Templ::MapKeyValue tokens  =
    {
            { "SALUTATION", "Mr." },
            { "NAME", "John Doe" },
            { "TEXT", "Hello World" }
    };

    try
    {
        std::string res = t.format( tokens );

        std::cout << "formatted string is '" << res << "'" << std::endl;
    }
    catch ( std::exception & e )
    {
        std::cout << "got exception '" << e.what() << "'" << std::endl;
    }
}

void test_07( const templtextkeeper::TemplTextKeeper & ttk )
{
    std::string name( "Text05" );

    std::cout << "07. looking for template named: " << name << std::endl;

    auto id = ttk.find_template_id_by_name( name );

    if( id )
    {
        std::cout << "07. OK: id = " << id << std::endl;
    }
    else
    {
        std::cout << "07. ERROR: cannot find template named " << name << std::endl;
    }
}

void test_08( const templtextkeeper::TemplTextKeeper & ttk )
{
    std::string name( "xxx" );

    std::cout << "08. looking for non-existing template named: " << name << std::endl;

    auto id = ttk.find_template_id_by_name( name );

    if( id == 0 )
    {
        std::cout << "08. OK: non-existing template not found" << std::endl;
    }
    else
    {
        std::cout << "08. ERROR: non-existing template found: id = " << id << std::endl;
    }
}

void test_09_find_templates( const templtextkeeper::TemplTextKeeper & ttk )
{
    uint32_t total_size;

    templtextkeeper::TemplTextKeeper::Records info = ttk.find_templates( & total_size, 0, "sa", lang_tools::lang_e::UNDEF, 1000, 0 );

    print( total_size, info );
}

void test_10_find_templates( const templtextkeeper::TemplTextKeeper & ttk )
{
    uint32_t total_size;

    templtextkeeper::TemplTextKeeper::Records info = ttk.find_templates( & total_size, 0, "sa", lang_tools::lang_e::DE, 1000, 0 );

    print( total_size, info );
}

void test_11_find_templates( const templtextkeeper::TemplTextKeeper & ttk )
{
    uint32_t total_size;

    templtextkeeper::TemplTextKeeper::Records info = ttk.find_templates( & total_size, 0, "", lang_tools::lang_e::RU, 1000, 0 );

    print( total_size, info );
}

void test_12_find_templates( const templtextkeeper::TemplTextKeeper & ttk )
{
    uint32_t total_size;

    templtextkeeper::TemplTextKeeper::Records info = ttk.find_templates( & total_size, 0, "ü", lang_tools::lang_e::UNDEF, 1000, 0 );

    print( total_size, info );
}

void test_13_find_templates( const templtextkeeper::TemplTextKeeper & ttk )
{
    uint32_t total_size;

    templtextkeeper::TemplTextKeeper::Records info = ttk.find_templates( & total_size, 3, "", lang_tools::lang_e::EN );

    print( total_size, info );
}

void test_14_find_templates( const templtextkeeper::TemplTextKeeper & ttk )
{
    uint32_t total_size;

    templtextkeeper::TemplTextKeeper::Records info = ttk.find_templates( & total_size, 3, "", lang_tools::lang_e::DE );

    print( total_size, info );
}

int main()
{
    templtextkeeper::TemplTextKeeper ttk;

    ttk.init( "templates.csv" );

    const templtext::Templ & t = * ttk.get_template( 3, lang_tools::lang_e::EN );

    test_01( ttk );
    test_01_b( ttk );
    test_02( t );
    test_03( t );
    test_04( t );
    test_05( t );
    test_06( t );
    test_07( ttk );
    test_08( ttk );
    test_09_find_templates( ttk );
    test_10_find_templates( ttk );
    test_11_find_templates( ttk );
    test_12_find_templates( ttk );
    test_13_find_templates( ttk );
    test_14_find_templates( ttk );

    return 0;
}
