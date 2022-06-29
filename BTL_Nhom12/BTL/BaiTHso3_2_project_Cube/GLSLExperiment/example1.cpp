/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat theta[] = { 0, 0, 0 };
GLfloat dr = 5;

mat4 model;
GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;
GLfloat mauAnhSang=2;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
mat4 instance;
void toMau(GLfloat a, GLfloat b, GLfloat c)
{
	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(0.0, 2.5, -2.5, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(mauAnhSang, mauAnhSang, mauAnhSang, 1.0);
	color4 light_specular(0.0, 0.0, 0.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(a/255.0,b/255.0, c/255.0, 1.0);
	color4 material_specular(1.0,1.0,1.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}

GLfloat width = 0.3, height =0.2, depth = 0.2;
mat4 model1;
mat4 model_t;
mat4 model2;
mat4 model3;
GLfloat rotateTau=0;

mat4 model4;
// Code phần kệ dạng cây để góc phòng. 
void ThanhGiua()   // cây trụ đứng 
{
	toMau(114, 222, 98);
	mat4 m = Scale(0.06, 1.5, 0.06);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model *model4* m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void thanhCanh(GLfloat x, GLfloat y, GLfloat z) // các thanh cạnh để đồ
{
	toMau(202, 232, 84);
	mat4 m = Translate(x, y, z) * Scale(0.5, 0.04, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model *model4* m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void ThanhNgang()// các thanh cạnh để đồ
{
	thanhCanh(-0.28, 0.65, 0);
	thanhCanh(0.28, 0.45, 0);
	thanhCanh(-0.28, 0.25, 0);
	thanhCanh(0.28, 0.05, 0);
	thanhCanh(-0.28, -0.25, 0);
	thanhCanh(0.28, -0.45, 0);
	thanhCanh(-0.28, -0.65, 0);
}
void BeKe() // phần bệ của kệ dạng cây
{
	toMau(94, 70, 51);
	mat4 m = Translate(0, -0.8, 0) * Scale(0.5, 0.1, 0.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model* model4* m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void KeTang()   // Kệ để đồ dạng cây.
{
	BeKe();
	ThanhGiua();
	ThanhNgang();
}
//ve oto
mat4 model_oto;
void thanOto(GLfloat x, GLfloat y, GLfloat z)//vẽ thân ô tô
{
	toMau(x,y,z);
	instance =  Scale(0.4, 0.2, 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4*model_oto*instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void cuaSoOto(GLfloat x, GLfloat y, GLfloat z)//vẽ cửa sổ ô tô
{
	toMau(255.0, 255.0, 0.0);
	instance = Translate(x, y, z) * Scale(0.1,0.1,0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4*model_oto * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void oTo(GLfloat x, GLfloat y, GLfloat z, GLfloat m, GLfloat n, GLfloat p)
{
	model_oto = Translate(m,n,p);
	thanOto(x,y,z);
	cuaSoOto(0.1, 0.05, -0.1);
	cuaSoOto(0.1, 0.05, 0.1);
}
// Code phần đèn bàn để kệ đầu tiên trên cùng.

void DeDen() // Phần chụp của đèn để bàn ( có chiếu sáng ).
{
	toMau(219, 104, 55);
	mat4 m = Translate(-0.25, 0.68, 0) * Scale(0.1, 0.02, 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ThanDen() // Phần thân của đèn để bàn (cây ở giữa đế đèn và chụp đèn) .
{
	toMau(240, 155, 70);
	mat4 m = Translate(-0.25, 0.79, 0) * Scale(0.02, 0.2, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ChupDen() // Phần chụp của đèn để bàn .
{
	toMau(44, 229, 232);
	mat4 m = Translate(-0.25, 0.89, 0) * Scale(0.16, 0.2, 0.16);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void DenBan()
{
	DeDen();
	ThanDen();
	ChupDen();
}

//  Code nhà đồ chơi để ở kệ thứ 2
void NhaTang1() // Khung nhà đồ chơi tầng 1
{
	toMau(10, 247, 105);
	mat4 m = Translate(0.15, 0.55, 0) * Scale(0.15, 0.15, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void CuaNhaTang1()  // Cửa của tầng 1 nhà đồ chơi
{
	toMau(222, 147, 7);
	mat4 m = Translate(0.15, 0.525, 0.075) * Scale(0.08, 0.1, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void CuaSo1Tang2()  // Cửa sổ trái của tầng 2 nhà đồ chơi
{
	toMau(227, 20, 220);
	mat4 m = Translate(0.115, 0.67, 0.07) * Scale(0.04, 0.05, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void CuaSo2Tang2()  // Cửa sổ phải của tầng 2 nhà đồ chơi
{
	toMau(227, 20, 220);
	mat4 m = Translate(0.185, 0.67, 0.07) * Scale(0.04, 0.05, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void NhaTang2() // Khung nhà đồ chơi tầng 2 
{
	toMau(23, 212, 3);
	mat4 m = Translate(0.15, 0.675, 0) * Scale(0.15, 0.12, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void NocNha() // Nóc nhà của nhà đồ chơi, nằm trên tầng 2 của nhà
{
	toMau(201, 27, 8);
	mat4 m = Translate(0.15, 0.73, 0) * Scale(0.15, 0.03, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ThanhRao(GLfloat x, GLfloat y, GLfloat z) // thanh rào ở nhà đổ màu trắng vào 
{
	toMau(255, 255, 255);
	mat4 m = Translate(x, y, z) * Scale(0.01, 0.05, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void HangRao() // Hang rào bên cạnh nhà ( đồ màu trắng )
{
	ThanhRao(0.235, 0.5, 0.07);
	ThanhRao(0.25, 0.5, 0.07);
	ThanhRao(0.265, 0.5, 0.07);
	ThanhRao(0.28, 0.5, 0.07);
	ThanhRao(0.295, 0.5, 0.07);
	ThanhRao(0.31, 0.5, 0.07);
	ThanhRao(0.325, 0.5, 0.07);
	ThanhRao(0.34, 0.5, 0.07);
	ThanhRao(0.355, 0.5, 0.07);
	ThanhRao(0.37, 0.5, 0.07);
}

void CotHienTruoc() // Cột của hiên nhà , cột trước
{
	toMau(240, 193, 26);
	mat4 m = Translate(0.385, 0.55, 0.07) * Scale(0.015, 0.15, 0.015);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void CotHienSau() // Cột của hiên nhà , cột trước
{
	toMau(240, 193, 26);
	mat4 m = Translate(0.385, 0.55, -0.07) * Scale(0.015, 0.15, 0.015);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void MaiHien() // mái hiên phía trên 2 cột.
{
	toMau(222, 219, 40);
	mat4 m = Translate(0.31, 0.62, 0) * Scale(0.165, 0.01, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model4 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void MaiHienNha()  // Mái hiên để xe . 
{
	HangRao();
	CotHienTruoc();
	CotHienSau();
	MaiHien();
}

void NhaDoChoi()//Nhà đồ chơi
{
	NhaTang1();
	CuaNhaTang1();
	NhaTang2();
	CuaSo1Tang2();
	CuaSo2Tang2();
	NocNha();
	MaiHienNha();
}

//vẽ rào an toàn
mat4 model8;
void thanh(GLfloat x, GLfloat y, GLfloat z)
{
	toMau(255, 255, 255);
	mat4 m = Translate(x, y, z) * Scale(0.05, 1.2, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model *model8* m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void hangRaoTruoc()
{
	
	GLfloat tam = -1;
	for (GLint i = 0; i < 9; i++)
	{
		tam += 0.2;
		thanh(tam, 0, 0.8);
	}
}

void hangRaoSau()
{
	GLfloat tam = -1;
	for (GLint i = 0; i < 9; i++)
	{
		tam += 0.2;
		thanh(tam, 0, -0.8);
	}
}

void hangRaoTrai()
{
	GLfloat tam = -0.8;
	for (GLint i = 0; i < 8; i++)
	{
		tam += 0.2;
		thanh(-0.8, 0, tam);
	}
}
void hangRaoPhai()
{
	GLfloat tam = -0.8;
	for (GLint i = 0; i < 8; i++)
	{
		tam += 0.2;
		thanh(0.8, 0, tam);
	}
}
void hangRao()
{
	hangRaoTruoc();
	hangRaoSau();
	hangRaoTrai();
	hangRaoPhai();

}


//vẽ điều hòa
mat4 model10;
void KhungDH() // Khung của điều hoà ( phần chính )
{
	toMau(255, 255, 255);
	mat4 m = Scale(1.2, 0.3, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model10*m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void KhungDuoi()  // Khung dưới chỗ thông gió
{
	toMau(255, 255, 255);
	mat4 m = Translate(0, -0.19, 0) * Scale(1.15, 0.02, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model10 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void KhungCanhTrai() // Khung cạnh trái chỗ thông gió
{
	toMau(255, 255, 255);
	mat4 m = Translate(-0.5875, -0.175, 0) * Scale(0.025, 0.05, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model10 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void KhungCanhPhai() // Khung cạnh phải chỗ thông gió
{
	toMau(255, 255, 255);
	mat4 m = Translate(0.5875, -0.175, 0) * Scale(0.025, 0.05, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * model10 * m);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void DieuHoa()
{
	model10 = Translate(2.45,2,0) * RotateY(90);
	KhungDH();
	KhungCanhTrai();
	KhungCanhPhai();
	KhungDuoi();
}


GLfloat eyeX = 0.0, eyeZ = -4.0,X1=0,Z1=0;
GLfloat X4 = 0, Z4 = 0;
GLfloat goc_ke_cay = 0;
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model = Translate(X1, 0, Z1)*RotateY(theta[0]);
	//kệ ngang trưng bày
	//tuNgang();
	//kệ dọc trưng bày
	//tuDoc();
	//Cửa hàng
	//veCuaHang();
	//Đèn đồ chơi
	//veDen();
	//Model4 để dùng cho kệ dạng cây
	model4 =Translate(2.5, -1.7, 1) * RotateY(270)* Translate(X4, 0, Z4) * RotateY(goc_ke_cay);
	//kệ dạng cây
	KeTang();
	//đèn
	DenBan();
	//nhà đồ chơi
	NhaDoChoi();
	//vẽ ô tô
	oTo(255, 42, 103, -0.25, -0.1, 0);
	oTo(30, 220, 103, -0.25, 0.4, 0);
	oTo(27, 94, 255, -0.25, -0.5, 0);
	oTo(27, 94, 255, 0.15, -0.2, 0);
	oTo(30, 220, 103, 0.15, 0.3, 0);
	//ban();
	//rô bốt trưng bày
	//Robot();
	//Vẽ hàng rào an toàn
	model8 = Translate(-0.3, -2, -0.5);
	hangRao();
	//Ghê xoay ngồi
	//ghe();
	//Điều hòa
	DieuHoa();
	//Đồng hồ
	//dongho();
	vec4 eye(eyeX, 1.25, eyeZ, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);
	projection = Frustum(-3, 3, -3, 3, 3, 12);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
	glutSwapBuffers();
}
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}
bool batDen = true;
GLfloat xoay_goc(GLfloat x) {
	return roundf(x * 10) / 10;
}
void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case 'k'://dịch chuyển cả căn phòng sang phải
		X1 += 0.2;
		glutPostRedisplay();
		break;
	case 'K'://dịch chuyển cả căn phòng sang trái
		X1 -= 0.2;
		glutPostRedisplay();
		break;
	case 'm'://dịch chuyển cả căn phòng ra sau
		Z1 += 0.2;
		glutPostRedisplay();
		break;
	case 'M'://dịch chuyển cả căn phòng lên trước
		Z1 -= 0.2;
		glutPostRedisplay();
		break;
	case 'q'://quay cả cửa hàng theo trục Oy 
		theta[0] += 5;
		if (theta[0] > 360)
		{
			theta[0] += 360;
		}
		glutPostRedisplay();
		break;
	case 'Q'://quay cả cửa hàng theo trục Oy
		theta[0] -= 5;
		if (theta[0] > 360)
		{
			theta[0] -= 360;
		}
		glutPostRedisplay();
		break;
	case 'f'://dịch chuyển kệ dạng cây doc sang phải
		X4 += 0.2;
		glutPostRedisplay();
		break;
	case 'F'://dịch chuyển kệ dạng cây doc sang trái
		X4 -= 0.2;
		glutPostRedisplay();
		break;
	case 'e'://dịch chuyển kệ dạng cây doc ra sau
		Z4 += 0.2;
		glutPostRedisplay();
		break;
	case 'E'://dịch chuyển kệ dạng cây doc lên trước
		Z4 -= 0.2;
		glutPostRedisplay();
		break;
	case 'a'://quay  ke dạng cây theo trục Oy theo chiều ngược kim đồng hồ
		goc_ke_cay += 5;
		if (goc_ke_cay > 360)
		{
			goc_ke_cay += 360;
		}
		glutPostRedisplay();
		break;
	case 'A'://quay  ke dạng cây theo trục Oy chiều kim đồng hồ
		goc_ke_cay-= 5;
		if (goc_ke_cay > 360)
		{
			goc_ke_cay -= 360;
		}
		glutPostRedisplay();
		break;
	
	case '0'://Thay đổi góc nhìn quanh trục OY
		eyeX = xoay_goc(eyeX);
		eyeZ = xoay_goc(eyeZ);
		if (eyeX <= 0 && eyeX > -4 && xoay_goc(eyeZ) >= -4 && eyeZ < 0) {
			eyeX -= 0.1;
			eyeZ += 0.1;
		}
		else if (eyeX >= -4 && eyeX < 0 && eyeZ >= 0 && eyeZ < 4) {
			eyeX += 0.1;
			eyeZ += 0.1;
		}
		else if (eyeX >= 0 && eyeX < 4 && eyeZ <= 4 && eyeZ > 0) {
			eyeX += 0.1;
			eyeZ -= 0.1;
		}
		else if (eyeX <= 4 && eyeX > 0 && eyeZ <= 0 && eyeZ > -4) {
			eyeX -= 0.1;
			eyeZ -= 0.1;
		}
		glutPostRedisplay();
		break;
	
	case '@'://Thay đổi góc nhìn quanh trục Y ngược chiều kim đồng hồ
		eyeX = xoay_goc(eyeX);
		eyeZ = xoay_goc(eyeZ);
		if (eyeX >= 0 && eyeX < 4 && xoay_goc(eyeZ) >= -4 && eyeZ < 0) {
			eyeX += 0.1;
			eyeZ += 0.1;
		}
		else if (eyeX <= 4 && eyeX > 0 && eyeZ >= 0 && eyeZ < 4) {
			eyeX -= 0.1;
			eyeZ += 0.1;
		}
		else if (eyeX <= 0 && eyeX > -4 && eyeZ <= 4 && eyeZ > 0) {
			eyeX -= 0.1;
			eyeZ -= 0.1;
		}
		else if (eyeX >= -4 && eyeX < 0 && eyeZ <= 0 && eyeZ > -4) {
			eyeX += 0.1;
			eyeZ -= 0.1;
		}
		glutPostRedisplay();
		break;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Mo hinh Cua hang ban do choi");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}
