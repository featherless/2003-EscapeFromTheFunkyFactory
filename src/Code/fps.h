/*
 Copyright 2003 Josh Kasten. All Rights Reserved.
 Copyright 2003, 2016 Jeff Verkoeyen. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#ifndef __FPS_H__
#define __FPS_H__

// FPS.h v1.0 By Josh Kasten
#include <windows.h> // Needed for SYSTEMTIME

class FPS
{
public:
	double *avg,time2,count;
	unsigned long time1;

	int savg,update_fps;

	FPS();

	void Set(int Update_MS,int FPS_Buf);
	void Start();
	void End();
	double Get();
};
#endif