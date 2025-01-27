// unCgi.cpp

/*===========================================================================
*
*                            PUBLIC DOMAIN NOTICE                          
*               National Center for Biotechnology Information
*                                                                          
*  This software/database is a "United States Government Work" under the   
*  terms of the United States Copyright Act.  It was written as part of    
*  the author's official duties as a United States Government employee and 
*  thus cannot be copyrighted.  This software/database is freely available 
*  to the public for use. The National Library of Medicine and the U.S.    
*  Government have not placed any restriction on its use or reproduction.  
*                                                                          
*  Although all reasonable efforts have been taken to ensure the accuracy  
*  and reliability of the software and data, the NLM and the U.S.          
*  Government do not and cannot warrant the performance or results that    
*  may be obtained by using this software or data. The NLM and the U.S.    
*  Government disclaim all warranties, express or implied, including       
*  warranties of performance, merchantability or fitness for any particular
*  purpose.                                                                
*                                                                          
*  Please cite the author in any work or product based on this material.   
*
* ===========================================================================
*
* Author: Vyacheslav Brover
*
* File Description:
*   Extract URL parameters
*
*/


#undef NDEBUG
#include "common.inc"

#include "common.hpp"
using namespace Common_sp;
#include "version.inc"



namespace 
{



inline bool isQuote (char c)  
  { return    c == '\'' 
           || c == '\"';
  }



struct ThisApplication : Application
{
  ThisApplication ()
    : Application ("Save HTTP parameters as files <prefix>_<parameter>")
	  // Why not <prefix>.<parameter> ??
  	{
      version = VERSION;
  	  addPositional("prefix", "string");
  	}



	void body () const final
  {
		const string prefix = getArg ("prefix");
		if (verbose ())
		  cout << prefix << endl;
  
  
  
    bool post = false;
    {
      const string RequestMethod (getenv ("REQUEST_METHOD"));
      ASSERT (! RequestMethod. empty ());
      if      (RequestMethod == "GET")
        post = false;
      else if (RequestMethod == "POST")
        post = true;
      else
        ERROR;
    }
  
  
    uint Len = 0;
    const string QueryString (getenv ("QUERY_STRING"));
    if (post)
    {
      const char* LenPtr = getenv ("CONTENT_LENGTH");
      ASSERT (LenPtr);
      if (verbose ())
        cout << string (LenPtr) << endl;
      if (sscanf (LenPtr, "%u", & Len) != 1)
        ERROR;
      if (verbose ())
        cout << Len << endl;
      if (! Len)
      	throw runtime_error ("Zero CONTENT_LENGTH");
    }
    else
    {
      if (QueryString. empty ())
      	throw runtime_error ("Empty QUERY_STRING");
    }
    
  
    size_t i = 0;
    const uint FNameLen = 255;
    char FName [FNameLen + 1];
    bool IsFName = true;
    FILE* F = nullptr;
    uint FNamePos = 0;
    bool Special = false;
    uint SpecialNum = 0;
    char SpecialChar [3] = "  ";
    for (;;)
    {
      if (post)
        if (i >= (size_t) Len)
          break;

      char c;
      if (post)
      {
        if (scanf ("%c", & c) != 1)
          ERROR;
      }
      else
      {
        c = QueryString [i];
        if (c == '\0')
          break;
      }
      if (verbose ())
        cout << i << ' ' << c << endl;

      if (Special)
      {
        ASSERT (! IsFName);
        ASSERT (SpecialNum < 2);
        SpecialChar [SpecialNum] = c;
        SpecialNum++;
        if (SpecialNum == 2)
        {
          uint N;
          if (sscanf (SpecialChar, "%x", & N) != 1)
            ERROR;
          if (N != '\x0D')  // Ignore Windows EOL 2nd character
          {
            c = (char) N;
          //ASSERT (! isQuote (c));
             // Insecure ??
            fprintf (F, "%c", c);
          }
          Special = false;
        }
      }
      else
        switch (c)
        {
          case '&':
            ASSERT (! IsFName);
            IsFName = true;
            FNamePos = 0;
            ASSERT (F);
            fclose (F);
            F = nullptr;
            break;

          case '+':
            ASSERT (! IsFName);
            fprintf (F, " ");
            break;

          case '=':
            if (IsFName)
            {
              IsFName = false;
              FName [FNamePos] = '\0';
              char PathName [1024];
              sprintf (PathName, "%s_%s", prefix. c_str (), FName);
              F = fopen (PathName, "w");
            //cout << PathName << endl;  
              ASSERT (F);
            }
            else
              fprintf (F, "%c", c);
            break;

          case '%':
            ASSERT (! IsFName);
            Special = true;
            SpecialNum = 0;
            break;

          default:
            ASSERT (! isQuote (c));
            if (IsFName)
            {
              ASSERT (FNamePos < FNameLen);
              FName [FNamePos] = c;
              FNamePos++;
            }
            else
              fprintf (F, "%c", c);
        }

      i++;
    }
    if (F)
      fclose (F);
    ASSERT (! IsFName);
    ASSERT (! Special);
  }
};



}  // namespace




int main (int argc, 
          const char* argv[])
{
  ThisApplication app;
  return app. run (argc, argv);
}


