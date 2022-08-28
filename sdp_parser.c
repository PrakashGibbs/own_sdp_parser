#include "sdp_parser.h"
#include <malloc.h>
#define CR      0x0d
#define LF      0x0a
const char* extract_each_line(const char* buf, 
        size_t buf_len, const char* out_line) {
    const char *linestart = buf, *lineend = buf + buf_len;
    const char *l_out = out_line;
    if((!buf) || (!buf_len) ) return NULL;
    int x = 0;
    while(linestart < lineend) {
        if(*buf == '\0') {
            x = buf-linestart;
            memcpy(l_out, linestart, x);
            return NULL;
        }
        else if(*buf == CR || *buf == LF) {
            char *eol = buf;
            buf++;
            if(*buf == LF && *eol == CR) {
                buf++;
            }
            x = eol-linestart;
            memcpy(l_out, linestart, x);
            while(*buf == " ") buf++; 
            return buf;
        } else {
            buf++; //if the current char is not CR/LF/NULL increment the buf ptr
        }
    }
}

void calculate_bandwidth(sdp_t *o_sdp, sdp_t *a_sdp) {
    if((!o_sdp) || (!a_sdp) || (!o_sdp->bw) || (!a_sdp->bw) || (!a_sdp->cline) || (!o_sdp->cline) ) {
        printf("Invalid offer/anser \n");
        return;
    }
    printf("permit %dkbps from %s to %s\n", a_sdp->bw->as, a_sdp->cline->ip_address, o_sdp->cline->ip_address);    
    printf("permit %dkbps from %s to %s\n", o_sdp->bw->as, o_sdp->cline->ip_address, a_sdp->cline->ip_address);    
    printf("permit %dkbps from %s to %s\n", o_sdp->bw->rr+o_sdp->bw->rs, o_sdp->cline->ip_address, a_sdp->cline->ip_address);    
    printf("permit %dkbps from %s to %s\n", a_sdp->bw->rr+a_sdp->bw->rs, a_sdp->cline->ip_address, o_sdp->cline->ip_address);    
    printf("Total uplink from %s is:%.2f kbps\n", a_sdp->cline->ip_address, a_sdp->bw->as+((float)(a_sdp->bw->rr+a_sdp->bw->rs)/1000));    
    printf("Total uplink from %s is:%.2f kbps\n", o_sdp->cline->ip_address, ((float)(o_sdp->bw->rr+o_sdp->bw->rs)/1000)+o_sdp->bw->as);    
}

    const char* 
get_each_phrase(const char *in, char* out_word)
{
    if (!in) return NULL;
    const char* tmp = in;

    while (*tmp && *tmp <= ' ') tmp++;

    if (*tmp == '\0') {
        return NULL;
    }

    const char* new_begin = tmp;
    while (*tmp > ' ') tmp++;

    if (tmp > new_begin) {      
        strncpy(out_word, new_begin, (tmp-new_begin));
        while (*tmp && *tmp <= ' ') tmp++;
    }
    return tmp;
}
int parse_bandwidth_info(const char* buffer, bandinfo_t *binfo)
{
    if ((buffer == NULL) || (!binfo)) {
        printf("%s Invalid buffer \n", __FUNCTION__);
        return 1;
    }
    char *tmp = buffer;
    char *bw_type = NULL;
    int8_t count = 0;
    char tmp_buf[128] = {0};
    while(1) {
        count++;
        if ((!tmp) || (*tmp == '\0')) {
            return 1;
        } else if (*tmp == ':') {
            if (count >= 3) {
                bw_type = tmp - 2;
            }
            ++tmp;
        } else {
            ++tmp;
            continue;
        }
        if(bw_type) {
            if(!strncmp(bw_type,"AS:",3)) {
                bw_type += 3;
                if((!bw_type) || (*bw_type == '\0') || (!(bw_type+1)) ||  (*(bw_type+1) == '\0')) {
                    printf("Failed ot get bandwidth for AS\n");
                    return 1;
                }
                strcpy(tmp_buf, bw_type); 
                binfo->as = atoi(tmp_buf); 
//                printf("AS Extracted:%d\n", binfo->as);
                return 0;
            } else if(!strncmp(bw_type,"RS:",3)) {
                bw_type += 3;
                if((!bw_type) || (*bw_type == '\0') || (!(bw_type+1)) ||  (*(bw_type+1) == '\0')) {
                    printf("Failed ot get bandwidth for RS\n");
                    return 1;
                }
                strcpy(tmp_buf, bw_type); 
                binfo->rs = atoi(tmp_buf); 
  //              printf("RS Extracted:%d\n", binfo->rs);
                return 0;
            } else if(!strncmp(bw_type,"RR:",3)) {
                bw_type += 3;
                if((!bw_type) || (*bw_type == '\0') || (!(bw_type+1)) ||  (*(bw_type+1) == '\0')) {
                    printf("Failed ot get bandwidth for RR\n");
                    return 1;
                }
                strcpy(tmp_buf, bw_type);
                binfo->rr = atoi(tmp_buf);
    //            printf("RR Extracted:%d\n", binfo->rr);
                return 0;
            } else if(!strncmp(bw_type,"CT:",3)) {
                return 0;//TODO
            }
            else {
                printf("Unknown bw type:%s\n", bw_type);
                return 1;
            }
        }

    }
    return 0;
}
int parse_origin_info(const char* buffer, oline_t *a_oline)
{
    if ((buffer == NULL) || (!a_oline)) {
        printf("%s Invalid buffer \n", __FUNCTION__);
        return 1;
    }

    const char* tmp = buffer;
    if (NULL == (tmp = get_each_phrase(tmp, &(a_oline->username)))) {
        printf("Invalid nettype");
        return 1;
    }
    if (NULL == (tmp = get_each_phrase(tmp, &(a_oline->sessid)))) {
        printf("Invalid nettype");
        return 1;
    }
    if (NULL == (tmp = get_each_phrase(tmp, &(a_oline->sessver)))) {
        printf("Invalid nettype");
        return 1;
    }
    if (NULL == (tmp = get_each_phrase(tmp, &(a_oline->con_info.network_type)))) {
        printf("Invalid nettype");
        return 1;
    }

    if (NULL == (tmp = get_each_phrase(tmp, &(a_oline->con_info.address_type)))) {
        printf("invalid addrtype");
        return 1;
    }

    if (NULL == (tmp = get_each_phrase(tmp, &(a_oline->con_info.ip_address)))) {
        printf("invalid ip address for connection");
        return 1;
    }

//    printf("username:%s sessid:%s sessver:%s network_type: %s \n address_type:%s ip_address:%s\n",a_oline->username, a_oline->sessid, a_oline->sessver, a_oline->con_info.network_type,a_oline->con_info.address_type, a_oline->con_info.ip_address);
    if (strcmp("IN",a_oline->con_info.network_type) == 0 &&
            (strcmp("IP4",a_oline->con_info.address_type) == 0 ||
             strcmp("IP4",a_oline->con_info.address_type) == 0)) {
        if(is_valid_ip_address(a_oline->con_info.ip_address)) {
        }
    } else {
        return 1;
    }
    return 0;
}
int parse_connection_info(const char* buffer, cline_t *a_cline)
{
    if ((buffer == NULL) || (!a_cline)) {
        printf("%s Invalid buffer \n", __FUNCTION__);
        return 1;
    }

    const char* tmp = buffer;
    if (NULL == (tmp = get_each_phrase(tmp, &(a_cline->network_type)))) {
        printf("Invalid nettype");
        return 1;
    }

    if (NULL == (tmp = get_each_phrase(tmp, &(a_cline->address_type)))) {
        printf("invalid addrtype");
        return 1;
    }

    if (NULL == (tmp = get_each_phrase(tmp, &(a_cline->ip_address)))) {
        printf("invalid ip address for connection");
        return 1;
    }

 //   printf("a_cline->network_type: %s \n a_cline->address_type:%s a_cline->ip_address:%s\n",a_cline->network_type,a_cline->address_type, a_cline->ip_address);
    if (strcmp("IN",a_cline->network_type) == 0 &&
            (strcmp("IP4",a_cline->address_type) == 0 ||
             strcmp("IP4",a_cline->address_type) == 0)) {
        if(is_valid_ip_address(a_cline->ip_address)) {
        }
    } else {
        return 1;
    }
    return 0;
}

int parse_media_info(const char* buffer, mline_t *a_mline)
{
    if ((buffer == NULL) || (!a_mline)) {
        printf("%s Invalid buffer \n", __FUNCTION__);
        return 1;
    }

    const char* tmp = buffer;
    char out_buf[32];
    memset(&out_buf, 0 , sizeof(out_buf));
    if (NULL == (tmp = get_each_phrase(tmp, &(a_mline->mtype)))) {
        printf("Invalid media type");
        return 1;
    }

    if (NULL == (tmp = get_each_phrase(tmp, &out_buf))) {
        printf("invalid media port");
        return 1;
    }
    unsigned short port = (unsigned short)(atoi(out_buf));
    if(port < 0 ) {
        printf("invalid media port:%d", port);
        return 1;
    }  
    a_mline->mport = port;

    if (NULL == (tmp = get_each_phrase(tmp, &(a_mline->mtrans)))) {
        printf("invalid media transport");
        return 1;
    }

 //   printf("media_type: %s \n mport:%d\n mtrans:%s\n",a_mline->mtype,a_mline->mport, a_mline->mtrans);

    return 0;
}


int parse_sdp(const char *sdp_buffer, sdp_t *a_sdp) {
    if((!sdp_buffer) || (!a_sdp) ) {
        printf("SDP parsing failed, sdp_buffer:%p and a_sdp:%p", sdp_buffer, a_sdp);
        return 1;
    }
    const char* buf = sdp_buffer;
    char out_line[255] = { 0};
    const char* next_line = extract_each_line(buf, strlen(buf), &out_line);
    while(next_line != NULL) {
        //        printf("outline:[%s]\n",out_line);
        //printf("outline:%c:%d %c:%d\n",(*(out_line+0)),((*(out_line+0)) == 'o'), (*(out_line+1)), ((*(out_line+1)) == '=' ));
        if(((*(out_line+0)) == 'c') && ((*(out_line+1)) == '=' )) {
            if(!a_sdp->cline) {
                a_sdp->cline = (cline_t*)malloc(sizeof(cline_t));
                memset(a_sdp->cline, 0, sizeof(cline_t));
            }
            parse_connection_info(out_line+2,a_sdp->cline);
        } else if(((*(out_line+0)) == 'o') && ((*(out_line+1)) == '=' )) {
            if(!a_sdp->oline) {
                a_sdp->oline = (oline_t*)malloc(sizeof(oline_t));
            }
            // printf("====oline:%c %c\n",(*(out_line+0)), (*(out_line+1)));
            parse_origin_info(out_line+2, a_sdp->oline);

        } else if(((*(out_line+0)) == 'b') && ((*(out_line+1)) == '=' )) {
            if(!a_sdp->bw) {
                a_sdp->bw = (bandinfo_t*)malloc(sizeof(bandinfo_t));
                memset(a_sdp->bw, 0, sizeof(bandinfo_t));
            }
            parse_bandwidth_info(out_line+2, a_sdp->bw);

        }else if(((*(out_line+0)) == 'm') && ((*(out_line+1)) == '=' )) {
            if(!a_sdp->mline) {
                a_sdp->mline = (mline_t*)malloc(sizeof(mline_t));
                memset(a_sdp->mline, 0, sizeof(mline_t));
            }
            parse_media_info(out_line+2, a_sdp->mline);
        }
        memset(out_line, 0, sizeof(out_line));
        next_line = extract_each_line(next_line, strlen(buf), &out_line);
    }
    if(!a_sdp->mline) {
        printf("mline not present\n"); return 1;
    } if(!a_sdp->cline) {
        printf("cline not present\n"); return 1;
    }
    return 0;
}
