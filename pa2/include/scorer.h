#ifndef PA2_SCORER_H_
#define PA2_SCORER_H_

#include "synch_queue.h"
#include "noisy_channel.h"
#include <boost/thread.hpp>
#include <string>
#include <map>

class Scorer {
public:
	Scorer(int id, SynchronisedQueue<std::string> *_queue, std::map<std::string, double> *_word_dict, std::map<std::string, double> *_biword_dict, NoisyChannel *_nc);
	void processQueue(std::string query, double *top_score, std::string *top_word);
private:
	std::string highest_cand, query;
	double highest_score;
	int thread_id;
	std::map<std::string, double> *word_dict;
	std::map<std::string, double> *biword_dict;
	SynchronisedQueue<std::string> *m_queue;
	NoisyChannel *nc;

	double score(const std::string &query, const std::string &cand);
};

#endif
