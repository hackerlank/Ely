#version 330

//<Ogre3DBeginnersGuide/Comp2>
in vec4        vertex;
in vec4        uv0;
out vec2       ouv0;
uniform mat4   worldViewMatrix;

void main ()
{
    gl_Position = worldViewMatrix * vertex;
    ouv0 = vec2(uv0);
}
//</Ogre3DBeginnersGuide/Comp2>
