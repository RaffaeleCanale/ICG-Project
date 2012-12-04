uniform int useTexture;
uniform sampler2D texture;
uniform vec3 diffuseColor;
//uniform float specularExp;
//uniform vec3 indirectlightcolor;
uniform vec3 lightcolor;


varying vec3 normal, lightDir; //, indirectLightDir;
		
void main()
{	
	vec3 color = diffuseColor;

	if (useTexture) {
		color = diffuseColor * texture2D(texture, gl_TexCoord[0].xy);
		//color = diffuseColor * texture2D(texture, gl_TexCoord[0].xy).xyz;
	}
	
	vec3 L = lightDir;
	vec3 N = normalize(normal);
	
	vec3 shadedColor = clamp(color * dot(L, N) * lightcolor, 0.0, 1.0);
	//vec3 shadedColor = clamp(lightcolor * color * abs(dot(lightDir, normal)) , 0.0, 1.0);
	//indcolor = clamp(indirectlightcolor * indcolor * dot(indirectLightDir, normal), 0.0, 1.0);

	//also add a small ambient term
	vec3 ambientColor = color * .2;
	
	gl_FragColor =  vec4(clamp( shadedColor + ambientColor, 0.0, 1.0), 1.0);
}