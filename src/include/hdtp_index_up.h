#include <msquic.h>


void load_data_items(data_item items[], char file_paths[]);
void load_indices(index_item *self, char folder_paths[]);
void load_global_conf(global_config *self);

hdtp_server * start_server_and_listen();