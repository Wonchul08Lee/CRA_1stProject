#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <functional>

using namespace std;
#define max_keyword_point 2100000000
#define numDataSet 500

struct Node {
	string word;
	string weekday;
	Node(const string& w, const string& wk) : word(w), weekday(wk) {}
};

struct Node2 {
	string name;
	long long int point;

	bool operator<(const Node2& other) const {
		return point > other.point;
	}
};

extern vector<Node2> weekBest[7]; //岿 ~ 老		
extern vector<Node2> twoBest[2]; //乞老, 林富
extern int UZ;

class KeywordRecommender {
public:
    using SimilarityFunc = std::function<int(const std::string&, const std::string&)>;

    explicit KeywordRecommender(SimilarityFunc func = nullptr) {
        if (func) {
            similarityFunc = func;
        }
        else {
            similarityFunc = [this](const std::string& a, const std::string& b) {
                return this->levenshtein(a, b); };
        }
		resetKeywordPoints();
	}

	std::pair<int, int> getDayIndexes(const std::string& wk);
	void calPointPerfectHit(vector<Node2>& bestList, const string& w, long long int& max, int& flag);

	string recommandKeyword(string w, string wk);
	void loadAndProcessKeyword(const std::string& filename);

private:
    SimilarityFunc similarityFunc;

	int levenshtein(const std::string& source, const std::string& target);
	bool similer(const std::string& name1, const std::string& name2);
	void resetPoints(vector<Node2>* arr, int size);
	void resetKeywordPoints();
	string findSimilarKeyword(const vector<Node2>& list, const string& w);
	void addAndSortKeyword(vector<Node2>& list, const string& w, int point, size_t maxSize = 10);
	vector<string> processKeywords(const vector<Node>& data);
	vector<Node> readInputFile(const string& filename);
};
