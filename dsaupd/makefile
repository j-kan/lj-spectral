include settings-make.inc

### g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/run-laplacian.d" -MT"src/run-laplacian.d" -o"src/run-laplacian.o" "../src/run-laplacian.cpp"

CPP          = g++

# CPP_WARNINGS = -fpermissive 
CPP_WARNINGS = -Wall
CPP_DEBUG    = -g3
CPP_OPTIM    = -O 
CPP_INCLUDES = -I $(BOOST_ROOT)
CPP_LIBS     = 
F77_LIBS     = 

OBJ          = obj
SRC          = src

CPP_FLAGS    = $(CPP_DEBUG) $(CPP_INCLUDES)
LIB_FLAGS    = -lm -lg2c -lboost_program_options-gcc-mt-1_33_1 -lboost_filesystem-gcc-mt-1_33_1


# Putting all libraries together.

ALL_LIBS     = $(CPP_LIBS) $(F77_LIBS) $(ARPACK_LIB) 


SHELL        = /bin/bash

vpath %.cpp src
vpath %.o   obj


sources = graph_laplacian.cpp test.cpp graph.cpp cmdline.cpp simulation.cpp kmeans.cpp


# compiling and linking all examples.

all: dsaupd laplacian testeig simulation sample297987 kmsample297987 modularity 

# compiling and linking each symmetric problem.

#dsaupd:		$(OBJ)/cmdline.o $(OBJ)/graph.o
#	$(CPP) -o $@ $(CPP_FLAGS) $^ $(ALL_LIBS) $(LIB_FLAGS) 

laplacian:	$(OBJ)/graph_laplacian.o $(OBJ)/graph.o # $(OBJ)/kmeans.o 
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(ALL_LIBS) $(LIB_FLAGS) 

testeig:	$(OBJ)/test.o
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(ALL_LIBS) $(LIB_FLAGS) 

simulation:	$(OBJ)/simulation.o $(OBJ)/graph.o 	# $(OBJ)/kmeans.o 
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(ALL_LIBS) $(LIB_FLAGS) 

uniquifier:	$(OBJ)/uniquifier.o 
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(ALL_LIBS) $(LIB_FLAGS) 

sample297987:	$(OBJ)/sample297987.o $(OBJ)/graph.o 	# $(OBJ)/kmeans.o 
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(ALL_LIBS) $(LIB_FLAGS) 

kmsample297987:	$(OBJ)/kmsample297987.o $(OBJ)/graph.o 	# $(OBJ)/kmeans.o 
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(ALL_LIBS) $(LIB_FLAGS) 

modularity:	$(OBJ)/modularity.o $(OBJ)/graph.o 	# $(OBJ)/kmeans.o 
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(ALL_LIBS) $(LIB_FLAGS) 

kmsample:	$(OBJ)/kmsample.o $(OBJ)/graph.o 	# $(OBJ)/kmeans.o 
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(ALL_LIBS) $(LIB_FLAGS) 

samplesmall:	$(OBJ)/kmeans_runner.o $(OBJ)/graph.o 	# $(OBJ)/kmeans.o 
	$(CPP) -o $@ $(CPP_FLAGS) $^ $(ALL_LIBS) $(LIB_FLAGS) 

#eig:	$(OBJ)/dsaupd.o $(OBJ)/matrix.o
#	$(CPP) $(CPP_FLAGS) $(OBJ)/dsaupd.o $(OBJ)/matrix.o $(ALL_LIBS) $(LIB_FLAGS) -o eig




# defining cleaning rule.

.PHONY:	clean
clean:
	rm -f *~ *.o $(OBJ)/*.o core

# defining pattern rules.

$(OBJ)/test.o: 	      	  $(SRC)/dsaupd.h $(SRC)/eigen.h
$(OBJ)/graph.o: 	      $(SRC)/graph.h $(SRC)/dsaupd.h
$(OBJ)/graph_laplacian.o: $(SRC)/graph.h $(SRC)/dsaupd.h $(SRC)/kmeans.h
$(OBJ)/simulation.o:      $(SRC)/graph.h $(SRC)/dsaupd.h $(SRC)/kmeans.h

#$(OBJ)/cmdline.o: 	      $(SRC)/graph.h $(SRC)/dsaupd.h
#$(OBJ)/kmeans.o: 	      $(SRC)/kmeans.h


$(OBJ)/%.o:	%.cpp
	$(CPP) $(CPP_FLAGS) -c $< -o $@


#%.d: %.cpp
#        $(SHELL) -ec '$(CPP) -MM $(CPP_FLAGS) $< \
#                      | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
#                      [ -s $@ ] || rm -f $@'

#include $(sources:.cpp=.d)

# DO NOT DELETE
