/*
 * SSupport.h
 *
 *  Created on: Oct 10, 2016
 *      Author: wfg
 */

#ifndef SUPPORT_H_
#define SUPPORT_H_

/// \cond EXCLUDE_FROM_DOXYGEN
#include <set>
#include <string>
#include <map>
#include <array>
/// \endcond

namespace adios
{


struct Support
{
    static const std::string Version; ///< current ADIOS version
    static const std::set<std::string> HostLanguages; ///< supported languages: C, C++, Fortran, Python, Java
    static const std::set<std::string> Numbers;
    static const std::set<std::string> Transports; ///< supported transport methods
    static const std::set<std::string> Transforms; ///< supported data transform methods
    static const std::map<std::string, std::set<std::string> > Datatypes; ///< supported data types, key: host language, value: all supported types
    static const std::map<std::string, std::set<std::string> > DatatypesAliases; ///< all supported int aliases, key: C++ type (e.g. int), value: aliases to type in key (e.g. int, integer)

    static const std::set<std::string> FileTransports; ///< file I/O transports
};


} //end namespace


#endif /* SUPPORT_H_ */
