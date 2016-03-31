#ifndef PA1_PA1_H_
#define PA1_PA1_H_

#include <iostream>
#include <vector>

class PostingList {
	public:
		PostingList();
		PostingList(int termID, const std::vector<int> &list);
		PostingList(const PostingList &obj);
		~PostingList();
		int GetTermID() const;
		size_t GetLength() const;
		void GetList(std::vector<int> &list) const;
	private:
		int termID;
		std::vector<int> postings;
};

bool operator< (const PostingList &p1, const PostingList &p2);
bool operator== (const PostingList &p1, const PostingList &p2);
bool operator> (const PostingList &p1, const PostingList &p2);
std::ostream& operator<< (std::ostream &os, const PostingList &p);

#endif //PA1_PA1_H_
