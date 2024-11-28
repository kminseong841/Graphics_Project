#include<iostream>
#include<GL/glut.h>
#include<vector>
#include"ASSN2.h"

using namespace std;

// 화면 객체
User ponix;
vector<Car*> cars;
vector<Tree> trees;

void Init();
void display();
void MyReshape(int NewWidth, int NewHeight);
void specialkey(int key, int x, int y);
void mytimer(int value);
void drawMap();
void crash(Car* vec, User* user);
bool isInnerPoint(float x_max, float x_min, float y_max, float y_min, float u_x, float u_y); // point가 boundary condition 안에 들어있는지 check.
bool isBlocked(vector<Tree>& t_vec);


int main(int argc, char** argv) {

	// window설정
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // 부드러운 애니메이션 사용을 위해 double buffer을 이용
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Cross_the_road");

	// 배경색, 색칠모드 설정
	Init();

	// 렌더링(그림을 그리는 함수)
	glutDisplayFunc(display);

	// window 크기 변화에 따른 대응
	glutReshapeFunc(MyReshape);

	// for animation
	glutTimerFunc(40, mytimer, 0);

	// keyboard 이동
	glutSpecialFunc(specialkey);


	glutMainLoop();

	// 동적 할당 해제
	auto c = cars.begin();
	while (c != cars.end()) {
		Car* temp = *c;
		c++;
		delete temp;
	}

	return 0;
}


void Init() {
	glClearColor(0, 1, 0, 0);
	glShadeModel(GL_FLAT);

	// 자동차 4개 생성
	int w = 30;
	int h = 25;
	float r = 3.5;
	cars.push_back(new Car(-100.0, 90 - h, 1, w, h, r, 1));
	cars.push_back(new Truck(100.0 - w, 45 - h, 2, w, h, r, -1));
	cars.push_back(new Truck(-100.0, -10 - h, 0.5, w, h, r, 1));
	cars.push_back(new Car(100.0 - w, -55 - h, 3, w, h, r, -1));


	// 나무 3개 생성
	for (int i = 0; i < 3; i++) {
		trees.push_back(Tree(-100 + 70 * i, -8, 20, 16));
	}
}

// 렌더링 함수
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	// map그리기
	drawMap();
	// user 만들기
	ponix.drawUser();
	// 자동차 그리기
	cars[0]->drawCar();
	cars[1]->drawCar();
	cars[2]->drawCar();
	cars[3]->drawCar();

	glutSwapBuffers();
}

// 좌표 변환
void MyReshape(int NewWidth, int NewHeight) {
	GLfloat nRange = 100.0f;
	if (NewHeight == 0) NewHeight = 1;
	glViewport(0, 0, NewWidth, NewHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (NewWidth <= NewHeight)
		glOrtho(-nRange, nRange, -nRange * NewHeight / NewWidth,
			nRange * NewHeight / NewWidth, -nRange, nRange);
	else
		glOrtho(-nRange * NewWidth / NewHeight, nRange * NewWidth / NewHeight, -nRange,
			nRange, -nRange, nRange);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// 키 입력 구현
void specialkey(int key, int x, int y) {

	if (key == GLUT_KEY_LEFT) {
		if (ponix.x > -95) // 맵 밖을 벗어나지 못하게
			ponix.x -= ponix.dx;
		// 하나라도 들어있으면, 되돌린다.
		if (isBlocked(trees))
			ponix.x += ponix.dx;
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_RIGHT) {
		if (ponix.x < 95) // 맵 밖을 벗어나지 못하게
			ponix.x += ponix.dx;
		// 하나라도 들어있으면, 종료
		if (isBlocked(trees))
			ponix.x -= ponix.dx;
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_UP) {
		ponix.y += ponix.dy;
		// 끝 영역에 닿으면 
		if (ponix.y > 100) {
			exit(0);
		}
		// 하나라도 들어있으면, 종료
		if (isBlocked(trees))
			ponix.y -= ponix.dy;
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_DOWN) {
		if (ponix.y > -95) // 맵 밖을 벗어나지 못하게
			ponix.y -= ponix.dy;
		// 하나라도 들어있으면, 종료
		if (isBlocked(trees))
			ponix.y += ponix.dy;
		glutPostRedisplay();
	}
}

// 애니메이션 구현
void mytimer(int value) {
	// 차 이동 구현
	cars[0]->move_car();
	cars[1]->move_car();
	cars[2]->move_car();
	cars[3]->move_car();

	glutPostRedisplay();
	glutTimerFunc(40, mytimer, 0);

	// 충돌감지
	for (auto c = cars.begin(); c != cars.end(); c++) {
		crash(*c, &ponix);
	}
}

// 그외 map을 그리는 함수
void drawMap() {
	// 도로
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex3f(-100, 90, 0);
	glVertex3f(100, 90, 0);
	glVertex3f(100, 10, 0);
	glVertex3f(-100, 10, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(-100, -10, 0);
	glVertex3f(100, -10, 0);
	glVertex3f(100, -90, 0);
	glVertex3f(-100, -90, 0);
	glEnd();

	// 도로 위 점선
	glColor3f(1, 1, 1); //흰색
	for (int i = 0; i < 10; i++) {
		glBegin(GL_POLYGON);
		glVertex3f(-100 + 20 * i, 55, 0);
		glVertex3f(-90 + 20 * i, 55, 0);
		glVertex3f(-90 + 20 * i, 45, 0);
		glVertex3f(-100 + 20 * i, 45, 0);
		glEnd();
	}
	for (int i = 0; i < 10; i++) {
		glBegin(GL_POLYGON);
		glVertex3f(-100 + 20 * i, -45, 0);
		glVertex3f(-90 + 20 * i, -45, 0);
		glVertex3f(-90 + 20 * i, -55, 0);
		glVertex3f(-100 + 20 * i, -55, 0);
		glEnd();
	}

	// Tree 그리기 (가로 20, 세로 16)
	// 나무 그리기
	for (auto t = trees.begin(); t != trees.end(); t++) {
		t->drawTree();
	}
}

// 충돌 시 게임 종료 구현
void crash(Car* vec, User* user) {
	// user의 boundary point 중 하나라도 car 안에 속해있으면 exit();
	float w = vec->get_w();
	float h = vec->get_h();
	float x_max = vec->get_x() + w;
	float x_min = vec->get_x();
	float y_max = vec->get_y() + h;
	float y_min = vec->get_y();
	if (isInnerPoint(x_max, x_min, y_max, y_min, user->x, user->y) ||
		isInnerPoint(x_max, x_min, y_max, y_min, user->x - 5, user->y - 5) ||
		isInnerPoint(x_max, x_min, y_max, y_min, user->x + 5, user->y - 5)) {
		exit(0);
	}
}

// boundary box에 포함되는지 확인
bool isInnerPoint(float x_max, float x_min, float y_max, float y_min, float u_x, float u_y) {
	if (u_x > x_min && u_x < x_max && u_y>y_min && u_y < y_max) {
		return true;
	}
	return false;
}

// 나무 충돌 점검
bool isBlocked(vector<Tree>& t_vec) {
	bool inTree = 0;

	// 모든 tree들에 대해 들어가 있는지 확인한다
	for (auto t = t_vec.begin(); t != t_vec.end(); t++) {
		inTree += isInnerPoint(t->x + t->width, t->x, t->y + t->height, t->y, ponix.x, ponix.y) ||
			isInnerPoint(t->x + t->width, t->x, t->y + t->height, t->y, ponix.x - 5, ponix.y - 5) ||
			isInnerPoint(t->x + t->width, t->x, t->y + t->height, t->y, ponix.x + 5, ponix.y - 5);
	}
	return inTree;
}


