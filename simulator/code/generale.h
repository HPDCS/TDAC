#include "application.h"
void SetPointer(int me, void *pointer);
void ProcessEvent(int me, double now, int event_type, event_content_type *event_content, void * pointer);
void InsertEvent(int gid_receiver, int gid_sender, double timestamp, double now, int event_type, event_content_type *event_content);
