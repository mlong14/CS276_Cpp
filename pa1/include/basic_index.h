#ifndef PA1_BASICINDEX_H_
#define PA1_BASICINDEX_H_

#include "base_index.h"
#include <fstream>

class BasicIndex: public BaseIndex {
	public:
		PostingList readPosting(std::ifstream &f_in);
		void writePosting(std::ofstream &f_out, const PostingList &p);
};

#endif //PA1_BASICINDEX_H_
