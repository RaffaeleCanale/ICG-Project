 uniform float exposure;
 uniform float decay;
 uniform float density;
 uniform float weight;

 uniform vec2 lightPos;

 uniform sampler2D texture;

 uniform int samples;


 void main() {	
 /*
	vec2 st = gl_TexCoord[0].st;

    vec4 color = texture2D(texture,st); 
    vec4 sum = color;
	
	
	vec2 orientation =  lightPos - st;
	orientation /= 40.0;
	//orientation = vec2(1.0, 0.0);

    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += texture2D( texture, st + orientation * i);
	}
	
    sum /= NUM_SAMPLES;
	gl_FragColor = sum;
	//gl_FragColor = color;//*/
/*
	vec2 st = gl_TexCoord[0].st;

    vec4 color = texture2D(texture,st); 
    vec4 sum = color;
	
	vec2 lightVector = lightPos - st;
	lightVector /= 40.0;
	//lightVector = vec2(1.0, 1.0);
	
    for (int i = 0; i < NUM_SAMPLES; i++) {
		vec2 position = st + lightVector*i;
        sum += texture2D( texture, position);
	}
	
    sum /= NUM_SAMPLES;
	gl_FragColor = sum;//*/

 //*
 	vec2 deltaTextCoord = vec2( gl_TexCoord[0].st - lightPos.xy );

 	vec2 textCoo = gl_TexCoord[0].st;

 	deltaTextCoord *= 1.0 /  float(samples) * density;

 	float illuminationDecay = 1.0;

	

	

 	for(int i=0; i < samples ; i++)	{

    			textCoo -= deltaTextCoord;

    			vec4 sample = texture2D(texture, textCoo );

    			
    			sample *= illuminationDecay * weight;

    			
    			gl_FragColor += sample;

    			
    			illuminationDecay *= decay;

 	}
	//gl_FragColor = texture2D(texture, gl_TexCoord[0].st);// - lightPos.xy);
//*/
}