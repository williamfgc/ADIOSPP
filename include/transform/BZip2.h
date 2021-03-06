/*
 * BZip2.h
 *
 *  Created on: Oct 17, 2016
 *      Author: wfg
 */

#ifndef BZIP2_H_
#define BZIP2_H_


#include "core/Transform.h"


namespace adios
{
namespace transform
{


class BZIP2 : public Transform
{

public:

    /**
     * Initialize parent method
     * @param compressionLevel
     * @param variable
     */
    BZIP2( );

    ~BZIP2( );

    void Compress( const std::vector<char>& bufferIn, std::vector<char>& bufferOut );

    void Decompress( const std::vector<char>& bufferIn, std::vector<char>& bufferOut );
};


} //end namespace transform
} //end namespace adios



#endif /* BZIP2_H_ */
