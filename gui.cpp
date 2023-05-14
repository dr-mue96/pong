#include "gui.h"
#include <random>

/* Der Inhalt dieser Datei muss nicht nachvollzogen werden können, hier stehen die
 * Details um die einfache Verwendung im Spiel möglich zu machen.
 * Sie dürfen natürlich gern genauer reinlesen -- nur müssen Sie es nicht :)
 */

static gint64 timer_start = 0;
static gint64 last_time = 0;
void (*tick_callback)(int time_diff) = nullptr;
void (*draw_callback)(int width, int height, cairo_t *cr) = nullptr;

static gboolean tick(GtkWidget *widget, GdkFrameClock *clock, gpointer user_data) {
	if (timer_start == 0) {
		timer_start = gdk_frame_clock_get_frame_time(clock);
		last_time = 0;
		return TRUE;
	}
	gint64 time =  (gdk_frame_clock_get_frame_time(clock) - timer_start) / 1000;
	gint64 time_diff = time - last_time;

	tick_callback(time_diff);
	
	gtk_widget_queue_draw(widget);
	last_time = time;
	return TRUE;
}

static gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	GtkWidget *win = gtk_widget_get_toplevel(widget);
	int width, height;
	gtk_window_get_size(GTK_WINDOW(win), &width, &height);

	draw_callback(width, height, cr);
	return TRUE;
}

static int pressed[256];
/*
 *	gdk_keyval_name(event->keyval):	Up	Down
 *	event->keyval:			65362	65364
 *
 */

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	if (strlen(event->string) > 0) {
		pressed[event->string[0]] = 1;
		return TRUE;
	} else if (event->keyval > 0) {
		pressed[event->keyval - 65300] = 1;
		return TRUE;
	}
	return FALSE; // to fall through
}

static gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	if (strlen(event->string) > 0) {
		pressed[event->string[0]] = 0;
		return TRUE;
	} else if (event->keyval > 0) {
		pressed[event->keyval - 65300] = 0;
	}
	return FALSE;
}

bool key_pressed(char c) {
	return pressed[c] != 0;
}



void start_gui(int w, int h) {

	char *args[] = { (char*)"ui-prog", nullptr };
	char **argv = args;
	int argc = 1;
	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkCssProvider *css_prov = gtk_css_provider_new();
	gtk_css_provider_load_from_data(css_prov, "* { background-image:none; background-color:black; }", -1, nullptr);
	GtkStyleContext *style_ctx = gtk_widget_get_style_context(window);
	gtk_style_context_add_provider(style_ctx, GTK_STYLE_PROVIDER(css_prov), GTK_STYLE_PROVIDER_PRIORITY_USER);

	GtkWidget *darea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), darea);

	gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);

	g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(draw), NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(on_key_press), NULL);
	g_signal_connect(G_OBJECT(window), "key_release_event", G_CALLBACK(on_key_release), NULL);
	gtk_widget_add_tick_callback(window, tick, nullptr, nullptr);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), w, h); 
	gtk_window_set_title(GTK_WINDOW(window), "PG2 Arcade App");

	gtk_widget_show_all(window);
	gtk_main();
}

void quit_gui() {
	gtk_main_quit(); 
}

void draw_circle(cairo_t *cr, int x, int y, int rad) {
	cairo_translate(cr, x, y);
	cairo_new_path(cr);
	cairo_arc(cr, 0, 0, rad, 0, 2 * M_PI);
	cairo_stroke(cr);
	cairo_translate(cr, -x, -y);
}

float random_number() {
	static std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<> dist(0.0f, 1.0f);
	return dist(mt);
}

