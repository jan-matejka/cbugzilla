#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <tidy.h>
#include <buffio.h>

#define SKIP_PEER_VERIFICATION 1
#define SKIP_HOSTNAME_VERIFICATION 1

char *url_login = "https://bugs.gentoo.org/index.cgi";
char *url_search_list = "https://bugs.gentoo.org/";
char *auth_file = "./auth";
FILE *devnull;

struct MemoryStruct {
    char *memory;
    size_t size;
};

int gb_login(CURL *curl, char *user, char *pass)
{
  CURLcode res;
  struct curl_httppost *formpost=NULL;
  struct curl_httppost *lastptr=NULL;
  struct curl_slist *headerlist=NULL;

  curl_easy_setopt(curl, CURLOPT_URL, url_login);

  /* Fill in the filename field */
  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "Bugzilla_login",
               CURLFORM_COPYCONTENTS, user,
               CURLFORM_END);

  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "Bugzilla_password",
               CURLFORM_COPYCONTENTS, pass,
               CURLFORM_END);

  //curl_easy_setopt(curl, CURLOPT_HEADER, 1);
  curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

  curl_easy_setopt(curl, CURLOPT_WRITEDATA, devnull);
  res = curl_easy_perform(curl);
  return 1;
   if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
  curl_formfree(formpost);
  return 0;
}

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

size_t
WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;

    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
     memcpy(&(mem->memory[mem->size]), ptr, realsize);
     mem->size += realsize;
     mem->memory[mem->size] = 0;
    }
    return realsize;
}

int read_auth(char **user, char **pass)
{
  FILE *fp;

  fp = fopen(auth_file,"r");

  *user = *pass = NULL;

  char buf[256];
  char *tok;
  fread(&buf, sizeof(char), 256, fp);
  tok = strtok(buf, "\n");
  *user = realloc(*user, strlen(tok) * sizeof(char));
  strcpy(*user, tok);
  tok = strtok(NULL, "\n");
  *pass = realloc(*pass, strlen(tok) * sizeof(char));
  strcpy(*pass, tok);

  if(user == NULL || pass == NULL) {
    printf("couldn't parse auth\n");
    exit(EXIT_FAILURE);
  }
}

/* Traverse the document tree */
void dumpNode(TidyDoc doc, TidyNode body )
{
  TidyNode ch1, ch2;
  ctmbstr s;
  TidyAttr tattr;

  ch1 = tidyGetChild(body);

  /* path where the saved query links are:
   *  html body div#footer ul#useful-links li#links-saved ul.links
   *
   * in there here's a single link:
   *  li a
   */

  while(1) {
    ch2 = tidyGetNext(ch1);
    if(TidyTag_DIV == tidyNodeGetId(ch2)) {
      tattr = tidyAttrGetById(ch2, TidyAttr_ID);
      printf("id=%s\n", tidyAttrValue(tattr));
    }

    ch1 = ch2;
  }
}

int main(void)
{
  CURL *curl;
  CURLcode res;
  devnull = fopen("/dev/null", "w");
  struct MemoryStruct chunk;
  chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
  chunk.size = 0;    /* no data at this point */

  char *user;
  char *pass;

  char *cookiejar = strcat(getenv("HOME"), "/gb_cookiejar");

  read_auth(&user, &pass);

  curl_global_init(CURL_GLOBAL_SSL);

  curl = curl_easy_init();
  if(!curl)
    return EXIT_FAILURE;

  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, SKIP_PEER_VERIFICATION);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, SKIP_HOSTNAME_VERIFICATION);

  curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
 // curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);

  gb_login(curl, user, pass);
  curl_easy_setopt(curl, CURLOPT_URL, url_search_list);


  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

  res = curl_easy_perform(curl);

  if(res != CURLE_OK)
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));

  TidyDoc tdoc;

  //tidyOptSetBool(tdoc, TidyForceOutput, yes); /* try harder */
  //tidyOptSetInt(tdoc, TidyWrapLen, 4096);
  //tidySetErrorBuffer( tdoc, &tidy_errbuf );
  //tidyBufInit(&docbuf);

  tdoc = tidyCreate();
  int err;
  TidyBuffer *buf;
  buf = malloc(8); // Why the hell does tidyBufInit assert buf != 0 ?
  tidyBufInit(buf);
  tidyBufAppend(buf, (void *) chunk.memory, (uint) chunk.size);
  err = tidyParseBuffer(tdoc, buf);
  if(err < 0)
      return 1;

  dumpNode( tdoc, tidyGetBody(tdoc) );
  //fprintf(stderr, "%s\n", tidy_errbuf.bp); /* show errors */

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  return EXIT_SUCCESS;
}

/* vim: set sw=2 sts=2 et fdm=marker : */
