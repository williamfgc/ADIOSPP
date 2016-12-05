/*
 * capsuleTemplates.h
 *
 *  Created on: Nov 18, 2016
 *      Author: wfg
 */

#ifndef CAPSULETEMPLATES_H_
#define CAPSULETEMPLATES_H_


/// \cond EXCLUDE_FROM_DOXYGEN
#include <cstring> //std::memcpy
#include <vector>
#include <thread>
/// \endcond


#include "core/Transport.h"


namespace adios
{

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
    //Now join the threads
    for( auto& thread : memcpyThreads )
        thread.join( );
}


/**
 *
 * @param data
 * @param size
 * @param transportIndex
 * @param transports
 * @param maxBufferSize
 * @param buffer
 */
template<class T>
void WriteToBuffer( const T* data, const size_t size, const int transportIndex,
                    std::vector< std::shared_ptr<Transport> >& transports,
                    const size_t maxBufferSize, std::vector<char>& buffer )
{
    auto lf_TransportsWrite = []( const int transportIndex, std::vector< std::shared_ptr<Transport> >& transports,
                                  std::vector<char>& buffer )
    {
        if( transportIndex == -1 ) // all transports
        {
            for( auto& transport : transports )
                transport->Write( buffer );
        }
        else
            transports[ transportIndex ]->Write( buffer );
    };

    //FUNCTION starts here
    const size_t dataBytes = size * sizeof( T ); //size of data in bytes

    //check for DataMan transport
    if( transportIndex == -1 ) // all transports
    {
        for( auto& transport : transports )
        {
            if( transport->m_Method == "DataMan" ) //DataMan needs all the information
                buffer.resize( dataBytes ); //resize buffer to fit all data
        }
    }
    else //just one transport
    {
        if( transports[transportIndex]->m_Method == "DataMan" )
            buffer.resize( dataBytes ); //resize buffer to fit all data
    }

    if( dataBytes <= buffer.size() ) // dataBytes < buffer.size()
    {
        buffer.resize( dataBytes ); //this resize shouldn't change capacity or call realloc
        MemcpyThreads( &buffer[0], data, dataBytes, 1 ); //copy memory in threaded fashion, need to test with size, serial for now
        lf_TransportsWrite( transportIndex, transports, buffer );
        return;
    }

    if( buffer.size() < dataBytes && dataBytes <= maxBufferSize ) //  buffer.size() < dataBytes <  maxBufferSize
    {
        buffer.resize( dataBytes );
        MemcpyThreads( &buffer[0], data, dataBytes, 1 ); //copy memory in threaded fashion, need to test with size, serial for now
        lf_TransportsWrite( transportIndex, transports, buffer );
        return;
    }

    // dataBytes > maxBufferSize == buffer.size() split the variable in buffer buckets
    buffer.resize( maxBufferSize ); //resize to maxBufferSize, this might call realloc
    const size_t buckets =  dataBytes / maxBufferSize + 1;
    const size_t remainder = dataBytes % maxBufferSize;

    for( unsigned int bucket = 0; buckets < buckets; ++bucket )
    {
        const size_t dataOffset = bucket * maxBufferSize / sizeof( T );

        if( bucket == buckets-1 )
            MemcpyThreads( &buffer[0], data[dataOffset], remainder, 1 );
        else
            MemcpyThreads( &buffer[0], data[dataOffset], maxBufferSize, 1 );

        lf_TransportsWrite( transportIndex, transports, buffer );
    }
}



} //end namespace



#endif /* CAPSULETEMPLATES_H_ */