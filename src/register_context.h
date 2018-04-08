#ifndef REGISTER_CONTEXT_H
#define REGISTER_CONTEXT_H

#include "list.h"
#include "context_descriptor.h"

context_descriptor *register_context(const char *context_string, int id);
void clear_registered_contexts(list *registered_descriptors);

#endif
