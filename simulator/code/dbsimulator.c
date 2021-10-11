#include "stdafx.h"
#include <string.h>
#include "generale.h"
#include <time.h>
#include <math.h>
#include "distribution_generator.h"
void *stato_oggetti[N_OGGETTI];



typedef struct _event *event_pointer;
typedef struct _event{
	int sender;
	int receiver;
	int event_type;
	double timestamp;
	event_content_type *event_content;
	event_pointer next;
} event;


event *first_event;
event *last_event;

void SetPointer(int me, void *pointer) {
	stato_oggetti[me]=pointer;
}
void add_to_event_list(event *ev){
	if (first_event==NULL) {
		first_event=ev;
		last_event=ev;
	} else if (ev->timestamp>=last_event->timestamp) {
		last_event->next=ev;
		last_event=ev;
	} else if (ev->timestamp<first_event->timestamp) {
		ev->next=first_event;
		first_event=ev;
	} else {
		event_pointer event_temp=first_event;
		while(event_temp->next) {
			if (ev->timestamp <event_temp->next->timestamp) {
				ev->next=event_temp->next;
				event_temp->next=ev;
				return;
			}
			event_temp=event_temp->next;
		}
		event_temp->next=ev;
	}
	return;
}

int deliver_next_event() {
	if (first_event) {
		event_pointer event_to_deliver=first_event;
		first_event=event_to_deliver->next;
		ProcessEvent(event_to_deliver->receiver, event_to_deliver->timestamp, event_to_deliver->event_type, event_to_deliver->event_content, stato_oggetti[event_to_deliver->receiver]);
		if (event_to_deliver->event_content!=NULL) free(event_to_deliver->event_content);
		free(event_to_deliver);
		return 1;
	}
	return 0;

}

int main() {
	printf("---------------------\n");
	printf("Running simulation...\n\n");
	printf("Threads\tThroughput\n");
	int i;
	// send INIT events
	for(i=0;i<N_OGGETTI;i++){
		ProcessEvent(i, 0, INIT, NULL, stato_oggetti[i]);
	}
	srand(time(NULL));
	srand(1);
	unsigned seed=10;
	Random(&seed);

	//srand(0);
	while(deliver_next_event());
	printf("\n----------------------\n");	
	printf("Simulation terminated!\n");
	printf("----------------------\n");	

	return 0;
}

void InsertEvent(int gid_receiver, int gid_sender, double timestamp, double now, int event_type, event_content_type *event_content){
	event *ev=(event*)malloc(sizeof(event));
	ev->sender=gid_sender;
	ev->receiver=gid_receiver;
	ev->event_type=event_type;
	ev->timestamp=timestamp;
	if (event_content != NULL ){
		ev->event_content=(event_content_type*)malloc(sizeof(event_content_type));
		memcpy(ev->event_content,event_content,sizeof(event_content_type));
	} else {
		ev->event_content=NULL;
	}
	ev->next=NULL;
	add_to_event_list(ev);
}

