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
	float b = 80; // boundary 한변의 길이의 절반
	float ex = 5; // 자동차는 boundary 밖에서 실행된다

	// 모델의 bounding box 중 적어도 한 점이 boundary 안에 존재한다.
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
	// 1. v_att에 모든 attribute copy
	vector<float> v_att; // vbo에 저장될 정보
	for (auto it = v_arr.begin(); it != v_arr.end(); it++) {
		// vertex 좌표  copy
		v_att.push_back((*it)[0]);
		v_att.push_back((*it)[1]);
		v_att.push_back((*it)[2]);
	}

	// 3, vector -> 배열
	float* v_att_arr = new float[v_att.size()];
	copy(v_att.begin(), v_att.end(), v_att_arr);

	// 4. 버퍼에 데이터 저장
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
		sizeof(float) * 3, //strip: 첫데이터에서 두 번째 데이터를 읽을때 얼마나 건너뛰어야 하는지
		0); // offset: buffer의 어디서 부터 읽을 것인지

	// 동적할당 해제
	delete[] v_att_arr;

}

// CarBody method
void CarBody::draw_car_body() {
	// carbody color 설정
	glUniform3f(color_loc, 0.0f, 0.0f, 1.0f); // blue color
	glUniform3f(m_ambient, 0.5f, 0.5f, 1.0f);
	glUniform3f(m_diffuse, 0.5f, 0.5f, 1.0f);
	glUniform3f(m_specular, 1.0f, 1.0f, 1.0f);
	glUniform1f(m_shineness, 32);
	
	model_view = glm::translate(model_view, glm::vec3(x_car, y_car, 0)); // CarBody frame -> Car frame
	model_view = glm::translate(model_view, glm::vec3(width / 2, height / 2, z_depth / 2)); // (0,0,0) = Model coordinate의 (x_min, y_min, z_min)이 되도록.
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
			glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map이 없는 것은 무조건 diffuse Mapping을 하지 않는다.
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
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map이 없는 것은 무조건 diffuse Mapping을 하지 않는다.
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
	model_view = glm::translate(model_view, glm::vec3(width / 2, height / 2, z_depth / 2)); // (0,0,0) = MC의 (x_min, y_min, z_min)이 되도록 설정.
	road_model.draw_model(width, height, z_depth);

	// glPopMatrix()
	model_view = matrix_stack.back();
	matrix_stack.pop_back();

	DrawStipple();
}

void Road::DrawStipple() { // 작은 사각형을 여러개 그리는 방식으로 점선을 그린다.

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
		model_view = glm::translate(model_view, glm::vec3(5.0f / 2, l_width / 2, st_depth / 2)); // (0,0,0) = MC의 (x_min, y_min, z_min)이 되도록 설정.
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
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map이 없는 것은 무조건 diffuse Mapping을 하지 않는다.
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
	model_view = glm::translate(model_view, glm::vec3(200 / 2, 10 / 2, 3 / 2)); // (0,0,0) = MC의 (x_min, y_min, z_min)이 되도록 설정.
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
			glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map이 없는 것은 무조건 diffuse Mapping을 하지 않는다.
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
		model_view = glm::translate(model_view, glm::vec3(width / 2, height / 2, z_depth / 2)); // (0,0,0) = MC의 (x_min, y_min, z_min)이 되도록 설정.
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
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map이 없는 것은 무조건 diffuse Mapping을 하지 않는다.
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

	// field 그리기
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
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map이 없는 것은 무조건 diffuse Mapping을 하지 않는다.
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

	// item 그리기
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
		glUniform1ui(glGetUniformLocation(shader, "diffuseToggle"), false); // diffuse Map이 없는 것은 무조건 diffuse Mapping을 하지 않는다.
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