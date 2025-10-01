/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once
#ifndef WEBM_UTIL_H
#define WEBM_UTIL_H

class WebmUtils : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Reset(void);
	void Shutdown();

	static void DrawWebm(int countframe, float nexttime, char folder1[64], char folder2[64], float w, float h, int h2, float x1, float y1, float x2, float y2, int r, int g, int b, int brightness);

private:
	duration_t tNextwebm;
	time_point_t timeseconds;
	duration_t tDeltaseconds;
	int frame;
	SharedTexture m_iTex2[5];
	char numfolder1[64];
	char numfolder2[64];
};

#endif