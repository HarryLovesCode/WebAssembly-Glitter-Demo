#version 300 es

out lowp vec4 oColor;

in lowp vec2 tTexCoords;
in lowp vec3 tWorldPos;
in lowp vec3 tNormal;

uniform lowp vec3 vCamPos;
uniform lowp vec3 vAlbedo;

uniform lowp vec3 vLightPos[4];
uniform lowp vec3 vLightCol[4];

uniform sampler2D iAlbedo1;
uniform sampler2D iNormal1;
uniform sampler2D iMetallic1;
uniform sampler2D iRoughness1;
uniform sampler2D iAO;

const lowp float PI = 3.14159265359;

lowp vec3 normalFromMap()
{
    lowp vec3 tangentNormal = texture(iNormal1, tTexCoords).xyz * 2.0 - 1.0;

    lowp vec3 Q1  = dFdx(tWorldPos);
    lowp vec3 Q2  = dFdy(tWorldPos);
    lowp vec2 st1 = dFdx(tTexCoords);
    lowp vec2 st2 = dFdy(tTexCoords);

    lowp vec3 N   = normalize(tNormal);
    lowp vec3 T  = normalize(Q1 * st2.t - Q2 * st1.t);
    lowp vec3 B  = -normalize(cross(N, T));
    lowp mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

lowp vec3 fresnelSchlick(lowp float cosTheta, lowp vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

lowp float distributionGGX(lowp vec3 N, lowp vec3 H, lowp float roughness)
{
    lowp float a      = roughness*roughness;
    lowp float a2     = a*a;
    lowp float NdotH  = max(dot(N, H), 0.0);
    lowp float NdotH2 = NdotH*NdotH;
	
    lowp float num   = a2;
    lowp float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

lowp float geometrySchlickGGX(lowp float NdotV, lowp float roughness)
{
    lowp float r = (roughness + 1.0);
    lowp float k = (r*r) / 8.0;

    lowp float num   = NdotV;
    lowp float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

lowp float geometrySmith(lowp vec3 N, lowp vec3 V, lowp vec3 L, lowp float roughness)
{
    lowp float NdotV = max(dot(N, V), 0.0);
    lowp float NdotL = max(dot(N, L), 0.0);
    lowp float ggx2  = geometrySchlickGGX(NdotV, roughness);
    lowp float ggx1  = geometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

void main()
{   
    lowp vec3 vAlbedo = pow(texture(iAlbedo1, tTexCoords).rgb, vec3(2.2));
    lowp float fMetallic = texture(iMetallic1, tTexCoords).b;
    lowp float fRoughness = texture(iMetallic1, tTexCoords).g;

    fMetallic = clamp(fMetallic, 0.0, 1.0);
    fRoughness = clamp(fRoughness, 0.04, 1.0);

    // lowp float fMetallic = clamp(texture(iMetallic1, tTexCoords).g, 0.04, 1.0);
    // lowp float fRoughness = clamp(texture(iMetallic1, tTexCoords).r, 0.04, 1.0);
    lowp float fAO = texture(iAO, tTexCoords).r;

    lowp vec3 norm = normalFromMap();
    lowp vec3 dir = normalize(vCamPos - tWorldPos);

    lowp vec3 lo = vec3(0.0);
    lowp vec3 f0 = vec3(0.04);
    f0 = mix(f0, vAlbedo, fMetallic);

    for (int i = 0; i < 4; ++i) {
        lowp vec3 to = vLightPos[i] - tWorldPos;

        lowp vec3 l = normalize(to);
        lowp vec3 h = normalize(dir + l);

        lowp float d = length(to);
        lowp float a = 1.0 / (d * d);

        lowp vec3 radiance = vLightCol[i] * a;
        lowp vec3 f = fresnelSchlick(max(dot(h, dir), 0.0), f0);

        lowp float ndf = distributionGGX(norm, h, fRoughness);
        lowp float g = geometrySmith(norm, dir, l, fRoughness);
        
        // Cook-Torrance BRDF
        lowp vec3 numerator = ndf * g * f;
        lowp float denom = 4.0 * max(dot(norm, dir), 0.0) * max(dot(norm, l), 0.0) + 0.001;
        lowp vec3 specular = numerator / denom;

        lowp vec3 kS = f;
        lowp vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - fMetallic;

        lowp float nDotL = max(dot(norm, l), 0.0);
        lo += (kD * vAlbedo / PI + specular) * radiance * nDotL;
    }

    lowp vec3 ambient = vec3(0.03) * vAlbedo; // * fAO;
    lowp vec3 color = ambient + lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    oColor = vec4(color, 1.0);
}
