#include "license.nodehun"
#include <node_version.h>
#define NODE_0_12_MODULE_VERSION 12
#if (NODE_MODULE_VERSION < NODE_0_12_MODULE_VERSION)
#include "pre0.12.0/nodehun.cpp"
#else
#include "post0.12.0/nodehun.cpp"
#endif
   
