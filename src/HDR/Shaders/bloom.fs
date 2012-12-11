uniform sampler2D texture;
		
uniform float treshold;

void main() {	
	vec4 color = texture2D(texture, gl_TexCoord[0]);
	
	float luminance = dot( color, vec4(1.0, 1.0, 1.0, 0.0)); // Define a constant here
	
	if (luminance > treshold) {
		gl_FragColor = clamp(color, 0.0, 1.0);
	} else {
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	//gl_FragColor =  color;
}