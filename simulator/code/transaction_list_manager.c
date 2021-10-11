#include "stdafx.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "generale.h"
#include <math.h>
#include "distribution_generator.h"

int class_tx_lenght[NUMBER_OF_TX_CLASS] = TX_CLASS_LENGHT;

//crea le operazioni della transazione
void create_operations(unsigned * seed, transaction_descriptor * tx_descr, int data_item_access_distribution, double data_items_zipf_const) {
    int j;
    int number_of_operations = 0;
    operation_descriptor * op = NULL;
    double val;
    // seleziona la lunghezza delle transazioni in operazioni
    switch (TRANSACTION_LENGTH) {
    case FIXED:
        number_of_operations = class_tx_lenght[tx_descr -> tx_class_id];
        break;
    case UNIFORM:
        number_of_operations = random_number(seed, class_tx_lenght[tx_descr -> tx_class_id] - 1) + 1;
        break;
    }
    for (j = 1; j <= number_of_operations; j++) {
        if (TLM_VERBOSE) printf("		creo operazione %i di %i\n", j, number_of_operations);
        //crea l'operazione
        op = (operation_descriptor * ) malloc(sizeof(operation_descriptor));
        if (MALLOC_FAIL_VERBOSE && op == NULL) printf("-------- Funzione create_operations: MALLOC FALLITA\n");
        op -> op_number = j;
        // seleziona il tipo di accesso
        double r = Random(seed);
        while (r == 1) {
            r = Random(seed);
        }
        if (r <= WRITE_PR)
            op -> op_type = TM_WRITE;
        else
            op -> op_type = TM_READ;
        // seleziona il data item
        operation_descriptor * l;

        switch (data_item_access_distribution) {

        case UNIFORM:
        case UNIFORM_WITH_ASCENDING_ORDER:
        case UNIFORM_WITH_INVERSE_ORDERS:
        case UNIFORM_WITH_MIXED_ORDERS:
            do {
                op -> di_id = random_number(seed, NUMBER_OF_DATA_ITEMS - 1);
                l = get_operation(tx_descr, op -> di_id);
            }
            while (l);
            break;
        case UNIFORM_ON_TABLES_WITH_ASCENDING_ORDER:
        case UNIFORM_ON_TABLES_WITH_INVERSE_ORDERS:
        case UNIFORM_ON_TABLES_WITH_MIXED_ORDERS:
            do {
                op -> di_id = random_number(seed, round(NUMBER_OF_DATA_ITEMS / number_of_operations) - 1);
                op -> di_id += (j - 1) * round(NUMBER_OF_DATA_ITEMS / number_of_operations);
                l = get_operation(tx_descr, op -> di_id);
            }
            while (l);
            break;
        case ZIPF:
            do {
                op -> di_id = zipf_data_items(data_items_zipf_const);
            }
            while (l);
            break;
        case SINGLE_BAR:
            val = Random(seed);
            if (val < 0.8) {
                op -> di_id = random_number(seed, 10 - 1);
            } else {
                op -> di_id = random_number(seed, NUMBER_OF_DATA_ITEMS - 10 - 1) + 10;
            }
            break;
        }

        if (TLM_VERBOSE) printf("			tipo %i\n", op -> op_type);
        if (TLM_VERBOSE) printf("			data_item %i\n", op -> di_id);
        op -> next = NULL;
        enqueue_operation(tx_descr, op);
    }
    if (data_item_access_distribution == UNIFORM_WITH_ASCENDING_ORDER)
        order_operations_ascending(tx_descr -> first_operation);

    if (data_item_access_distribution == UNIFORM_WITH_INVERSE_ORDERS || data_item_access_distribution == UNIFORM_WITH_MIXED_ORDERS || data_item_access_distribution == UNIFORM_ON_TABLES_WITH_INVERSE_ORDERS || data_item_access_distribution == UNIFORM_ON_TABLES_WITH_MIXED_ORDERS) {
        double r = Random(seed);
        while (r == 1) {
            r = Random(seed);
        }
        if (r < FRACION_OF_C1_TRANSACTIONS)
            order_operations_ascending(tx_descr -> first_operation);
        else if (data_item_access_distribution == UNIFORM_WITH_INVERSE_ORDERS || data_item_access_distribution == UNIFORM_ON_TABLES_WITH_INVERSE_ORDERS)
            order_operations_descending(tx_descr -> first_operation);
        else if (data_item_access_distribution == UNIFORM_ON_TABLES_WITH_MIXED_ORDERS)
            	replace_with_random_di(seed, tx_descr -> first_operation);
    }
}

// carica una transazione e la accoda
transaction_descriptor * create_new_transaction(unsigned * seed, transaction_descriptor ** tx_list, int tx_class_id, int tx_id, int data_item_access_distribution, double data_items_zipf_const) {
    transaction_descriptor * tx_descr = (transaction_descriptor * ) malloc(sizeof(transaction_descriptor));
    if (MALLOC_FAIL_VERBOSE && tx_descr == NULL) printf("-------- Funzione create_new_transaction: MALLOC FALLITA\n");
    tx_descr -> tx_id = tx_id;
    tx_descr -> tx_class_id = tx_class_id;
    tx_descr -> next = NULL;
    tx_descr -> first_operation = NULL;
    tx_descr -> last_operation = NULL;
    tx_descr -> current_operation = NULL;

    if (TLM_VERBOSE) printf("creo nuova transazione con tx_id %i\n", tx_id);

    create_operations(seed, tx_descr, data_item_access_distribution, data_items_zipf_const);

    enqueue_transaction(tx_list, tx_descr);
    return tx_descr;
}

// inserisce un'operazione alla fine della transazione
void enqueue_operation(transaction_descriptor * transaction, operation_descriptor * operation) {
    // ricalcola i puntatori
    if (transaction -> last_operation != NULL) {
        transaction -> last_operation -> next = operation;
    }
    transaction -> last_operation = operation;
    if (transaction -> first_operation == NULL) {
        transaction -> first_operation = operation;
        transaction -> current_operation = operation;
    };
}

// inserisce una transazione alla fine della lista
void enqueue_transaction(transaction_descriptor ** tx_list, transaction_descriptor * tx_descr) {
    // scorre la lista fino alla prima tx con next nullo

    if (( * tx_list) == NULL) {
        * tx_list = tx_descr;
    } else {
        transaction_descriptor_pointer tx_list_temp = * tx_list;
        while (tx_list_temp -> next) {
            tx_list_temp = tx_list_temp -> next;
        }
        tx_list_temp -> next = tx_descr;
    }
}

/* function to swap data of two nodes a and b*/
void swap(operation_descriptor * a, operation_descriptor * b) {
    int temp = a -> di_id;
    a -> di_id = b -> di_id;
    b -> di_id = temp;
}

// ordina le operazioni della transazione in ordine crescente di di_id
void order_operations_ascending(operation_descriptor * first_operation) {
    // usa bubble sort
    int swapped;
    operation_descriptor * ptr1;
    operation_descriptor * lptr = NULL;

    /* Checking for empty list */
    if (first_operation == NULL)
        return;

    do {
        swapped = 0;
        ptr1 = first_operation;

        while (ptr1 -> next != lptr) {
            if (ptr1 -> di_id > ptr1 -> next -> di_id) {
                swap(ptr1, ptr1 -> next);
                swapped = 1;
            }
            ptr1 = ptr1 -> next;
        }
        lptr = ptr1;
    }
    while (swapped);
}

// ordina le operazioni della transazione in ordine decrescente di di_id
void order_operations_descending(operation_descriptor * first_operation) {
    // usa bubble sort
    int swapped;
    operation_descriptor * ptr1;
    operation_descriptor * lptr = NULL;

    /* Checking for empty list */
    if (first_operation == NULL)
        return;

    do {
        swapped = 0;
        ptr1 = first_operation;

        while (ptr1 -> next != lptr) {
            if (ptr1 -> di_id < ptr1 -> next -> di_id) {
                swap(ptr1, ptr1 -> next);
                swapped = 1;
            }
            ptr1 = ptr1 -> next;
        }
        lptr = ptr1;
    }
    while (swapped);
}

// sostituisce i di_id delle operazioni con di_id random
void replace_with_random_di(unsigned * seed, operation_descriptor * first_operation) {
   if (first_operation == NULL)
        return;
        
    operation_descriptor * ptr=first_operation;
    do {
        ptr->di_id=random_number(seed, NUMBER_OF_DATA_ITEMS - 1);
        ptr = ptr -> next;       
        } while (ptr);
}


operation_descriptor * get_operation(transaction_descriptor * tx_desc, int di_id) {
    if (tx_desc -> first_operation == NULL)
        return NULL;
    if (tx_desc -> first_operation -> di_id == di_id)
        return tx_desc -> first_operation;
    operation_descriptor * op_descr = tx_desc -> first_operation;
    while (op_descr -> next) {
        if (op_descr -> next -> di_id == di_id)
            return op_descr -> next;
        op_descr = op_descr -> next;
    }
    return NULL;
}

// restituisce l'operazione corrente della transazione e passa alla successiva
operation_descriptor * get_next_operation(transaction_descriptor * tx_desc) {
    if (tx_desc -> current_operation != NULL) {
        operation_descriptor * op_descr = tx_desc -> current_operation;
        tx_desc -> current_operation = tx_desc -> current_operation -> next;
        return op_descr;
    } else {
        return NULL;
    }
}

// resetta i puntatori current della transazione e rigenera le operazioni
void reset_transaction_state(unsigned * seed, transaction_descriptor * transaction, int data_item_access_distribution, double data_items_zipf_const) {
    // resetta il current della transazione
    if (transaction -> first_operation != NULL) {
        transaction -> current_operation = transaction -> first_operation;
    }
    remove_all_operations(transaction);
    create_operations(seed, transaction, data_item_access_distribution, data_items_zipf_const);
}
//restituisce la transazione tx_id
transaction_descriptor * get_transaction(int tx_id, transaction_descriptor * tx_list) {
    //scorre la lista delle transazioni fino a quando non trova l'id dela tx
    if (tx_list == NULL) {
        return NULL;
    } else {
        transaction_descriptor_pointer tx_temp = tx_list;
        while (tx_temp -> tx_id != tx_id) {
            tx_temp = tx_temp -> next;
        }
        return tx_temp;
    }
}

void remove_all_operations(transaction_descriptor * tx_desc) {
    operation_descriptor * to_delete;
    if (tx_desc -> first_operation == NULL) {
        return;
    } else {
        while (tx_desc -> first_operation != NULL) {
            to_delete = tx_desc -> first_operation;
            tx_desc -> first_operation = tx_desc -> first_operation -> next;
            // libera la memoria dell'operazione
            free(to_delete);
        }
        tx_desc -> last_operation = NULL;
        return;
    }
}

// rimuove una transazione dalla lista
void remove_transaction(int tx_id, transaction_descriptor ** tx_list) {
    // scorre la lista fino alla tx tx_id
    if (( * tx_list) == NULL) {
        return;
    } else {
        if (( * tx_list) -> tx_id == tx_id) {
            transaction_descriptor * to_delete = * tx_list;
            * tx_list = ( * tx_list) -> next;
            // libera la memoria dalle operazioni,
            remove_all_operations(to_delete);
            // libera la memoria della transazione
            free(to_delete);
            return;
        } else {
            transaction_descriptor * tx_temp = * tx_list;
            while (tx_temp -> next) {
                if (tx_temp -> next -> tx_id == tx_id) {
                    transaction_descriptor * to_delete = tx_temp -> next;
                    tx_temp -> next = tx_temp -> next -> next;
                    // libera la memoria dalle operazioni,
                    remove_all_operations(to_delete);
                    // libera la memoria della transazione
                    free(to_delete);
                    return;
                }
                tx_temp = tx_temp -> next;
            }
            return;
        }
    }
}
