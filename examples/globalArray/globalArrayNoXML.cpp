/*
 * globalArrayNoXML.cpp
 *
 *  Created on: Oct 31, 2016
 *      Author: pnorbert
 */

#include <stdexcept>
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../../include/ADIOS.h"

int main( int argc, char* argv [] )
{
    int         rank, size;
    const int   NX = 10;
    double      t[NX];
    std::vector<double> p(NX);
    MPI_Comm    comm=MPI_COMM_WORLD;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (comm, &rank);
    MPI_Comm_size (comm, &size);

    try
    {
        // ADIOS manager object creation. MPI must be initialized
        adios::ADIOS adios( "globalArrayNoXML.xml", comm, true );

        // set a maximum buffersize that ADIOS can use (for one group).
        // multiple groups may use each such buffersize if they are overlapped or
        // use time-aggregation or use zero-copy staging

        // Define Group and its variables
        // The group's transport can be defined at runtime in the input XML file
        adios.CreateGroup ("arrays");

        // Set the maximum buffersize for this group. This must happen before
        // defining a zero copy variable, which will cause the group's internal buffer
        // allocated (during the variable definition call)
        //adios.SetMaxBuffersize ("arrays", 10000000);

        adios.CreateVariable ("arrays", "NX", "int"); // scalar variable
        adios.CreateVariable ("arrays", "size", "int");
        adios.CreateVariable ("arrays", "size", "rank");

        // define a 2D array with 1D decomposition
        adios.CreateVariable ("arrays", "temperature", "double", "1,NX", "NONE", "size,NX", "rank,0");

//        // set a variable-level transformation for this variable
//        adios.SetVariableTransform ("arrays", "temperature", "none");

        adios.CreateVariable ("arrays", "pressure", "std::vector<double>", "1,NX", "size,NX", "rank,0" );

        // set a group-level transport
        adios.SetTransport ( "arrays", "time-aggregate" ); // no options passed here

        //Get Monitor info
        std::ofstream logStream( "info_" + std::to_string(rank) + ".log" );
        adios.MonitorGroups( logStream );

        adios.Open("arrays", "globalArray.bp", "w", 100000000 );

        for (int it = 1; it <= 13; it++)
        {

            for (int i = 0; i < NX; i++)
            {
                t[i] = it*100.0 + rank*NX + i;
                p[i] = it*1000.0 + rank*NX + i;
            }

//            if (it==1)
//
//            else
//                adios.Open("arrays", "globalArray.bp", "a", 100000000 );

            //uint64_t    adios_groupsize, adios_totalsize;
            // adios_groupsize = 4 + 4 + 4 + 2*NX*sizeof(double);
            // adios_totalsize = adios.GroupSize("arrays", adios_groupsize);

            adios.Write ("arrays", "NX", &NX);
            adios.Write ("arrays", "size", &size);
            adios.Write ("arrays", "rank", &rank);
            adios.Write ("arrays", "temperature", t);
            adios.Write ("arrays", "pressure", &p);

            MPI_Barrier (comm);
            if (rank==0) printf("Timestep %d written\n", it);
        }

        adios.Close ("arrays");

        MPI_Barrier (comm);
        // need barrier before we destroy the ADIOS object here automatically
        if (rank==0) printf("Finalize adios\n");
    }
    catch( std::exception& e ) //need to think carefully how to handle C++ exceptions with MPI to avoid deadlocking
    {
        if( rank == 0 )
        {
            std::cout << e.what() << "\n";
        }
    }

    if (rank==0) printf("Finalize MPI\n");
    MPI_Finalize ();
    return 0;
}
