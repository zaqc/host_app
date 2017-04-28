precision mediump float;
//-------------------------------------------------------------------------------

uniform sampler2D Texture;
varying float bb_timer;
//-------------------------------------------------------------------------------

float DrawLine(in vec2 cor, in vec2 pa, in vec2 pb)
{
    float x = cor.x;
    float y = cor.y;
    vec2 pt = vec2(x, y);
	
    float a = abs(distance(pa, pt));
	float b = abs(distance(pb, pt));
	float c = abs(distance(pa, pb));
		if ( a >= c || b >=  c ) 
            return 0.0;

	float p = (a + b + c) * 0.5;
	float h = 2.0 / c * sqrt( p * ( p - a) * ( p - b) * ( p - c));
	return mix(1.0, 0.0, smoothstep(0.5, 2.0, h));    
}
//-------------------------------------------------------------------------------

float DrawCircle(in vec2 cor, in vec2 c, float d) 
{
	return clamp((1.0 - abs(d - distance(cor, c)) / 8.0), 0.0, 1.0);
}
//-------------------------------------------------------------------------------

float FillRect(in vec2 cor, in vec2 pa, in vec2 pb)
{
	float l = clamp((cor.x - pa.x), 0.0, 1.0);
	float t = clamp((cor.y - pa.y), 0.0, 1.0);
	float r = clamp((pb.x - cor.x), 0.0, 1.0);
	float b = clamp((pb.y - cor.y), 0.0, 1.0);
	return clamp(l * r * t * b, 0.0, 1.0);
	//return cor.x > pa.x && cor.x < pb.x && cor.y > pa.y && cor.y < pb.y ? 1.0 : 0.0;
}
//-------------------------------------------------------------------------------

float DrawBevel_LT(in vec2 cor, in vec2 pa, in vec2 pb)
{
	float l = clamp((1.0 - abs(cor.x - pa.x) / 4.0), 0.0, 1.0);
	float t = clamp((1.0 - abs(cor.y - pa.y) / 4.0), 0.0, 1.0);
	return clamp(max(l, t) * FillRect(cor, pa, pb), 0.0, 1.0);
}
//-------------------------------------------------------------------------------

float DrawBevel_RB(in vec2 cor, in vec2 pa, in vec2 pb)
{
	float r = clamp((1.0 - abs(pb.x - cor.x) / 4.0), 0.0, 1.0);
	float b = clamp((1.0 - abs(pb.y - cor.y) / 4.0), 0.0, 1.0);
	return clamp(max(r, b) * FillRect(cor, pa, pb), 0.0, 1.0);
}
//-------------------------------------------------------------------------------

vec4 CopyTexture(in vec2 cor, in vec2 in_a, in vec2 in_b, in vec2 out_a, in vec2 out_b)
{
	float x = clamp((in_a.x + (in_b.x - in_a.x) / (out_b.x - out_a.x) * (cor.x - out_a.x)) / 96.0 / 4.0, 0.0, 1.0);
	float y = clamp((in_a.y + (in_b.y - in_a.y) / (out_b.y - out_a.y) * (cor.y - out_a.y)) / 96.0, 0.0, 1.0);
	return texture2D(Texture, vec2(x, y));
}
//-------------------------------------------------------------------------------

float DrawSprite(in vec2 cor, in vec2 pos)
{
	float y = cor.y - pos.y;
	float nx = 0.0;
	nx = (y < 1.0) ? 0.0 :
	 	(y < 2.0) ? 6.0 :
		(y < 3.0) ? 30.0 :
		(y < 4.0) ? 124.0 :
		(y < 5.0) ? 508.0 :
		(y < 6.0) ? 2040.0 :
		(y < 7.0) ? 8184.0 :
		(y < 8.0) ? 32752.0 :
		(y < 9.0) ? 131056.0 :
		(y < 10.0) ? 524256.0 :
		(y < 11.0) ? 2097120.0 :
		(y < 12.0) ? 8388544.0 :
		(y < 13.0) ? 4194240.0 :
		(y < 14.0) ? 2097024.0 :
		(y < 15.0) ? 1048448.0 :
		(y < 16.0) ? 524032.0 :
		(y < 17.0) ? 1048320.0 :
		(y < 18.0) ? 2096640.0 :
		(y < 19.0) ? 4193792.0 :
		(y < 20.0) ? 8354816.0 :
		(y < 21.0) ? 16661504.0 :
		(y < 22.0) ? 33298432.0 :
		(y < 23.0) ? 66586624.0 :
		(y < 24.0) ? 133169152.0 :
		(y < 25.0) ? 266338304.0 :
		(y < 26.0) ? 532676608.0 :
		(y < 27.0) ? 1065353216.0 :
		(y < 28.0) ? 2130706432.0 :
		(y < 29.0) ? 1040187392.0 :
		(y < 30.0) ? 469762048.0 :
		(y < 31.0) ? 134217728.0 :
		(y < 32.0) ? 0.0 : 0.0;
	
	float res = 0.0;
	
	float x = cor.x - pos.x;
		
	if(x >= 0.0 && x < 32.0)
	{
		res = clamp(mod(nx, pow(2.0, x + 1.0)) / pow(2.0, x), 0.0, 1.0);
	}
	return res;
}
//-------------------------------------------------------------------------------

uniform sampler2D Zm;
uniform sampler2D DData;

vec4 Stretch(in vec2 cor)
{
	vec4 p1 = texture2D(Zm, vec2(0.0, 0.0));
	vec4 p2 = texture2D(Zm, vec2(1.0, 0.0));
	float ch1 = p1.r * 256.0 + p1.g;
	float ch2 = p1.b * 256.0 + p1.a;
	float ch3 = p2.r * 256.0 + p2.g;
	float ch4 = p2.b * 256.0 + p2.a;
	float v1 = 0.0;
	float v2 = 0.0;
	float v3 = 0.0;
	float v4 = 0.0;
	float px = 0.0;
	while((ch1 < 4096.0 || ch2 < 4096.0 || ch3 < 4096.0 || ch4 < 4096.0) && px < 128.0){
		if(ch1 < 4096.0)
			v1 = max(v1, texture2D(DData, vec2(ch1, 0.0)).a);
		if(ch2 < 4096.0)
			v2 = max(v2, texture2D(DData, vec2(ch2, 0.0)).a);
		if(ch3 < 4096.0)
			v3 = max(v3, texture2D(DData, vec2(ch3, 0.0)).a);
		if(ch4 < 4096.0)
			v4 = max(v4, texture2D(DData, vec2(ch4, 0.0)).a);
		
		p1 = texture2D(Zm, vec2(px, 0.0));
		px = px + 1.0;
		p2 = texture2D(Zm, vec2(px, 0.0));
		px = px + 1.0;
		ch1 = p1.r * 256.0 + p1.g;
		ch2 = p1.b * 256.0 + p1.a;
		ch3 = p2.r * 256.0 + p2.g;
		ch4 = p2.b * 256.0 + p2.a;
	}
	return vec4(v1, v2, v3, v4);
}

//-------------------------------------------------------------------------------

uniform float GlobalTime;
uniform float LeftLevelMask;

#define COLOR_WHITE	vec4(1.0, 1.0, 1.0, 1.0)
#define COLOR_RED	vec4(1.0, 0.0, 0.0, 1.0)

void main() {
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
	vec2 cor = gl_FragCoord.xy;
	cor.y = 480.0 - cor.y;

	//float a = 0.5;	
	//mat2 rm = mat2(sin(a), cos(a), cos(a), -sin(a));
	//cor = cor * rm;
	
	col = mix(col, vec4(0.0, 0.0, 0.15, 1.0), FillRect(cor, vec2(0.0, 0.0), vec2(800.0, 480.0)));

	col = mix(col, vec4(0.0, 1.0, 1.0, 0.25), FillRect(cor, vec2(10.0, 10.0), vec2(106.0, 106.0)));
	col = mix(col, vec4(0.0, 0.0, 0.0, 1.0), DrawBevel_RB(cor, vec2(10.0, 10.0), vec2(106.0, 106.0)));
	col = mix(col, vec4(1.0, 1.0, 1.0, 1.0), DrawBevel_LT(cor, vec2(10.0, 10.0), vec2(106.0, 106.0)));
	/*
	vec2 cc = vec2(400.0 + sin(GlobalTime * 4.0) * 100.0, 240.0 + cos(GlobalTime * 5.0) * 100.0);
	col = mix(col, vec4(1.0, 0.0, 1.0, 1.0), DrawCircle(cor, cc, abs(sin(GlobalTime * .25)) * 100.0));
	col = mix(col, vec4(1.0, 1.0, 0.0, 1.0), DrawCircle(cor, cc, abs(sin(GlobalTime * .5 + 0.5)) * 100.0));
	col = mix(col, vec4(0.0, 1.0, 1.0, 1.0), DrawCircle(cor, cc, abs(sin(GlobalTime * .75 + 1.0)) * 100.0));
	col = mix(col, vec4(0.0, 1.0, 0.0, 1.0), DrawCircle(cor, cc, abs(sin(GlobalTime)) * 100.0));
	*/
	//col = mix(col, vec4(1.0, 0.0, 1.0, 0.5), DrawLine(cor, vec2(0.0, 0.0), vec2(800.0, 480.0)));
	if(cor.x >= 0.0 && cor.x <= 120.0) {
		float flash_color = abs(sin(GlobalTime * 3.0));
		/*
		col = mix(col, COLOR_RED, DrawCircle(cor, vec2(26.0, 26.0), 8.0 + flash_color * 16.0));
	
		//col = mix(col, COLOR_WHITE, DrawCircle(cor, vec2(26.0 + 64.0, 26.0 + 64.0), 8.0 + 1.0 * 16.0));
		// 70
		col = mix(col, COLOR_RED, DrawCircle(cor, vec2(26.0 + 64.0, 26.0 + 64.0), 8.0 + flash_color * 16.0));

		// 58
		col = mix(col, COLOR_RED, DrawCircle(cor, vec2(26.0 + 64.0, 26.0 + 96.0 + 20.0), 8.0 + flash_color * 16.0));
	
		// 45 70
		col = mix(col, COLOR_RED, DrawCircle(cor, vec2(36.0, 258.0), 8.0 + flash_color * 16.0));
		col = mix(col, COLOR_RED, DrawCircle(cor, vec2(80.0, 258.0), 8.0 + flash_color * 16.0));

		// 0
		col = mix(col, COLOR_RED, DrawCircle(cor, vec2(58.0, 374.0), 8.0 + flash_color * 16.0));
		*/
		if(cor.y >= 0.0 && cor.y < 120.0) {	// 70
			if(clamp(mod(LeftLevelMask, 2.0), 0.0, 1.0) != 0.0)
				col = mix(col, COLOR_RED, DrawCircle(cor, vec2(26.0, 26.0), 8.0 + flash_color * 16.0));
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(90.0, 26.0), 8.0 + flash_color * 16.0));
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(26.0, 90.0), 8.0 + flash_color * 16.0));
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(90.0, 90.0), 8.0 + flash_color * 16.0));
			
			vec4 tx = CopyTexture(cor, vec2(96.0, 0.0), vec2(192.0, 96.0), vec2(10.0, 10.0), vec2(106.0, 106.0));
			col = mix(col, tx, FillRect(cor, vec2(10.0, 10.0), vec2(106.0, 106.0)) * tx.a);
		}
		
		if(cor.y >= 110.0 && cor.y < 240.0)	{ // 58
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(26.0, 142.0), 8.0 + flash_color * 16.0));
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(90.0, 142.0), 8.0 + flash_color * 16.0));
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(26.0, 208.0), 8.0 + flash_color * 16.0));
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(90.0, 208.0), 8.0 + flash_color * 16.0));
			
			vec4 tx = CopyTexture(cor, vec2(192.0, 0.0), vec2(288.0, 96.0), vec2(10.0, 126.0), vec2(106.0, 222.0));
			col = mix(col, tx, FillRect(cor, vec2(10.0, 126.0), vec2(106.0, 222.0)) * tx.a);
		}
	
		//col = mix(col, vec4(1.0, 1.0, 1.0, 1.0), DrawCircle(cor, vec2(26.0, 316.0), 20.0));
	
		if(cor.y >= 230.0 && cor.y < 360.0)	{
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(36.0, 258.0), 8.0 + flash_color * 16.0));
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(80.0, 258.0), 8.0 + flash_color * 16.0));
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(36.0, 322.0), 8.0 + flash_color * 16.0));
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(80.0, 322.0), 8.0 + flash_color * 16.0));
			
			vec4 tx = CopyTexture(cor, vec2(0.0, 0.0), vec2(96.0, 96.0), vec2(10.0, 242.0), vec2(106.0, 338.0));
			col = mix(col, tx, FillRect(cor, vec2(10.0, 242.0), vec2(106.0, 338.0)) * tx.a);
		}
			
		if(cor.y >= 340.0 && cor.y < 470.0)	{
			col = mix(col, COLOR_RED, DrawCircle(cor, vec2(58.0, 374.0), 8.0 + flash_color * 16.0));
			
			vec4 tx = CopyTexture(cor, vec2(288.0, 0.0), vec2(384.0, 96.0), vec2(10.0, 358.0), vec2(106.0, 454.0));
			col = mix(col, tx, FillRect(cor, vec2(10.0, 358.0), vec2(106.0, 454.0)) * tx.a);
		}
	}
	//col = mix(col, vec4(1.0, 0.0, 0.0, 1.0), DrawSprite(cor, vec2(100.0, 100.0)));
	
	//col = mix(col, vec4(1.0, 0.0, 0.0, 1.0), DrawCircle(cor, vec2(200.0, 200.0), 30.0 - sin(GlobalTime + 3.14 / 4.0) * 30.0));
	//col = mix(col, vec4(0.0, 1.0, 0.0, 1.0), DrawCircle(cor, vec2(200.0, 200.0), 30.0 - sin(GlobalTime) * 30.0));

	gl_FragColor = col;
}
//-------------------------------------------------------------------------------

