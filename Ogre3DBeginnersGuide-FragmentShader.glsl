#version 330

//<MyMaterial13>
//out vec4        gl_FragColor;

//void main()
//{
//	gl_FragColor = vec4(0,0,1,0);
//}
//</MyMaterial13>

//<MyMaterial14>
in vec2         ouv0;
out vec4        gl_FragColor;
uniform sampler2D texture0;

void main()
{
    gl_FragColor = texture(texture0,ouv0);
}
//</MyMaterial14>

//<MyMaterial15>
//in vec4         ocolour;
//out vec4        gl_FragColor;

//void main()
//{
//    gl_FragColor = ocolour;
//}
//</MyMaterial15>

