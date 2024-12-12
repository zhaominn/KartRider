#pragma once
#include "ModeManager.h"

#include "RoadModel.h"
#include "KartModel.h"
// Models
std::vector<Model*> karts;
//std::vector<Model*> roads;
std::vector<Model*> road1;
std::vector<Model*> road2;
std::vector<Model*> selectMaps;

// mode
ModeManager MM;

int window_width = 980;
int window_height = 780;
int location_x = 0;
int location_y = 0;

// 소리 크기
float volume = 0.5;

GLvoid Reshape(int w, int h) {
    glViewport(location_x, location_y, w, h);
    width = w;
    height = h;
}
