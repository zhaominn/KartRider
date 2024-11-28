#version 330 core

// 입력 데이터
in vec3 FragPos;   // 월드 공간에서의 프래그먼트 위치 (Vertex Shader에서 전달)
in vec3 Normal;    // 정점의 법선 벡터 (Vertex Shader에서 전달)
in vec2 TexCoords; // 텍스처 좌표 (Vertex Shader에서 전달)

// 출력 데이터
out vec4 FragColor; // 최종 출력 색상

// 카메라와 광원 관련 Uniform 변수
uniform vec3 viewPos;      // 카메라(뷰어) 위치
uniform vec3 lightPos;     // 광원의 위치
uniform vec3 lightColor;   // 광원의 색상

// 텍스처 및 텍스처 유무 플래그
uniform sampler2D map_Ka;  // 환경광 텍스처
uniform sampler2D map_Kd;  // 난반사광 텍스처
uniform sampler2D map_Ks;  // 반사광 텍스처
uniform bool hasKaTexture; // 환경광 텍스처 유무 플래그
uniform bool hasKdTexture; // 난반사광 텍스처 유무 플래그
uniform bool hasKsTexture; // 반사광 텍스처 유무 플래그

// 기본 재질 속성
uniform vec3 Ka;           // 기본 환경광 계수
uniform vec3 Kd;           // 기본 난반사광 계수
uniform vec3 Ks;           // 기본 반사광 계수
uniform float Ns;          // 반짝임 강도 (Shininess)

void main() {
    // 1. 법선 벡터 정규화
    vec3 norm = normalize(Normal);

    // 2. 광원 방향 계산
    vec3 lightDir = normalize(lightPos - FragPos);

    // 3. 카메라 방향 계산
    vec3 viewDir = normalize(viewPos - FragPos);

    // 4. **환경광 (Ambient) 계산**
    vec3 ambientColor = hasKaTexture ? texture(map_Ka, TexCoords).rgb : Ka; // 텍스처 또는 기본값
    vec3 ambient = ambientColor * lightColor;

    // 5. **난반사광 (Diffuse) 계산**
    float diff = max(dot(norm, lightDir), 0.0); // Lambert's cosine law
    vec3 diffuseColor = hasKdTexture ? texture(map_Kd, TexCoords).rgb : Kd; // 텍스처 또는 기본값
    vec3 diffuse = diffuseColor * diff * lightColor;

    // 6. **반사광 (Specular) 계산**
    vec3 reflectDir = reflect(-lightDir, norm); // 반사 방향 계산
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns); // Phong reflection model
    vec3 specularColor = hasKsTexture ? texture(map_Ks, TexCoords).rgb : Ks; // 텍스처 또는 기본값
    vec3 specular = specularColor * spec * lightColor;

    // 7. **최종 색상 계산**
    vec3 result = ambient + diffuse + specular;

    // 8. 최종 출력
    FragColor = vec4(result, 1.0);
}


 //FragColor = vec4(result, 1.0);
 //FragColor = vec4(Normal * 0.5 + 0.5, 1.0); // 법선을 (0, 1) 범위로 변환
 //FragColor = vec4(TexCoords, 0.0, 1.0); // UV 좌표를 색상으로 출력
 //FragColor = vec4(diff * lightColor, 1.0); // 조명계산확인
 //FragColor = vec4(texture(map_Kd, TexCoords).rgb, 1.0); //텍스처 적용된 이미지만 바로나오게
 //FragColor = vec4(ambient, 1.0); //조명 계산이 정상인지 확인 (환경광 색상으로 나오면됨)
 //FragColor = vec4(diffuse, 1.0); //난반사광(diffuse)만 출력하여 조명 계산이 정상인지 확인
 //FragColor = vec4(specular, 1.0); //반사광(specular)만 출력하여 조명 계산이 정상인지 확인
