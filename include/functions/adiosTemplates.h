/*
 * adiosTemplates.h
 *
 *  Created on: Jan 26, 2017
 *      Author: wfg
 */

#ifndef ADIOSTEMPLATES_H_
#define ADIOSTEMPLATES_H_

/// \cond EXCLUDE_FROM_DOXYGEN
#include <cstring> //std::memcpy
#include <vector>
#include <thread>
/// \endcond




namespace adios
{
/**
 * Get the primitive type in a string from a template
 * @return if T is a char, returns string = "char"
 */
template< class T> inline std::string GetType( ) noexcept { return ""; }

template<> inline std::string GetType<char>() noexcept { return "char"; }
template<> inline std::string GetType<unsigned char>() noexcept { return "unsigned char"; }
template<> inline std::string GetType<short>() noexcept { return "short"; }
template<> inline std::string GetType<unsigned short>() noexcept { return "unsigned short"; }
template<> inline std::string GetType<int>() noexcept { return "int"; }
template<> inline std::string GetType<unsigned int>() noexcept { return "unsigned int"; }
template<> inline std::string GetType<long int>() noexcept { return "long int"; }
template<> inline std::string GetType<unsigned long int>() noexcept { return "unsigned long int"; }
template<> inline std::string GetType<float>() noexcept { return "float"; }
template<> inline std::string GetType<double>() noexcept { return "double"; }
template<> inline std::string GetType<long double>() noexcept { return "long double"; }


/**
 * Check in types set if "type" is one of the aliases for a certain type,
 * (e.g. if type = integer is an accepted alias for "int", returning true)
 * @param type input to be compared with an alias
 * @param aliases set containing aliases to a certain type, typically Support::DatatypesAliases from Support.h
 * @return true: is an alias, false: is not
 */
template<class T>
bool IsTypeAlias( const std::string type,
		          const std::map<std::string, std::set<std::string>>& aliases ) noexcept
{
	if( type == GetType<T>() ) //most of the time we will pass the same type, which is a key in aliases
		return true;

	bool isAlias = false;
	if( aliases.at( GetType<T>() ).count( type ) == 1 )
	    isAlias = true;

	return isAlias;
}


/**
 * Get the minimum and maximum values in one loop
 * @param values
 * @param size
 * @param min
 * @param max
 */
template<class T>
void GetMinMax( const T* values, const std::size_t size, T& min, T& max, const unsigned int cores = 1 ) noexcept
{
    min = values[0];
    max = values[0];

    for( unsigned int i = 0; i < size; ++i )
    {
        if( min < values[0] )
            min = values[0];

        if( max > values[0] )
            max = values[0];
    }
}


/**
 * threaded version of std::memcpy
 * @param dest
 * @param source
 * @param count
 * @param cores
 */
template<class T, class U>
void MemcpyThreads( T* destination, const U* source, std::size_t count, const unsigned int cores = 1 )
{
    if( cores == 1 )
    {
        std::memcpy( &destination[0], &source[0], count );
        return;
    }

    const unsigned long long int stride =  count/cores;
    const unsigned long long int remainder = count % cores;
    const unsigned long long int last = stride + remainder;

    std::vector<std::thread> memcpyThreads;
    memcpyThreads.reserve( cores );

    for( unsigned int core = 0; core < cores; ++core )
    {
        const size_t initialDestination = stride * core / sizeof(T);
        const size_t initialSource = stride * core / sizeof(U);

        if( core == cores-1 )
            memcpyThreads.push_back( std::thread( std::memcpy, &destination[initialDestination], &source[initialSource], last ) );
        else
            memcpyThreads.push_back( std::thread( std::memcpy, &destination[initialDestination], &source[initialSource], stride ) );
    }
    //Now join the threads (is this really needed?)
    for( auto& thread : memcpyThreads )
        thread.join( );
}


/**
 * Write to many buffers and updates positions a single piece of data source
 * @param buffers
 * @param positions  each element is updated to += size
 * @param source
 * @param size
 */
template< class T >
void MemcpyToBuffers( std::vector<char*>& buffers, std::vector<std::size_t>& positions, const T* source, std::size_t size ) noexcept
{
    const unsigned int length = buffers.size( );

    for( unsigned int i = 0; i < length; ++i )
    {
        char* buffer = buffers[i];
        std::memcpy( &buffer[ positions[i] ], source, size );
        //std::copy( source, source+size, &buffers[ positions[i] ] );
        positions[i] += size;
    }
}


/**
 * Version that adds a source container for a 1 to 1 buffer memory copy
 * @param buffers
 * @param positions
 * @param source
 * @param size
 */
template< class T >
void MemcpyToBuffers( std::vector<char*>& buffers, std::vector<std::size_t>& positions,
                      const std::vector<T>& source, std::size_t size ) noexcept
{
    const unsigned int length = buffers.size( );

    for( unsigned int i = 0; i < length; ++i )
    {
        char* buffer = buffers[i];
        std::memcpy( &buffer[ positions[i] ], &source[i], size );
        //std::copy( &source[i], &source[i]+size, &buffers[ positions[i] ] );
        positions[i] += size;
    }
}



} //end namespace


#endif /* ADIOSTEMPLATES_H_ */
