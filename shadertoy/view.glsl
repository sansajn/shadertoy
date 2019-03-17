// shadertoy hello shader

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	// Normalized pixel coordinates (from 0 to 1)
	vec2 uv = fragCoord/iResolution.xy;

	fragColor = texture2D(iChannel0, uv);
}
