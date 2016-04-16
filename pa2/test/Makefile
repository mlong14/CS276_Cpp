CXX = g++
CXXFLAGS = -g -lgtest -lgtest_main -lpthread
INCS = -I./include -I../include -I/opt/gtest/include
OBJS = ../build/language_model.o ../build/utils.o ./build/LanguageModel_Test.o ./build/Utils_Test.o

testAll: $(OBJS)
	@echo $(CXX) $(CXXFLAGS) $(INCS) -o runTests src/Main_TestAll.cc $(OBJS); $(CXX) $(CXXFLAGS) $(INCS) -o runTests src/Main_TestAll.cc $(OBJS)

./build/%.o: ./src/%.cc
	@echo $(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS); $(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

clean:
	rm runTests ./build/*.o
