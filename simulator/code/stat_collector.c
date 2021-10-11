#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include "generale.h"

stat_transaction_descriptor **stat_tx_list;
int tx_class_lenght[NUMBER_OF_TX_CLASS] = TX_CLASS_LENGHT;

// inserisce una transazione alla fine della lista
void stat_enqueue_transaction(int tx_id, int tx_class_id, double begin_req_time) {
	// crea la nuova tx
	stat_transaction_descriptor *stat_tx_descr=(stat_transaction_descriptor *)malloc(sizeof(stat_transaction_descriptor));
	if (MALLOC_FAIL_VERBOSE && stat_tx_descr==NULL) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: MALLOC FALLITA\n");
	stat_tx_descr->tx_id=tx_id;
	stat_tx_descr->tx_class_id=tx_class_id;
	stat_tx_descr->begin_req_time=begin_req_time;
	stat_tx_descr->first_run=NULL;
	stat_tx_descr->next=NULL;
	// scorre la lista fino alla prima tx con next nullo
  	if((*stat_tx_list)==NULL){
        	*stat_tx_list=stat_tx_descr;

	} else {
		stat_transaction_descriptor *stat_tx_list_temp=*stat_tx_list;
      		while(stat_tx_list_temp->next) {
			stat_tx_list_temp=stat_tx_list_temp->next;
		}
		stat_tx_list_temp->next=stat_tx_descr;
	}
}

//restituisce la transazione tx_id
stat_transaction_descriptor *stat_get_transaction(int tx_id) {
	//scorre la lista delle tx fino a quando non trova l'id della tx
	if(*stat_tx_list==NULL){
        	return NULL;
	} else {
		stat_transaction_descriptor_pointer tx_temp=*stat_tx_list;
      		while(tx_temp->tx_id!=tx_id) {

			tx_temp=tx_temp->next;
			if (tx_temp==NULL) return NULL;
		}
		return tx_temp;
	}
}

// inserisce un run alla fine della lista dei runs della transazione
void stat_enqueue_run(int tx_id, int run_number, double run_begin_req_time) {
	// crea il nuovo run
    stat_transaction_descriptor *stat_tx_temp;
	stat_tx_run_descriptor *stat_run_descr=(stat_tx_run_descriptor *)malloc(sizeof(stat_tx_run_descriptor));
	if (MALLOC_FAIL_VERBOSE && stat_run_descr==NULL) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: MALLOC FALLITA\n");
	stat_run_descr->run_number=run_number;
	stat_run_descr->begin_req_time=run_begin_req_time;
	stat_run_descr->begin_start_wait_time=0;
	stat_run_descr->begin_end_wait_time=0;
	stat_run_descr->commit_time=0;
	stat_run_descr->abort_time=0;
	stat_run_descr->abort_for_commit=0;
	stat_run_descr->abort_for_invalidation=0;
	stat_run_descr->first_operation=NULL;
	stat_run_descr->next=NULL;

	// prende la tx
	stat_tx_temp=stat_get_transaction(tx_id);
	// scorre la lista fino al primo run con next nullo
  	if((stat_tx_temp->first_run)==NULL){
        	stat_tx_temp->first_run=stat_run_descr;
	} else {
		stat_tx_run_descriptor *stat_run_temp=stat_tx_temp->first_run;
      		while(stat_run_temp->next) {
			stat_run_temp=stat_run_temp->next;
		}
		stat_run_temp->next=stat_run_descr;
	}
}

//restituisce il run run_number della transazione tx_id
stat_tx_run_descriptor *stat_get_tx_run(int tx_id, int run_number) {
	// prende la tx
	stat_transaction_descriptor *tx_temp=stat_get_transaction(tx_id);
	// prende il primo run della tx
	stat_tx_run_descriptor *tx_first_run=tx_temp->first_run;
	//scorre la lista dei runs fino a quando non trova l'id del run
	if(tx_first_run==NULL){
        	return NULL;
	} else {
		stat_tx_run_descriptor *tx_run_temp=tx_first_run;
      		while(tx_run_temp->run_number!=run_number) {
			tx_run_temp=tx_run_temp->next;
		}
		return tx_run_temp;
	}
}



// inserisce un'operatione alla fine della lista delle operazioni del run della transazione
void stat_enqueue_operation(int tx_id, int run_number, int op_number, int op_type, int op_di_id, double op_req_time) {
	// crea la nuova operazione
	stat_operation_descriptor *stat_operation_descr=(stat_operation_descriptor *)malloc(sizeof(stat_operation_descriptor));
	if (MALLOC_FAIL_VERBOSE && stat_operation_descr==NULL) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: MALLOC FALLITA\n");
	stat_operation_descr->op_number=op_number;
	stat_operation_descr->op_type=op_type;
	stat_operation_descr->op_di_id=op_di_id;
	stat_operation_descr->op_enc_time_read_lock_contention=0;
	stat_operation_descr->op_com_time_read_lock_contention=0;
	stat_operation_descr->op_enc_time_validation_fail=0;
	stat_operation_descr->op_com_time_validation_fail=0;
	stat_operation_descr->op_enc_time_write_lock_contention=0;
	stat_operation_descr->op_com_time_write_lock_contention=0;
	stat_operation_descr->op_write_lock_request_time=0;
	stat_operation_descr->op_write_lock_release_time=0;
	stat_operation_descr->next=NULL;

	// prende il run
	stat_tx_run_descriptor *run_temp=stat_get_tx_run(tx_id, run_number);
	// scorre la lista fino alla prima operazione con next nullo
  	if((run_temp->first_operation)==NULL){
        	run_temp->first_operation=stat_operation_descr;
	} else {
		stat_operation_descriptor *stat_operation_temp=run_temp->first_operation;
      		while(stat_operation_temp->next) {
			stat_operation_temp=stat_operation_temp->next;
		}
		stat_operation_temp->next=stat_operation_descr;
	}
}

//restituisce l'operazione op_number del run run_number della transazione tx_id
stat_operation_descriptor *stat_get_operation(int tx_id, int run_number, int op_number) {

	// prende il run
	stat_tx_run_descriptor *run_temp=stat_get_tx_run(tx_id, run_number);
	// prende la prima operazione
	stat_operation_descriptor *first_operation=run_temp->first_operation;
	//scorre la lista delle operazioni fino a quando non trova l'op_number dell'operazione
	if(first_operation==NULL){
        	return NULL;
	} else {
		stat_operation_descriptor *operation_temp=first_operation;
      		while(operation_temp->op_number!=op_number) {
			operation_temp=operation_temp->next;

		}
		return operation_temp;
	}
}

void stat_remove_all_data_items(stat_tx_run_descriptor *run_desc) {
	stat_operation_descriptor *to_delete;
	if(run_desc->first_operation==NULL) {
		return;
	} else {
		while(run_desc->first_operation!=NULL){
			to_delete=run_desc->first_operation;
			run_desc->first_operation=run_desc->first_operation->next;
			// libera la memoria dell'operazione
			free(to_delete);
		}
		return;
	}
}

void stat_remove_all_runs(stat_transaction_descriptor *tx_descr) {
	stat_tx_run_descriptor *to_delete;
	if(tx_descr->first_run==NULL) {
		return;
	} else {
		while(tx_descr->first_run!=NULL){
			to_delete=tx_descr->first_run;
			tx_descr->first_run=tx_descr->first_run->next;
			// libera la memoria del run
			free(to_delete);
		}
		return;
	}
}

void stat_remove_transaction(int tx_id) {
	// scorre la lista fino alla tx tx_id
	if((*stat_tx_list)==NULL) {
		return;
	} else {
		if((*stat_tx_list)->tx_id==tx_id){
			stat_transaction_descriptor *to_delete=*stat_tx_list;
			*stat_tx_list=(*stat_tx_list)->next;
			// libera la memoria dei runs,
			stat_remove_all_runs(to_delete);
			// libera la memoria della transazione
			free(to_delete);
			return;
		} else {
			stat_transaction_descriptor *tx_temp=*stat_tx_list;
      			while(tx_temp->next) {
				if(tx_temp->next->tx_id==tx_id){
					stat_transaction_descriptor *to_delete=tx_temp->next;
					tx_temp->next=tx_temp->next->next;
					// libera la memoria dei runs,
					stat_remove_all_runs(to_delete);
					// libera la memoria della transazione
					free(to_delete);
					return;
				}
				tx_temp=tx_temp->next;
			}
			return;
		}
	}
}

void print_stats() {
	stat_transaction_descriptor *tx_temp;
	stat_tx_run_descriptor *run_temp;
	stat_operation_descriptor *op_temp;

	// prende la prima tx
	tx_temp=*stat_tx_list;
	while(tx_temp){
		printf("----------------------------------------------------------------------\n");
		printf("----------------------------------------------------------------------\n");
		printf("tx_id:%i\n",tx_temp->tx_id);
		// prende il primo run
		run_temp=tx_temp->first_run;
		while(run_temp){
		printf(" ---------------------------------------------------------------------\n");
			printf(" run:%i\tbeg_req:%f\tstart_wait:%f\tend_wait:%f\tcomt:%f\tabt:%f\n",run_temp->run_number,run_temp->begin_req_time,run_temp->begin_start_wait_time,run_temp->begin_end_wait_time,run_temp->commit_time,run_temp->abort_time);
			// prende la prima operazione
			op_temp=run_temp->first_operation;
			while(op_temp){

				// prende la prossima operazione
				op_temp=op_temp->next;
			};
			// prende il prossimo run
			run_temp=run_temp->next;
		};
		// prende la prossima tx
		tx_temp=tx_temp->next;
	};
	printf("----------------------------------------------------------------------\n");
	printf("----------------------------------------------------------------------\n");
}

void sum_stat(int tx_id, STAT_COLLECTOR_lp_state_type * pointer) {

	stat_transaction_descriptor *tx_temp;
	stat_tx_run_descriptor *run_temp;
	stat_operation_descriptor *op_temp;


	int current_op=0;

	//prende la tx
	tx_temp=stat_get_transaction(tx_id);
	pointer->stat_total_tx_number[tx_temp->tx_class_id]++;
	// prende il primo run
	run_temp=tx_temp->first_run;

	while(run_temp){

		pointer->stat_total_run_number[tx_temp->tx_class_id]++;

		// prende la prima operazione
		op_temp=run_temp->first_operation;
		current_op=0;
		while(op_temp){
			pointer->stat_total_op_number[tx_temp->tx_class_id]++;
			if (op_temp->op_type==TM_WRITE) {
				//numero di write
				pointer->stat_total_writes_number_per_di[op_temp->op_di_id]++;
				pointer->stat_total_writes_number_per_tx[tx_temp->tx_class_id]++;
				//numero di write locks richiesti
				if (op_temp->op_write_lock_request_time>0)
					pointer->stat_total_op_com_time_write_lock_requests[op_temp->op_di_id]++;
				//numero di write locks acquisiti
				if (op_temp->op_write_lock_release_time>0)
					pointer->stat_total_op_com_time_write_lock_acquired[op_temp->op_di_id]++;
				//durata dei write lock
				if (op_temp->op_write_lock_release_time>0)
					pointer->stat_total_op_com_time_write_lock_time[op_temp->op_di_id]+=op_temp->op_write_lock_release_time-op_temp->op_write_lock_request_time;
				//numero di abort per contese write-write
#ifdef TL2_EAGER
				if (op_temp->op_enc_time_write_lock_contention>0) {
					pointer->stat_total_op_aborted_for_enc_time_write_lock_contention[op_temp->op_di_id]++;
					pointer->stat_total_aborts_per_operation[current_op]++;
				}
#endif
#ifdef TL2_LAZY
				if (op_temp->op_com_time_write_lock_contention>0) {
					pointer->stat_total_op_aborted_for_com_time_write_lock_contention[op_temp->op_di_id]++;
				}
#endif
			} else { // è una read
				//numero di read
				pointer->stat_total_reads_number_per_di[op_temp->op_di_id]++;
				pointer->stat_total_reads_number_per_tx[tx_temp->tx_class_id]++;
				//numero di abort per contese enc time read-write
				if (op_temp->op_enc_time_read_lock_contention>0) {
					pointer->stat_total_op_aborted_for_enc_time_read_lock_contention[op_temp->op_di_id]++;
					pointer->stat_total_aborts_per_operation[current_op]++;
				}
				//numero di abort per contese com time read-write
				if (op_temp->op_com_time_read_lock_contention>0)
					pointer->stat_total_op_aborted_for_com_time_read_lock_contention[op_temp->op_di_id]++;
				//numero di abort per validazioni enc time
				if (op_temp->op_enc_time_validation_fail>0) {
					pointer->stat_total_op_aborted_for_enc_time_validation_fail[op_temp->op_di_id]++;
					pointer->stat_total_aborts_per_operation[current_op]++;
				}
				//numero di abort per validazioni com time
				if (op_temp->op_com_time_validation_fail>0)
					pointer->stat_total_op_aborted_for_com_time_validation_fail[op_temp->op_di_id]++;
			}

			// prende la prossima operazione
			op_temp=op_temp->next;
			current_op++;
		};
		if (run_temp->commit_time>0)
			pointer->stat_total_committed_run_number[tx_temp->tx_class_id]++;
		// aggiunge il response time del run
		pointer->stat_total_run_response_time[tx_temp->tx_class_id]+=(run_temp->commit_time+run_temp->abort_time)-run_temp->begin_req_time;
		// prende il prossimo run
		run_temp=run_temp->next;
	};
	// aggiunge il response time della tx
	pointer->stat_total_tx_response_time[tx_temp->tx_class_id]+=(tx_temp->commit_time-tx_temp->begin_req_time);
}

void print_average_value(double now, STAT_COLLECTOR_lp_state_type * pointer){
	int i;
	int commit=0;
	int runs=0;
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		commit+=pointer->stat_total_committed_run_number[i];
		runs+=pointer->stat_total_run_number[i];
	}
	int number_of_runs=0;
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		number_of_runs+=pointer->stat_total_run_number[i];
	}
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		printf("\t%f",(double)commit/(double)number_of_runs);
	}
}

void print_average_value1(double now, STAT_COLLECTOR_lp_state_type * pointer){
	int i;
	printf("-------------------------------------------------\n");
	printf("-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			printf("tx eseguite di classe [%i]\t\t %i\n",i,
				pointer->stat_total_tx_number[i]);
	}
	printf("-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			printf("resp. time tx di classe [%i]\t\t %f\n",i,
				(double)pointer->stat_total_tx_response_time[i] /
				(double)pointer->stat_total_tx_number[i]);
	}
	printf("-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			printf("transactional time ratio classe [%i]\t %f\n",i,
				(double)pointer->stat_total_tx_response_time[i] / now);
	}
	printf("-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			printf("runs eseguiti di classe [%i]\t\t %i\n",i,
				pointer->stat_total_run_number[i]);
	}
	printf("-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			printf("runs committati di classe [%i]\t\t %i\n",i,
				pointer->stat_total_committed_run_number[i]);
	}
	printf("-------------------------------------------------\n");
	int commit=0;
	int runs=0;
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		commit+=pointer->stat_total_committed_run_number[i];
		runs+=pointer->stat_total_run_number[i];
	}
	printf("prob. di commit per run di classe [%i]\t %f\n",i,(double)commit/(double)runs);
	printf("-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			printf("resp.time per run di classe [%i]\t\t %f\n",i,
				(double)pointer->stat_total_run_response_time[i] /
				(double)pointer->stat_total_run_number[i]);
	}
	printf("-------------------------------------------------\n");
	int reads=0;
	int writes=0;
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		reads+=pointer->stat_total_reads_number_per_tx[i];
		writes+=pointer->stat_total_writes_number_per_tx[i];
	}
	printf("reads eseguite\t\t\t\t %i\n",reads);
	printf("writes eseguite\t\t\t\t %i\n",writes);
	printf("read rate\t\t\t\t %f\n",(double)reads/(double)now);
	printf("write rate\t\t\t\t %f\n",(double)writes/(double)now);
	printf("-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		printf("reads per run eseguite di classe [%i]\t %f\n",i,
			(double)pointer->stat_total_reads_number_per_tx[i]/
				(double)pointer->stat_total_run_number[i]);
		printf("writes per run eseguite di classe [%i]\t %f\n",i,
			(double)pointer->stat_total_writes_number_per_tx[i]/
				(double)pointer->stat_total_run_number[i]);
		printf("op per run eseguite di classe [%i]\t %f\n",i,
			((double)pointer->stat_total_reads_number_per_tx[i]+(double)pointer->stat_total_writes_number_per_tx[i])/
			(double)pointer->stat_total_run_number[i]);
	}


	printf("-------------------------------------------------\n");
	int exc_locks_requests=0;
	int exc_locks_acquired=0;
	for (i=0;i<NUMBER_OF_DATA_ITEMS; i++) {
		exc_locks_requests+=pointer->stat_total_op_com_time_write_lock_requests[i];
		exc_locks_acquired+=pointer->stat_total_op_com_time_write_lock_acquired[i];
	}
	printf("locks exc richiesti a ct\t\t %i\n",exc_locks_requests);
	printf("locks exc acquisiti a ct\t\t %i\n",exc_locks_acquired);
	printf("-------------------------------------------------\n");
	double di_utilization=0;
	for (i=0;i<NUMBER_OF_DATA_ITEMS; i++) {
		di_utilization+=pointer->stat_total_op_com_time_write_lock_time[i];
	}
	printf("utilizzazione media dei data items\t %f\n", ((double)di_utilization/(double)NUMBER_OF_DATA_ITEMS)/now);
	printf("-------------------------------------------------\n");
	int enc_time_validation_fail=0;
	int com_time_validation_fail=0;
	int enc_time_read_lock_contention=0;
	int com_time_read_lock_contention=0;
	int enc_time_write_lock_contention=0;
	int com_time_write_lock_contention=0;

	for (i=0;i<NUMBER_OF_DATA_ITEMS; i++) {
		enc_time_validation_fail+=pointer->stat_total_op_aborted_for_enc_time_validation_fail[i];
		com_time_validation_fail+=pointer->stat_total_op_aborted_for_com_time_validation_fail[i];
		enc_time_read_lock_contention+=pointer->stat_total_op_aborted_for_enc_time_read_lock_contention[i];
		com_time_read_lock_contention+=pointer->stat_total_op_aborted_for_com_time_read_lock_contention[i];
#ifdef TL2_EAGER
		enc_time_write_lock_contention+=pointer->stat_total_op_aborted_for_enc_time_write_lock_contention[i];
#endif
#ifdef TL2_LAZY
		com_time_write_lock_contention+=pointer->stat_total_op_aborted_for_com_time_write_lock_contention[i];
#endif
	}
	int number_of_runs=0;
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		number_of_runs+=pointer->stat_total_run_number[i];
	}
	printf("run abr pr. enc time \t\t\t %f\n", (double)enc_time_validation_fail/(double)number_of_runs+(double)enc_time_read_lock_contention/(double)number_of_runs);
#ifdef TL2_EAGER
	printf("run abr pr. enc time write lock cont.\t %f\n", (double)enc_time_write_lock_contention/(double)number_of_runs);
#endif
#ifdef TL2_LAZY
	printf("run abr pr. com time write lock cont.\t %f\n", (double)com_time_write_lock_contention/(double)number_of_runs);
#endif
	printf("run abr pr. com time read val fail\t %f\n", (double)com_time_validation_fail/(double)number_of_runs);
	printf("run abr pr. com time read lock cont.\t %f\n", (double)com_time_read_lock_contention/(double)number_of_runs);
	printf("-------------------------------------------------\n");

}

void print_average_value2(double now, STAT_COLLECTOR_lp_state_type * pointer){
	int i;
	int commit=0;
	int runs=0;
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		commit+=pointer->stat_total_committed_run_number[i];
		runs+=pointer->stat_total_run_number[i];
	}
	int number_of_runs=0;
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		number_of_runs+=pointer->stat_total_run_number[i];
	}
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		printf("\t%f",(double)commit/(double)number_of_runs);
	}
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			printf("\t%f",
				(double)pointer->stat_total_run_response_time[i] /
				(double)pointer->stat_total_tx_number[i]);
	}
	int enc_time_read_lock_contention=0;
	int enc_time_write_lock_contention=0;
	int enc_time_validation_fail=0;
	int com_time_validation_fail=0;
	int com_time_read_lock_contention=0;
	int com_time_write_lock_contention=0;
	for (i=0;i<NUMBER_OF_DATA_ITEMS; i++) {
		enc_time_read_lock_contention+=pointer->stat_total_op_aborted_for_enc_time_read_lock_contention[i];
		enc_time_validation_fail+=pointer->stat_total_op_aborted_for_enc_time_validation_fail[i];
		enc_time_write_lock_contention+=pointer->stat_total_op_aborted_for_enc_time_write_lock_contention[i];
		com_time_write_lock_contention+=pointer->stat_total_op_aborted_for_com_time_write_lock_contention[i];
		com_time_validation_fail+=pointer->stat_total_op_aborted_for_com_time_validation_fail[i];
		com_time_read_lock_contention+=pointer->stat_total_op_aborted_for_com_time_read_lock_contention[i];
	}

	printf("\t%f", (double)enc_time_read_lock_contention/(double)number_of_runs);
	printf("\t%f", (double)enc_time_validation_fail/(double)number_of_runs);
	printf("\t%f", (double)enc_time_write_lock_contention/(double)number_of_runs);
	printf("\t%f", (double)com_time_write_lock_contention/(double)number_of_runs);
	printf("\t%f", (double)com_time_validation_fail/(double)number_of_runs);
	printf("\t%f", (double)com_time_read_lock_contention/(double)number_of_runs);

	for (i=0;i<tx_class_lenght[0]; i++) {
		//printf("\n%f", (double)pointer->stat_total_aborts_per_operation[i]/(double)number_of_runs);
	}
}

void print_average_value_on_file(double now, STAT_COLLECTOR_lp_state_type * pointer){
	int i;
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			fprintf(pointer->output_file,"tx eseguite di classe [%i]\t\t %i\n",i,
				pointer->stat_total_tx_number[i]);
	}
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			fprintf(pointer->output_file,"resp. time tx di classe [%i]\t\t %f\n",i,
				(double)pointer->stat_total_tx_response_time[i] /
				(double)pointer->stat_total_tx_number[i]);
	}
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			fprintf(pointer->output_file,"transactional time ratio classe [%i]\t %f\n",i,
				(double)pointer->stat_total_tx_response_time[i] / now);
	}
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			fprintf(pointer->output_file,"runs eseguiti di classe [%i]\t\t %i\n",i,
				pointer->stat_total_run_number[i]);
	}
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			fprintf(pointer->output_file,"runs committati di classe [%i]\t\t %i\n",i,
				pointer->stat_total_committed_run_number[i]);
	}
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	int commit=0;
	int runs=0;
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		commit+=pointer->stat_total_committed_run_number[i];
		runs+=pointer->stat_total_run_number[i];
	}
	fprintf(pointer->output_file,"prob. di commit per run di classe [%i]\t %f\n",i,(double)commit/(double)runs);
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
			fprintf(pointer->output_file,"resp.time per run di classe [%i]\t\t %f\n",i,
				(double)pointer->stat_total_run_response_time[i] /
				(double)pointer->stat_total_run_number[i]);
	}
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	int reads=0;
	int writes=0;
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		reads+=pointer->stat_total_reads_number_per_tx[i];
		writes+=pointer->stat_total_writes_number_per_tx[i];
	}
	fprintf(pointer->output_file,"reads eseguite\t\t\t\t %i\n",reads);
	fprintf(pointer->output_file,"writes eseguite\t\t\t\t %i\n",writes);
	fprintf(pointer->output_file,"read rate\t\t\t\t %f\n",(double)reads/(double)now);
	fprintf(pointer->output_file,"write rate\t\t\t\t %f\n",(double)writes/(double)now);
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		fprintf(pointer->output_file,"reads per run eseguite di classe [%i]\t %f\n",i,
			(double)pointer->stat_total_reads_number_per_tx[i]/
				(double)pointer->stat_total_run_number[i]);
		fprintf(pointer->output_file,"writes per run eseguite di classe [%i]\t %f\n",i,
			(double)pointer->stat_total_writes_number_per_tx[i]/
				(double)pointer->stat_total_run_number[i]);
		fprintf(pointer->output_file,"op per run eseguite di classe [%i]\t %f\n",i,
			((double)pointer->stat_total_reads_number_per_tx[i]+(double)pointer->stat_total_writes_number_per_tx[i])/
			(double)pointer->stat_total_run_number[i]);
	}


	fprintf(pointer->output_file,"-------------------------------------------------\n");
	int exc_locks_requests=0;
	int exc_locks_acquired=0;
	for (i=0;i<NUMBER_OF_DATA_ITEMS; i++) {
		exc_locks_requests+=pointer->stat_total_op_com_time_write_lock_requests[i];
		exc_locks_acquired+=pointer->stat_total_op_com_time_write_lock_acquired[i];
	}
	fprintf(pointer->output_file,"locks exc richiesti a ct\t\t %i\n",exc_locks_requests);
	fprintf(pointer->output_file,"locks exc acquisiti a ct\t\t %i\n",exc_locks_acquired);
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	double di_utilization=0;
	for (i=0;i<NUMBER_OF_DATA_ITEMS; i++) {
		di_utilization+=pointer->stat_total_op_com_time_write_lock_time[i];
	}
	fprintf(pointer->output_file,"utilizzazione media dei data items\t %f\n", ((double)di_utilization/(double)NUMBER_OF_DATA_ITEMS)/now);
	fprintf(pointer->output_file,"-------------------------------------------------\n");
	int enc_time_validation_fail=0;
	int com_time_validation_fail=0;
	int enc_time_read_lock_contention=0;
	int com_time_read_lock_contention=0;
	int com_time_write_lock_contention=0;

	for (i=0;i<NUMBER_OF_DATA_ITEMS; i++) {
		enc_time_validation_fail+=pointer->stat_total_op_aborted_for_enc_time_validation_fail[i];
		com_time_validation_fail+=pointer->stat_total_op_aborted_for_com_time_validation_fail[i];
		enc_time_read_lock_contention+=pointer->stat_total_op_aborted_for_enc_time_read_lock_contention[i];
		com_time_read_lock_contention+=pointer->stat_total_op_aborted_for_com_time_read_lock_contention[i];
		com_time_write_lock_contention+=pointer->stat_total_op_aborted_for_com_time_write_lock_contention[i];
	}
	int number_of_runs=0;
	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		number_of_runs+=pointer->stat_total_run_number[i];
	}
	fprintf(pointer->output_file,"run abr pr. enc time lock+val.fail\t\t %f\n", (double)enc_time_validation_fail/(double)number_of_runs+(double)enc_time_read_lock_contention/(double)number_of_runs);
	fprintf(pointer->output_file,"run abr pr. com time write lock cont.\t %f\n", (double)com_time_write_lock_contention/(double)number_of_runs);
	fprintf(pointer->output_file,"run abr pr. com time val.fail\t\t %f\n", (double)com_time_validation_fail/(double)number_of_runs);
	fprintf(pointer->output_file,"run abr pr. com time read lock cont.\t %f\n", (double)com_time_read_lock_contention/(double)number_of_runs);
	fprintf(pointer->output_file,"-------------------------------------------------\n");
}



void reset_stat(STAT_COLLECTOR_lp_state_type * pointer){

	//inizializza le variabili per le statistiche generali
	int i;
	for(i=0; i<NUMBER_OF_DATA_ITEMS; i++){
		pointer->stat_total_op_aborted_for_com_time_read_lock_contention[i]=0;
		pointer->stat_total_op_aborted_for_com_time_validation_fail[i]=0;
		pointer->stat_total_op_aborted_for_enc_time_write_lock_contention[i]=0;
		pointer->stat_total_op_aborted_for_com_time_write_lock_contention[i]=0;
		pointer->stat_total_op_aborted_for_enc_time_read_lock_contention[i]=0;
		pointer->stat_total_op_aborted_for_enc_time_validation_fail[i]=0;
		pointer->stat_total_op_com_time_write_lock_acquired[i]=0;
		pointer->stat_total_op_com_time_write_lock_requests[i]=0;
		pointer->stat_total_op_com_time_write_lock_time[i]=0;
		pointer->stat_total_op_number[i]=0;
		pointer->stat_total_reads_number_per_di[i]=0;
		pointer->stat_total_writes_number_per_di[i]=0;


	}

	for (i=0;i<NUMBER_OF_TX_CLASS; i++) {
		pointer->stat_total_tx_number[i]=0;
		pointer->stat_total_tx_response_time[i]=0;
		pointer->stat_total_run_number[i]=0;
		pointer->stat_total_committed_run_number[i]=0;
		pointer->stat_total_run_response_time[i]=0;

		pointer->stat_total_reads_number_per_tx[i]=0;
		pointer->stat_total_writes_number_per_tx[i]=0;
	}

	pointer->stat_total_aborts_per_operation=(unsigned int*)malloc(sizeof(unsigned int)*tx_class_lenght[0]);
	for (i=0;i<tx_class_lenght[0]; i++) {
		pointer->stat_total_aborts_per_operation[i]=0;
	}
}

void STAT_COLLECTOR_ProcessEvent(int me,double now, int event_type, event_content_type *event_content, STAT_COLLECTOR_lp_state_type * pointer) {

	stat_transaction_descriptor *tx;
	stat_tx_run_descriptor *run;
	stat_operation_descriptor *op;

	// memorizza nella variabile locale il puntatore alla lista delle tx
	stat_tx_list=&pointer->stat_transactions_list;

	switch (event_type)	{

		case INIT:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento INIT\n");
            pointer = (STAT_COLLECTOR_lp_state_type*)malloc(sizeof(STAT_COLLECTOR_lp_state_type));
			if (MALLOC_FAIL_VERBOSE && pointer==NULL) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: MALLOC FALLITA\n");
			SetPointer(me,pointer);
			//inizializza il puntatore alla lista di statistiche delle transazioni
			pointer->stat_transactions_list=NULL;
			// Azzera le statistiche
			reset_stat(pointer);
			pointer->output_file=fopen(OUTPUT_FILE_NAME,"w");

			if(pointer->output_file==NULL) printf("Errore nell'apertura del file\n");
			break;

		case STAT_ADD_tx:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_ADD_tx con tx_id %i al tempo %f\n",event_content->tx_id, now);
			stat_enqueue_transaction(
				event_content->tx_id,
					event_content->tx_class_id,
					event_content->time);
			break;

		case STAT_ADD_tx_run:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_ADD_tx_run con tx_id %i e tx_run_number %i al tempo %f\n", event_content->tx_id,event_content->tx_run_number, now);
			stat_enqueue_run(
				event_content->tx_id,
				event_content->tx_run_number,
				event_content->time);
			break;


		case STAT_ADD_op:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_ADD_op al tempo %f\n", now);
			stat_enqueue_operation(
				event_content->tx_id,
				event_content->tx_run_number,
				event_content->op_number,
				event_content->op_type,
				event_content->di_id,
				event_content->time);
			break;


		case STAT_SET_run_commit:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_SET_run_commit al tempo %f\n", now);
			//prende la transazione
			tx=stat_get_transaction(event_content->tx_id);
			tx->commit_time=event_content->time;
			//prende il run
			run=stat_get_tx_run(
				event_content->tx_id,
				event_content->tx_run_number);
			run->commit_time=event_content->time;
			// aggiorna le statistiche generali
			if (now>STAT_START_TIME) sum_stat(event_content->tx_id, pointer);
			//rimuove la transazione
			stat_remove_transaction(event_content->tx_id);
			fprintf(pointer->output_file,"%f\n", now);
			break;

		case STAT_SET_run_abort:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_SET_run_commit al tempo %f\n", now);
			//prende il run
			run=stat_get_tx_run(
				event_content->tx_id,
				event_content->tx_run_number);
			run->abort_time=event_content->time;
			break;

		case STAT_SET_op_lock_requested:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_SET_op_lock_requested al tempo %f\n", now);
			//prende l'operazione
			op=stat_get_operation(
				event_content->tx_id,
				event_content->tx_run_number,
				event_content->op_number);
			op->op_write_lock_request_time=event_content->time;

			break;

		case STAT_SET_op_com_time_write_lock_contention:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_SET_op_lock_contention al tempo %f\n", now);
			//prende l'operazione
			op=stat_get_operation(
				event_content->tx_id,
				event_content->tx_run_number,
				event_content->op_number);
				op->op_com_time_write_lock_contention=1;
			break;


		case STAT_SET_op_enc_time_write_lock_contention:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_SET_op_enc_time_write_lock_contention al tempo %f\n", now);
			//prende l'operazione
			op=stat_get_operation(
				event_content->tx_id,
				event_content->tx_run_number,
				event_content->op_number);
				op->op_enc_time_write_lock_contention=1;
			break;

		case STAT_SET_op_lock_reseased:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_SET_op_lock_reseased al tempo %f\n", now);
			//prende l'operazione
			op=stat_get_operation(
				event_content->tx_id,
				event_content->tx_run_number,
				event_content->op_number);
			op->op_write_lock_release_time=event_content->time;
			break;


		case STAT_SET_op_enc_time_validation_fail:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_SET_op_caused_abort_for_invalidation al tempo %f\n", now);
			//prende l'operazione
			op=stat_get_operation(
				event_content->tx_id,
				event_content->tx_run_number,
				event_content->op_number);
			op->op_enc_time_validation_fail=1;
			break;

		case STAT_SET_op_com_time_validation_fail:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_SET_op_caused_abort_for_commit al tempo %f\n", now);
			//prende l'operazione
			op=stat_get_operation(
				event_content->tx_id,
				event_content->tx_run_number,
				event_content->op_number);
			op->op_com_time_validation_fail=1;
			break;

		case STAT_SET_op_enc_time_read_lock_contention:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_SET_op_caused_abort_for_commit al tempo %f\n", now);
			//prende l'operazione
			op=stat_get_operation(
				event_content->tx_id,
				event_content->tx_run_number,
				event_content->op_number);
			op->op_enc_time_read_lock_contention=1;
			break;

		case STAT_SET_op_com_time_read_lock_contention:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento STAT_SET_op_caused_abort_for_commit al tempo %f\n", now);
			//prende l'operazione
			op=stat_get_operation(
				event_content->tx_id,
				event_content->tx_run_number,
				event_content->op_number);
			op->op_com_time_read_lock_contention=1;

			break;

		case PRINT_STAT:
			if (STAT_COLLECTOR_VERBOSE) printf("-------- Funzione STAT_COLLECTOR_ProcessEvent: ricevuto evento PRINT_STAT tempo %f\n", now);
			//if (PRINT_FINAL_STATS_WITH_LABELS) print_average_value(now,pointer);
			//if (PRINT_FINAL_STATS_WITHOUT_LABELS) print_average_value_no_label(now,pointer);
			//if (PRINT_STATS_ON_FILE) print_average_value_on_file(now,pointer);
			//fclose(pointer->output_file);
			break;
	}
}



