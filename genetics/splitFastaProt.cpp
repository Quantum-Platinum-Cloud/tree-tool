// splitFastaProt.cpp

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
*   Split a peptide multi-fasta file into a set of fasta files each containing one sequence
*
*/


#undef NDEBUG
#include "../common.inc"

#include "../common.hpp"
using namespace Common_sp;
#include "seq.hpp"
using namespace Seq_sp;



namespace 
{


struct ThisApplication : Application
{
  ThisApplication ()
    : Application ("Split a peptide multi-fasta file into a set of fasta files each containing one sequence")
  	{
  	  addFlag ("sparse", "Sparse sequence");
  	  addPositional ("in", "Input multi-fasta file with peptides");
  	  addPositional ("len_min", "Minimum peptide length");
  	  addPositional ("out_dir", "Output directory");
  	}



	void body () const final
  {
		const bool sparse    = getFlag ("sparse");
		const string in      = getArg ("in");
		const size_t len_min = (size_t) arg2uint ("len_min");
		const string out_dir = getArg ("out_dir");

    {    
      // For ~Progress()
		  Multifasta fa (in, true);
		  while (fa. next ())
		  {
	      const Peptide seq (fa, Peptide::stdAveLen, sparse);
		    ASSERT (! seq. name. empty ());
		    if (seq. seq. size () < (size_t) len_min)
		    	continue;
		    string s (seq. name);
		    s = findSplit (s);
		    seq. saveFile (out_dir + "/" + findSplit (s, '|'));
		  }
		}
	}
};



}  // namespace



int main(int argc, 
         const char* argv[])
{
  ThisApplication app;
  return app. run (argc, argv);  
}


