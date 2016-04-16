
#include "gtest/gtest.h"
#include "language_model.h"
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <sstream>

class LanguageModelTest : public ::testing::Test {
	protected:
		virtual void SetUp() {
			system("mkdir ./output");
		}
		virtual void TearDown() {
			system("rm -r ./output");
		}
};

TEST_F(LanguageModelTest,ReadWrite) {
	std::string start, end;
	std::ifstream f_in;
	std::ofstream f_out;
	std::stringstream ss_s, ss_e;

	LanguageModel *lm1 = LanguageModel::getInstance("../../data_pa2/tiny_corpus");

	ss_s << *lm1;
	start = ss_s.str();

	f_out.open("./output/test.txt");
	f_out << *lm1;
	f_out.close();

	LanguageModel::destroy();
	
	LanguageModel *lm2 = LanguageModel::getInstance();
	f_in.open("./output/test.txt");
	f_in >> *lm2;
	f_in.close();

	ss_e << *lm2;
	end = ss_e.str();

	EXPECT_EQ((end == start), 1);

	LanguageModel::destroy();
	
}

TEST_F(LanguageModelTest, GetFunctions) {
	LanguageModel *lm1 = LanguageModel::getInstance("../../data_pa2/tiny_corpus");
	std::map<std::string, double> actual_uni;
	lm1->getUni(&actual_uni);

	std::map<std::string, double> test_uni;
	test_uni[SOL] = 0.266667;
	test_uni["hello"] = 0.0666667;
	test_uni["hi"] = 0.0666667;
	test_uni["is"] = 0.2;
	test_uni["matt"] = 0.266667;
	test_uni["my"] = 0.133333;
	test_uni["name"] = 0.133333;
	test_uni["there"] = 0.0666667;
	test_uni["what"] = 0.0666667;

	for (std::map<std::string,double>::iterator iter = test_uni.begin(); iter != test_uni.end(); iter++) {
		EXPECT_EQ(fabs(test_uni[iter->first]-actual_uni[iter->first])<.001, 1);	
	}
}
