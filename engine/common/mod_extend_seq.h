#ifndef MOD_EXTEND_SEQ_H
#define MOD_EXTEND_SEQ_H

#include "mod_local.h"

#ifdef __cplusplus
extern "C" {
#endif

int Mod_NumExtendSeq(const char *mod_name);
byte *Mod_LoadExtendSeq(const char *mod_name, byte *buffer, fs_offset_t *filesize);


#ifdef __cplusplus
}
#endif
#endif