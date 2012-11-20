uniform int useTexture;
uniform sampler2D texture;
uniform vec3 diffuseColor;
uniform float specularExp;
//uniform vec3 indirectlightcolor;
uniform vec3 lightcolor;

varying vec3 normal, lightDir; //, indirectLightDir;
		
void main()
{	
	vec3 color = diffuseColor;

	if (useTexture) {
		color = diffuseColor * texture2D(texture, gl_TexCoord[0].xy);
		//indcolor = diffuseColor * texture2D(texture, gl_TexCoord[0].xy);
	}
	
	vec3 shadedColor = clamp(lightcolor * color * dot(lightDir, normal) , 0.0, 1.0);
	//indcolor = clamp(indirectlightcolor * indcolor * dot(indirectLightDir, normal), 0.0, 1.0);

	//also add a small ambient term
	gl_FragColor =  vec4(shadedColor, 1.0) + vec4(color,1.0)*0.1;		
}