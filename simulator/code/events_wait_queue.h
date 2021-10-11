void copy_into_wait_queue(int event_type, event_content_type *event_content,double now, event_wait_queue *queue);
void get_from_wait_queue(int *event_type, event_content_type **event_content, event_wait_queue *queue);
int is_empty(event_wait_queue *queue);
int get_queue_length(event_wait_queue *queue);

