#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) uniform mat4 u_mv;
layout(location = 1) uniform mat4 u_p;
layout(location = 2) uniform mat4 u_imvp;

/* in clip coornates */
layout(location = 0) in vec2 vtx;

out vec3 v_ro;
out vec3 v_rd;

void main() {
	gl_Position = vec4(vtx, 0, 1);

	//mat4 u_imv = inverse(u_mv);
	//v_ro = (u_imv*vec4(0,0, 0,1)).xyz;
	//v_rd = (u_imv*vec4(vtx,-1,0)).xyz;

	v_ro = (-vec4(u_mv[3].xyz,1)*u_mv).xyz;
	v_rd = ( vec4(vtx,-1,0)     *u_mv).xyz;
}
