class script {
    [entity] int entity_id;
    [text] int health;
    bool changed_health = false;
    scene@ g;

    script() {
        @g = get_scene();
    }
    
    void step(int entities) {
        if(not changed_health) {
            //puts(""+entity_by_id(entity_id));
            check_entity_health(entity_by_id(entity_id));
        }
    }
    
    void checkpoint_load() {
        changed_health = false;
        //puts("loading checkpoint");
    }
    
    void check_entity_health(entity@ e) {
        if(e != null && e.id() == entity_id ) {
            //puts("adding entity");
            if(not changed_health) {
                e.as_controllable().life(health);
                changed_health = true;
                //puts("changing health");
            }
        }
    }
}