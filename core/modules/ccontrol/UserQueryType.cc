/*
 * LSST Data Management System
 * Copyright 2015-2016 AURA/LSST.
 *
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the LSST License Statement and
 * the GNU General Public License along with this program.  If not,
 * see <https://www.lsstcorp.org/LegalNotices/>.
 */

// Class header
#include "ccontrol/UserQueryType.h"

// System headers

// Third-party headers
#include "boost/regex.hpp"

// LSST headers
#include "lsst/log/Log.h"

// Qserv headers

namespace {

LOG_LOGGER _log = LOG_GET("lsst.qserv.ccontrol.UserQueryType");

// regex for DROP {DATABASE|SCHEMA} dbname; db name can be in quotes;
// db name will be in group 3.
// Note that parens around whole string are not part of the regex but raw string literal
boost::regex _dropDbRe(R"(^drop\s+(database|schema)\s+(["`]?)(\w+)\2\s*;?\s*$)",
                       boost::regex::ECMAScript | boost::regex::icase | boost::regex::optimize);

// regex for DROP TABLE [dbname.]table; both table and db names can be in quotes;
// db name will be in group 3, table name in group 5.
// Note that parens around whole string are not part of the regex but raw string literal
boost::regex _dropTableRe(R"(^drop\s+table\s+((["`]?)(\w+)\2[.])?(["`]?)(\w+)\4\s*;?\s*$)",
                          boost::regex::ECMAScript | boost::regex::icase | boost::regex::optimize);

// regex for SELECT *
// Note that parens around whole string are not part of the regex but raw string literal
boost::regex _selectRe(R"(^select\s+.+$)",
                       boost::regex::ECMAScript | boost::regex::icase | boost::regex::optimize);

// regex for FLUSH QSERV_CHUNKS_CACHE [FOR database]
// Note that parens around whole string are not part of the regex but raw string literal
// db name will be in group 3.
boost::regex _flushEmptyRe(R"(^flush\s+qserv_chunks_cache(\s+for\s+(["`]?)(\w+)\2)?\s*;?\s*$)",
                           boost::regex::ECMAScript | boost::regex::icase | boost::regex::optimize);
}

namespace lsst {
namespace qserv {
namespace ccontrol {

/// Returns true if query is DROP DATABASE
bool
UserQueryType::isDropDb(std::string const& query, std::string& dbName) {
    LOGS(_log, LOG_LVL_DEBUG, "isDropDb: " << query);
    boost::smatch sm;
    bool match = boost::regex_match(query, sm, _dropDbRe);
    if (match) {
        dbName = sm.str(3);
        LOGS(_log, LOG_LVL_DEBUG, "isDropDb: match: " << dbName);
    }
    return match;
}

/// Returns true if query is DROP TABLE
bool
UserQueryType::isDropTable(std::string const& query, std::string& dbName, std::string& tableName) {
    LOGS(_log, LOG_LVL_DEBUG, "isDropTable: " << query);
    boost::smatch sm;
    bool match = boost::regex_match(query, sm, _dropTableRe);
    if (match) {
        dbName = sm.str(3);
        tableName = sm.str(5);
        LOGS(_log, LOG_LVL_DEBUG, "isDropTable: match: " << dbName << "." << tableName);
    }
    return match;
}

/// Returns true if query is SELECT
bool
UserQueryType::isSelect(std::string const& query) {
    LOGS(_log, LOG_LVL_DEBUG, "isSelect: " << query);
    boost::smatch sm;
    bool match = boost::regex_match(query, sm, _selectRe);
    if (match) {
        LOGS(_log, LOG_LVL_DEBUG, "isSelect: match");
    }
    return match;
}

/// Returns true if query is FLUSH QSERV_CHUNKS_CACHE [FOR database]
bool
UserQueryType::isFlushChunksCache(std::string const& query, std::string& dbName) {
    LOGS(_log, LOG_LVL_DEBUG, "isFlushChunksCache: " << query);
    boost::smatch sm;
    bool match = boost::regex_match(query, sm, _flushEmptyRe);
    if (match) {
        dbName = sm.str(3);
        LOGS(_log, LOG_LVL_DEBUG, "isFlushChunksCache: match: " << dbName);
    }
    return match;
}

}}} // namespace lsst::qserv::ccontrol
