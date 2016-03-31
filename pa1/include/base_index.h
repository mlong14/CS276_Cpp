#ifndef PA1_BASEINDEX_H_
#define PA1_BASEINDEX_H_

#include "pa1.h"
#include <fstream>

class BaseIndex {
public:
	BaseIndex() {};
	~BaseIndex() {};
	virtual PostingList readPosting(std::ifstream &f_in) = 0;
	virtual void writePosting(std::ofstream &f_out, const PostingList &p) = 0;
};

#endif //PA1_BASEINDEX_H_

