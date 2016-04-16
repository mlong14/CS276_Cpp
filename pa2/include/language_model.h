#ifndef PA2_LANGUAGE_MODEL_H_
#define PA2_LANGUAGE_MODEL_H_

#include <string>
#include <map>

extern const std::string SOL;
extern const std::string EOL;
extern const std::string BIGRAM;
extern const std::string UNIGRAM;

class LanguageModel {
	public:
		static LanguageModel* getInstance(std::string input = "");
		static void destroy();
		void getUni(std::map<std::string, double> &_uni);
		void getBi(std::map<std::string, double> &_bi);

		friend std::ostream& operator<< (std::ostream &os, const LanguageModel &lm);
		friend std::istream& operator>> (std::istream &is, LanguageModel &lm);
	private:
		LanguageModel();
		LanguageModel(std::string input);
		~LanguageModel();
		void createDictionaries(std::string input);

		static LanguageModel *_lm;
		std::map<std::string, double> unigram_f;
		std::map<std::string, double> bigram_f;
};

#endif //PA2_LANGUAGE_MODEL_H_
