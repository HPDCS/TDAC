// copia l'evento nella lista di attesa
void copy_into_wait_list(int event_type, event_content_type *event_content, wait_event**locks_event_wait_list);

// prende il primo evento in attesa del detaitem di_id e lo elimina dalla lista, restituisce 1 se lo trova, 0 altrimenti
int get_first_waiting_event(int di_id,int *event_type, event_content_type **event_content, wait_event**locks_event_wait_list);

// prende il primo evento di read in attesa del detaitem di_id e lo elimina dalla lista, restituisce 1 se lo trova, 0 altrimenti
int get_first_read_waiting_event(int di_id,int table_id, int *event_type, event_content_type **event_content, wait_event**locks_event_wait_list);

// se in lista è presente un evento della transazione tx_id restituisce l'id e  table_id del dataitem che attende, 0 altrimenti
int get_waiting_data_item(int tx_id, int  table_id, int di_id, wait_event**locks_event_wait_list);

// prende il tipo del primo evento in attesa del detaitem di_id, restituisce 0 se non lo trova, 1 se lo READ, 2 se WRITE
int get_type_first_waiting_event(int di_id,int table_id, wait_event**locks_event_wait_list);

// restituisce al lunghezza della lista
int get_list_length(wait_event**locks_event_wait_list);

// prende il tx_id del primo evento in attesa del detaitem di_id, restituisce 0 se non lo trova
int get_tx_id_of_first_waiting_event(int di_id,int table_id, wait_event**locks_event_wait_list);

