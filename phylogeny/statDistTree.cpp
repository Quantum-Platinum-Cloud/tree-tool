// statDistTree.cpp

#undef NDEBUG
#include "../common.inc"

#include "../common.hpp"
using namespace Common_sp;
#include "../dm/dataset.hpp"
using namespace DM_sp;
#include "distTree.hpp"
using namespace DistTree_sp;



namespace 
{


const string distName ("dist"); 



struct ThisApplication : Application
{
	ThisApplication ()
	: Application ("Compute different statistics of a distance tree")
	{
	  // Input
	  addPositional ("input_tree", "Distance tree file");
	  addKey ("dist_request", "File with requests to compute tree distances, tab-delimited line format: <obj1> <obj2>, to be printed in the file <output_dist>");
    // Output
	  addKey ("dist2", "A " + dmSuff + "-file with a two-way distance attribute " + strQuote (distName));
	  addKey ("noise", strQuote (distName) + " += Normal(0,noise)", "0");
	  addFlag ("sqrt", "Apply square root to the attribute " + strQuote (distName));
	//addKey ("pair_residuals", "Output " + dmSuff + "-file with quality statistics for each object pair"); ??
	  addKey ("arc_length", "Output file with arc length statistics: " + Tree::printArcLengthsColumns ());
	  addKey ("dist_pairs", "Output file with all or <dist_request> tree distances, tab-delimited line format: <obj1> <obj2> <dist>");
	  addKey ("height", "Output file with interior node average heights");
	}



	void body () const final
  {
		const string input_tree      = getArg ("input_tree");
		const string dist_request    = getArg ("dist_request");
		const string distDmFName     = getArg ("dist2");
		const Real noise             = str2<Real> (getArg ("noise"));
		const bool sqrtP             = getFlag ("sqrt");
	//const string pair_residuals  = getArg ("pair_residuals");
		const string arc_length      = getArg ("arc_length");
		const string dist_pairs      = getArg ("dist_pairs");
		const string heightFName     = getArg ("height");
		ASSERT (noise >= 0.0); 
		IMPLY (noise, ! distDmFName. empty ());
		IMPLY (sqrtP, ! distDmFName. empty ());
    if (! dist_request. empty () && dist_pairs. empty ())
    	throw runtime_error ("-dist_request exists, but no -dist_pairs");
    

    DistTree tree (input_tree, string (), string (), string()); 
    tree. qc ();     
      
    
    // Statistics 
    {
      tree. setFrequentChild (rareProb);  
      tree. setFrequentDegree (rareProb); 

      const ONumber on (cout, relCriterionDecimals, false);
      cout << "# Interior nodes (with root) = " << tree. countInteriorNodes () << " (max = " << tree. getDiscernibles (). size () - 1 << ')' << endl;
      cout << "# Interior undirected arcs = " << tree. countInteriorUndirectedArcs () << endl;
      cout << "Tree length = " << tree. getLength () << endl;
      {
      	const ONumber on1 (cout, dissimDecimals, true); 
        cout << "Min. discernible leaf length = " << tree. getMinLeafLen () << endl;
          // = 0 => epsilon2_0 > 0
      }
      cout << "Ave. arc length = " << tree. getAveArcLength () << endl;
        // Check exponential distribution ??
      cout << "Interior height = " << tree. getInteriorHeight () << endl;
      const Real bifurcatingInteriorBranching = tree. getBifurcatingInteriorBranching ();
      cout << "Bifurcating interior branching = " << bifurcatingInteriorBranching << endl;
      // #dissimilarities = 2 #discernibles log_2(#discernibles) #sparsing_leaves

      {      
        size_t freqChildrenInteriors = 0;
        size_t freqChildrenLeaves    = 0;
        size_t stableInteriors       = 0;
        size_t stableLeaves          = 0;
        for (const DiGraph::Node* node : tree. nodes)
        {
          const Tree::TreeNode* tn = static_cast <const Tree::TreeNode*> (node);
          if (tn->frequentChild)
          {
            if (tn->isInteriorType ())
              freqChildrenInteriors++;
            else
              if (tn->isLeafType ())
                freqChildrenLeaves++;
          }
          if (   tn->isInteriorType ()
              && tn->frequentDegree >= 3
             )
            stableInteriors++;
          if (   tn->isLeafType ()
              && tn->frequentDegree == 1
             )
            stableLeaves++;
        }
        
        // Depend on root ??
        cout << "# Frequent children interior nodes = " << freqChildrenInteriors << endl;
        cout << "# Frequent children leaves = "         << freqChildrenLeaves << endl;
        
        cout << "# Frequent interior nodes = "          << stableInteriors << endl;
        cout << "# Frequent leaves = "                  << stableLeaves << endl;
        cout << "Rareness threshold = " << rareProb * 100 << " %" << endl;
      }      
    }


    if (! distDmFName. empty ())
    {
      Dataset ds;
      
      VectorPtr<Leaf> leaves;
      for (const auto& it : tree. name2leaf)
      {
        ds. appendObj (it. first);
        ASSERT (it. second->graph);
        leaves << it. second;
      }
      ASSERT (ds. objs. size () == leaves. size ());
      
      auto distAttr = new PositiveAttr2 (distName, ds);
      Normal norm;
      norm. setParam (0.0, noise); 
      Tree::LcaBuffer buf;
      FOR (size_t, row, ds. objs. size ())
      {
        distAttr->put (row, row, 0);
        FOR (size_t, col, row)
        {
          const Tree::TreeNode* lca = nullptr;
          const VectorPtr<Tree::TreeNode>& path = Tree::getPath (leaves [row], leaves [col], nullptr, lca, buf);
          Real dist = tree. path2prediction (path);
          if (noise)
            dist += norm. rand ();
          if (sqrtP)
            dist = sqrt (dist);  
          distAttr->putSymm (row, col, dist);
        }
      }

      ds. saveFile (distDmFName);    
    }


  #if 0
    ??
    if (! pair_residuals. empty ())
    {
      checkOptimizable (*tree, "pair_residuals");
      OFStream f ("", pair_residuals, dmExt);
      const RealAttr1* resid2Attr  = tree. getResiduals2 ();
      const RealAttr1* logDiffAttr = tree. getLogPredictionDiff ();
      tree. pairResiduals2dm (resid2Attr, logDiffAttr, f); 
    }
  #endif
    

    if (! arc_length. empty ())
    {
      // dm-file ??
      // cout << arcLenRel.SD after outlier deleting ??
      OFStream f (arc_length);
      const ONumber on (f, dissimDecimals, true);
      tree. printArcLengths (f);
    }


    Vector<Pair<const Leaf*>> distRequestPairs;  
    if (! dist_request. empty ())
    {
    	PairFile f (dist_request);
    	while (f. next ())
    	{
        const Leaf* leaf1 = findPtr (tree. name2leaf, f. name1);
        if (! leaf1)
        	throw runtime_error ("Object " + f. name1 + " is not found");
        const Leaf* leaf2 = findPtr (tree. name2leaf, f. name2);
        if (! leaf2)
        	throw runtime_error ("Object " + f. name2 + " is not found");
			  distRequestPairs << Pair<const Leaf*> (leaf1, leaf2);
    	}
    }
    
    
    if (! dist_pairs. empty ())
    {
    	// distRequestPairs
    	if (dist_request. empty ())
    	{
	    	VectorPtr<Leaf> leaves;  leaves. reserve (tree. nodes. size ());
	 	    for (const DiGraph::Node* node : tree. nodes)
	        if (const Leaf* leaf = static_cast <const DTNode*> (node) -> asLeaf ())
	          if (leaf->graph)
	        	  leaves << leaf;
	      FFOR (size_t, i, leaves. size ())
	        FOR (size_t, j, i)
	        {
	        	const Leaf* leaf1 = leaves [i];
	        	const Leaf* leaf2 = leaves [j];
					  if (leaf1->name > leaf2->name)
					    swap (leaf1, leaf2);
					  distRequestPairs << Pair<const Leaf*> (leaf1, leaf2);
					}
    	}

    	OFStream f (dist_pairs);
    	Tree::LcaBuffer buf;
    	for (const auto p : distRequestPairs)
    	{
    		const Leaf* leaf1 = p. first;
    		const Leaf* leaf2 = p. second;
			  const Tree::TreeNode* lca_ = nullptr;
	    	const VectorPtr<Tree::TreeNode>& path = Tree::getPath (leaf1, leaf2, nullptr, lca_, buf);
		  	f         << leaf1->name 
		  	  << '\t' << leaf2->name
		  	  << '\t' << DistTree::path2prediction (path)
		  	  << endl;
		  }
    }
    
    
    if (! heightFName. empty ())
    {
      tree. setHeight ();
      OFStream f (heightFName);
      const ONumber on (f, dissimDecimals, true);
 	    for (const DiGraph::Node* node : tree. nodes)
        if (const Steiner* st = static_cast <const DTNode*> (node) -> asSteiner ())
      	  f << st->getLcaName () << '\t' << st->getHeight_ave () << endl;
    }
    
    
  #if 0
    {
      Real arcLen_min = NaN;
      Real outlier_EValue_max = 10;  // ??
      while (outlier_EValue_max >= 1e-6)
      {
        const VectorPtr<DTNode> tooLongArcs (tree->findOutlierArcs (outlier_EValue_max, arcLen_min));
        cout << endl;
        {
          ONumber on (cout, 1, true);  // PAR
          cout << "outlier_EValue_max: " << outlier_EValue_max << endl;
        }
        cout << "# Too long arcs: " << tooLongArcs. size () << endl;
        cout << "Min. length of too long arcs: " << arcLen_min << endl;
        outlier_EValue_max /= 10;
      }
    }
  #endif
  //tree->findTopologicalClusters ();
  #if 0
    {
      Real outlier_EValue_max = 0.001;  
      while (outlier_EValue_max >= 1e-10)
      {
        ONumber on (cout, 1, true);  // PAR
        cout << "outlier_EValue_max: " << outlier_EValue_max << endl;
        tree->findDepthOutliers (outlier_EValue_max);  
        outlier_EValue_max /= 10;
      }
    }
  #endif
	}
};



}  // namespace



int main (int argc, 
          const char* argv[])
{
  ThisApplication app;
  return app. run (argc, argv);
}

