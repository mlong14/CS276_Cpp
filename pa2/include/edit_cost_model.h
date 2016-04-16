#ifndef PA2_EDIT_COST_MODEL_H_
#define PA2_EDIT_COST_MODEL_H_

#include <string>
#include <map>

extern const char SOW;
extern const std::string alphabet;

class EditCostModel {
	public:
		virtual ~EditCostModel() {};
		virtual double editProbability(std::string original, std::string R) = 0; 
};

class UniformCostModel : public EditCostModel {
	public:
		UniformCostModel(double _error_prob = 0.04);
		~UniformCostModel() {};
		double editProbability(std::string original, std::string R);
		
		friend std::ostream& operator<< (std::ostream &os, const UniformCostModel &uc);
		friend std::istream& operator>> (std::istream &is, UniformCostModel &uc);
	private:
		double error_prob;
};

class EmpiricalCostModel : public EditCostModel {
	public:
		EmpiricalCostModel(std::string input = "", double _error_prob = 0.04);
		~EmpiricalCostModel() {};
		double editProbability(std::string original, std::string R);
		
		friend std::ostream& operator<< (std::ostream &os, const EmpiricalCostModel &ec);
		friend std::istream& operator>> (std::istream &is, EmpiricalCostModel &ec);

		struct LetterPairs {
			char first;
			char second;
		};
	private:
		void buildECM(std::string input);
		void processLines(std::string noisy, std::string clean);
		void normalize();

		std::map<LetterPairs, double> substitution;
		std::map<LetterPairs, double> deletion;
		std::map<LetterPairs, double> insertion;
		std::map<LetterPairs, double> transposition;
		
		double _error_prob;

		std::map<char, double> unigrams;
		std::map<LetterPairs, double> bigrams;

};

bool operator< (const EmpiricalCostModel::LetterPairs &lp1, const EmpiricalCostModel::LetterPairs &lp2);
bool operator==(const EmpiricalCostModel::LetterPairs &lp1, const EmpiricalCostModel::LetterPairs &lp2);

std::ostream& operator<< (std::ostream &os, const std::map<EmpiricalCostModel::LetterPairs, double> &m);
std::istream& operator>> (std::istream &is, std::map<EmpiricalCostModel::LetterPairs, double> &m);
std::ostream& operator<< (std::ostream &os, const EmpiricalCostModel::LetterPairs &lp);
std::istream& operator>> (std::istream &is, EmpiricalCostModel::LetterPairs &lp);
std::ostream& operator<< (std::ostream &os, const std::map<char, double> &m);
std::istream& operator>> (std::istream &is, std::map<char, double> &m);

#endif //PA2_EDIT_COST_MODEL_H_
