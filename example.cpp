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

// $Revision: 6925 $ $Date:: 2017-05-11 #$ $Author: serge $

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

void test02( const templtext::Templ & t )
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

void test03( const templtext::Templ & t )
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

void test04( const templtext::Templ & t )
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

void test05( const templtext::Templ & t )
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


void test06( const templtext::Templ & t )
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

int main()
{
    templtextkeeper::TemplTextKeeper tt;

    tt.init( "templates.csv" );

    templtextkeeper::TemplTextKeeper::Records info;

    tt.get_templates( info );

    std::cout << "all templates:\n" << std::endl;

    for( auto & e : info )
    {
        std::cout << e.id << ";" << e.name << ";" << lang_tools::to_string_iso( e.locale ) << ";" << e.localized_name << ";" << std::endl;
    }

    std::cout << "\ntests:\n" << std::endl;

    std::vector<lang_tools::lang_e> langs   = { lang_tools::lang_e::EN, lang_tools::lang_e::DE, lang_tools::lang_e::RU };

    for( unsigned int i = 1; i <= 7; ++i )
    {
        for( auto l : langs )
        {
            if( tt.has_template( i, l ) )
            {
                const templtext::Templ & t = * tt.find_template( i, l );
                std::cout << "templ " << i << ":" << lang_tools::to_string( l ) << " - " << t.get_template() << " " << show_placeholders( t.get_placeholders() ) << std::endl;
            }
        }
    }

    const templtext::Templ & t = * tt.find_template( 3, lang_tools::lang_e::EN );

    test02( t );
    test03( t );
    test04( t );
    test05( t );
    test06( t );

    return 0;
}
