#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

attribute vec4 position;
attribute vec2 texcoord;

varying vec2 var_texcoord;

//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = projection_matrix * view_matrix * model_matrix * position;

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    var_texcoord = texcoord;
}
//! [0]
