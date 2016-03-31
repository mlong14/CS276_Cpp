CXX = g++
CXXFLAGS = -g -lgtest -lgtest_main -lpthread
INCS = -I./ -I../../include -I/opt/gtest/include
OBJS = ../../build/pa1.o ../build/PostingsList_Test.o ../../build/basic_index.o ../build/BasicIndex_Test.o

testAll: $(OBJS)
	@echo $(CXX) $(CXXFLAGS) $(INCS) -o runTests  Main_TestAll.cc $(OBJS); $(CXX) $(CXXFLAGS) $(INCS) -o runTests  Main_TestAll.cc $(OBJS)

../build/%.o: %.cc
	@echo $(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS); $(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

clean:
	rm runTests ../build/*.o
