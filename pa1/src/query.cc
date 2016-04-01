
#include "pa1.h"
#include "base_index.h"
#include "basic_index.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

const string ROOT = "/Users/matthewlong/Documents/stanford_coursework/CS276/c++_ass/";
const string TERM_DICT = "term.dict";
const string DOC_DICT = "doc.dict";
const string POSTINGS_DICT = "posting.dict";
const string INDEX_FILE = "corpus.index";

string INPUT_DIR;
BaseIndex *INDEX;

void loadIndexFiles(map<string, int> &term_dict, map<string, int> &doc_dict, map<int, int> &postings_dict);
void intersectPL(vector<int> &pl1, vector<int> &pl2);

void loadIndexFiles(map<string, int> &term_dict, map<int, string> &doc_dict, map<int, int> &postings_dict) {
	ifstream td_f, dd_f, pd_f;
	int tid, did;
	long f_p;
	string word, doc;
	
	td_f.open(INPUT_DIR + TERM_DICT, ios::in);
	while (td_f >> word >> tid) {
		term_dict[word] = tid;
	}
	td_f.close();

	dd_f.open(INPUT_DIR + DOC_DICT, ios::in);
	while (dd_f >> doc >> did) {
		doc_dict[did] = doc;
	}
	dd_f.close();

	pd_f.open(INPUT_DIR + POSTINGS_DICT, ios::in);
	while (pd_f >> tid >> f_p) {
		postings_dict[tid] = f_p;
	}
	pd_f.close();
}

void intersectPL(vector<int> &p1, vector<int> &p2) {
	vector<int> p_out;
	vector<int>::iterator iter1,iter2;
	iter1 = p1.begin();
	iter2 = p2.begin();

	while (true) {
		if ((iter1 == p1.end()) || (iter2 == p2.end())) {
			break;
		}
		
		if (*iter1 == *iter2) {
			p_out.push_back(*iter1);
			iter1++;
			iter2++;
		}
		else if (*iter1 < *iter2) {
			iter1++;
		}
		else {
			iter2++;
		}
	}
	
	p1 = p_out;
}


int main(int arc, char* argv[]) {
	cout << "Querying..." << endl;

	INPUT_DIR = ROOT + "pa1/output/"; 
	string index_type = "Basic";

	if (index_type == "Basic") {
		BasicIndex basic_ind;
		INDEX = &basic_ind;
	}
	else {
		cerr << "Unknown index type" << endl;
	}

	// load all info
	map<string, int> term_dict;
	map<int, string> doc_dict;
	map<int, int> postings_dict;
	loadIndexFiles(term_dict, doc_dict, postings_dict);

	// open index file
	ifstream index_f(INPUT_DIR + INDEX_FILE, ios::binary);

	string user_input;
	string word;
	stringstream ui_stream;
	vector<string> tokens;
	int tid;
	vector<int> curr_docs, out_docs;
	long f_p;
	PostingList pl;
	

	while (true) {
		tokens.clear();

		cout << endl << "Enter Query: ";
		getline(cin, user_input);

		if (user_input == "") {
			break;
		}

		ui_stream.str(user_input);
		ui_stream.clear();

		while (ui_stream >> word) {
			tokens.push_back(word);
		}

		try {
			for (int i=0; i<tokens.size(); i++) {
				tid = term_dict.at(tokens[i]);
				f_p = postings_dict.at(tid);
				index_f.seekg(f_p);
				pl = INDEX->readPosting(index_f);
				pl.GetList( curr_docs );
			
				if (i == 0) {
					out_docs = curr_docs;
				}
				else {
					intersectPL(out_docs, curr_docs);
				}
			}

			for (int i=0; i<out_docs.size(); i++) {
				cout << doc_dict[out_docs[i]] << endl;
			}
		
		}
		catch (const out_of_range& e) {
			cout << "No results found" << endl;
		}
		
	}

	

	return 0;
}
