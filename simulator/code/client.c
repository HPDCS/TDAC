#include "stdafx.h"
#include <time.h>

#include <stdlib.h>
#include "generale.h"
#include "distribution_generator.h"

int number_of_threads=MIN_NUMBER_OF_THREAD;
double last_stat_thread_time=0;
void print_stat_header(	double now) {
	printf("\nTxs iniz.\truns iniz.\ttxs commit.\trun abr\tin run\tin bkf\tthrougput\ttime\n\n");
}

void print_stat(CLIENT_lp_state_type * pointer, double now) {
	printf("%i\t\t%i\t\t%i\t\t%i\t%i\t%i\t%f\t%f\n",
		pointer->next_transaction_id,
			pointer->started_runs,
				pointer->committed_transactions,
					pointer->aborted_runs,
						pointer->active_transactions,
							pointer->in_backoff_transactions,
								pointer->next_transaction_id/now,
								now
					);
	fflush(stdout);
}

void create_a_new_transaction(CLIENT_lp_state_type * pointer, double now) {
	create_new_transaction(&pointer->tx_seed, &pointer->transaction_list,0,pointer->next_transaction_id, DATA_ITEMS_ACCESS_DISTRIBUTION, (double)DATA_ITEMS_ZIPF_CONST);

}

transaction_descriptor* transaction_run_request(int tx_id, int run_number, double tt,CLIENT_lp_state_type * pointer, double now) {
	transaction_descriptor *tx_descr = get_transaction(tx_id,pointer->transaction_list);
	//creazione del nuovo evento
	event_content_type new_event_content;
	new_event_content.tx_id=tx_descr->tx_id;
	new_event_content.tx_class_id=tx_descr->tx_class_id;
	new_event_content.tx_run_number=run_number;
	InsertEvent(TM, CLIENT, now+tt, now , TM_BEGIN, &new_event_content);
	InsertEvent(CLIENT, CLIENT, now+tt, now , RUN_STARTED, &new_event_content);
	if (CLIENT_VERBOSE) printf("- Funzione Client_ProcessEvent: inviato evento TM_BEGIN a TM con tx_id %i al tempo %f\n",tx_id,now+tt);
	return tx_descr;
}

operation_descriptor *next_operation_request(int tx_id, int tx_run_number, double tt,CLIENT_lp_state_type * pointer, double now) {
	transaction_descriptor *tx_descr = get_transaction(tx_id, pointer->transaction_list);
	operation_descriptor *op_descr = get_next_operation(tx_descr);
	if (op_descr!=NULL)  {
		//creazione del nuovo evento
		event_content_type new_event_content;
		new_event_content.tx_id=tx_id;
		new_event_content.tx_run_number=tx_run_number;
		new_event_content.op_number=op_descr->op_number;
		new_event_content.di_id=op_descr->di_id;
		InsertEvent(TM, CLIENT, now+tt, now, op_descr->op_type, &new_event_content);
		if (CLIENT_VERBOSE) printf("- Funzione Client_ProcessEvent: inviato evento TM_%i a TM con tx_id %i al tempo %f\n", op_descr->op_type,tx_id,now+tt);
		return op_descr;
	} else {
		// le operazioni della transazione sono finite
		return NULL;
	}
}

void commit_request(int tx_id, int tx_run_number, double tt, double now){
		//creazione del nuovo evento
		event_content_type new_event_content;
		new_event_content.tx_id=tx_id;
		new_event_content.tx_run_number=tx_run_number;
		InsertEvent(TM, CLIENT, now+tt, now, TM_COMMIT, &new_event_content);
		if (CLIENT_VERBOSE) printf("- Funzione Client_ProcessEvent: inviato evento TM_COMMIT a TM con tx_id %i al tempo %f\n",tx_id,now+tt);
}
void CLIENT_ProcessEvent(int me,double now, int event_type, event_content_type *event_content, CLIENT_lp_state_type * pointer) {

	event_content_type new_event_content;
	double tt;
	switch (event_type)	{

		case INIT:

			if (CLIENT_VERBOSE) printf("- Funzione Client_ProcessEvent: ricevuto evento INIT\n");
			//oper di inizializzazione
			pointer=(CLIENT_lp_state_type*)malloc(sizeof(CLIENT_lp_state_type));
			if (MALLOC_FAIL_VERBOSE && pointer==NULL) printf("---- Funzione Client_ProcessEvent: MALLOC FALLITA\n");
			SetPointer(me,pointer);
			pointer->number_of_threads=number_of_threads;
			int i,j;
			for (i=0;i<MAX_NUMBER_OF_THREAD+1;i++) {
				for (j=0;j<MAX_NUMBER_OF_THREAD+1;j++) {
					pointer->threads_state_time[i][j]=0;
				}
			}
			pointer->transaction_list=NULL;
			pointer->next_transaction_id=0,
			pointer->committed_transactions=0;
			pointer->active_transactions=0;
			pointer->in_backoff_transactions=0;
			pointer->aborted_runs=0;
			pointer->started_runs=0;
			pointer->tx_seed=time(NULL);
			pointer->ending=0;

			last_stat_thread_time=0;

			for (i=0;i<pointer->number_of_threads;i++) {
				if (pointer->next_transaction_id < NUMBER_OF_TRANSACTIONS) {
					create_a_new_transaction(pointer, now);
					tt=Expent(&pointer->tx_seed,((double)20));
					transaction_run_request(pointer->next_transaction_id,0,tt, pointer, now);
					pointer->next_transaction_id++;
				}
			}
			if (PRINT_EXECUTION_INFO) {
				print_stat_header(now);
			}
			break;

		case TM_BEGIN_EXECUTED:
			if (CLIENT_VERBOSE) printf("- Funzione Client_ProcessEvent: ricevuto evento TM_BEGIN_EXECUTED con tx_id %i al tempo %f\n",event_content->tx_id,now);
			tt=Expent(&pointer->tx_seed,((double)TCB_TIME));
			next_operation_request(event_content->tx_id,event_content->tx_run_number, tt, pointer, now);
			break;

		case TM_OPERATION_EXECUTED:
			if (CLIENT_VERBOSE) printf("- Funzione Client_ProcessEvent: ricevuto evento TM_OPERATION_EXECUTED per op %i della tx %i al tempo %f\n",	event_content->op_number, event_content->tx_id,now);
			tt=TCB_TIME;//Expent(&pointer->tx_seed,((double)TCB_TIME));
			if (next_operation_request(event_content->tx_id, event_content->tx_run_number, tt, pointer, now)==NULL) {
				commit_request(event_content->tx_id,event_content->tx_run_number, tt, now);
			}
			break;

		case TM_ABORT_EXECUTED:
			if (CLIENT_VERBOSE) printf("- Funzione Client_ProcessEvent: ricevuto evento TM_ABORT_EXECUTED con tx_id %i al tempo %f\n",event_content->tx_id,now);
			// statistiche sullo stato dei threads
			//--------------------------------------
			pointer->threads_state_time
				[pointer->active_transactions]
					[pointer->in_backoff_transactions]+=
						now-last_stat_thread_time;
			last_stat_thread_time=now;
			//--------------------------------------
			pointer->active_transactions--;
			pointer->in_backoff_transactions++;
			pointer->aborted_runs++;
			// effettua il rerun della transzione
			// tempo di backoff
			tt=Expent(&pointer->tx_seed,((double)BACKOFF_TIME));
			//resetta lo stato della tx
			reset_transaction_state(&pointer->tx_seed, get_transaction(event_content->tx_id, pointer->transaction_list), DATA_ITEMS_ACCESS_DISTRIBUTION, (double)DATA_ITEMS_ZIPF_CONST);
			//effettua il rerun
			transaction_run_request(event_content->tx_id,event_content->tx_run_number+1,tt, pointer, now);
			break;

		case RUN_STARTED:
			// statistiche sullo stato dei threads
			//--------------------------------------
			pointer->threads_state_time
				[pointer->active_transactions]
					[pointer->in_backoff_transactions]+=
						now-last_stat_thread_time;
			last_stat_thread_time=now;
			//--------------------------------------
			pointer->active_transactions++;
			pointer->started_runs++;
			if (event_content->tx_run_number>0)
				pointer->in_backoff_transactions--;
			if (PRINT_EXECUTION_INFO) {
				if (((pointer->next_transaction_id*100)%NUMBER_OF_TRANSACTIONS)==0) {
					print_stat(pointer, now);
				}
			}
			break;

		case TM_COMMIT_EXECUTED:
			if (CLIENT_VERBOSE) printf("- Funzione Client_ProcessEvent: ricevuto evento TM_COMMIT_EXECUTED per tx %i al tempo %f\n",event_content->tx_id,now);
			// statistiche sullo stato dei threads
			//--------------------------------------
			pointer->threads_state_time
				[pointer->active_transactions]
					[pointer->in_backoff_transactions]+=
						now-last_stat_thread_time;
			last_stat_thread_time=now;
			//--------------------------------------
			pointer->committed_transactions++;
			pointer->active_transactions--;
			remove_transaction(event_content->tx_id,&pointer->transaction_list);
			//if (pointer->next_transaction_id>=NUMBER_OF_TRANSACTIONS) printf("- Funzione Client_ProcessEvent: transazioni eseguite %i\n",pointer->executed_transactions);
			if (CLIENT_VERBOSE) printf("- Funzione Client_ProcessEvent: transazioni committate %i\n",pointer->committed_transactions);
			// verifica se ci sono altre transazioni da generare
			if (pointer->next_transaction_id < NUMBER_OF_TRANSACTIONS &&  now<SIMULATION_TIME) {
				create_a_new_transaction(pointer, now);
				tt=Expent(&pointer->tx_seed,((double)NTCB_TIME));
				transaction_run_request(pointer->next_transaction_id,0,tt, pointer, now);
			} else {
				// termine della simulazione
				if (pointer->ending==0) {
					pointer->ending=1;
					/**************************************/
					// stampa delle statistiche finali
					printf("\n%d\t%f",pointer->number_of_threads, pointer->next_transaction_id/now);
					InsertEvent(STAT_COLLECTOR, me, now+2*simbolicDelay, now, PRINT_STAT, &new_event_content);
					/**************************************/
					//	printf("threads: %i\ttime: %f\tthroughput: %f\ttxs_iniz: %i\trun iniz: %i\n",
					//		pointer->number_of_threads,
					//			now,
					//				pointer->next_transaction_id/now,
					//					pointer->next_transaction_id,
					//						pointer->started_runs);
					//printf("\t%f",pointer->next_transaction_id/now);

					if (PRINT_STATS_ON_FILE) {
						pointer->output_file=fopen(OUTPUT_FILE_NAME,"w");
						fprintf(pointer->output_file,"threads: %i\ttime: %f\tthroughput: %f\ttxs_iniz: %i\trun iniz: %i\n",
							pointer->number_of_threads,
								now,
									pointer->next_transaction_id/now,
										pointer->next_transaction_id,
											pointer->started_runs);
						fclose(pointer->output_file);
					}

					if (PRINT_THREAD_STATES) {
						// stampa delle statistiche sullo stato dei threads
						double tot_time=0;
						//--------------------------------------
						printf("\n");
						for (j=0;j<pointer->number_of_threads+1;j++) {
							for (i=0;i<pointer->number_of_threads+1-j;i++) {
								tot_time+=pointer->threads_state_time[i][j]/now;
								printf("%f\t", pointer->threads_state_time[i][j]/now);
							}
							printf("\n");
						}
						printf("totat time: %f\t", tot_time);
					}
				}
				// aspetta che tutte le transazioni abbiano terminato
				if (pointer->active_transactions+pointer->in_backoff_transactions==0) {
					if (pointer->number_of_threads<MAX_NUMBER_OF_THREAD){
						// resetta la simulazione con il numero di thread incrementato del passo
						// invia gli eventi di INIT
						for(i=0;i<N_OGGETTI;i++){
							InsertEvent(i, CLIENT, 0, 0 , INIT,NULL);
						}
						number_of_threads=number_of_threads+NUMBER_OF_THREAD_STEP;
					}
				}
			}
			pointer->next_transaction_id++;
			break;
	}
}

int CLIENT_check_termination(CLIENT_lp_state_type *snapshot){
	if (
		(snapshot[CLIENT].committed_transactions<NUMBER_OF_TRANSACTIONS)
			||
				(snapshot[CLIENT].termination))
		return 0;
	else
		return 1;
}
