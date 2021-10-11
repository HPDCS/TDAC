#include "transaction_list_manager.h"
#include "stat_collector.h"
#include "cc_functions.h"

#define N_OGGETTI 7

#define OPEN					                        1
#define CLOSED					                        2
#define FIXED					                        3
#define UNIFORM					                        4
#define EXPONENTIAL				                        5
#define ZIPF					                        6
#define RANDOM					                        7
#define SINGLE_BAR				                        8
#define UNIFORM_WITH_ASCENDING_ORDER                    9
#define UNIFORM_WITH_INVERSE_ORDERS                     10
#define UNIFORM_ON_TABLES_WITH_ASCENDING_ORDER          11
#define UNIFORM_ON_TABLES_WITH_INVERSE_ORDERS           12
#define UNIFORM_WITH_MIXED_ORDERS			13
#define UNIFORM_ON_TABLES_WITH_MIXED_ORDERS			14
//#define DECREASING_POPULARITY	9

#include "configuration.h"


/***********************************************************************/

// delay minimo
#define simbolicDelay			0.000000000

/***********************************************************************/

/* oggetti di simulazione */
#define	CLIENT			0
#define	NW				1
#define DBNI			2
#define TM				3
#define CC				4
#define GVC				5
#define STAT_COLLECTOR	6


/***********************************************************************/

/* EVENT TYPES */
#define INIT							0
//CLIENT->CLIENT
#define CRATE_NEW_BEGIN_REQUEST			1

//CLIENT->CLIENT
#define RUN_STARTED						2
//TM->CC
#define TM_BEGIN						10
#define TM_READ							11
#define TM_WRITE						12
#define TM_COMMIT						13

//CC->TM
#define TM_BEGIN_EXECUTED				20
#define TM_OPERATION_EXECUTED			21
#define TM_ABORT_EXECUTED				23
#define TM_COMMIT_EXECUTED				24

//CC->GVC
#define GVC_REQUEST						30

//GVC->GVC
#define GVC_REQUEST_SERVED				31

//GVC->CC
#define	GVC_RESPONSE					32
/***********************************************************************/


// struttura rappresentante il contenuto applicativo di un evento
typedef struct _event_content_type {
//	int request_type;
	double next_TT;
	int tx_class_id;
	int tx_id;
	int tx_run_number;
	int op_number;
	int op_type;
  	int di_id;
	double time;
    int origin_object;
    int origin_event_type;
	int number_of_threads;
} event_content_type;

/***********************************************************************/

//strutture e prototipi per le code di attesta degli eventi

typedef struct _wait_event *wait_event_pointer;

typedef struct _wait_event {
	int event_type;
	event_content_type *event_content;
	double insertion_time;
	wait_event_pointer next;
} wait_event;

/***********************************************************************/

typedef struct _event_wait_queue {
	wait_event_pointer top;
	wait_event_pointer bottom;
} event_wait_queue;

/* Funzioni di gestione per la coda di attesa degli eventi*/


/***********************************************************************/

// strutture per gli stati degli ogetti di simulazione
typedef struct _CLIENT_lp_state_type{
	transaction_descriptor *transaction_list;
	int next_transaction_id;
	int committed_transactions;
	int active_transactions;
	int in_backoff_transactions;
	int aborted_runs;
	int started_runs;
	unsigned tx_seed;
	unsigned st_seed;
	double client_tx_avg_rate;
	double client_statements_avg_think_time;
	int number_of_threads;
	double threads_state_time[MAX_NUMBER_OF_THREAD+1][MAX_NUMBER_OF_THREAD+1];
	FILE *output_file;
	int ending;
	int termination;
} CLIENT_lp_state_type;


typedef struct _TM_lp_state_type{
	int termination;
    int actives_transactions;
    event_wait_queue begin_request_wait_queue;
	transaction_descriptor_pointer active_transactions_list;
	int data_item_access_distribution;
	double data_items_zipf_const;
} TM_lp_state_type;

typedef struct _CC_lp_state_type{
	cc_data_item *cc_dataitems_list[NUMBER_OF_DATA_ITEMS];	// lista dei data items del cc
	cc_transaction_descriptor *cc_transactions_list;		// lista delle transazioni da gestire
	cc_data_item *cc_tx_write_set_items_list;				// lista temporanea dei data item lockati
    wait_event **locks_event_wait_list;						//lista temporanea delle tx in attesa per lock
	unsigned int max_locks_event_wait_list_length;
	FILE *output_file;
} CC_lp_state_type;

typedef struct _GVC_lp_state_type{
	int avaiable_servants;
	event_wait_queue requests_queue;
	int served_requests;
	int queue_length;
	unsigned GVC_op_seed;
} GVC_lp_state_type;

typedef struct _STAT_COLLECTOR_lp_state_type{
	stat_transaction_descriptor *stat_transactions_list;
	FILE *output_file;
	int data_Access[NUMBER_OF_DATA_ITEMS];
	double last_stat_time;
	double next_stat_time;
	double stat_total_run_response_time[NUMBER_OF_TX_CLASS];
	double stat_total_tx_response_time[NUMBER_OF_TX_CLASS];
	double stat_total_op_com_time_write_lock_time[NUMBER_OF_DATA_ITEMS];
	unsigned int stat_total_tx_number[NUMBER_OF_TX_CLASS];
	unsigned int stat_total_run_number[NUMBER_OF_TX_CLASS];
	unsigned int stat_total_op_number[NUMBER_OF_TX_CLASS];
	unsigned int stat_total_committed_run_number[NUMBER_OF_TX_CLASS];
	unsigned int stat_total_writes_number_per_tx[NUMBER_OF_TX_CLASS];
	unsigned int stat_total_reads_number_per_tx[NUMBER_OF_TX_CLASS];
	unsigned int stat_total_writes_number_per_di[NUMBER_OF_DATA_ITEMS];
	unsigned int stat_total_reads_number_per_di[NUMBER_OF_DATA_ITEMS];
	unsigned int stat_total_op_aborted_for_com_time_validation_fail[NUMBER_OF_DATA_ITEMS];
	unsigned int stat_total_op_aborted_for_enc_time_validation_fail[NUMBER_OF_DATA_ITEMS];
	unsigned int stat_total_op_aborted_for_enc_time_read_lock_contention[NUMBER_OF_DATA_ITEMS];
	unsigned int stat_total_op_aborted_for_com_time_read_lock_contention[NUMBER_OF_DATA_ITEMS];
	unsigned int stat_total_op_aborted_for_enc_time_write_lock_contention[NUMBER_OF_DATA_ITEMS];
	unsigned int stat_total_op_aborted_for_com_time_write_lock_contention[NUMBER_OF_DATA_ITEMS];
	unsigned int stat_total_op_com_time_write_lock_requests[NUMBER_OF_DATA_ITEMS];
	unsigned int stat_total_op_com_time_write_lock_acquired[NUMBER_OF_DATA_ITEMS];
	unsigned int *stat_total_aborts_per_operation;

} STAT_COLLECTOR_lp_state_type;


typedef union _lp_state_type{
	STAT_COLLECTOR_lp_state_type STAT_COLLECTOR_lp_state;
} lp_state_type;
/***********************************************************************/



