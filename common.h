#ifndef COMMON_H
#define COMMON_H

#pragma once

#include <gst.h>

extern GST *g_pGST;

#define SAFE_DELETE(p) { if(p) { delete (p);(p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete [](p);(p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release();(p)=NULL; } }

#endif
