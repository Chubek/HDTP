#include "include/hdtp.h"

char * get_server_root() {
    char *venv = getenv("HOME");
    char *follow = "/.hdtp";

    int i = 0;
    char c;

         
    char path_fin[strlen(venv) + strlen(follow) + 2];

    memcpy(path_fin, venv, sizeof(path_fin));
    strcat(path_fin, follow);
    strcat(path_fin, "/");

    char *path_alloc = malloc(sizeof(path_fin));
    strcpy(path_alloc, path_fin);

    make_dir(path_fin);

    return path_alloc;
}


void load_data_items(data_item items[],char file_paths[]) {
    int cnt;
    size_t paths_size = sizeof(*file_paths);
    int i = 0;

    char *sptr = strtok(file_paths, DELIM);

    while (sptr != NULL) {
        data_item curr_item;

        for(int j = 0; j < strlen(sptr); j++) {
            curr_item.path_on_sys[j] = sptr[j];
        }
               
        items[i] = curr_item;

        sptr = strtok(NULL, DELIM);
    }
}

void load_indices(index_item self[], char folder_paths[]) {
    char *sptr = strtok(folder_paths, DELIM);
    char *server_root = get_server_root();
    int i = 0;

    while (sptr != NULL) {
        index_config curr_config;
        char paths[10000];
        char *append = "/.config";
        char curr_path_1[strlen(server_root) + strlen(sptr) + 1];
        unset_array(curr_path_1, strlen(server_root) + strlen(sptr) + 1);

        strncpy(curr_path_1, server_root, sizeof(curr_path_1));
        strcat(curr_path_1, sptr);
        list_files(curr_path_1, paths);
        int cnt = 0;

        char *pptr = paths;
        for (cnt=0; pptr[cnt]; pptr[cnt] == '|' ? cnt++ : *pptr++);
        index_item *curr_index = malloc(sizeof(index_item) + (sizeof(data_item) * cnt));

        data_item ditems[cnt];
        curr_index->num_items = cnt;
        load_data_items(ditems, paths);

        char curr_path_2[strlen(curr_path_1) + strlen(append)];  
        unset_array(curr_path_2, strlen(curr_path_1) + strlen(append));      
        
        strncpy(curr_path_2, curr_path_1, sizeof(curr_path_2));
        strcat(curr_path_2, append);

        char *config_content = read_file(curr_path_2);

        parse_index_conf(&curr_config, config_content);           
       
        for (int j = 0; j < strlen(server_root) + strlen(sptr) + 1; j++) {
            curr_index->path_on_sys[j] = curr_path_1[j];
        }
        
        memcpy(curr_index->config.password, curr_config.password, 1000);        
        memcpy(curr_index->config.index_desc, curr_config.index_desc, 10000);
        memcpy(curr_index->config.index_name, curr_config.index_name, 1000);
        memcpy(curr_index->config.hidden_paths, curr_config.hidden_paths, 10000);
                    
        curr_index->config.default_sort = curr_config.default_sort;

        for (int j = 0; j < (sizeof(ditems) / sizeof(data_item)); j++) {
            curr_index->data_items[j] = ditems[j];
        }

        sptr = strtok(NULL, DELIM);

        self[i] = *curr_index;

        ++i;
    }

    
}


void load_global_conf(global_config *self) {
    char *venv = get_server_root();
    char *append = ".config";

    char conf_path[strlen(venv) + strlen(append) + 1];

    memcpy(conf_path, venv, sizeof(conf_path));
    strcat(conf_path, append);

    char *contents = read_file(conf_path);
    parse_global_conf(self, contents);   

}


hdtp_server * start_server_and_listen() {
    char dirs[10000];
    int cnt = 0;

    list_subdirs(get_server_root(), dirs);

    char *dirs_ptr = dirs;

    for (cnt=0; dirs_ptr[cnt]; dirs_ptr[cnt] == '|' ? cnt++ : *dirs_ptr++);
    hdtp_server *server  = malloc(sizeof(hdtp_server) + (sizeof(index_item) * cnt));

    index_item items[cnt];
    load_indices(items, dirs);

    global_config conf;
    load_global_conf(&conf);

    server->config = conf;

    for (int i = 0; i < cnt; i++) {
       memcpy(
            server->indices[i].config.hidden_paths, 
            items[i].config.hidden_paths,
            10000
        );
       memcpy(
            server->indices[i].config.index_desc, 
            items[i].config.index_desc,
            10000
        );
        memcpy(
            server->indices[i].config.index_name, 
            items[i].config.index_name,
            1000
        );
        memcpy(
            server->indices[i].config.password, 
            items[i].config.password,
            1000
        );
        server->indices[i].config.default_sort = items[i].config.default_sort;
        server->indices[i].num_items = items[i].num_items;

        memcpy(server->indices[i].data_items, items[i].data_items, items[i].num_items);
    }

    return server; 
    
}



int main() {
    static hdtp_server *server;
    server = start_server_and_listen();       

    free(server);

    return 0;
}