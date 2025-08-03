#ifndef _CONFIG_NV_H_
#define _CONFIG_NV_H_

extern const char *default_config_data;
extern const u32   default_config_size;
extern struct
{
  u16  size;
  char data[256];
} config;

void device_config_write_to_nv();
void device_config_remove_from_nv();
void network_config_remove_from_nv();
void device_config_read_from_nv();

#endif
