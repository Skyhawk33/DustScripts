#include '../../../cmann/lib/input/Mouse.cpp'
#include '../../../cmann/lib/enums/GVB.cpp'
#include '../../../cmann/lib/math/Bezier.cpp';
#include '../../../cmann/lib/math/Line.cpp';

const int POINT_SIZE = 10;
const int LINE_SIZE = 3;
const uint PATH_COLOR = 0xFFFFFFFF;

const int HANDLE_SIZE = 5;
const int HANDLE_LINE_SIZE = 2;
const uint HANDLE_COLOR = 0xFFFF0000;

class script: callback_base {
    scene@ g;
    
    //--Play Vars--
    int num_players;
    array <dustman@> players;
    
    Line controllable_line;
    Line surface_line;
    
    //--Editor Vars--
    Mouse m(false);
    editor_api@ ed;
    Node@ selected = null;
    
    [hidden] NodePath np;
    [text] bool clear_nodes = false;

    script() {
        @g = get_scene();
        
        num_players = num_cameras();
        players.resize(num_players);
    }

    void step(int entities) {
        //if no players are defined, get the players
        if(@players[0] == null){
            for(uint i=0; i < players.length(); i++) {
                controllable@ c = controller_controllable(i);
                if(@c != null){
                    @players[i] = c.as_dustman();
                    players[i].set_collision_handler(this, "custom_collision", 0);
                    players[i].on_subframe_end_callback(this, "subframe_step", 0);
                }
            }
        }
    }
    
    void subframe_step(dustman@ dm, int) {
        //puts("\tSubframe End");
        if(dm.ground())
            dm.rotation(dm.ground_surface_angle()/2);
    }
    
    void custom_collision(controllable@ ec, tilecollision@ tc, int side, bool moving, float snap_offset, int arg) {
        
        if(side == SideType::ground) {
            float x, y, t;
            
            controllable_line.x1 = ec.x();
            controllable_line.x2 = ec.x();
            controllable_line.y1 = ec.y()-96;
            controllable_line.y2 = ec.y()+10;
            
            // find a surface to collide with
            Bezier@ surface;
            for(uint i = 0; i < np.segments(); i++) {
                if(np.collide_with_segment_bound(i, ec)) {
                    @surface = np.get_segment(i);
                    
                    for(int j = 0; j < surface.num_arcs; j++) {
                        surface_line.x1 = surface.x(float(j)/surface.num_arcs);
                        surface_line.y1 = surface.y(float(j)/surface.num_arcs);
                        surface_line.x2 = surface.x(float(j+1)/surface.num_arcs);
                        surface_line.y2 = surface.y(float(j+1)/surface.num_arcs);
                        
                        if(controllable_line.intersection(surface_line, x, y, t)) {
                            tc.hit(true);
                            tc.type(0);
                            tc.type(int(-atan2(surface_line.x2 - surface_line.x1,
                                                surface_line.y2 - surface_line.y1) * RAD2DEG + 90));
                            tc.hit_x(x);
                            tc.hit_y(y);
                            return;
                        }
                    }
                }
            }
        }
        
        ec.check_collision(@tc, side, moving, snap_offset);
    }
    
    void checkpoint_load() {
        for(uint i=0; i< players.length(); i++)
            @players[i] = null;
    }
    
    void editor_step() {
        if(@ed == null) {
            @ed = get_editor_api();
        }
        
        if(clear_nodes) {
            np.clear();
            clear_nodes = false;
            editor_sync_vars_menu();
        }
        
        m.step(ed.mouse_in_gui());
        
        if(m.left_press && ed.key_check_gvb(GVB::Shift) && !ed.key_check_gvb(GVB::Space)) {
            @selected = np.appendNode(m.x, m.y);
        }
        
        if(@selected != null && m.left_down && !ed.key_check_gvb(GVB::Space)) {
            selected.move_both_handles(m.x, m.y);
        }
    }
    
    void draw(float sub_frame) {
        np.draw(@g, false);
    }
    
    void editor_draw(float sub_frame) {
        np.draw(@g, true);
    }
}

namespace SideType {
    const int left = 0;
    const int right = 1;
    const int roof = 2;
    const int ground = 3;
}

class NodePath {
    [hidden] private array<Node> nodes;
    private Bezier@ draw_curve = Bezier();
    private Bezier@ curve = Bezier();
    
    void clear() {
        nodes.resize(0);
    }
    
    Node@ appendNode(float x, float y) {
        nodes.insertLast(Node(x, y));
        return nodes[nodes.length-1];
    }
    
    void draw(scene@ g, bool show_nodes) {
        for(uint i = 0; i < nodes.length; i++) {
            
            if(show_nodes)
                nodes[i].draw(@g);
            
            if(i > 0) {
                update_bezier(@draw_curve, nodes[i-1], nodes[i]);
                
                for(int j = 0; j < draw_curve.num_arcs; j++) {
                    g.draw_line_world(22, 0,
                                    draw_curve.x(float(j)/draw_curve.num_arcs),
                                    draw_curve.y(float(j)/draw_curve.num_arcs),
                                    draw_curve.x(float(j+1)/draw_curve.num_arcs),
                                    draw_curve.y(float(j+1)/draw_curve.num_arcs),
                                    LINE_SIZE, PATH_COLOR);
                }
            }
        }
    }
    
    uint segments() {
        return max(0.0, nodes.length-1);
    }
    
    Bezier@ get_segment(uint i) {
        if(i < 0 || i >= nodes.length-1)
            return null;
        
        update_bezier(@curve, nodes[i], nodes[i+1]);
        
        return curve;
    }
    
    bool collide_with_segment_bound(uint i, controllable@ c) {
        if(i < 0 || i >= nodes.length-1)
            return false;
        
        rectangle@ r = c.base_rectangle();
        
        float top = min(min(min(nodes[i].y(), nodes[i].handle_next_y()), nodes[i+1].handle_prev_y()), nodes[i+1].y());
        float bottom = max(max(max(nodes[i].y(), nodes[i].handle_next_y()), nodes[i+1].handle_prev_y()), nodes[i+1].y());
        float left = min(min(min(nodes[i].x(), nodes[i].handle_next_x()), nodes[i+1].handle_prev_x()), nodes[i+1].x());
        float right = max(max(max(nodes[i].x(), nodes[i].handle_next_x()), nodes[i+1].handle_prev_x()), nodes[i+1].x());
        
        return c.x()+r.left() < right && c.x()+r.right() > left && c.y()+r.top() < bottom && c.x()+r.bottom() > top;
    }
    
    private void update_bezier(Bezier@ b, Node@ n1, Node@ n2) {
        b.x1 = n1.x();
        b.y1 = n1.y();
        b.x2 = n1.handle_next_x();
        b.y2 = n1.handle_next_y();
        b.x3 = n2.handle_prev_x();
        b.y3 = n2.handle_prev_y();
        b.x4 = n2.x();
        b.y4 = n2.y();
    }
}

class Node {
    [hidden] private Point base;
    [hidden] private Point handle_prev;
    [hidden] private Point handle_next;
    
    Node() {}
    
    Node(float x, float y) {
        base = Point(x, y);
        handle_prev = Point(x, y);
        handle_next = Point(x, y);
    }
    
    void move_both_handles(float x_next, float y_next) {
        handle_next.set_xy(x_next, y_next);
        handle_prev.set_xy(base.x() - (x_next-base.x()), base.y() - (y_next-base.y()));
    }
    
    float x() {
        return base.x();
    }
    
    float y() {
        return base.y();
    }
    
    float handle_next_x() {
        return handle_next.x();
    }
    
    float handle_next_y() {
        return handle_next.y();
    }
    
    float handle_prev_x() {
        return handle_prev.x();
    }
    
    float handle_prev_y() {
        return handle_prev.y();
    }
    
    void draw(scene@ g) {
        base.draw(@g, POINT_SIZE, PATH_COLOR);
        handle_prev.draw(@g, HANDLE_SIZE, HANDLE_COLOR);
        handle_next.draw(@g, HANDLE_SIZE, HANDLE_COLOR);
        
        g.draw_line_world(22, 0,
                            base.x(), base.y(),
                            handle_next.x(), handle_next.y(),
                            HANDLE_LINE_SIZE, HANDLE_COLOR);
        
        g.draw_line_world(22, 0,
                            base.x(), base.y(),
                            handle_prev.x(), handle_prev.y(),
                            HANDLE_LINE_SIZE, HANDLE_COLOR);
    }
}

class Point {
    [hidden] private float x_pos;
    [hidden] private float y_pos;
    
    Point() {}
    
    Point(float x, float y) {
        x_pos = x;
        y_pos = y;
    }
    
    float x() {
        return x_pos;
    }
    
    float y() {
        return y_pos;
    }
    
    void set_xy(float x, float y) {
        x_pos = x;
        y_pos = y;
    }
    
    void draw(scene@ g, int size, uint color) {
        g.draw_rectangle_world(22, 1,
                                x_pos-size, y_pos-size,
                                x_pos+size, y_pos+size,
                                0, color);
    }
}
