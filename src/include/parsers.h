#define IS_COLON(c)  ((c == 58) ? 1 : 0)
#define IS_SPACE(c)  isspace(c)
#define IS_SEMICOLON(c) ((c == 59) ? 1 : 0)
#define IS_PLUS(c) ((c == 43) ? 1 : 0)
#define IS_DASH(c) ((c == 45) ? 1 :  0)
#define IS_DOLLAR(c) ((c == 36) ? 1 : 0)
#define IS_CAPITAL(c) ((c > 64 && c <= 90) ? 1 : 0)
#define IS_SMALL(c) ((c > 96 && c <= 122) ? 1 : 0)
#define IS_LETTER(c) ((c > 64 && c <= 122) ? 1 : 0)
#define IS_CARR_RET(c) ((c == 13) ? 1 : 0)
#define IS_NEW_LINE(c) (c == 10 ? 1 : 0)
#define IS_PUNCT(c) ((c > 31 && c <= 47) || (c > 57 && c <= 65) ? 1 : 0)
#define IS_DIGIT(c) ((c > 47 && c <= 57) ? 1 : 0)

#define PASSWORD "PASSWORD"
#define REVERSE_PROXY_DOMAIN "GLOBAL_REVERSE_PROXY_DOMAIN"
#define INDEX_NAME  "INDEX_NAME"
#define INDEX_DESCRIPTION   "INDEX_DESCRIPTION"
#define HIDDEN_PATHS    "HIDDEN_PATHS"
#define DEFRAULT_SORT   "DEFAULT_SORT"
#define PORT    "PORT"
#define ADMIN_NAME  "ADMIN_NAME"
#define WELCOME_MESSAGE "WELCOME_MESSAGE"
#define HEADER "HEADER"

#define BY_SIZE_ASC "ASC Size"
#define BY_SIZE_DSC "DSC Size"
#define BY_NAME_ASC "ASC Name"
#define BY_NAME_DSC "DSC Name"
#define BY_EXT_ASC "ASC Extn"
#define BY_EXT_DSC "DSC Extn"

typedef enum ConfigId {
    Init,
    Terminal,
    Colon,
    BeforeEnd,
    End,
    Password,
    ReverseProxy,
    IndexName,
    IndexDesc,
    HiddenPaths,
    DefaultSort,
    Port,
    AdminName,
    WelcomeMessage,
    Header,
} config_id;

typedef enum ActionNext {
    ParseLabels,
    ParseContent,
} action_next;

typedef enum SortType {
    BySizeAsc,
    BySizeDsc,
    ByNameAsc,
    ByNameDsc,
    ByExtAsc,
    ByExtDsc,

} sort_type;


typedef struct IndexConfig {
    char password[1000];
    char index_name[1000];
    char index_desc[10000];
    char hidden_paths[10000];
    sort_type default_sort;
} index_config;

typedef struct GlobalConfig {
    int port;
    char global_reverse_proxy_domain[1000];
    char header[1000];
    char admin_name[1000];
    char welcome_message[10000];
    char hidden_paths[10000];
} global_config;



typedef struct DataItem {
    char path_on_sys[1000];
} data_item;

typedef struct IndexItem {
    index_config config;
    char path_on_sys[500];
    int num_items;
    data_item data_items[];
} index_item;


typedef struct HdtpServer {
    global_config config;
    index_item indices[];
} hdtp_server;


char * assert_config_id(config_id *self, action_next *next_action, char buffer[], char curr_content[]);
void parse_index_conf(index_config *self, char buffer[]);
void parse_global_conf(global_config *self, char buffer[]);
void unset_array(char arr[], int size);