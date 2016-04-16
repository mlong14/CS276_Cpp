#ifndef PA1_UTILS_H_
#define PA1_UTILS_H_

#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include <map>

enum Opcode { subs, del, ins, trans };

struct Edits {
	char first;
	char second;
	Opcode opC;
};

const extern char NO_CHAR;

bool operator< (const Edits &e1, const Edits &e2);
bool operator== (const Edits &e1, const Edits &e2);
std::ostream& operator<< (std::ostream &os, const Edits &e);

std::vector<std::pair<std::string,std::string> > listDirectory(std::string input_dir);
std::vector<std::pair<std::string,std::string> > listFiles(std::string input_dir);
int getEditDistanceNum(std::string clean, std::string noisy);
std::vector<Edits> getEditDistance(std::string clean, std::string noisy);
std::vector<std::string> split(const std::string &s);

#endif
