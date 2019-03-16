// tinyraytracer tutorial

struct light 
{
	vec3 position;
	float intensity;
};

struct material
{
	vec2 albedo;
	vec3 diffuse_color;
	float specular_exponent;
};

struct sphere 
{
	vec3 center;
	float radius;
	material properties;
};

const material ivory = material(vec2(0.6,  0.3), vec3(0.4, 0.4, 0.3), 50.0);
const material red_rubber = material(vec2(0.9,  0.1), vec3(0.3, 0.1, 0.1), 10.0);

const light light1 = light(vec3(-20, 20, 20), 1.5);
const light light2 = light(vec3(30, 50, -25), 1.8);
const light light3 = light(vec3(30, 20, 30), 1.7);

sphere s1 = sphere(vec3(-3, 0, -16), 2.0, ivory);
sphere s2 = sphere(vec3(-1.0, -1.5, -12), 2.0, red_rubber);
sphere s3 = sphere(vec3(1.5, -0.5, -18), 3.0, red_rubber);
sphere s4 = sphere(vec3(7, 5, -18), 4.0, ivory);


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

bool scene_intersect(in vec3 orig, in vec3 dir, out material properties, 
	out vec3 hit, out vec3 N)
{
	float sphere_dist = 100000000.0;  // FLOAT_MAX
	
	float dist;
	if (sphere_ray_intersect(s1, orig, dir, dist) && dist < sphere_dist) {
		sphere_dist = dist;
		properties = s1.properties;
		hit = orig + dir * dist;
		N = normalize(hit - s1.center);
	}
	if (sphere_ray_intersect(s2, orig, dir, dist) && dist < sphere_dist) {
		sphere_dist = dist;
		properties = s2.properties;
		hit = orig + dir * dist;
		N = normalize(hit - s2.center);
	}
	if (sphere_ray_intersect(s3, orig, dir, dist) && dist < sphere_dist) {
		sphere_dist = dist;
		properties = s3.properties;
		hit = orig + dir * dist;
		N = normalize(hit - s3.center);
	}
	if (sphere_ray_intersect(s4, orig, dir, dist) && dist < sphere_dist) {
		sphere_dist = dist;
		properties = s4.properties;
		hit = orig + dir * dist;
		N = normalize(hit - s4.center);
	}
	
	return sphere_dist < 1000.0;
}

vec3 cast_ray(in vec3 orig, in vec3 dir)
{
	vec3 point, N;
	material properties;
	if (!scene_intersect(orig, dir, properties, point, N))
		return vec3(0.2, 0.7, 0.8);  // background color

	float diffuse_light_intensity = 0.0;
	float specular_light_intensity = 0.0;
	// light1
	vec3 light_dir = normalize(light1.position - point);
	vec3 shadow_orig = dot(light_dir, N) < 0.0 ? point - N*1e-3 : point + N*1e-3;
	vec3 shadow_pt, shadow_N;
	material shadow_properties;
	if (!scene_intersect(shadow_orig, light_dir, shadow_properties, shadow_pt, shadow_N))
	{
		diffuse_light_intensity += light1.intensity * max(0.0, dot(light_dir,N));
		specular_light_intensity += pow(max(0.0, dot(-reflect(-light_dir, N), dir)), properties.specular_exponent)*light1.intensity;
	}
	// light2
	light_dir = normalize(light2.position - point);
	shadow_orig = dot(light_dir, N) < 0.0 ? point - N*1e-3 : point + N*1e-3;
	if (!scene_intersect(shadow_orig, light_dir, shadow_properties, shadow_pt, shadow_N))
	{
		diffuse_light_intensity += light2.intensity * max(0.0, dot(light_dir,N));
		specular_light_intensity += pow(max(0.0, dot(-reflect(-light_dir, N), dir)), properties.specular_exponent)*light2.intensity;
	}
	// light3
	light_dir = normalize(light3.position - point);
	shadow_orig = dot(light_dir, N) < 0.0 ? point - N*1e-3 : point + N*1e-3;
	if (!scene_intersect(shadow_orig, light_dir, shadow_properties, shadow_pt, shadow_N))
	{
		diffuse_light_intensity += light3.intensity * max(0.0, dot(light_dir,N));
		specular_light_intensity += pow(max(0.0, dot(-reflect(-light_dir, N), dir)), properties.specular_exponent)*light3.intensity;
	}
	
	return properties.diffuse_color * diffuse_light_intensity * properties.albedo[0] + vec3(1.0, 1.0, 1.0) * specular_light_intensity * properties.albedo[1];
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	const float pi = 3.14159265358979323846;
	int fov = int(pi/2.0);
	float fov_2 = float(fov)/2.0;
	
	vec3 dir = normalize(vec3(
		(2.0 * (fragCoord.x + 0.5) / iResolution.x - 1.0)*tan(fov_2)*iResolution.x/iResolution.y,
		-(2.0 * ((iResolution.y - fragCoord.y) + 0.5) / iResolution.y - 1.0)*tan(fov_2),
		-1.0
	));
	vec3 col = cast_ray(vec3(0,0,0), dir);

	fragColor = vec4(col,1.0);  // output to screen
}
