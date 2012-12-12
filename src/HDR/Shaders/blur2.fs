
uniform sampler2D texture;
uniform vec2 orientation;

uniform int blurAmount;

uniform float samples[300];

void main () {

	
	vec2 st = gl_TexCoord[0].st;

    vec4 color = texture2D(texture,st); 
    vec4 sum = color;

    for (int i = 0; i < blurAmount; i++) {
        sum += texture2D( texture, st + orientation * samples[i]);
	}
	
    sum /= blurAmount;
	gl_FragColor = sum;
}