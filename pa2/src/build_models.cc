
#include "language_model.h"
#include "noisy_channel.h"
#include "edit_cost_model.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

string ROOT = "/Users/matthewlong/Documents/stanford_coursework/CS276/c++_ass/";
string OUTPUT = ROOT + "pa2/output";
string CORPUS;
string QUERIES;

int main(int argc, char *argv[]) {

	if (argc > 1) {
		if (strcmp(argv[1], "full") == 0) {
			std::cout << "Building models w/ full corpus" << std::endl;
			CORPUS = ROOT + "data_pa2/corpus";
		}
		else if (strcmp(argv[1], "tiny") == 0) {
			std::cout << "Building models w/ tiny corpus" << std::endl;
			CORPUS = ROOT + "data_pa2/tiny_corpus";
		}
		else if (strcmp(argv[1], "toy") == 0) {
			std::cout << "Building models w/ toy corpus" << std::endl;
			CORPUS = ROOT + "data_pa2/toy_corpus";
		}
		else {
			std::cerr << "Unknown input file" << std::endl;
			exit(1);
		}
	}
	else {
		CORPUS = ROOT + "data_pa2/toy_corpus";
	}
	
	QUERIES = ROOT + "data_pa2";

	LanguageModel *lm = LanguageModel::getInstance(CORPUS);

	ofstream f_out(OUTPUT+"/lang_mod.txt");
	f_out << *lm;
	f_out.close();

	LanguageModel::destroy();

	NoisyChannel *nc = NoisyChannel::getInstance(QUERIES);

	f_out.open(OUTPUT+"/noisy_chan.txt");
	f_out << *nc;
	f_out.close();

	NoisyChannel::destroy();

	return 0;
}
