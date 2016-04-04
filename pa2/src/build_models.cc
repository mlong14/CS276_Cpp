
#include "language_model.h"

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

string ROOT = "/Users/matthewlong/Documents/stanford_coursework/CS276/c++_ass/";
string OUTPUT = ROOT + "pa2/output";
string DATA;

int main(int argc, char *argv[]) {
	
	DATA = ROOT + "data_pa2/tiny_corpus";

	LanguageModel *lm = LanguageModel::getInstance(DATA);

	ofstream f_out(OUTPUT+"/test.txt");
	f_out << *lm;
	f_out.close();

	LanguageModel::destroy();

	lm = LanguageModel::getInstance();

	ifstream f_in(OUTPUT+"/test.txt");
	f_in >> *lm;
	f_in.close();
	
	f_in.open(OUTPUT+"/test.txt");
	f_in >> *lm;
	f_in.close();

	LanguageModel::destroy();

	return 0;
}
