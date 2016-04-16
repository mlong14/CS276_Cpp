
#include "language_model.h"
#include "utils.h"
#include <utility>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <dirent.h>

typedef std::map<std::string, double> lm_map;

const std::string SOL = "STARTOFLINE";
const std::string EOL = "ENDOFLINE";
const std::string BIGRAM = "bigrams.txt"; 
const std::string UNIGRAM = "unigrams.txt";

LanguageModel *LanguageModel::_lm = nullptr;

LanguageModel* LanguageModel::getInstance(std::string input) {
	if (!_lm) {
		if (input != "") {
			_lm = new LanguageModel(input);	
		}
		else {
			_lm = new LanguageModel();
		}
	}
	return _lm;
}

void LanguageModel::destroy() {
	delete _lm;
	_lm = nullptr;
}

LanguageModel::LanguageModel() {};

LanguageModel::LanguageModel(std::string input) {
	std::cout << "Creating LM with " << input << std::endl;
	createDictionaries(input);
}

LanguageModel::~LanguageModel() {
	// nothing to do
}

void LanguageModel::createDictionaries(std::string input) {
	int word_count = 0;
	std::ifstream in_f;
	std::string line_str, tok_str, prev_str, bigram_str;
	std::string first_word, second_word;
	std::stringstream line_stream;

	std::vector<std::pair<std::string,std::string> > file_list = listFiles(input);

	unigram_f[SOL] = 0;

	for (size_t i = 0; i<file_list.size(); i++) {
		std::cout << "Processing " << std::get<0>(file_list[i]) << std::endl;
		in_f.open(std::get<1>(file_list[i]));		
		if (in_f.is_open()) {
			while (std::getline(in_f, line_str)) {
				prev_str = SOL;
				unigram_f[SOL] += 1;
				line_stream.str(line_str);
				line_stream.clear();

				while (line_stream >> tok_str) {
					if (unigram_f.count(tok_str) == 0) {
						unigram_f[tok_str] = 0;
					}
					unigram_f[tok_str] += 1;

					bigram_str = prev_str + " " + tok_str;
					if (bigram_f.count(bigram_str) == 0) {
						bigram_f[bigram_str] = 0;
					}
					bigram_f[bigram_str] += 1;

					prev_str = tok_str;
					word_count++;
				}
				bigram_str = prev_str + " " + EOL;
				if (bigram_f.count(bigram_str) == 0) {
					bigram_f[bigram_str] = 0;
				}
				bigram_f[bigram_str] += 1;
			}
		}
		else {
			std::cerr << "Error reading file " << std::get<1>(file_list[i]) << std::endl;
			exit(1);
		}
		in_f.close();
	}

	// normalize bigram features
	for (lm_map::iterator iter = bigram_f.begin(); iter != bigram_f.end(); iter++) {
		line_stream.str(iter->first);
		line_stream.clear();
		line_stream >> first_word >> second_word;

		assert(unigram_f.count(first_word) != 0);

		bigram_f[iter->first] = iter->second/unigram_f[first_word];
	}

	// normalize unigram features
	for (lm_map::iterator iter = unigram_f.begin(); iter != unigram_f.end(); iter++) {
		unigram_f[iter->first] = iter->second/word_count;
	}

}

void LanguageModel::getUni(lm_map &_uni) {
	_uni = unigram_f;
}

void LanguageModel::getBi(lm_map &_bi) {
	_bi = bigram_f;
}

std::ostream& operator<< (std::ostream &os, const LanguageModel &lm) {
	
	for (lm_map::const_iterator iter = lm.bigram_f.begin(); iter != lm.bigram_f.end(); iter++) {
		os << iter->first << "\t" << iter->second << std::endl;
	}
	
	os << std::endl;

	for (lm_map::const_iterator iter = lm.unigram_f.begin(); iter != lm.unigram_f.end(); iter++) {
		os << iter->first << "\t" << iter->second << std::endl;
	}

	return os;
}

std::istream& operator>> (std::istream &is, LanguageModel &lm) {
	
	std::stringstream line_stream;
	std::string line, first_word, second_word;
	double score;
	int readMode = 0;
	
	while (std::getline(is, line)) {
		line_stream.str(line);
		line_stream.clear();

		if (line == "") {
			readMode = 1;
		}
		else {
			if (readMode) {
				while (line_stream >> first_word >> score) {
					lm.unigram_f[first_word] = score;
				}
			}
			else {
				while (line_stream >> first_word >> second_word >> score) {
					lm.bigram_f[first_word+" "+second_word] = score;
				
				}
			}
		}
	}
	return is;
}
