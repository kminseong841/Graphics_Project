#include <cmath>
#include "shader.h"
#include "loadobj.h"
#include "Obj.h"


using namespace std;

//external gloobal variable
unsigned int shader;
GLint color_loc;
GLint model_view_loc;
GLint m_ambient;
GLint m_diffuse;
GLint m_specular;
GLint m_shineness;
glm::mat4 model_view; // model_view matrix.
vector<glm::mat4> matrix_stack;
bool diffuseMapMode = true; // diffuse Mapping check
bool normalMapMode = true;

// for texture mapping
unsigned int car_diffuse;
unsigned int road_diffuse;
unsigned int road_normal;
unsigned int tree_diffuse;
unsigned int field_diffuse;
unsigned int field_normal;
unsigned int item_diffuse;
unsigned int agent_diffuse;

//shader mode
bool shader_mode = true;

//directional light
float theta = 0;
float pi = 0;
const double PI = 3.14;

//For objects
float st = 10.0f;
int score = 0;
vector<Car*> cars;
vector<Tree> trees;
vector<Field> fields;
vector<Road> roads;
Item shoes;
Agent agent;
Road r1;
Road r2;
Road endline;

//Viewing parameters
int view = 0; //type of view mode (0: first, 1: third, 2: side)
bool hidden = false; //hidden line removal; do removal when true

// function prototype
bool isBlocked(vector<Tree>& t_vec);
bool isInnerPoint(float x_max, float x_min, float u_x_min, float u_x_max);
void Crashed();
void init_shader();

void display() {

	//player position
	GLfloat pos[6] = { agent.GetX(), agent.GetX() + agent.GetW(),
		agent.GetY(), agent.GetY() + agent.GetH(),
		agent.GetZ(), agent.GetZ() + agent.GetD() };
	GLfloat center_x = ((pos[1] - pos[0]) / 2) + pos[0];
	GLfloat center_y = ((pos[3] - pos[2]) / 2) + pos[2];
	GLfloat height = (pos[5] - pos[4]) / 2 + pos[4];

	//point light position
	unsigned int point = glGetUniformLocation(shader, "p_light.position");
	glUniform3f(point, center_x, center_y + 5, height);

	//camera positions
	GLfloat cam_p[3][3] = { {center_x, pos[2] - 50, height + 15},
							{center_x, pos[3] + 1, height},
							{center_x + 15, pos[2] - 25, height + 30} };
	GLfloat cam_d[3][3] = { {center_x, pos[2] + 55, 0},
							{center_x, pos[3] + 65, 0},
							{center_x - 10, pos[3] + 50, 0} };
	GLfloat cam_u[3][3] = { {0, 1, 2},
							{0, 6, 13},
							{0, 0, 1} };

	//camera positions
	glm::vec3 cam = glm::vec3(cam_p[view][0], cam_p[view][1], cam_p[view][2]);
	glm::vec3 cen = glm::vec3(cam_d[view][0], cam_d[view][1], cam_d[view][2]);
	glm::vec3 up = glm::vec3(cam_u[view][0], cam_u[view][1], cam_u[view][2]);

	//change view mode
	unsigned int view_mat = glGetUniformLocation(shader, "view");
	glm::mat4 view_matrix = glm::mat4(1.0f);
	view_matrix = glm::lookAt(cam, cen, up);
	glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view_matrix));

	unsigned int view_pos_loc = glGetUniformLocation(shader, "view_pos");
	glUniform3f(view_pos_loc, cam.x, cam.y, cam.z);

	// vc->cvc
	glm::mat4 projection_matrix = glm::perspective(glm::radians(40.0f), 4.0f / 3.0f, 0.1f, 200.0f);
	unsigned int pro_mat = glGetUniformLocation(shader, "proj");
	glUniformMatrix4fv(pro_mat, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (hidden) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	
	GLfloat cx = agent.GetX(), cy = agent.GetY();

	for (int i = 0; i < 2; i++) { //Cars
		cars[i]->drawCar(cx, cy);
	}

	for (int i = 0; i < 2; i++) {
		roads[i].DrawRoad();
	}

	// Draw Endline
	endline.DrawEndline();


	for (int i = 0; i < 3; i++) { // field
		fields[i].DrawField();
	}

	if (shoes.GetS())
		shoes.DrawItem(cx, cy);

	for (int i = 0; i < 4; i++) { //Tree
		trees[i].DrawTree(cx, cy);
	}

	for (int i = 2; i < 4; i++) { //Cars
		cars[i]->drawCar(cx, cy);
	}

	agent.DrawAgent();

	glFlush();
}

void MyKeyboard(unsigned char key, int X, int Y)
{
	// 강제 종료 키
	if (key == 'q') {
		exit(0);
	}

	// cheat mode
	if (key == 'p')
	{
		agent.ChangeM();
	}

	// view mode
	if (key == 'v')
	{
		view = (view + 1) % 3;
	}

	// rendering mode
	if (key == 'r')
	{
		hidden = !hidden;
	}

	// shading mode
	if (key == 'x')
	{
		if (shader_mode)
		{
			Shader source("ASSN4/5_21/gouraud.shader");
			shader = source.CreateShader();
			glUseProgram(shader);
			init_shader();
			shader_mode = false;
		}
		else
		{
			Shader source("ASSN4/5_21/phong.shader");
			shader = source.CreateShader();
			glUseProgram(shader);
			init_shader();
			shader_mode = true;
		}
	}

	// diffuse Map mode
	if (key == 't') {
		diffuseMapMode = !diffuseMapMode;
	}

	// normal Map mode
	if (key == 'n') {
		normalMapMode = !normalMapMode;
	}
}

void MySpecial(int key, int X, int Y)
{
	int speed = agent.GetS() ? 2 : 1;

	if (key == GLUT_KEY_UP) {
		agent.ChangeY(5.0f * speed);
		if (agent.GetY() >= 90) { //Endline 에 도달한 경우, 게임을 종료한다.
			cout << "You Win" << endl;
			auto c = cars.begin();
			while (c != cars.end()) {
				Car* temp = *c;
				c++;
				delete temp;
			}
			exit(0);
		}
		if (isBlocked(trees))
			agent.ChangeY(-5.0f * speed);
	}
	if (key == GLUT_KEY_DOWN) {
		// 맵 안에 있을 때만 키가 정상 작동한다.
		if (agent.GetY() > -90) 
			agent.ChangeY(-5.0f * speed);
		// 나무와 하나라도 충돌한 경우, 키 변화가 적용되지 않는다.
		if (isBlocked(trees))
			agent.ChangeY(5.0f * speed);
	}
	if (key == GLUT_KEY_LEFT) {
		if (agent.GetX() > -98)
			agent.ChangeX(-5.0f * speed);
		if(isBlocked(trees))
			agent.ChangeX(5.0f * speed);
	}
	if (key == GLUT_KEY_RIGHT) {
		if (agent.GetX() < 92)
			agent.ChangeX(5.0f * speed);
		if (isBlocked(trees))
			agent.ChangeX(-5.0f * speed);
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

void MyTimer2(int value) {
	//change direction of directional light
	if (theta >= PI)
		theta = 0;
	else
		theta += 1 * (PI / 180);
	pi = (theta > PI / 2) ? PI - theta : theta;

	unsigned int dir_light_dir = glGetUniformLocation(shader, "d_light.direction");
	glUniform3f(dir_light_dir, -cos(theta), -cos(pi), -sin(theta));

	glutTimerFunc(100, MyTimer2, 2);
}

void init_shader()
{
	// uniform location 저장, matrix identity.
	color_loc = glGetUniformLocation(shader, "object_color");
	model_view_loc = glGetUniformLocation(shader, "model_view");
	m_ambient = glGetUniformLocation(shader, "material.ambient");
	m_diffuse = glGetUniformLocation(shader, "material.diffuse");
	m_specular = glGetUniformLocation(shader, "material.specular");
	m_shineness = glGetUniformLocation(shader, "material.shineness");
	model_view = glm::mat4(1.0f); // Load identity

	//point light
	unsigned int plight_ambient = glGetUniformLocation(shader, "p_light.ambient");
	unsigned int plight_diffuse = glGetUniformLocation(shader, "p_light.diffuse");
	unsigned int plight_specular = glGetUniformLocation(shader, "p_light.specular");
	unsigned int linear_loc = glGetUniformLocation(shader, "p_light.linear");
	unsigned int quadratic_loc = glGetUniformLocation(shader, "p_light.quadratic");

	//when max range is 100
	glUniform1f(linear_loc, 0.045);
	glUniform1f(quadratic_loc, 0.0075);
	glUniform3f(plight_ambient, 0.5f, 0.5f, 0.5f);
	glUniform3f(plight_diffuse, 0.8f, 0.8f, 0.8f);
	glUniform3f(plight_specular, 1.0f, 1.0f, 1.0f);

	//directional light
	unsigned int dlight_ambient = glGetUniformLocation(shader, "d_light.ambient");
	unsigned int dlight_diffuse = glGetUniformLocation(shader, "d_light.diffuse");
	unsigned int dlight_specular = glGetUniformLocation(shader, "d_light.specular");

	glUniform3f(dlight_ambient, 0.2f, 0.2f, 0.2f);
	glUniform3f(dlight_diffuse, 0.6f, 0.6f, 0.6f);
	glUniform3f(dlight_specular, 0.8f, 0.8f, 0.8f);
}

void initFunc() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);

	int w = 30;
	int h = 15;
	int d = 15;
	float r = 3.5;

	// 모든 함수포인터 초기화: glew의 function들을 사용하기 전 반드시 사용해야한다.
	GLenum err = glewInit();

	Shader source("ASSN4/5_21/phong.shader");
	shader = source.CreateShader();
	glUseProgram(shader);
	init_shader();

	// init texture
	car_diffuse = loadTexture("obstacle2.jpg");
	road_diffuse = loadTexture("Ground_Dirt_009_BaseColor.jpg");
	road_normal = loadTexture("Ground_Dirt_009_Normal.jpg");
	tree_diffuse = loadTexture("tree.jpg");
	field_diffuse = loadTexture("grass.jpg");
	field_normal = loadTexture("Ground_Dirt_009_Normal.jpg");
	agent_diffuse = loadTexture("character.jpg");
	item_diffuse = loadTexture("grass.jpg");

	// init object
	cars.push_back(new Car(-100.0, 49.0, 2, w, h, d, r, 1)); // x_min, y_min, dx, dy, dz, radius, or
	cars.push_back(new Car(100.0 - w, 29, 3, w, h + 2, d, r, -1));
	cars.push_back(new Car(-100.0, -16, 1, w, h + 2, d, r, 1));
	cars.push_back(new Car(100.0 - w, -33, 0.5, w, h, d, r, -1));

	trees.push_back(Tree(-8 * st, st, 0.1 * st, 0.5 * st, st, st)); // x_min, y_min, z_min, w, h, d
	trees.push_back(Tree(-5 * st, st, 0.1 * st, 3 * st, st, st));
	trees.push_back(Tree(st, st, 0.1 * st, 1.5 * st, st, st));
	trees.push_back(Tree(5 * st, st, 0.1 * st, st, st, st));

	fields.push_back(Field(-10 * st, 6.8 * st, 0.5 * st)); // y_min(World Coordinate), dy, dz
	fields.push_back(Field(0.8 * st, 2.2 * st, 0.5 * st)); // y_min(World Coordinate), dy, dz
	fields.push_back(Field(7 * st, 2 * st, 0.5 * st)); // y_min(World Coordinate), dy, dz

	roads.push_back(Road(-100, 3 * st, -1, 200, 40, 1));
	roads.push_back(Road(-100, -3.2 * st, -1, 200, 40, 1));
	endline = Road(-100, 9 * st, -1, 200, 40, 1);

	shoes = Item(-6 * st, st, 0.1 * st, 0);
	agent = Agent(-0.3 * st, -9 * st, 0, 0.6 * st, 0.6 * st, 0.6 * st);

}

int main(int argc, char** argv)
{
	glutInit(&argc, argv); // 이거 왜 하는거지? 이거 안하면 도스 상에서 실행이 안된다고 함 (윈도우는 생략가능?)
	glutInitWindowPosition(100, 100); // 실행되는 시작 위치? 이거 바꾸면 어캐되는거지
	glutInitWindowSize(500, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH); // RGB 색상모드와 싱글버퍼 사용
	glutCreateWindow("OpenGL drawing Ex");
	glutSetWindowTitle("Crown"); 
	initFunc();


	glutDisplayFunc(display);
	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(MySpecial);
	glutTimerFunc(40, MyTimer, 1);
	glutTimerFunc(100, MyTimer2, 2);
	glutMainLoop();
	glDeleteProgram(shader); //셰이더 삭제
	return 0;
}

// 나무 충돌 점검
bool isBlocked(vector<Tree>& t_vec) {
	if (agent.GetM()) // cheat mode인 경우 충돌을 점검하지 않는다.
		return 0;

	bool inTree = 0;

	float agent_x_min = agent.GetX();
	float agent_x_max = agent.GetX() + agent.GetW();
	float agent_y_min = agent.GetY();
	float agent_y_max = agent.GetY() + agent.GetH();
	// 모든 tree들에 대해 들어가 있는지 확인한다
	for (auto t = t_vec.begin(); t != t_vec.end(); t++) {
		float x_min = t->GetX();
		float x_max = t->GetW() + x_min;
		float y_min = t->GetY();
		float y_max = t->GetH() + y_min;
		
		// 충돌 점검
		if (isInnerPoint(x_max, x_min, agent_x_min, agent_x_max) &&
			isInnerPoint(y_max, y_min, agent_y_min, agent_y_max)) {
			inTree = 1;
		}
	}
	return inTree;
}

// 내부의 점인지 확인
bool isInnerPoint(float x_max, float x_min, float u_x_min, float u_x_max) {
	if ((x_min <= u_x_min && u_x_min <= x_max) || (x_min <= u_x_max && u_x_max <= x_max)) {
		return true;
	}
	return false;
}

// 충돌 점검
void Crashed()
{
	if (!agent.GetM()) {
		float agent_x_min = agent.GetX();
		float agent_x_max = agent.GetX() + agent.GetW();
		float agent_y_min = agent.GetY();
		float agent_y_max = agent.GetY() + agent.GetH();
		for (int i = 0; i < 4; i++) //check crash with car
		{
			float w = cars[i]->get_w();
			float h = cars[i]->get_h();
			float x_max = cars[i]->get_x() + w;
			float x_min = cars[i]->get_x();
			float y_max = cars[i]->get_y() + h;
			float y_min = cars[i]->get_y();
			if (isInnerPoint(x_max, x_min, agent_x_min, agent_x_max) &&
				isInnerPoint(y_max, y_min, agent_y_min, agent_y_max)) {
				cout << "Crashed" << endl;
				auto c = cars.begin();
				while (c != cars.end()) {
					Car* temp = *c;
					c++;
					delete temp;
				}
				exit(0);
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
