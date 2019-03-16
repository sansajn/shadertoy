// kaboom tinyraytracer tutorial shader

const float noise_amplitude = 1.0;
const float explosion_velocity = 0.7;

float hash(in float n)
{
	float x = sin(n) * 43758.5453;
	return x - floor(x);
}

float lerp(in float x, in float y, in float a)
{
	return x + (y - x) * max(0.0, min(1.0, a));
}

vec3 lerp(in vec3 x, in vec3 y, in float a)
{
	return x + (y - x) * max(0.0, min(1.0, a));
}

float noise(in vec3 x) {
    vec3 p = vec3(floor(x.x), floor(x.y), floor(x.z));
    vec3 f = vec3(x.x - p.x, x.y - p.y, x.z - p.z);
    f = f * (dot(f, vec3(3.0, 3.0, 3.0) - f*2.0));
    float n = dot(p, vec3(1.0, 57.0, 113.0));
    return lerp(lerp(
                     lerp(hash(n +  0.0), hash(n +  1.0), f.x),
                     lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
                lerp(
                    lerp(hash(n + 113.0), hash(n + 114.0), f.x),
                    lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);
}

vec3 rotate(in vec3 v) {
    return vec3(dot(vec3(0.0,  0.8,  0.6), v), dot(vec3(-0.80,  0.36, -0.48), v), dot(vec3(-0.60, -0.48,  0.64), v));
}

float fractal_brownian_motion(in vec3 x) {
    vec3 p = rotate(x);
    float f = 0.0;
    f += 0.5000*noise(p); p = p*2.32;
    f += 0.2500*noise(p); p = p*3.03;
    f += 0.1250*noise(p); p = p*2.61;
    f += 0.0625*noise(p);
    return f / 0.9375;
}

vec3 palette_fire(float d)
{
	const vec3 yellow = vec3(1.7, 1.3, 1.0);  // note that the color is "hot", i.e. has components >1
	const vec3 orange = vec3(1.0, 0.6, 0.0);
	const vec3 red = vec3(1.0, 0.0, 0.0);
	const vec3 darkgray = vec3(0.2, 0.2, 0.2);
	const vec3 gray = vec3(0.4, 0.4, 0.4);
	
	float x = max(0.0, min(1.0, d));
	if (x < 0.25)
		return lerp(gray, darkgray, x * 4.0);
	else if (x < 0.5)
		return lerp(darkgray, red, x * 4.0 - 1.0);
	else if (x < 0.75)
		return lerp(red, orange, x * 4.0 - 2.0);
	return lerp(orange, yellow, x * 4.0 - 3.0);
}

float signed_distance(in vec3 p, float radius)
{
	float displacement = -fractal_brownian_motion(p * 3.4) * noise_amplitude;
	return length(p) - (radius + displacement);
}

bool sphere_trace(in vec3 orig, in vec3 dir, in float radius, out vec3 pos)
{
	if (dot(orig, orig) - pow(dot(orig, dir), 2.0) > pow(radius, 2.0)) 
		return false;  // early discard
	    
	pos = orig;
	for (int i = 0; i < 128; ++i)
	{
		float d = signed_distance(pos, radius);
		if (d < 0.0) 
			return true;
		pos = pos + dir * max(d*0.1, 0.01);
	}
	return false;
}

vec3 distance_field_normal(in vec3 pos, in float radius)
{
	const float eps = 0.1;
	float d = signed_distance(pos, radius);
	return normalize(vec3(
		signed_distance(pos + vec3(eps, 0.0, 0.0), radius) - d,
		signed_distance(pos + vec3(0.0, eps, 0.0), radius) - d,
		signed_distance(pos + vec3(0.0, 0.0, eps), radius) - d
	));
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	const float pi = 3.14159265358979323846;
	const float fov = pi / 3.0;
	
	vec3 dir = normalize(vec3(
		(fragCoord.x + 0.5) - iResolution.x / 2.0,
		-((iResolution.y - fragCoord.y) + 0.5) + iResolution.y / 2.0,
		-iResolution.y / (2.0*tan(fov / 2.0))
	));
	
	float sphere_radius = 1.0 + explosion_velocity * iTime;
	vec3 hit, col;
	if (sphere_trace(vec3(0.0, 0.0, 3.0), dir, sphere_radius, hit))
	{
		float noise_level = (sphere_radius - length(hit)) / noise_amplitude;
		vec3 light_dir = normalize(vec3(10.0, 10.0, 10.0) - hit);
		float light_intensity = max(0.4, dot(light_dir, distance_field_normal(hit, sphere_radius)));
		col = palette_fire((-0.2 + noise_level) * 2.0) * light_intensity;
	}
	else
		col = vec3(0.2, 0.7, 0.8);  // background
		
	fragColor = vec4(col, 1.0);  // output to screen
}
