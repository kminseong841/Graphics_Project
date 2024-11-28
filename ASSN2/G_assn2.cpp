#include<iostream>
#include<GL/glut.h>
#include<vector>
#include "Obj.h"

using namespace std;

float st = 10.0f;
int score = 0;
vector<Car*> cars;
Item shoes = Item(-6 * st, st, 0);
Item h1 = Item(-9 * st, -10 * st, 0);
Item h2 = Item(-8 * st, -10 * st, 0);
Item h3 = Item(-7 * st, -10 * st, 0);
Item heart[3] = { h1, h2, h3 };
Agent agent = Agent();
Road r1 = Road(3 * st);
Road r2 = Road(-3.2 * st);
Road road_list[2] = { r1, r2 };
Tree tree1 = Tree(0.5 * st, st, -8 * st, st);
Tree tree2 = Tree(3 * st, st, -5 * st, st);
Tree tree3 = Tree(1.5 * st, st, st, st);
Tree tree4 = Tree(st, st, 5 * st, st);
Tree tree_list[4] = { tree1, tree2, tree3, tree4 };

void CheckTree(int* check_list)
{
	if (!agent.GetM())
	{
		for (int i = 0; i < 4; i++)
		{
			float min_x = tree_list[i].GetX();
			float min_y = tree_list[i].GetY();
			float tree_w = tree_list[i].GetW();
			float tree_h = tree_list[i].GetH();
			if (min_x <= agent.GetX() && agent.GetX() <= (min_x + tree_w))
			{
				if (min_y <= agent.GetY() && agent.GetY() <= (min_y + tree_h))
					check_list[0] = 1; // rightup
				if (min_y <= (agent.GetY() + agent.GetH()) && (agent.GetY() + agent.GetH()) <= (min_y + tree_h))
					check_list[1] = 1; // rightdown
			}
			if (min_x <= (agent.GetX() + agent.GetW()) && (agent.GetX() + agent.GetW()) <= (min_x + tree_w))
			{
				if (min_y <= agent.GetY() && agent.GetY() <= (min_y + tree_h))
					check_list[2] = 1; // leftup
				if (min_y <= (agent.GetY() + agent.GetH()) && (agent.GetY() + agent.GetH()) <= (min_y + tree_h))
					check_list[3] = 1; // leftdown
			}
		}
	}
}

void MyDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int i = 0; i < 2; i++) //Road
	{
		road_list[i].DrawRoad();
	}
	glEnable(GL_LINE_STIPPLE);
	GLushort pat = 0xaaaa;
	GLint fac = 7;
	glLineWidth(4.0f);
	glLineStipple(fac, pat);
	for (int i = 0; i < 2; i++)
	{
		glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 0.0);
		glVertex2f(100.0f, -14.0f + (i * 62.0f));
		glVertex2f(-100.0f, -14.0f + (i * 62.0f));
		glEnd();
	}
	glDisable(GL_LINE_STIPPLE);

	for (int i = 0; i < 4; i++) { //Tree
		tree_list[i].DrawTree();
	}

	glBegin(GL_POLYGON); //End LIne
	glColor3f(1.0, 1.0, 0.0);
	glVertex3f(100.0f, 100.0f, 0.0f);
	glVertex3f(-100.0f, 100.0f, 0.0f);
	glVertex3f(-100.0f, 90.0f, 0.0f);
	glVertex3f(100.0f, 90.0f, 0.0f);
	glEnd();

	for (int i = 0; i < 4; i++) { //Cars
		cars[i]->drawCar();
	}

	if(shoes.GetS())
		shoes.DrawItem();
	for (int i = 0; i < agent.GetLife(); i++)
	{
		heart[i].DrawItem();
	}
	glPopMatrix();
	glPushMatrix();
	agent.DrawAgent();
	glutSwapBuffers();
}

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

void MyKeyboard(unsigned char key, int X, int Y)
{
	if (key == 'p')
	{
		agent.ChangeM();
	}
	if (key == 'h')
	{
		agent.ChangeHt();
	}
}

void MySpecial(int key, int X, int Y)
{
	int check_list[4] = { 0, 0, 0, 0 };
	int speed = agent.GetS()? 2 : 1;
	if (agent.GetY() >= 90) { //Endline
		cout << "You Win" << endl << "Score: " << score << endl;
		auto c = cars.begin();
		while (c != cars.end()) {
			Car* temp = *c;
			c++;
			delete temp;
		}
		exit(0);
	}
	CheckTree(check_list);
	if (key == GLUT_KEY_UP) {
		if (check_list[1] || check_list[3]) {
			agent.ChangeY(-5.0f * speed);
		}
		else {
			agent.ChangeY(5.0f * speed);
		}
	}
	if (key == GLUT_KEY_DOWN && agent.GetY() > -90) {
		if (check_list[0] || check_list[2]) {
			agent.ChangeY(5.0f * speed);
		}
		else {
			agent.ChangeY(-5.0f * speed);
		}
	}
	if (key == GLUT_KEY_LEFT && agent.GetX() > -98) {
		if (check_list[0] || check_list[1]) {
			agent.ChangeX(5.0f * speed);
		}
		else {
			agent.ChangeX(-5.0f * speed);
		}
	}
	if (key == GLUT_KEY_RIGHT && agent.GetX() < 92) {
		if (check_list[2] || check_list[3]) {
			agent.ChangeX(-5.0f * speed);
		}
		else {
			agent.ChangeX(5.0f * speed);
		}
	}
}

bool isInnerPoint(float x_max, float x_min, float u_x_min, float u_x_max) {
	if ((x_min <= u_x_min && u_x_min <= x_max) || (x_min <= u_x_max && u_x_max <= x_max)) {
		return true;
	}
	return false;
}

void Crashed()
{	
	if (!agent.GetM())
	{
		for (int i = 0; i < 4; i++) //check crash with car
		{
			float w = cars[i]->get_w();
			float h = cars[i]->get_h();
			float x_max = cars[i]->get_x() + w;
			float x_min = cars[i]->get_x();
			float y_max = cars[i]->get_y() + h;
			float y_min = cars[i]->get_y();
			if (isInnerPoint(x_max, x_min, agent.GetX(), agent.GetX() + agent.GetW()) &&
				isInnerPoint(y_max, y_min, agent.GetY(), agent.GetY() + agent.GetH())) {
				if (!agent.GetSheild())
				{
					agent.ChangeLife();
					if (agent.GetLife() == 0)
					{
						cout << "Game Over" << endl;
						auto c = cars.begin();
						while (c != cars.end()) {
							Car* temp = *c;
							c++;
							delete temp;
						}
						exit(0);
					}
					cout << "Loose Life" << endl;
					agent.ChangeSheild();
				}
			}
		}
	}
	float min_x_shoes = shoes.GetX();
	float min_y_shoes = shoes.GetY();
	if (shoes.GetS()) {
		if ((min_x_shoes <= agent.GetX() && agent.GetX() <= (min_x_shoes + 4.0f)) || (min_x_shoes <= (agent.GetX() + agent.GetW()) && (agent.GetX() + agent.GetW()) <= (min_x_shoes + 4.0f)))
		{
			if ((min_y_shoes <= agent.GetY() && agent.GetY() <= (min_y_shoes + 4.0f)) || (min_y_shoes <= (agent.GetY() + agent.GetH()) && (agent.GetY() + agent.GetH()) <= (min_y_shoes + 4.0f)))
			{
				shoes.GetItem();
				agent.GetShoes();
			}
		}
	}
}

void MyTimer(int Value) {
	for (int i = 0; i < 4; i++)
	{
		cars[i]->move_car(); //Change car's location		
	}
	glutPostRedisplay();
	Crashed();
	glutTimerFunc(40, MyTimer, 1);
}

void MovePlayer(int value)
{
	if (!agent.GetS())
	{
		agent.ChangeRL();
		
	}
	glutTimerFunc(160, MovePlayer, 2);
}

void FastPlayer(int value)
{	
	if (agent.GetS())
	{
		agent.ChangeRL();
		
	}
	glutTimerFunc(80, FastPlayer, 3);
}

void SheildTimer(int value)
{
	if (agent.GetSheild())
		agent.ChangeSheild();
	glutTimerFunc(3200, SheildTimer, 4);
}

void initFunc() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

	int w = 30;
	int h = 15;
	float r = 3.5;
	cars.push_back(new Car(-100.0, 52.0, 2, w, h, r, 1));
	cars.push_back(new Truck(100.0 - w, 31, 3, w, h+2, r, -1));
	cars.push_back(new Truck(-100.0, -11, 1, w, h+2, r, 1));
	cars.push_back(new Car(100.0 - w, -31, 0.5, w, h, r, -1));
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("길건너 친구들");
	initFunc();
	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(MySpecial);
	glutTimerFunc(40, MyTimer, 1);
	glutTimerFunc(160, MovePlayer, 2);
	glutTimerFunc(80, FastPlayer, 3);
	glutTimerFunc(1600, SheildTimer, 4);
	glutMainLoop();
	return 0;
}
