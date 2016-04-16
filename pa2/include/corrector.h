#ifndef PA2_CORRECTOR_H_
#define PA2_CORRECTOR_H_

#include "noisy_channel.h"
#include "language_model.h"
#include "edit_cost_model.h"
#include <string>
#include <vector>
#include <map>
#include <set>

class Corrector {
	public:
		Corrector(NoisyChannel *nc, LanguageModel *lm, bool multithreaded = true);
		~Corrector();
		std::string getCorrection(const std::string &s);
	public:
		NoisyChannel *_nc;
		LanguageModel *_lm;
		bool _multithreaded;
		std::map<std::string, double> word_dict;
		std::map<std::string, double> biword_dict;
		
		void generateCandidates(const std::string &s, std::set<std::string> &candidates);
		double score(const std::string &clean, const std::string &noisy);
		void getAllEdit1(const std::string &original, std::set<std::string> &edited_words);
		void crossCandidates(const std::map<std::string, std::set<std::string> > &m, const std::vector<std::string> &split_words, const std::vector<std::string> &first_change, const std::vector<std::string> &second_change, std::set<std::string> &candidates);
		bool validString(const std::string &s);
		bool isNumeric(const std::string &s);
		void printWordCand(const std::map<std::string, std::set<std::string> > &m);
};

#endif //PA2_CORRECTOR_
