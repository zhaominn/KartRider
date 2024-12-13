#pragma once
#include <iostream>

class Mode {
public:
	static Mode* currentInstance;  // 현재 활성화된 Mode 인스턴스를 추적하는 정적 포인터

	virtual void init() = 0; // 모드 시작시 셋팅
	virtual void keyboard(unsigned char key, int x, int y) = 0;
	virtual void specialKey(int key, int x, int y) = 0;
	virtual void specialKeyUp(int key, int x, int y) = 0;
	virtual void mouseClick(int button, int state, int x, int y) = 0;
	virtual void draw_model() = 0;
	virtual void draw_bb() = 0;
	virtual void finish() = 0;

	virtual ~Mode() {}  // 가상 소멸자
};