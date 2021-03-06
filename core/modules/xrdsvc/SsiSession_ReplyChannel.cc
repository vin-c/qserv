// -*- LSST-C++ -*-
/*
 * LSST Data Management System
 * Copyright 2014-2016 LSST Corporation.
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

// Class header
#include "xrdsvc/SsiSession_ReplyChannel.h"

// LSST headers
#include "lsst/log/Log.h"

// Qserv headers
#include "util/Timer.h"
#include "xrdsvc/ChannelStream.h"

namespace {
LOG_LOGGER _log = LOG_GET("lsst.qserv.xrdsvc.SsiSession_ReplyChannel");
}

namespace lsst {
namespace qserv {
namespace xrdsvc {

bool
SsiSession::ReplyChannel::send(char const* buf, int bufLen) {
    Status s = _ssiSession.SetResponse(buf, bufLen);
    if (s != XrdSsiResponder::wasPosted) {
        LOGS(_log, LOG_LVL_ERROR, "DANGER: Couldn't post response of length=" << bufLen);
        return false;
    }
    return true;
}

bool
SsiSession::ReplyChannel::sendError(std::string const& msg, int code) {
    Status s = _ssiSession.SetErrResponse(msg.c_str(), code);
    if (s != XrdSsiResponder::wasPosted) {
        LOGS(_log, LOG_LVL_ERROR, "DANGER: Couldn't post error response " << msg);
        return false;
    }
    return true;
}

bool
SsiSession::ReplyChannel::sendFile(int fd, Size fSize) {
    util::Timer t;
    t.start();
    Status s = _ssiSession.SetResponse(fSize, fd);
    if (s == XrdSsiResponder::wasPosted) {
        LOGS(_log, LOG_LVL_DEBUG, "file posted ok");
    } else {
        if (s == XrdSsiResponder::notActive) {
            LOGS(_log, LOG_LVL_ERROR, "DANGER: Couldn't post response file of length="
                 << fSize << ", responder not active.");
        } else {
            LOGS(_log, LOG_LVL_ERROR, "DANGER: Couldn't post response file of length=" << fSize);
        }
        release();
        sendError("Internal error posting response file", 1);
        return false; // sendError handles everything else.
    }
    t.stop();
    LOGS(_log, LOG_LVL_DEBUG, "sendFile took " << t.getElapsed() << " seconds");
    return true;
}

bool
SsiSession::ReplyChannel::sendStream(char const* buf, int bufLen, bool last) {
    // Initialize streaming object if not initialized.
    LOGS(_log, LOG_LVL_DEBUG, "sendStream, checking stream " << (void *) _stream
         << " len=" << bufLen << " last=" << last);
    if (!_stream) {
        _initStream();
    } else if (_stream->closed()) {
        return false;
    }
    _stream->append(buf, bufLen, last);
    return true;
}

void
SsiSession::ReplyChannel::_initStream() {
    //_stream.reset(new Stream);
    _stream = new ChannelStream();
    _ssiSession.SetResponse(_stream);
}

}}} // lsst::qserv::xrdsvc
