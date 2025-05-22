#pragma once
#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Node {
	string w;
	string wk;
};

struct Node2 {
	string name;
	int point;

	bool operator<(const Node2& other) const {
		return point < other.point;
	}
};


extern vector<Node2> weekBest[7]; //岿 ~ 老		
extern vector<Node2> twoBest[2]; //乞老, 林富
extern int UZ;

void loadAndProcessKeyword(const std::string& filename);
std::pair<int, int> getDayIndexes(const std::string& wk);
void calPointPerfectHit(vector<Node2>& bestList, const string& w, long long int& max, int& flag);
void resetPoints(vector<Node2>* arr, int size);
void resetKeywordPoints();
string findSimilarKeyword(const vector<Node2>& list, const string& w);
string recommandKeyword(string w, string wk);