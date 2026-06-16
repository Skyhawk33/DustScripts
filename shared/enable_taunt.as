class script : callback_base {
    scene@ g;
    array <dustman@> players;

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
                }
            }
            if(@players[i] != null) {
                check_taunt(@players[i]);
            }
        }
    }
    
    void on_subframe_end(dustman@ dm, int i) {
        check_taunt(@dm);
    }
    
    void check_taunt(dustman@ dm)
    {
        if(dm.taunt_intent() == 1 && dm.state() == 0 && dm.attack_state() == 0 )
        {
            dm.taunt_intent(2);
            dm.state(1);
        }
    }
    
    void checkpoint_load() {
        for(uint i=0; i< players.length(); i++)
            @players[i] = null;
    }
}