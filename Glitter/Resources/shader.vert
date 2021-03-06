#version 300 es
precision highp float;
#define HAS_UV 1 
#define HAS_NORMALS 1

layout (location=0) in vec4 a_Position;
#ifdef HAS_NORMALS
layout (location=1) in vec4 a_Normal;
#endif
#ifdef HAS_TANGENTS
layout (location=3) in vec4 a_Tangent;
#endif
#ifdef HAS_UV
layout (location=2) in vec2 a_UV;
#endif

uniform mat4 u_MVPMatrix;
uniform mat4 u_ModelMatrix;
uniform mat4 u_NormalMatrix;

out vec3 v_Position;
out vec2 v_UV;

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
out mat3 v_TBN;
#else
out vec3 v_Normal;
#endif
#endif


void main()
{
    vec4 pos = u_ModelMatrix * a_Position;
    v_Position = vec3(pos.xyz) / pos.w;

#ifdef HAS_NORMALS
#ifdef HAS_TANGENTS
    vec3 normalW = normalize(vec3(u_NormalMatrix * vec4(a_Normal.xyz, 0.0)));
    vec3 tangentW = normalize(vec3(u_ModelMatrix * vec4(a_Tangent.xyz, 0.0)));
    vec3 bitangentW = cross(normalW, tangentW) * a_Tangent.w;
    v_TBN = mat3(tangentW, bitangentW, normalW);
#else // HAS_TANGENTS != 1
    v_Normal = normalize(vec3(u_ModelMatrix * vec4(a_Normal.xyz, 0.0)));
#endif
#endif

#ifdef HAS_UV
    v_UV = a_UV;
#else
    v_UV = vec2(0.,0.);
#endif

    gl_Position = u_MVPMatrix * a_Position; // needs w for proper perspective correction
}