#pragma once
#include "ModeManager.h"

#include "RoadModel.h"
#include "KartModel.h"
#include "CharacterModel.h"
// Models
std::vector<Model*> karts;
//std::vector<Model*> roads;
std::vector<Model*> road1;
std::vector<Model*> road2;
std::vector<Model*> selectMaps;
std::vector<Model*> character;

// mode
ModeManager MM;

int window_width = 980;
int window_height = 780;
int location_x = 0;
int location_y = 0;

// �Ҹ� ũ��
float volume = 0.5;

GLvoid Reshape(int w, int h) {
    glViewport(location_x, location_y, w, h);
    width = w;
    height = h;
}
