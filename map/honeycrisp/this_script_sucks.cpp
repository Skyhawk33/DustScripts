/**
 * Author: Jdude0822
 */

class script {
    scene@ g;
    [entity] uint apple_id;
    entity@ apple;
    [entity] uint old_text_id;
    [entity] uint new_text_id;
    // trigger area
    // top left
    [position,mode:world,layer:19,y:trigg_tly] int trigg_tlx = 0;
    [hidden] int trigg_tly = 0;
    // bottom right
    [position,mode:world,layer:19,y:trigg_bry] int trigg_brx = 0;
    [hidden] int trigg_bry = 0;
    // blocks to remove
    // top left
    [position,mode:world,layer:19,y:door_tly] int door_tlx = 0;
    [hidden] int door_tly = 0;
    // bottom right
    [position,mode:world,layer:19,y:door_bry] int door_brx = 0;
    [hidden] int door_bry = 0;
    
    // blocks to remove
    // top left
    [position,mode:world,layer:19,y:warp_y] int warp_x = 0;
    [hidden] int warp_y = 0;

    bool removed = false;
    
    script() {
        @g = get_scene();
    }
    
    void on_level_start() {
        @apple = entity_by_id(apple_id);
    }
    
    void entity_on_add(entity@ e){
        if(e.id() == apple_id){
            @apple = e;
        }
    }
    
    void entity_on_remove(entity@ e){
        if(e.id() == apple_id){
            @apple = null;
        }
    }
    
    void step(int entities) {
        if(@apple != null) {
            if(!removed && apple.x() > trigg_tlx && apple.x() < trigg_brx && apple.y() > trigg_tly && apple.y() < trigg_bry) {
                //apple is in the trigger area
                removed = true;
                puts("Remove!");
                for(int i = door_tlx/48; i <= door_brx/48; i++) {
                    for(int j = door_tly/48; j <= door_bry/48; j++) {
                        g.set_tile(i,j,19,false,0,0,0,0);
                    }
                }
                apple.x(warp_x);
                apple.y(warp_y);
                entity@ new_text = entity_by_id(new_text_id);
                entity@ old_text = entity_by_id(old_text_id);
                float temp_x = old_text.x();
                float temp_y = old_text.y();
                old_text.x(new_text.x());
                old_text.y(new_text.y());
                new_text.x(temp_x);
                new_text.y(temp_y);
            }
        }
    }
}