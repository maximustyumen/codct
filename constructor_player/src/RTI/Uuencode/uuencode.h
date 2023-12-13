/****************************************************************
*                                                               *
* Sample UUENCODE and UUDECODE routines.                        *
* Copyright 1995, Socket Communications.                        *
*                                                               *
* These routines may be freely copied and distributed.          *
*                                                               *
****************************************************************/
/*slightly modified by chitchat, 3/20/2000 */
//http://www.koders.com/cpp/fidC887DC940EB34D678E16659F3F512842C78B6DCB.aspx

//example use
/*
 const std::string s = "ADP GmbH\nAnalyse Design & Programmierung\nGesellschaft mit beschränkter Haftung" ;

  std::string encoded = base64_encode(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
  std::string decoded = base64_decode(encoded);

  std::cout << "encoded: " << encoded << std::endl;
  std::cout << "decoded: " << decoded << std::endl;

 */


#ifndef UUENCODE_H
#define UUENCODE_H

#include <stdlib.h>
#include <string.h>
#include <string>


class Uuencode
{
public:
    Uuencode();

    static std::string base64_encode(unsigned char const* , unsigned int len);
    static std::string base64_decode(std::string const& s);

    static bool is_base64(unsigned char c);


};

#endif // UUENCODE_H
