const int MAX_PLAYERS = 4;

class script {
	scene@ g;
	array <dustman@> players(MAX_PLAYERS, null);

	script() {
		@g = get_scene();
	}

	void step(int entities) {
		entity@ e;
		for (uint i = 0; i < MAX_PLAYERS; i++)
		{
			if (@players[i] == null) {
				@e = controller_entity(i);
				if (@e != null) {
					@players[i] = e.as_dustman();
				}
			}

			if (@players[i] != null) {
				//if the player exists, check if they want to swap
				if(players[i].taunt_intent()==1)
				{
					puts("changing character: Player "+i+" Character = "+players[i].character());
					string character = players[i].character();
					if(character == "dustman"){
						players[i].character("dustgirl");
					}
					else if(character == "dustgirl"){
						players[i].character("dustkid");
					}
					else if(character == "dustkid"){
						players[i].character("dustworth");
					}
					else if(character == "dustworth"){
						players[i].character("dustman");
					}
					
					if(character == "vdustman"){
						players[i].character("vdustgirl");
					}
					else if(character == "vdustgirl"){
						players[i].character("vdustkid");
					}
					else if(character == "vdustkid"){
						players[i].character("vdustworth");
					}
					else if(character == "vdustworth"){
						players[i].character("vdustman");
					}
				}
			}
		}
	}
	
	void checkpoint_save() {
		
	}
	
	void checkpoint_load() {
		for(uint i=0; i< players.length(); i++)
			@players[i] = null;
	}
}