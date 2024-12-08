#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "root.h"
#include "Camera.h"

//키
std::unordered_map<char, bool> keyState;

void keyDown_s(const char& key) {
    keyState[key] = !keyState[key];
}

void keyUp_s(const char& key) {
    keyState[key] = false;
}

bool isKeyPressed_s(const char& key) {
    return keyState[key];
}

void keyUp(unsigned char key, int x, int y) {
    //keyUp_s(key);
    glutPostRedisplay();
}

void keyDown(unsigned char key, int x, int y) {

    MM.keyboard(key, x, y);

    keyDown_s(key);

    switch (key)
    {
    case '[':
        volume -= 0.05;
        break;
    case ']':
        volume += 0.05;
        break;
    case 'q':
        std::cout << " 프로그램 종료 " << std::endl;
        exit(0);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void specialKey(int key, int x, int y) {
    MM.specialKey(key, x, y);

    glutPostRedisplay();
}

void specialKeyUp(int key, int x, int y) {
    MM.specialKeyUp(key, x, y);

    glutPostRedisplay();
}