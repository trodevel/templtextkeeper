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

// $Revision: 5130 $ $Date:: 2016-12-02 #$ $Author: serge $

#ifndef LIB_TEMPLTEXTKEEPER_H
#define LIB_TEMPLTEXTKEEPER_H

#include <string>                   // std::string
#include <map>                      // std::map

#include "../templtext/templ.h"     // Templ

#include "namespace_lib.h"          // NAMESPACE_TEMPLTEXTKEEPER_START

NAMESPACE_TEMPLTEXTKEEPER_START

class TemplTextKeeper
{
public:
    typedef templtext::Templ Templ;

public:

    TemplTextKeeper();
    ~TemplTextKeeper();

    bool init(
            const std::string & config_file );

    bool has_template( uint32_t id ) const;

    const Templ & get_template( uint32_t id ) const;

private:
    typedef std::map<uint32_t, std::string>     MapIdToTemplName;
    typedef std::map<uint32_t, Templ*>          MapIdToTempl;

    struct LocalizedTemplate
    {
        uint32_t    id;
        uint32_t    parent_id;
        std::string locale;
        std::string name;
        std::string templ;
    };

private:

    void process_line( const std::string & l );
    void process_line_t( const std::string & l );
    void process_line_l( const std::string & l );

    LocalizedTemplate to_localized_templ( const std::string & l );

    void parse_lines( const std::vector<std::string> & lines );

private:

    MapIdToTempl        templs_;
};

NAMESPACE_TEMPLTEXTKEEPER_END

#endif // LIB_TEMPLTEXTKEEPER_H
