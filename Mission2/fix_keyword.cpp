#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <tuple>
#include "fix_keyword.h"

using namespace std;

vector<Node2> weekBest[7];
vector<Node2> twoBest[2];
int UZ = 9;


// 레벤슈타인 거리 계산 알고리즘 (문자열 유사도 검사)
int KeywordRecommender::levenshtein(const std::string& source, const std::string& target) {
	const size_t sourceLength = source.size();
	const size_t targetLength = target.size();

	std::vector<std::vector<int>> distance(sourceLength + 1, std::vector<int>(targetLength + 1));

	for (size_t i = 0; i <= sourceLength; ++i) distance[i][0] = static_cast<int>(i);
	for (size_t j = 0; j <= targetLength; ++j) distance[0][j] = static_cast<int>(j);

	for (size_t i = 1; i <= sourceLength; ++i) {
		for (size_t j = 1; j <= targetLength; ++j) {
			if (source[i - 1] == target[j - 1])
				distance[i][j] = distance[i - 1][j - 1];
			else
				distance[i][j] = 1 + std::min({ distance[i - 1][j], distance[i][j - 1], distance[i - 1][j - 1] });
		}
	}
	return distance[sourceLength][targetLength];
}

// 점수 환산, sililer 함수 이름은 유지한채, levenshtein 을 다른 알고리즘으로 대체할려면 추상화 필요.
bool KeywordRecommender::similer(const std::string& name1, const std::string& name2) {
	if (name1.empty() && name2.empty()) return true;    // 둘 다 비어있으면 true 반환
	if (name1.empty() || name2.empty()) return false;   // 하나라도 비어있으면 false

	int editDistance = levenshtein(name1, name2);
	int maxLength = std::max(name1.length(), name2.length());
	// 유사도 비율 (1.0: 완전히 같음, 0.0: 전혀 다름)
	double similarityRatio = 1.0 - (double)editDistance / maxLength;
	int similarityScore = 1 + static_cast<int>(similarityRatio * 99);

	return similarityScore >= 80;
}

// 요일 문자열로부터 index, index2 반환
std::pair<int, int> KeywordRecommender::getDayIndexes(const std::string& wk) {
	int weekdayIndex = 0;
	if (wk == "monday") weekdayIndex = 0;
	else if (wk == "tuesday") weekdayIndex = 1;
	else if (wk == "wednesday") weekdayIndex = 2;
	else if (wk == "thursday") weekdayIndex = 3;
	else if (wk == "friday") weekdayIndex = 4;
	else if (wk == "saturday") weekdayIndex = 5;
	else if (wk == "sunday") weekdayIndex = 6;

	int weekTypeIndex = (weekdayIndex >= 0 && weekdayIndex <= 4) ? 0 : 1;
	return { weekdayIndex, weekTypeIndex };
}


void KeywordRecommender::calPointPerfectHit(vector<Node2>& bestList, const string& w, long long int& max, int& flag) {
	for (Node2& node : bestList) {
		if (node.name == w) {		
			max = static_cast<long long>(node.point) + static_cast<long long>(node.point * 0.1);
			node.point += static_cast<long long>(node.point * 0.1);
			flag = 1;
			break;
		}
	}
}

void KeywordRecommender::resetPoints(vector<Node2>* arr, int size) {
	for (int i = 0; i < size; i++) {
		int num = 1;
		for (Node2& node : arr[i]) {
			node.point = num++;
		}
	}
}

void KeywordRecommender::resetKeywordPoints() {
	UZ = 9; // init.
	resetPoints(weekBest, 7);	// 요일별 weekBest 점수 초기화 
	resetPoints(twoBest, 2);	// 평일/주말 twoBest 점수 초기화	
}

// 유사한 키워드가 있으면 반환, 없으면 빈 문자열 반환
string KeywordRecommender::findSimilarKeyword(const vector<Node2>& list, const string& w) {
	for (const Node2& node : list) {
		if (similer(node.name, w)) {
			return node.name;
		}
	}
	return "";
}

// 키워드 벡터에 새 키워드 추가 및 정렬, 최대 크기 유지
void KeywordRecommender::addAndSortKeyword(vector<Node2>& list, const string& w, int point, size_t maxSize) {
	if (list.size() < maxSize) {	// 10이하
		list.push_back({ w, point });
		std::sort(list.begin(), list.end());
	}
	else if (!list.empty() && list.back().point < point) {	
		list.pop_back();
		list.push_back({ w, point });
		std::sort(list.begin(), list.end());
	}
}

string KeywordRecommender::recommandKeyword(string keyword, string weekday) {
	
	UZ++;
	int weekdayIndex, weekTypeIndex;
	std::tie(weekdayIndex, weekTypeIndex) = getDayIndexes(weekday);

	int point = UZ;
	long long int max1 = 0;
	long long int max2 = 0;
	// 1. 완벽 HIT
	int flag = 0;
	calPointPerfectHit(weekBest[weekdayIndex], keyword, max1, flag);
	calPointPerfectHit(twoBest[weekTypeIndex], keyword, max2, flag);

 	//재정렬 작업, 
	if (UZ >= max_keyword_point || max1 >= max_keyword_point || max2 >= max_keyword_point) {
		resetKeywordPoints();
	}

	if (flag == 1) {
		return keyword;		// keyword 반환
	}

	// 2. 찰떡 HIT
	string similarKeyword = findSimilarKeyword(weekBest[weekdayIndex], keyword);
	if (!similarKeyword.empty()) return similarKeyword;

	similarKeyword = findSimilarKeyword(twoBest[weekTypeIndex], keyword);
	if (!similarKeyword.empty()) return similarKeyword;

	// 3. 완벽 HIT / 찰떡 HIT 둘다 아닌경우
	addAndSortKeyword(weekBest[weekdayIndex], keyword, point);
	addAndSortKeyword(twoBest[weekTypeIndex], keyword, point);

	return keyword;
}

// 1. 파일에서 (word, day) 쌍을 읽어오는 함수
vector<Node> KeywordRecommender::readInputFile(const string & filename) {
	vector<Node> data;
	ifstream fin{ filename };
	if (!fin.is_open()) {
		cerr << "Can not open the file: " << filename << endl;
		return data;
	}
	for (int idx = 0; idx < numDataSet; idx++) {
		string word, day;
		if (!(fin >> word >> day)) break;
		data.emplace_back(word, day);
	}
	return data;
}


// 입력 벡터를 받아 결과 벡터를 반환
vector<string> KeywordRecommender::processKeywords(const vector<Node>& data) {
	vector<string> results;
	for (const auto& p : data) {
		string word = p.word;
		string day = p.weekday;
		results.push_back(recommandKeyword(word, day));
	}
	return results;
}

void KeywordRecommender::loadAndProcessKeyword(const std::string& filename) {
	vector<Node> data = readInputFile(filename);
	auto results = processKeywords(data);
	for (const auto& ret : results) {
		std::cout << ret << "\n";
	}
}