uniform sampler2D texture; //the depth texture that need to be post processed with the sobel operator

uniform float dx; //use this uniform to move 1 pixel in x
uniform float dy; //use this uniform to move 1 pixel in y
		
void main()
{	
	//
	// calculate sobel edges
	//               -1  0  1
	// x derivative: -2  0  2
	//               -1  0  1
	// 
	//                1  2  1
	// y derivative:  0  0  0
	//               -1 -2 -1
	//
	vec2 coord = gl_TexCoord[0].xy;
	vec4 color_x = - texture2D(texture, coord + vec2(-dx, dy)) + texture2D(texture, coord + vec2(dx, dy))
					- 2*texture2D(texture, coord + vec2(-dx, 0)) + 2*texture2D(texture, coord + vec2(dx, 0))
					- texture2D(texture, coord + vec2(-dx, -dy)) + texture2D(texture, coord + vec2(dx, -dy));
	vec4 color_y = texture2D(texture, coord + vec2(-dx, dy)) + 2*texture2D(texture, coord + vec2(0, dy)) + texture2D(texture, coord + vec2(dx, dy))
					- texture2D(texture, coord + vec2(-dx, -dy)) - 2*texture2D(texture, coord + vec2(0, -dy)) - texture2D(texture, coord + vec2(dx, -dy));
					
	vec4 M = sqrt(color_x*color_x + color_y*color_y);
	
	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0) - 10.0 * M;
	
	//gl_FragColor = texture2D(texture, gl_TexCoord[0].xy);
}