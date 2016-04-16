
#include "edit_cost_model.h"
#include "utils.h"
#include <string>
#include <tuple>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

typedef EmpiricalCostModel::LetterPairs LetterPairs;

const char SOW = '$';
const std::string alphabet = std::string("abcdefghijklmnopqrstuvwxyz0123456789,'&#$.+_|");

bool operator <(const LetterPairs& x, const LetterPairs& y) {
		if (x.first < y.first) {
			return true;
		}
		if (y.first < x.first) {
			return false;
		}
		if (x.second < y.second) {
			return true;
		}
		if (y.second < x.second) {
			return false;
		}
		return false;
}

bool operator ==(const LetterPairs& x, const LetterPairs& y) {
		return x.first == y.first && x.second == y.second;
}

std::ostream& operator<< (std::ostream &os, const std::map<LetterPairs, double> &m) {
	for (std::map<LetterPairs, double>::const_iterator iter = m.begin(); iter!=m.end(); iter++) {
		os << iter->first << "\t" << iter->second << std::endl;
	}
	return os;
}

std::istream& operator>> (std::istream &is, std::map<LetterPairs, double> &m) {
	LetterPairs lp;
	double d;
	while (is >> lp.first >> lp.second >> d) {
		m[lp] = d;
	}
	return is;
}

std::ostream& operator<< (std::ostream &os, const LetterPairs &lp) {
	os << lp.first << " " << lp.second;
	return os;
}

std::istream& operator>> (std::istream &is, LetterPairs &lp) {
	is >> lp.first >> lp.second;
	return is;
}

std::ostream& operator<< (std::ostream &os, const std::map<char, double> &m) {
	for (std::map<char, double>::const_iterator iter = m.begin(); iter!=m.end(); iter++) {
		os << iter->first << "\t" << iter->second << std::endl;
	}
	return os;
}

std::istream& operator>> (std::istream &is, std::map<char, double> &m) {
	char c;
	double d;
	while (is >> c >> d) {
		m[c] = d;
	}
	return is;
}

UniformCostModel::UniformCostModel(double _error_prob) : error_prob(_error_prob) {}

double UniformCostModel::editProbability(std::string original, std::string R) {
	std::vector<Edits> edits;
	edits = getEditDistance(R, original);

	if (original == R) {
		return 1-error_prob;
	}
	else {
		return pow(error_prob,edits.size());
	}
}

std::ostream& operator<< (std::ostream &os, const UniformCostModel &uc) {
	os << uc.error_prob << std::endl;
	return os;
}

std::istream& operator>> (std::istream &is, UniformCostModel &uc) {
	is >> uc.error_prob;	
	return is;
}

EmpiricalCostModel::EmpiricalCostModel(std::string input, double error_prob) {
	if (input != "") {
		_error_prob = error_prob;
		buildECM(input);
	}
}

double EmpiricalCostModel::editProbability(std::string original, std::string R) {
	if (unigrams.size() == 0) {
		std::cerr << "Nothing loaded" << std::endl;
		exit(1);
	}

	if (original == R) {
		return log(1-_error_prob);
	}
	else {
		std::vector<Edits> edits;
		double prob = 0.0;
		LetterPairs *lp;

		edits = getEditDistance(R, original);

		for (int i=0; i<edits.size(); i++) {
			lp = new LetterPairs();
			lp->first = edits[i].first;
			lp->second = edits[i].second;
			switch(edits[i].opC) {
				case trans: prob += log(transposition[*lp]); break;
				case subs: prob += log(substitution[*lp]); break;
				case del: prob += log(deletion[*lp]); break;
				case ins: prob += log(insertion[*lp]); break;
			}
		}
	return prob;
	}
}

void EmpiricalCostModel::buildECM(std::string input) {
	std::ifstream queries;
	std::string queries_path;
	std::string clean_line, noisy_line, query_line;
	LetterPairs *edits;

	std::string fn = "edit1s.txt";

	std::cout << "Building ECM Dictionary" << std::endl;

	if (input.back() == '/') {
		queries_path = input + fn; 
	}
	else {
		queries_path = input + "/" + fn;
	}

	for (int i=0; i<alphabet.size(); i++) {
		unigrams[alphabet[i]] = 0.0;
		for (int j=0; j<alphabet.size(); j++) {
			edits = new LetterPairs();
			edits->first = alphabet[i];
			edits->second = alphabet[j];

			bigrams[*edits] = 0.0;
			substitution[*edits] = 0.0;
			deletion[*edits] = 0.0;
			insertion[*edits] = 0.0;
			transposition[*edits] = 0.0;

			delete edits;
		}

	}
	
	queries.open(queries_path);

	int counter = 0;

	if (queries.is_open()) {
		while (std::getline(queries, query_line)) {
			std::stringstream iss(query_line);
			std::getline(iss, noisy_line, '\t');
			std::getline(iss, clean_line, '\t');
			processLines(noisy_line, clean_line);
			counter++;
			if (counter % 100000 == 0) {
				std::cout << "Processed " << counter << " lines" << std::endl;
			}
		}
	}
	else {
		std::cerr << "Cannot open query files " << queries_path << std::endl;
		exit(1);
	}
	queries.close();

	normalize();
}

void EmpiricalCostModel::processLines(std::string noisy, std::string clean) {
	LetterPairs *lp;
	std::vector<Edits> edits;
	
	std::replace(clean.begin(), clean.end(), ' ', SOW);
	std::replace(noisy.begin(), noisy.end(), ' ', SOW);

	noisy = SOW + noisy + SOW;
	clean = SOW + clean + SOW;

	// first add to unigram/bigrams
	for (int i=0;i<clean.size();i++) {
		unigrams[clean[i]] += 1.0;
	}

	for (int i=0;i<clean.size()-1;i++) {
		lp = new LetterPairs();
		lp->first = clean[i];
		lp->second = clean[i+1];
		bigrams[*lp] += 1.0;
		delete lp;
	}

	// now parse the errors
	edits = getEditDistance(clean, noisy);

	for (int i=0;i<edits.size();i++) {
		lp = new LetterPairs();
		lp->first = edits[i].first;
		lp->second = edits[i].second;

		switch(edits[i].opC) {
			case subs: substitution[*lp]+=1.00; 
							   break;
			case del: deletion[*lp]+=1.00; 
							   break;
			case ins: insertion[*lp]+=1.00; 
							   break;
			case trans: transposition[*lp]+=1.00; 
							   break;
		}
	}
}

void EmpiricalCostModel::normalize() {

	double num, den;
	for (std::map<LetterPairs,double>::iterator iter = deletion.begin(); iter!=deletion.end(); iter++) {
		num = 1+iter->second;
		if (bigrams.count(iter->first) == 0) {
			std::cerr << iter->first << " " << iter->second << std::endl;
			exit(1);
		}
		den = bigrams.at(iter->first)+bigrams.size();
		iter->second = num/den;
	}

	for (std::map<LetterPairs,double>::iterator iter = insertion.begin(); iter!=insertion.end(); iter++) {
		num = 1+iter->second;
		den = unigrams.at(iter->first.first)+unigrams.size();
		iter->second = num/den;
	}

	for (std::map<LetterPairs,double>::iterator iter = substitution.begin(); iter!=substitution.end(); iter++) {
		num = 1+iter->second;
		den = unigrams.at(iter->first.second)+unigrams.size();
		iter->second = num/den;
	}

	for (std::map<LetterPairs,double>::iterator iter = transposition.begin(); iter!=transposition.end(); iter++) {
		num = 1+iter->second;
		den = bigrams.at(iter->first)+bigrams.size();
		iter->second = num/den;
	}

}

std::ostream& operator<< (std::ostream &os, const EmpiricalCostModel &ec) {
	std::string SEP = "----------";
	os << ec._error_prob << std::endl;
	os << ec.unigrams << SEP << std::endl;
	os << ec.bigrams << SEP << std::endl;
	os << ec.substitution << SEP << std::endl;
	os << ec.deletion << SEP << std::endl;
	os << ec.transposition << SEP << std::endl;
	os << ec.insertion << std::endl;
	return os;
}

std::istream& operator>> (std::istream &is, EmpiricalCostModel &ec) {
	std::string sep;

	is >> ec._error_prob;
	is >> ec.unigrams;
	is.clear();
	std::getline(is,sep);
	is >> ec.bigrams;
	is.clear();
	std::getline(is,sep);
	is >> ec.substitution;
	is.clear();
	std::getline(is,sep);
	is >> ec.deletion;
	is.clear();
	std::getline(is,sep);
	is >> ec.transposition;
	is.clear();
	std::getline(is,sep);
	is >> ec.insertion;
	return is;
}
