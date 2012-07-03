// -*- LSST-C++ -*-
/* 
 * LSST Data Management System
 * Copyright 2012 LSST Corporation.
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
// PluginNotFoundError is an exception class thrown when a plugin is requested
// by a name that has not been registered.


#ifndef LSST_QSERV_MASTER_PLUGINNOTFOUNDERROR_H
#define LSST_QSERV_MASTER_PLUGINNOTFOUNDERROR_H
#include <exception>
#include <string>
#include <sstream>

namespace lsst { namespace qserv { namespace master {

class PluginNotFoundError: public std::exception {
public:
    explicit PluginNotFoundError(std::string const& name) {
        std::stringstream ss;
        ss << "PluginNotFoundError '" << name << "' requested but not found.";
        _descr = ss.str();
    }
    virtual ~PluginNotFoundError() throw() {}

    virtual const char* what() const throw() {
        return _descr.c_str();
    }
private:
    std::string _descr;
};

}}} // namespace lsst::qserv::master


#endif // LSST_QSERV_MASTER_PLUGINNOTFOUNDERROR_H

