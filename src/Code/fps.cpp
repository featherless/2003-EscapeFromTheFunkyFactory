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

#include "FPS.h"

FPS::FPS()
{
	count=time2=0;
	savg=1;
	update_fps=1;

	avg=new double[savg];

	for(int a=0;a<savg;a++)
		avg[a]=0;
}

void FPS::Set(int Update_MS,int FPS_Buf)
{
	if(avg)
		delete [] avg;

	update_fps=Update_MS;
	savg=FPS_Buf;
	avg=new double[savg];

	for(int a=0;a<savg;a++)
		avg[a]=0;
}

void FPS::Start()
{
	time1=GetTickCount();
}

void FPS::End()
{
	count++;

	time2+=GetTickCount()-time1;
	
	if(update_fps <= time2)
	{
		double *temp=new double[savg];
		
		for(int a=0;a<savg;a++)
		temp[a]=avg[a];

		avg[0]=1000/(time2/count);
		
		for(a=1;a<savg;a++)
		avg[a]=temp[a-1];
		delete []temp;

		count=time2=0;
	}
}

double FPS::Get()
{
	double addup=0;

	for(float a=0;a<savg;a++)
	{
		if(avg[int(a)])
		addup+=avg[int(a)];
		else
		break;
	}

	a+=(a<1);
	addup+=(addup<1);

	return addup/a;
}
