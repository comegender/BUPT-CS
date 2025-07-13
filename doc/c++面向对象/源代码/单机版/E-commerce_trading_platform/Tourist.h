#pragma once
#include"Behavior.h"

class Tourist :public UserBehavior {
public:
	virtual int getUserType(string userName);

	void updateSheet();
};
