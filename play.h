#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include "gui.h"

enum p_state { LEFT, RIGHT, IN };

class puck {
	private:
		float x_pos = 0.5;
		float y_pos = 0.5;
		float r = 0.015;
		float s = 0.4;
		float x_dir, y_dir;
		
	public:
		puck() {
			float angle = init_dir();
			x_dir = cos(angle);
			y_dir = sin(angle);
		}

		const float x() { return x_pos; }
		const float y() { return y_pos; }
		const float rad() { return r; }
		const float dx() { return x_dir; }

		const float init_dir() {
			float angle;
			do angle = random_number() * 2 * M_PI;
			while ((angle > M_PI/3 && angle < 2./3*M_PI) || (angle > 4./3*M_PI && angle < 5./3*M_PI));
			return angle;
		}

		void reset() {
			x_pos = 0.5;
			y_pos = 0.5;
			r = 0.015;
			s = 0.4;
			float angle = init_dir();
			x_dir = cos(angle);
			y_dir = sin(angle);
		}

		void  move(float speed) {
			/*
			if(x_pos + r >= 1 || x_pos - r <= 0)
				x_dir = -x_dir;
			*/

			if(y_pos + r >= 1 || y_pos - r <= 0) {
				y_dir = -y_dir;
				s *= 1.02;
			}
		
			x_pos += x_dir * s*speed/1000;
			y_pos += y_dir * s*speed/1000;

		}

		void change_dir() {
			x_dir = -x_dir;
			s *= 1.02;
		}
			
	
		const p_state ingame() {	
			if(x_pos + r >= 1)
				return RIGHT;
			else if(x_pos - r <= 0)
				return LEFT;
			else
				return IN;
		}


};

extern puck p;

class bat {
	private:
		float x_pos;
		float y_pos = 0.4;
	       	float w = 0.002;
		float h = 0.2;
		int points = 0;

	public:
		bat() {}
		bat(float x) : x_pos(x) {}

		const float x() { return x_pos; }
		const float y() { return y_pos; }
		const float width() { return w; }
		const float height() { return h; }
		const std::string score() { return std::to_string(points); }

		void reset(float x) {
			x_pos = x;
			y_pos = 0.4;
			w = 0.002;
			h = 0.2;
		}

		void goal() {
			points++;
		}
		
		void move(float speed, char up, char down) {
			if(key_pressed(up) && y_pos > 0.)
				y_pos -= speed/1000;
			if(key_pressed(down) && y_pos < 1-h)
				y_pos += speed/1000;
		}

		const bool deflect() {
			static bool flag = false; // change dir only once
		
			if(p.dx() < 0 && x_pos < 0.5) { // left bat
				if(p.x() < x_pos) {
					flag = true;
				} else if(p.x()-p.rad() <= x_pos+w && p.y()+p.rad() > y_pos && p.y()-p.rad() < y_pos+h) {
					flag = true;
					return true;
				} else {
					flag = false;
				}
			} else if(p.dx() > 0 && x_pos > 0.5) { // right bat
				if(p.x() > x_pos+w) {
					flag = true;
				} else if(p.x()+p.rad() >= x_pos && p.y()+p.rad() > y_pos && p.y()-p.rad() < y_pos+h) {
					flag = true;
					return true;
				} else {
					flag = false;
				}
			}
			return false;
		}
};
