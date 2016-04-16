
#include "corrector.h"
#include "noisy_channel.h"
#include "language_model.h"
#include "edit_cost_model.h"
#include "utils.h"
#include "synch_queue.h"
#include "scorer.h"

#include <vector>
#include <string>
#include <sstream>
#include <exception>
#include <iostream>
#include <cmath>
#include <time.h>

const std::string alphabet_space = std::string("abcdefghijklmnopqrstuvwxyz0123456789, '&#$.+_|");

struct word_scores {
	double *score;
	std::string *word;
};

Corrector::Corrector(NoisyChannel *nc, LanguageModel *lm, bool multithreaded) {
	if (multithreaded) {
		std::cout << "Running multithreaded corrector" << std::endl;
	}
	else {
		std::cout << "Running single thread corrector" << std::endl;
	}
	_multithreaded = multithreaded;
	_nc = nc;
	_lm = lm;
	_lm->getUni(word_dict);
	_lm->getBi(biword_dict);
}

Corrector::~Corrector() {}

std::string Corrector::getCorrection(const std::string &s) {
	std::string top_word = "";
	double top_score = -1000.0;
	std::set<std::string> candidates;
	generateCandidates(s, candidates);

	time_t start = time(0);
	double duration;

	if (_multithreaded) {
		SynchronisedQueue<std::string> q;
		for (auto iter=candidates.begin(); iter!=candidates.end(); iter++) {
			q.Enqueue(*iter);
		}

		int numThreads = 4;
		boost::thread_group scorers;
		std::vector<Scorer*> scorer_objs;
		std::vector<word_scores> top_word_scores;

		for (int i=0; i<numThreads; i++) {
			std::string *thread_top_word = new std::string();
			double *thread_top_score = new double(-10000);

			std::cout << "ts: " << thread_top_score << ", tw: " << thread_top_word << std::endl;

			Scorer sc(i, &q, &word_dict, &biword_dict, _nc);
			scorers.create_thread(boost::bind(&Scorer::processQueue, &sc, s, thread_top_score, thread_top_word));
			word_scores ws = {.score = thread_top_score, .word = thread_top_word};
			top_word_scores.push_back(ws);
		}

		scorers.interrupt_all();
		scorers.join_all();
		
		for (int i=0; i<numThreads; i++) {
			//std::cout << i << " (" << *top_word_scores[i].word << ", " << *top_word_scores[i].score << ")  ";
			if (*top_word_scores[i].score > top_score) {
				top_score = *top_word_scores[i].score;
				top_word = *top_word_scores[i].word;
			}
			delete top_word_scores[i].score;
			delete top_word_scores[i].word;
		}
		//std::cout << std::endl;
	}
	else {
		double curr_score;

		for (auto iter=candidates.begin(); iter!=candidates.end(); iter++) {
			curr_score = score(*iter, s);
			if (curr_score > top_score || iter==candidates.begin()) {
				top_word = *iter;
				top_score = curr_score;
			}
		}

		if (top_score < -10000) {
			top_word = "";
		}
	}

	duration = difftime(time(0), start);
	std::cout << "(" << top_word << ", " << top_score << "), " << candidates.size() << " cand., time: " << duration << " secs." << std::endl;

	return top_word;
	
}

double Corrector::score(const std::string &clean, const std::string &noisy) {

	std::vector<std::string> split_words;
	double P_RQ, P_Q, lambda;

	lambda = 0.1;
	split_words = split(clean);
	P_Q = log(word_dict.at(split_words[0]));

	P_RQ = _nc->_em->editProbability(noisy, clean);

	for (int i=0;i<split_words.size()-1;i++) {
		if (biword_dict.count(split_words[i] + " " + split_words[i+1]) != 0) {
			P_Q += log(lambda*word_dict.at(split_words[i+1])+(1-lambda)*biword_dict.at(split_words[i]+" "+split_words[i+1]));
		}
		else {
			P_Q += log(lambda*word_dict.at(split_words[i+1]));
		}
	}

	return P_RQ+1.5*P_Q;
}

void Corrector::generateCandidates(const std::string &s, std::set<std::string> &candidates) {
	std::vector<std::string> split_words;
	std::map<std::string, std::set<std::string> > word_cand;
	std::string word;
	std::stringstream ss;
	std::vector<std::string> must_change, first_change, second_change;

	ss.str(s);

	while (ss >> word) {
		split_words.push_back(word);
		word_cand[word] = std::set<std::string>();
		
		if (word_dict.count(word) == 0) {
			must_change.push_back(word);
		}
		else {
			word_cand[word].insert(word+" ");
		}
	}

	if (must_change.size() == 1) {
		first_change.push_back(must_change[0]);
		for (int i=0;i<split_words.size();i++) {
			second_change.push_back(split_words[i]);
		}
	}
	else if (must_change.size() == 2) {
		first_change.push_back(must_change[0]);
		second_change.push_back(must_change[1]);
	}
	else if (must_change.size() == 0) {
		for (int i=0;i<split_words.size();i++) {
			first_change.push_back(split_words[i]);
			second_change.push_back(split_words[i]);
		}
	}

	for (int i=0;i<split_words.size()-1;i++) {
		word_cand[split_words[i]].insert(split_words[i]);
	}

	std::string s1,s2;
	for (int i=0;i<split_words.size();i++) {
		if (isNumeric(split_words[i])) {
			for (int j=0;j<split_words[i].size();j++) {
				s1 = split_words[i].substr(0,j);
				s2 = split_words[i].substr(j);
				if (word_dict.count(s1) != 0 && word_dict.count(s2) != 0) {
					word_cand[split_words[i]].insert(s1 + " " + s2 + " ");
				}
			}
		}
	}

	//insertions
	for (int i=0;i<split_words.size();i++) {
		if (!isNumeric(split_words[i])) {
			for (int j=0;j<split_words[i].size()+1;j++) {
				for (int k=0;k<alphabet_space.size();k++) {
					if (alphabet_space[k] != ' ') {
						s1 = split_words[i].substr(0,j) + alphabet_space[k] + split_words[i].substr(j);
						if (word_dict.count(s1) != 0) {
							word_cand[split_words[i]].insert(s1+" ");
						}
					}
					else {
						s1 = split_words[i].substr(0,j);
						s2 = split_words[i].substr(j);
						
						if (word_dict.count(s1) != 0 && word_dict.count(s2) != 0) {
							word_cand[split_words[i]].insert(s1+" "+s2+" ");
						}
					}
				}
			}
		}
	}

	//substitutions
	for (int i=0;i<split_words.size();i++) {
		if (!isNumeric(split_words[i])) {
			for (int j=0;j<split_words[i].size();j++) {
				for (int k=0;k<alphabet_space.size();k++) {
					if (alphabet_space[k] != ' ' || j != 0 || j != word.length()-1) {
						s1 = split_words[i].substr(0,j) + alphabet_space[k] + split_words[i].substr(j+1);
						if (word_dict.count(s1) != 0) {
							word_cand[split_words[i]].insert(s1+" ");
						}
					}
				}
			}
		}
	}

	//deletion
	for (int i=0;i<split_words.size();i++) {
		if (!isNumeric(split_words[i])) {
			for (int j=0;j<split_words[i].size();j++) {
				s1 = split_words[i].substr(0,j) + split_words[i].substr(j+1);
				if (word_dict.count(s1) != 0) {
					word_cand[split_words[i]].insert(s1+" ");	
				}
			}
		}
	}

	//transposition
	for (int i=0;i<split_words.size();i++) {
		if (!isNumeric(split_words[i])) {
			for (int j=0;j<split_words[i].size()-1;j++) {
				char c1 = split_words[i][j];
				char c2 = split_words[i][j+1];
				s1 = split_words[i].substr(0,j) + c2 + c1 + split_words[i].substr(j+2);
				if (word_dict.count(s1) != 0) {
					word_cand[split_words[i]].insert(s1+" ");
				}
			}
		}
	}

	std::set<std::string> edit3, new_cand;
	crossCandidates(word_cand, split_words, first_change, second_change, new_cand);

	if (new_cand.size() > 100) {
		candidates = new_cand;
	}
	else {
		for (auto iter = new_cand.begin(); iter!=new_cand.end();iter++) {
			edit3.clear();
			getAllEdit1(*iter, edit3);
			for (auto edit_it = edit3.begin(); edit_it != edit3.end(); edit_it++) {
				candidates.insert(*edit_it);
			}
		}
	}
}

void Corrector::crossCandidates(const std::map<std::string, std::set<std::string> > &word_cand, const std::vector<std::string> &split_words, const std::vector<std::string> &first_change, const std::vector<std::string> &second_change, std::set<std::string> &candidates) {
	std::string s1, s2, cand;
	std::map<std::string, int> split_words_lookup;
	int s1_pos, s2_pos;
	
	for (int i=0;i<split_words.size();i++) {
		split_words_lookup[split_words[i]] = i;
	}

	for (int i=0;i<first_change.size();i++) {
		s1 = first_change[i];
		for (int j=0;j<second_change.size();j++) {
			s2 = second_change[j];

			bool flag1 = true;
			s1_pos = split_words_lookup[s1];
			s2_pos = split_words_lookup[s2];

			if (first_change.size() == split_words.size()) {
				if (s1_pos>=s2_pos) {
					flag1 = false;
				}
			}

			if (s1_pos!=s2_pos && flag1) {
				auto iter1 = word_cand.at(s1).begin();
				while (iter1 != word_cand.at(s1).end()) {
					auto iter2 = word_cand.at(s2).begin();
					while(iter2 != word_cand.at(s2).end()) {
						bool concatFlag = false;
						bool allWordsInDict = true;
						cand = "";

						for (int m=0;m<split_words.size();m++) {
							if (m==s1_pos) {
								cand += *iter1;
								if (iter1->at(iter1->size()-1) != ' ') {
									concatFlag = true;
								}
							}
							else if (m==s2_pos) {
								cand += *iter2;
								if (iter2->at(iter2->size()-1) != ' ') {
									concatFlag = true;
								}
							}
							else {
								cand += split_words[m] + " ";
							}
						}

						if (cand.at(cand.size()-1) == ' ') {
							cand = cand.substr(0,cand.size()-1);
						}
						if (cand.at(0) == ' ') {
							cand = cand.substr(1);
						}
						
						if (concatFlag) {
							allWordsInDict = validString(cand);
						}

						if (allWordsInDict) {
							candidates.insert(cand);
						}
						iter2++;
					}
					iter1++;
				}
			}
		}
	}
}

void Corrector::printWordCand(const std::map<std::string, std::set<std::string> > &m) {
	for (auto iter=m.begin(); iter!=m.end(); iter++) {
		std::cout << iter->first << ": ";
		for (auto iter_set=iter->second.begin(); iter_set!=iter->second.end(); iter_set++) {
			std::cout << *iter_set << ", ";
		}
		std::cout << std::endl;
	}
}

bool Corrector::isNumeric(const std::string &s) {
	std::string::const_iterator iter = s.begin();
	while (iter!=s.end() && std::isdigit(*iter)) ++iter;
	return !s.empty() && iter == s.end();
}

// individual word splits
void Corrector::getAllEdit1(const std::string &original, std::set<std::string> &edited_words) {
	std::string replacement;

	if (validString(original)) {
		edited_words.insert(original);
	}

	// deletions
	for (int i=0;i<original.size();i++) {
		if (i == 0) {
			replacement = original.substr(1, original.size()-1);
		}
		else if (i == original.size()-1) {
			replacement = original.substr(0, original.size()-1);
		}
		else {
			replacement = original.substr(0, i) + original.substr(i+1,original.size()-i-1);
		}
	
		if (validString(replacement)) {
			edited_words.insert(replacement);
		}
	}

	// insertions
	for (int i=0;i<original.size()+1;i++) {
		for (int j=0;j<alphabet_space.size();j++) {
			replacement = original;
			replacement.insert(i,1,alphabet_space[j]);
			if (validString(replacement)) {
				edited_words.insert(replacement);
			}
		}
	}

	// substitutions 
	for (int i=0;i<original.size()+1;i++) {
		for (int j=0;j<alphabet_space.size();j++) {
			replacement = original;
			replacement[i] = alphabet_space[j];
			if (validString(replacement)) {
				edited_words.insert(replacement);
			}
		}
	}

	// transposition
	for (int i=0;i<original.size()-1;i++) {
		replacement = original;
		replacement[i] = original[i+1];
		replacement[i+1] = original[i];
		if (validString(replacement)) {
			edited_words.insert(replacement);
		}
	}
}

bool Corrector::validString(const std::string &s) {
	std::string word;
	std::stringstream ss;
	ss.str(s);

	while (ss >> word) {
		if (word_dict.count(word) == 0) {
			return false;
		}
	}
	return true;
}

