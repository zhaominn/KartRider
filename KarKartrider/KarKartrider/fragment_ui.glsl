#version 330 core

in vec3 Color;         // 버텍스 셰이더에서 전달받은 색상
in vec2 TexCoords;     // 텍스처 좌표 (필요 없는 경우 삭제 가능)

out vec4 FragColor;    // 최종 출력 색상

uniform sampler2D texture1; // 텍스처 (옵션: 필요 없는 경우 삭제 가능)
uniform bool isRed;          // UI 모드 플래그
uniform bool isTimer;          // UI 모드 플래그
uniform bool isTextrue;

void main() {
     
      if(isRed){
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
      }
      if(isTimer){
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
      }
      if(isTextrue){
            // 일반 렌더링 모드 (필요 시 텍스처 사용)
            FragColor = texture(texture1, TexCoords);
      }
}
