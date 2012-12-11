
uniform sampler2D texture;

uniform vec2 orientation;
uniform int blurAmount;

uniform float blurStrength;


float gaussianCoef(float x, float deviation) {
	return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));	
}

void main () {
	
	float halfBlur = float(blurAmount) * 0.5;
	vec4 color = vec4(0.0);
	vec2 st = gl_TexCoord[0];
	
	// Gaussian deviation
	float deviation = halfBlur * 0.35;
	deviation *= deviation;
	float strength = 1.0 - blurStrength;
	
	
	
	for (int i = 0; i < blurAmount; ++i) {
		float offset = float(i) - halfBlur;
		color += texture2D(texture, st + orientation * offset) * gaussianCoef(offset * strength, deviation);
	}
	
	
	// Apply color
	gl_FragColor = color;
	gl_FragColor.w = 1; //*/
}