// https://www.shadertoy.com/view/3dlXzS

// lol simple rainbow because everyone starts simple

vec3 hue2rgb(in float hue)
{
    // Converting pure HUE to RGB (see http://www.chilliant.com/rgb2hsv.html)
  
    vec3 out_rgb = vec3(      abs(hue * 6. - 3.) - 1.  ,
    				     2. - abs(hue * 6. - 2.)       ,
        			     2. - abs(hue * 6. - 4.)       );    
    return out_rgb;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;

    
    // Time varying pixel color
    
    vec3 col = vec3( hue2rgb( fract( uv.x - (iTime * 1.)  + sin( uv.y + iTime))  ).r,
                     hue2rgb( fract( uv.x - (iTime * 1.1) + sin( uv.y + iTime))  ).g,
                     hue2rgb( fract( uv.x - (iTime * 1.2) + sin( uv.y + iTime))  ).b                     
                   ); 
    
    
    // Output to screen
    fragColor = vec4(col,1.0);
}
