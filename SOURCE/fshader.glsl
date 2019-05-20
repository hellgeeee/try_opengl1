//#ifdef GL_ES
//// Set default precision to medium
//precision mediump int;
//precision mediump float;
//#endif

uniform sampler2D texture;
varying vec2 var_texcoord;

uniform highp vec4 light_position;
uniform highp float light_power;
varying highp vec4 position;
varying highp vec3 normal;

//! [0]
void main()
{
    vec4 rezultColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 eyePosition = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 diffMatColor = texture2D(texture, var_texcoord);
    vec3 eyeVec = normalize(position.xyz - eyePosition.xyz);
    vec3 lightVec = normalize(position.xyz - light_position.xyz);
    vec3 reflectLight = normalize(reflect(lightVec, normal));
    float len = length(position.xyz - eyePosition.xyz);
    float specularFactor = 10.0;
    float ambientFactor = 0.4;

    vec4 diffColor = diffMatColor * light_power * dot(normal, -lightVec) / (1.0 + 0.25 * pow(len, 2.0) );
    rezultColor += diffColor;

    vec4 ambientColor = ambientFactor * diffMatColor;
    rezultColor += ambientColor;

//    vec4 specularColor = vec4(1.0, 1.0, 1.0, 1.0) * light_power * pow( max(0.0, dot(reflectLight, -eyeVec)), 2.0 );
//    rezultColor += specularColor;

    gl_FragColor = rezultColor;
}
//! [0]

