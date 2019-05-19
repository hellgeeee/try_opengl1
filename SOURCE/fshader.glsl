//#ifdef GL_ES
//// Set default precision to medium
//precision mediump int;
//precision mediump float;
//#endif

uniform sampler2D texture;
varying vec2 v_texcoord;

uniform highp vec4 light_position;
uniform highp float light_power;
varying highp vec4 v_position;
varying highp vec3 v_normal;

//! [0]
void main()
{
    vec4 rezultColor = vec4(0.0, 1.0, 0.0, 0.0);
    vec4 eyePosition = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 diffMatColor = texture2D(texture, v_texcoord);
    vec3 eyeVec = normalize(v_position.xyz - light_position.xyz);
    vec3 lightVec = normalize(v_position.xyz - light_position.xyz);
    vec3 reflectLight = normalize(reflect(lightVec, v_normal));
    float len = length(v_position.xyz - light_position.xyz);
    float specularFactor = 60.0;
    float ambientFactor = 0.1;

    vec4 diffColor = diffMatColor * light_power * max(0.0, dot(v_normal, -lightVec)) / (1.0 + 0.25 * pow(len, 2.0) );
    rezultColor += diffColor;

    vec4 ambientColor = ambientFactor * diffMatColor;
    rezultColor += ambientColor;

    vec4 specularColor = vec4(1.0, 1.0, 1.0, 1.0) * light_power * pow( max(0.0, dot(reflectLight, -eyeVec)), 2.0 );
    rezultColor += specularColor;
    // Set fragment color from texture
    //gl_FragColor = texture2D(texture, v_texcoord);
}
//! [0]

