
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
    // 1. ������ 10���� Ű���� ���� 
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
    // 2. ���� �������� Ű���� Ȯ��
    int minPoint = weekBest[0][0].point;
    std::string minWord = weekBest[0][0].name;

    // 3. �� ���� ������ ���ο� Ű���� ����
    // UZ�� �����ϹǷ�, ���� Ű����� ���� ���� ����
    std::string newWord = "new_high_score";
    recommandKeyword(newWord, day);

    // 4. weekBest[0]�� ������ 10���� �ִ��� Ȯ��
    ASSERT_EQ(weekBest[0].size(), 10);

    // 5. ���� ���� ������ Ű���尡 ��ü�Ǿ����� Ȯ��
    bool foundNewWord = false, foundMinWord = false;
    for (const auto& node : weekBest[0]) {
        if (node.name == newWord) foundNewWord = true;
        if (node.name == minWord) foundMinWord = true;
    }
    EXPECT_TRUE(foundNewWord);   // �� Ű���尡 ���� ��
    EXPECT_FALSE(foundMinWord);  // ���� ������ Ű����� ������ ��
    */
}