#include "cgb.h"


char *url_login = "/index.cgi";
char *url_search_list = "/";
char *auth_file = "./auth";
char *url_namedcmd = "/buglist.cgi?cmdtype=runnamed&namedcmd=%s&limit=0";

CGB_t *CGB_new(void) {
  CGB_t *cgb = malloc(sizeof(CGB_t));
  return cgb;
}

int CGB_init(CGB_t *cgb) {
  cgb->verify_peer = 1;
  cgb->verify_host = 1;

  CGBString_init(&cgb->response);
  CGBString_init(&cgb->url);
  CGBString_init(&cgb->auth_user);
  CGBString_init(&cgb->auth_pass);

  BO(CGB_authRead(cgb))
  return EXIT_SUCCESS;
}

int CGB_cleanup(CGB_t *cgb) {
  curl_easy_cleanup(cgb->curl);
  curl_global_cleanup();
  return EXIT_SUCCESS;
}

int CGB_authRead(CGB_t * cgb)
{
  FILE *fp;
  char buf[256];
  char *tok;

  fp = fopen(auth_file,"r");
  if(!fp)
    { perror("fopen"); return EXIT_FAILURE; }

  if(256 != fread(&buf, sizeof(char), 256, fp))
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
    cgb->log_response = fopen("./response.log", "a");
    if(cgb->log_response == NULL) {
      perror("fopen");
      return EXIT_FAILURE;
    }
  }

  fprintf(cgb->log_response, "NEW %s:\n", name);
  if(cgb->response.len != fwrite(cgb->response.mem, cgb->response.len-1, sizeof(char), cgb->log_response))
    return EXIT_FAILURE;

  fprintf(cgb->log_response, "\n\n");
  return EXIT_SUCCESS;
}

/* vim: set sw=2 sts=2 et fdm=marker : */