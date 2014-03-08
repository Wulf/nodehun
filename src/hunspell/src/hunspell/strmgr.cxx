#include "license.hunspell"
#include "license.myspell"


#include "strmgr.hxx"
int StrMgr::fail(const char * err, const char * par) {
  fprintf(stderr, err,par);
  return -1;
}

StrMgr::StrMgr(const char * str, const char * key) {
  linenum = 0;
  index = 0;
  done = false;
  size_t strl = strlen(str);
  if(strl > 0){
    st = new char[strl + 1];
    strcpy(st,str);
  }
  else{
    done = true;
  }
  if (!st) fail(MSG_OPEN, "Buffer allocation failed in StrMgr.");
}

StrMgr::~StrMgr()
{
  if (st)
    delete st;
}

char * StrMgr::getline() {
  if(done)
    return NULL;
  int i = 0;
  for(; st[index] != '\n' && st[index] != EOF && st[index] != '\0' && i < BUFSIZE - 1; i++, index++)
    in[i] = st[index];
  if(st[index] == EOF || st[index] == '\0' && i == 0){
    done = true;
    return NULL;
  }
  if(i == BUFSIZE - 1)
    in[0] = '\0';
  else
    in[i] = st[index];
  done = st[index] == EOF || st[index] == '\0';
  in[i + 1] = '\0';
  linenum++;
  index++;
  return in;
}

int StrMgr::getlinenum() {
  return linenum;
}
