
#include "gtest/gtest.h"
#include "pa1.h"
#include <vector>
#include <iostream>

class PostingsListTest : public ::testing::Test {
	protected:
		virtual void SetUp() {
			docs.push_back(10);
			docs.push_back(20);
			docs.push_back(30);
			docs.push_back(40);
			tid = 5;
			pl = new PostingList(tid,docs);
		}
		virtual void TearDown() {
			delete pl;
		}

		std::vector<int> docs;
		int tid;
		PostingList *pl;
};

TEST_F(PostingsListTest,Test1) {
	EXPECT_EQ(pl->GetTermID(),5);
	EXPECT_EQ(pl->GetLength(),4);
}

TEST_F(PostingsListTest,Test2) {
	EXPECT_EQ(pl->GetLength(),4);
}
