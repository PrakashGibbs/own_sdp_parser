#include<stdio.h>
#include<malloc.h>
#include <arpa/inet.h> 
#include "sdp_parser.h"
int is_valid_ip_address(char *ip_address)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip_address, &(sa.sin_addr));
    return result != 0;
}

size_t read_file(const char* filename, char **a_out) {
    FILE *fp = NULL;
    size_t file_size = 0;
    fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("Failed to open offer.txt file");
        return 1;
    }
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    if(file_size <= 0) {
        printf("Fiel:%s has invalid size:%d\n", filename, file_size);
        return 1;
    }
    fseek(fp, 0, SEEK_SET);
    *a_out = (char*) malloc(file_size+1);
    memset(*a_out, 0, file_size+1);
    fread(*a_out, file_size, 1, fp);
    fclose(fp); 
    return 0;
}

void destroy_sdp(sdp_t* arg_sdp) {
  if(!arg_sdp) return;
  if(arg_sdp->oline) free(arg_sdp->oline);
  if(arg_sdp->cline) free(arg_sdp->cline);
  if(arg_sdp->mline) free(arg_sdp->mline);
  if(arg_sdp->bw) free(arg_sdp->bw);
  free(arg_sdp);
}

int main() {
    printf("==============================================================================\n");
    printf("This program would parse offer and answer SDP placed in offer/answer text file\n");
    printf("It calculates the RTP/RTCP bandwidth\n");
    printf("==============================================================================\n");
    const char* offer = NULL;
    const char* answer = NULL;
    if(read_file("offer.txt", &offer)) {
        return 1;
    }
    printf("SDP Offer:\n%s\n", offer); 
    if(read_file("answer.txt", &answer)) {
        return 1;
    }
    printf("SDP Answer:\n%s\n", answer); 
    //const char* buf = "v=0\r\nc=IN IP6 2001::1:79bf:d746:a887:c550\r\nm=audio 49120 RTP/AVP 104 107 97 100\r\n";
    sdp_t *o_sdp = NULL, *a_sdp = NULL;
    o_sdp = (sdp_t*)malloc(sizeof(o_sdp));
    memset(o_sdp, 0, sizeof(o_sdp));
    if(o_sdp) {
        if(parse_sdp(offer, o_sdp)) {
            printf("Parsing of Offer SDP failed\n");
            return 1;    
        }
    }
    a_sdp = (sdp_t*)malloc(sizeof(a_sdp));
    memset(a_sdp, 0, sizeof(a_sdp));
    if(a_sdp) {
        if(parse_sdp(answer, a_sdp)) {
            printf("Parsing of Answer SDP failed\n");
            return 1;    
        }
    }

    //Calculate the bandwidth
    calculate_bandwidth(o_sdp,a_sdp);

    destroy_sdp(o_sdp);
    destroy_sdp(a_sdp);
    if(offer) {
        free(offer);
        offer = NULL;
    }
    if(answer) {
        free(answer);
        answer = NULL;
    }
    return 0;
}
