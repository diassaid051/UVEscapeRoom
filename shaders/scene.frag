#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D tex;
uniform sampler2D hiddenTex;

uniform vec3  objColor;
uniform bool  useTex;
uniform bool  useHidden;   // this object carries UV-only writing
uniform bool  emissive;
uniform float shininess;

// light 1: desk lamp (point light, toggled with L)
uniform bool lampOn;
uniform vec3 lampPos;

// light 2: UV flashlight (spotlight attached to the camera, toggled with F)
uniform bool  flashOn;
uniform vec3  flashPos;
uniform vec3  flashDir;
uniform float innerCut;    // cos(inner angle)
uniform float outerCut;    // cos(outer angle)

vec3 blinnPhong(vec3 N, vec3 V, vec3 L, vec3 lightColor, vec3 albedo)
{
    float diff = max(dot(N, L), 0.0);
    vec3  H    = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), shininess);
    return lightColor * (diff * albedo + spec * 0.35);
}

void main()
{
    if (emissive) { FragColor = vec4(objColor, 1.0); return; }

    vec3 albedo = objColor;
    if (useTex) albedo *= texture(tex, TexCoords).rgb;

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec3 result = 0.05 * albedo;                       // ambient

    if (lampOn) {
        vec3  L   = lampPos - FragPos;
        float d   = length(L);
        float att = 1.0 / (1.0 + 0.22 * d + 0.20 * d * d);
        result += blinnPhong(N, V, normalize(L), vec3(1.0, 0.75, 0.45) * 1.6, albedo) * att;
    }

    if (flashOn) {
        vec3  L     = flashPos - FragPos;
        float d     = length(L);
        L           = normalize(L);
        float theta = dot(L, normalize(-flashDir));
        float cone  = smoothstep(outerCut, innerCut, theta);   // soft edge
        float att   = 1.0 / (1.0 + 0.09 * d + 0.032 * d * d);
        result += blinnPhong(N, V, L, vec3(0.55, 0.45, 1.0) * 1.3, albedo) * att * cone;

        // the secret: hidden writing is visible ONLY inside the cone
        if (useHidden && cone > 0.0) {
            float h = texture(hiddenTex, TexCoords).r;
            result += vec3(0.35, 1.0, 0.75) * h * cone * 2.2;
        }
    }

    result = pow(result, vec3(1.0 / 2.2));             // gamma
    FragColor = vec4(result, 1.0);
}
