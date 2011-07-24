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

//<Ogre3DBeginnersGuide/Comp5>
//in vec2         ouv0;
//out vec4        gl_FragColor;
//uniform sampler2D texture0;

//void main()
//{
//	float num= 50;
//	float stepsize = 1.0/ num;

//	vec2 fragment = vec2(stepsize * floor(ouv0.x * num),stepsize * floor(ouv0.y * num));
	
//    gl_FragColor = texture(texture0,fragment);
//}
//</Ogre3DBeginnersGuide/Comp5>

//<Ogre3DBeginnersGuide/Comp6>
//in vec2         ouv0;
//out vec4        gl_FragColor;
//uniform sampler2D texture0;
//uniform float numpixels;

//void main()
//{
//	float num = numpixels;
//	float stepsize = 1.0/ num;

//	vec2 fragment = vec2(stepsize * floor(ouv0.x * num),stepsize * floor(ouv0.y * num));	
//  gl_FragColor = texture(texture0,fragment);
//}
//</Ogre3DBeginnersGuide/Comp6>

//<Ogre3DBeginnersGuide/Comp7>
//in vec2         ouv0;
//out vec4        gl_FragColor;
//uniform sampler2D texture0;
//uniform vec4 factors;

//void main()
//{
//	gl_FragColor = texture(texture0,ouv0);
//	gl_FragColor *= factors;
//}
//</Ogre3DBeginnersGuide/Comp7>

