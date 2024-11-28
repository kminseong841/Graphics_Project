#include "Obj.h"

extern unsigned int shader;
extern GLint color_loc;
extern GLint model_view_loc;
extern GLint m_ambient;
extern GLint m_diffuse;
extern GLint m_specular;
extern GLint m_shineness;
extern glm::mat4 model_view;
extern vector<glm::mat4> matrix_stack;
extern bool diffuseMapMode;
extern bool normalMapMode;

bool check_render(float cx, float cy, float x, float y, float w, float h) {
	float b = 80; // boundary �Ѻ��� ������ ����
	float ex = 5; // �ڵ����� boundary �ۿ��� ����ȴ�

	// ���� bounding box �� ��� �� ���� boundary �ȿ� �����Ѵ�.
	if ((x >= cx - b - ex && x <= cx + b + ex) ||
		(x + h >= cx - b - ex && x + h <= cx + b + ex)) {
		if ((y >= cy - b - ex && y <= cy + b + ex) ||
			(y + h >= cy - b - ex && y + h <= cy + b + ex)) {
			return true;
		}
	}
	return false;
}

void store_data(vector<vector<float>>& v_arr) {
	// 1. v_att�� ��� attribute copy
	vector<float> v_att; // vbo�� ����� ����
	for (auto it = v_arr.begin(); it != v_arr.end(); it++) {
		// vertex ��ǥ  copy
		v_att.push_back((*it)[0]);
		v_att.push_back((*it)[1]);
		v_att.push_back((*it)[2]);
	}

	// 3, vector -> �迭
	float* v_att_arr = new float[v_att.size()];
	copy(v_att.begin(), v_att.end(), v_att_arr);

	// 4. ���ۿ� ������ ����
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
		sizeof(float) * 3, //strip: ù�����Ϳ��� �� ��° �����͸� ������ �󸶳� �ǳʶپ�� �ϴ���
		0); // offset: buffer�� ��� ���� ���� ������

	// �����Ҵ� ����
	delete[] v_att_arr;

}

// CarBody method
void CarBody::draw_car_body() {
	// carbody color ����
	glUniform3f(color_loc, 0.0f, 0.0f, 1.0f); // blue color
	glUniform3f(m_ambient, 0.5f, 0.5f, 1.0f);
	glUniform3f(m_diffuse, 0.5f, 0.5f, 1.0f);
	glUniform3f(m_specular, 1.0f, 1.0f, 1.0f);
	glUniform1f(m_shineness, 32);
	
	model_view = glm::translate(model_view, glm::vec3(x_car, y_car, 0)); // CarBody frame -> Car frame
	model_view = glm::translate(model_view, glm::vec3(width / 2, height / 2, z_depth / 2)); // (0,0,0) = Model coordinate�� (x_min, y_min, z_min)�� �ǵ���.
	body_model.draw_model(width, height, z_depth);
}

// Car method
void Car::move_car() {

	if (orientation == 1) {
		move_car_change_x();
		if (x > 100 - width) {
			x = -100;
		}
	}

	else if (orientation == -1) {
		move_car_change_x();
		if (x < -100) {
			x = 100 - width;
		}
	}
}

void Car::drawCar(float cx, float cy) {
	if (check_render(cx, cy, x, y, width, height)) {

		// texture Toggle check
		if (diffuseMap != 0) {
			// update diffuseMapModepingMode
			glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), diffuseMapMode);
			// bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
		}
		else {
			glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map�� ���� ���� ������ diffuse Mapping�� ���� �ʴ´�.
		}

		// Normal Toggle check
		if (normalMap != 0) {
			glUniform1ui(glGetUniformLocation(shader, "normalToggle"), normalMapMode);
			// bind normal map
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap);
		}
		else {
			glUniform1ui(glGetUniformLocation(shader, "normalToggle"), false);
		}

		// glPushMatrix
		matrix_stack.push_back(model_view);

		// Car frame -> World frame
		model_view = glm::translate(model_view, glm::vec3(x, y, 0)); 
		bd.draw_car_body();

		// glPopMatrix()
		model_view = matrix_stack.back();
		matrix_stack.pop_back();
	}
}

void Road::DrawRoad()
{
	// texture Toggle check
	if (diffuseMap != 0) {
		// update diffuseMapModepingMode
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), diffuseMapMode);
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
	}
	else {
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map�� ���� ���� ������ diffuse Mapping�� ���� �ʴ´�.
	}

	// Normal Toggle check
	if (normalMap != 0) {
		glUniform1ui(glGetUniformLocation(shader, "normalToggle"), normalMapMode);
		// bind normal map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
	}
	else {
		glUniform1ui(glGetUniformLocation(shader, "normalToggle"), false);
	}

	glUniform3f(color_loc, 1.0f, 1.0f, 1.0f); // white color
	glUniform3f(m_ambient, 1.0f, 1.0f, 1.0f);
	glUniform3f(m_diffuse, 1.0f, 1.0f, 1.0f);
	glUniform3f(m_specular, 0.3f, 0.3f, 0.3f);
	glUniform1f(m_shineness, 20);

	// glPushMatrix
	matrix_stack.push_back(model_view);

	// MC->WC
	model_view = glm::translate(model_view, glm::vec3(x_min, y_min, z_min));
	model_view = glm::translate(model_view, glm::vec3(width / 2, height / 2, z_depth / 2)); // (0,0,0) = MC�� (x_min, y_min, z_min)�� �ǵ��� ����.
	road_model.draw_model(width, height, z_depth);

	// glPopMatrix()
	model_view = matrix_stack.back();
	matrix_stack.pop_back();

	DrawStipple();
}

void Road::DrawStipple() { // ���� �簢���� ������ �׸��� ������� ������ �׸���.

	glUniform3f(color_loc, 0.0f, 0.0f, 0.0f); // black color
	glUniform3f(m_ambient, 0.0f, 0.0f, 0.0f);
	glUniform3f(m_diffuse, 0.0f, 0.0f, 0.0f);
	glUniform3f(m_specular, 0.3f, 0.3f, 0.3f);
	glUniform1f(m_shineness, 20);

	float l_width = 4;
	float st_y_min = y_min + (height - l_width) / 2;
	float st_z_min = -1;
	float st_depth = 1.1;
	for (int i = 0; i < 20; i++) {
		string filepath = "cube.obj";
		Model stipple_model;
		stipple_model.read_file(filepath);

		float st_x_min = 10.0f * i - 100;

		// glPushMatrix
		matrix_stack.push_back(model_view);

		// MC->WC
		model_view = glm::translate(model_view, glm::vec3(st_x_min, st_y_min, st_z_min));
		model_view = glm::translate(model_view, glm::vec3(5.0f / 2, l_width / 2, st_depth / 2)); // (0,0,0) = MC�� (x_min, y_min, z_min)�� �ǵ��� ����.
		stipple_model.draw_model(5.0f, l_width, st_depth);

		// glPopMatrix()
		model_view = matrix_stack.back();
		matrix_stack.pop_back();

	}
}

void Road::DrawEndline() {
	// texture Toggle check
	if (diffuseMap != 0) {
		// update diffuseMapModepingMode
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), diffuseMapMode);
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
	}
	else {
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map�� ���� ���� ������ diffuse Mapping�� ���� �ʴ´�.
	}

	// Normal Toggle check
	if (normalMap != 0) {
		glUniform1ui(glGetUniformLocation(shader, "normalToggle"), normalMapMode);
		// bind normal map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
	}
	else {
		glUniform1ui(glGetUniformLocation(shader, "normalToggle"), false);
	}

	glUniform3f(color_loc, 1.0f, 1.0f, 0.0f); // white color
	glUniform3f(m_ambient, 1.0f, 1.0f, 0.5f);
	glUniform3f(m_diffuse, 1.0f, 1.0f, 0.5f);
	glUniform3f(m_specular, 0.3f, 0.3f, 0.3f);
	glUniform1f(m_shineness, 20);

	// glPushMatrix
	matrix_stack.push_back(model_view);

	// MC->WC
	model_view = glm::translate(model_view, glm::vec3(-100, 90, -0.5));
	model_view = glm::translate(model_view, glm::vec3(200 / 2, 10 / 2, 3 / 2)); // (0,0,0) = MC�� (x_min, y_min, z_min)�� �ǵ��� ����.
	road_model.draw_model(200, 10, 3);

	// glPopMatrix()
	model_view = matrix_stack.back();
	matrix_stack.pop_back();

}

void Tree::DrawTree(float cx, float cy)
{
	if (check_render(cx, cy, x_min, y_min, width, height)) {
		// texture Toggle check
		if (diffuseMap != 0) {
			// update diffuseMapModepingMode
			glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), diffuseMapMode);
			// bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
		}
		else {
			glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map�� ���� ���� ������ diffuse Mapping�� ���� �ʴ´�.
		}

		// Normal Toggle check
		if (normalMap != 0) {
			glUniform1ui(glGetUniformLocation(shader, "normalToggle"), normalMapMode);
			// bind normal map
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap);
		}
		else {
			glUniform1ui(glGetUniformLocation(shader, "normalToggle"), false);
		}

		glUniform3f(color_loc, 1.0f, 0.0f, 1.0f); // purple color
		glUniform3f(m_ambient, 1.0f, 0.5f, 1.0f);
		glUniform3f(m_diffuse, 1.0f, 0.5f, 1.0f);
		glUniform3f(m_specular, 0.3f, 0.3f, 0.3f);
		glUniform1f(m_shineness, 10);

		// glPushMatrix
		matrix_stack.push_back(model_view);

		// MC->WC
		model_view = glm::translate(model_view, glm::vec3(x_min, y_min, z_min));
		model_view = glm::translate(model_view, glm::vec3(width / 2, height / 2, z_depth / 2)); // (0,0,0) = MC�� (x_min, y_min, z_min)�� �ǵ��� ����.
		tree_model.draw_model(width, height, z_depth);

		// glPopMatrix()
		model_view = matrix_stack.back();
		matrix_stack.pop_back();
	}
}

void Field::DrawField() {

	// texture Toggle check
	if (diffuseMap != 0) {
		// update diffuseMapModepingMode
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), diffuseMapMode);
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
	}
	else {
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map�� ���� ���� ������ diffuse Mapping�� ���� �ʴ´�.
	}

	// Normal Toggle check
	if (normalMap != 0) {
		glUniform1ui(glGetUniformLocation(shader, "normalToggle"), normalMapMode);
		// bind normal map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
	}
	else {
		glUniform1ui(glGetUniformLocation(shader, "normalToggle"), false);
	}

	// field �׸���
	glUniform3f(color_loc, 0.0f, 1.0f, 0.0f); // green color
	glUniform3f(m_ambient, 0.5f, 1.0f, 0.5f);
	glUniform3f(m_diffuse, 0.5f, 1.0f, 0.5f);
	glUniform3f(m_specular, 0.5f, 0.5f, 0.5f);
	glUniform1f(m_shineness, 10);
	
	// glPushMatrix
	matrix_stack.push_back(model_view);

	// MC->WC
	model_view = glm::translate(model_view, glm::vec3(0, y, 0));
	model_view = glm::translate(model_view, glm::vec3(0, dy / 2, 0));
	field_model.draw_model(200, dy, dz);

	// glPopMatrix
	model_view = matrix_stack.back();
	matrix_stack.pop_back();
}

void Item::DrawItem(float cx, float cy)
{

	// texture Toggle check
	if (diffuseMap != 0) {
		// update diffuseMapModepingMode
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), diffuseMapMode);
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
	}
	else {
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map�� ���� ���� ������ diffuse Mapping�� ���� �ʴ´�.
	}

	// Normal Toggle check
	if (normalMap != 0) {
		glUniform1ui(glGetUniformLocation(shader, "normalToggle"), normalMapMode);
		// bind normal map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
	}
	else {
		glUniform1ui(glGetUniformLocation(shader, "normalToggle"), false);
	}

	// item �׸���
	int size = 4;
	if (check_render(cx, cy, x_min, y_min, size, size)) {
		glUniform3f(color_loc, 1.0f, 1.0f, 0.0f); // yellow color
		glUniform3f(m_ambient, 1.0f, 1.0f, 0.5f);
		glUniform3f(m_diffuse, 1.0f, 1.0f, 0.5f);
		glUniform3f(m_specular, 0.5f, 0.5f, 0.5f);
		glUniform1f(m_shineness, 20);
		
		// glPushMatrix
		matrix_stack.push_back(model_view);

		// MC->WC
		model_view = glm::translate(model_view, glm::vec3(x_min, y_min, z_min));
		model_view = glm::translate(model_view, glm::vec3(size / 2, size / 2, size / 2));
		item_model.draw_model(size, size, size);

		// glPopMatrix
		model_view = matrix_stack.back();
		matrix_stack.pop_back();
	}
}

void Agent::DrawAgent()
{

	// texture Toggle check
	if (diffuseMap != 0) {
		// update diffuseMapModepingMode
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), diffuseMapMode);
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
	}
	else {
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map�� ���� ���� ������ diffuse Mapping�� ���� �ʴ´�.
	}

	// Normal Toggle check
	if (normalMap != 0) {
		glUniform1ui(glGetUniformLocation(shader, "normalToggle"), normalMapMode);
		// bind normal map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
	}
	else {
		glUniform1ui(glGetUniformLocation(shader, "normalToggle"), false);
	}

	glUniform3f(color_loc, 1.0f, 0.0f, 0.0f); // red color
	glUniform3f(m_ambient, 1.0f, 0.5f, 0.5f);
	glUniform3f(m_diffuse, 1.0f, 0.5f, 0.5f);
	glUniform3f(m_specular, 0.5f, 0.5f, 0.5f);
	glUniform1f(m_shineness, 20);

	// glPushMatrix
	matrix_stack.push_back(model_view);

	// MC->WC
	model_view = glm::translate(model_view, glm::vec3(xm, ym, zm));
	model_view = glm::translate(model_view, glm::vec3(w / 2, h / 2, d / 2));

	agent_model.draw_model(w, h, d);

	// glPopMatrix
	model_view = matrix_stack.back();
	matrix_stack.pop_back();
}