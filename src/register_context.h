#ifndef REGISTER_CONTEXT_H
#define REGISTER_CONTEXT_H

#include "list.h"
#include "context_descriptor.h"

context_descriptor *register_context(const char *context_string, ctx_descriptor_id_t id);
void unregister_context(context_descriptor *cd);

#endif
