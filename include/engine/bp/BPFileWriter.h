/*
 * BPWriter.h
 *
 *  Created on: Dec 16, 2016
 *      Author: wfg
 */

#ifndef BPFILEWRITER_H_
#define BPFILEWRITER_H_

#include "core/Engine.h"
#include "format/BP1Writer.h"

//supported capsules
#include "capsule/heap/STLVector.h"


namespace adios
{

class BPFileWriter : public Engine
{

public:

    /**
     * Constructor for Writer writes in BP format into a single heap capsule, manages several transports
     * @param name unique name given to the engine
     * @param accessMode
     * @param mpiComm
     * @param method
     * @param debugMode
     */
    BPFileWriter( ADIOS& adios, const std::string name, const std::string accessMode, MPI_Comm mpiComm,
                  const Method& method, const bool debugMode = false, const unsigned int cores = 1 );

    ~BPFileWriter( );

    void Write( Variable<char>& variable, const char* values );
    void Write( Variable<unsigned char>& variable, const unsigned char* values );
    void Write( Variable<short>& variable, const short* values );
    void Write( Variable<unsigned short>& variable, const unsigned short* values );
    void Write( Variable<int>& variable, const int* values );
    void Write( Variable<unsigned int>& variable, const unsigned int* values );
    void Write( Variable<long int>& variable, const long int* values );
    void Write( Variable<unsigned long int>& variable, const unsigned long int* values );
    void Write( Variable<long long int>& variable, const long long int* values );
    void Write( Variable<unsigned long long int>& variable, const unsigned long long int* values );
    void Write( Variable<float>& variable, const float* values );
    void Write( Variable<double>& variable, const double* values );
    void Write( Variable<long double>& variable, const long double* values );
    void Write( Variable<std::complex<float>>& variable,       const std::complex<float>* values );
    void Write( Variable<std::complex<double>>& variable,      const std::complex<double>* values );
    void Write( Variable<std::complex<long double>>& variable, const std::complex<long double>* values );
    void Write( VariableCompound& variable, const void* values );

    void Write( const std::string variableName, const char* values );
    void Write( const std::string variableName, const unsigned char* values );
    void Write( const std::string variableName, const short* values );
    void Write( const std::string variableName, const unsigned short* values );
    void Write( const std::string variableName, const int* values );
    void Write( const std::string variableName, const unsigned int* values );
    void Write( const std::string variableName, const long int* values );
    void Write( const std::string variableName, const unsigned long int* values );
    void Write( const std::string variableName, const long long int* values );
    void Write( const std::string variableName, const unsigned long long int* values );
    void Write( const std::string variableName, const float* values );
    void Write( const std::string variableName, const double* values );
    void Write( const std::string variableName, const long double* values );
    void Write( const std::string variableName, const std::complex<float>* values );
    void Write( const std::string variableName, const std::complex<double>* values );
    void Write( const std::string variableName, const std::complex<long double>* values );
    void Write( const std::string variableName, const void* values );



    void Advance( );

    /**
     * Closes a single transport or all transports
     * @param transportIndex, if -1 (default) closes all transports, otherwise it closes a transport in m_Transport[transportIndex]. In debug mode the latter is bounds-checked.
     */
    void Close( const int transportIndex = -1 );


private:

    capsule::STLVector m_Buffer; ///< heap capsule using STL std::vector<char>
    format::BP1Writer m_BP1Writer; ///< format object will provide the required BP functionality to be applied on m_Buffer and m_Transports
    format::BP1MetadataSet m_MetadataSet; ///< metadata set accompanying the heap buffer data in bp format. Needed by m_BP1Writer

    bool m_IsFirstClose = true; ///< set to false after first Close is reached so metadata doesn't have to be accommodated for a subsequent Close
    std::size_t m_MaxBufferSize; ///< maximum allowed memory to be allocated
    float m_GrowthFactor = 1.5; ///< capsule memory growth factor, new_memory = m_GrowthFactor * current_memory

    bool m_TransportFlush = false; ///< true: transport flush happened, buffer must be reset

    void Init( );
    void InitParameters( );
    void InitTransports( );
    void InitProcessGroup( );

    void WriteProcessGroupIndex( );



    /**
     * Common function for primitive (including std::complex) writes
     * @param group
     * @param variableName
     * @param variable
     */
    template< class T >
    void WriteVariableCommon( Variable<T>& variable, const T* values )
    {
        //set variable
        variable.m_AppValues = values;
        m_WrittenVariables.insert( variable.m_Name );

        //pre-calculate new metadata and payload sizes
        m_TransportFlush = CheckBuffersAllocation( m_BP1Writer.GetVariableIndexSize( variable ), variable.PayLoadSize(),
                                                   m_GrowthFactor, m_MaxBufferSize,
                                                   m_MetadataSet.VarsIndexPosition, m_MetadataSet.VarsIndex,
                                                   m_Buffer.m_DataPosition, m_Buffer.m_Data );

        //WRITE INDEX to data buffer and metadata structure (in memory)//
        m_BP1Writer.WriteVariableIndex( variable, m_Buffer, m_MetadataSet );

        if( m_TransportFlush == true ) //in batches
        {
            //write pg index

            //flush to transports

            //reset relative positions to zero, update absolute position

        }
        else //Write data to buffer
        {
            m_BP1Writer.WriteVariablePayload( variable, m_Buffer, m_Cores );
        }

        variable.m_AppValues = nullptr; //setting pointer to null as not needed after write
    }

};


} //end namespace adios


#endif /* BPFILEWRITER_H_ */