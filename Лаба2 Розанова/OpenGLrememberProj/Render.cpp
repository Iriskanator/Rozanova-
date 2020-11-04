#include "Render.h"

#include <sstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

double* CalculateNorm(double v1[], double v2[], double v3[])
{

	double a[] = { v1[0] - v2[0],v1[1] - v2[1],v1[2] - v2[2] };
	double b[] = { v3[0] - v2[0],v3[1] - v2[1],v3[2] - v2[2] };;
	double Result[] = { a[1] * b[2] - b[1] * a[2],(-1.0 * a[0] * b[2] + b[0] * a[2]),a[0] * b[1] - b[0] * a[1] };
	for (int i = 0; i < 3; i++)
		Result[i] = Result[i] / (sqrt(pow(Result[0], 2) + pow(Result[1], 2) + pow(Result[2], 2)));
	return &Result[0];
}

void DrawPolygon(double v1[], double v2[], double v3[], double v4[], double v5[])
{
	glColor3d(0.1488, 0.228, 0.322);
	glBegin(GL_POLYGON);
	glVertex3dv(v1);
	glVertex3dv(v2);
	glVertex3dv(v3);
	glVertex3dv(v4);
	glVertex3dv(v5);
	glEnd();
}

void DrawSquare(double v1[], double v2[], double v3[], double v4[], int flag = 0)
{
	glBegin(GL_QUADS);
	if (flag == 0)
		glColor3d(0.228, 0.1488, 0.322);
	else
		if (flag == 1)
			glColor3d(1, 1, 1);
		else
			if (flag == 2)
				glColor3d(1, 0, 0);
	glVertex3dv(v1);
	glVertex3dv(v2);
	glVertex3dv(v3);
	glVertex3dv(v4);
	glEnd();
}



void DrawWall(double v1[], double v2[], int flag = 0, int k = 4)
{
	double tmp1[3];
	double tmp2[3];
	tmp1[0] = v1[0];
	tmp1[1] = v1[1];
	tmp1[2] = v1[2] - k;
	tmp2[0] = v2[0];
	tmp2[1] = v2[1];
	tmp2[2] = v2[2] - k;
	glNormal3d(CalculateNorm(v1, tmp1, v2)[0], CalculateNorm(v1, tmp1, v2)[1], CalculateNorm(v1, tmp1, v2)[2]);
	DrawSquare(v1, v2, tmp2, tmp1, flag);
}

void DrawWalls(double Figure[8][3], int n)
{
	for (int i = 0; i <= n; i++)
	{
		if (i != 4)
			if (i == n)
				DrawWall(Figure[0],Figure[i]);
			else
				DrawWall(Figure[i + 1], Figure[i]);

	}
}


void DrawTriangle(double v1[], double v2[], double v3[], int flag = 0)
{
	glBegin(GL_TRIANGLES);
	if (flag == 0)
		glColor3d(0.1488, 0.228, 0.322);
	else
		if (flag == 1)
			glColor3d(0, 1, 0);
		else
			if (flag == 2)
				glColor3d(1, 0, 0);
			else
				if (flag == 3)
					glColor3d(0, 0, 1);
	glVertex3dv(v1);
	glVertex3dv(v2);
	glVertex3dv(v3);
	glEnd();
}

void DrawPoint(double v1[])
{
	glBegin(GL_POINTS);
	//glColor3d(0.1488, 0.228,  0.322);
	glVertex3dv(v1);

	glEnd();
}

void DrawLine(double v1[], double v2[])
{
	glBegin(GL_LINES);
	//glColor3d(0, 0, 0);
	glVertex3dv(v1);
	glVertex3dv(v2);
	glEnd();
}


void DrawHalfOfCircle(double v2[], double v3[], int m, double l)
{
	double v1[3];
	double v4[3];
	double r;
	double n = 1000;
	double k;
	if ((v2[0] - v3[0]) != 0)
		k = (v2[1] - v3[1]) / (v2[0] - v3[0]);
	else
		k = tan(M_PI / 2.0);
	v1[0] = (v3[0] + v2[0]) / 2;
	v1[1] = (v3[1] + v2[1]) / 2;
	v1[2] = (v3[2] + v2[2]) / 2;
	r = sqrt(pow(v3[0] - v2[0], 2) + pow(v3[1] - v2[1], 2) + pow(v3[2] - v2[2], 2)) / 2;
	for (int i = 1; i <= n; i++)
	{
		v4[0] = r * cos((atan(k) - l * ((M_PI) / n) * i)) + v1[0];
		v4[1] = r * sin((atan(k) - l * ((M_PI) / n) * i)) + v1[1];
		v4[2] = m;
		glNormal3d(CalculateNorm(v2, v4, v1)[0], CalculateNorm(v2, v4, v1)[1], CalculateNorm(v2, v4, v1)[2]);
		
		DrawTriangle(v2, v1, v4);
		DrawWall(v2, v4);
		v1[2] = v1[2] - m;
		v2[2] = v2[2] - m;
		v4[2] = v4[2] - m;
		glNormal3d(CalculateNorm(v2, v1, v4)[0], CalculateNorm(v2, v1, v4)[1], CalculateNorm(v2, v1, v4)[2]);
		DrawTriangle(v2, v1, v4);
		v1[2] = v1[2] + m;
		v2[2] = v2[2] + m;
		v4[2] = v4[2] + m;
		v2[0] = v4[0];
		v2[1] = v4[1];
		v2[2] = v4[2];
	}
}




void DrawCuted(double v1[], double v2[], double v3[])
{
	double O[] = { -2.794117647058824, 8.264705882352941, (v1[2] + v2[2]) / 2 };
	double v0[] = { (v1[0] + v2[0]) / 2,(v1[1] + v2[1]) / 2,0 };
	double A[3];
	double C[3];
	double r = 8.884592627643068;
	int n = 50000;
	glColor3d(0.1488, 0.228, 0.322);
	for (int i = 0; i <= n; i++)
	{
		v3[2] = v1[2];
		A[0] = r * cos((((2.0 * M_PI) / n) * i)) + O[0];
		A[1] = r * sin(((2.0 * (M_PI) / n) * i)) + O[1];
		A[2] = v1[2];
		C[0] = r * cos((((2.0 * M_PI) / n) * (i + 1))) + O[0];
		C[1] = r * sin(((2.0 * (M_PI) / n) * (i + 1))) + O[1];
		C[2] = v1[2];
		if ((A[1] >= (-2.6666666666667 * A[0] + 23)) && ((C[1] >= (-2.6666666666667 * C[0] + 23))))
		{
			if (((A[2] + C[2]) / 2) != 0)
				glNormal3d(CalculateNorm(A,C, v3)[0], CalculateNorm(A, C, v3)[1], CalculateNorm(A, C, v3)[2]);
			else
				glNormal3d(CalculateNorm(A, v3, C)[0], CalculateNorm(A, v3, C)[1], CalculateNorm(A, v3, C)[2]);
			DrawTriangle(A, C, v3);
			if (v1[2] != 0)
				DrawWall(A, C);
		}

	}
}

void DrawFigure(double Figure[8][3])
{
	glNormal3d(CalculateNorm(Figure[0], Figure[1], Figure[2])[0], CalculateNorm(Figure[0], Figure[1], Figure[2])[1], CalculateNorm(Figure[0], Figure[1], Figure[2])[2]);
	DrawTriangle(Figure[5], Figure[6], Figure[7]);
	DrawTriangle(Figure[4], Figure[3], Figure[7]);
	DrawCuted(Figure[4], Figure[5], Figure[7]);
	DrawTriangle(Figure[2], Figure[3], Figure[7]);
	DrawPolygon(Figure[7], Figure[0], Figure[1], Figure[2], Figure[2]);
}

void DrawUpperFigure(double Figure[8][3])
{
	for (int i = 0; i < 8; i++)
		Figure[i][2] = Figure[i][2] + 4;
	glNormal3d(CalculateNorm(Figure[0], Figure[2], Figure[1])[0], CalculateNorm(Figure[0], Figure[2], Figure[1])[1], CalculateNorm(Figure[0], Figure[2], Figure[1])[2]);
	DrawTriangle(Figure[5], Figure[6], Figure[7]);
	glNormal3d(CalculateNorm(Figure[0], Figure[2], Figure[1])[0], CalculateNorm(Figure[0], Figure[2], Figure[1])[1], CalculateNorm(Figure[0], Figure[2], Figure[1])[2]);
	DrawTriangle(Figure[4], Figure[3], Figure[7]);
	DrawCuted(Figure[4], Figure[5], Figure[7]);
	glNormal3d(CalculateNorm(Figure[0], Figure[2], Figure[1])[0], CalculateNorm(Figure[0], Figure[2], Figure[1])[1], CalculateNorm(Figure[0], Figure[2], Figure[1])[2]);
	DrawTriangle(Figure[2], Figure[3], Figure[7]);
	DrawPolygon(Figure[7], Figure[0], Figure[1], Figure[2], Figure[2]);
}

void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  
	double Figure[8][3] = { {16,3,0},{23,19,0},{17,16,0},{9,20,0},{3,15,0},{6,7,0},{11,6,0},{13,13,0} };
	double M[3] = { 5,4,0 };
	DrawFigure(Figure);
	DrawUpperFigure(Figure);
	DrawWalls(Figure, 7);
	DrawHalfOfCircle(Figure[1], Figure[0], 4, 1);

	//Начало рисования квадратика станкина
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//конец рисования квадратика станкина


   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}