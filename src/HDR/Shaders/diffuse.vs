uniform mat4 modelworld;
uniform mat4 worldcamera;
uniform mat4 projection;

uniform mat3 worldcameraNormal;
uniform mat3 modelworldNormal;

uniform vec3 lightposition; // in camera coordinates

varying vec3 normal, lightDir;

void main() {	  
	vec3 vertex = vec3( worldcamera * modelworld * gl_Vertex );
	
	lightDir =  normalize(lightposition - vertex);
	normal = normalize( worldcameraNormal * modelworldNormal * gl_Normal );

	
	gl_TexCoord[0]  = gl_MultiTexCoord0;
	gl_Position = projection * vec4( vertex, 1.0 );	
}
