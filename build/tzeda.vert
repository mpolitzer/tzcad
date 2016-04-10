#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

/* -------------------------------------------------------------------------- */
layout(location = 0) uniform mat4 u_mv;
layout(location = 1) uniform mat4 u_p;

/* in clip coornates */
layout(location = 0) in vec2 vtx;

/* ro - ray origin
 * rd - ray direction
 */
out vec3 v_ro;
out vec3 v_rd;

void main() {
	gl_Position = vec4(vtx, 0, 1);

	v_ro = (-vec4(u_mv[3].xyz,1)*u_mv).xyz;
	v_rd = ( vec4(vtx,-1,0)     *u_mv).xyz;
}
