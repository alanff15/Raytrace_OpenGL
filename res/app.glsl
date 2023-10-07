#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
out vec2 FragCoord;

void main() {
    gl_Position = position;
    FragCoord = position.xy;
}

#-----------------------------------------------------------#

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 FragCoord;

uniform float u_FOV;
uniform vec2 u_ScreenResolution;
uniform vec3 u_DirectorAngles;

#define MAX_STEPS 128
#define MAX_DIST 500.0f
#define EPSILON 1.0e-2

// camera
float FOV = u_FOV;              // abertura
vec3 ang = u_DirectorAngles;    //angulos diretores
float offsetx = 0.0;            // deslocamento da câmera em X
float offsety = 0.0;            // deslocamento da câmera em Y
float zoom = 1.0;               // aproximacao

///////////////////////////////////////////////////////////////////////////////

mat3 rotationMatrixX(float angle) {
    return mat3(1, 0, 0, 0, cos(angle), -sin(angle), 0, sin(angle), cos(angle));
}

mat3 rotationMatrixY(float angle) {
    return mat3(cos(angle), 0, sin(angle), 0, 1, 0, -sin(angle), 0, cos(angle));
}

mat3 rotationMatrixZ(float angle) {
    return mat3(cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0, 0, 0, 1);
}

///////////////////////////////////////////////////////////////////////////////
// Obj operations

vec2 fOpUnionID(vec2 res1, vec2 res2) {
    return (res1.x < res2.x) ? res1 : res2;
}

vec2 fOpDifferenceID(vec2 res1, vec2 res2) {
    return (res1.x > -res2.x) ? res1 : vec2(-res2.x, res2.y);
}

///////////////////////////////////////////////////////////////////////////////
// SDF operations

float opUnion(float d1, float d2) {
    return min(d1, d2);
}
float opSubtraction(float d1, float d2) {
    return max(-d1, d2);
}
float opIntersection(float d1, float d2) {
    return max(d1, d2);
}
float opSmoothUnion(float d1, float d2, float k) {
    float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return mix(d2, d1, h) - k * h * (1.0 - h);
}

float opSmoothSubtraction(float d1, float d2, float k) {
    float h = clamp(0.5 - 0.5 * (d2 + d1) / k, 0.0, 1.0);
    return mix(d2, -d1, h) + k * h * (1.0 - h);
}

float opSmoothIntersection(float d1, float d2, float k) {
    float h = clamp(0.5 - 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return mix(d2, d1, h) + k * h * (1.0 - h);
}

///////////////////////////////////////////////////////////////////////////////
// SDFs

float sdSphere(vec3 p, float s) {
    return length(p) - s;
}

float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

///////////////////////////////////////////////////////////////////////////////

vec2 map(vec3 p) {
    vec2 res = vec2(sdBox(p, vec3(20, 20, EPSILON)), 0);                          // chao
    res = fOpUnionID(res, vec2(sdSphere(p + vec3(0, 0, -2), 2.0), 1));                // obj1
    res = fOpUnionID(res, vec2(sdBox(p + vec3(4, -3, -4), vec3(1.5, 2, 1)), 2));  // obj2
    return res;
}

vec2 rayMarch(vec3 ro, vec3 rd) {
    vec2 hit, object = vec2(0);
    for(int i = 0; i < MAX_STEPS; i++) {
        vec3 p = ro + object.x * rd;
        hit = map(p);
        object.x += hit.x;
        object.y = hit.y;
        if(abs(hit.x) < EPSILON || object.x > MAX_DIST)
            break;
    }
    return object;
}

vec3 getMaterial(vec3 p, float id) {
    if(id == 0.0)
        return vec3(0.2 + 0.4 * mod(floor(p.x) + floor(p.y), 2.0));
    if(id == 1.0)
        return vec3(0.9, 0.0, 0.0);
    if(id == 2.0)
        return vec3(0.7, 0.8, 0.9);
    return vec3(0.0, 0.0, 0.0);
}

vec3 getNormal(vec3 p) {
    vec3 lx = vec3(EPSILON, 0.0, 0.0);
    vec3 ly = vec3(0.0, EPSILON, 0.0);
    vec3 lz = vec3(0.0, 0.0, EPSILON);
    vec3 n = vec3(map(p).x) - vec3(map(p - lx).x, map(p - ly).x, map(p - lz).x);
    return normalize(n);
}

vec3 getLight(vec3 p, vec3 rd, vec3 color) {
    vec3 lightPos = vec3(10.0, 10.0, 50.0);
    vec3 lightColor = vec3(0.5);

    vec3 L = normalize(lightPos - p);
    vec3 N = getNormal(p);
    vec3 V = -rd;
    vec3 R = reflect(-L, N);

    float ambient = 0.20;
    float diffuse = 0.80 * clamp(dot(L, N), 0.0, 1.0);
    float specular = 0.90 * pow(clamp(dot(R, V), 0.0, 1.0), 20.0);
    float fresnel = 0.10 * pow(1.0 + dot(rd, N), 3.0);

    // sombra
    float d = rayMarch(p + N * 2.0 * EPSILON, L).x;
    if(d < length(lightPos - p))
        return (ambient + fresnel) * color;

    return (ambient + fresnel + diffuse) * mix(color, lightColor, ambient / 5.0) + specular * lightColor;
}

vec3 renderPixel(vec2 pixPos, vec2 imgSize) {
    vec3 ro, rd;
    // distância em pixels do ponto atual para o centro da câmera
    float ratio = imgSize.x / imgSize.y;
    float distx = pixPos.x * imgSize.x * ratio / 2.0;
    float disty = -pixPos.y * imgSize.y / 2.0;
    // maior distância do volume em voxels
    int max_dist = 25;
    // posição e orientação da câmera em pixels antes da rotação (sobre o eixo y apontando para a origem)
    vec3 camera_pos = vec3(0.0, -float(max_dist) / 2.0, 0.0);
    vec3 camera_x = vec3(1.0, 0.0, 0.0);
    vec3 camera_y = vec3(0.0, 0.0, -1.0);
    vec3 camera_z = vec3(0.0, 1.0, 0.0);
    // rotação da câmera
    mat3 matRot = rotationMatrixZ(ang.z) * rotationMatrixX(ang.x) * rotationMatrixY(ang.y);
    camera_pos = matRot * camera_pos;
    camera_x = matRot * camera_x;
    camera_y = matRot * camera_y;
    camera_z = matRot * camera_z;

    if(true) {
        // camera perspectiva
        ro = normalize(camera_pos) * 1.25 * length(camera_pos) * FOV / zoom + offsetx * camera_x - offsety * camera_y;
        rd = mat3(camera_x, camera_y, camera_z) * normalize(vec3(vec2(distx, disty) / (imgSize), FOV));
    } else {
        // camera paralela
        ro = camera_pos + (distx + offsetx) / zoom * camera_x + (disty - offsety) / zoom * camera_y;
        rd = camera_z;
    }

    vec2 object = rayMarch(ro, rd);

    vec3 rgb = vec3(0.0);
    vec3 background = vec3(0.5, 0.8, 0.9);
    if(object.x < MAX_DIST) {
        vec3 p = ro + object.x * rd;
        vec3 material = getMaterial(p, object.y);
        rgb = getLight(p, rd, material);
        // fog
        rgb = mix(rgb, background, 1.0 - exp(-0.00001 * object.x * object.x));
    } else {
        rgb = background - max(0.9 * rd.z, 0.0);
    }
    return clamp(rgb, vec3(0.0), vec3(1.0));
}

vec2 SSAA[4] = vec2[4](//
vec2(-1.0, 3.0) / (4.0 * u_ScreenResolution),       // subpixel 0
vec2(3.0, 1.0 / 3.0) / (4.0 * u_ScreenResolution),  // subpixel 1
vec2(-3.0, -1.0) / (4.0 * u_ScreenResolution),      // subpixel 2
vec2(1.0, -3.0) / (4.0 * u_ScreenResolution)        // subpixel 3
);

void main() {
    color = vec4(0.0);
    for(int i = 0; i < 4; i++) {
        color += vec4(renderPixel(SSAA[i] + FragCoord, u_ScreenResolution), 1.0);
    }
    color /= 4.0;
}