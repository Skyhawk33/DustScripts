//const string EMBED_hexagon = "dandelion/prism.png";

const string SMALL_ENTITY = "enemy_tutorial_square";
const string EMBED_small0 = "anim/f1.png";
const string EMBED_small1 = "anim/f2.png";
const string EMBED_small2 = "anim/f3.png";
const string EMBED_small3 = "anim/f4.png";
const string EMBED_small4 = "anim/f5.png";
const string EMBED_small5 = "anim/f6.png";
const string EMBED_small6 = "anim/f7.png";
const string EMBED_small7 = "anim/f8.png";

const string LARGE_ENTITY = "enemy_tutorial_hexagon";
const string EMBED_large0 = "anim/f1.png";
const string EMBED_large1 = "anim/f2.png";
const string EMBED_large2 = "anim/f3.png";
const string EMBED_large3 = "anim/f4.png";
const string EMBED_large4 = "anim/f5.png";
const string EMBED_large5 = "anim/f6.png";
const string EMBED_large6 = "anim/f7.png";
const string EMBED_large7 = "anim/f8.png";

const int ANIMATE_LEN = 8;
const float ANIMATE_SPEED = 0.2;

const float ROTATE_SPEED = 0.4;

//leave this blank to not use a particle
const string EMBED_particle = "";
const int NUM_PARTICLES = 50;
const int PARTICLE_WIDTH = 20;
const int PARTICLE_HEIGHT = 40;

const int LOAD_DIST = 10000;

class script: callback_base {
    scene@ g;
    sprites@ spr;
    controllable@ player;
    [text] array<EntityData> enemies(0);
    array<Particle@> particles(0);

    script() {
        @g = get_scene();
        @spr = create_sprites();
    }
    
    void build_sprites(message@ msg) {
        build_spriteset(@msg, "small", 8, 96);
        build_spriteset(@msg, "large", 8, 96);
        
        if(EMBED_particle != "") {
            msg.set_string("particle","particle");
            msg.set_int("particle|offsetx",PARTICLE_WIDTH/2);
            msg.set_int("particle|offsety",PARTICLE_HEIGHT/2);
        }
    }
    
    void build_spriteset(message@ msg, string name, uint len, int offset) {
        for (uint i = 0; i < len; i++) {
            msg.set_string(name + i, name + i);
            msg.set_int(name + i + "|offsetx", offset);
            msg.set_int(name + i + "|offsety", offset);
        }
    }
    
    void on_level_start() {
        spr.add_sprite_set("script");
    }
    
    void step(int entities) {
        if(@player == null){
            @player = controller_controllable(0);
        }
        
        init_enemies();
        
        for(uint i = 0; i < enemies.length(); i++) {
            enemies[i].step();
        }
        
        for(int i = particles.length()-1; i >= 0; i--) {
            particles[i].step();
            if( particles[i].expired() ) {
                particles.removeAt(i);
            }
        }
    }
    
    void draw(float subframe) {
        entity@ e;
        for(uint i = 0; i < enemies.length(); i++) {
            enemies[i].draw(@spr);
        }
        
        for(int i = particles.length()-1; i >= 0; i--) {
            particles[i].draw();
        }
    }
    
    void entity_on_remove(entity@ e) {
        if(EMBED_particle == "")
            return;
        
        bool in_array = false;
        for(uint i = 0; i < enemies.length() && !in_array; i++) {
            in_array = (enemies[i].id == e.id());
        }
        
        if (in_array) {
            for(int i = 0; i < NUM_PARTICLES; i++) {
                particles.insertLast(Particle(e.x(), e.y(), @spr, "particle"));
            }
        }
    }
    
    void init_enemies() {
        entity@ e;
        for(uint i = 0; i < enemies.length(); i++) {
            if(!enemies[i].processed) {
                @e = entity_by_id(enemies[i].id);
                if(@e != null && abs(e.x()-player.x()) < LOAD_DIST && abs(e.y()-player.y()) < LOAD_DIST) {
                    enemies[i].process(@e);
                    e.set_sprites(@spr);
                }
            }
        }
    }
    
    void checkpoint_load() {
        @player = null;
        
        for(uint i = 0; i < enemies.length(); i++) {
            enemies[i].processed = false;
        }
    }
}

class EntityData {
    [entity] uint id;
    string sprite_set = "";
    bool is_rotating;
    float rotation;
    float animation_timer;
    bool processed = false;
    
    void process(entity@ e) {
        if(e.type_name() == LARGE_ENTITY) {
            sprite_set = "large";
            is_rotating = false;
        }
        else if(e.type_name() == SMALL_ENTITY) {
            sprite_set = "small";
            is_rotating = true;
        }
        else
            sprite_set = "";
        
        if (is_rotating)
            rotation = rand() % 360 - 180;
        
        animation_timer = rand() % ANIMATE_LEN;
        
        processed = true;
    }
    
    void step() {
        if(processed) {
            if (is_rotating)
                rotation = (rotation + ROTATE_SPEED + 180) % 360 - 180;
            animation_timer = (animation_timer + ANIMATE_SPEED) % ANIMATE_LEN;
        }
    }
    
    void draw(sprites@ spr) {
        if(processed) {
            entity@ e = entity_by_id(id);
            if(@e != null) {
                spr.draw_world(18, 8, sprite_set + int(animation_timer), 0, 0,
                                e.x(), e.y(), rotation, 1, 1, 0xFFFFFFFF);
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