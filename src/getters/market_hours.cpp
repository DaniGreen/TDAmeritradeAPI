/*
Copyright (C) 2018 Jonathon Ogden <jeog.dev@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses.
*/

#include <vector>
#include <unordered_map>
#include <iostream>
#include <tuple>
#include <cctype>
#include <string>

#include "../../include/_tdma_api.h"

using namespace std;

namespace tdma {

class MarketHoursGetterImpl
        : public APIGetterImpl {
    MarketType _market_type;
    std::string _date;

    void
    _build()
    {
        string qstr = util::build_encoded_query_str({{"date", _date}});
        string url = URL_MARKETDATA + util::url_encode(to_string(_market_type))
                     + "/hours?" + qstr;
        APIGetterImpl::set_url(url);
    }

    virtual void
    build()
    { _build(); }

public:
    typedef MarketHoursGetter ProxyType;

    MarketHoursGetterImpl( Credentials& creds,
                              MarketType market_type,
                              const string& date )
        :
            APIGetterImpl(creds, data_api_on_error_callback),
            _market_type(market_type),
            _date(date)
        {
            _build();
        }


    MarketType
    get_market_type() const
    { return _market_type; }

    void
    set_market_type(MarketType market_type)
    {
        _market_type = market_type;
        build();
    }

    std::string
    get_date() const
    { return _date; }

    void
    set_date(const string& date)
    {
        if( !util::is_valid_iso8601_datetime(date) ){
            throw ValueException("invalid ISO-8601 date/time: " + date);
        }
        _date = date;
        build();

    }
};

} /* tdma */

using namespace tdma;

int
MarketHoursGetter_Create_ABI( struct Credentials *pcreds,
                                 int market_type,
                                 const char* date,
                                 MarketHoursGetter_C *pgetter,
                                 int allow_exceptions )
{
    int err = getter_is_creatable<MarketHoursGetterImpl>(
        pcreds, pgetter, allow_exceptions
        );
    if( err )
        return err;

    if( !date ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        if( allow_exceptions ){
            throw ValueException("'date' can not be null");
        }
        return TDMA_API_VALUE_ERROR;
    }

    static auto meth = +[](Credentials *c, int m, const char* d){
        return new MarketHoursGetterImpl(*c, static_cast<MarketType>(m), d);
    };

    MarketHoursGetterImpl *obj;
    tie(obj, err) = CallImplFromABI( allow_exceptions, meth, pcreds,
                                           market_type, date );
    if( err ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    assert(MarketHoursGetter::TYPE_ID_LOW == MarketHoursGetter::TYPE_ID_HIGH);
    pgetter->type_id = MarketHoursGetter::TYPE_ID_LOW;
    return 0;
}


int
MarketHoursGetter_Destroy_ABI( MarketHoursGetter_C *pgetter,
                                  int allow_exceptions )
{
    return destroy_getter<MarketHoursGetterImpl>(pgetter, allow_exceptions);
}


int
MarketHoursGetter_GetMarketType_ABI( MarketHoursGetter_C *pgetter,
                                          int *market_type,
                                          int allow_exceptions)
{
    return GetterImplAccessor<int>::template
        get<MarketHoursGetterImpl, MarketType>(
            pgetter, &MarketHoursGetterImpl::get_market_type, market_type,
            "market_type", allow_exceptions
        );
}


int
MarketHoursGetter_SetMarketType_ABI( MarketHoursGetter_C *pgetter,
                                         int market_type,
                                         int allow_exceptions )
{
    return GetterImplAccessor<int>::template
        set<MarketHoursGetterImpl, MarketType>(
            pgetter, &MarketHoursGetterImpl::set_market_type, market_type,
            allow_exceptions
        );
}


int
MarketHoursGetter_GetDate_ABI( MarketHoursGetter_C *pgetter,
                                   char **buf,
                                   size_t *n,
                                   int allow_exceptions)
{
    return GetterImplAccessor<char**>::template get<MarketHoursGetterImpl>(
        pgetter, &MarketHoursGetterImpl::get_date, buf, n, allow_exceptions
        );
}


int
MarketHoursGetter_SetDate_ABI( MarketHoursGetter_C *pgetter,
                                  const char* date,
                                  int allow_exceptions )
{
    return GetterImplAccessor<char**>::template set<MarketHoursGetterImpl>(
        pgetter, &MarketHoursGetterImpl::set_date, date, allow_exceptions
        );
}
