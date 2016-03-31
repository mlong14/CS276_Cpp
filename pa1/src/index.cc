
#include "pa1.h"
#include "base_index.h"
#include "basic_index.h"
#include "utils.h"
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <deque>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <string>
#include <stdio.h>
#include <dirent.h>
#include <utility>     

using namespace std;

const string ROOT = "/Users/matthewlong/Documents/stanford_coursework/CS276/c++_ass/";
string OUTPUT;
string DATA;
BaseIndex *INDEX;

void mergeBlocks(vector<string> blocknames) {
	// N-way merge
	
	cout << "Merging..." << endl;
	
	vector<ifstream*> block_streams;
	ofstream block_out, block_final, term_pointers;
	ifstream block_int;
	string block_out_name = "merged_int";
	string block_final_name = "corpus.index";
	string term_pointers_name = "posting.dict";
	PostingList tmp_pl;
	priority_queue<PostingList, vector<PostingList>, greater<PostingList> > block_pls;
	map<int, deque<int> > tid2ind;	
	
	for (int i=0; i<blocknames.size(); i++) {
		ifstream *tmp_stream = new ifstream();
		tmp_stream->open(OUTPUT+blocknames[i], ios::binary);
		block_streams.push_back(tmp_stream);
		
		tmp_pl = INDEX->readPosting(*block_streams[i]);

		if (tid2ind.count(tmp_pl.GetTermID()) == 0) {
			tid2ind[tmp_pl.GetTermID()] = deque<int>();
		}
		tid2ind[tmp_pl.GetTermID()].push_front(i);
		
		block_pls.push(tmp_pl);
	}

	block_out.open(OUTPUT+block_out_name, ios::binary);

	int peek_char;
	int curr_tid;

	PostingList *pl_out;
	set<int> curr_postings;
	vector<int> curr_list;
	vector<int> out_postings;
	priority_queue<tid_triples> tid_mappings;
 
	while (block_pls.size()>=1) {
		curr_tid = block_pls.top().GetTermID();
		curr_postings.clear();
		out_postings.clear();
		while ((block_pls.top().GetTermID() == curr_tid) && (block_pls.size()>=1)) {
			tmp_pl = block_pls.top();
			block_pls.pop();
			tmp_pl.GetList( curr_list );
			for (int i=0; i<curr_list.size(); i++) {
				curr_postings.insert(curr_list[i]);
			}
		}
		copy(curr_postings.begin(),curr_postings.end(),back_inserter(out_postings));
		sort(out_postings.begin(),out_postings.end());
		
		tid_mappings.push((tid_triples){curr_tid, out_postings.size(), block_out.tellp()});
		pl_out = new PostingList(curr_tid,out_postings);
		INDEX->writePosting( block_out, *pl_out);
		delete pl_out;
		
		while (tid2ind[curr_tid].size()>=1) {
			peek_char = block_streams[tid2ind[curr_tid].front()]->peek();
			if (peek_char != EOF) {
				tmp_pl = INDEX->readPosting(*block_streams[tid2ind[curr_tid].front()]);
				block_pls.push(tmp_pl);
				if (tid2ind.count(tmp_pl.GetTermID())==0) {
					tid2ind[tmp_pl.GetTermID()] = deque<int>();
				}
				tid2ind[tmp_pl.GetTermID()].push_front(tid2ind[curr_tid].front());
			}
			tid2ind[curr_tid].pop_front();
		}
		tid2ind.erase(curr_tid);
	}

	block_out.close();
	block_int.open(OUTPUT+block_out_name, ios::binary);
	block_final.open(OUTPUT+block_final_name, ios::binary);
	term_pointers.open(OUTPUT+term_pointers_name, ios::out);

	while (tid_mappings.size()>=1) {
		block_int.seekg(tid_mappings.top().pl_pos);
		tmp_pl = INDEX->readPosting(block_int);

		term_pointers << tid_mappings.top().tid << "\t" << block_final.tellp() << endl;

		INDEX->writePosting(block_final,tmp_pl);
		tid_mappings.pop();
	}
	
	// close all files
	for (int i=0; i<block_streams.size(); i++) {
		block_streams[i]->close();
		delete block_streams[i];
	}
	term_pointers.close();
	block_int.close();
	block_final.close();

	// remove unnessecary files
	string tmp_fn;
	for (int i=0; i<blocknames.size(); i++) {
		tmp_fn = OUTPUT + blocknames[i];
		remove(tmp_fn.c_str());
	}
	
	tmp_fn = OUTPUT + block_out_name;
	remove(tmp_fn.c_str());
}

int main(int argc, char* argv[]) {
	cout << "Indexing...\n";
	
	//string index_type(argv[0]);
	//string data_dir(argv[1]);
	
	DATA = ROOT+"toy_data/";
	OUTPUT = ROOT + "pa1/output/";
	string index_type = "Basic";

	double duration;
	time_t start = time(0);

	if (index_type == "Basic") {
		BasicIndex basic_ind;
		INDEX = &basic_ind;
	}
	else {
		cerr << "Unknown index type" << endl;
	}
	
	ifstream indexing_file;
	ofstream output_file;
	int doc_id = 0;
	int word_id = 0;
	int prev;
	PostingList *tmp_pl;
	string line;
	string word;
	string out_fn;
	vector<string> blocknames;
	istringstream line_stream;
	map<string,int> doc_dict;
	map<string,int> term_dict;
	vector<int> postings;
	set<int> docs_seen;
	vector<pair<int, int> > term_doc_pairs;
	vector<pair<string,string> > file_list;
	vector<pair<string,string> > dir_list = listDirectory(DATA);
	
	//Read files into posting lists
	for (size_t i = 0; i<dir_list.size(); i++) {
		file_list = listFiles(get<1>(dir_list.at(i)));
		term_doc_pairs.clear();

		for (size_t j = 0; j<file_list.size(); j++) {
			doc_dict[get<0>(dir_list.at(i)) + "/" + get<0>(file_list.at(j))] = doc_id++;	
			indexing_file.open(get<1>(file_list.at(j)),ios::binary);
			if (indexing_file.is_open()) {
				while (getline(indexing_file,line)) {
					line_stream.str(line);
					line_stream.clear();
					while (line_stream >> word) {
						if (term_dict.count(word)==0) {
							term_dict[word] = word_id++;
						}
						term_doc_pairs.push_back(make_pair(doc_id-1,term_dict[word]));
					}
				}
			}	
			indexing_file.close();
		}

		sort(term_doc_pairs.begin(),term_doc_pairs.end(),postingPairComparator);

		out_fn = OUTPUT + get<0>(dir_list[i]);
		output_file.open(out_fn,ios::binary);
		blocknames.push_back(get<0>(dir_list[i]));
		
		if (output_file.is_open()) {
			prev = -1;
			for (size_t k = 0; k<term_doc_pairs.size(); k++) {
				if ((prev != -1) && (get<1>(term_doc_pairs[k]) != prev)) {
					tmp_pl = new PostingList(prev,postings);
					INDEX->writePosting(output_file,*tmp_pl);
					delete tmp_pl;

					prev = get<1>(term_doc_pairs[k]);
					postings.clear();
					postings.push_back(get<0>(term_doc_pairs[k]));
				}
				else {
					prev = get<1>(term_doc_pairs[k]);
					if (docs_seen.count(get<0>(term_doc_pairs[k])) == 0) {
						postings.push_back(get<0>(term_doc_pairs[k]));
						docs_seen.insert(get<0>(term_doc_pairs[k]));
					}
				}
			}
			tmp_pl = new PostingList(prev,postings);
			INDEX->writePosting(output_file,*tmp_pl);
			delete tmp_pl;
		}
		else {
			cerr << "Error opening output file" << endl;
		}
		output_file.close();
	}

	cout << "Total Files: " << doc_id << endl;

	//Merge blocks	
	mergeBlocks(blocknames);

	ofstream termID_out, docID_out;
	termID_out.open(OUTPUT + "term.dict",ios::out);
	docID_out.open(OUTPUT + "doc.dict",ios::out);

	for (map<string,int>::iterator iter=term_dict.begin(); iter!=term_dict.end(); iter++) {
		termID_out << iter->first << "\t" << iter->second << endl;
	}

	for (map<string,int>::iterator iter=doc_dict.begin(); iter!=doc_dict.end(); iter++) {
		docID_out << iter->first << "\t" << iter->second << endl;
	}
	
	termID_out.close();
	docID_out.close();

	duration = difftime(time(0), start);
	cout << "Duration: " << duration << " seconds" << endl;

	return 0;
}
