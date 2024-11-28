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

// Obj private vector���� push_back �ϴ� �Լ�
void Obj::add_v(string& line, int len) {
	vector<float> v_tmp; // ���� vector�� �ּҸ� ������ ������
	v_tmp = my_strtok_f((char*)line.substr(2, len - 2).c_str(), (char*)" ");
	v_arr.push_back(v_tmp);
}

void Obj::add_vn(string& line, int len) {
	vector<float> v_tmp; // ���� vector�� �ּҸ� ������ ������
	v_tmp = my_strtok_f((char*)line.substr(3, len - 2).c_str(), (char*)" ");
	vn_arr.push_back(v_tmp);
}

void Obj::add_vt(string& line, int len) {
	vector<float> v_tmp; // ���� vector�� �ּҸ� ������ ������
	v_tmp = my_strtok_f((char*)line.substr(3, len - 2).c_str(), (char*)" ");
	vt_arr.push_back(v_tmp);
}

void Obj::add_f(string& line, int len) {
	Face face_tmp;
	vector<string> s;
	vector<int> vi_tmp; // ���� int�� vector�� �ּҸ� ������ ������

	s = my_strtok_s((char*)line.substr(2, len - 2).c_str(), (char*)" ");
	face_tmp.nVertexes = s.size();

	for (int i = 0; i < face_tmp.nVertexes; i++) {
		vi_tmp = my_strtok_i((char*)s[i].c_str(), (char*)"/");
		face_tmp.v_pairs_arr.push_back(vi_tmp);
	}
	f.push_back(face_tmp);
}

void Obj::draw_obj() {
	int nFaces = f.size(); // ���� ����.
	int nVertex = v_arr.size();
	vector<float> v_att; // vbo�� ����� ����

	// 1. �� �� ���� �ش� ���� �̷�� vertex���� attribute ���� ����
	for (int k = 0; k < nFaces; k++) {
		int nPoints = f[k].v_pairs_arr.size(); // �� ��� vertex��.
		for (int i = 0; i < nPoints; i++) {
			// vertex index update
			int v_id = f[k].v_pairs_arr[i][0]; // Ư�� ���� �� vertex�� id ��.
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

	// 2. vector -> �迭
	float* v_att_arr = new float[v_att.size()];
	copy(v_att.begin(), v_att.end(), v_att_arr);

	// 3. ���ۿ� ������ ����
	// buffer ����
	unsigned int vao;
	glGenVertexArrays(1, &vao); //vao ����
	glBindVertexArray(vao); //vao ���ε� 

	unsigned int vbo;
	glGenBuffers(1, &vbo); //vbo ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //vbo ���ε� 
	glBufferData(GL_ARRAY_BUFFER, v_att.size() * sizeof(float), v_att_arr, GL_STATIC_DRAW);  //3. �۾� ���� ���ۿ� ������ ����

	//position attributes
	glEnableVertexAttribArray(0); //1. �� ��° Location�� attribute�� Ȱ��ȭ(enable)
	glVertexAttribPointer(0, // ������ �Է� location
		3, // ������ ��� �����;� ����������
		GL_FLOAT, // �д� ������ Ÿ��
		GL_FALSE,
		sizeof(float) * 8, //strip: ù�����Ϳ��� �� ��° �����͸� ������ �󸶳� �ǳʶپ�� �ϴ���
		0); // offset: buffer�� ��� ���� ���� ������

	// Vertex Normal attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));

	// Texture Coordinate attributes;
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(5 * sizeof(float)));

	// uniform matrix ����
	glUniformMatrix4fv(model_view_loc, 1, GL_FALSE, glm::value_ptr(model_view));

	// �׸���
	glDrawArrays(GL_TRIANGLES, 0, nFaces * 3);

	// �����Ҵ� ����
	delete[] v_att_arr;
}

void Model::add_Obj(string& line, int len) {
	Obj obj_tmp; // o�� ������ ����, obj_tmp ����.
	obj_tmp.name = line.substr(2, len - 2);
	objs.push_back(obj_tmp);
	cnt += 1;
}

void Model::loadObj(ifstream& fin) {
	{
		string line;
		while (getline(fin, line)) {
			int len = line.length();

			// obj ���� �߰�
			if (line[0] == 'o' && line[1] == ' ') {
				add_Obj(line, len);
			}

			// v, vn, vt, f �Է�
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

// Model coordinate���� ������� �׸��� �Լ�
void Model::draw_model(float dx, float dy, float dz) {

	model_view = glm::scale(model_view, glm::vec3(dx / (x_max - x_min), dy / (y_max - y_min), dz / (z_max - z_min)));

	for (int j = 0; j < cnt; j++) {
		objs[j].draw_obj();
	}
}

void Model::read_file(string& filepath) {
	// �� �б�
	ifstream fin(filepath);

	loadObj(fin);

	fin.close();
}
