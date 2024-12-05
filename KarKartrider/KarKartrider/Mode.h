#pragma once
#include <iostream>

class Mode {
public:
	virtual void init() = 0; // 모드 시작시 셋팅
	virtual void keyboard() = 0;
	virtual void draw_model() = 0;
	virtual void draw_bb() = 0;
	virtual void finish() = 0;

	virtual ~Mode() {}  // 가상 소멸자
};