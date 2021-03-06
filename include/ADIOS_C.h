/*
 * ADIOS_C.h  "C" interface to ADIOS C++ class. This header defines the C API
 *
 *  Created on: Oct 31, 2016
 *      Author: wfg
 */

#ifndef ADIOS_C_H_
#define ADIOS_C_H_

#ifdef HAVE_MPI
  #include <mpi.h>
#else
  #include "../mpidummy.h"
  using adios::MPI_Comm_rank;
  using adios::MPI_Comm;
#endif



typedef void ADIOS;
typedef void Group;
typedef void Method;
typedef void Engine;



#ifdef __cplusplus
extern "C"
{
#endif


void adios_init( const char* xmlConfigFile, const MPI_Comm mpicomm );
void adios_init_debug( const char* xmlConfigFile, const MPI_Comm mpicomm );

int adios_open( const char* fileName, const char* accessMode, MPI_Comm );
void adios_write(const char* groupName, const char* variableName, const void* values  );

void adios_close( const int handler );

void adios_finalize( const ADIOS* adiosC ); // deallocate ADIOS pointer

void adios_monitor_groups( const ADIOS* adiosC );

void adios_monitor_groups_file( const ADIOS* adiosC, const char* fileName, const char* mode );



#ifdef __cplusplus
} //end extern C
#endif

#endif /* ADIOS_C_H_ */
