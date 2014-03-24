/*
 * LSST Data Management System
 * Copyright 2013 LSST Corporation.
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
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
// queryMsg.cc houses the implementation of
// queryMsg.h (SWIG-exported functions for accessing QueryMessages)

#include "control/queryMsg.h"
#include <iostream>
#include "control/AsyncQueryManager.h"
#include "control/SessionManagerAsync.h"
#include "qdisp/MessageStore.h"


namespace lsst {
namespace qserv {
namespace control {        
    
int queryMsgGetCount(int session) {
    control::AsyncQueryManager& qm = getAsyncManager(session);
    boost::shared_ptr<qdisp::MessageStore> ms = qm.getMessageStore();
    return ms->messageCount();
}

// Python call: msg, chunkId, code, timestamp = queryMsgGetMsg(session, idx)
std::string queryMsgGetMsg(int session, int idx, int* chunkId, int* code, time_t* timestamp) {
    control::AsyncQueryManager& qm = getAsyncManager(session);
    boost::shared_ptr<qdisp::MessageStore> ms = qm.getMessageStore();
    qdisp::QueryMessage msg = ms->getMessage(idx);
    *chunkId = msg.chunkId;
    *code = msg.code;
    *timestamp = msg.timestamp;
    return msg.description;
}

void queryMsgAddMsg(int session, int chunkId, int code,
                            std::string const& message) {
    control::AsyncQueryManager& qm = getAsyncManager(session);
    boost::shared_ptr<qdisp::MessageStore> ms = qm.getMessageStore();
    ms->addMessage(chunkId, code, message);
}

}}} // namespace lsst::qserv::control
