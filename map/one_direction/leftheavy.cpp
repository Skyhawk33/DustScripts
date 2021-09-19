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
        
        if(player.attack_state() == 2) {
            count++;
            if(count == 1) {
                //hitbox@ hb = controller_controllable(0).hitbox();
                //hb.attack_dir(-hb.attack_dir());
                //player.attack_face(-1);
                player.x_intent(-1);
            }
        } else {
            count = 0;
        }
    }
    
    void checkpoint_load() {
        @player = null;
    }
}