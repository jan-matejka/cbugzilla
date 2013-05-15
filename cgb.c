#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

char *url_login = "https://bugs.gentoo.org/index.cgi";
char *auth_file = "./auth";

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

  char *a = "abcd";
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

  curl_easy_setopt(curl, CURLOPT_HEADER, 1);
  curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

  printf("logging in\n");
  res = curl_easy_perform(curl);
   if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
  curl_easy_cleanup(curl);
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

int main(void)
{
  CURL *curl;
  CURLcode res;

  struct MemoryStruct chunk;
  chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
  chunk.size = 0;    /* no data at this point */

  char *user;
  char *pass;

  char *cookiejar = strcat(getenv("HOME"), "/gb_cookiejar");

  read_auth(&user, &pass);

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  if(!curl)
    return EXIT_FAILURE;
  //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
  curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiejar);

    gb_login(curl, user, pass);
    print_cookies(curl);
    exit(10);
    curl_easy_setopt(curl, CURLOPT_URL, "https://bugs.gentoo.org/");

#ifdef SKIP_PEER_VERIFICATION
    /*
     * If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you.
     */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
    /*
     * If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure.
     */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    /* Perform the request, res will get the return code */
    printf("getting the login page in\n");
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    printf("%s", chunk.memory);

    /* always cleanup */
    curl_easy_cleanup(curl);

  curl_global_cleanup();

  return 0;
}

/* vim: set sw=2 sts=2 et fdm=marker : */
