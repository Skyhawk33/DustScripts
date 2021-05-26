#include '../../lib/entities.cpp';

//const string EMBED_alive_r = 'biker/idle0001.png';
const string EMBED_alive_l = 'biker/idle0004.png';
const string EMBED_dead_r = 'biker/cleansed10001.png';
const string EMBED_dead_l = 'biker/cleansed20001.png';

class script {
	scene@ g;
	sprites@ spr;

	script() {
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void build_sprites(message@ msg) {
		//puts("BUILDING");
		msg.set_string("idle","alive_l");
		msg.set_int("idle|offsetx",96);
		msg.set_int("idle|offsety",192);
		
		//because the dead totem isnt an entity, none of these sprites actually work :(
		msg.set_string("cfall1","dead_r");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",192);
		
		msg.set_string("cfall2","dead_l");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",192);
		
		msg.set_string("chover1","dead_r");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",192);
		
		msg.set_string("chover2","dead_l");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",192);
		
		msg.set_string("cidle1","dead_r");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",192);
		
		msg.set_string("cidle2","dead_l");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",192);
		
		msg.set_string("cland1","dead_r");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",192);
		
		msg.set_string("cland2","dead_l");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",192);
		
		msg.set_string("cleansed1","dead_r");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",192);
		
		msg.set_string("cleansed2","dead_l");
		msg.set_int("fall|offsetx",96);
		msg.set_int("fall|offsety",192);
	}
	
	void on_level_start() {
		//puts("LEVEL START");
		spr.add_sprite_set("script");
	}
	
    //only works when entities are added to the scene. for this reason,
    //the script requires an initial deathwarp to force the change
	void entity_on_add(entity@ e){
		//puts("ADDING ENTITY "+e.type_name());
		if(e.type_name() == "enemy_stonebro") {
			e.set_sprites(spr);
		}
	}
}