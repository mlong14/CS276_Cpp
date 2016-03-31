
#include "pa1.h"

PostingList::PostingList() {}

PostingList::PostingList(int tid, const std::vector<int> &list) : 
	termID(tid), postings(list)
{}

PostingList::~PostingList() {
	//Nothing to do
}

PostingList::PostingList(const PostingList &obj) : 
	termID(obj.termID), postings(obj.postings) 
{}

int PostingList::GetTermID() const {
	return this->termID;
}

void PostingList::GetList( std::vector<int> &list) const {
	list = this->postings;
}

size_t PostingList::GetLength() const {
	return this->postings.size();
}

bool operator< (const PostingList &p1, const PostingList &p2) {
	return p1.GetTermID()<p2.GetTermID();
}

bool operator== (const PostingList &p1, const PostingList &p2) {
	return p1.GetTermID()==p2.GetTermID();
}


bool operator> (const PostingList &p1, const PostingList &p2) {
	return p1.GetTermID()>p2.GetTermID();
}

std::ostream& operator<<(std::ostream &os, const PostingList &p) {
	std::vector<int> post;
	p.GetList(post);
	os << p.GetTermID() << ": ";
	for (std::vector<int>::const_iterator i = post.begin(); i != post.end(); i++) {
		os << *i << " ";
	}
	os << "\n";
	return os;
}

