
#include "utils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <dirent.h>
#include <utility>
#include <algorithm>

const char NO_CHAR = '-';

typedef std::vector<std::vector<int> > grid;
std::ostream& operator<< (std::ostream &os, const grid &g);
void buildMinPath(const grid &g, grid *path, grid *memoize, int i, int j);

std::ostream& operator<< (std::ostream &os, const grid &g) {
	for (int i=0; i<g.size(); i++) {
		for (int j=0; j<g[i].size(); j++) {
			os << g[i][j] << ", ";
		}
		os << std::endl;
	}
	return os;
}

std::vector<std::pair<std::string,std::string> > listDirectory(std::string input_dir) {
	DIR *dirp;
	struct dirent *dp;
	std::string tmp;
	dirp = opendir(input_dir.c_str());
	std::vector<std::pair<std::string,std::string> > file_list;

	if (dirp == NULL) {
		std::cerr << "Cannot Open Directory " << input_dir << std::endl;
		throw 1;
	}

	while ((dp = readdir(dirp))!=NULL) {
		tmp = dp->d_name;
		if ((tmp.at(0) != '.') && (dp->d_type == DT_DIR)) {
			file_list.push_back(std::make_pair(tmp,input_dir + "/" + tmp));
		}
	}
	(void)closedir(dirp);
	return file_list;
}

std::vector<std::pair<std::string,std::string> > listFiles(std::string input_dir) {
	DIR *dirp;
	struct dirent *dp;
	std::string tmp;
	dirp = opendir(input_dir.c_str());
	std::vector<std::pair<std::string,std::string> > file_list;

	if (dirp == NULL) {
		std::cerr << "Cannot Open Directory " << input_dir << "\n";
		throw 1;
	}

	while ((dp = readdir(dirp))!=NULL) {
		tmp = dp->d_name;
		if ((tmp.at(0) != '.') && (dp->d_type == DT_REG)) {
			file_list.push_back(std::make_pair(tmp,input_dir + "/" + tmp));
		}
	}
	(void)closedir(dirp);
	return file_list;
}

int getEditDistanceNum(std::string clean, std::string noisy) {
	std::vector<Edits> edits;
	edits = getEditDistance(clean, noisy);
	return edits.size();
}

std::vector<Edits> getEditDistance(std::string clean, std::string noisy) {
	std::vector<Edits> final_edits;

	grid edit_mat;

	// Build Matrix
	for (int i=0;i<clean.size()+1;i++) {
		edit_mat.push_back(std::vector<int>());
		for (int j=0;j<noisy.size()+1;j++) {
			edit_mat[i].push_back(0);
		}
	}
	
	for (int i=0;i<clean.size()+1;i++) {
		edit_mat[i][0] = i;
	}
	for (int j=0;j<noisy.size()+1;j++) {
		edit_mat[0][j] = j;
	}

	for (int i=1;i<edit_mat.size();i++) {
		for (int j=1;j<edit_mat[i].size();j++) {
			if (noisy[j-1] == clean[i-1]) {
				edit_mat[i][j] = std::min(edit_mat[i-1][j-1], 1+std::min(edit_mat[i-1][j],edit_mat[i][j-1]));
			}
			else {
				if ((i>=2 && j>=2) && (noisy[j-1] == clean[i-2]) && (noisy[j-2] == clean[i-1])) {
					edit_mat[i][j] = std::min(edit_mat[i-1][j-1], std::min(edit_mat[i-1][j],edit_mat[i][j-1]));
				}
				else {
					edit_mat[i][j] = 1+std::min(edit_mat[i-1][j-1],std::min(edit_mat[i-1][j],edit_mat[i][j-1]));
				}
			}
		}
	}

	// Determine Edits
	grid memoize;
	grid path;

	for (int i=0;i<clean.size()+2;i++) {
		memoize.push_back(std::vector<int>());
		path.push_back(std::vector<int>());
		for (int j=0;j<noisy.size()+2;j++) {
			memoize[i].push_back(-1);
			path[i].push_back(-1);
		}
	}
	
	buildMinPath(edit_mat, &path, &memoize, clean.size()+1, noisy.size()+1);

	int i = clean.size()+1;
	int j = noisy.size()+1;
	Edits *e;

	while (true) {
		if (i == 1 && j == 1) {
			break;
		}

		if (path[i][j] == 0) {
			if ( (i>=3 && j>=3) && clean[i-2] == noisy[j-3] && clean[i-3] == noisy[j-2]) {
				e = new Edits();
				e->first = clean[i-2];
				e->second = noisy[j-2];
				e->opC = trans;
				final_edits.push_back(*e);
				delete e;
				i-=2;
				j-=2;
			}
			else if (clean[i-2] != noisy[j-2]) {
				e = new Edits();
				e->first = clean[i-2];
				e->second = noisy[j-2];
				e->opC = subs;
				final_edits.push_back(*e);
				delete e;
				i--;
				j--;
			}
			else {
				i--;
				j--;
			}
		}
		else {
			e = new Edits();
			e->second = NO_CHAR;
			if (path[i][j] == 1) {
				e->first = clean[i-2];
				e->second = noisy[j-2];
				e->opC = del;
				i--;
			}
			else if (path[i][j] == 2) {
				e->first = clean[i-2];
				e->second = noisy[j-2];
				e->opC = ins;
				j--;
			}
			else {
				std::cerr << "Unknown cmd" << std::endl;
			}
				
			final_edits.push_back(*e);
			delete e;
		}	
	}	

	return final_edits;
}


void buildMinPath(const grid &g, grid *path, grid *memoize, int i, int j) {
	
	if ((i == 0 || j == 0) || (memoize->at(i)[j] != -1)) {
		return;
	}

	buildMinPath(g, path, memoize, i-1, j-1);
	buildMinPath(g, path, memoize, i-1, j);
	buildMinPath(g, path, memoize, i, j-1);

	int min_weight, min_index;
	min_weight = -1;
	min_index = 0;
	if ((min_weight == -1 || min_weight > memoize->at(i-1)[j-1]) && memoize->at(i-1)[j-1] != -1) {
		min_index = 0;
		min_weight = memoize->at(i-1)[j-1];
	}
	if ((min_weight == -1 || min_weight > memoize->at(i-1)[j]) && memoize->at(i-1)[j] != -1) {
		min_index = 1;
		min_weight = memoize->at(i-1)[j];
	}
	if ((min_weight == -1 || min_weight > memoize->at(i)[j-1]) && memoize->at(i)[j-1] != -1) {
		min_index = 2;
		min_weight = memoize->at(i)[j-1];
	}
	
	if (min_weight == -1) { min_weight++; }
	memoize->at(i)[j] = min_weight+g[i-1][j-1];
	path->at(i)[j] = min_index;
}


bool operator< (const Edits &e1, const Edits &e2) {
	if (e1.first == e2.first) {
		if (e1.second == e2.second) {
			return e1.opC < e2.opC;
		}
		else {
			return e1.second < e2.second;
		}
	}
	else {
		return e1.first < e2.second;
	}
}

bool operator== (const Edits &e1, const Edits &e2) {
	return (e1.first == e2.first) && (e1.second == e2.second) && (e1.opC == e2.opC);
}

std::ostream& operator<< (std::ostream &os, const Edits &e) {
	if (e.second == NO_CHAR) {
		os << e.first << ", NO_CHAR\t" << e.opC;
	}
	else {
		os << e.first << ", " << e.second << "\t" << e.opC;
	}
	return os;
}

std::vector<std::string> split(const std::string &s) {
	std::vector<std::string> ret;
	std::stringstream ss;
	std::string word;
	ss.str(s);

	while (ss >> word) {
		ret.push_back(word);
	}

	return ret;
}
