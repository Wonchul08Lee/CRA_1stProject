
#include "gmock/gmock.h"
#include <fstream>
#include <sstream>
#include <streambuf>
#include <cstdio>
#include "fix_keyword.h"

using namespace testing;
class KeywordFixture : public Test {
public:

    KeywordRecommender recommender;

    void clearKeywordState() {
        for (int i = 0; i < 7; ++i) weekBest[i].clear();
        for (int i = 0; i < 2; ++i) twoBest[i].clear();
        UZ = 9;
    }
};

TEST_F(KeywordFixture, LoadAndProcessKeyword_Basic) {
    const char* testFile_fake = "";
    std::ofstream fout_fake(testFile_fake);
    recommender.loadAndProcessKeyword(testFile_fake);
    ASSERT_FALSE(fout_fake.is_open());

    const char* testFile = "tmp.txt";
    std::ofstream fout(testFile);
    ASSERT_TRUE(fout.is_open());
    fout << "apple monday\n";
    fout << "banana tuesday\n";
    fout.close();

    internal::CaptureStdout();
    recommender.loadAndProcessKeyword(testFile);
    std::string output = internal::GetCapturedStdout();

    EXPECT_NE(output.find("apple"), std::string::npos);
    EXPECT_NE(output.find("banana"), std::string::npos);

    std::remove(testFile);
}

TEST_F(KeywordFixture, 10SimilarkeywordsPerDay) {
    clearKeywordState();

    std::string day = "monday";
    // 1. 유사한 10개의 키워드 삽입 
    vector<string> results;
    for (int i = 0; i < 9; ++i) {
        std::string word = "apple" + std::to_string(i);
        results.push_back(recommender.recommandKeyword(word, day));
    }
    std::string word = "amonde" ;
    results.push_back(recommender.recommandKeyword(word, day));

    ASSERT_EQ(weekBest[0].size(), 2);

    int appleCount = 0;
    int amondeCount = 0;
    for (const auto& s : results) {
        if (s.find("apple") != std::string::npos) {
            ++appleCount;
        }
        if (s.find("amonde") != std::string::npos) {
            ++amondeCount;
        }
    }
    ASSERT_EQ(appleCount, 9);
    ASSERT_EQ(amondeCount, 1);
}

TEST_F(KeywordFixture, morethan10differentkeywordsPerDay) {
    clearKeywordState();

    std::string day = "monday";
    // 1. 다른 11개의 키워드 삽입 
    std::vector<std::string> inputWords = {
        "apple", "banana", "car", "dog", "egg", "fish", "grape", "hat", "ice", "jungle", "kite"
    };

    std::vector<std::string> results;
    for (const auto& word : inputWords) {
        recommender.recommandKeyword(word, day);
    }

    for (const auto& node : weekBest[0]) {       
        results.push_back(node.name);
	}

	ASSERT_EQ(weekBest[0].size(), 10);  // monday에 10개 키워드가 있어야 함
    std::vector<std::string> actual(inputWords.rbegin(), inputWords.rend() - 1);
    EXPECT_EQ(actual, results);
}

TEST_F(KeywordFixture, DayIndex) {
    EXPECT_EQ(recommender.getDayIndexes("monday"), std::make_pair(0, 0));
    EXPECT_EQ(recommender.getDayIndexes("tuesday"), std::make_pair(1, 0));
    EXPECT_EQ(recommender.getDayIndexes("wednesday"), std::make_pair(2, 0));
    EXPECT_EQ(recommender.getDayIndexes("thursday"), std::make_pair(3, 0));
    EXPECT_EQ(recommender.getDayIndexes("friday"), std::make_pair(4, 0));
    // 주말
    EXPECT_EQ(recommender.getDayIndexes("saturday"), std::make_pair(5, 1));
    EXPECT_EQ(recommender.getDayIndexes("sunday"), std::make_pair(6, 1));

    // 잘못된 입력은 모두 (0, 0) 반환
    EXPECT_EQ(recommender.getDayIndexes(""), std::make_pair(0, 0));
    EXPECT_EQ(recommender.getDayIndexes("holiday"), std::make_pair(0, 0));
    EXPECT_EQ(recommender.getDayIndexes("MONDAY"), std::make_pair(0, 0)); // 대소문자 구분
}


TEST_F(KeywordFixture, PerfectHit) {
    // Given: name이 "apple"이고 point가 100인 Node2를 bestList에 추가
    std::vector<Node2> bestList = { { "apple", 100 } };
    std::string w = "apple";
    long long int max = 0;
    int flag = 0;

    recommender.calPointPerfectHit(bestList, w, max, flag);

    // Then ; 조건문이 실행되어야 하므로, flag == 1, max와 point가 10% 증가했는지 확인
    EXPECT_EQ(flag, 1);
    EXPECT_EQ(max, 110); // 100 + 10% = 110
    EXPECT_EQ(bestList[0].point, 110);
}


TEST_F(KeywordFixture, ResetKeywordPointsWhenUZExceedsMax) {
    clearKeywordState();

    // UZ를 max_keyword_point - 1로 설정
    UZ = max_keyword_point - 1;
    weekBest[0].push_back({ "apple", UZ });
    std::string result = recommender.recommandKeyword("apple", "monday");

    // recommandKeyword 내부에서 UZ가 증가하여 max_keyword_point에 도달하므로 초기화
    EXPECT_EQ(UZ, 9); // resetKeywordPoints()에서 9로 초기화됨
    EXPECT_EQ(weekBest[0][0].point, 1); // weekBest[0][0].point도 초기화됨
}

TEST_F(KeywordFixture, ResetKeywordPointsWhenMax1ExceedsMax) {
    clearKeywordState();

    weekBest[0].push_back({ "apple", max_keyword_point });
    std::string result = recommender.recommandKeyword("apple", "monday");
    EXPECT_EQ(UZ, 9);
    EXPECT_EQ(weekBest[0][0].point, 1);
}

TEST_F(KeywordFixture, ResetKeywordPointsWhenMax2ExceedsMax) {
    clearKeywordState();

    // twoBest[0]에 point가 max_keyword_point 이상인 노드 추가
    twoBest[1].push_back({ "apple", max_keyword_point });
    std::string result = recommender.recommandKeyword("apple", "saturday");
    // calPointPerfectHit에서 max2가 max_keyword_point 이상이 되어 초기화됨
    EXPECT_EQ(UZ, 9);
    EXPECT_EQ(twoBest[1][0].point, 1);
}