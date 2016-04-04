#include "pa1.h"
#include "basic_index.h"
#include <fstream>
#include <iostream>
#include <vector>

void BasicIndex::writePosting(std::ofstream &f_out, const PostingList &p) {
	std::vector<int> docs;
	p.GetList(docs);

	int tid = p.GetTermID();
	size_t pllen = p.GetLength();
	f_out.write(reinterpret_cast<const char *>(&tid), sizeof(int));
	f_out.write(reinterpret_cast<const char *>(&pllen), sizeof(size_t));
	
	for (size_t i=0;i<docs.size();i++) {
		f_out.write(reinterpret_cast<const char *>(&docs[i]), sizeof(int));
	}
}

PostingList BasicIndex::readPosting(std::ifstream &f_in) {
	int tid = 0;
	int tmp;
	std::vector<int> postings;
	size_t pllen;
	char *memblock = new char[8];

	if (f_in.is_open()) {
    	f_in.read (memblock, 4);
    	tid = *(reinterpret_cast< int *>(memblock));
    	f_in.read (memblock, 8);
		pllen = *(reinterpret_cast< size_t *>(memblock));
		postings.resize(pllen);

		for (size_t i = 0; i<pllen; i++) {
			f_in.read (memblock, 4);
			tmp = *(reinterpret_cast< int *>(memblock));
			postings[i] = tmp;
		}
	}
	else {
		std::cerr << "File did not open" << std::endl;
	}
	
	delete[] memblock;
	PostingList pl_out(tid,postings);
	return pl_out;
}
