#include "license.nodehun"
#ifdef _WIN32
#pragma message("Nodehun is no longer being actively maintained. Please email nathanjsweet@gmail.com If you are interested in taking over the project.")
#else
#warning "Nodehun is no longer being actively maintained. Please email nathanjsweet@gmail.com If you are interested in taking over the project."
#endif
#include <node_version.h>
#define NODE_0_12_MODULE_VERSION 12
#if (NODE_MODULE_VERSION < NODE_0_12_MODULE_VERSION)
#include "pre0.12.0/nodehun.cpp"
#else
#include "post0.12.0/nodehun.cpp"
#endif
   
