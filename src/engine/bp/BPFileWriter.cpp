/*
 * BPFileWriter.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: wfg
 */

#include "engine/bp/BPFileWriter.h"
#include "ADIOS.h"

//supported transports
#include "transport/file/FileDescriptor.h"
#include "transport/file/FilePointer.h"
#include "transport/file/FStream.h"

namespace adios
{


BPFileWriter::BPFileWriter( ADIOS& adios, const std::string name, const std::string accessMode, MPI_Comm mpiComm,
                            const Method& method, const bool debugMode, const unsigned int cores ):
    Engine( adios, "BPFileWriter", name, accessMode, mpiComm, method, debugMode, cores, " BPFileWriter constructor (or call to ADIOS Open).\n" ),
    m_Buffer{ capsule::STLVector( accessMode, m_RankMPI, m_DebugMode ) },
    m_MaxBufferSize{ m_Buffer.m_Data.max_size() }
{
    Init( );
}


BPFileWriter::~BPFileWriter( )
{ }


void BPFileWriter::Init( )
{
    InitTransports( );
    InitProcessGroup( );
}


void BPFileWriter::Write( Variable<char>& variable, const char* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<unsigned char>& variable, const unsigned char* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<short>& variable, const short* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<unsigned short>& variable, const unsigned short* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<int>& variable, const int* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<unsigned int>& variable, const unsigned int* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<long int>& variable, const long int* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<unsigned long int>& variable, const unsigned long int* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<long long int>& variable, const long long int* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<unsigned long long int>& variable, const unsigned long long int* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<float>& variable, const float* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<double>& variable, const double* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<long double>& variable, const long double* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<std::complex<float>>& variable, const std::complex<float>* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<std::complex<double>>& variable, const std::complex<double>* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( Variable<std::complex<long double>>& variable, const std::complex<long double>* values )
{ WriteVariableCommon( variable, values ); }

void BPFileWriter::Write( VariableCompound& variable, const void* values )
{ }

//String version
void BPFileWriter::Write( const std::string variableName, const char* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<char>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const unsigned char* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<unsigned char>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const short* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<short>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const unsigned short* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<unsigned short>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const int* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<int>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const unsigned int* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<unsigned int>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const long int* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<long int>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const unsigned long int* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<unsigned long int>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const long long int* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<long long int>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const unsigned long long int* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<unsigned long long int>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const float* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<float>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const double* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<double>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const long double* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<long double>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const std::complex<float>* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<std::complex<float>>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const std::complex<double>* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<std::complex<double>>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const std::complex<long double>* values )
{ WriteVariableCommon( m_ADIOS.GetVariable<std::complex<long double>>( variableName ), values ); }

void BPFileWriter::Write( const std::string variableName, const void* values )
{ }


void BPFileWriter::Advance( )
{
    //first close current pg
}


void BPFileWriter::Close( const int transportIndex )
{
    CheckTransportIndex( transportIndex );
    if( transportIndex == -1 )
    {
        for( auto& transport : m_Transports ) //by reference or value or it doesn't matter?
            m_BP1Writer.Close( m_MetadataSet, m_Buffer, *transport, m_IsFirstClose );
    }
    else
    {
        m_BP1Writer.Close( m_MetadataSet, m_Buffer, *m_Transports[transportIndex], m_IsFirstClose );
    }
}


//PRIVATE FUNCTIONS
void BPFileWriter::InitParameters( )
{
    auto itGrowthFactor = m_Method.m_Parameters.find( "buffer_growth" );
    if( itGrowthFactor != m_Method.m_Parameters.end() )
    {
        const float growthFactor = std::stof( itGrowthFactor->second );
        if( m_DebugMode == true )
        {
            if( growthFactor == 1.f )
                throw std::invalid_argument( "ERROR: buffer_growth argument can't be less of equal than 1, in " + m_EndMessage + "\n" );
        }

        m_BP1Writer.m_GrowthFactor = growthFactor;
        m_GrowthFactor = growthFactor; //float
    }

    auto itMaxBufferSize = m_Method.m_Parameters.find( "max_size_MB" );
    if( itMaxBufferSize != m_Method.m_Parameters.end() )
    {
        if( m_DebugMode == true )
        {
            if( m_GrowthFactor <= 1.f )
                throw std::invalid_argument( "ERROR: Method buffer_growth argument can't be less of equal than 1, in " + m_EndMessage + "\n" );
        }

        m_MaxBufferSize = std::stoul( itMaxBufferSize->second ) * 1048576; //convert to bytes
    }

    auto itVerbosity = m_Method.m_Parameters.find( "verbose" );
    if( itVerbosity != m_Method.m_Parameters.end() )
    {
        int verbosity = std::stoi( itVerbosity->second );
        if( m_DebugMode == true )
        {
            if( verbosity < 0 || verbosity > 5 )
                throw std::invalid_argument( "ERROR: Method verbose argument must be an integer in the range [0,5], in call to Open or Engine constructor\n" );
        }
        m_BP1Writer.m_Verbosity = verbosity;
    }

    auto itProfile = m_Method.m_Parameters.find( "profile_buffering_units" );
    if( itProfile != m_Method.m_Parameters.end() )
    {
        if( itProfile->second == "mus" || itProfile->second == "microseconds" )
            m_Profiler.m_Timers.emplace_back( "Buffering", Support::Resolutions::mus );

        else if( itProfile->second == "ms" || itProfile->second == "milliseconds" )
            m_Profiler.m_Timers.emplace_back( "Buffering", Support::Resolutions::ms );

        else if( itProfile->second == "s" || itProfile->second == "seconds" )
            m_Profiler.m_Timers.emplace_back( "Buffering", Support::Resolutions::s );

        else if( itProfile->second == "min" || itProfile->second == "minutes" )
            m_Profiler.m_Timers.emplace_back( "Buffering", Support::Resolutions::m );

        else if( itProfile->second == "h" || itProfile->second == "hours" )
            m_Profiler.m_Timers.emplace_back( "Buffering", Support::Resolutions::h );
        else
        {
            if( m_DebugMode == true )
                throw std::invalid_argument( "ERROR: Method profile_buffering_units argument must be mus, ms, s, min or h, in call to Open or Engine constructor\n" );
        }

        m_Profiler.m_IsActive = true;
    }
}


void BPFileWriter::InitTransports( )
{
    if( m_DebugMode == true )
    {
        if( TransportNamesUniqueness( ) == false )
        {
            throw std::invalid_argument( "ERROR: two transports of the same kind (e.g file IO) "
                                         "can't have the same name, modify with name= in Method AddTransport\n" );
        }
    }

    for( const auto& parameters : m_Method.m_TransportParameters )
    {
        auto itProfile = parameters.find( "profile_units" );
        Support::Resolutions resolution = Support::Resolutions::s; //default is seconds
        if( itProfile != parameters.end() )
        {
            if( itProfile->second == "mus" || itProfile->second == "microseconds" )
                resolution = Support::Resolutions::mus;

            else if( itProfile->second == "ms" || itProfile->second == "milliseconds" )
                resolution = Support::Resolutions::ms;

            else if( itProfile->second == "s" || itProfile->second == "seconds" )
                resolution = Support::Resolutions::s;

            else if( itProfile->second == "min" || itProfile->second == "minutes" )
                resolution = Support::Resolutions::m;

            else if( itProfile->second == "h" || itProfile->second == "hours" )
                resolution = Support::Resolutions::h;

            else
            {
                if( m_DebugMode == true )
                    throw std::invalid_argument( "ERROR: Transport profile_units argument must be mus, ms, s, min or h " + m_EndMessage );
            }
        }

        auto itTransport = parameters.find( "transport" );

        if( itTransport->second == "file" || itTransport->second == "File" )
        {
            auto itLibrary = parameters.find( "library" );
            if( itLibrary == parameters.end() || itLibrary->second == "POSIX" ) //use default POSIX
            {
                auto file = std::make_shared<transport::FileDescriptor>( m_MPIComm, m_DebugMode );
                if( parameters.count( "profile_units" ) == 1 )
                    file->InitProfiler( resolution );

                m_BP1Writer.OpenRankFiles( m_Name, m_AccessMode, *file );
                m_Transports.push_back( std::move( file ) );

            }
            else if( itLibrary->second == "FILE*" || itLibrary->second == "stdio" )
            {
                auto file = std::make_shared<transport::FilePointer>( m_MPIComm, m_DebugMode );
                if( parameters.count( "profile_units" ) == 1 )
                    file->InitProfiler( resolution );

                m_BP1Writer.OpenRankFiles( m_Name, m_AccessMode, *file );
                m_Transports.push_back( std::move( file ) );
            }
            else if( itLibrary->second == "fstream" || itLibrary->second == "std::fstream" )
            {
                auto file = std::make_shared<transport::FStream>( m_MPIComm, m_DebugMode );
                if( parameters.count( "profile_units" ) == 1 )
                    file->InitProfiler( resolution );

                m_BP1Writer.OpenRankFiles( m_Name, m_AccessMode, *file );
                m_Transports.push_back( std::move( file ) );
            }
            else if( itLibrary->second == "MPI_File" || itLibrary->second == "MPI-IO" )
            {

            }
            else
            {
                if( m_DebugMode == true )
                    throw std::invalid_argument( "ERROR: file transport library " + itLibrary->second + " not supported, in " +
                                                  m_Name + m_EndMessage );
            }
        }
        else
        {
            if( m_DebugMode == true )
                throw std::invalid_argument( "ERROR: transport " + itTransport->second + " (you mean File?) not supported, in " +
                                              m_Name + m_EndMessage );
        }
    }
}


void BPFileWriter::InitProcessGroup( )
{
    if( m_AccessMode == "a" )
    {
        //Get last pg timestep and update timestep counter in format::BP1MetadataSet
    }
    WriteProcessGroupIndex( );
}



void BPFileWriter::WriteProcessGroupIndex( )
{
    //pg = process group
    const std::string name( std::to_string( m_RankMPI ) ); //using rank as name
    const unsigned int timeStep = m_MetadataSet.TimeStep;
    const std::string timeStepName( std::to_string( timeStep ) );
    const std::size_t pgIndexSize = m_BP1Writer.GetProcessGroupIndexSize( name, timeStepName, m_Transports.size() );

    //metadata
    GrowBuffer( pgIndexSize, m_GrowthFactor, m_MetadataSet.PGIndexPosition, m_MetadataSet.PGIndex );

    //data? Need to be careful, maybe add some trailing tolerance in variable ????
    GrowBuffer( pgIndexSize, m_GrowthFactor, m_Buffer.m_DataPosition, m_Buffer.m_Data );

    const bool isFortran = ( m_HostLanguage == "Fortran" ) ? true : false;
    const unsigned int processID = static_cast<unsigned int> ( m_RankMPI );

    m_BP1Writer.WriteProcessGroupIndex( isFortran, name, processID, timeStepName, timeStep, m_Transports,
                                        m_Buffer, m_MetadataSet );

}



} //end namespace adios