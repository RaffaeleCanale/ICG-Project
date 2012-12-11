uniform int useTexture;
uniform sampler2D texture;

uniform vec4 diffuseColor;
//uniform float specularExp;
uniform vec4 lightcolor;

varying vec3 normal, lightDir;
		
void main() {	
	vec4 color;

	if (useTexture) {
		color = texture2D(texture, gl_TexCoord[0]);
	} else {
		color = diffuseColor;
	}
	
	vec3 L = lightDir;
	vec3 N = normal;
	
	vec4 shadedColor = color * dot(L, N) * lightcolor;
	shadedColor = max(shadedColor, vec4(0.0, 0.0, 0.0, 1.0));

	//also add a small ambient term
	vec4 ambientColor = color * .2;
	
	color = shadedColor + ambientColor;	
	
	gl_FragColor =  color;
}