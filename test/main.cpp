/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <iostream>
#include <dollar/Engine.hpp>

#include <etk/etk.hpp>
#include <test-debug/debug.hpp>
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
	/*
	dollar::Gesture gest;
	gest.set("test", 55, dollar::loadPoints("DATA:test/P.json"));
	gest.configure(0.1, 64, false, 0.1f);
	*/
}

/*
 * single-stroke gesture recognition
 */
TEST(TestAll, singleStroke_normal) {
	ememory::SharedPtr<dollar::Engine> reco = dollar::createEngine("$N");
	reco->loadPath("DATA:figure");
	dollar::Results res = reco->recognize(dollar::loadPoints("DATA:test/arrow.json"));
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
	ememory::SharedPtr<dollar::Engine> reco = dollar::createEngine("$N");
	reco->loadPath("DATA:figure");
	dollar::Results res = reco->recognize(dollar::loadPoints("DATA:test/arrow.json"));
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
	ememory::SharedPtr<dollar::Engine> reco = dollar::createEngine("$N");
	reco->loadPath("DATA:text");
	dollar::Results res = reco->recognize(dollar::loadPoints("DATA:test/P.json"));
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
	ememory::SharedPtr<dollar::Engine> reco = dollar::createEngine("$N-protractor");
	reco->loadPath("DATA:text");
	dollar::Results res = reco->recognize(dollar::loadPoints("DATA:test/P.json"));
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
	ememory::SharedPtr<dollar::Engine> reco = dollar::createEngine("$P");
	reco->loadPath("DATA:text");
	dollar::Results res = reco->recognize(dollar::loadPoints("DATA:test/P.json"));
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
	ememory::SharedPtr<dollar::Engine> reco = dollar::createEngine("$P+");
	reco->loadPath("DATA:text");
	dollar::Results res = reco->recognize(dollar::loadPoints("DATA:test/P.json"));
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
