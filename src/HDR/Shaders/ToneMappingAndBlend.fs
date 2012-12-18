uniform sampler2D texture, bloom, scatter;

uniform float textureRatio;
uniform float bloomRatio;
uniform float scatterRatio;


uniform float exposure;
uniform float brightThreshold;


		
void main() {	

	vec2 position = gl_TexCoord[0].st;	
	
	vec4 color = textureRatio * texture2D(texture, position);
	color += bloomRatio * texture2D(bloom, position);
	color += scatterRatio * texture2D(scatter, position);
	
	//color = texture2D(bloom, position);	
	
	// Define tone mapping here
	float YD = exposure * (exposure/brightThreshold + 1.0) / (exposure + 1.0);
	color *= YD;
	
	//color = texture2D(texture, position);	
	
	gl_FragColor =  clamp( color, 0.0, 1.0);	
	
	
	/*
	color = texture2D(bloom, position) + texture2D(texture, position)*.00000001;
	//color = texture2D(texture, position);
	
	YD = exposure * (exposure/brightThreshold + 1.0) / (exposure + 1.0);
	color *= YD;
	
	gl_FragColor =  clamp( color, 0.0, 1.0);//*/
	
}