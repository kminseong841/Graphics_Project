#include "loadobj.h"

using namespace std;

extern GLint color_loc;
extern GLint model_view_loc;
extern GLint shineness_loc;
extern glm::mat4 model_view;

vector<float> my_strtok_f(char* str, char* delimeter) {
	vector<float> v;
	char* context;
	char* tok = strtok_s(str, delimeter, &context);

	while (tok != NULL) {
		v.push_back(atof(tok));
		tok = strtok_s(NULL, delimeter, &context);
	}
	return v;
}

vector<string> my_strtok_s(char* str, char* delimeter) {
	vector<string> v;
	char* context;
	char* tok = strtok_s(str, delimeter, &context);

	while (tok != NULL) {
		v.push_back(tok);
		tok = strtok_s(context, delimeter, &context);
	}
	return v;
}

vector<int> my_strtok_i(char* str, char* delimeter) {
	vector<int> v;
	char* context;
	char* tok = strtok_s(str, delimeter, &context);

	while (tok != NULL) {
		v.push_back(atoi(tok));
		tok = strtok_s(context, delimeter, &context);
	}
	return v;
}

// Obj private vector들을 push_back 하는 함수
void Obj::add_v(string& line, int len) {
	vector<float> v_tmp; // 읽은 vector의 주소를 저장할 포인터
	v_tmp = my_strtok_f((char*)line.substr(2, len - 2).c_str(), (char*)" ");
	v_arr.push_back(v_tmp);
}

void Obj::add_vn(string& line, int len) {
	vector<float> v_tmp; // 읽은 vector의 주소를 저장할 포인터
	v_tmp = my_strtok_f((char*)line.substr(3, len - 2).c_str(), (char*)" ");
	vn_arr.push_back(v_tmp);
}

void Obj::add_vt(string& line, int len) {
	vector<float> v_tmp; // 읽은 vector의 주소를 저장할 포인터
	v_tmp = my_strtok_f((char*)line.substr(3, len - 2).c_str(), (char*)" ");
	vt_arr.push_back(v_tmp);
}

void Obj::add_f(string& line, int len) {
	Face face_tmp;
	vector<string> s;
	vector<int> vi_tmp; // 읽은 int형 vector의 주소를 저장할 포인터

	s = my_strtok_s((char*)line.substr(2, len - 2).c_str(), (char*)" ");
	face_tmp.nVertexes = s.size();

	for (int i = 0; i < face_tmp.nVertexes; i++) {
		vi_tmp = my_strtok_i((char*)s[i].c_str(), (char*)"/");
		face_tmp.v_pairs_arr.push_back(vi_tmp);
	}
	f.push_back(face_tmp);
}

void Obj::draw_obj() {
	int nFaces = f.size(); // 면의 개수.
	int nVertex = v_arr.size();
	vector<float> v_att; // vbo에 저장될 정보

	// 1. 각 면 별로 해당 면을 이루는 vertex들의 attribute 정보 저장
	for (int k = 0; k < nFaces; k++) {
		int nPoints = f[k].v_pairs_arr.size(); // 한 면당 vertex수.
		for (int i = 0; i < nPoints; i++) {
			// vertex index update
			int v_id = f[k].v_pairs_arr[i][0]; // 특정 면의 한 vertex의 id 값.
			int vt_id = f[k].v_pairs_arr[i][1];
			int vn_id = f[k].v_pairs_arr[i][2];

			// vertex coordinate
			v_att.push_back(v_arr[v_id - 1][0]);
			v_att.push_back(v_arr[v_id - 1][1]);
			v_att.push_back(v_arr[v_id - 1][2]);
			// texture coordinate
			v_att.push_back(vt_arr[vt_id - 1][0]);
			v_att.push_back(vt_arr[vt_id - 1][1]);
			// normal vector
			v_att.push_back(vn_arr[vn_id - 1][0]);
			v_att.push_back(vn_arr[vn_id - 1][1]);
			v_att.push_back(vn_arr[vn_id - 1][2]);
		}
	}

	// 2. vector -> 배열
	float* v_att_arr = new float[v_att.size()];
	copy(v_att.begin(), v_att.end(), v_att_arr);

	// 3. 버퍼에 데이터 저장
	// buffer 생성
	unsigned int vao;
	glGenVertexArrays(1, &vao); //vao 생성
	glBindVertexArray(vao); //vao 바인딩 

	unsigned int vbo;
	glGenBuffers(1, &vbo); //vbo 생성
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //vbo 바인딩 
	glBufferData(GL_ARRAY_BUFFER, v_att.size() * sizeof(float), v_att_arr, GL_STATIC_DRAW);  //3. 작업 상태 버퍼에 데이터 전달

	//position attributes
	glEnableVertexAttribArray(0); //1. 몇 번째 Location의 attribute를 활성화(enable)
	glVertexAttribPointer(0, // 데이터 입력 location
		3, // 버퍼의 몇개의 데이터씩 읽을것인지
		GL_FLOAT, // 읽는 데이터 타입
		GL_FALSE,
		sizeof(float) * 8, //strip: 첫데이터에서 두 번째 데이터를 읽을때 얼마나 건너뛰어야 하는지
		0); // offset: buffer의 어디서 부터 읽을 것인지

	// Vertex Normal attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));

	// Texture Coordinate attributes;
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(5 * sizeof(float)));

	// uniform matrix 변경
	glUniformMatrix4fv(model_view_loc, 1, GL_FALSE, glm::value_ptr(model_view));

	// 그리기
	glDrawArrays(GL_TRIANGLES, 0, nFaces * 3);

	// 동적할당 해제
	delete[] v_att_arr;
}

void Model::add_Obj(string& line, int len) {
	Obj obj_tmp; // o가 읽히는 순간, obj_tmp 선언.
	obj_tmp.name = line.substr(2, len - 2);
	objs.push_back(obj_tmp);
	cnt += 1;
}

void Model::loadObj(ifstream& fin) {
	{
		string line;
		while (getline(fin, line)) {
			int len = line.length();

			// obj 파일 추가
			if (line[0] == 'o' && line[1] == ' ') {
				add_Obj(line, len);
			}

			// v, vn, vt, f 입력
			if (line[0] == 'v' && line[1] == ' ') {
				objs[cnt - 1].add_v(line, len);
			}
			else if (line[0] == 'v' && line[1] == 'n') {
				objs[cnt - 1].add_vn(line, len);
			}
			else if (line[0] == 'v' && line[1] == 't') {
				objs[cnt - 1].add_vt(line, len);
			}
			else if (line[0] == 'f' && line[1] == ' ') {
				objs[cnt - 1].add_f(line, len);
			}
		}
	}

	// update min and Max value per Model
	load_min_and_Max();
}

void Model::load_min_and_Max() {
	vector<float> x_arr;
	vector<float> y_arr;
	vector<float> z_arr;

	for (int i = 0; i < cnt; i++) {
		for (auto it = objs[i].v_arr.begin(); it != objs[i].v_arr.end(); it++) {
			x_arr.push_back((*it)[0]);
			y_arr.push_back((*it)[1]);
			z_arr.push_back((*it)[2]);
		}
	}

	x_min = *min_element(x_arr.begin(), x_arr.end());
	x_max = *max_element(x_arr.begin(), x_arr.end());
	y_min = *min_element(y_arr.begin(), y_arr.end());
	y_max = *max_element(y_arr.begin(), y_arr.end());
	z_min = *min_element(z_arr.begin(), z_arr.end());
	z_max = *max_element(z_arr.begin(), z_arr.end());
}

// Model coordinate에서 색상없이 그리는 함수
void Model::draw_model(float dx, float dy, float dz) {

	model_view = glm::scale(model_view, glm::vec3(dx / (x_max - x_min), dy / (y_max - y_min), dz / (z_max - z_min)));

	for (int j = 0; j < cnt; j++) {
		objs[j].draw_obj();
	}
}

void Model::read_file(string& filepath) {
	// 모델 읽기
	ifstream fin(filepath);

	loadObj(fin);

	fin.close();
}
