#include "stdafx.h"
#include "mylist.h"
#include "waterfallsolver.h"

#include <stdio.h>
#include <stdlib.h>

Line *mLine = NULL;
Point *mPoint = NULL;
int mLine_num;
int mPoint_num;

mylist* m_flow_point = NULL;

bool init_state = false;
bool draw_state = false;
bool sele_state = false;
int  curr_point = 0;

void init_data(void){
	if (mLine != NULL) {
		delete mLine;
	}
	if (mPoint != NULL) {
		delete mPoint;
	}
	mLine_num = 0;
	mPoint_num = 0;
	init_state = false;
	draw_state = false;
	sele_state = false;
	curr_point=0;
}

void data_read(LPCTSTR fname){
	FILE* file;
	int i, tempx, tempy;
	file = fopen(fname,"r");
	fscanf(file,"%d",&mLine_num);
	mLine = (Line*)malloc(mLine_num*sizeof(Line));
	for(i=0;i<mLine_num;i++){
		fscanf(file,"%d %d %d %d", &((mLine+i)->xl),  &((mLine+i)->yl), &((mLine+i)->xr), &((mLine+i)->yr));
	}
	fscanf(file,"%d",&mPoint_num);
	mPoint = (Point*)malloc(mPoint_num*sizeof(Point*));
	for(i=0;i<mPoint_num;i++){
		fscanf(file,"%f %f",&((mPoint+i)->x), &((mPoint+i)->y));
	}
	fclose(file);
	init_state = true;
	sele_state = true;
}

/* ������ �� �׸��� Ǭ��. �Ʒ��� drawWaterflow() ������ ȣ���Ѵ�. */
void waterfall_Solver(void){
	Point S; 
	Line *minP, *P;
	int i, j;
	float tempY;
	int miny,minx,key;
	int mini, minj;
	if(init_state==false) return;
	
	if (m_flow_point != NULL) { delete m_flow_point; }//m_flow_point�� ������� ������ ����.
	m_flow_point = new mylist();
	P = (Line*)malloc(mLine_num * sizeof(Line));//������ ���̰� ���� ������ ���� �����Ͽ� ������ ����.
	minP = (Line*)malloc(sizeof(Line));//P�� ������ �� swap�� �ʿ��� ���п� �������.

	for (i = 0; i < mLine_num; i++) {
		P [i] = mLine[i];
	}//P�� ���п� ���� ������ �Ѱ��ش�.
	S.x = mPoint[curr_point].x; 
	S.y = mPoint[curr_point].y;
	m_flow_point->add(&S);//�������� ���� ������ �����Ѵ�.
	for (i = 0; i < mLine_num; i++) {
		if (P[i].yl > P[i].yr) { mini = P[i].yr; }
		else { mini = P[i].yl; }//���п��� ���� y�� ���� mini�� �����Ѵ�. �̴� ���Ŀ� ������ �����̶� ���Ͽ� ��� ������ ���� �ִ��� �Ǵ��� �� ����Ѵ�.
		key = i;//���� ���� ���п� ���� �迭�� ��ȣ�� ����(ó���� i���� ����).
		for (j = i; j < mLine_num; j++) {
			if (P[j].yl > P[j].yr) { minj = P[j].yr; }
			else { minj = P[j].yl; }//���п��� ���� y�� ���� minj�� �����Ѵ�.
			if (minj > mini) {
				key = j;
				mini = minj;
			}//mini�� minj�� ���Ͽ� ��������� ���� �ִ� ���п� ���� �迭�� ��ȣ�� key���� �����Ѵ�.
		}
		if (key != i) {
			*minP = P[i];
			P[i] = P[key];
			P[key] = *minP;
		}//key���� i�� ���� �ٸ��� swap�� �Ѵ�.
	}//���: line�� ���� �ִ� ������ �����Ͽ� P�� �����Ѵ�.
	
	for (i = 1; i < mLine_num; i++) {
		if (P[i].yl > P[i].yr) { minx = P[i].xr; miny = P[i].yr;}
		else { minx = P[i].xl; miny = P[i].yl;}//���п��� ��������� ���� ���� x�� y�� ������ ���� minx�� miny�� �����Ѵ�.
		if ((float)miny <= S.y && (float)P[i].xl <= S.x && (float)P[i].xr >= S.x) {//xl�� xr�翡�� S.x�� ��ġ�ϰ�, miny���� ���� ���� S.y�� ������ ��� ����.
			tempY = ((((float)P[i].yl - (float)P[i].yr) / ((float)P[i].xl - (float)P[i].xr)))*(S.x - (float)(P[i].xl)) + (float)(P[i].yl);//X�� ���� S.x�� ����� Y���� ���Ѵ�.
			if (tempY <= S.y) {//X �� ���� S.x�� ���� ���а� ������ ���� Y�� ���� S.y���� ���� ��� ����.
				S.y = tempY;
				m_flow_point->add(&S);
				S.x = (float)minx;
				S.y = (float)miny;
				m_flow_point->add(&S);
			}//���а� ������ ���� �����ϰ�, ���п��� ��������� ���� ���� �����Ѵ�.
		}
	}//��� ���п� ���� ��츦 ó���Ѵ�.
	S.y = 0; m_flow_point->add(&S);//�ٴڿ� ���������(y = 0)�� �����Ѵ�.
}

void drawBackground(CDC* pDC){
	int i; CPen MyPen; 
	
	if(init_state==false) return;

	MyPen.CreatePen(PS_SOLID, 10, RGB(0, 0, 154));
	pDC->SelectObject(MyPen);
	pDC->MoveTo(gXmin, gYmin); pDC->LineTo(gXmax, gYmin);
	pDC->MoveTo(gXmin, gYmax); pDC->LineTo(gXmax, gYmax);

	MyPen.DeleteObject(); MyPen.CreatePen(PS_SOLID, 5, RGB(100, 0, 0));
	pDC->SelectObject(MyPen);
	for(i=0;i<mLine_num;i++){
		pDC->MoveTo(gXmin+mLine[i].xl*lineSize, gYmax -mLine[i].yl*lineSize);
		pDC->LineTo(gXmin+mLine[i].xr*lineSize, gYmax -mLine[i].yr*lineSize);
	}
	MyPen.DeleteObject(); MyPen.CreatePen(PS_SOLID, 10, RGB(0, 0, 0));
	pDC->SelectObject(MyPen);
	for(i=0;i<mPoint_num;i++){
		pDC->Ellipse(gXmin + (int)mPoint[i].x*lineSize - cirsize,
					 gYmax - (int)mPoint[i].y*lineSize - cirsize,
					 gXmin + (int)mPoint[i].x*lineSize + cirsize,
					 gYmax - (int)mPoint[i].y*lineSize + cirsize);
	}
}

void drawStartPoint(CDC* pDC){
	CPen MyPen;

	if(init_state==false) return;
	if(sele_state==false) return;
	MyPen.CreatePen(PS_SOLID, 10, RGB(255, 0, 0));
	pDC->SelectObject(MyPen);
	pDC->Ellipse(gXmin + (int)mPoint[curr_point].x*lineSize - cirsize,
					 gYmax - (int)mPoint[curr_point].y*lineSize - cirsize,
					 gXmin + (int)mPoint[curr_point].x*lineSize + cirsize,
					 gYmax - (int)mPoint[curr_point].y*lineSize + cirsize);
	
}

void drawWaterflow(CDC* pDC){
	node *temp;
	CPen MyPen;
	if(init_state==false) return;
	if(sele_state==false) return;
	if(draw_state==false) return;

	waterfall_Solver();
	MyPen.CreatePen(PS_SOLID, 10, RGB(0, 0, 255));
	pDC->SelectObject(MyPen);
	temp = m_flow_point->move_first();

	while (1) {
		pDC->MoveTo((gXmin + (temp->point->x)*lineSize), (gYmax - (temp->point->y)*lineSize));
		pDC->LineTo(gXmin + (temp->next->point->x)*lineSize, gYmax - (temp->next->point->y)*lineSize);
		if (temp->next->next== NULL) {break;}
		temp = temp->next;
	}//m_flow_point�� ����� ���� NULL �� ���� ������ ����Ѵ�.(ó�� ������ �ٴ�(y=0)�̴�. ���� ������ ���� ��ġ)
}

void change_start_point(void){
	//�ڵ� �ʿ�
	curr_point++;//���� ���� ����Ű�� ������ ������Ų��.
	if(curr_point==mPoint_num){//���� ���� ���� ����Ű�� ������ ���� ������ ���� ��� �ٽ� �� ó�� ���� ����Ű�� ��(0)���� ��ȯ.
		curr_point = 0;
	}
}

void enable_drawWaterflow(bool mEnable){
	if(init_state==false) return;
	draw_state = mEnable;
}