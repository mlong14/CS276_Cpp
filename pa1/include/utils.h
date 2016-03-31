#ifndef PA1_UTILS_H_
#define PA1_UTILS_H_

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <map>
#include <deque>

struct tid_triples {
	int tid;
	size_t posting_size;
	long pl_pos;
};

std::ostream& operator<< (std::ostream &os, const std::pair<int,int> &p);
bool operator< (const tid_triples &x, const tid_triples &y);
std::vector<std::pair<std::string,std::string> > listDirectory(std::string input_dir);
std::vector<std::pair<std::string,std::string> > listFiles(std::string input_dir);
bool postingPairComparator(std::pair<int,int> p1, std::pair<int,int> p2);
void mergeBlocks(std::vector<std::string> blocknames);
void printMap(std::map<int, std::deque<int> > map_obj);

#endif
