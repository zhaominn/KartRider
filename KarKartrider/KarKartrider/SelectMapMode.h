#pragma once
#include <iostream>
#include "Mode.h"
#include "shaderMaker.h"
#include "root.h"
#include "KeyBoard.h"
#include "PlayMode.h"

class SelectMapMode : public Mode {
public:

	SelectMapMode() {}

	void init() override {
	}

	void keyboard(unsigned char key, int x, int y) override {
		switch (key) {
		case '\r': {
			PlayMode* playMode = new PlayMode();
			MM.SetMode(playMode);
			break;
		}
		default:
			break;
		}
	}

	void draw_model() override {

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "Error in glUseProgram: " << error << std::endl;
		}

		view = glm::mat4(1.0f);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		projection = glm::mat4(1.0f);
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		glEnable(GL_DEPTH_TEST);

		for (const auto& maps : selectMaps) { // ½ÇÁ¦ ¸ðµ¨ draw
			maps->draw(shaderProgramID, isKeyPressed_s);
		}

		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {
	}

	void finish() override {

	}

};
