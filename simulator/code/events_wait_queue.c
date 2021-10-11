#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "generale.h"

int get_queue_length(event_wait_queue *queue) {
	int i=0;
	if(queue->top) {
		wait_event *ev=queue->top;
		while(ev) {
			ev=ev->next;
			 i++;
		}
	}
	return i;
}

void copy_into_wait_queue(int event_type, event_content_type *event_content,double now, event_wait_queue *queue) {
	wait_event *event;
	event_content_type *new_event_content=(event_content_type*)malloc(sizeof(event_content_type));
	if (MALLOC_FAIL_VERBOSE && new_event_content==NULL) printf("---- Funzione copy_into_wait_queue: MALLOC FALLITA\n");
  	memcpy(new_event_content,event_content,sizeof(event_content_type));
	event=(wait_event*)malloc(sizeof(wait_event));
	if (MALLOC_FAIL_VERBOSE && event==NULL) printf("---- Funzione copy_into_wait_queue: MALLOC FALLITA\n");
	event->event_type=event_type;	
	event->event_content=new_event_content;
	event->insertion_time=now;
	event->next=NULL;
	if (queue->bottom) {
        	queue->bottom->next=event;
	}
	queue->bottom=event;
	if(!queue->top){
		queue->top=event;
	}
}

void get_from_wait_queue(int *event_type, event_content_type **event_content, event_wait_queue *queue) { 	

    if (queue->top) {
		*event_type=queue->top->event_type;	
		*event_content=queue->top->event_content;	
		queue->top=queue->top->next;

    	if (!queue->top)
        	queue->bottom=NULL; 


	} else {
		event_content=NULL;
		queue->top=NULL;
	} 


}

int is_empty(event_wait_queue *queue) {
	if (queue->top==NULL) return 1;
		else return 0;		
}



