#ifndef MOD_DECRYPTOR_H
#define MOD_DECRYPTOR_H

#include "mod_local.h"

#ifdef __cplusplus
extern "C" {
#endif

	BOOL Mod_IsModelEncrypted(const char* model_name, const byte* buffer);
	void Mod_DecryptModel(model_t *mod, byte *buffer);

	
#ifdef __cplusplus
}
#endif
#endif