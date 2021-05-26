class script: callback_base {
    scene@ g;
    dustman@ player;
    int filth = 9999;
    
    [tooltip:"The palette dust will be set to."]
    int dust_type = 1;
    
    [tooltip:"The distance around the player that will be checked for dust spread.\nincreasing this too much may cause lag"]
    int range = 15;

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
    }
    
    void step_post(int entities) {
        int f, _;
        g.get_filth_remaining(f, _, _);
        if(f > filth) {
            replace_filth(player.x(), player.y(), range);
        }
        filth = f;
    }
    
    void checkpoint_load() {
        @player = null;
    }
    
    void replace_filth(float ox, float oy, int range) {
        int tile_x = int(ox / 48);
        int tile_y = int(oy / 48);
        tilefilth@ tf;
        
        for (int x = tile_x - range; x <= tile_x + range; x++ ) {
            for (int y = tile_y - range; y <= tile_y + range; y++ ) {
                @tf = g.get_tile_filth(x, y);
                if (tf.top() > 0)
                    tf.top(dust_type);
                if (tf.bottom() > 0)
                    tf.bottom(dust_type);
                if (tf.left() > 0)
                    tf.left(dust_type);
                if (tf.right() > 0)
                    tf.right(dust_type);
                g.set_tile_filth(x, y, tf);
            }
        }
    }
}