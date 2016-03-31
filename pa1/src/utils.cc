
#include "utils.h"
#include "pa1.h"
#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <map>
#include <vector>
#include <dirent.h>
#include <utility>

std::ostream& operator<< (std::ostream &os, const std::pair<int,int> &p) {
	os << std::get<0>(p) << ": " << std::get<1>(p);
	return os;
}

void printMap(std::map<int,std::deque<int> > map_obj) {
	for (std::map<int,std::deque<int> >::iterator iter = map_obj.begin(); iter!=map_obj.end(); iter++) {
		std::cout << iter->first << ": ";
		for (std::deque<int>::iterator iter2 = iter->second.begin(); iter2!=iter->second.end(); iter2++) {
			std::cout << *iter2 << ", ";
		}
		std::cout << std::endl;
	}
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

bool postingPairComparator(std::pair<int,int> p1, std::pair<int,int> p2) {
	return std::get<1>(p1) < std::get<1>(p2);
}

bool operator< (const tid_triples &x, const tid_triples &y) {
	return x.posting_size < y.posting_size;
}

std::ostream& operator<< (std::ostream &os, const tid_triples &x) {
	os << "{" << x.tid << ", " << x.posting_size << ", " << x.pl_pos << "}";
	return os;
}

