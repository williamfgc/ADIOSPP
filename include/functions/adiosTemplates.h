/*
 * adiosTemplates.h
 *
 *  Created on: Jan 26, 2017
 *      Author: wfg
 */

#ifndef ADIOSTEMPLATES_H_
#define ADIOSTEMPLATES_H_

#include <cstdint>

namespace adios
{

template<class T>
inline std::string GetType() noexcept
{
    return "";
}

// specializations

template<> inline std::string GetType<char>()          noexcept { return "char"; }
template<> inline std::string GetType<unsigned char>() noexcept { return "unsigned char"; }
template<> inline std::string GetType<int16_t>()       noexcept { return "short"; }
template<> inline std::string GetType<uint16_t>()      noexcept { return "unsigned short"; }
template<> inline std::string GetType<int32_t>()       noexcept { return "int"; }
template<> inline std::string GetType<uint32_t>()      noexcept { return "unsigned int"; }
template<> inline std::string GetType<int64_t>()       noexcept { return "long int"; }
template<> inline std::string GetType<uint64_t>()      noexcept { return "unsigned long int"; }
template<> inline std::string GetType<float>()         noexcept { return "float"; }
template<> inline std::string GetType<double>()        noexcept { return "double"; }
template<> inline std::string GetType<long double>()   noexcept { return "long double"; }

} //end namespace


#endif /* ADIOSTEMPLATES_H_ */
