include MakeRules
CPPFLAGS=$(OptCPPFLAGS) 
#CPPFLAGS=$(DebugCPPFLAGS)




############################### Programs ################################

all:	\
  cpp_test \
  extractPairs \
  graph_test \
  list2pairs \
  selColumn \
  setMinus \
  setRandOrd \
  splitList \
  str2hash \
  trav \
  unCgi
	

cpp_test.o:  $(COMMON_HPP)  
cpp_testOBJS=cpp_test.o $(CPP_DIR)/common.o
cpp_test:	$(cpp_testOBJS)
	$(CXX) -o $@ $(cpp_testOBJS) $(LIBS)
	$(ECHO)

extractPairs.o:  $(COMMON_HPP)  
extractPairsOBJS=extractPairs.o $(CPP_DIR)/common.o
extractPairs:	$(extractPairsOBJS)
	$(CXX) -o $@ $(extractPairsOBJS) $(LIBS)
	$(ECHO)

graph_test.o:  $(COMMON_HPP) $(CPP_DIR)/graph.hpp
graph_testOBJS=graph_test.o $(CPP_DIR)/common.o $(CPP_DIR)/graph.o
graph_test:	$(graph_testOBJS)
	$(CXX) -o $@ $(graph_testOBJS) $(LIBS)
	$(ECHO)

list2pairs.o:  $(COMMON_HPP)  
list2pairsOBJS=list2pairs.o $(CPP_DIR)/common.o
list2pairs:	$(list2pairsOBJS)
	$(CXX) -o $@ $(list2pairsOBJS) $(LIBS)
	$(ECHO)

selColumn.o:  $(COMMON_HPP)  
selColumnOBJS=selColumn.o $(CPP_DIR)/common.o
selColumn:	$(selColumnOBJS)
	$(CXX) -o $@ $(selColumnOBJS) $(LIBS)
	$(ECHO)

setMinus.o:  $(COMMON_HPP)  
setMinusOBJS=setMinus.o $(CPP_DIR)/common.o
setMinus:	$(setMinusOBJS)
	$(CXX) -o $@ $(setMinusOBJS) $(LIBS)
	$(ECHO)

setRandOrd.o:  $(COMMON_HPP)  
setRandOrdOBJS=setRandOrd.o $(CPP_DIR)/common.o
setRandOrd:	$(setRandOrdOBJS)
	$(CXX) -o $@ $(setRandOrdOBJS) $(LIBS)
	$(ECHO)

splitList.o:  $(COMMON_HPP)  
splitListOBJS=splitList.o $(CPP_DIR)/common.o
splitList:	$(splitListOBJS)
	$(CXX) -o $@ $(splitListOBJS) $(LIBS)
	$(ECHO)

str2hash.o:  $(COMMON_HPP)  
str2hashOBJS=str2hash.o $(CPP_DIR)/common.o
str2hash:	$(str2hashOBJS)
	$(CXX) -o $@ $(str2hashOBJS) $(LIBS)
	$(ECHO)

trav.o:  $(COMMON_HPP)  
travOBJS=trav.o $(CPP_DIR)/common.o
trav:	$(travOBJS)
	$(CXX) -o $@ $(travOBJS) $(LIBS)
	$(ECHO)

unCgi.o:  $(COMMON_HPP)  
unCgiOBJS=unCgi.o $(CPP_DIR)/common.o
unCgi:	$(unCgiOBJS)
	$(CXX) -o $@ $(unCgiOBJS) $(LIBS)
	$(ECHO)
