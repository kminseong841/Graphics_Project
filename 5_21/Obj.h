#ifndef __OBJ_H__ 
#define __OBJ_H__

#include"loadobj.h"

using namespace std;

// for texture mapping
extern unsigned int car_diffuse;
extern unsigned int road_diffuse;
extern unsigned int road_normal;
extern unsigned int tree_diffuse;
extern unsigned int field_diffuse;
extern unsigned int field_normal;
extern unsigned int item_diffuse;
extern unsigned int agent_diffuse;

bool check_render(float cx, float cy, float x, float y, float w, float h); // system
void store_data(vector<vector<float>>& v_arr); // buffer에 vertex 저장

class CarBody {
private:
	float x_car, y_car, z_car; // car frame에서 x_min y_min, z_min
	float width, height, z_depth;
	int orientation;
	Model body_model;
public:
	CarBody() : x_car(0), y_car(0), width(0), height(0), orientation(0) {}
	CarBody(float x_i, float y_i, float z_i, float w_i, float h_i, float d_i, int o_i) : x_car(x_i), y_car(y_i), z_car(z_i),
		width(w_i), height(h_i), z_depth(d_i),
		orientation(o_i)
	{
		// 파일 읽고 모델 초기화
		string filepath = "cube.obj";
		body_model.read_file(filepath);
	}
	void draw_car_body();
	float get_x() { return x_car; };
	float get_y() { return y_car; };
	float get_z() { return z_car; };
	float get_w() { return width; };
	float get_h() { return height; };
	float get_d() { return z_depth; };
};

class Car {
private:
	float x, y; // min value(WC) of car
	float dx;
	float width, height, z_depth;
	float wheel_r;
	int orientation;
	CarBody bd;
	unsigned int diffuseMap = 0;
	unsigned int normalMap = 0;
public:
	Car() : x(0), y(0), dx(0), width(0), height(0), orientation(0) {}
	Car(float x_i, float y_i, float dx_i, float w_i, float h_i, float d_i, float r_i, int o_i) : x(x_i), y(y_i),
		dx(dx_i), width(w_i), height(h_i), z_depth(d_i),
		wheel_r(r_i), orientation(o_i),
		bd(0, 2 * r_i, 0, w_i, h_i - 2 * r_i, d_i, o_i) {
		// Texture load
		diffuseMap = car_diffuse;
	} 
	virtual void move_car();
	virtual void drawCar(float cx, float cy);
	void move_car_change_x() { x += orientation * dx; }
	void init_x() {
		if (orientation == 1)
			x = -100;
		else
			x = 100 - width;
	}

	float get_x() { return x; }
	float get_y() { return y; }
	float get_dx() { return dx; }
	float get_w() { return width; }
	float get_h() { return height; }
	float get_r() { return wheel_r; }
	int get_or() { return orientation; }
	CarBody* get_bd() { return &bd; }
};


class Road
{
private:
	float x_min, y_min, z_min, width, height, z_depth;
	Model road_model;
	unsigned int diffuseMap = 0;
	unsigned int normalMap = 0;
public:
	Road() {};
	Road(float ym)
	{
		y_min = ym;
	}
	Road(float x, float y, float z, float w, float h, float d)
	{
		// 파일 읽고 모델 초기화
		string filepath = "cube.obj";
		road_model.read_file(filepath);
		x_min = x;
		y_min = y;
		z_min = z;
		width = w;
		height = h;
		z_depth = d;

		// Texture load
		diffuseMap = road_diffuse;
		normalMap = road_normal;
	}
	float GetY() { return y_min; };
	void DrawRoad();
	void DrawStipple(); // 도로 점선 그리기
	void DrawEndline(); // End line 그리기
};


class Tree
{
private:
	float x_min, y_min, z_min, width, height, z_depth;
	Model tree_model;
	unsigned int diffuseMap = 0;
	unsigned int normalMap = 0;
public:
	Tree(float x, float y, float z, float w, float h, float d)
	{
		// 파일 읽고 모델 초기화
		string filepath = "cube.obj";
		tree_model.read_file(filepath);
		x_min = x;
		y_min = y;
		z_min = z;
		width = w;
		height = h;
		z_depth = d;

		// Texture load
		diffuseMap = tree_diffuse;
	}
	float GetX() { return x_min; };
	float GetY() { return y_min; };
	float GetZ() { return z_min; };
	float GetW() { return width; };
	float GetH() { return height; };
	float GetD() { return z_depth; };
	void DrawTree(float cx, float cy);
};


class Field
{
private:
	float y; // WC에서 들판이 그려질 y의 최솟값
	float dy; // 들판의 길이 
	float dz; // 들판의 높이
	Model field_model;
	unsigned int diffuseMap = 0;
	unsigned int normalMap = 0;
public:
	Field() {};
	Field(float y_i, float dy_i, float dz_i) : y(y_i), dy(dy_i), dz(dz_i)
	{
		// 파일 읽고 모델 초기화
		string filepath = "cube.obj";
		field_model.read_file(filepath);

		// Texture load
		diffuseMap = field_diffuse;
		normalMap = field_normal;
	}
	void DrawField();
};

class Item
{
private:
	float x_min, y_min, z_min, vel;
	bool show;
	Model item_model;
	unsigned int diffuseMap = 0;
	unsigned int normalMap = 0;
public:
	Item() {};
	Item(float xm, float ym, float zm, float v)
	{
		x_min = xm;
		y_min = ym;
		z_min = zm;
		vel = v;
		show = true;

		// 파일 읽고 모델 초기화
		string filepath = "cube.obj";
 		item_model.read_file(filepath);

		// Texture load
		diffuseMap = item_diffuse;
	}
	float GetX() { return x_min; };
	float GetY() { return y_min; };
	bool GetS() { return show; };
	void GetItem() { show = false; };
	void DrawItem(float cx, float cy);
};

class Agent
{
private:
	float st = 10.0f;
	float xm, ym, zm, w, h, d;
	bool cheat_mode = false; // 치트기능을 구현하는 변수
	bool is_shoes = false; // 신발을 획득했는지 확인하는 변수
	Model agent_model;
	unsigned int diffuseMap = 0;
	unsigned int normalMap = 0;
public:
	Agent() {};
	Agent(float x, float y, float z, float wid, float hei, float dep) {
		// 파일 읽고 모델 초기화
		string filepath = "cube.obj";
		agent_model.read_file(filepath);
		xm = x;
		ym = y;
		zm = z;
		w = wid;
		h = hei;
		d = dep;

		// Texture load
		diffuseMap = agent_diffuse;
	};
	void DrawAgent();
	void ChangeX(float d) { xm += d; };
	void ChangeY(float d) { ym += d; };
	void ChangeZ(float d) { zm += d; };
	void ChangeM() { cheat_mode = cheat_mode ? false : true; };
	float GetX() { return xm; };
	float GetY() { return ym; };
	float GetZ() { return zm; };
	float GetW() { return w; };
	float GetH() { return h; };
	float GetD() { return d; };
	bool GetM() { return cheat_mode; };
	bool GetS() { return is_shoes; };
	Model GetModel() { return agent_model; };
	void GetShoes() { is_shoes = true; };
};

#endif
