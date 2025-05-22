
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
    // 1. ������ 10���� Ű���� ���� 
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
    // 1. �ٸ� 11���� Ű���� ���� 
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

	ASSERT_EQ(weekBest[0].size(), 10);  // monday�� 10�� Ű���尡 �־�� ��
    std::vector<std::string> actual(inputWords.rbegin(), inputWords.rend() - 1);
    EXPECT_EQ(actual, results);
}

TEST_F(KeywordFixture, DayIndex) {
    EXPECT_EQ(recommender.getDayIndexes("monday"), std::make_pair(0, 0));
    EXPECT_EQ(recommender.getDayIndexes("tuesday"), std::make_pair(1, 0));
    EXPECT_EQ(recommender.getDayIndexes("wednesday"), std::make_pair(2, 0));
    EXPECT_EQ(recommender.getDayIndexes("thursday"), std::make_pair(3, 0));
    EXPECT_EQ(recommender.getDayIndexes("friday"), std::make_pair(4, 0));
    // �ָ�
    EXPECT_EQ(recommender.getDayIndexes("saturday"), std::make_pair(5, 1));
    EXPECT_EQ(recommender.getDayIndexes("sunday"), std::make_pair(6, 1));

    // �߸��� �Է��� ��� (0, 0) ��ȯ
    EXPECT_EQ(recommender.getDayIndexes(""), std::make_pair(0, 0));
    EXPECT_EQ(recommender.getDayIndexes("holiday"), std::make_pair(0, 0));
    EXPECT_EQ(recommender.getDayIndexes("MONDAY"), std::make_pair(0, 0)); // ��ҹ��� ����
}


TEST_F(KeywordFixture, PerfectHit) {
    // Given: name�� "apple"�̰� point�� 100�� Node2�� bestList�� �߰�
    std::vector<Node2> bestList = { { "apple", 100 } };
    std::string w = "apple";
    long long int max = 0;
    int flag = 0;

    recommender.calPointPerfectHit(bestList, w, max, flag);

    // Then ; ���ǹ��� ����Ǿ�� �ϹǷ�, flag == 1, max�� point�� 10% �����ߴ��� Ȯ��
    EXPECT_EQ(flag, 1);
    EXPECT_EQ(max, 110); // 100 + 10% = 110
    EXPECT_EQ(bestList[0].point, 110);
}


TEST_F(KeywordFixture, ResetKeywordPointsWhenUZExceedsMax) {
    clearKeywordState();

    // UZ�� max_keyword_point - 1�� ����
    UZ = max_keyword_point - 1;
    weekBest[0].push_back({ "apple", UZ });
    std::string result = recommender.recommandKeyword("apple", "monday");

    // recommandKeyword ���ο��� UZ�� �����Ͽ� max_keyword_point�� �����ϹǷ� �ʱ�ȭ
    EXPECT_EQ(UZ, 9); // resetKeywordPoints()���� 9�� �ʱ�ȭ��
    EXPECT_EQ(weekBest[0][0].point, 1); // weekBest[0][0].point�� �ʱ�ȭ��
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

    // twoBest[0]�� point�� max_keyword_point �̻��� ��� �߰�
    twoBest[1].push_back({ "apple", max_keyword_point });
    std::string result = recommender.recommandKeyword("apple", "saturday");
    // calPointPerfectHit���� max2�� max_keyword_point �̻��� �Ǿ� �ʱ�ȭ��
    EXPECT_EQ(UZ, 9);
    EXPECT_EQ(twoBest[1][0].point, 1);
}