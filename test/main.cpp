/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <iostream>
#include <dollar/Engine.h>

#include <etk/etk.h>
#include <test-debug/debug.h>
#include <gtest/gtest.h>

int main(int _argc, const char *_argv[]) {
	// init Google test :
	::testing::InitGoogleTest(&_argc, const_cast<char **>(_argv));
	// init etk log system and file interface:
	etk::init(_argc, _argv);
	// Run all test with gtest
	return RUN_ALL_TESTS();
}

TEST(TestAll, plop) {
	dollar::Gesture gest;
	gest.set("test", 55, dollar::loadPoints("DATA:test/P.json"));
	gest.configure(0.1, 64, false, 0.1f);
}

/*
 * single-stroke gesture recognition
 */
TEST(TestAll, singleStroke_normal) {
	dollar::Engine reco;
	reco.loadPath("DATA:figure");
	dollar::Results res = reco.recognize(dollar::loadPoints("DATA:test/Arrow.json"), "$N");
	EXPECT_EQ(res.haveMath(), true);
	if (res.haveMath() == false) {
		TEST_INFO("   Recognise noting ...");
		return;
	}
	EXPECT_EQ(res.getName(), "arrow");
	TEST_INFO("Results");
	for (size_t iii=0; iii<res.getSize(); ++iii) {
		TEST_INFO("    - " << res.getName(iii) << " score=" << res.getConfidence(iii));
	}
}

TEST(TestAll, singleStroke_protractor) {
	dollar::Engine reco;
	reco.loadPath("DATA:figure");
	dollar::Results res = reco.recognize(dollar::loadPoints("DATA:test/Arrow.json"), "$N-protractor");
	EXPECT_EQ(res.haveMath(), true);
	if (res.haveMath() == false) {
		TEST_INFO("   Recognise noting ...");
		return;
	}
	EXPECT_EQ(res.getName(), "arrow");
	TEST_INFO("Results");
	for (size_t iii=0; iii<res.getSize(); ++iii) {
		TEST_INFO("    - " << res.getName(iii) << " score=" << res.getConfidence(iii));
	}
}


/*
 * multi-stroke gesture recognition
 */
TEST(TestAll, multiStroke_normal) {
	dollar::Engine reco;
	reco.loadPath("DATA:text");
	dollar::Results res = reco.recognize(dollar::loadPoints("DATA:test/P.json"), "$N");
	EXPECT_EQ(res.haveMath(), true);
	if (res.haveMath() == false) {
		TEST_INFO("   Recognise noting ...");
		return;
	}
	EXPECT_EQ(res.getName(), "P");
	TEST_INFO("Results");
	for (size_t iii=0; iii<res.getSize(); ++iii) {
		TEST_INFO("    - " << res.getName(iii) << " score=" << res.getConfidence(iii));
	}
}

TEST(TestAll, multiStroke_protractor) {
	dollar::Engine reco;
	reco.loadPath("DATA:text");
	dollar::Results res = reco.recognize(dollar::loadPoints("DATA:test/P.json"), "$N-protractor");
	EXPECT_EQ(res.haveMath(), true);
	if (res.haveMath() == false) {
		TEST_INFO("   Recognise noting ...");
		return;
	}
	EXPECT_EQ(res.getName(), "P");
	TEST_INFO("Results");
	for (size_t iii=0; iii<res.getSize(); ++iii) {
		TEST_INFO("    - " << res.getName(iii) << " score=" << res.getConfidence(iii));
	}
}
/*
 * $P algorithms
 */
TEST(TestAll, multiStroke_point) {
	dollar::Engine reco;
	reco.loadPath("DATA:text");
	dollar::Results res = reco.recognize(dollar::loadPoints("DATA:test/P.json"), "$P");
	EXPECT_EQ(res.haveMath(), true);
	if (res.haveMath() == false) {
		TEST_INFO("   Recognise noting ...");
		return;
	}
	EXPECT_EQ(res.getName(), "P");
	TEST_INFO("Results");
	for (size_t iii=0; iii<res.getSize(); ++iii) {
		TEST_INFO("    - " << res.getName(iii) << " score=" << res.getConfidence(iii));
	}
}
/*
 * $P+ algorithms
 */
TEST(TestAll, multiStroke_pointPlus) {
	dollar::Engine reco;
	reco.loadPath("DATA:text");
	dollar::Results res = reco.recognize(dollar::loadPoints("DATA:test/P.json"), "$P+");
	EXPECT_EQ(res.haveMath(), true);
	if (res.haveMath() == false) {
		TEST_INFO("   Recognise noting ...");
		return;
	}
	EXPECT_EQ(res.getName(), "P");
	TEST_INFO("Results");
	for (size_t iii=0; iii<res.getSize(); ++iii) {
		TEST_INFO("    - " << res.getName(iii) << " score=" << res.getConfidence(iii));
	}
}
