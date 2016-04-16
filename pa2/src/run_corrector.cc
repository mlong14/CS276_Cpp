
#include "noisy_channel.h"
#include "language_model.h"
#include "corrector.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>  
#include <boost/program_options.hpp>

std::string ROOT = "/Users/matthewlong/Documents/stanford_coursework/CS276/c++_ass/";
std::string INPUT = ROOT + "pa2/output";

int main(int argc, char *argv[]) {

	std::string query_set;
	bool interactive = false;
	bool multithreaded = false;
	std::string query_fn, gold_fn;

	namespace po = boost::program_options;
	po::options_description desc("Options");
	desc.add_options()
		("interactive,i", "run in interactive mode")
		("multithreaded,m", "enable multithreading")
		("query_set,q", po::value<std::string>(&query_set), "queries to run");

	po::variables_map vm;

	try {
		po::store(po::command_line_parser(argc, argv).options(desc).run(),vm);
		po::notify(vm);
	}
	catch (po::error &e) {
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
		std::cerr << desc << std::endl; 
		return 1; 
	}

	if ( vm.count("interactive") || argc == 1 ) {
		interactive = true;
	}
	if ( vm.count("multithreaded") ) {
		multithreaded = true;
	}

	if ( query_set == "full" ) {
		std::cout << "Running with full set of queries" << std::endl;
		gold_fn = "data_pa2/gold.txt";
		query_fn = "data_pa2/queries.txt";
	}
	else if ( query_set == "test" ) {
		std::cout << "Running with test set of queries" << std::endl;
		gold_fn = "data_pa2/gold_test.txt";
		query_fn = "data_pa2/queries_test.txt";
	}
	else if ( query_set == "tiny" ) {
		std::cout << "Running with tiny set of queries" << std::endl;
		gold_fn = "data_pa2/gold_tiny.txt";
		query_fn = "data_pa2/queries_tiny.txt";
	}


	std::ifstream f_in;
	NoisyChannel *nc = NoisyChannel::getInstance();
	LanguageModel *lm = LanguageModel::getInstance();

	f_in.open(INPUT + "/noisy_chan.txt");
	f_in >> *nc;
	f_in.close();

	f_in.open(INPUT + "/lang_mod.txt");
	f_in >> *lm;
	f_in.close();

	nc->setProbType("empirical");
	Corrector corr(nc, lm, multithreaded);

	if (interactive) {
		std::string test_word;
		std::cout << "Enter a query: ";
		while (std::getline(std::cin, test_word)) {
			if (test_word == "") {
				break;
			}
			std::string correction = corr.getCorrection(test_word);
			std::cout << "Correction : " << test_word << " -> " << correction << std::endl;
		}
	}
	else {
		std::ifstream query, gold;
		std::string query_line, gold_line, correction;
		time_t start = time(0);
		double duration;
		float total = 0.0;
		float right = 0.0;
		query.open(ROOT + query_fn);
		gold.open(ROOT + gold_fn);
		
		while (std::getline(query, query_line) && std::getline(gold, gold_line)) {
			correction = corr.getCorrection(query_line);
			if (correction == gold_line) {
				right++;
			}
			else {
				std::cout << "Query " << total << ": " << query_line << ", Gold: " << gold_line << ", Corr: " << correction << std::endl;
			}
			total++;
		}
		duration = difftime(time(0), start);
		std::cout << "Accuracy: " << right/total << " " << right << "/" << total << std::endl;
		std::cout << "Time: " << duration << " seconds" << std::endl;
		std::cout.flush();
	}

	return 0;
}
