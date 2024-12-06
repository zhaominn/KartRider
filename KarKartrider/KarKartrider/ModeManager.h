#pragma once
#include "Mode.h"

class ModeManager {
private:
	Mode* mode;

public:
	ModeManager() : mode(nullptr) {}

	void SetMode(Mode* m) {
		if (mode)
			mode->finish();
		mode = m;
		mode->init();
	}

	void draw_model() {
		if (mode)
			mode->draw_model();
	}

	void draw_bb() {
		if (mode)
			mode->draw_bb();
	}

	void finish(){
		if (mode)
			mode->finish();
	}

	void keyboard(unsigned char key, int x, int y) {
		if (mode)
			mode->keyboard(key, x, y);
	}

	void keySpecial(int key, int x, int y) {
		if (mode)
			mode->keySpecial(key, x, y);
	}
};