#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "parsers.h"
#include "io.h"
#include "hdtp_index_up.h"
#include "hdtp_index_down.h"
#include "hdpt_index_daemon.h"

#define DELIM   "|"


