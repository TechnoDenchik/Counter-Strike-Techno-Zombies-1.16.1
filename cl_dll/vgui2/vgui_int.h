#pragma once
#ifdef __cplusplus
extern "C" {
#endif

	void GetHudSize(int& w, int& h);
	void InvalidateHudSizeCache();
	void GetHudSizeEx(int& w, int& h, int mode);

#ifdef __cplusplus
}
#endif