#include "Render.h"
#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <math.h>

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

void DrawSquare(double v1[], double v2[], double v3[], double v4[],int flag=0)
{
	glBegin(GL_QUADS);
	if(flag==0)
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



void DrawWall(double v1[], double v2[],int flag=0,int k=4)
{
	double tmp1[3];
	double tmp2[3];
	tmp1[0] = v1[0];
	tmp1[1] = v1[1];
	tmp1[2] = v1[2] - k;
	tmp2[0] = v2[0];
	tmp2[1] = v2[1];
	tmp2[2] = v2[2] - k;
	DrawSquare(v1, v2, tmp2, tmp1,flag);
}

void DrawWalls(double Figure[8][3],int n)
{
	for (int i = 0; i <= n; i++)
	{
		if(i!=4)
			if (i == n)
				DrawWall(Figure[i], Figure[0]);
			else
				DrawWall(Figure[i], Figure[i + 1]);
		
	}
}


void DrawTriangle(double v1[], double v2[], double v3[],int flag=0)
{
	glBegin(GL_TRIANGLES);
	if(flag==0)
		glColor3d(0.1488,  0.228,  0.322);
	else
		if(flag==1)
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


void DrawHalfOfCircle(double v2[], double v3[],int m,double l)
{
	double v1[3];
	double v4[3];
	double r;
	double n = 1000;
	double k;
	if ((v2[0] - v3[0]) != 0)
		k = (v2[1] - v3[1]) / (v2[0] - v3[0]);
	else
		k = tan(M_PI/2.0);
	v1[0] = (v3[0] + v2[0])/2;
	v1[1] = (v3[1] + v2[1]) / 2;
	v1[2] = (v3[2] + v2[2]) / 2;
	r = sqrt(pow(v3[0] - v2[0],2) + pow(v3[1] - v2[1],2) + pow(v3[2] - v2[2],2))/2;	
	for (int i = 1; i <= n; i++)
	{
		v4[0] = r * cos((atan(k) - l*((M_PI) / n) * i)) + v1[0];
		v4[1] = r * sin((atan(k) - l*((M_PI) / n) * i)) + v1[1];
		v4[2] = m;
		DrawTriangle(v2, v1, v4);
		DrawWall(v4, v2);
		v1[2] = v1[2] - m;
		v2[2] = v2[2] - m;
		v4[2] = v4[2] - m;
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
	int n = 10000;
	glColor3d(0.1488, 0.228, 0.322);
	for (int i = 0; i <= n; i++)
	{
		v3[2] = v1[2];
		A[0] = r * cos((((2.0 * M_PI) / n) *i)) + O[0];
		A[1] = r * sin(((2.0 * (M_PI) / n) * i)) + O[1];
		A[2] = v1[2];
		C[0] = r * cos((((2.0 * M_PI) / n) * (i + 1))) + O[0];
		C[1] = r * sin(((2.0 * (M_PI) / n) * (i + 1))) + O[1];
		C[2] = v1[2];
		if ((A[1] >= (-2.6666666666667 * A[0] + 23)) && ((C[1] >= (-2.6666666666667 * C[0] + 23))))
		{
			DrawTriangle(A, C, v3);
			if(v1[2]!=0)
			DrawWall(A, C);
		}

	}
}

void DrawFigure(double Figure[8][3])
{
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
	DrawFigure(Figure);
}


void Render(double delta_time)
{    
	double Figure[8][3] = { {16,3,0},{23,19,0},{17,16,0},{9,20,0},{3,15,0},{6,7,0},{11,6,0},{13,13,0} };
	double M[3] = { 5,4,0 };
	DrawFigure(Figure);
	DrawUpperFigure(Figure);
	DrawWalls(Figure,7);
	DrawHalfOfCircle(Figure[1], Figure[0],4,1);

	
}   