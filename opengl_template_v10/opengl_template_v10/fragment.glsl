#version 330 core

in vec3 FragPos;   // 프래그먼트 위치 (vertex shader에서 전달)
in vec3 Normal;    // 정점의 법선 벡터 (vertex shader에서 전달)
in vec2 TexCoords; // 텍스처 좌표 (vertex shader에서 전달)

out vec4 FragColor; // 최종 출력 색상

// 카메라와 광원 관련 Uniform 변수
uniform vec3 viewPos;      // 카메라(뷰어) 위치
uniform vec3 lightPos;     // 광원의 위치
uniform vec3 lightColor;   // 광원의 색상

// 재질 속성
uniform vec3 Ka;           // 환경광 계수 (Ambient)
uniform vec3 Ks;           // 반사광 계수 (Specular)
uniform float Ns;          // 반짝임 강도 (Shininess)

// 텍스처 관련 Uniform 변수
uniform sampler2D texture1; // 텍스처 샘플러
uniform bool hasTexture;    // 텍스처 유무 플래그

// 렌더링 상태
uniform bool isRigidBody;   // true면 rigid body 상태로 렌더링

void main() {
    // 강체 상태 (Rigid Body)라면 빨간색으로 출력
    if (isRigidBody) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }

    // 월드 공간에서의 법선 벡터 정규화
    vec3 norm = normalize(Normal);

    // 광원 방향 계산
    vec3 lightDir = normalize(lightPos - FragPos);

    // 1. **환경광 (Ambient)**: 고정 값으로 설정
    vec3 ambient = Ka * lightColor;

    // 2. **난반사광 (Diffuse)**: Lambert's cosine law
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = hasTexture ? texture(texture1, TexCoords).rgb : vec3(1.0, 1.0, 1.0); // 텍스처 유무 확인
    vec3 diffuse = diffuseColor * diff * lightColor;

    // 3. **반사광 (Specular)**: Phong reflection model
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), clamp(Ns, 1.0, 128.0)); // Ns 클램핑
    vec3 specular = Ks * spec * lightColor;

    // 최종 색상 계산 (환경광 + 난반사광 + 반사광)
    vec3 result = ambient + diffuse + specular;

    // 최종 출력
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(Normal * 0.5 + 0.5, 1.0); // 법선을 (0, 1) 범위로 변환
}