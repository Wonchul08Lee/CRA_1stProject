#pragma once
#include "gmock/gmock.h"

int main() {

	//loadAndProcessKeyword("keyword_weekday_500.txt");

	::testing::InitGoogleMock();
	return RUN_ALL_TESTS();
}
