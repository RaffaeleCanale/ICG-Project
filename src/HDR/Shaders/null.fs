// Texture to blur
uniform sampler2D texture;

void main()
{	
	vec4 color = texture2D(texture, gl_TexCoord[0].st);

	gl_FragColor = color;// + vec4(0.0, 0.0, 1.0, 0.0);
	
}