// Texture to blur
uniform sampler2D tex;

// Texture size
uniform float dx, dy;

void main()
{
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
    samples[9] =  0.08;
	vec2 uv = gl_TexCoord[0].st;
    vec2 dir = 0.5 - uv; 
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y); 
    dir = dir/dist; 

    vec4 color = texture2D(tex,uv); 
    vec4 sum = color;

    for (int i = 0; i < 10; i++)
        sum += texture2D( tex, uv + dir * samples[i] * .2);

    sum *= 1.0/11.0;
    float t = dist * 10.0;
    t = clamp( t ,0.0,1.0);

    gl_FragColor = mix( color, sum, t );
//*/
	//*
	//float dx = 1.0 / w;
	//float dy = 1.0 / h;
	vec2 st = gl_TexCoord[0].st;
	
	// Apply 3x3 gaussian filter
	
	//vec4 color	 = 4.0 * texture2D(tex, st);
	//color		+= 2.0 * texture2D(tex, st + vec2(+dx, 0.0));
	//color		+= 2.0 * texture2D(tex, st + vec2(-dx, 0.0));
	//color		+= 2.0 * texture2D(tex, st + vec2(0.0, +dy));
	//color		+= 2.0 * texture2D(tex, st + vec2(0.0, -dy));
	//color		+= texture2D(tex, st + vec2(+dx, +dy));
	//color		+= texture2D(tex, st + vec2(-dx, +dy));
	//color		+= texture2D(tex, st + vec2(-dx, -dy));
	//color		+= texture2D(tex, st + vec2(+dx, -dy));
	
	vec4 color = texture2D(tex, st + vec2(- 2.0*dx, 2.0*dy));
	color += 4.0 * texture2D(tex, st + vec2(- dx, 2.0*dy));
	color += 7.0 * texture2D(tex, st + vec2(0.0, 2.0*dy));
	color += 4.0 * texture2D(tex, st + vec2(dx, 2.0*dy));
	color += texture2D(tex, st + vec2(2.0*dx, 2.0*dy));
	
	color += 4.0 * texture2D(tex, st + vec2(- 2.0*dx, dy));
	color += 16.0 * texture2D(tex, st + vec2(- dx, dy));
	color += 26.0 * texture2D(tex, st + vec2(0.0, dy));
	color += 16.0 * texture2D(tex, st + vec2(dx, dy));
	color += 4.0 * texture2D(tex, st + vec2(2.0*dx, dy));
	
	color += 7.0 * texture2D(tex, st + vec2(- 2.0*dx, 0.0));
	color += 26.0 * texture2D(tex, st + vec2(- dx, 0.0));
	color += 41.0 * texture2D(tex, st + vec2(0.0, 0.0));
	color += 26.0 * texture2D(tex, st + vec2(dx, 0.0));
	color += 7.0 * texture2D(tex, st + vec2(2.0*dx, 0.0));
	
	color += 4.0 * texture2D(tex, st + vec2(- 2.0*dx, -dy));
	color += 16.0 * texture2D(tex, st + vec2(- dx, -dy));
	color += 26.0 * texture2D(tex, st + vec2(0.0, -dy));
	color += 16.0 * texture2D(tex, st + vec2(dx, -dy));
	color += 4.0 * texture2D(tex, st + vec2(2.0*dx, -dy));
	
	color += texture2D(tex, st + vec2(- 2.0*dx, -2.0*dy));
	color += 4.0 * texture2D(tex, st + vec2(- dx, -2.0*dy));
	color += 7.0 * texture2D(tex, st + vec2(0.0, -2.0*dy));
	color += 4.0 * texture2D(tex, st + vec2(dx, -2.0*dy));
	color += texture2D(tex, st + vec2(2.0*dx, -2.0*dy));
	
	//gl_FragColor = color / 16.0;
	gl_FragColor = color / 273.0;
	//gl_FragColor = color + vec4(0.0, 1.0, 0.0, 0.0);
	//gl_FragColor = clamp(texture2D(tex, st), 0.0, 1.0);
	//*/
}