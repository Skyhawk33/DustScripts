const string EMBED_hexagon = 'dandelion/prism.png';
const string EMBED_square = 'dandelion/prism2.png';
const string EMBED_particle = 'dandelion/particle.png';
const float SPEED = 0.4;
const int NUM_PARTICLES = 50;

class script: callback_base {
    scene@ g;
    sprites@ spr;
    bool init;
    float rotation = 0;
    array<int> enemy_ids(0);
    array<Particle@> particles(0);

    script() {
        @g = get_scene();
        @spr = create_sprites();
    }
    
    void build_sprites(message@ msg) {
        puts("BUILDING");
        msg.set_string("enemy_tutorial_hexagon","hexagon");
        msg.set_int("enemy_tutorial_hexagon|offsetx",96);
        msg.set_int("enemy_tutorial_hexagon|offsety",96);
        
        msg.set_string("enemy_tutorial_square","square");
        msg.set_int("enemy_tutorial_square|offsetx",96);
        msg.set_int("enemy_tutorial_square|offsety",96);
        
        msg.set_string("particle","particle");
        msg.set_int("particle|offsetx",10);
        msg.set_int("particle|offsety",20);
    }
    
    void on_level_start() {
        puts("LEVEL START");
        spr.add_sprite_set("script");
        
        g.override_stream_sizes(32, 12);
        init = false;
    }
    
    void step(int entities) {
        if(!init) {
            init_enemies(entities, "enemy_tutorial_hexagon");
            init_enemies(entities, "enemy_tutorial_square");
            g.override_stream_sizes(32, 8);
            init = true;
        }
        
        rotation = (rotation + SPEED + 180) % 360 - 180;
        
        for(int i = particles.length()-1; i >= 0; i--) {
            particles[i].step();
            if( particles[i].expired() ) {
                particles.removeAt(i);
            }
        }
    }
    
    void draw(float subframe) {
        entity@ e;
        for(uint i = 0; i < enemy_ids.length(); i++) {
            @e = entity_by_id(enemy_ids[i]);
            if(@e != null) {
                spr.draw_world(18, 8, e.type_name(), 0, 0, e.x(), e.y(), rotation, 1, 1, 0xFFFFFFFF);
            }
        }
        
        for(int i = particles.length()-1; i >= 0; i--) {
            particles[i].draw();
        }
    }
    
    void entity_on_remove(entity@ e) {
        bool in_array = false;
        for(uint i = 0; i < enemy_ids.length() && !in_array; i++) {
            in_array = (enemy_ids[i] == e.id());
        }
        
        if (in_array) {
            for(int i = 0; i < NUM_PARTICLES; i++) {
                
                particles.insertLast(Particle(e.x(), e.y(), @spr, "particle"));
            }
        }
    }
    
    
    void init_enemies(int entities, string type) {
        entity@ e;
        for(int i = 0; i < entities; i++) {
            @e = entity_by_index(i);
            if(e.type_name() == type) {
                e.set_sprites(spr);
                enemy_ids.insertLast(e.id());
            }
        }
    }
}

class Particle {
    sprites@ spr;
    string sprite;
    float px, py, pr;
    float vx, vy, vr;
    int age, max_age;
    
    Particle() {
        
    }
    
    Particle(float x, float y, sprites@ spritesheet, string sprite_name) {
        float v = rand()%1000000/200000.0-2.5;
        float dir = rand()%3600/1800.0*3.14159;
        float vy = rand()%1000000/100000.0-5;
        float vr = rand()%1000000/100000.0-5;
        int life = rand()%120+30;
        
        initialize(x, y, vr, v*cos(dir), v*sin(dir), vr, life, @spritesheet, sprite_name);
    }
    
    Particle(float x, float y, float r, float x_speed, float y_speed, float r_speed,
            int lifetime, sprites@ spritesheet, string sprite_name) {
        initialize(x, y, r, x_speed, y_speed, r_speed, lifetime, @spritesheet, sprite_name);
    }
    
    private void initialize(float x, float y, float r, float x_speed, float y_speed, float r_speed,
                            int lifetime, sprites@ spritesheet, string sprite_name) {
        px = x;
        py = y;
        pr = r;
        vx = x_speed;
        vy = y_speed;
        vr = r_speed;
        
        age = 0;
        max_age = lifetime;
        
        @spr = @spritesheet;
        sprite = sprite_name;
    }
    
    void step() {
        age++;
        px += vx;
        py += vy;
        pr = (pr + vr + 180) % 360 - 180;
    }
    
    void draw() {
        spr.draw_world(18, 3, sprite, 0, 0, px, py, pr, 1, 1, 0xFFFFFFFF);
    }
    
    bool expired() {
        return age > max_age;
    }
}