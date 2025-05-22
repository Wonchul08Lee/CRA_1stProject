# CRA_1stProject
공통과제 1차

## mission 1
* 함수레벨로 클린코드만들기
* 요구사항
  - 가독성좋은코드
  - 안전성이좋은코드
    - 안전한문법사용
    - 순수함수권장
   
* UT 를 위해, test.cpp, main.cpp 수정
* logic 은 fix_keyword.cpp/.h 로 작성

## mission 2

| 요구사항 | 체크리스트 |
|--------|-----|
| D1   | O   | 
| D2   | O   | 
| D3   | O   | 
| D4   | O   | 
| D5   | O   | 


* 1. 10개의 키워드가 모두 채워진 상태에서 새로운 키워드가 주어지는경우, 최저점수를 가진 키워드와 비교하여 대체될수있다.
   -> TEST(KeywordTest, morethan10differentkeywordsPerDay)  시나리오 통해, sorting 역순으로 안됨을 확인하고, Node2 operator 수정
     
* 2. 키워드 점수가 21억에 도달하면 점수 재정렬이 일어난다
   int 로 되어있는 point 와, 연산 결과에 대한 overflow 방지위해, type 변경과 casting 이 필요
   TEST(RecommandKeywordTest, ResetKeywordPointsWhenMax1ExceedsMax)  UT 시나리오로 확인

* 3. 유사도 검사 알고리즘 확장성 고려위해, 알고리즘을 캡슐화하여, 동적으로 변경 가능하도록 전략 패턴을 사용
 
* coverage
* ![image](https://github.com/user-attachments/assets/b2025ae5-a1be-45e7-ba09-dbb0dad8c3f3)
