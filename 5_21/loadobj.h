#ifndef __LOADOBJ_H__ 
#define __LOADOBJ_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include<GL/glew.h>
#include<GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "texture.h"

using namespace std;

vector<float> my_strtok_f(char* str, char* delimeter);
vector<string> my_strtok_s(char* str, char* delimeter);
vector<int> my_strtok_i(char* str, char* delimeter);

class Face {
public:
	int nVertexes;
	vector<vector<int>> v_pairs_arr; // 한면을 이루는 
};

class Obj {
private:
	float position[5] = { 0, 0, 0, 0, 0 }; //xmin, ymin, width, height, zmin

public:
	Obj() {};
	Obj(float x, float y, float w, float h, float z) {
		position[0] = x; position[1] = y;
		position[2] = w; position[3] = h; position[4] = z;
	};
	string name;
	vector<vector<float>> v_arr;
	vector<vector<float>> vt_arr;
	vector<vector<float>> vn_arr;
	vector<Face> f;

	// file을 읽고, 이를 obj의 변수에 저장하는 함수
	void add_v(string& line, int len);
	void add_vn(string& line, int len);
	void add_vt(string& line, int len);
	void add_f(string& line, int len);

	// 모든 face를 그리는 함수
	void draw_obj();
	float* get_pos() { return position; }; //get information of postion
};

class Model {
public:
	int cnt = 0; // 한 파일에서 읽은 object의 수. 
	int x_min, x_max, y_min, y_max, z_min, z_max;
	vector<Obj> objs;
	void add_Obj(string& line, int len); // obj파일 안에 o를 읽는 함수
	void loadObj(ifstream& fin); // obj파일을 읽는 함수
	void load_min_and_Max();
	void draw_model(float dx, float dy, float dz); // dx, dy, dz의 크기로 Model coordinate에서 색상없이 그리는 함수
	void read_file(string& filepath);
};

#endif