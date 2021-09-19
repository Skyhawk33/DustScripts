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
                    //if we successfully got a dustman object, process it
                    if (@players[i] != null) {
                        puts("changing character: Player "+i+" Character = "+players[i].character());
                        string character = players[i].character();
                        if(character == "dustkid"){
                            players[i].character("dustgirl");
                        }
                        else if(character == "dustworth"){
                            players[i].character("dustman");
                        }
                        if(character == "vdustkid"){
                            players[i].character("vdustgirl");
                        }
                        else if(character == "vdustworth"){
                            players[i].character("vdustman");
                        }
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