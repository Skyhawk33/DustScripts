const int MAX_PLAYERS = 4;

class script {
	scene@ g;
	
	//the player entities
	array <dustman@> players(MAX_PLAYERS, null);
	
	//the inputs on the previous frame
	//used for detecting the rising-edge of inputs
	array <uint> prev(MAX_PLAYERS, 0);
	
	//the array of button combinations each player has pressed
	array <array <uint>> total(MAX_PLAYERS, array<uint>(1,0));

	script() {
		@g = get_scene();
	}
	
	int frame_count = 0;
	bool level_started() {
		
		//this code prevents a bug involving holding buttons during startup.
		//however, it allows players to hold a direction during the countdown
		//without penalty, so for now I'm keeping it out. If I add it in later,
		//scores will strictly improve, so it's still backwards compatible
		
		//if(frame_count < 55) {
		//	frame_count++;
		//	return false;
		//}
		//else if (frame_count == 55) {
		//	//ignore the frame one input, because lights and heavies do not register correctly
		//	for(uint i=0; i < players.length(); i++) {
		//		if(@players[i] != null) {
		//			prev[i] = compress_intents(@players[i]);
		//		}
		//	}
		//	frame_count++;
		//	return false;
		//}
		
		return true;
	}
	
	void step(int entities) {
		//if no players are defined, get the players
		if(@players[0] == null){
			for(uint i=0; i < players.length(); i++) {
				controllable@ c = controller_controllable(i);
				if(@c != null){
					@players[i] = c.as_dustman();
				}
			}
		}
		
		if(!level_started()) {
			// dont process inputs until after the first frame
			return;
		}
		
		for(uint i=0; i < players.length(); i++) {
			if(@players[i] != null) {
				
				//get the intents on the current frame
				uint intents = compress_intents(@players[i]);
				//reduce it to only the intents that were pressed this frame
				uint rising = intents & (~prev[i]);
				
				//if this combination of buttons has never been pressed before, record it
				if(total[i].find(rising) < 0) {
					total[i].insertLast(rising);
					
					//debug info
					puts("player "+(i+1)+" pressed: " + button_string(rising));
				}
				
				//record the current intents for next frame
				prev[i] = intents;
			}
		}
	}
	
	/**
	 * to generate the leaderboard score, we need to find out how many buttons were used
	 * when two buttons are pressed together, they aren't necessarily bound to the same button
	 * for example during a regular dashjump. To tell the difference between mapping the
	 * actions to the same button and simply pressing the buttons together, we can look at
	 * all the combinations. for example, if one frame the player pressed Jump+Dash, and another
	 * frame they pressed just Jump, Jump must be its own button, because Dustforce does not allow
	 * binding an action to multiple keys. And therefore, Dash must be its own button as well.
	 * 
	 * We can compute this by generating the minimal power set that encloses all the combinations
	 * that occurred. I swear there's some math related to this but I've googled it a bunch and
	 * I can't seem to find the name of the subject.
	 */
	void on_level_end() {
		
		//the sum of all player's buttons
		uint total_buttons = 0;
		
		for(uint p = 0; p < players.length(); p++) {
			if(@players[p] != null) {
				
				//complete the power set of the given combinations
				for(uint i = 0; i < total[p].length(); i++) {
					for(uint j = 0; j < total[p].length(); j++) {
						
						//include the difference in combinations
						uint diff = total[p][j] & (~total[p][i]);
						if(total[p].find(diff) < 0)
							total[p].insertLast(diff);
						
						//include the superset of combinations
						uint sup = total[p][j] | (total[p][i]);
						if(total[p].find(sup) < 0)
							total[p].insertLast(sup);
					}
				}
				
				//the size of a power set is 2^n, for n seperate buttons
				uint max = total[p].length();
				uint buttons = 0;
				while(max > 1) {
					max /= 2;
					buttons += 1;
				}
				
				total_buttons += buttons;
				
				//debugging
				puts("player "+(p+1)+" buttons: "+buttons);
			}
		}
		
		//set the score to the total number of buttons used
		g.plugin_score(total_buttons);
		
		//debugging
		puts("total_buttons: "+total_buttons);
	}
	
	//creates a bitmap out of the current frame's intents
	uint compress_intents(dustman@ dm) {
		int compressed = 0;
		//if either intent is up, the dash button was just pressed
		//double tap dash probably messes with this assumption
		//bit 0 is dash
		if(dm.dash_intent()+dm.fall_intent() > 0)
			compressed += 1;
		//1 is right, 2 is left
		compressed += ((dm.x_intent()+3)%3)<<1;
		//3 is down, 4 is up
		compressed += ((dm.y_intent()+3)%3)<<3;
		// 10 or 11 means the attack is pressed
		compressed += (dm.heavy_intent()/10)<<5;
		compressed += (dm.light_intent()/10)<<6;
		
		if(dm.jump_intent() > 0)
			compressed += 1<<7;
		
		//ensure that the bitmap is in range just in case
		return compressed%(1<<8);
	}
	
	//debugging method. displays which button combination a bitmap represents
	string button_string(int compressed) {
		if(compressed == 0)
			return "nothing";
		
		//the order here has to match the order of bits used in compress_intents
		array<string> button_codes = {"Dash","Right","Left","Down","Up","Heavy","Light","Jump"};
		string buttons = "";
		
		//traverse through each bit
		for(uint i = 0; i < button_codes.length(); i++) {
			if(compressed & (1<<i) > 0) {
				//if the bit is up, include the input in the string
				if(buttons != "")
					buttons += " + ";
				buttons += button_codes[i];
			}
		}
		
		return buttons;
	}
	
	//if a player dies, their entity needs to be reloaded
	void checkpoint_load() {
		for(uint i=0; i< players.length(); i++)
			@players[i] = null;
	}
}