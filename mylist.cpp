#include "stdafx.h"
#include "mylist.h"
#include <stdio.h>
#include <stdlib.h>

mylist::mylist(){
	num = 0;
	head = prev_node = curr_node = NULL;
}

mylist::~mylist(){
	move_first();
	while(isEmpty()==false) del();
}

bool mylist::isEmpty(void){
	if(num==0) return true;
	return false;
}

void mylist::add(Point* tpoint){

	node *new_node, *temp;
	Point *temp_point;
	new_node=(node*)malloc(sizeof(node));
	temp_point = (Point*)malloc(sizeof(Point));
	temp_point->x = tpoint->x;
	temp_point->y = tpoint->y;
	new_node->point = temp_point;
	temp = head;
	head = new_node;
	new_node->next = temp;
	num++;
		
}


node* mylist::del(void){
	node* temp;
	if (head == NULL) {
		return NULL;
	}//주어진 코드에서는 여기에서 계속 무한루프 돌기에 수정하였습니다.
	temp = head;
	head = head->next;
	free(temp->point);
	free(temp);
	num--;
	return head;
}

node* mylist::move_first(void){
	
	return this->head;
	//코드 필요
}

node* mylist::move_next(void){
	return this->head->next;
	//코드 필요
	//return curr_node->next;
}
