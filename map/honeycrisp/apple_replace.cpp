#include '../../lib/entities.cpp';

const string EMBED_fall = 'apple/fall0001.png';
const string EMBED_hitwall1 = 'apple/hitwall10001.png';
const string EMBED_hitwall2 = 'apple/hitwall20001.png';
const string EMBED_hover = 'apple/hover0001.png';
const string EMBED_idle = 'apple/idle0001.png';
const string EMBED_land = 'apple/land0001.png';
const string EMBED_stun1 = 'apple/stun10001.png';
const string EMBED_stun2 = 'apple/stun20001.png';

class script {
	scene@ g;
	[entity] int apple_id;
	entity@ apple;
	sprites@ spr;

	script() {
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void build_sprites(message@ msg) {
		//puts("BUILDING");
		msg.set_string("idle","idle");
		msg.set_int("idle|offsetx",96);
		msg.set_int("idle|offsety",168);
		
		msg.set_string("fall","fall");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",100);
		
		msg.set_string("hitwall1","hitwall1");
		msg.set_int("hitwall1|offsetx",96);
		msg.set_int("hitwall1|offsety",90);
		
		msg.set_string("hitwall2","hitwall2");
		msg.set_int("hitwall2|offsetx",96);
		msg.set_int("hitwall2|offsety",90);
		
		msg.set_string("hover","hover");
		msg.set_int("hover|offsetx",96);
		msg.set_int("hover|offsety",170);
		
		msg.set_string("land","land");
		msg.set_int("land|offsetx",96);
		msg.set_int("land|offsety",170);
		
		msg.set_string("stun1","stun1");
		msg.set_int("stun1|offsetx",96);
		msg.set_int("stun1|offsety",90);
		
		msg.set_string("stun2","stun2");
		msg.set_int("stun2|offsetx",96);
		msg.set_int("stun2|offsety",90);
	}
	
	void on_level_start() {
		//puts("LEVEL START");
		spr.add_sprite_set("script");
		@apple = entity_by_id(apple_id);
		if(@apple != null && apple.type_name() == "hittable_apple")
			apple.set_sprites(spr);
	}
	
	void entity_on_add(entity@ e){
		if(e.id() == apple_id)
			e.set_sprites(spr);
	}
	
    //debugging code for finding apple IDs
	bool printed = true;
	void step(int entities) {
		if(!printed) {
			for(int i = 0; i < entities; i++) {
				puts(entity_string(entity_by_index(i)));
			}
			printed = true;
		}
	}
}