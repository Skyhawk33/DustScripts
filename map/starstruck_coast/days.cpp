/**
 * Author: BigDiesel
 */

class script {
	scene@ g;
	sprites@ sun;
	sprites@ moon;
	timedate@ time;
	float second;
	float tick = 0;
	
	[boolean|tooltip:"Enable to test settings at accelerated speed"]
	bool superfast_mode = false;
	
	[text|tooltip:"Increase to widen the arc"] float xtravel = 160000;
	[text|tooltip:"Adjusts the centering of the arc"] float xoffset = 0;
	
	[text|tooltip:"Increase to make the arc steeper"] float ytravel = 35000;
	[text|tooltip:"Increase to lower the arc"] float yoffset = 20000;
	
	script() {
		@g = get_scene();
		@sun = create_sprites();
		@moon = create_sprites();
		sun.add_sprite_set('props2');
		moon.add_sprite_set('props1');
	}
	
	void draw(float sub_frame) {
		@time = localtime();
		if (superfast_mode) {
			tick++;
			if (tick > 120) {
				tick 	= 0;
			}
			second = tick/120;
		}
		else {
			second = ((time.hour()*3600)+(time.min()*60)+time.sec())/86400.0;
		}
		float anglemath = cos(second*3.14159*2.0);
		
		moon.draw_world(0, 1, "backdrops_4", 1, 1, (floor(second+0.5)*xtravel)+xoffset-(second*xtravel), (-anglemath*ytravel)+yoffset, 0, 1.0, 1.0, 0xFFFFFFFF);
		sun.draw_world(0, 1, "backdrops_4", 1, 1, (xtravel/2)+xoffset-(second*xtravel), (anglemath*ytravel)+yoffset, 0, 1.0, 1.0, 0xFFFFFFFF);
	}
}