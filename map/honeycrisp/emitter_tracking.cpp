#include '../../lib/entities.cpp';

class script {
    scene@ g;
    dustman@ player;
    [text] array<FollowGroup> groups;

    script() {
        @g = get_scene();
    }
    
    void on_level_start() {
        puts("LEVEL START");
        
        for(uint i=0; i < groups.length; i++) {
            groups[i].get_entities();
        }
    }
    
    bool printed = true;
    
    void step(int entities) {
        //if no player is defined, get the player
        if(@player == null) {
            controllable@ c = controller_controllable(0);
            if(@c != null) {
                @player = c.as_dustman();
            }
            
            if(@player == null)
                return;
        }
        
        for(uint i=0; i < groups.length; i++) {
            groups[i].move_followers();
        }
        
        if(!printed) {
            for(int i = 0; i < entities; i++) {
                if(entity_by_index(i).type_name() == "entity_emitter")
                    puts(entity_string(entity_by_index(i)));
            }
            printed = true;
        }
    }
    
    void entity_on_add(entity@ e) {
        if(e.type_name() == "hittable_apple")
            puts("ADDED APPLE");
    }
    
    void entity_on_remove(entity@ e) {
        if(e.type_name() == "hittable_apple")
            puts("REMOVED APPLE");
        
        if(e.id() > 0) {
            for(uint i=0; i < groups.length; i++) {
                if(groups[i].source_id == e.id()) {
                    puts("SOURCE REMOVED");
                    groups[i].remove_followers(g);
                }
            }
        }
    }
    
    void checkpoint_load() {
        //the player handle is no longer valid when a checkpoint loads
        @player = null;
        for(uint i=0; i < groups.length; i++) {
            groups[i].get_entities();
        }
    }
}

class FollowGroup {
    [entity] uint source_id;
    [text] array<int> follow_id;
    
    entity@ source;
    array<entity@> followers(0);
    
    void get_entities() {
        @source = entity_by_id(source_id);        
        
        entity@ follow;
        for(uint i = 0; i < follow_id.length; i++) {
            @follow = entity_by_id(follow_id[i]);
            if(@follow != null) {
                followers.insertLast(follow);
            }
        }
    }
    
    void move_followers() {
        if(@source != null) {
            for(uint i = 0; i < followers.length; i++) {
                followers[i].x(source.x());
                followers[i].y(source.y());
            }
        }
        else {
            puts("GONE");
        }
    }
    
    void remove_followers(scene@ g) {
        for(uint i = 0; i < followers.length; i++) {
            g.remove_entity(followers[i]);
        }
    }
}