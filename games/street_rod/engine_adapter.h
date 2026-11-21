#ifndef STREET_ROD_ENGINE_ADAPTER_H
#define STREET_ROD_ENGINE_ADAPTER_H

#include <stddef.h>
#include <stdbool.h>

extern const char *sr_save_dir;
void *sr_load_file(const char *path, size_t *size);
bool sr_writable_file(const char *name);
bool sr_is_driving(void);

#endif
