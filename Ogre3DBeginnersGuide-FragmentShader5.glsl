#version 330

//<Ogre3DBeginnersGuide/Comp2>
in vec2         ouv0;
out vec4        gl_FragColor;
uniform sampler2D texture0;

void main()
{
    vec4 temp_color = texture(texture0,ouv0);
	float greyvalue = temp_color.r * 0.3 + temp_color.g * 0.59 + temp_color.b * 0.11;
	gl_FragColor = vec4 (greyvalue, greyvalue, greyvalue, 0);
}
//</Ogre3DBeginnersGuide/Comp2>

