#include "synch_queue.h"
#include "scorer.h"
#include "utils.h"
#include "noisy_channel.h"
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <iostream>

Scorer::Scorer(int id, SynchronisedQueue<std::string> *_queue, std::map<std::string, double> *_word_dict, std::map<std::string, double> *_biword_dict, NoisyChannel *_nc) {
	thread_id = id;
	word_dict = _word_dict;
	biword_dict = _biword_dict;
	m_queue = _queue;
	highest_score = -1000;
	nc = _nc;
}

void Scorer::processQueue (std::string query, double *top_score, std::string *top_word) {
	std::string cand;
	double cand_score;

	while (true) {
		cand = m_queue->Dequeue();	
		cand_score = score(query, cand);
		if (cand_score>*top_score) {
			*top_score = cand_score;
			*top_word = cand;
		}
	}
	boost::this_thread::interruption_point();
}

double Scorer::score(const std::string &query, const std::string &cand) {

	std::vector<std::string> split_words;
	double P_RQ, P_Q, lambda;

	lambda = 0.1;
	split_words = split(cand);
	P_Q = log(word_dict->at(split_words[0]));

	P_RQ = nc->_em->editProbability(query, cand);

	for (int i=0;i<split_words.size()-1;i++) {
		if (biword_dict->count(split_words[i] + " " + split_words[i+1]) != 0) {
			P_Q += log(lambda*word_dict->at(split_words[i+1])+(1-lambda)*biword_dict->at(split_words[i]+" "+split_words[i+1]));
		}
		else {
			P_Q += log(lambda*word_dict->at(split_words[i+1]));
		}
	}

	return P_RQ+1.5*P_Q;
}

