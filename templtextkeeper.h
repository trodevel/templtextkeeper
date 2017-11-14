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

// $Revision: 8368 $ $Date:: 2017-11-14 #$ $Author: serge $

#ifndef LIB_TEMPLTEXTKEEPER_H
#define LIB_TEMPLTEXTKEEPER_H

#include <string>                   // std::string
#include <map>                      // std::map
#include <limits>                   // std::numeric_limits

#include "templtext/templ.h"        // Templ
#include "lang_tools/language_enum.h"    // lang_tools::lang_e

#include "types.h"                  // id_t

NAMESPACE_TEMPLTEXTKEEPER_START

class TemplTextKeeper
{
public:
    typedef templtext::Templ Templ;

    struct Record
    {
        id_t                id;
        group_id_t          group_id;
        lang_tools::lang_e  locale;
        std::string         name;
        std::string         localized_name;
        std::string         templ;
    };

    typedef std::vector<Record> Records;

public:

    TemplTextKeeper();
    ~TemplTextKeeper();

    bool init(
            const std::string & config_file );

    Records find_templates(
            uint32_t            * total_size,
            group_id_t          group_id,
            const std::string   & filter,
            lang_tools::lang_e  locale,
            uint32_t            page_size   = std::numeric_limits<uint32_t>::max(),
            uint32_t            page_num    = 0 ) const;

    bool has_template( id_t id, lang_tools::lang_e locale ) const;
    const Templ * get_template( id_t id, lang_tools::lang_e locale ) const;
    const id_t find_template_id_by_name( const std::string & name ) const;

private:

    struct GeneralTemplate
    {
        id_t            id;
        group_id_t      group_id;
        std::string     name;
    };

    struct LocalizedTemplate
    {
        id_t        id;
        lang_tools::lang_e locale;
        std::string name;
        std::string templ;
    };

    struct LocalizedTemplateInfo
    {
        std::string name;
        std::string templ;
        Templ       * t;
    };

    typedef std::map<lang_tools::lang_e, LocalizedTemplateInfo>    MapLocaleToLocTemplInfo;

    struct TemplateInfo
    {
        std::string             name;
        group_id_t              group_id;
        MapLocaleToLocTemplInfo localized_templ_info;
    };

    typedef std::map<std::string, id_t>     MapTemplNameToTemplId;
    typedef std::map<id_t, TemplateInfo>    MapIdToTemplateInfo;

private:

    void process_line( const std::string & l );
    void process_line_t( const std::string & l );
    void process_line_l( const std::string & l );

    GeneralTemplate     to_general_templ( const std::string & l );
    LocalizedTemplate   to_localized_templ( const std::string & l );

    void parse_lines( const std::vector<std::string> & lines );

    static bool is_match( const TemplateInfo & c, group_id_t group_id );
    static bool is_match( const MapLocaleToLocTemplInfo::value_type & c, const std::string & name_filter, lang_tools::lang_e lang );

private:

    MapTemplNameToTemplId   templ_names_;   // map: general template name --> general template id
    MapIdToTemplateInfo     templs_;        // map: general template id --> template info
};

NAMESPACE_TEMPLTEXTKEEPER_END

#endif // LIB_TEMPLTEXTKEEPER_H
