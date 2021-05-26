/**
 * Author: Cmann1
 * https://github.com/cmann1/DustScripts
 */

const uint8 COL_TYPE_AI_CONTROLLER = 15;
const uint8 COL_TYPE_HITTABLE = 7;

float dist_sqr(float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return dx * dx + dy * dy;
}

int rand_range(int min, int max)
{
	return min + (rand() % (max - min + 1));
}
float rand_range(float min, float max)
{
	return min + (max - min) * frand();
}

float frand()
{
	return float(rand()) / float(0x3fffffff);
}

void outline_rect(scene@ g, float x1, float y1, float x2, float y2, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
{
	if(world)
	{
		// Top
		g.draw_rectangle_world(layer, sub_layer,
			x1 - thickness, y1 - thickness,
			x2 + thickness, y1 + thickness,
			0, colour);
		// Bottom
		g.draw_rectangle_world(layer, sub_layer,
			x1 - thickness, y2 - thickness,
			x2 + thickness, y2 + thickness,
			0, colour);
		// Left
		g.draw_rectangle_world(layer, sub_layer,
			x1 - thickness, y1 - thickness,
			x1 + thickness, y2 + thickness,
			0, colour);
		// Right
		g.draw_rectangle_world(layer, sub_layer,
			x2 - thickness, y1 - thickness,
			x2 + thickness, y2 + thickness,
			0, colour);
	}
	else
	{
		// Top
		g.draw_rectangle_hud(layer, sub_layer,
			x1 - thickness, y1 - thickness,
			x2 + thickness, y1 + thickness,
			0, colour);
		// Bottom
		g.draw_rectangle_hud(layer, sub_layer,
			x1 - thickness, y2 - thickness,
			x2 + thickness, y2 + thickness,
			0, colour);
		// Left
		g.draw_rectangle_hud(layer, sub_layer,
			x1 - thickness, y1 - thickness,
			x1 + thickness, y2 + thickness,
			0, colour);
		// Right
		g.draw_rectangle_hud(layer, sub_layer,
			x2 - thickness, y1 - thickness,
			x2 + thickness, y2 + thickness,
			0, colour);
	}
}

void draw_dot(scene@ g, int layer, int sub_layer, float x, float y, float size=1, uint colour=0xFFFFFFFF, float rotation=0)
{
	g.draw_rectangle_world(layer, sub_layer, x-size, y-size, x+size, y+size, rotation, colour);
}