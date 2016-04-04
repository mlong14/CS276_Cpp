
#include "utils.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <dirent.h>
#include <utility>

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
