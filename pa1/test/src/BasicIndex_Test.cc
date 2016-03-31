
#include "gtest/gtest.h"
#include "pa1.h"
#include "basic_index.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <dirent.h>

class BasicIndexTest: public ::testing::Test {
	protected:
		virtual void SetUp() {
			std::vector<int> docs (4,99);
			docs.push_back(10);
			docs.push_back(20);
			docs.push_back(30);
			int tid = 5;
			pl = new PostingList(tid,docs);
			system("mkdir ./tmp");
		}
		virtual void TearDown() {
			delete pl;
			system("rm -r ./tmp");
		}
		
		BasicIndex index;
		PostingList *pl;
};

TEST_F(BasicIndexTest, Test1) {
	std::ofstream f_out;
	std::ifstream f_in;
	PostingList read_pl;
	
	f_out.open("./tmp/pl1",std::ios::binary);
	index.writePosting(f_out, *pl);
	f_out.close();
	f_in.open("./tmp/pl1",std::ios::binary);
    read_pl = index.readPosting(f_in);
	f_in.close();

	std::vector<int> pl1_postings;
	std::vector<int> pl2_postings;
	pl->GetList( pl1_postings );
	read_pl.GetList( pl2_postings );

	EXPECT_EQ((pl1_postings==pl2_postings),1);
	EXPECT_EQ((pl->GetTermID() == read_pl.GetTermID()),1);
	EXPECT_EQ((pl->GetLength() == read_pl.GetLength()),1);
}
