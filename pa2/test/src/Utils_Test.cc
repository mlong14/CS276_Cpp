
#include "gtest/gtest.h"
#include "utils.h"
#include <vector>
#include <iostream>
#include <string>

class UtilsTest : public ::testing::Test {
	protected:
		virtual void SetUp() {}
		virtual void TearDown() {}

		std::string clean, noisy;
		Edits *e;
		std::vector<Edits> edits;
		std::vector<Edits> gold;
};

TEST_F(UtilsTest, distance1) {
	clean = "boco";
	noisy = "boko";

	e = new Edits();
	e->first = 'c';
	e->second = 'k';
	e->opC = subs;
	gold.push_back(*e);
	delete e;
	
	edits = getEditDistance(clean,noisy);

	EXPECT_EQ(edits.size(), 1);
	
	sort(edits.begin(), edits.end());
	sort(gold.begin(), gold.end());

	for (int i=0;i<edits.size();i++) {
		EXPECT_EQ(edits[i], gold[i]);
	}
};

TEST_F(UtilsTest, distance2) {
	clean = "book";
	noisy = "boko";

	e = new Edits();
	e->first = 'k';
	e->second = 'o';
	e->opC = trans;
	gold.push_back(*e);
	delete e;
	
	edits = getEditDistance(clean,noisy);

	EXPECT_EQ(edits.size(), 1);
	
	sort(edits.begin(), edits.end());
	sort(gold.begin(), gold.end());

	for (int i=0;i<edits.size();i++) {
		EXPECT_EQ(edits[i], gold[i]);
	}
};

TEST_F(UtilsTest, distance3) {
	clean = "book";
	noisy = "bomok";

	e = new Edits();
	e->first = 'o';
	e->second = 'm';
	e->opC = ins;
	gold.push_back(*e);
	delete e;
	
	edits = getEditDistance(clean,noisy);

	EXPECT_EQ(edits.size(), 1);
	
	sort(edits.begin(), edits.end());
	sort(gold.begin(), gold.end());

	for (int i=0;i<edits.size();i++) {
		EXPECT_EQ(edits[i], gold[i]);
	}
};

TEST_F(UtilsTest, distance4) {
	clean = "book";
	noisy = "bok";

	e = new Edits();
	e->first = 'o';
	e->second = 'o';
	e->opC = del;
	gold.push_back(*e);
	delete e;
	
	edits = getEditDistance(clean,noisy);

	EXPECT_EQ(edits.size(), 1);
	
	sort(edits.begin(), edits.end());
	sort(gold.begin(), gold.end());

	for (int i=0;i<edits.size();i++) {
		EXPECT_EQ(edits[i], gold[i]);
	}
};

TEST_F(UtilsTest, distance5) {
	clean = "obkh";
	noisy = "bokoh";

	e = new Edits();
	e->first = 'k';
	e->second = 'o';
	e->opC = ins;
	gold.push_back(*e);
	delete e;
	
	e = new Edits();
	e->first = 'b';
	e->second = 'o';
	e->opC = trans;
	gold.push_back(*e);
	delete e;

	edits = getEditDistance(clean,noisy);

	EXPECT_EQ(edits.size(), 2);
	
	sort(edits.begin(), edits.end());
	sort(gold.begin(), gold.end());

	for (int i=0;i<edits.size();i++) {
		EXPECT_EQ(edits[i], gold[i]);
	}
};



