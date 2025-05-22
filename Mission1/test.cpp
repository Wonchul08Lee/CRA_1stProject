
#include "gmock/gmock.h"
#include <fstream>
#include <sstream>
#include <streambuf>
#include <cstdio>
#include "fix_keyword.h"

class CoutRedirect {
    std::streambuf* old;
    std::ostringstream ss;
public:
    CoutRedirect() : old(std::cout.rdbuf(ss.rdbuf())) {}
    ~CoutRedirect() { std::cout.rdbuf(old); }
    std::string str() const { return ss.str(); }
};

TEST(KeywordTest, LoadAndProcessKeyword_Basic) {
    const char* testFile = "tmp.txt";
    std::ofstream fout(testFile);
    ASSERT_TRUE(fout.is_open());
    fout << "apple monday\n";
    fout << "banana tuesday\n";
    fout.close();

    CoutRedirect capture;
    loadAndProcessKeyword(testFile);
    std::string output = capture.str();

    EXPECT_NE(output.find("apple"), std::string::npos);
    EXPECT_NE(output.find("banana"), std::string::npos);

    std::remove(testFile);
}


void clearKeywordState() {
    for (int i = 0; i < 7; ++i) weekBest[i].clear();
    for (int i = 0; i < 2; ++i) twoBest[i].clear();
    UZ = 9;
}

TEST(KeywordTest, 10SimilarkeywordsPerDay) {
    clearKeywordState();

    std::string day = "monday";
    // 1. 유사한 10개의 키워드 삽입 
    vector<string> results;
    for (int i = 0; i < 10; ++i) {
        std::string word = "apple" + std::to_string(i);
        results.push_back(recommandKeyword(word, day));
    }
    ASSERT_EQ(weekBest[0].size(), 1);

    bool hasApple = false;
    for (const auto& s : results) {
        if (s.find("apple") != std::string::npos) {
            hasApple = true;
            break;
        }
    }
    ASSERT_TRUE(hasApple);
}

TEST(KeywordTest, 10SimilarkeywordsPerDay1) {
    clearKeywordState();
    std::string day = "monday";
    std::vector<std::string> distinctWords = {
        "apple", "banana", "car", "dog", "egg", "fish", "grape", "hat", "ice", "jungle", "kite"
    };

    std::vector<std::string> results;
    for (const auto& word : distinctWords) {
        results.push_back(recommandKeyword(word, day));
    }
}



TEST(KeywordTest, ReplaceLowestScoreKeywordWhenFull) {
  
    /*
    // 2. 현재 최저점수 키워드 확인
    int minPoint = weekBest[0][0].point;
    std::string minWord = weekBest[0][0].name;

    // 3. 더 높은 점수의 새로운 키워드 삽입
    // UZ는 증가하므로, 다음 키워드는 가장 높은 점수
    std::string newWord = "new_high_score";
    recommandKeyword(newWord, day);

    // 4. weekBest[0]에 여전히 10개만 있는지 확인
    ASSERT_EQ(weekBest[0].size(), 10);

    // 5. 가장 낮은 점수의 키워드가 대체되었는지 확인
    bool foundNewWord = false, foundMinWord = false;
    for (const auto& node : weekBest[0]) {
        if (node.name == newWord) foundNewWord = true;
        if (node.name == minWord) foundMinWord = true;
    }
    EXPECT_TRUE(foundNewWord);   // 새 키워드가 들어가야 함
    EXPECT_FALSE(foundMinWord);  // 기존 최저점 키워드는 빠져야 함
    */
}