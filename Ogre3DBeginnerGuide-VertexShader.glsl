#version 330

//<MyMaterial13>
//in vec4 position;
//uniform mat4 worldViewMatrix;

//void main()
//{
//	gl_Position = worldViewMatrix * position;
//}
//</MyMaterial13>

//<MyMaterial14>
//in vec4        vertex;
//in vec4        uv0;
//out vec2       ouv0;
//uniform mat4   worldViewMatrix;

//void main ()
//{
//    gl_Position = worldViewMatrix * vertex;
//    ouv0 = vec2(uv0);
//}
//</MyMaterial14>

//<MyMaterial15>
//in vec4        vertex;
//in vec4        colour;
//out vec4       ocolour;
//uniform mat4   worldViewMatrix;

//void main ()
//{
//    gl_Position = worldViewMatrix * vertex;
//    ocolour = colour;
//}
//</MyMaterial15>

//<MyMaterial17>
in vec4        vertex;
in vec4        uv0;
out vec2       ouv0;
uniform float  pulseTime;
uniform mat4   worldViewMatrix;

void main ()
{
    gl_Position = worldViewMatrix * vertex;
    gl_Position.x *= (2+sin(pulseTime));
    ouv0 = vec2(uv0);
}
//</MyMaterial17>

