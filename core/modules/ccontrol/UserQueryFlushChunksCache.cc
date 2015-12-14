/*
 * LSST Data Management System
 * Copyright 2015 AURA/LSST.
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
#include "ccontrol/UserQueryFlushChunksCache.h"

// System headers

// LSST headers
#include "lsst/log/Log.h"

// Qserv headers
#include "css/CssAccess.h"
#include "css/EmptyChunks.h"
#include "qdisp/MessageStore.h"
#include "sql/SqlConnection.h"
#include "sql/SqlErrorObject.h"

namespace {

LOG_LOGGER getLogger() {
    static LOG_LOGGER logger = LOG_GET("lsst.qserv.ccontrol.UserQueryFlushChunksCache");
    return logger;
}

}

namespace lsst {
namespace qserv {
namespace ccontrol {

// Constructor
UserQueryFlushChunksCache::UserQueryFlushChunksCache(std::shared_ptr<css::CssAccess> const& css,
                                                     std::string const& dbName,
                                                     sql::SqlConnection* resultDbConn,
                                                     std::string const& resultTable)
    : _css(css), _dbName(dbName), _resultDbConn(resultDbConn),
      _resultTable(resultTable),  _qState(UNKNOWN),
      _messageStore(std::make_shared<qdisp::MessageStore>()) {
}

std::string UserQueryFlushChunksCache::getError() const {
    return std::string();
}

// Attempt to kill in progress.
void UserQueryFlushChunksCache::kill() {
}

// Submit or execute the query.
void UserQueryFlushChunksCache::submit() {

    LOGF(getLogger(), LOG_LVL_INFO, "going to flush empty chunks - db: %s" % _dbName);

    // create result table first, exact schema does not matter but mysql
    // needs at least one column in table DDL
    LOGF(getLogger(), LOG_LVL_DEBUG, "creating result table: %s" % _resultTable);
    std::string sql = "CREATE TABLE " + _resultTable + " (CODE INT)";
    sql::SqlErrorObject sqlErr;
    if (not _resultDbConn->runQuery(sql, sqlErr)) {
        // There is no way to return success if we cannot create result table so just stop here
        std::string message = "Failed to create result table: " + sqlErr.errMsg();
        _messageStore->addMessage(-1, 1005, message, MessageSeverity::MSG_ERROR);
        _qState = ERROR;
        return;
    }

    // reset empty chunk cache , this does not throw
    _css->getEmptyChunks().clearCache(_dbName);

    _qState = SUCCESS;
}

// Block until a submit()'ed query completes.
QueryState UserQueryFlushChunksCache::join() {
    // everything should be done in submit()
    return _qState;
}

// Release resources.
void UserQueryFlushChunksCache::discard() {
    // no resources
}

}}} // lsst::qserv::ccontrol