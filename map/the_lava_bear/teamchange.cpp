class script {
    scene@ g;
    
    array <dustman@> players;
    [fixed] array <int> team = {1, 2, 3, 4};

    script() {
        @g = get_scene();
        
        int num_players = num_cameras();
        players.resize(num_players);
    }

    void step(int entities) {
        //if no players are defined, get the players
        if(@players[0] == null){
            for(uint i=0; i < players.length(); i++) {
                controllable@ c = controller_controllable(i);
                if(@c != null){
                    c.team(team[i]);
                    @players[i] = c.as_dustman();
                }
            }
        }
    }
    
    void checkpoint_load() {
        for(uint i=0; i< players.length(); i++)
            @players[i] = null;
    }
}