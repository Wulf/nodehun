#include "license.hunspell"
#include "license.myspell"


#include "strmgr.hxx"
#include <iostream>
#include <string.h>
#include <stdlib.h>

int StrMgr::fail(const char * err, const char * par) {
  std::cout << err << "\n" << par;
  return -1;
}

StrMgr::StrMgr(const char * str, const char * key) {
  linenum = 0;
  index = 0;
  size = strlen(str);
  if(size > 0)
    st = strdup(str);
  if (!st) fail(MSG_OPEN, "Buffer allocation failed in StrMgr.");
}

StrMgr::~StrMgr()
{
  if (st)
    free(st);
}

char * StrMgr::getline() {
  if(index >= size)
    return NULL;
  int i = 0,
    l = BUFSIZE -1;
  for(;index < size && st[index] != '\n' && st[index] != EOF && i <= l; index++, i++)
    in[i] = st[index];
  in[i] = '\0';
  index++;
  if(index < size)
    linenum++;
  return strdup(in);
}

int StrMgr::getlinenum() {
  return linenum;
}
