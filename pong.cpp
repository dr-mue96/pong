#include "gui.h"
#include "play.h"

using namespace std;
enum game_state { INIT, PLAY, GAMEOVER };
game_state state = INIT;
bool key_flag = true;

const char *start = "Press SPACE to start playing";
const char *over = "GAME OVER";
string points_str = "";
puck p;
bat bl, br;

void draw(int width, int height, cairo_t *cr) {					// anzeige
	cairo_select_font_face(cr, "Purisa", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 20);
	cairo_text_extents_t extents;
	cairo_set_line_width(cr, max(0.002*width,1.0));
	
	switch(state) {
		case INIT:
			// start
			cairo_set_source_rgb(cr, .5, .75, 1.);
			cairo_text_extents(cr, start, &extents);
			cairo_move_to(cr, width/2-extents.width/2, height/2);
			cairo_show_text(cr, start);
			break;
		case PLAY:
			cairo_set_source_rgb(cr, 1., 1., 1.);
			cairo_set_line_width(cr, bl.width()*width);

			// bats
			cairo_move_to(cr, bl.x()*width, bl.y()*height);
			cairo_line_to(cr, bl.x()*width, (bl.y()+bl.height())*height);
			cairo_move_to(cr, (br.x()+br.width())*width, br.y()*height);
			cairo_line_to(cr, (br.x()+br.width())*width, (br.y()+br.height())*height);
			cairo_stroke(cr);
			
			// puck
			draw_circle(cr, p.x()*width, p.y()*height, 
					(width<=height ? p.rad()*width : p.rad()*height));
			break;
		case GAMEOVER:
			// gameover
			cairo_set_source_rgb(cr, .5, .75, 1.);
			cairo_text_extents(cr, over, &extents);
			cairo_move_to(cr, width/2-extents.width/2, height/2);
			cairo_show_text(cr, over);
			
			// points
			cairo_text_extents(cr, points_str.c_str(), &extents);
			cairo_move_to(cr, width/2-extents.width/2, height/4);
			cairo_show_text(cr, points_str.c_str());
			break;
	}
}

void tick(int time_diff) {							// update, logik, physik, geschiwndigkeiten
	switch(state) {
		p_state where;
		case INIT:
			if(!key_pressed(' '))
				key_flag = true;
			if(key_pressed(' ') && key_flag) {
				state = PLAY;
				p.reset();
				bl.reset(0.3);
				br.reset(0.7);
			}
			break;
		case PLAY:
			p.move(time_diff);
			where = p.ingame();
			bl.move(time_diff, 'w', 's');
			br.move(time_diff, 62, 64);

			if(bl.deflect() || br.deflect()) {
				p.change_dir();
			}

			if(where == LEFT) {
				state = GAMEOVER;
				br.goal();
			} else if(where == RIGHT) {
				state = GAMEOVER;
				bl.goal();
			}
			break;
		case GAMEOVER:
			points_str = bl.score() + ":" + br.score();
			if(key_pressed(' ')) {
				key_flag = false;
				state = INIT;
			}
			break;
	}
	if(key_pressed(033))
		quit_gui();
}

int main(int argc, char *argv[])
{
	tick_callback = tick;
	draw_callback = draw;
	start_gui(700, 400);

return 0;
}
