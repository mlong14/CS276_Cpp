#ifndef PA1_UTILS_H_
#define PA1_UTILS_H_

#include <utility>
#include <string>
#include <vector>
#include <map>

std::vector<std::pair<std::string,std::string> > listDirectory(std::string input_dir);
std::vector<std::pair<std::string,std::string> > listFiles(std::string input_dir);

#endif
