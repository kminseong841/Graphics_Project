#include<iostream>
#include<GL/glut.h>
#include<vector>
#include"ASSN2.h"

using namespace std;

// ȭ�� ��ü
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
bool isInnerPoint(float x_max, float x_min, float y_max, float y_min, float u_x, float u_y); // point�� boundary condition �ȿ� ����ִ��� check.
bool isBlocked(vector<Tree>& t_vec);


int main(int argc, char** argv) {

	// window����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // �ε巯�� �ִϸ��̼� ����� ���� double buffer�� �̿�
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Cross_the_road");

	// ����, ��ĥ��� ����
	Init();

	// ������(�׸��� �׸��� �Լ�)
	glutDisplayFunc(display);

	// window ũ�� ��ȭ�� ���� ����
	glutReshapeFunc(MyReshape);

	// for animation
	glutTimerFunc(40, mytimer, 0);

	// keyboard �̵�
	glutSpecialFunc(specialkey);


	glutMainLoop();

	// ���� �Ҵ� ����
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

	// �ڵ��� 4�� ����
	int w = 30;
	int h = 25;
	float r = 3.5;
	cars.push_back(new Car(-100.0, 90 - h, 1, w, h, r, 1));
	cars.push_back(new Truck(100.0 - w, 45 - h, 2, w, h, r, -1));
	cars.push_back(new Truck(-100.0, -10 - h, 0.5, w, h, r, 1));
	cars.push_back(new Car(100.0 - w, -55 - h, 3, w, h, r, -1));


	// ���� 3�� ����
	for (int i = 0; i < 3; i++) {
		trees.push_back(Tree(-100 + 70 * i, -8, 20, 16));
	}
}

// ������ �Լ�
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	// map�׸���
	drawMap();
	// user �����
	ponix.drawUser();
	// �ڵ��� �׸���
	cars[0]->drawCar();
	cars[1]->drawCar();
	cars[2]->drawCar();
	cars[3]->drawCar();

	glutSwapBuffers();
}

// ��ǥ ��ȯ
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

// Ű �Է� ����
void specialkey(int key, int x, int y) {

	if (key == GLUT_KEY_LEFT) {
		if (ponix.x > -95) // �� ���� ����� ���ϰ�
			ponix.x -= ponix.dx;
		// �ϳ��� ���������, �ǵ�����.
		if (isBlocked(trees))
			ponix.x += ponix.dx;
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_RIGHT) {
		if (ponix.x < 95) // �� ���� ����� ���ϰ�
			ponix.x += ponix.dx;
		// �ϳ��� ���������, ����
		if (isBlocked(trees))
			ponix.x -= ponix.dx;
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_UP) {
		ponix.y += ponix.dy;
		// �� ������ ������ 
		if (ponix.y > 100) {
			exit(0);
		}
		// �ϳ��� ���������, ����
		if (isBlocked(trees))
			ponix.y -= ponix.dy;
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_DOWN) {
		if (ponix.y > -95) // �� ���� ����� ���ϰ�
			ponix.y -= ponix.dy;
		// �ϳ��� ���������, ����
		if (isBlocked(trees))
			ponix.y += ponix.dy;
		glutPostRedisplay();
	}
}

// �ִϸ��̼� ����
void mytimer(int value) {
	// �� �̵� ����
	cars[0]->move_car();
	cars[1]->move_car();
	cars[2]->move_car();
	cars[3]->move_car();

	glutPostRedisplay();
	glutTimerFunc(40, mytimer, 0);

	// �浹����
	for (auto c = cars.begin(); c != cars.end(); c++) {
		crash(*c, &ponix);
	}
}

// �׿� map�� �׸��� �Լ�
void drawMap() {
	// ����
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

	// ���� �� ����
	glColor3f(1, 1, 1); //���
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

	// Tree �׸��� (���� 20, ���� 16)
	// ���� �׸���
	for (auto t = trees.begin(); t != trees.end(); t++) {
		t->drawTree();
	}
}

// �浹 �� ���� ���� ����
void crash(Car* vec, User* user) {
	// user�� boundary point �� �ϳ��� car �ȿ� ���������� exit();
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

// boundary box�� ���ԵǴ��� Ȯ��
bool isInnerPoint(float x_max, float x_min, float y_max, float y_min, float u_x, float u_y) {
	if (u_x > x_min && u_x < x_max && u_y>y_min && u_y < y_max) {
		return true;
	}
	return false;
}

// ���� �浹 ����
bool isBlocked(vector<Tree>& t_vec) {
	bool inTree = 0;

	// ��� tree�鿡 ���� �� �ִ��� Ȯ���Ѵ�
	for (auto t = t_vec.begin(); t != t_vec.end(); t++) {
		inTree += isInnerPoint(t->x + t->width, t->x, t->y + t->height, t->y, ponix.x, ponix.y) ||
			isInnerPoint(t->x + t->width, t->x, t->y + t->height, t->y, ponix.x - 5, ponix.y - 5) ||
			isInnerPoint(t->x + t->width, t->x, t->y + t->height, t->y, ponix.x + 5, ponix.y - 5);
	}
	return inTree;
}


