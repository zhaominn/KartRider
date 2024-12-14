#pragma once
#include "ModeManager.h"

#include "RoadModel.h"
#include "KartModel.h"
#include "CharacterModel.h"
#include "Ui.h"
// Models
std::vector<Model*> karts;
//std::vector<Model*> roads;
std::vector<Model*> road1;
std::vector<Model*> road1_barricate;

std::vector<Model*> road2;
std::vector<Model*> road2_barricate;

std::vector<Model*> selectMaps;

std::vector<Model*> character;

std::vector<Model*> countDown;
std::vector<Model*> pause;

std::vector<Model*> booster_ui;

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
