#include "stdafx.h"

#include "generale.h"
#include "client.h"
#include "tm.h"
#include "cc.h"
#include "gvc.h"



void STAT_COLLECTOR_ProcessEvent(int me,double now, int event_type, event_content_type *event_content, STAT_COLLECTOR_lp_state_type * pointer);

// me indica il processo che deve elaborare l'evento
// now e' l'istante corrente (pari all'lvt del processo)
// event_content punta alle informazioni dell'evento
// pointer e' un puntatore allo stato del processo identificato da me
void ProcessEvent(int me, double now, int event_type, event_content_type *event_content, void * pointer) {

	switch(me) {

		case CLIENT:
			if (APP_VERBOSE) printf("** Funzione ProcessEvent: ricevuto evento %d, inoltro al CLIENT\n",event_type);
			CLIENT_ProcessEvent(me, now, event_type, event_content,(CLIENT_lp_state_type*) pointer);
			break;

		case TM:
			if (APP_VERBOSE) printf("** Funzione ProcessEvent: ricevuto evento %d, inoltro a TM\n",event_type);
			TM_ProcessEvent(me, now, event_type, event_content,(TM_lp_state_type*) pointer);
			break;

		case CC:
			if (APP_VERBOSE) printf("** Funzione ProcessEvent: ricevuto evento %d, inoltro a :CC\n",event_type);
			CC_ProcessEvent(me, now, event_type, event_content,(CC_lp_state_type*) pointer);
			break;

		case GVC:
			if (APP_VERBOSE) printf("** Funzione ProcessEvent: ricevuto evento %d, inoltro a :GVC\n",event_type);
			GVC_ProcessEvent(me, now, event_type, event_content,(GVC_lp_state_type*) pointer);
			break;

		case STAT_COLLECTOR:
			if (APP_VERBOSE) printf("** Funzione ProcessEvent: ricevuto evento %d, inoltro a STATS_COLLECTOR\n",event_type);
			STAT_COLLECTOR_ProcessEvent(me, now, event_type, event_content, (STAT_COLLECTOR_lp_state_type*) pointer);
			break;
	}
}

int CheckTermination(void *snapshot) {
	//if (CLIENT_check_termination(snapshot)) return 1;
//	else return 0;
	return 0;
 }



