attribute vec4 VertexPos;
uniform float Time;
varying float bb_timer;
//attribute vec2 TexturePos; 
//varying vec2 TextCoord;
//uniform vec2 OffsetXY;
//uniform vec4 FontColor; 
//float x;
//float y;

void main() 
{
	//x = (OffsetXY.x + VertexPos.x) / 400.0 - 1.0;
	//y = 1.0 - (OffsetXY.y + VertexPos.y) / 240.0;
	gl_Position = VertexPos; 
	//vec4(x, y, VertexPos.z, VertexPos.w);
	//TextCoord = TexturePos;
}
