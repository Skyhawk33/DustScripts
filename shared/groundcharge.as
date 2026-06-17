class script : callback_base {
    scene@ g;
    array <dustman@> players;
    array <int> max_charges(4, 1);
    array <int> ground_charges(4, 1);
    array <bool> disabled(4, false);

    script() {
        @g = get_scene();
        
        int num_players = num_cameras();
        players.resize(num_players);
    }

    void step(int entities) {
        //if no player is defined, get the player
        for(uint i=0; i < players.length(); i++) {
            if(@players[i] == null){
                controllable@ c = controller_controllable(i);
                if(@c != null){
                    @players[i] = c.as_dustman();
                    players[i].on_subframe_end_callback(this, "on_subframe_end", i);
                    if( players[i].character() == "dustkid" ||
                            players[i].character() == "vdustkid" ||
                            players[i].character() == "trashking" ||
                            players[i].character() == "vtrashking" )
                        max_charges[i] = 2;
                    else
                        max_charges[i] = 1;
                    ground_charges[i] = max_charges[i];
                }
            }
            if(@players[i] != null) {
                disabled[i] = (players[i].dash_intent() == 1 && ground_charges[i] <= 0);
                if(disabled[i])
                    players[i].dash_intent(0);
            }
        }
    }
    
    void on_subframe_end(dustman@ dm, int i) {
        if(!dm.ground()) {
            ground_charges[i] = max_charges[i];
            if(disabled[i]) {
                disabled[i] = false;
                players[i].dash_intent(1);
            }
        }
    }
    
    void step_post(int entities) {
        for(uint i=0; i < players.length(); i++)
            if(players[i].dash_intent() == 2)
                ground_charges[i] -= 1;
    }
    
    void checkpoint_load() {
        for(uint i=0; i< players.length(); i++)
            @players[i] = null;
    }
}