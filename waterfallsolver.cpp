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

/* 문제는 물 그릴때 푼다. 아래의 drawWaterflow() 내에서 호출한다. */
void waterfall_Solver(void){
	Point S; 
	Line *minP, *P;
	int i, j;
	float tempY;
	int miny,minx,key;
	int mini, minj;
	if(init_state==false) return;
	
	if (m_flow_point != NULL) { delete m_flow_point; }//m_flow_point가 비어있지 않으면 비운다.
	m_flow_point = new mylist();
	P = (Line*)malloc(mLine_num * sizeof(Line));//선분의 높이가 높은 순서에 따라 정렬하여 저장할 공간.
	minP = (Line*)malloc(sizeof(Line));//P를 정렬할 때 swap에 필요한 여분용 저장공간.

	for (i = 0; i < mLine_num; i++) {
		P [i] = mLine[i];
	}//P에 선분에 대한 정보를 넘겨준다.
	S.x = mPoint[curr_point].x; 
	S.y = mPoint[curr_point].y;
	m_flow_point->add(&S);//시작점의 대한 정보를 저장한다.
	for (i = 0; i < mLine_num; i++) {
		if (P[i].yl > P[i].yr) { mini = P[i].yr; }
		else { mini = P[i].yl; }//선분에서 낮은 y의 값을 mini에 저장한다. 이는 이후에 나오는 선분이랑 비교하여 어느 선분이 위에 있는지 판단할 때 사용한다.
		key = i;//가장 높은 선분에 대한 배열의 번호를 저장(처음은 i값을 저장).
		for (j = i; j < mLine_num; j++) {
			if (P[j].yl > P[j].yr) { minj = P[j].yr; }
			else { minj = P[j].yl; }//선분에서 낮은 y의 값을 minj에 저장한다.
			if (minj > mini) {
				key = j;
				mini = minj;
			}//mini와 minj를 비교하여 상대적으로 높이 있는 선분에 대한 배열의 번호를 key값에 저장한다.
		}
		if (key != i) {
			*minP = P[i];
			P[i] = P[key];
			P[key] = *minP;
		}//key값과 i의 값이 다르면 swap을 한다.
	}//결론: line을 높이 있는 순으로 정리하여 P에 저장한다.
	
	for (i = 1; i < mLine_num; i++) {
		if (P[i].yl > P[i].yr) { minx = P[i].xr; miny = P[i].yr;}
		else { minx = P[i].xl; miny = P[i].yl;}//선분에서 상대적으로 낮은 쪽의 x와 y의 정보를 각각 minx와 miny에 저장한다.
		if ((float)miny <= S.y && (float)P[i].xl <= S.x && (float)P[i].xr >= S.x) {//xl과 xr사에에 S.x가 위치하고, miny보다 높은 곳에 S.y가 존재할 경우 실행.
			tempY = ((((float)P[i].yl - (float)P[i].yr) / ((float)P[i].xl - (float)P[i].xr)))*(S.x - (float)(P[i].xl)) + (float)(P[i].yl);//X의 값이 S.x인 경우의 Y값을 구한다.
			if (tempY <= S.y) {//X 의 값이 S.x일 때의 선분과 만나는 점의 Y의 값이 S.y보다 작을 경우 실행.
				S.y = tempY;
				m_flow_point->add(&S);
				S.x = (float)minx;
				S.y = (float)miny;
				m_flow_point->add(&S);
			}//선분과 만나는 점을 저장하고, 선분에서 상대적으로 낮은 점을 저장한다.
		}
	}//모든 선분에 대한 경우를 처리한다.
	S.y = 0; m_flow_point->add(&S);//바닥에 떨어진경우(y = 0)를 저장한다.
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
	}//m_flow_point에 저장된 값을 NULL 이 나올 떄까지 출력한다.(처음 시작은 바닥(y=0)이다. 끝은 선택한 점의 위치)
}

void change_start_point(void){
	//코드 필요
	curr_point++;//다음 점을 가리키는 변수를 증가시킨다.
	if(curr_point==mPoint_num){//만약 다음 점을 가리키는 변수가 점의 개수와 같은 경우 다시 맨 처음 점을 가리키는 수(0)으로 변환.
		curr_point = 0;
	}
}

void enable_drawWaterflow(bool mEnable){
	if(init_state==false) return;
	draw_state = mEnable;
}