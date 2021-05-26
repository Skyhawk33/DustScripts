#include 'include.as'
#include '../../lib/entities.cpp'

const int SLOPEJUMP_START = 11100;
const int SLOPEJUMP_END = 11150;
const int DUSTMAN_HALF_GAP_X = 11720;
const int DUSTGIRL_HALF_GAP_X = 11700;
const int DUSTKID_HALF_GAP_X = 11500;
const int DUSTKID_HALF_GAP_X2 = 11700;
const int DUSTWORTH_HALF_GAP_X = 11700;
const int LIGHT_HEIGHT = 150;
const int GAP_END_X = 12060;

class script : callback_base{
	scene@ g;
	[entity] int entity_id = 0;
	controllable@ player;
	controllable@ old_player;
	bool needs_init = true;
	int initial_life;
	int life_remaining;
	
	dustman@ man = null;
	entity@ target = null;
	[position,mode:world,layer:19,y:room_y1] float room_x1;
	[hidden] float room_y1;
	[position,mode:world,layer:19,y:room_y2] float room_x2;
	[hidden] float room_y2;
	[position,mode:world,layer:19,y:man_y] float man_x;
	[hidden] float man_y;
	[position,mode:world,layer:19,y:death_y] float death_x;
	[hidden] float death_y;
	
	int t = 0;
	int t_step = 10;
	int attack_timer = 0;
	int attack_timer_max1 = 30;
	int attack_timer_max2 = 70;
	int jump_timer = 0;
	int jump_timer_max_forced = 15;
	int jump_timer_max = 45;
	int jump_timer_max_kid = 30;
	
	int dustman_starting_health = 4;
	int dustman_health = dustman_starting_health;
	
	bool active = false;
	bool dead = false;
	bool trigger_ending = false;
	bool level_ended = false;
	
	int dustblock_x = 251;
	int dustblock_y = 83;
	int dustblock_width = 11;
	int dustblock_height = 4;
	
	int roof_y = 36;
	int floor_y = 55;
	int gap_x = 251;
	int wall_x = 274;
	
	array <entity@> entity_storage(12, null);
	bool entities_removed = false;

	script() {
		@g = get_scene();
	}
	
	void on_level_start() {
		if(entity_id == 0) {
			needs_init = false;
		}
		active = true;
	}
	
	void player_attack(controllable@ player, controllable@ target, hitbox@ attack, int arg) {
		dustman_health--;
		if(dustman_health <= 0) {
			kill_dustman();
		}
	}
	
	void kill_dustman() {
		if(@man != null) {
			puts("Player dying!");
			man.dead(false);
			dustman_health = 0;
			man.set_xy(death_x,death_y);
			dead = true;
			if(!level_ended) {
				trigger_ending = true;
			}
		}
	}
	
	void entity_on_remove(entity@ e) {
		if(e.is_same(player)) {
			puts("Removing player!");
			controller_entity(0, old_player.as_controllable());
			if(!level_ended) {
				life_remaining = 0;
				trigger_ending = true;
			}
		}
	}
	
	void step_post(int entities) {
		if(trigger_ending && !level_ended) {
			g.end_level(death_x,death_y);
			level_ended = true;
		}
	}
	
	void on_level_end() {
		level_ended = true;
		g.combo_break_count(initial_life-life_remaining);
		
		//int f1,b1,e1, f2, b2, e2;
		//g.get_filth_level(f1,b1,e1);
		//puts(f1+" "+b1+" "+e1);
		//g.get_filth_remaining(f2,b2,e2);
		//puts(f2+" "+b2+" "+e2);
		//int dust_removed = (f1-f2)+(b1-b2)+(e1-e2);
		//puts("dust removed "+dust_removed);
		
		int D_add_amount = 30;
		int C_add_amount = 20;
		int B_add_amount = 10;
		int A_add_amount = 1;
		int S_add_amount = 0;
		int add_amount = 0;
		
		if(dustman_health == 0) {
			add_amount = S_add_amount;
			puts('rewarding S');
		}
		else if(dustman_health*3 < dustman_starting_health) {
			add_amount = A_add_amount;
			puts('rewarding A');
		}
		else if(dustman_health*3 < dustman_starting_health*2) {
			add_amount = B_add_amount;
			puts('rewarding B');
		}
		else if(dustman_health < dustman_starting_health) {
			add_amount = C_add_amount;
			puts('rewarding C');
		}
		else {
			add_amount = D_add_amount;
			puts('rewarding D');
		}
		puts('adding '+add_amount);
		
		for(int x=0; x<dustblock_width; x++) {
			for(int y=0; y<dustblock_height; y++) {
				if(add_amount-- <= 0)
					break;
				g.set_tile(dustblock_x+x, dustblock_y+y, 19, true, 0, 1, 21, 1);
			}
			if(add_amount <= 0)
				break;
		}
		
		tilefilth@ filth = create_tilefilth();
		for(int x=gap_x; x<=wall_x; x++) {
			g.set_tile_filth(x, roof_y, filth);
			g.set_tile_filth(x, floor_y, filth);
		}
		for(int y=roof_y; y<=floor_y; y++) {
			g.set_tile_filth(wall_x, y, filth);
		}
		g.set_tile_filth(wall_x-1,roof_y+1, filth);
		
		int count = g.get_entity_collision(room_y1, room_y2, room_x1, room_x2, COL_TYPE_HITTABLE);
		for(int i = 0; i < count; i++)
		{
			entity@ e = g.get_entity_collision_index(i);
			if(@e.as_dustman() == null) {
				@entity_storage[i] = @e;
			}
		}
		for(int i = 0; i < count; i++)
		{
			if(@entity_storage[i] != null) {
				g.remove_entity(entity_storage[i]);
			}
		}
		entities_removed = true;
	}
	
	void draw(float sub_frame) {
		textfield@ tf = create_textfield();
		tf.text("Health: "+life_remaining+" / "+initial_life);
		tf.align_horizontal(-1);
		tf.align_vertical(1);
		tf.draw_hud(0, 1, -770, 400, 1, 1, 0);
		
		int healthbar_length = 1200 * dustman_health / dustman_starting_health;
		
		g.draw_rectangle_hud(0, 0, -610, -430, 610, -370, 0, 0xFF444444);
		g.draw_rectangle_hud(0, 0, -600, -420, 600, -380, 0, 0xFF501010);
		g.draw_rectangle_hud(0, 0, -600, -420, -600+healthbar_length, -380, 0, 0xFF40C040);
	}
	
	void editor_draw(float sub_frame) {
		draw_dot(g, 22, 22, man_x, man_y, 5, 0xFF0000FF, 0);
		outline_rect(g, room_x1, room_y1, room_x2, room_y2, 22, 22, 2, 0x88FF0000);
	}
	
	void step(int entities) {
		if(entities_removed) {
			for(uint i=0; i<entity_storage.length; i++)
				if(@entity_storage[i] != null) {
					if(entity_storage[i].is_same(player)) {
						g.add_entity(entity_storage[i]);
					}
					else {
						float x = entity_storage[i].x();
						float y = entity_storage[i].y();
						string name = entity_storage[i].type_name();
						int face = entity_storage[i].face();
						add_entity(g,name,x,y).face(face);
					}
				}
			entities_removed = false;
		}
		
		if(needs_init) {
			puts("running!");
			@old_player = controller_controllable(0);
			entity@ e = entity_by_id(entity_id);
			if(@e != null) {
				@player = e.as_controllable();
				if(@player != null) {
					controller_entity(0,player);
					player.on_hit_callback(this, "player_attack", 0);
					initial_life = player.life();
					life_remaining = player.life();
				}
			}
			needs_init = false;
		}
		if(@player != null) {
			if(!level_ended && player.life()>0) {
				life_remaining = player.as_controllable().life();
			}
		}
		
		if(dead) {
			man.x_intent(0);
			man.y_intent(0);
			man.jump_intent(0);
			return;
		}
	
		if(!active)
			return;
		
		if(man is null)
		{
			entity@ e = create_entity(old_player.type_name()).as_entity();
			e.set_xy(man_x, man_y);
			
			@man = e.as_dustman();
			man.ai_disabled(true);
			man.team(1);
			man.auto_respawn(false);
			
			g.add_entity(e, false);
		}
		
		if(man.dead()) {
			kill_dustman();
		}
		
		const float dm_x = man.x();
		const float dm_y = man.y();
		
		if(t++ % t_step == 0)
		{
			if(man is null) return;
				
			int count = g.get_entity_collision(room_y1, room_y2, room_x1, room_x2, COL_TYPE_HITTABLE);
			float entity_distance = 0;
			
			bool target_alive = false;
			for(int i = 0; i < count; i++)
			{
				entity@ e = g.get_entity_collision_index(i);
				if(e.is_same(target)) {
					target_alive = true;
					break;
				}
			}
			
			if(!target_alive || t > 300) {
				@target = null;
				if(count > 0) {
					int target_index = rand_range(0,count-1);
					entity@ e = g.get_entity_collision_index(target_index);
						
					if(e.is_same(man)) {
						if(count > 1) {
							@e = g.get_entity_collision_index((target_index+1)%count);
						}
						else {
							@e = null;
						}
					}
					@target = e;
					t = 1;
					if(@target != null)
						puts("Targetting "+e.type_name());
				}
			}
			
			if(target is null)
			{
				man.x_intent(0);
				man.y_intent(0);
				man.jump_intent(0);
			}
		}
		
		if(attack_timer > 0)
			attack_timer--;
		if(jump_timer > 0)
			jump_timer--;
		
		if(@target != null)
		{
			const float tx = target.x();
			const float ty = target.y();
			const float dx = dm_x - tx;
			const float dy = dm_y - ty;
			
			if(dm_x < tx)
				man.x_intent(abs(dx) > 48 ? 1 : 0);
			else
				man.x_intent(abs(dx) > 48 ? -1 : 0);
			
			if(DUSTKID_HALF_GAP_X < dm_x && dm_x < GAP_END_X) {
				man.y_intent(0);
			}
			else if(dy > 96)
				man.y_intent(-1);
			else if(dy <= -48)
				man.y_intent(1);
			else
				man.y_intent(frand() > 0.80 ? 1 : 0);
			
			if(jump_timer == 0 && (
				(SLOPEJUMP_START < dm_x && dm_x < SLOPEJUMP_END) ||
				(man.type_name() == "dust_man" && DUSTMAN_HALF_GAP_X < dm_x && dm_x < DUSTMAN_HALF_GAP_X + 48) ||
				(man.type_name() == "dust_girl" && DUSTGIRL_HALF_GAP_X < dm_x && dm_x < DUSTGIRL_HALF_GAP_X + 48) ||
				(man.type_name() == "dust_kid" && DUSTKID_HALF_GAP_X < dm_x && dm_x < DUSTKID_HALF_GAP_X + 48) ||
				(man.type_name() == "dust_kid" && DUSTKID_HALF_GAP_X2 < dm_x && dm_x < DUSTKID_HALF_GAP_X2 + 48) ||
				(man.type_name() == "dust_worth" && DUSTWORTH_HALF_GAP_X < dm_x && dm_x < DUSTWORTH_HALF_GAP_X + 48)))
			{
				man.jump_intent(1);
				jump_timer = jump_timer_max_forced;
			}
			else if (jump_timer == 0 && dy > 50 && GAP_END_X < dm_x)
			{
				man.jump_intent(1);
				if(man.type_name() == "dust_kid")
					jump_timer = jump_timer_max_kid;
				else
					jump_timer = jump_timer_max;
			}
			
			else if(attack_timer == 0)
			{
				if(@player != null && dm_x < GAP_END_X && abs(dm_x-player.x()) < 350 && abs(dm_y-player.y()) < 96 && man.dash()<man.dash_max()) {
					if(dx < 0 && man.face() < 0)
						man.face(1);
					else if(dx > 0 && man.face() > 0)
						man.face(-1);
					
					man.y_intent(0);
					man.heavy_intent(1);
					attack_timer = 5;
				}
				else if(abs(dx) < 96)
				{
					if(dx < 0 && man.face() < 0)
						man.face(1);
					else if(dx > 0 && man.face() > 0)
						man.face(-1);
					
					if(dy > LIGHT_HEIGHT)
						man.heavy_intent(1);
					else if(frand() > 0.6)
						man.heavy_intent(1);
					else
						man.light_intent(1);
					attack_timer = rand_range(attack_timer_max1, attack_timer_max2);
				}
			}
		}
	}
}