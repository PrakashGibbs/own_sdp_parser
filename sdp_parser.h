#include<stdio.h>
#include <stdint.h>
#include<string.h>
#define default_buf_size 255
const char* extract_each_line(const char* buf, size_t buf_len, const char* out_line);
enum ses_mode_e  {
   inactive = -1,
   sendrecv = 0,
   sendonly ,
   recvonly 
};

typedef struct c_line {
    char address_type[default_buf_size];
    char network_type[default_buf_size];
    char ip_address[default_buf_size];
}cline_t;

typedef struct o_line {
    char username[default_buf_size];
    char sessid[default_buf_size];
    char sessver[default_buf_size];
    cline_t con_info;
}oline_t;

typedef struct bandinfo {
   int as;
   int ct;
   int rr;
   int rs;
}bandinfo_t;
typedef struct m_line {
    char mtype[32];
    unsigned short mport;
    char mtrans[default_buf_size];
//    list_t codec_list;
}mline_t;

typedef struct _sdp {
    char ver[default_buf_size];
    char info[default_buf_size];
    char uri[default_buf_size];
    char phone[default_buf_size];
    cline_t *cline;
    oline_t *oline;
    mline_t *mline;
    bandinfo_t *bw;
}sdp_t;
