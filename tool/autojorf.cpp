class script {
	scene@ g;
	dustman@ player;
	float start_x, start_y;
	bool player_moved = false;
	
	script() {
		@g = get_scene();
	}
	
	void step(int entities) {
		//if no player is defined, get the player
		if(@player == null){
			controllable@ c = controller_controllable(0);
			if(@c != null){
				@player = c.as_dustman();
				
				player.auto_respawn(false);
				start_x = player.x();
				start_y = player.y();
				
				float jorf_x = start_x;
				jorf_x -= jorf_x >= 0 ? jorf_x%48 : 48+jorf_x%48;
				
				// make an educated guess about which side they want to jorf on
				if((start_x%48+48)%48 < 24)
					jorf_x += 10;
				else
					jorf_x += 37;
				
				player.x(jorf_x);
				g.save_checkpoint(int(jorf_x), int(player.y()));
			}
		}
		
		if(player.x_intent() != 0 || player.y_intent() != 0
				|| player.dash_intent() != 0 || player.jump_intent() != 0) {
			player_moved = true;
			player.auto_respawn(true);
		}
		
		// if our initial positioning killed them, try the other side
		if(@player != null && player.dead() && !player_moved) {
			player.dead(false);
			player.state(0);
			player.set_speed_xy(0,0);
			
			float jorf_x = start_x;
			jorf_x -= jorf_x >= 0 ? jorf_x%48 : 48+jorf_x%48;
			
			// make an educated guess about which side they want to jorf on
			if((start_x%48+48)%48 < 24)
				jorf_x -= 11;
			else
				jorf_x += 58;
			
			player.set_xy(jorf_x, start_y);
			g.save_checkpoint(int(jorf_x), int(start_y));
			player.auto_respawn(true);	
		}
	}
}