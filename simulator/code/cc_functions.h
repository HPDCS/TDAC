// struttura rappresentante una versione di un dataitem nel database (per protocolli di concorrenza multiversione)
typedef struct _cc_data_item_version *cc_data_item_version_pointer;
typedef struct _cc_data_item_version {
	double creation_time;	// tempo di creazione della versione
	double creaor_id;
	double commit_time;		// tempo di commit della tx che ha creatp la versione
	double abort_time;		// tempo di abort della tx che ha creatp la versione
	cc_data_item_version_pointer next;
} cc_data_item_version;

//Struttura per la lista delle transazioni con lock condivisi 
typedef struct _cc_shared_reader *cc_shared_reader_pointer;
typedef struct _cc_shared_reader {
	int tx_id_locker;	// id transazione che ha lock condiviso
	cc_shared_reader_pointer next;
} cc_shared_reader;

//dataitems del cc
typedef struct _cc_data_item *cc_data_item_pointer;
typedef struct _cc_data_item {
	int di_id;	
	int tx_id_locker_exclusive;				// id della transazione che ha lock esclusivo sul dataitem, -1: no lock exclusive
	double commit_timestamp;				// tempo di commit dell'ultiomo aggiornamento
} cc_data_item;

//items per il read set
typedef struct _cc_tx_read_set_item *cc_tx_read_set_item_pointer;
typedef struct _cc_tx_read_set_item {
	int di_id;
	int run_op_number;
	cc_tx_read_set_item_pointer next;
} cc_tx_read_set_item;

//items per il write set
typedef struct _cc_tx_write_set_item *cc_tx_write_set_item_pointer;
typedef struct _cc_tx_write_set_item {
	int di_id;
	int run_op_number;
	cc_tx_write_set_item_pointer next;
} cc_tx_write_set_item;

//transazioni
typedef struct _cc_transaction_descriptor *cc_transaction_descriptor_pointer;
typedef struct _cc_transaction_descriptor {
	int tx_id;
	int tx_run_number;
	double start_timestamp;
	cc_tx_write_set_item *cc_first_tx_write_set_item;
	cc_tx_read_set_item *cc_first_tx_read_set_item;	
	cc_transaction_descriptor_pointer next;	// punta alla prossima transazione
	
} cc_transaction_descriptor;


// inserisce una transazione alla fine della lista
void cc_enqueue_transaction(int tx_id,int tx_run_number, double start_timestamp) ;
//restituisce la transazione tx_id 
cc_transaction_descriptor *cc_get_transaction(int tx_id) ;
//restituisce il cc_tx_write_set_item se di_id è presente nel write set della tx
cc_tx_write_set_item *get_write_set_item_by_id(cc_transaction_descriptor* cc_tx_descr, int di_id);
//restituisce il cc_tx_read_set_item se di_id è presente nel read set della tx
cc_tx_read_set_item *get_read_set_item_by_id(cc_transaction_descriptor* cc_tx_descr, int di_id);
// svuota read e write set
void remove_read_and_write_set(cc_transaction_descriptor *cc_tx_descr);
// elimina la transazione tx_id dalla lista con tutti i dataitem aggiornati
void cc_remove_transaction(int tx_id) ;
// inserisce un dataitem al write set della tx
void cc_add_write_set_item(int tx_id, int di, int run_op_number);
// inserisce un dataitem al read set della tx
void cc_add_read_set_item(int tx_id, int di, int run_op_number);
// prende un item del write set della tx per posizione
cc_tx_write_set_item *cc_get_tx_write_set_item_by_position(int tx_id, int position);
// prende un item del read set della tx per posizione
cc_tx_read_set_item *cc_get_tx_read_set_item_by_position(int tx_id, int position);

