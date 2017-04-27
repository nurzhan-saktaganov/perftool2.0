#ifndef REGISTER_CONTEXT_H
#define REGISTER_CONTEXT_H

#include "context_descriptor.h"

context_descriptor *register_context(const char *context_string);
void unregister_contexts();

#endif
