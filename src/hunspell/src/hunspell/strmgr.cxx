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

  int size = 0,
    curIndex = index;

  while(st[index] != '\n' && st[index] != '\0')
  {
    index++;
    size++;
  }

  if(st[index] == '\0')
    done = true;
  else
    index++;

  if(size == 0)
    return getline();
  linenum++;
  int i = 0;
  while(st[curIndex] != '\n' && st[curIndex] != '\r' && st[curIndex] != '\0')
  {
    if(st[curIndex] != '\r')
      in[i++] = st[curIndex++];
    else
      curIndex++;
  }
  in[++i] = '\0';
  return in;
}

int StrMgr::getlinenum() {
  return linenum;
}
