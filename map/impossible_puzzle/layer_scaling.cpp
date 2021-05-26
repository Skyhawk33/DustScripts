#include '../../lib/entities.cpp'

class script {
	scene@ g;

	script() {
		@g = get_scene();
		for(uint layer=6; layer<12; layer++) {
			g.layer_scale(layer, 1.0 - (12-layer)*0.05);
		}
		for(uint layer=12; layer<17; layer++) {
			g.layer_scale(layer, 1.0);
		}
		for(uint i=1; i<20; i++) {
			puts(""+g.layer_scale(i));
		}
	}
	
	void on_level_start() {
		for(uint layer=6; layer<17; layer++) {
			g.layer_scale(layer, 1.0 - (17-layer)*0.03);
		}
	}
	
	float layer_gap = 0.03;
	float amplitude = 0.03;
	float layer_radian_gap = 0.28;
	float pulse_speed = 0.025;
	float radian = -layer_radian_gap*11;
	
	void step(int entities) {
		for(uint layer=6; layer<17; layer++) {
			g.layer_scale(layer, .95 - (17-layer)*layer_gap
				+ max(0,amplitude*sin(radian + layer_radian_gap*(layer-6))));
		}
		radian += pulse_speed;
		if(radian > 3.2)
			radian = -3.2;
	}
	
	//float layer_gap = 0.03;
	//float amplitude = 0.03;
	//float layer_radian_gap = 0.28;
	//float pulse_speed = 0.02;
	//float radian = 0;
	//
	//void step(int entities) {
	//	for(uint layer=6; layer<17; layer++) {
	//		g.layer_scale(layer, 1.0 - (17-layer)*layer_gap
	//			+ amplitude*abs(sin(radian + layer_radian_gap*(layer-6))));
	//	}
	//	radian += pulse_speed;
	//	radian %= 220;
	//}
}