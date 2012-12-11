
uniform sampler2D texture;
uniform int useTexture;
uniform vec4 diffuseColor;

void main() {	

	if (useTexture) {
		gl_FragColor = texture2D(texture, gl_TexCoord[0]);// - vec4(2.0, 2.0, 2.0, 1.0);
	} else {
		gl_FragColor = diffuseColor;
	}
	
}