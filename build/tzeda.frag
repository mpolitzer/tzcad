#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

struct material_t {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct light_t {
	vec3 position;
	vec3 attenuation;
};

struct field_t {
	float d;
	int   m;
};

material_t materials[] = material_t[](
	material_t( // copper
		vec3(0.21, 0.13, 0.05),
		vec3(0.71, 0.43, 0.18),
		vec3(0.39, 0.27, 0.17), 25.6),
	material_t( // obsidian
		vec3(0.05, 0.05, 0.06),
		vec3(0.18, 0.17, 0.22),
		vec3(0.33, 0.32, 0.34), 38.4)
);

light_t lights[] = light_t[](
	light_t(vec3( 0, 5, 0), vec3(0.5, 0.1, 0.05)),
	light_t(vec3(-3, 2, 2), vec3(0.5, 0.1, 0.05)),
	light_t(vec3( 3,-4, 2), vec3(0.5, 0.1, 0.05))
);

/* -------------------------------------------------------------------------- */
layout(location = 0) uniform mat4 u_mv;
layout(location = 1) uniform mat4 u_p;

/* ro - ray origin
 * rd - ray direction <- requires normalization */
in  vec3 v_ro;
in  vec3 v_rd;
out vec4 color;

float vmax(vec2 v) {
	return max(v.x, v.y);
}

float vmax(vec3 v) {
	return max(max(v.x, v.y), v.z);
}

float vmax(vec4 v) {
	return max(max(v.x, v.y), max(v.z, v.w));
}

float box(vec3 p, vec3 b) {
	vec3 d = abs(p) - b;
	return length(max(d, vec3(0))) + vmax(min(d, vec3(0)));
}

float scene(vec3 p)
{
	p.x = mod(p.x, 5) - 2.5;
	p.z = mod(p.z, 5) - 2.5;
	float sphere0 = length(p)-1;

	return sphere0;
}

/* ro - ray origin
 * rd - ray direction
 * n  - max iterations
 *
 * TODO: study over relaxation for search. */
float trace(vec3 ro, vec3 rd, int n) {
	float d=0, ds=0.0;
	for(int i=0; i<n; ++i, ds += 1.02*d) {
		d = scene(ro + ds * rd);
		if (d < 0.01) {
			break;
		}
	}
	return ds;
}

/* sample around p to calculate the normal */
vec3 normal(vec3 p) {
	float e = 1e-5;
	vec3 n = vec3(
			scene(p+vec3(e,0,0))-scene(p-vec3(e,0,0)),
			scene(p+vec3(0,e,0))-scene(p-vec3(0,e,0)),
			scene(p+vec3(0,0,e))-scene(p-vec3(0,0,e)));
	return normalize(n);
}

bool apply_light(vec3 s, vec3 rd, vec3 n, int li, int mi, inout vec4 color)
{
	vec3  ldir  = normalize(lights[li].position - s);
	float llen  = length   (lights[li].position - s);
	float ldotn = dot(ldir,n);
	float att   = 1/dot(vec3(1, llen, llen*llen), lights[li].attenuation);

	vec3 ambient = att*materials[mi].ambient;
	color+= att*vec4(ambient,0);

	if (ldotn <= 0.0)
		return false;

	vec3 diffuse = att*materials[mi].diffuse*ldotn;
	color+= vec4(diffuse,0);

	// specular
	vec3 r = reflect(ldir, n);
	float specular = att*pow(max(0.0, dot(rd, r)), materials[0].shininess);
	color += vec4(specular*materials[0].specular, 0);

	return true;
}

void main()
{
	vec3 ro = v_ro;
	vec3 rd = normalize(v_rd);

	float d = trace(ro, rd, 128);
	vec3  s = ro + d*rd;

	color = vec4(0,0,0,1);
	for (int li=0; li<lights.length(); ++li) {
		apply_light(s, rd, normal(s), 0, 0, color);
	}
}
