#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <tidy.h>
#include <buffio.h>

#define SKIP_PEER_VERIFICATION 1
#define SKIP_HOSTNAME_VERIFICATION 1

#define BO(...) if(EXIT_FAILURE == __VA_ARGS__) return EXIT_FAILURE;

char *url_login = "https://bugs.gentoo.org/index.cgi";
char *url_search_list = "https://bugs.gentoo.org/";
char *auth_file = "./auth";
char *url_namedcmd = "https://bugs.gentoo.org/buglist.cgi?cmdtype=runnamed&namedcmd=%s";

// {{{ CGBString
struct CBGString_s;
typedef struct CGBString_s CGBString_t;
struct CGBString_s {
    char *mem;
    size_t size;
    int len;
};

int CGBString_init(CGBString_t cgbs) {
  cgbs.mem = NULL;
  cgbs.size = 0;
  cgbs.len  = 0;
}


int CGBString_realloc(CGBString_t *cgbs, int len) {
  cgbs->len  = len+1;
  cgbs->size = sizeof(char) * (cgbs->len);
  cgbs->mem = realloc(cgbs->mem, cgbs->size);
  if(cgbs->mem == NULL) {
    perror("realloc");
    return EXIT_FAILURE;
  }
  cgbs->mem[len] = 0;
}

void CGBString_free(CGBString_t *s) {
  s->len = 0;
  s->size = 0;
  if(s->mem != NULL)
    free(s->mem);
  s->mem = NULL;
}
// }}}


// {{{ CGB
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
};

CGB_t *CGB_new(void) {
  CGB_t *cgb = malloc(sizeof(CGB_t));
  return cgb;
}

int CGB_init(CGB_t *cgb) {
  BO(CGB_init_curl(cgb))

  cgb->devnull = fopen("/dev/null", "w");

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

size_t
CGB_curl_WMemCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  size_t oldsize;

  CGBString_t *cgbs = (CGBString_t *)data;
  CGBString_init(*cgbs);
  if(size != sizeof(char)) {
    fprintf(stderr, "unexpected size");
    return EXIT_FAILURE;
  }
  oldsize = cgbs->size;
  CGBString_realloc(cgbs, cgbs->len + nmemb);

  memcpy(&(cgbs->mem[oldsize]), ptr, realsize);
  return realsize;
}

int CGB_init_curl(CGB_t *cgb) {
  //char *cookiejar = strcat(getenv("HOME"), "/gb_cookiejar");
  curl_global_init(CURL_GLOBAL_SSL);

  cgb->curl = curl_easy_init();
  if(!cgb->curl)
    return EXIT_FAILURE;

  curl_easy_setopt(cgb->curl, CURLOPT_SSL_VERIFYPEER, SKIP_PEER_VERIFICATION);
  curl_easy_setopt(cgb->curl, CURLOPT_SSL_VERIFYHOST, SKIP_HOSTNAME_VERIFICATION);

  curl_easy_setopt(cgb->curl, CURLOPT_VERBOSE, 0);
  // curl_easy_setopt(cgb->curl, CURLOPT_COOKIEJAR, cookiejar);
  //curl_easy_setopt(cgb->curl, CURLOPT_HEADER, 1);

  curl_easy_setopt(cgb->curl, CURLOPT_WRITEFUNCTION, CGB_curl_WMemCallback);
  curl_easy_setopt(cgb->curl, CURLOPT_WRITEDATA, (void *)&cgb->response);
  return EXIT_SUCCESS;
}

int CGB_curl_perform(CGB_t *cgb) {
  CGBString_free(&cgb->response);
  cgb->res = curl_easy_perform(cgb->curl);
  if(cgb->res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(cgb->res));
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int CGB_cleanup(CGB_t *cgb) {
  curl_easy_cleanup(cgb->curl);
  curl_global_cleanup();
}

int CGB_bz_login(CGB_t * cgb)
{
  struct curl_httppost *formpost=NULL;
  struct curl_httppost *lastptr=NULL;
  struct curl_slist *headerlist=NULL;

  curl_easy_setopt(cgb->curl, CURLOPT_URL, url_login);

  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "Bugzilla_login",
               CURLFORM_COPYCONTENTS, cgb->auth_user.mem,
               CURLFORM_END);

  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "Bugzilla_password",
               CURLFORM_COPYCONTENTS, cgb->auth_pass.mem,
               CURLFORM_END);

  curl_easy_setopt(cgb->curl, CURLOPT_HTTPPOST, formpost);
  BO(CGB_curl_perform(cgb))

  curl_formfree(formpost);

  return EXIT_SUCCESS;
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

// }}}

static int
print_cookies(CURL *curl)
{
  CURLcode res;
  struct curl_slist *cookies;
  struct curl_slist *nc;
  int i;

  printf("Cookies, curl knows:\n");
  res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
  if (res != CURLE_OK) {
    fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n", curl_easy_strerror(res));
    return 1;
  }
  nc = cookies, i = 1;
  while (nc) {
    printf("[%d]: %s\n", i, nc->data);
    nc = nc->next;
    i++;
  }
  if (i == 1) {
    printf("(none)\n");
  }
  curl_slist_free_all(cookies);
  return 0;
}

/* Traverse the document tree */
void CGB_SavedQueries_parse(TidyDoc doc, TidyNode body )
{
  TidyNode ch1, ch2;
  ctmbstr s;
  TidyAttr tattr;
  int step=0;

  ch1 = tidyGetChild(body);

  /* path where the saved query links are:
   *  html body div#footer ul#useful-links li#links-saved ul.links
   *
   * in there here's a single link:
   *  li a
   */

  while(1) {
    switch(step) {
      case 0:
        // Find div#footer
        ch2 = tidyGetNext(ch1);
        if(TidyTag_DIV == tidyNodeGetId(ch2)) {
          tattr = tidyAttrGetById(ch2, TidyAttr_ID);
          if(0 == strcmp("footer", tidyAttrValue(tattr))) {
            ch1 = tidyGetChild(ch2);
            step++;
          }else
            ch1 = ch2;
        }else
          ch1 = ch2;
      break;
      case 1:
        // find ul#useful-links

      break;

      default:
        fprintf(stderr, "DOM traversal failed\n");
    }
  }
}

int CGB_SavedQueries_get(CGB_t *cgb) {
  curl_easy_setopt(cgb->curl, CURLOPT_URL, url_search_list);

  BO(CGB_curl_perform(cgb))

  TidyDoc tdoc;

  tdoc = tidyCreate();
  int err;
  TidyBuffer *buf;
  buf = malloc(8); // Why the hell does tidyBufInit assert buf != 0 ?
  tidyBufInit(buf);
  tidyBufAppend(buf, (void *) cgb->response.mem, (uint) cgb->response.size);
  err = tidyParseBuffer(tdoc, buf);
  if(err < 0)
      return EXIT_FAILURE;

  CGB_SavedQueries_parse( tdoc, tidyGetBody(tdoc) );
}

int CGB_bz_RecordsCount_get(CGB_t *cgb, char *namedcmd, int *count) {
  char *url=NULL;
  int len;
  len = strlen(url_namedcmd) -2 + strlen(namedcmd) +1;
  url = realloc(url, sizeof(char) * len);
  snprintf(url, len, url_namedcmd, namedcmd);

  curl_easy_setopt(cgb->curl, CURLOPT_URL, url);

  if(EXIT_FAILURE == CGB_curl_perform(cgb))
    return EXIT_FAILURE;

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
  fwrite(cgb->response.mem, cgb->response.len-1, sizeof(char), cgb->log_response);
  fprintf(cgb->log_response, "\n\n");
}

int main(void)
{
  CGB_t *cgb;

  cgb = CGB_new();
  BO(CGB_init(cgb))

  BO(CGB_bz_login(cgb))
  CGB_log_response(cgb, "bz_login");

  int records;
  BO(CGB_bz_RecordsCount_get(cgb, "python-herd", &records))

  CGB_log_response(cgb, "rec: python-herd");

  CGB_cleanup(cgb);
  return EXIT_SUCCESS;
}

/* vim: set sw=2 sts=2 et fdm=marker : */
