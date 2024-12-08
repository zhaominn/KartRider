#include <iostream>
#include <vector> 

#include "shaderMaker.h"
#include "DefaultModel.h"
#include "KeyBoard.h"
#include "Camera.h"
#include "Light.h"
#include "Debug.h"
#include "LoadProgress.h"
#include "root.h"
#include "LogoMode.h"

using namespace std;

void InitBuffer();
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);


int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int screenWidth = glutGet(GLUT_SCREEN_WIDTH);  // 모니터의 가로 해상도
	int screenHeight = glutGet(GLUT_SCREEN_HEIGHT); // 모니터의 세로 해상도
	glutInitWindowPosition(location_x, location_y);
	glutInitWindowSize(window_width, window_height); // 창 크기를 화면 크기로 설정
	glutCreateWindow("KarKartrider");
	//glutFullScreen(); // 전체 화면으로 전환

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else
		cout << "GLEW Initialized\n";

	make_shaderProgram();

	initPhysics(); // Bullet 초기화 함수 호출

	LogoMode* logoMode = new LogoMode();
	MM.SetMode(logoMode); //초기 모드 세팅

	// 디버깅 출력
	/*debug_model(models.back());
	debug_materials(models.back()->materials);*/


	InitBuffer();

	// 초기 프레임 강제 렌더링
	drawScene();  // 디스플레이 콜백 함수 직접 호출

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(keySpecial);
	glutMainLoop();

	return 0;
}

GLvoid drawScene() {


	MM.draw_model();
	MM.draw_bb();

	glutSwapBuffers();

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error: " << err << endl;
	}
}

// 버퍼 초기화 함수
void InitBuffer() {
	//-----------------------------------------------------------------------------------------------------------
	for (const auto& model : karts) { // 모든 모델 initBuffer
		model->initBuffer();
	}

	for (const auto& model : road1) { // 모든 모델 initBuffer
		model->initBuffer();
	}

	for (const auto& model : road2) { // 모든 모델 initBuffer
		model->initBuffer();
	}

	for (const auto& model : selectMaps) { // 모든 모델 initBuffer
		model->initBuffer();
	}
}