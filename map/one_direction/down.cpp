class script : callback_base {
    scene@ g;
    dustman@ player;
    int count;

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
        
        player.x_intent(0);
        if(player.y_intent() == -1)
            player.y_intent(0);
    }
    
    void checkpoint_load() {
        @player = null;
    }
}