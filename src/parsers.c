#include "include/hdtp.h"

void unset_array(char arr[], int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = 0;
    }
}


char * assert_config_id(
    config_id *self, 
    action_next *next_action, 
    char buffer[], 
    char curr_content[]
) {
    unset_array(curr_content, 10000);
    char c = *buffer++;  

    if (c == 0x00) {
        *self = End;
        return buffer;
    }

    if (*self == Init) {
        while (IS_SPACE(c) || IS_NEW_LINE(c)) {
            c = *buffer++;          
        }
    }

    if (*next_action == ParseContent) {        
        goto parse_content;
    } else if (*next_action == ParseLabels) {
        goto parse_labels;
    }
        
    parse_labels:
    char identifier[100];
    unset_array(identifier, 100);

    if (IS_DOLLAR(c)) {
        c = *buffer++;
        if (IS_CAPITAL(c)) {
            identifier[0] = c;

            int i = 1;

            while (1) {
                c = *buffer++;               

                if (IS_COLON(c)) {
                    *next_action = ParseContent;
                    break;
                }
                identifier[i] = c;
                ++i;   
            }
            
            int slen = strlen(identifier);
            char identifier_fin[slen + 1];
            unset_array(identifier_fin, slen + 1);

            int m = 0;
            for (int k = 0; k < slen; k++) {
                identifier_fin[k] = identifier[k];

                ++m;
            }

            identifier_fin[m] = '\0';

            if (strcmp(identifier_fin, PASSWORD) == 0) {
                *self = Password;
                return buffer;
            }else if (strcmp(identifier_fin, REVERSE_PROXY_DOMAIN) == 0) {
                *self = ReverseProxy;
                return buffer;
            } else if (strcmp(identifier_fin, INDEX_NAME) == 0) {
                *self = IndexName;
                return buffer;
            } else if (strcmp(identifier_fin, INDEX_DESCRIPTION) == 0) {
                *self = IndexDesc;
                return buffer;
            } else if (strcmp(identifier_fin, HIDDEN_PATHS) == 0) {
                *self = HiddenPaths;
                return buffer;
            } else if (strcmp(identifier_fin, DEFRAULT_SORT) == 0) {
                *self = DefaultSort;
                return buffer;
            } else if (strcmp(identifier_fin, PORT) == 0) {
                *self = Port;
                return buffer;
            } else if (strcmp(identifier_fin, ADMIN_NAME) == 0) {
                *self = AdminName;
                return buffer;
            } else if (strcmp(identifier_fin, WELCOME_MESSAGE) == 0) {
                *self = WelcomeMessage;
                return buffer;
            } else if (strcmp(identifier_fin, HEADER) == 0 ) {
                *self = Header;
                return buffer;
            } else {
                printf("\nError Occured: Wrong config option %s", identifier_fin);
                exit(1);
            }

        }
    }

    parse_content:
    int i = 0;
    int reached_non_space = 0;
    
    while(1) {
        c = *buffer++;      

        if (IS_SPACE(c) && !reached_non_space) {
            continue;
        } else {
            reached_non_space = 1;
        }

        if (IS_SEMICOLON(c)) {
            break;
        }
        curr_content[i] = c;

        ++i;
    }
    char cc;
    while (1) {
        cc = buffer[0];

        if (!(IS_SPACE(cc) || IS_NEW_LINE(cc))) {
            break;
        } 

        *buffer++;      
    }

    *self = Terminal;
    *next_action = ParseLabels;

    return buffer;

}


void parse_index_conf(index_config *self, char buffer[]) {
    int i = 0;
    char c;
    char content_buffer[10000];
    config_id conf_id = Init;
    action_next next_action = ParseLabels;   
    
    while (conf_id != End) {
        buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);

        switch (conf_id) {
            case Password:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);
            
                for (int i = 0; i < strlen(content_buffer); i++) {
                    self->password[i] = content_buffer[i];

                }
               
                break;            
            case IndexName:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);
            
                for (int i = 0; i < strlen(content_buffer); i++) {
                    self->index_name[i] = content_buffer[i];

                }

                break;
            case IndexDesc:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);
            
                for (int i = 0; i < strlen(content_buffer); i++) {
                    self->index_desc[i] = content_buffer[i];

                }

                break;
            case HiddenPaths:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);
            
                for (int i = 0; i < strlen(content_buffer); i++) {
                    self->hidden_paths[i] = content_buffer[i];

                }

                break;
            case DefaultSort:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);

                char sort[9];

                for (int j = 0; j < strlen(content_buffer); j++) {
                    sort[j] = content_buffer[j];
                }

                int cmp_size_asc = strcmp(sort, BY_SIZE_ASC);
                int cmp_size_dsc = strcmp(sort, BY_SIZE_DSC);
                int cmp_name_asc = strcmp(sort, BY_NAME_ASC);
                int cmp_name_dsc = strcmp(sort, BY_NAME_DSC);
                int cmp_ext_asc = strcmp(sort, BY_EXT_ASC);
                int cmp_ext_dsc = strcmp(sort, BY_EXT_DSC);

                if (!cmp_size_asc) {
                    self->default_sort = BySizeAsc;
                } else if (!cmp_size_dsc) {
                    self->default_sort = BySizeDsc;
                } else if (!cmp_name_asc) {
                    self->default_sort = ByNameAsc;
                } else if (!cmp_name_dsc) {
                    self->default_sort = ByNameDsc;
                } else if (!cmp_ext_asc) {
                    self->default_sort = ByExtAsc;
                } else if (!cmp_ext_dsc) {
                    self->default_sort = ByExtDsc;
                } else {
                    printf("Error parsing config file: Incorrect `DEFAULT_SORT`, must have 8 characters only. ASC, DSC first, Size, Name, Extn second.");
                    exit(1);
                }

                break;
            default:
                break;
        }
    }

}

void parse_global_conf(global_config *self, char buffer[]) {
    int i = 1;
    char c;
    char content_buffer[10000];
    config_id conf_id = Init;
    action_next next_action = ParseLabels;

    while (conf_id != End) {
        buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);

        switch (conf_id) {
            case Port:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);

                char port_buffer[6];

                for (int i = 0; i < strlen(content_buffer); i++) {
                    port_buffer[i] = content_buffer[i];

                }

                self->port = atoi(port_buffer);

                break;
            case ReverseProxy:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);
            
                for (int i = 0; i < strlen(content_buffer); i++) {
                    self->global_reverse_proxy_domain[i] = content_buffer[i];

                }

                i = 1;

                break;
            case Header:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);
            
                for (int i = 0; i < strlen(content_buffer); i++) {
                    self->header[i] = content_buffer[i];

                }

                break;
            case AdminName:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);
            
                for (int i = 0; i < strlen(content_buffer); i++) {
                    self->admin_name[i] = content_buffer[i];

                }

                break;
            case WelcomeMessage:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);
            
                for (int i = 0; i < strlen(content_buffer); i++) {
                    self->welcome_message[i] = content_buffer[i];

                }

                break;
            case HiddenPaths:
                conf_id = Colon;
                buffer = assert_config_id(&conf_id, &next_action, buffer, content_buffer);
            
                for (int i = 0; i < strlen(content_buffer); i++) {
                    self->hidden_paths[i] = content_buffer[i];

                }

                break;
        }  
    }

}