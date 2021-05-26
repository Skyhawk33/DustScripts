class script {
	scene@ g;
	dustman@ player;

	script() {
		@g = get_scene();
		
	}

	void step(int entities) {
		//if no player is defined, get the player
		if(@player == null){
			controllable@ c = controller_controllable(0);
			if(@c != null){
				@player = c.as_dustman();
			}
		}
        
        if(player.skill_combo() >= 100)
            player.kill(false);
	}
	
	void checkpoint_save() {
		
	}
	
	void checkpoint_load() {
		@player = null;
	}
}