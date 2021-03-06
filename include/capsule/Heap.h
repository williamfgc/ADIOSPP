/*
 * Heap.h
 *
 *  Created on: Dec 19, 2016
 *      Author: wfg
 */

#ifndef HEAP_H_
#define HEAP_H_


#include "core/Capsule.h"


namespace adios
{

/**
 * Data and Metadata buffers are allocated in the Heap
 */
class Heap : public Capsule
{

public:

    std::vector<char> m_Data; ///< data buffer allocated using the STL in heap memory, default size = 16 Mb
    std::vector<char> m_Metadata; ///< metadata buffer allocated using the STL in heap memory, default size = 100 Kb

    /**
     * Unique constructor
     * @param accessMode read, write or append
     * @param rankMPI MPI rank
     * @param debugMode true: extra checks, slower
     */
    Heap( const std::string accessMode, const int rankMPI, const bool debugMode = false );

    ~Heap( );

    char* GetData( );
    char* GetMetadata( );

    std::size_t GetDataSize( ) const;
    std::size_t GetMetadataSize( ) const;

    void ResizeData( const std::size_t size );
    void ResizeMetadata( const std::size_t size );

};





} //end namespace






#endif /* HEAP_H_ */
