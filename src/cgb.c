#include "cgb.h"
#include <unistd.h>

int V_MAJOR=0;
int V_MINOR=1;
int V_MICRO=0;

char *url_login = "/index.cgi";
char *url_search_list = "/";
char *url_namedcmd = "/buglist.cgi?cmdtype=runnamed&namedcmd=%s&limit=0";

CGB_t *CGB_new(void) {
  CGB_t *cgb = malloc(sizeof(CGB_t));
  return cgb;
}

int CGB_init(CGB_t *cgb) {
  cgb->verify_peer = 1;
  cgb->verify_host = 1;

  CGBString_init(&cgb->response);
  CGBString_init(&cgb->response_log_f);
  CGBString_init(&cgb->url);
  CGBString_init(&cgb->auth_user);
  CGBString_init(&cgb->auth_pass);
  cgb->log_response = NULL;

  return EXIT_SUCCESS;
}

int CGB_cleanup(CGB_t *cgb) {
  curl_easy_cleanup(cgb->curl);
  curl_global_cleanup();
  return EXIT_SUCCESS;
}

int CGB_authRead(CGB_t * cgb, char *auth_file)
{
  FILE *fp;
  char buf[256];
  char *tok;

  fp = fopen(auth_file,"r");
  if(!fp)
    { perror("fopen"); return EXIT_FAILURE; }

  if(0 == fread(&buf, sizeof(char), 256, fp))
    return EXIT_FAILURE;

  tok = strtok(buf, "\n");
  BO(CGBString_realloc(&cgb->auth_user, strlen(tok)))
  strcpy(cgb->auth_user.mem, tok);

  tok = strtok(NULL, "\n");
  BO(CGBString_realloc(&cgb->auth_pass, strlen(tok)))
  strcpy(cgb->auth_pass.mem, tok);

  if(cgb->auth_user.size == 0 || cgb->auth_pass.size == 0) {
    fprintf(stderr, "couldn't parse auth\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int CGB_log_response(CGB_t *cgb, char *name) {
  if(cgb->log_response == NULL) {
    cgb->log_response = fopen(cgb->response_log_f.mem, "a");
    if(cgb->log_response == NULL) {
      perror("fopen");
      return EXIT_FAILURE;
    }
  }

  fprintf(cgb->log_response, "NEW %s:\n", name);
  unsigned int written = fwrite(cgb->response.mem, sizeof(char), cgb->response.len-1, cgb->log_response);
  if(cgb->response.len-1 != written)
    return EXIT_FAILURE;

  fprintf(cgb->log_response, "\n\n");
  sync();
  return EXIT_SUCCESS;
}

/* vim: set sw=2 sts=2 et fdm=marker : */
