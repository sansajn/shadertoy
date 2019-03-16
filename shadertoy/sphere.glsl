// tinyraytracer tutorial

struct sphere 
{
	vec3 center;
	float radius;
};

sphere s1 = sphere(vec3(-3, 0, -16), 2.0);

bool sphere_ray_intersect(in sphere s, in vec3 orig, in vec3 dir, out float t0)
{
	vec3 L = s.center - orig;
	float tca = dot(L,dir);
	float d2 = dot(L,L) - tca*tca;
	if (d2 > s.radius*s.radius) 
		return false;
	
	float thc = sqrt(s.radius*s.radius - d2);
	t0 = tca - thc;
	float t1 = tca + thc;
	if (t0 < 0.0)
		t0 = t1;
	if (t0 < 0.0)
		return false;

	return true;
}

vec3 cast_ray(in vec3 orig, in vec3 dir, in sphere s)
{
	float sphere_dist = 100000000.0;  // FLOAT_MAX
	if (!sphere_ray_intersect(s, orig, dir, sphere_dist))
		return vec3(0.2, 0.7, 0.8);  // background color
	else
		return vec3(0.4, 0.4, 0.3);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	const float pi = 3.14159265358979323846;
	int fov = int(pi/2.0);
	float fov_2 = float(fov)/2.0;
	
	vec3 dir = normalize(vec3(
		(2.0 * (fragCoord.x + 0.5) / iResolution.x - 1.0)*tan(fov_2)*iResolution.x/iResolution.y,
		-(2.0 * (fragCoord.y + 0.5) / iResolution.y - 1.0)*tan(fov_2),
		-1.0
	));
	vec3 col = cast_ray(vec3(0,0,0), dir, s1);

	fragColor = vec4(col,1.0);  // output to screen
}
