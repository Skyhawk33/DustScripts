const int MAX_PLAYERS = 4;

class script {
	scene@ g;
	array <controllable@> players(MAX_PLAYERS, null);
	[text] array<Clone> clones;
    bool checkpoint;

	script() {
		@g = get_scene();
	}
	
	void spawn_player(message@ msg) {
		//puts(""+msg.get_float("x"));
		//puts(""+msg.get_float("y"));
		//puts(""+msg.get_int("player"));
		//puts(msg.get_string("character"));
		//puts("");
		
		float x = msg.get_float("x");
		float y = msg.get_float("y");
		int player_num = msg.get_int("player");
		string character = msg.get_string("character");
		
		for(uint i=0; i<clones.length(); i++) {
			if (clones[i].parent == player_num + 1) {
				clones[i].spawn(g,x,y,character, checkpoint);
			}
		}
	}

	void step(int entities) {
		//if no players are defined, get the players
		if(@players[0] == null){
			for(uint i=0; i < players.length(); i++) {
				@players[i] = controller_controllable(i);
			}
		}
		
		for(uint i=0; i < players.length(); i++) {
			if(@players[i] != null){
				for(uint j=0; j<clones.length(); j++) {
					if (clones[j].parent == int(i+1)) {
						clones[j].copy_intents(players[i]);
					}
				}
			}
		}
	}
	
	void checkpoint_save() {
		checkpoint = true;
        for(uint i=0; i<clones.length(); i++) {
			clones[i].save_checkpoint();
		}
	}
	
	void checkpoint_load() {
		for(uint i=0; i< players.length(); i++)
			@players[i] = null;
	}
}

class Clone {
	[text] int parent = 1;
	[text] int team = 1;
	[position,mode:world,layer:18,y:spawn_y] int spawn_x = 0;
	[hidden]int spawn_y = 0;
	[boolean] bool relative_pos = false;
	[text] float rel_x = 0;
	[text] float rel_y = 0;
	[boolean] bool lock_character = false;
	[text] string lock_type = "dustman";
	[boolean] bool virtual_character = false;
	[boolean] bool invert_horizontal = false;
	
    float checkpoint_x;
    float checkpoint_y;
    
	string character;
	controllable@ clone;
	
	Clone() {
		
	}
	
	void copy_intents(controllable@ c) {
		if(invert_horizontal) {
			clone.x_intent(	-c.x_intent());
		}
		else {
			clone.x_intent(c.x_intent());
		}
		clone.y_intent(c.y_intent());
		clone.taunt_intent(c.taunt_intent());
		clone.heavy_intent(c.heavy_intent());
		clone.light_intent(c.light_intent());
		clone.dash_intent(c.dash_intent());
		clone.jump_intent(c.jump_intent());
		clone.fall_intent(c.fall_intent());
	}
    
    void save_checkpoint() {
        checkpoint_x = clone.x();
        checkpoint_y = clone.y();
    }
	
	//create a clone entity based on attributes & parent
	void spawn(scene@ g, float parent_x, float parent_y, string parent_character, bool checkpoint) {
		float x=0, y=0;
		character = lock_type;
		//if they're using relative position, place the clone at an offset from the parent
		if(relative_pos) {
			x = parent_x + rel_x;
			y = parent_y + rel_y;
		}
		//otherwise use the fixed spawn point
		else {
			x = spawn_x;
			y = spawn_y;
		}
        
        if(checkpoint) {
            x = checkpoint_x;
            y = checkpoint_y;
        }
        else {
            checkpoint_x = x;
            checkpoint_y = y;
        }
        
		//if they didnt pick a specific character, match the parent
		if(!lock_character) {
			character = parent_character;
		}
		
		entity@ clone_entity = null;
		// if they've locked it to a non-dustman controllable, try to create it
		@clone_entity = create_entity(character);
		// otherwise, the thing they're creating must be a dustman
		if(@clone_entity == null)
			@clone_entity = create_entity("dust_man");
		clone_entity.x(x);
		clone_entity.y(y);
		
		@clone = clone_entity.as_controllable();
		clone.team(team);
		
		if(@clone.as_dustman() != null){
			//if the character isnt virtual, make it virtual
			if(virtual_character && character.substr(0,1) != "v") {
				character = "v"+character;
			}
			
			clone.as_dustman().character(character);
			clone.as_dustman().ai_disabled(true);
		}
		
		g.add_entity(clone_entity, false);
	}
}