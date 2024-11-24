#pragma once
#include <gl/glm/glm/glm.hpp>

//Ä«¸Þ¶ó
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 70);
glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);