// delays the teleport on a room completion
// to make sure no extra dust is spread from heavies
// MUST BE AT LEAST 10
const int CONFIRM_DELAY = 10;

class script {
    scene@ g;
    textfield@ tf = null;
    
    [text] bool hide_overlay = false;
    [text] array<RoomData> rooms;
    int r = 0;
    int dust_initial, blocks_initial, enemy_initial;
    int confirmation_countdown = 0;
    
    script() {
        @g = get_scene();
    }
    
    void on_level_start()
    {
        g.get_filth_remaining( dust_initial, blocks_initial, enemy_initial );
    }
    
    void step_post(int)
    {
        if( r < int(rooms.length) ) {
            int dust, blocks, enemy;
            g.get_filth_remaining( dust, blocks, enemy );
            
            if( dust_initial - dust >= rooms[r].dust 
                    && blocks_initial - blocks >= rooms[r].blocks
                    && enemy_initial - enemy >= rooms[r].enemy ) {
                if( confirmation_countdown == 0 )
                    confirmation_countdown = CONFIRM_DELAY;
                else
                    confirmation_countdown--;
                
                if( confirmation_countdown <= 0 ) {
                    for(uint i=0; i<num_cameras(); i++) {
                        controller_controllable(i).set_xy(rooms[r].teleport_x, rooms[r].teleport_y);
                        reset_camera(i);
                    }
                    
                    r++;
                    dust_initial = dust;
                    blocks_initial = blocks;
                    enemy_initial = enemy;
                }
            }
            else
                confirmation_countdown = 0;
        }
    }
    
    void editor_var_changed(var_info@ info) {
        if(info.get_name() == "save_dust_counts") {
            rooms[info.get_index(0)].load_counts(@g);
            rooms[info.get_index(0)].save_dust_counts = false;
        }
        if(info.get_name() == "top_left_x")
            rooms[info.get_index(0)].top_left_x -= (rooms[info.get_index(0)].top_left_x % 48 + 48) % 48;
        if(info.get_name() == "top_y")
            rooms[info.get_index(0)].top_y -= (rooms[info.get_index(0)].top_y % 48 + 48) % 48;
        if(info.get_name() == "bottom_right_x")
            rooms[info.get_index(0)].bottom_right_x -= (rooms[info.get_index(0)].bottom_right_x % 48 - 48) % 48;
        if(info.get_name() == "bottom_y")
            rooms[info.get_index(0)].bottom_y -= (rooms[info.get_index(0)].bottom_y % 48 - 48) % 48;
    }
    
    void editor_draw(float) {
        if(@tf == null) {
            @tf = create_textfield();
            tf.align_vertical(-1);
            tf.align_horizontal(-1);
        }
        
        if( !hide_overlay )
            for(uint i=0; i<rooms.length; i++)
                rooms[i].editor_draw(@g, @tf, i);
    }
}

class RoomData {
    [position, mode:world, layer:19, y:top_y] float top_left_x;
    [hidden] float top_y;
    [position, mode:world, layer:19, y:bottom_y] float bottom_right_x;
    [hidden] float bottom_y;
    [text] bool save_dust_counts;
    
    [position, mode:world, layer:19, y:teleport_y] float teleport_x;
    [hidden] float teleport_y;
    
    [hidden] int dust;
    [hidden] int blocks;
    [hidden] int enemy;
    
    void load_counts(scene@ g) {
        tileinfo@ ti;
        tilefilth@ tf;
        hittable@ h;
        
        dust = blocks = enemy = 0;
        
        for( int x = int(top_left_x) / 48; x <= int(bottom_right_x) / 48; x++) {
            for( int y = int(top_y) / 48; y <= int(bottom_y) / 48; y++) {
                if( g.get_tile(x, y).is_dustblock() )
                    blocks++;
                
                @tf = g.get_tile_filth(x, y);
                if( 0 < tf.top() && tf.top() < 6 )
                    dust++;
                if( 0 < tf.bottom() && tf.bottom() < 6 )
                    dust++;
                if( 0 < tf.left() && tf.left() < 6 )
                    dust++;
                if( 0 < tf.right() && tf.right() < 6 )
                    dust++;
            }
        }
        
        int e = g.get_entity_collision(top_y, bottom_y, top_left_x, bottom_right_x, 1);
        for(int i=0; i<e; i++) {
            @h = g.get_entity_collision_index(i).as_hittable();
            if( h.type_name() != "hittable_apple" )
                enemy += h.life_initial();
        }
    }
    
    void editor_draw(scene@ g, textfield@ tf, int i) {
        uint color = 0x30FFFFFF;
        if(top_y > bottom_y || top_left_x > bottom_right_x)
            color = 0x30FF0000;
        
        g.draw_rectangle_world(21, 0, top_left_x, top_y, bottom_right_x, bottom_y, 0, color);
        
        g.draw_rectangle_world(21, 0, teleport_x-10, teleport_y-10, teleport_x+10, teleport_y+10, 0, 0xFF77FF77);
        
        tf.text(""+i);
        tf.draw_world(21, 1, teleport_x - 10, teleport_y + 12, 1, 1, 0);
        
        tf.text("Dust: "+dust);
        tf.draw_world(21, 1, top_left_x+10, top_y+10, 1, 1, 0);
        
        tf.text("Blocks: "+blocks);
        tf.draw_world(21, 1, top_left_x+10, top_y+40, 1, 1, 0);
        
        tf.text("Enemy: "+enemy);
        tf.draw_world(21, 1, top_left_x+10, top_y+70, 1, 1, 0);
    }
}
