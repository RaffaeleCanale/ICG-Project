uniform sampler2D texture, bloom;

uniform float exposure;
uniform float brightThreshold;

		
void main() {	

	vec2 position = gl_TexCoord[0].st;
	vec4 color = texture2D(texture, position);
	vec4 bloomColor = texture2D(bloom, position);
	
	color += bloomColor * 0.6; //Define ratio here	
	
	//color = texture2D(bloom, position);	
	
	// Define tone mapping here
	float YD = exposure * (exposure/brightThreshold + 1.0) / (exposure + 1.0);
	color *= YD;
	
	//color = texture2D(texture, position);	
	
	gl_FragColor =  clamp( color, 0.0, 1.0);	
	
	
	/*
	color = texture2D(bloom, position) + texture2D(texture, position)*.00000001;
	color = texture2D(texture, position);
	
	YD = exposure * (exposure/brightThreshold + 1.0) / (exposure + 1.0);
	color *= YD;
	
	gl_FragColor =  clamp( color, 0.0, 1.0);//*/
	
}