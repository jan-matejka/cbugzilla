#define VERBOSE 0

#define BO(...) if(EXIT_FAILURE == __VA_ARGS__) return EXIT_FAILURE;

char *url_login = "/index.cgi";
char *url_search_list = "/";
char *auth_file = "./auth";
char *url_namedcmd = "/buglist.cgi?cmdtype=runnamed&namedcmd=%s&limit=0";

struct CGB_s;
typedef struct CGB_s CGB_t;
struct CGB_s {
  CURL *curl;
  CURLcode res;
  CGBString_t hostname;
  CGBString_t response;
  FILE *devnull, *log_response;
  CGBString_t auth_user;
  CGBString_t auth_pass;
  CGBString_t cookiejar;
  int verify_peer;
  int verify_host;
};

CGB_t *CGB_new(void) {
  CGB_t *cgb = malloc(sizeof(CGB_t));
  return cgb;
}

int CGB_init(CGB_t *cgb) {
  char cookiejar[] = "./cookiejar";
  int jarlen = strlen(cookiejar);
  BO(CGBString_realloc(&cgb->cookiejar, jarlen))
  strncpy(cgb->cookiejar.mem, cookiejar, jarlen);

  BO(CGB_init_curl(cgb))

  cgb->devnull = fopen("/dev/null", "w");
  cgb->verify_peer = 1;
  cgb->verify_host = 1;

  CGBString_init(cgb->response);
  CGBString_init(cgb->hostname);
  CGBString_init(cgb->auth_user);
  CGBString_init(cgb->auth_pass);

  char hostname[] = "https://bugs.gentoo.org";
  int hostlen = strlen(hostname);
  BO(CGBString_realloc(&cgb->hostname, hostlen))
  strncpy(cgb->hostname.mem, hostname, hostlen);

  BO(CGB_authRead(cgb))
  return EXIT_SUCCESS;
}

int CGB_cleanup(CGB_t *cgb) {
  curl_easy_cleanup(cgb->curl);
  curl_global_cleanup();
}

int CGB_authRead(CGB_t * cgb)
{
  FILE *fp;
  char buf[256];
  char *tok;

  fp = fopen(auth_file,"r");
  fread(&buf, sizeof(char), 256, fp);

  tok = strtok(buf, "\n");
  BO(CGBString_realloc(&cgb->auth_user, strlen(tok)))
  strcpy(cgb->auth_user.mem, tok);

  tok = strtok(NULL, "\n");
  BO(CGBString_realloc(&cgb->auth_pass, strlen(tok)))
  strcpy(cgb->auth_pass.mem, tok);

  if(cgb->auth_user.size == 0 || cgb->auth_pass.size == 0) {
    fprintf(stderr, "couldn't parse auth\n");
    exit(EXIT_FAILURE);
  }
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
  fwrite(cgb->response.mem, cgb->response.len-1, sizeof(char), cgb->log_response);
  fprintf(cgb->log_response, "\n\n");
}

/* vim: set sw=2 sts=2 et fdm=marker : */
