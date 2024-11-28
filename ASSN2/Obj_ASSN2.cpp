#include<iostream>
#include<GL/glut.h>
#include"ASSN2.h"

using namespace std;

// User method
void User::drawUser() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x, y, 0);

	glBegin(GL_TRIANGLES); //비행기
	glColor3f(1.0, 0, 0);
	glVertex3f(0.0f, 0, 0.0f);
	glVertex3f(-5.0f, -5, 0.0f);
	glVertex3f(5.0f, -5, 0.0f);
	glEnd();

	glPopMatrix();
}

// Wheel method
void Wheel::move_wheel() {
	// orientation 1이면 오른쪽, -1이면 왼쪽
	theta -= orientation * d_theta;
}


void Wheel::draw_wheel() {
	glPushMatrix();
	glTranslatef(x_car, y_car, 0); // Wheel frame -> Car frame

	// wheel frame에서 바퀴 그리기
	glRotatef(theta, 0.0f, 0.0f, 1.0f); // (0,0,1)이 회전축
	
	// 원그리기 
	glBegin(GL_POLYGON); // Draw a Red 1x1 Square centered at origin
	glColor3ub(192, 192, 192);//silver-tire
	for (int i = 0; i < 200; i++)
	{
		float pi = 3.1416;
		float A = (i * 2 * pi) / 200;
		float x = r * cos(A);
		float y = r * sin(A);
		glVertex3f(x, y, 0);
	}
	glEnd();

	// 림 그리기
	glBegin(GL_LINES);
	glColor3ub(0, 0, 0);//black-rim: 바퀴 회전이 시각적으로 보이게

	glVertex3f(0, 0, 1);
	glVertex3f(3.5, 0, 1);

	glVertex3f(0, 0, 1);
	glVertex3f(0, 3.5, 1);

	glVertex3f(0, 0, 1);
	glVertex3f(-3.5, 0, 1);

	glVertex3f(0, 0, 1);
	glVertex3f(0, -3.5, 1);

	glEnd();

	glPopMatrix();
}

// CarBody method
void CarBody::draw_car_body() {
	glPushMatrix();
	glTranslatef(x_car, y_car, 0); // CarBody frame -> Car frame

	// 왼쪽아래 -> 왼쪽 위 -> 오른쪽 위 -> 오른쪽 아래
	glColor3f(0, 0, 1); // Blue color
	glBegin(GL_POLYGON);
	glVertex3f(0, 0, 0);
	glVertex3f(0, height, 0);
	glVertex3f(width, height, 0);
	glVertex3f(width, 0, 0);
	glEnd();

	glPopMatrix();
}

void CarBody::draw_truck_body() {
	int alpha = (width - (orientation * width)) / 2; // 이동 방향에 따른 조정 parameter
	glPushMatrix();
	glTranslatef(x_car + alpha, y_car, 0); // TruckBody frame -> Car frame
	

	// 왼쪽 아래서부터 시계방향으로 그리기
	glColor3f(1, 0, 0); // 빨간색 트럭
	glBegin(GL_POLYGON);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 3 * height / 4, 0);
	glVertex3f(width * orientation, 3 * height / 4, 0);
	glVertex3f(width * orientation, 0, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(5 * width * orientation / 6, 3 * height / 4, 0);
	glVertex3f(5 * width * orientation / 6, height, 0);
	glVertex3f(width * orientation, height, 0);
	glVertex3f(width * orientation, 3 * height / 4, 0);
	glEnd();

	glPopMatrix();
}

// Board method
void Board::move_board() {
	
	theta += -sign * d_theta;

	if (range > 0) {
		if (sign == 1) {
			if (theta <= -sign * range) {
				sign *= -1;
				range = 0;
			}
		}
		else {
			if (theta > -sign * range) {
				sign *= -1;
				range = 0;
			}
		}
	}
	else {
		if (sign == 1) {
			if (theta <= -sign * range) {
				sign *= -1;
				range = 10;
			}
		}
		else {
			if (theta >= -sign * range) {
				sign *= -1;
				range = 10;
			}
		}
	}
}

void Board::draw_board() {
	glPushMatrix();
	glTranslatef(x_car, y_car, 0); // Board frame -> Car frame, 이 x_car은 alpha parameter가 포함되어 있다.

	// Board frame에서 트럭 상판 그리기
	glRotatef(theta, 0, 0, 1);

	glBegin(GL_POLYGON);
	glColor3ub(255, 255, 255); //흰색 판자

	glVertex3f(0, 0, 0);
	glVertex3f(-orientation * length, 0, 0);
	glVertex3f(-orientation * length, 1, 0);
	glVertex3f(0, 1, 0);

	glEnd();

	glPopMatrix();
}

void Board::board_init_theta() {

}

// Car method
void Car::move_car() {
	// 바퀴 굴리기
	w1.move_wheel();
	w2.move_wheel();

	// 차가 오른쪽으로 이동하는 경우
	if (orientation == 1) {
		move_car_change_x();
		if (x > 100 - width) {
			x = -100;
			w1.init_theta();
			w2.init_theta();
		}
	}

	// 차가 왼쪽으로 이동하는 경우
	else if (orientation == -1) {
		move_car_change_x();
		if (x < -100) {
			x = 100 - width;
			w1.init_theta();
			w2.init_theta();
		}
	}
}

void Car::drawCar() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x, y, 0); // Car frame -> World frame

	// 자동차 생성
	bd.draw_car_body();
	w1.draw_wheel();
	w2.draw_wheel();

	glPopMatrix();
}

// Truck method
void Truck::move_car() {
	// 바퀴 굴리기
	get_w1()->move_wheel();
	get_w2()->move_wheel();

	// 판자 회전
	truck_board.move_board();

	// 차가 오른쪽으로 이동하는 경우
	if (get_or() == 1) {
		move_car_change_x();
		if (get_x() > 100 - get_w()) {
			init_x();
			get_w1()->init_theta();
			get_w2()->init_theta();
		}
	}

	// 차가 왼쪽으로 이동하는 경우
	else if (get_or() == -1) {
		move_car_change_x();
		if (get_x() < -100) {
			init_x();
			get_w1()->init_theta();
			get_w2()->init_theta();
		}
	}
}

void Truck::drawCar() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(get_x(), get_y(), 0); // Car frame -> World frame

	// 트럭 생성
	truck_board.draw_board();
	get_bd()->draw_truck_body();
	get_w1()->draw_wheel();
	get_w2()->draw_wheel();

	glPopMatrix();
}

// Tree method
void Tree::drawTree() {
	// 가로 20 세로 2y trees

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x, y, 0);

	// 왼쪽아래 -> 왼쪽 위 -> 오른쪽 위 -> 오른쪽 아래
	// 가로 width 세로 height 인 자동차
	glColor3i(139, 69, 19);
	glBegin(GL_POLYGON);
	glVertex3f(0, 0, 0);
	glVertex3f(0, height, 0);
	glVertex3f(width, height, 0);
	glVertex3f(width, 0, 0);
	glEnd();

	glPopMatrix();
}