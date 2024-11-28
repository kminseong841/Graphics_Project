#pragma once
class CarBody;

class User {
private:
public:
	float x, y; // 삼각형의 머리 부분 좌표
	float dx, dy;
	User() { x = 0.0, y = -95, dx = 5.0, dy = 5.0; }
	void drawUser(); // User을 그리는 함수
};

class Wheel {
private:
	float x_car, y_car; // car frame에서 바퀴의 x, y값
	float r;
	float theta, d_theta;
	int orientation;
public:
	Wheel() : x_car(0), y_car(0), r(0), theta(0), d_theta(0), orientation(0) {}
	Wheel(float x_i, float y_i, float r_i, float t_i, float dt_i, int o_i) : x_car(x_i), y_car(y_i), r(r_i), theta(t_i), d_theta(dt_i),
	orientation(o_i) {}
	void move_wheel();
	void init_theta() { // 맵 끝에서 차가 다시 리젠될때 사용
		theta = 0;
	}
	void draw_wheel();
};

class CarBody {
private:
	float x_car, y_car; // car frame에서 x_min, y_min 값.
	float width, height;
	int orientation;
public:
	CarBody() : x_car(0), y_car(0), width(0), height(0), orientation(0) {}
	CarBody(float x_i, float y_i, float w_i, float h_i, int o_i) : x_car(x_i), y_car(y_i), width(w_i), height(h_i),
	orientation(o_i) {}
	void draw_car_body();
	void draw_truck_body();
};


class Board {
private:
	float x_car, y_car; // car frame의 위치
	float length; // 상판의 길이
	float theta; // 상판의 initial 각도
	float d_theta; // 상판이 움직이는 정도
	int orientation;
	int sign;
	int range = 10; // 판자 이동 범위각
public:
	Board() : x_car(0), y_car(0), length(0), theta(0), d_theta(0), orientation(0), sign(0) {}
	Board(float x_i, float y_i, float l_i, float t_i, float dt_i, float o_i) : x_car(x_i),
		y_car(y_i), length(l_i), theta(t_i), d_theta(dt_i), orientation(o_i), sign(o_i) {}
	void move_board();
	void draw_board();
	void board_init_theta();
};


class Car {
private:
	float x, y; // min value of car
	float dx;
	float width, height;
	float wheel_r;
	int orientation; // 이동방향. 오른쪽->+1, 왼쪽->-1
	CarBody bd;
	Wheel w1, w2;
public:
	Car() : x(0), y(0), dx(0), width(0), height(0), wheel_r(0), orientation(0) {}
	Car(float x_i, float y_i, float dx_i, float w_i, float h_i, float r_i, int o_i) : x(x_i), y(y_i), dx(dx_i), width(w_i), height(h_i), wheel_r(r_i),
		orientation(o_i),
	bd(0, 2*r_i, w_i, h_i-2*r_i, o_i), w1(2*r_i, r_i, r_i, 0, 10* dx_i / r_i, o_i),
		w2(w_i - 2 * r_i, r_i, r_i, 0, 10*dx_i / r_i, o_i) {}
	virtual void move_car(); // 차 이동 구현
	virtual void drawCar();
	void move_car_change_x() { x += orientation * dx; }
	void init_x() { 
		if (orientation == 1)
			x = -100;
		else
			x = 100 - width;
	}

	// private variable을 얻는 함수
	float get_x() { return x; }
	float get_y() { return y; }
	float get_dx() { return dx; }
	float get_w() { return width; }
	float get_h() { return height; }
	float get_r() { return wheel_r; }
	int get_or() { return orientation; }
	CarBody* get_bd() { return &bd; }
	Wheel* get_w1() { return &w1; }
	Wheel* get_w2() { return &w2; }
};

class Truck : public Car {
private:
	Board truck_board;
public:
	Truck() {}
	Truck(float x_i, float y_i, float dx_i, float w_i, float h_i, float r_i, int o_i) : Car(x_i, y_i, dx_i, w_i, h_i, r_i, o_i),
		truck_board((w_i - (o_i * w_i)) / 2 + o_i* w_i * 5 / 6, 2*get_r()+ (h_i- 2 * get_r())*3/4, 5 * w_i / 6, 0, 0.1, o_i) {}
	void move_car();
	void drawCar();
};

class Tree {
private:
public:
	float x, y; // Tree x_min, y_min
	float width, height;
	Tree() : x(0), y(0), width(0), height(0) {}
	Tree(float x_i, float y_i, float w_i, float h_i) : x(x_i), y(y_i), width(w_i), height(h_i) {}
	void drawTree();
};
#pragma once
