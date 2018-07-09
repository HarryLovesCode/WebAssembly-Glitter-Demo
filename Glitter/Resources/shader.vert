#version 300 es

layout (location=0) in lowp vec3 bPos;
layout (location=1) in lowp vec3 bNormal;
layout (location=2) in lowp vec2 bTexCoords;

out lowp vec2 tTexCoords;
out lowp vec3 tWorldPos;
out lowp vec3 tNormal;

uniform mat4 mModel;
uniform mat4 mProjection;
uniform mat4 mView;

void main()
{
    tTexCoords = bTexCoords;  
    tNormal = mat3(mModel) * bNormal;
    tWorldPos = vec3(mModel * vec4(bPos, 1.0));

    gl_Position = mProjection * mView * mModel * vec4(bPos, 1.0);
}
