/*** CONFIGURATION PARAMETERS ***/

/** transactions **/

#define NUMBER_OF_DATA_ITEMS		128	/* total number of data items (d) */
#define TX_CLASS_LENGHT			{8}	/* number of transactional operations per transaction */
#define NUMBER_OF_TRANSACTIONS		10000	/* total number of transaction to execute */


/** transaction data access patterns **/

#define DATA_ITEMS_ACCESS_DISTRIBUTION	UNIFORM_ON_TABLES_WITH_MIXED_ORDERS
/* options:
UNIFORM 					-> Case 1.1 and Case 2.1
UNIFORM_WITH_ASCENDING_ORDER			-> Case 1.2	
UNIFORM_WITH_MIXED_ORDERS			-> Case 1.3
UNIFORM_WITH_INVERSE_ORDERS			-> Case 1.4
UNIFORM_ON_TABLES_WITH_ASCENDING_ORDER 	        -> Case 2.2 
UNIFORM_ON_TABLES_WITH_MIXED_ORDERS		-> Case 2.3
UNIFORM_ON_TABLES_WITH_INVERSE_ORDERS		-> Case 2.4
*/

/* the following parameter is used only for Case 1.3, Case 1.4, Case 2.3 and Case 2.4 */
#define FRACION_OF_C1_TRANSACTIONS	0.5 /* must be a value between 0 and 1 */
