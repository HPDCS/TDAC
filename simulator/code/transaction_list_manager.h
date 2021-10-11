
#include <stdio.h>
//operazioni
typedef struct _operation_descriptor *operation_descriptor_pointer;
typedef struct _operation_descriptor {
	int op_number;		// indica il numero progressivo dell'operazione nello statement
	int op_type;		// indica il tipo di operazione
	int di_id;  		// indica il dataitem all'interno della tabella
	operation_descriptor_pointer next;
} operation_descriptor;

//transazioni
typedef struct _transaction_descriptor *transaction_descriptor_pointer;
typedef struct _transaction_descriptor {
	int tx_id;				// viene assegnato a run time dal TM
	int tx_class_id;		// indica la casse della tx
	operation_descriptor *first_operation;		// punta alla prima operazione della transazione
	operation_descriptor *last_operation;		// punta all'ultima operazione della transazione
	operation_descriptor *current_operation;	// punta all'operazione corrente in esecuzione di questo statements
	transaction_descriptor_pointer next;	// punta alla prossima transazione
} transaction_descriptor;


// crea una transazione e la accoda
transaction_descriptor*create_new_transaction(unsigned *seed, transaction_descriptor **tx_list, int tx_class_id, int tx_id, int data_item_access_distribution, double data_items_zipf_const);

// inserisce un'operazione alla fine della transazione
void enqueue_operation(transaction_descriptor *transaction, operation_descriptor *operation);

// inserisce una transazione alla fine della lista
void enqueue_transaction(transaction_descriptor **tx_list, transaction_descriptor *tx_descr);

// restituisce l'operazione corrente della transazione e passa alla successiva
operation_descriptor *get_next_operation(transaction_descriptor *transaction);

// resetta i puntatori current della transazione e rigenera le operazioni
void reset_transaction_state(unsigned * seed, transaction_descriptor * transaction, int data_item_access_distribution, double data_items_zipf_const);
//restituisce la transazione tx_id
transaction_descriptor* get_transaction(int tx_id,transaction_descriptor *tx_list);

void remove_all_operations(transaction_descriptor *tx_desc);

// rimuove una transazione dalla lista
void remove_transaction(int tx_id,transaction_descriptor **tx_list);

// restituisce l'operazione che accede al dataitem di_id
operation_descriptor *get_operation(transaction_descriptor *tx_desc, int di_id);

// ordina le operazioni della transazione in ordine crescente di di_id
void order_operations_ascending(operation_descriptor * first_operation);

// ordina le operazioni della transazione in ordine decrescente di di_id
void order_operations_descending(operation_descriptor * first_operation);

// sostituisce i di_id delle operazioni con di_id random
void replace_with_random_di(unsigned * seed, operation_descriptor * first_operation);
