/***********************************************************************/
//tipi di eventi ricevuti dall'oggetto STAT_COLLECTOR

#define STAT_ADD_tx									1
#define STAT_ADD_tx_run								2
#define STAT_ADD_st									3
#define STAT_ADD_op									4
#define STAT_SET_run_commit							8
#define STAT_SET_run_abort							9
#define STAT_SET_op_start_wait_time					11
#define STAT_SET_op_end_wait_time					12
#define STAT_SET_op_exec_time						13
#define STAT_SET_op_buf_hit							14
#define PRINT_STAT									17
#define STAT_SET_op_lock_requested					19
#define STAT_SET_op_com_time_write_lock_contention	20
#define STAT_SET_op_enc_time_read_lock_contention	21
#define STAT_SET_op_com_time_read_lock_contention	22
#define STAT_SET_op_enc_time_write_lock_contention	23
#define STAT_SET_op_enc_time_validation_fail		24
#define STAT_SET_op_com_time_validation_fail		25
#define STAT_SET_op_lock_reseased					26


/***********************************************************************/

// strutture

//operazione
typedef struct _stat_operation_descriptor *stat_operation_descriptor_pointer;
typedef struct _stat_operation_descriptor {
	int op_number;	
	int op_type;
	int op_di_id;
	double op_write_lock_request_time;
	double op_write_lock_release_time;
	int op_enc_time_write_lock_contention;
	int op_com_time_write_lock_contention;
	int op_enc_time_read_lock_contention;
	int op_com_time_read_lock_contention;
	int op_enc_time_validation_fail;
	int op_com_time_validation_fail;
	stat_operation_descriptor_pointer next;
} stat_operation_descriptor;


//run di una transazione
typedef struct _stat_tx_run_descriptor *stat_tx_run_descriptor_pointer;
typedef struct _stat_tx_run_descriptor {
	int run_number;				
	double begin_req_time;
	double begin_start_wait_time;
	double begin_end_wait_time;
	double commit_time;
	double abort_time;
	int abort_for_invalidation;
	int abort_for_commit;
	stat_operation_descriptor *first_operation;
	stat_tx_run_descriptor_pointer next;		// punta al prossimo run
} stat_tx_run_descriptor;

// transazione
typedef struct _stat_transaction_descriptor *stat_transaction_descriptor_pointer;
typedef struct _stat_transaction_descriptor {
	int tx_id;				
	int tx_class_id;
	double begin_req_time;
	double commit_time;
	stat_tx_run_descriptor *first_run;	
	stat_transaction_descriptor_pointer next;	// punta alla prossima transazione
} stat_transaction_descriptor;

