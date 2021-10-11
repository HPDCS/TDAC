#include "stdafx.h"


#include "generale.h"
#include "events_wait_list.h"

// puntatore al primo puntatore della lista delle transazioni
cc_transaction_descriptor **cc_tx_list;

void stat_set_op_lock_requested(event_content_type* event_content, int op_number,int di_id, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=op_number;
	new_event_content.di_id=di_id;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, CC, now, now, STAT_SET_op_lock_requested, &new_event_content);
}

void stat_set_op_enc_time_read_lock_contention(event_content_type* event_content, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=event_content->op_number;
	new_event_content.di_id=event_content->di_id;
	new_event_content.op_type=event_content->op_type;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, CC, now, now, STAT_SET_op_enc_time_read_lock_contention, &new_event_content);
}

void stat_set_op_com_time_read_lock_contention(event_content_type* event_content, int op_number, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=op_number;
	new_event_content.di_id=event_content->di_id;
	new_event_content.op_type=event_content->op_type;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, CC, now, now, STAT_SET_op_com_time_read_lock_contention, &new_event_content);
}

void stat_set_op_enc_time_write_lock_contention(event_content_type* event_content,int op_number, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=op_number;
	new_event_content.di_id=event_content->di_id;
	new_event_content.op_type=event_content->op_type;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, CC, now, now, STAT_SET_op_enc_time_write_lock_contention, &new_event_content);
}

void stat_set_op_com_time_write_lock_contention(event_content_type* event_content,int op_number, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=op_number;
	new_event_content.di_id=event_content->di_id;
	new_event_content.op_type=event_content->op_type;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, CC, now, now, STAT_SET_op_com_time_write_lock_contention, &new_event_content);
}

void stat_set_op_enc_time_validation_fail(event_content_type* event_content, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=event_content->op_number;
	new_event_content.di_id=event_content->di_id;
	new_event_content.op_type=event_content->op_type;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, CC, now, now, STAT_SET_op_enc_time_validation_fail, &new_event_content);
}

void stat_set_op_lock_released(event_content_type* event_content, int op_number,int di_id, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=op_number;
	new_event_content.di_id=di_id;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, CC, now, now, STAT_SET_op_lock_reseased, &new_event_content);
}

void stat_set_op_com_time_validation_fail(event_content_type* event_content, int op_number, double now) {
	event_content_type new_event_content;
	new_event_content.tx_id=event_content->tx_id;
	new_event_content.tx_run_number=event_content->tx_run_number;
	new_event_content.op_number=op_number;
	new_event_content.di_id=event_content->di_id;
	new_event_content.op_type=event_content->op_type;
	new_event_content.time=now;
	InsertEvent(STAT_COLLECTOR, CC, now, now, STAT_SET_op_com_time_validation_fail, &new_event_content);
}

// toglie i lock da tutti i dataitems nel write set della tx
void unlock_data_items(int tx_id, CC_lp_state_type * pointer, event_content_type *event_content, double now){
	int i=1;
	cc_tx_write_set_item_pointer item_temp;
	while((item_temp=cc_get_tx_write_set_item_by_position(tx_id, i))!=NULL){
		if (pointer->cc_dataitems_list[item_temp->di_id]->tx_id_locker_exclusive==tx_id) {
			pointer->cc_dataitems_list[item_temp->di_id]->tx_id_locker_exclusive=-1;
			/**************************************/
			// operazioni di raccolta statistiche
			if (TM_PRINT_STAT) stat_set_op_lock_released(event_content, item_temp->run_op_number, item_temp->di_id, now);
			/**************************************/
		}
		i++;
	}

}



// sveglia le eventuali transazioni in attesa sui dataitems lockati dalla tx
void wake_up_first_waiting_transactions(int tx_id, int me, double now, CC_lp_state_type * pointer) {
	int i=1;
	cc_tx_write_set_item *item;
	int event_type;
	event_content_type *event_content;
	// legge tutti i dataitem nel write set della tx
	while((item=cc_get_tx_write_set_item_by_position(tx_id, i))!=NULL){
		// prende la prima tx in attesa sul dataitem di->di_id
		if (get_first_waiting_event(item->di_id, &event_type, &event_content,(wait_event **)&(pointer->locks_event_wait_list)) ){
			InsertEvent(CC, me, now, now, TM_WRITE, event_content);
		}

		if (CC_VERBOSE) printf("----- Funzione CC_ProcessEvent: risvegliata operazione %i su dataitem %i della tx %i\n",event_content->op_number, event_content->di_id,event_content->tx_id);
		i++;
	}
};

// effettua l'abort della transazione
void abort_transaction(int me,double now, event_content_type *event_content, CC_lp_state_type * pointer) {

	// toglie i lock da tutti i dataitems aggiornati dalla tx
	unlock_data_items(event_content->tx_id, pointer, event_content, now);
	// elimina la transazione dalla lista delle transazioni del CC
	cc_remove_transaction(event_content->tx_id);
}


void CC_ProcessEvent(int me,double now, int event_type, event_content_type *event_content, CC_lp_state_type * pointer) {

	// memorizza nelle variabili locali i puntatori alle liste
	cc_tx_list=&pointer->cc_transactions_list;
	cc_transaction_descriptor *cc_tx_descr;
	int i;
	cc_tx_write_set_item *item;
	cc_tx_read_set_item *read_set_item;

	switch (event_type){

		case INIT:
			if (CC_VERBOSE) printf("----- Funzione CC_ProcessEvent: ricevuto evento INIT\n");
            pointer = (CC_lp_state_type*)malloc(sizeof(CC_lp_state_type));
			if (MALLOC_FAIL_VERBOSE && pointer==NULL) printf("---- Funzione CC_ProcessEvent: MALLOC FALLITA\n");
			SetPointer(me,pointer);
			pointer->cc_transactions_list=NULL;
			pointer->locks_event_wait_list=NULL;

			// memorizza nelle variabili locali i puntatori alle liste
			cc_tx_list=&pointer->cc_transactions_list;


			//inizializza i metadati
			int k;
			for(k=0; k<NUMBER_OF_DATA_ITEMS; k++){
				// crea il nuovo dataitem
				pointer->cc_dataitems_list[k]=(cc_data_item *)malloc(sizeof(cc_data_item));
				pointer->cc_dataitems_list[k]->commit_timestamp=0;
				pointer->cc_dataitems_list[k]->tx_id_locker_exclusive =-1;
				pointer->cc_dataitems_list[k]->di_id=k;
			}

			break;

		case TM_BEGIN:

			if (CC_VERBOSE) printf("----- Funzione CC_ProcessEvent: ricevuta richiesta di TM_BEGIN da tx %i\n",event_content->tx_id);
			// aggiunge la transazione nel cc con lo start_timestamp
			cc_enqueue_transaction(event_content->tx_id, event_content->tx_run_number, now);
			// risponde al TM
			InsertEvent(TM, me, now+TIME_FOR_TM_BEGIN_EXECUTION, now, TM_BEGIN_EXECUTED, event_content);

			break;

		case TM_READ:

			if (CC_VERBOSE) printf("----- Funzione CC_ProcessEvent: ricevuta richiesta di TM_READ da tx %i dataitem %i\n",event_content->tx_id,event_content->di_id);
			//verifica se il data item è già nel proprio write set
			cc_tx_descr= cc_get_transaction(event_content->tx_id);
			if (get_write_set_item_by_id(cc_tx_descr, event_content->di_id)==NULL) {
				// non è nel proprio write set

				if (ENC_TIME_READ_LOCK) {
					// verifica che non ci siano lock
					if (pointer->cc_dataitems_list[event_content->di_id]->tx_id_locker_exclusive!=-1) {
						// c'è un lock
						/**************************************/
						// operazioni di raccolta statistiche
						if (TM_PRINT_STAT) stat_set_op_enc_time_read_lock_contention(event_content, now);
						/**************************************/
						// abortisce la transazione e risponde al TM
						abort_transaction(me,now,event_content, pointer);
						// invia l'evento di TM_ABORT_EXECUTED al TM
						InsertEvent(TM, me, now+TIME_FOR_TM_READ_EXECUTION+TIME_FOR_TM_ABORT_EXECUTION, now, TM_ABORT_EXECUTED, event_content);
						return;
					}
				}

				if (ENC_TIME_READ_VALIDATION) {
					// effettua la read validation
					if (pointer->cc_dataitems_list[event_content->di_id]->commit_timestamp > cc_tx_descr->start_timestamp) {
						// read validation fallita
						/**************************************/
						// operazioni di raccolta statistiche
						if (TM_PRINT_STAT) stat_set_op_enc_time_validation_fail(event_content, now);
						/**************************************/
						// abortisce la transazione e risponde al TM
						abort_transaction(me,now,event_content, pointer);
						// invia l'evento di TM_ABORT_EXECUTED al TM
						InsertEvent(TM, me, now+TIME_FOR_TM_READ_EXECUTION+TIME_FOR_TM_ABORT_EXECUTION, now, TM_ABORT_EXECUTED, event_content);
						return;
					}
				}
				// inserisce il data item nel read set se non è già presente
				cc_add_read_set_item(event_content->tx_id, event_content->di_id, event_content->op_number);
			}
			// esegue la read
			InsertEvent(TM, me, now+TIME_FOR_TM_READ_EXECUTION, now, TM_OPERATION_EXECUTED, event_content);
			break;

		case TM_WRITE:

			if (CC_VERBOSE) printf("----- Funzione CC_ProcessEvent: ricevuta richiesta di TM_WRITE da tx %i dataitem %i\n",event_content->tx_id,event_content->di_id);

			if (TL2_LOCKING_MODE==TL2_EAGER) {
				if (ENC_TIME_WRITE_LOCK) {
					// tenta di prendere il lock
					/**************************************/
					// operazioni di raccolta statistiche
					if (TM_PRINT_STAT) stat_set_op_lock_requested(event_content,event_content->op_number, event_content->di_id, now);
					/**************************************/
					// prende il lock se è libero o ce l'ha gia
					if (pointer->cc_dataitems_list[event_content->di_id]->tx_id_locker_exclusive!=-1 &&
						pointer->cc_dataitems_list[event_content->di_id]->tx_id_locker_exclusive!=event_content->tx_id) {
						// c'è un lock su data item
						/**************************************/
						// operazioni di raccolta statistiche
						if (TM_PRINT_STAT) stat_set_op_enc_time_write_lock_contention(event_content, event_content->op_number, now);
						/**************************************/
						/*
						if (TL2_EAGER_LOCKING_MODE==TL2_EAGER_WAIT){
							//mette la tx in attesa
							copy_into_wait_list(event_type, event_content,(wait_event **)&(pointer->locks_event_wait_list));
							return;
						} else {
						*/
							// abortisce la transazione
							abort_transaction(me,now,event_content, pointer);
							// invia l'evento TM_ABORT_EXECUTED al TM
							InsertEvent(TM, me, now+TIME_FOR_TM_ABORT_EXECUTION, now, TM_ABORT_EXECUTED, event_content);
							return;
						/*
						} // TL2_EAGER_WAIT
						*/
					}
					//mette il lock
					pointer->cc_dataitems_list[event_content->di_id]->tx_id_locker_exclusive=event_content->tx_id;
				}
			}	//TL2_EAGER

			// inserisce il data item nel write set se non è già presente
			cc_add_write_set_item(event_content->tx_id, event_content->di_id, event_content->op_number);
			// esegue la write
			InsertEvent(TM, me, now+TIME_FOR_TM_WRITE_EXECUTION, now, TM_OPERATION_EXECUTED, event_content);

			break;

		case TM_COMMIT:



			// prende tutti i lock
			if (TL2_LOCKING_MODE==TL2_LAZY){
				i=1;
				while((item=cc_get_tx_write_set_item_by_position(event_content->tx_id, i))!=NULL){
					// tenta di prendere il lock
					/**************************************/
					// operazioni di raccolta statistiche
					if (TM_PRINT_STAT) stat_set_op_lock_requested(event_content,item->run_op_number,item->di_id, now);

					/**************************************/
					// prende il lock se è libero o ce l'ha gia
					if (COM_TIME_WRITE_LOCK) {
						if (pointer->cc_dataitems_list[item->di_id]->tx_id_locker_exclusive!=-1 &&
							pointer->cc_dataitems_list[item->di_id]->tx_id_locker_exclusive!=event_content->tx_id) {
							/**************************************/
							// operazioni di raccolta statistiche
							if (TM_PRINT_STAT) stat_set_op_com_time_write_lock_contention(event_content, item->run_op_number, now);
							/**************************************/
							// abortisce la transazione
							abort_transaction(me,now,event_content, pointer);
							// invia l'evento TM_ABORT_EXECUTED al TM
							InsertEvent(TM, me, now+TIME_FOR_COMMIT_EXECUTION+TIME_FOR_TM_ABORT_EXECUTION, now, TM_ABORT_EXECUTED, event_content);
							return;
						}
					}
					pointer->cc_dataitems_list[item->di_id]->tx_id_locker_exclusive=event_content->tx_id;
					i++;
				}
				// ha preso tutti i locks
			} // TL2_LAZY

			// incrementa il global version clock
			event_content->origin_object=me;
			InsertEvent(GVC, me, now+TIME_FOR_COMMIT_EXECUTION, now, GVC_REQUEST, event_content);

			break;


		case GVC_RESPONSE:

			//  ha incrementato il global version clock


			cc_tx_descr=cc_get_transaction(event_content->tx_id);
			if (COM_TIME_READ_VALIDATION) {
				// valida il read set
				i=1;
				while((read_set_item=cc_get_tx_read_set_item_by_position(event_content->tx_id, i))!=NULL){
					// valida la lettura
					if (pointer->cc_dataitems_list[read_set_item->di_id]->commit_timestamp > cc_tx_descr->start_timestamp) {
						/**************************************/
						// operazioni di raccolta statistiche
						if (TM_PRINT_STAT) stat_set_op_com_time_validation_fail(event_content, read_set_item->run_op_number, now);
						/**************************************/
						//abortisce la transazione
						abort_transaction(me, now,event_content, pointer);
						// invia l'evento TM_ABORT_EXECUTED al TM
						InsertEvent(TM, me, now+TIME_FOR_TM_ABORT_EXECUTION, now, TM_ABORT_EXECUTED, event_content);
						return;
					}
					i++;
				}
			}
			if (COM_TIME_READ_LOCK) {
				i=1;
				while((read_set_item=cc_get_tx_read_set_item_by_position(event_content->tx_id, i))!=NULL){
					// verifica che non ci sia un un lock
					if (pointer->cc_dataitems_list[read_set_item->di_id]->tx_id_locker_exclusive !=-1
						&&
						pointer->cc_dataitems_list[read_set_item->di_id]->tx_id_locker_exclusive !=event_content->tx_id) {
						// c'è un lock
						/**************************************/
						// operazioni di raccolta statistiche
						if (TM_PRINT_STAT) stat_set_op_com_time_read_lock_contention(event_content, read_set_item->run_op_number, now);
						/**************************************/
						// abortisce la transazione e risponde al TM
						abort_transaction(me,now,event_content, pointer);
						// invia l'evento di TM_ABORT_EXECUTED al TM
						InsertEvent(TM, me, now+TIME_FOR_TM_ABORT_EXECUTION, now, TM_ABORT_EXECUTED, event_content);
						return;
					}
					i++;
				}
				// ha validato il read set
			}

			// aggiorna i timestamp dei data items
			i=1;
			while((item=cc_get_tx_write_set_item_by_position(event_content->tx_id, i))!=NULL){
				pointer->cc_dataitems_list[item->di_id]->commit_timestamp=now;
				i++;
			}
			// toglie i locks
			unlock_data_items(event_content->tx_id, pointer, event_content, now);
			// sveglia le eventuali prime transazioni in attesa per ogni dataitem lockato dalla txF
			wake_up_first_waiting_transactions(event_content->tx_id, me, now, pointer);
			cc_remove_transaction(event_content->tx_id);
			InsertEvent(TM, me, now, now, TM_COMMIT_EXECUTED, event_content);
			// invia l'evento TM_COMMIT_EXECUTED al TM
			break;
	}
}


