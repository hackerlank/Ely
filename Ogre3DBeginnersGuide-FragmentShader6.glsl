#version 330

//<Ogre3DBeginnersGuide/Comp3>
in vec2         ouv0;
out vec4        gl_FragColor;
uniform sampler2D texture0;

void main()
{
    vec4 temp_color = texture(texture0,ouv0);
	gl_FragColor = vec4 (1.0 - temp_color.r, 1.0 - temp_color.g, 1.0 - temp_color.b, 0);
}
//</Ogre3DBeginnersGuide/Comp3>

