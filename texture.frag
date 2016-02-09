varying highp vec2 qt_TexCoord;

uniform sampler2D qt_Texture;
uniform lowp vec4 color;

void main()
{
    gl_FragColor = texture2D(qt_Texture, qt_TexCoord) * color;
}
