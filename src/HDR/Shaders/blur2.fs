
uniform sampler2D texture;
uniform vec2 orientation;

uniform int blurAmount;

uniform float samples[300];

void main () {
	/*
	float samples[10];
    samples[0] = -0.08;
    samples[1] = -0.05;
    samples[2] = -0.03;
    samples[3] = -0.02;
    samples[4] = -0.01;
    samples[5] =  0.01;
    samples[6] =  0.02;
    samples[7] =  0.03;
    samples[8] =  0.05;
    samples[9] =  0.08;//*/
	
	
	vec2 st = gl_TexCoord[0].st;
    /*
	vec2 dir = vec2(1.0, 0.0);
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y); 
    dir = dir/dist; //*/

    vec4 color = texture2D(texture,st); 
    vec4 sum = color;

    for (int i = 0; i < blurAmount; i++) {
        sum += texture2D( texture, st + orientation * samples[i]);
	}
	
    sum /= blurAmount;
	/*
    float t = dist * 10.0;
    t = clamp( t ,0.0,1.0);//*/

    //gl_FragColor = mix( color, sum, t );
	gl_FragColor = sum;
}