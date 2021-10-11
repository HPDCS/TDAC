/***********************************************************************/
/***********************************************************************/
//parametri di configurazione

#include "../input_parameters.h"
#include "tl2_configuration.h"

#define TL2_LOCKING_MODE					TL2_EAGER

#define SIMULATION_TIME					    200000000

#define MIN_NUMBER_OF_THREAD				1
#define MAX_NUMBER_OF_THREAD				32
#define NUMBER_OF_THREAD_STEP				1
#define NTCB_TIME						    0	//tempo di servizio dei blocchi di codice esterni alle transazioni
#define TCB_TIME						    1	//tempo di servizio dei blocchi di codice interni alle transazioni
#define BACKOFF_TIME						0.000000001	//tempo di backoff

#define ACCESS_PATTERN_TYPE					PROBABILISTIC

// profilo delle transazioni
#define NUMBER_OF_TX_CLASS				 	1
#define TRANSACTION_LENGTH					FIXED
#define WRITE_PR						    1


#define DATA_ITEMS_ZIPF_CONST					0.6

// tempi delle operazioni
#define TIME_FOR_TM_BEGIN_EXECUTION				0
#define TIME_FOR_TM_READ_EXECUTION		 		0
#define TIME_FOR_TM_WRITE_EXECUTION		 		0
#define TIME_FOR_TM_ABORT_EXECUTION		 		0
#define TIME_FOR_COMMIT_EXECUTION			 	0
#define TIME_FOR_GVC_INCREMENT_EXECUTION		0.0000001

// cc configuration
#define ENC_TIME_READ_LOCK					    1
#define ENC_TIME_READ_VALIDATION				1
#define ENC_TIME_WRITE_LOCK					    1
#define COM_TIME_WRITE_LOCK					    1
#define COM_TIME_READ_VALIDATION				1
#define COM_TIME_READ_LOCK					    1


//----------------------------------------------------------------------/

// stampa delle statistiche
#define STAT_START_TIME					        0
#define PRINT_EXECUTION_INFO					0
#define PRINT_THREAD_STATES					    0
#define PRINT_FINAL_STATS_WITH_LABELS			1
#define PRINT_FINAL_STATS_WITHOUT_LABELS		0
#define PRINT_STATS_ON_FILE					    0

// file di output per le statistiche
#define OUTPUT_FILE_NAME "OUTPUT.TXT"

//----------------------------------------------------------------------/

/* VERBOSE APPLICAZIONE */
#define APP_VERBOSE				        0
#define CLIENT_VERBOSE				    0
#define TLM_VERBOSE				        0
#define TM_VERBOSE				        0
#define CC_VERBOSE				        0
#define GVC_VERBOSE				        0
#define GVC_QUEUE_VERBOSE			    0
#define STAT_COLLECTOR_VERBOSE  		0


#define PRINT_MAX_BLOCKED_TRANSACTIONS	0

#define MALLOC_FAIL_VERBOSE 			0

/* VERBOSE STATISTICHE  */
#define APP_PRINT_STAT			0
#define CLIENT_PRINT_STAT		0
#define TM_PRINT_STAT			0
#define CC_PRINT_STAT			0
#define TLM_PRINT_STAT			0


//fine parametri di configurazione
/***********************************************************************/
/***********************************************************************/



