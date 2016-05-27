#include "Win32Window.h"
#include "Logger.h"
#include "common.h"
#include "resource.h"
#include <windows.h>
#include <string.h>
#include <commctrl.h>

#define ISPOW2(a) ((a) & ((a)-1))

enum { OBS_CONVEYOR, OBS_LASER, OBS_TURN, OBS_HAMMER, OBS_BUTTON,
        OBS_PIT, OBS_CRANE, OBS_WALL, OBS_GOAL, OBS_MULTI, OBS_TIMER, OBS_NUMOBS };

BOOL CALLBACK ObstacleProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AnalyzeProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void LoadObsList();
void LoadObsComboBox(HWND temp);

enum { ID_FILE_NEW=700, ID_FILE_OPEN, ID_FILE_CLOSE, ID_FILE_SAVE, ID_FILE_SAVEAS,
		ID_HELP_ABOUT };

enum { ANALYZE, OBSTACLE, BMPS, BMPPREVIEW, ABOUT, ADD, CHANGEFINAL, NUMWINDOWS };

Win32Window Window;
Logger Log;

HWND ahWnd[NUMWINDOWS];

FileData SaveData;

char GlobalDir[512];

int ConvertID()
{
	char value[64]="";
	DlgDirSelectEx(ahWnd[ANALYZE],value,64,IDC_OBSTACLELIST);

	value[strlen(value)-1]='\0';

	char Index[4];
	for(int a=strlen(value)-1;a>=0;a--)
	{
		if(value[a]==' ' && value[a-1]=='-')
			break;
	}
	int stop=a+1;
	for(a=stop;a<(signed)strlen(value);a++)
		Index[a-stop]=value[a];
	Index[a-stop]='\0';
	return atoi(Index);
}

double FileSize(char *filename)
{
	ifstream file;
	
	file.open(filename,ios::nocreate | ios::binary | ios::in);
	
	if(!file)
		return 0;
	
	double first=file.tellg();
	
	file.seekg(0,ios::end);
	
	double size=file.tellg()-first;
	
	file.close();

	return size;
}

void OpenLevel(char* FileName)
{
	if(ahWnd[ANALYZE]!=NULL)
	{
		EndDialog(ahWnd[ANALYZE],0);
		DestroyWindow(ahWnd[ANALYZE]);
		ahWnd[ANALYZE]=NULL;
	}
	ahWnd[ANALYZE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_ANALYZE),Window.GetHWND(),(DLGPROC)AnalyzeProc);

	SetWindowText(ahWnd[ANALYZE],FileName);
	HWND temp;

	char Buff[1024]="Invalid Finalized Format";

	switch(SaveData.Finalized)
	{
	case 0:
		strcpy(Buff,"Unfinalized");
		break;
	case 1:
		strcpy(Buff,"Finalized");
		break;
	case 2:
		strcpy(Buff,"Tutorial Level");
		break;
	case 3:
		strcpy(Buff,"Story Mode Level");
		break;
	}
	SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_FINALIZED),Buff);

	SetDlgItemInt(ahWnd[ANALYZE],IDC_X,SaveData.PlayerStart.X,false);
	SetDlgItemInt(ahWnd[ANALYZE],IDC_Y,SaveData.PlayerStart.Y,false);

	SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_VERSION),SaveData.Version);

	temp=GetDlgItem(ahWnd[ANALYZE],IDC_TILEBMP);
	strcpy(Buff,SaveData.TileName);
	if(!FileSize(Buff))
	{
		EnableWindow(temp,false);
		Window.Error("Error",MB_OK,"%s does not exist, make sure that it is in the SAME folder as this level!",Buff);
		strcat(Buff,"  File does not exist.");
	}
	SetWindowText(temp,Buff);

	temp=GetDlgItem(ahWnd[ANALYZE],IDC_OBJECTBMP);
	strcpy(Buff,SaveData.ObjeName);
	if(!FileSize(Buff))
	{
		EnableWindow(temp,false);
		Window.Error("Error",MB_OK,"%s does not exist, make sure that it is in the SAME folder as this level!",Buff);
		strcat(Buff,"  File does not exist.");
	}
	SetWindowText(temp,Buff);
	
	temp=GetDlgItem(ahWnd[ANALYZE],IDC_SPRITEBMP);
	strcpy(Buff,SaveData.PlayName);
	if(!FileSize(Buff))
	{
		EnableWindow(temp,false);
		Window.Error("Error",MB_OK,"%s does not exist, make sure that it is in the SAME folder as this level!",Buff);
		strcat(Buff,"  File does not exist.");
	}
	SetWindowText(temp,Buff);

	temp=GetDlgItem(ahWnd[ANALYZE],IDC_TRANSITIONBMP);
	if(SaveData.Transition[0]=='\0')
	{
		EnableWindow(temp,false);
		SetWindowText(temp,"( NO TRANSITION PICTURE )");
	}
	else
	{
		strcpy(Buff,SaveData.Transition);
		if(!FileSize(Buff))
		{
			EnableWindow(temp,false);
			Window.Error("Error",MB_OK,"%s does not exist, make sure that it is in the SAME folder as this level!",Buff);
			strcat(Buff,"  File does not exist.");
		}
		SetWindowText(temp,Buff);
	}

	temp=GetDlgItem(ahWnd[ANALYZE],IDC_NUMOBSTACLES);
	sprintf(Buff,"%d",SaveData.numObstacles);
	SetWindowText(temp,Buff);

	temp=GetDlgItem(ahWnd[ANALYZE],IDC_NEXTLEVEL);
	if(SaveData.NextLevel[0]=='\0')
	{
		SetWindowText(temp,"( NO NEXT LEVEL )");
		EnableWindow(temp,false);
	}
	else
	{
		strcpy(Buff,SaveData.NextLevel);
		if(!FileSize(Buff))
		{
			EnableWindow(temp,false);
			Window.Error("Error",MB_OK,"%s does not exist, make sure that it is in the SAME folder as this level!",Buff);
			strcat(Buff,"  File does not exist.");
		}
		SetWindowText(temp,Buff);
	}

	temp=GetDlgItem(ahWnd[ANALYZE],IDC_SORTED);
	SendMessage(temp,BM_SETCHECK,BST_CHECKED,NULL);

	LoadObsList();

	ShowWindow(ahWnd[ANALYZE],SW_SHOW);
	UpdateWindow(ahWnd[ANALYZE]);
}

void RemObstacle(int ObstacleNum)
{
	unsigned char byte1,byte2,byte3;
	for(int a=0;a<SaveData.numObstacles;a++)
	{
		switch(SaveData.Obstacles[a].Type)
		{
		case OBS_BUTTON:
			if(SaveData.Obstacles[a].Dir>=SaveData.numObstacles-1)
				SaveData.Obstacles[a].Dir--;
			else if(SaveData.Obstacles[a].Dir>ObstacleNum)
				SaveData.Obstacles[a].Dir--;
			break;
		case OBS_MULTI:
			byte1=SaveData.Obstacles[a].Dir & 0x000000FF;
			if(byte1>=SaveData.numObstacles-1)
				byte1--;
			else if(byte1>ObstacleNum)
				byte1--;

			byte2=(SaveData.Obstacles[a].Dir & 0x0000FF00) >> 8;
			if(byte2>=SaveData.numObstacles-1)
				byte2--;
			else if(byte2>ObstacleNum)
				byte2--;

			byte3=(SaveData.Obstacles[a].Dir & 0x00FF0000) >> 16;
			if(byte3>=SaveData.numObstacles-1)
				byte3--;
			else if(byte3>ObstacleNum)
				byte3--;

			SaveData.Obstacles[a].Dir=SaveData.Obstacles[a].Dir & 0xFF000000;

			SaveData.Obstacles[a].Dir |= ((byte3<<16) | (byte2<<8) | (byte1));

			break;
		case OBS_TIMER:
			byte1=SaveData.Obstacles[a].Dir & 0x000000FF;
			if(byte1>=SaveData.numObstacles-1)
				byte1--;
			else if(byte1>ObstacleNum)
				byte1--;

			SaveData.Obstacles[a].Dir=SaveData.Obstacles[a].Dir & 0xFFFFFF00;
			SaveData.Obstacles[a].Dir |= byte1;

			break;
		}
	}
	if(ObstacleNum<SaveData.numObstacles)
	{
		for(a=ObstacleNum;a<SaveData.numObstacles-1;a++)
			SaveData.Obstacles[a]=SaveData.Obstacles[a+1];
	}
	SaveData.numObstacles--;
	char NumObs[32];
	sprintf(NumObs,"%d",SaveData.numObstacles);
	SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_NUMOBSTACLES),NumObs);
}

void CreateObstWindow(int ID)
{
	ObstacleSave tempObst;

	tempObst=SaveData.Obstacles[ID];
	char WindowName[64]="Default",State[32]="Default",Dir[32]="Default",Face[32]="Default";
	switch(tempObst.Type)
	{
	case OBS_CONVEYOR:
		strcpy(WindowName,"Conveyor Belt");

		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_DEFAULT),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"Down/Left");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"Up/Right");

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_SETCURSEL,tempObst.State,NULL);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_ADDSTRING,NULL,(LPARAM)"Down");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_ADDSTRING,NULL,(LPARAM)"Left");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_ADDSTRING,NULL,(LPARAM)"Up");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_ADDSTRING,NULL,(LPARAM)"Right");

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_SETCURSEL,(tempObst.State*2)+tempObst.Dir,NULL);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Doesn't Matter");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_SETCURSEL,0,NULL);

		EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),false);
		break;
	case OBS_LASER:
		strcpy(WindowName,"Laser");
		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_LASER),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"Off");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"On");

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_SETCURSEL,tempObst.State,NULL);

		SetDlgItemInt(ahWnd[OBSTACLE],IDC_DIRECTION,tempObst.Dir,NULL);
		
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Up");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Right");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Down");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Left");

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_SETCURSEL,tempObst.Face-2,NULL);
		break;
	case OBS_TURN:
		strcpy(WindowName,"Turn-Table");
		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_DEFAULT),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"CounterClockwise");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"Clockwise");

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_SETCURSEL,tempObst.State,NULL);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Doesn't Matter");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_SETCURSEL,0,NULL);
		EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),false);
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_ADDSTRING,NULL,(LPARAM)"Doesn't Matter");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_SETCURSEL,0,NULL);
		EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),false);
		break;
	case OBS_HAMMER:
		strcpy(WindowName,"Hammer");
		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_DEFAULT),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"Off");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"On");

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_SETCURSEL,tempObst.State,NULL);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_ADDSTRING,NULL,(LPARAM)"Doesn't Matter");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_SETCURSEL,0,NULL);
		EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),false);
		
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Down");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Left");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Up");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Right");

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_SETCURSEL,tempObst.Face,NULL);
		break;
	case OBS_BUTTON:
		strcpy(WindowName,"Button");
		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_BUTTON),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"Off");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"On");

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_SETCURSEL,!tempObst.State,NULL);

		SetDlgItemInt(ahWnd[OBSTACLE],IDC_BUTTONFOLLOW,tempObst.Dir,NULL);

		LoadObsComboBox(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX));
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX),CB_SETCURSEL,tempObst.Dir,NULL);
		break;
	case OBS_PIT:
		strcpy(WindowName,"Pit");
		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_DEFAULT),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"Closed");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"Open");

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_SETCURSEL,tempObst.State,NULL);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Doesn't Matter");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_SETCURSEL,0,NULL);
		EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),false);
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_ADDSTRING,NULL,(LPARAM)"Doesn't Matter");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_SETCURSEL,0,NULL);
		EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),false);
		break;
	case OBS_CRANE:
		strcpy(WindowName,"Crane");
		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_CRANE),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"Off");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"On");

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_SETCURSEL,tempObst.State,NULL);

		SetDlgItemInt(ahWnd[OBSTACLE],IDC_DROPX,(tempObst.Dir & 0x000000FF),NULL);
		SetDlgItemInt(ahWnd[OBSTACLE],IDC_DROPY,((tempObst.Dir & 0x0000FF00) >> 8),NULL);
		break;
	case OBS_WALL:
		strcpy(WindowName,"Wall (not used)");
		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_DEFAULT),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);
		break;
	case OBS_GOAL:
		strcpy(WindowName,"Goal");
		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_DEFAULT),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);

		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_ADDSTRING,NULL,(LPARAM)"Doesn't Matter");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),CB_SETCURSEL,0,NULL);
		EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_FACE),false);
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_ADDSTRING,NULL,(LPARAM)"Doesn't Matter");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),CB_SETCURSEL,0,NULL);
		EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_DIRECTION),false);
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_ADDSTRING,NULL,(LPARAM)"Doesn't Matter");
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),CB_SETCURSEL,0,NULL);
		EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_STATE),false);
		break;
	case OBS_MULTI:
		{
			strcpy(WindowName,"Multi-Obs (Hidden)");
			ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_MULTIOBS),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);

			int Obs1,Obs2,Obs3=0;

			Obs1=(tempObst.Dir & 0x000000FF);
			Obs2=((tempObst.Dir & 0x0000FF00) >> 8);

			SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_NUMOBS),CB_ADDSTRING,NULL,(LPARAM)"2");
			SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_NUMOBS),CB_ADDSTRING,NULL,(LPARAM)"3");

			if(GETBIT(tempObst.Dir,24))
			{
				SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_NUMOBS),CB_SETCURSEL,1,NULL);

				Obs3=((tempObst.Dir & 0x00FF0000) >> 16);
				EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_MULTIFOLLOW3),true);
				EnableWindow(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX3),true);

				SetDlgItemInt(ahWnd[OBSTACLE],IDC_MULTIFOLLOW3,Obs3,false);
				LoadObsComboBox(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX3));
				SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX3),CB_SETCURSEL,Obs3,NULL);
			}
			else
				SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_NUMOBS),CB_SETCURSEL,0,NULL);

			SetDlgItemInt(ahWnd[OBSTACLE],IDC_MULTIFOLLOW1,Obs1,false);
			LoadObsComboBox(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX1));
			SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX1),CB_SETCURSEL,Obs1,NULL);

			SetDlgItemInt(ahWnd[OBSTACLE],IDC_MULTIFOLLOW2,Obs2,false);
			LoadObsComboBox(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX2));
			SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX2),CB_SETCURSEL,Obs2,NULL);
		}
		break;
	case OBS_TIMER:
		strcpy(WindowName,"Timer (Hidden)");
		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_TIMER),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);

		SetDlgItemInt(ahWnd[OBSTACLE],IDC_FACE,tempObst.Face,NULL);
		SetDlgItemInt(ahWnd[OBSTACLE],IDC_DELAY,((tempObst.Dir & 0x00FFFF00) >> 8),NULL);
		SetDlgItemInt(ahWnd[OBSTACLE],IDC_TIMERFOLLOW,(tempObst.Dir & 0x000000FF),NULL);

		LoadObsComboBox(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX));
		SendMessage(GetDlgItem(ahWnd[OBSTACLE],IDC_OBSTACLEBOX),CB_SETCURSEL,(tempObst.Dir & 0x000000FF),NULL);
		break;
	default:
		strcpy(WindowName,"What the hell...ERROR");
		ahWnd[OBSTACLE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_DEFAULT),ahWnd[ANALYZE],(DLGPROC)ObstacleProc);
		break;
	}
	SetWindowText(ahWnd[OBSTACLE],WindowName);

	SetDlgItemInt(ahWnd[OBSTACLE],IDC_X,tempObst.Pos.X,NULL);
	SetDlgItemInt(ahWnd[OBSTACLE],IDC_Y,tempObst.Pos.Y,NULL);

	SetDlgItemInt(ahWnd[OBSTACLE],IDC_ID,ID,NULL);

	ShowWindow(ahWnd[OBSTACLE],SW_SHOW);
	UpdateWindow(ahWnd[OBSTACLE]);
}

void LoadObsComboBox(HWND temp)
{
	char Buff[1024];

	SendMessage(temp,LB_RESETCONTENT,NULL,NULL);

	if(SaveData.numObstacles>0)
	{
		for(int a=0;a<SaveData.numObstacles;a++)
		{
			switch(SaveData.Obstacles[a].Type)
			{
			case OBS_CONVEYOR:
				strcpy(Buff,"Conveyor Belt ");
				break;
			case OBS_LASER:
				strcpy(Buff,"Laser ");
				break;
			case OBS_TURN:
				strcpy(Buff,"Turn-Table ");
				break;
			case OBS_HAMMER:
				strcpy(Buff,"Hammer ");
				break;
			case OBS_BUTTON:
				strcpy(Buff,"Button ");
				break;
			case OBS_PIT:
				strcpy(Buff,"Pit ");
				break;
			case OBS_CRANE:
				strcpy(Buff,"Crane ");
				break;
			case OBS_WALL:
				strcpy(Buff,"Wall (not used) ");
				break;
			case OBS_GOAL:
				strcpy(Buff,"Goal ");
				break;
			case OBS_MULTI:
				strcpy(Buff,"Multi-Obs (Hidden) ");
				break;
			case OBS_TIMER:
				strcpy(Buff,"Timer (Hidden) ");
				break;
			default:
				strcpy(Buff,"What the hell...ERROR ");
				break;
			}
			sprintf(Buff,"%s - %d",Buff,a);
			SendMessage(temp,CB_ADDSTRING,NULL,(LPARAM)Buff);
		}
	}
}

void LoadObsList()
{
	HWND temp;
	char Buff[1024];
	temp=GetDlgItem(ahWnd[ANALYZE],IDC_OBSTACLELIST);

	int CurrSelect=SendMessage(temp,LB_GETCURSEL,NULL,NULL);

	SendMessage(temp,LB_RESETCONTENT,NULL,NULL);

	if(SaveData.numObstacles>0)
	{
		int state=SendMessage(GetDlgItem(ahWnd[ANALYZE],IDC_SORTED),BM_GETCHECK,NULL,NULL);

		for(int a=0;a<SaveData.numObstacles;a++)
		{
			int Counter=a;
			if(state!=BST_CHECKED)
				Counter=SaveData.numObstacles-a-1;
			switch(SaveData.Obstacles[Counter].Type)
			{
			case OBS_CONVEYOR:
				strcpy(Buff,"Conveyor Belt ");
				break;
			case OBS_LASER:
				strcpy(Buff,"Laser ");
				break;
			case OBS_TURN:
				strcpy(Buff,"Turn-Table ");
				break;
			case OBS_HAMMER:
				strcpy(Buff,"Hammer ");
				break;
			case OBS_BUTTON:
				strcpy(Buff,"Button ");
				break;
			case OBS_PIT:
				strcpy(Buff,"Pit ");
				break;
			case OBS_CRANE:
				strcpy(Buff,"Crane ");
				break;
			case OBS_WALL:
				strcpy(Buff,"Wall (not used) ");
				break;
			case OBS_GOAL:
				strcpy(Buff,"Goal ");
				break;
			case OBS_MULTI:
				strcpy(Buff,"Multi-Obs (Hidden) ");
				break;
			case OBS_TIMER:
				strcpy(Buff,"Timer (Hidden) ");
				break;
			default:
				strcpy(Buff,"What the hell...ERROR ");
				break;
			}
			sprintf(Buff,"%s - %d",Buff,Counter);
			if(state==BST_CHECKED)
				SendMessage(temp,LB_ADDSTRING,NULL,(LPARAM)Buff);
			else
				SendMessage(temp,LB_INSERTSTRING,0,(LPARAM)Buff);
		}
		if(CurrSelect>=SendMessage(temp,LB_GETCOUNT,NULL,NULL))
			CurrSelect--;
		SendMessage(temp,LB_SETCURSEL,CurrSelect,NULL);
	}
	else
	{
		EnableWindow(GetDlgItem(ahWnd[ANALYZE],IDC_OBSTACLEINFO),false);
		EnableWindow(GetDlgItem(ahWnd[ANALYZE],IDC_DELETE),false);
	}
}

void SaveFile(char* FileName, char* Filter, char* Ex)
{
	OPENFILENAME ofn;
	
	ZeroMemory(&ofn, sizeof(ofn));
	
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = Window.GetHWND();
	ofn.lpstrFilter = Filter;
	ofn.lpstrFile = FileName;

	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = Ex;
	
	GetSaveFileName(&ofn);
}

bool SaveLevel(char* FileName)
{
	if(FileName[0]!=' ')
	{
		FILE *savefile;
		savefile = fopen(FileName,"wb");
		if(savefile)
		{
			fwrite(&SaveData.Finalized,sizeof(char),1,savefile);
			fwrite(&SaveData.Version,sizeof(char)*10,1,savefile);
			fwrite(&SaveData.TileName,sizeof(char)*64,1,savefile);
			fwrite(&SaveData.ObjeName,sizeof(char)*64,1,savefile);
			fwrite(&SaveData.PlayName,sizeof(char)*64,1,savefile);

			fwrite(&SaveData.PlayerStart,sizeof(iCOORD),1,savefile);

			fwrite(&SaveData.numObstacles,sizeof(int),1,savefile);

			for(int a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
				fwrite(&SaveData.LevelData[a],sizeof(int),1,savefile);

			for(a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
				fwrite(&SaveData.LevelColDat[a],sizeof(int),1,savefile);

			fwrite(&SaveData.NextLevel,sizeof(char)*64,1,savefile);

			for(a=0;a<SaveData.numObstacles;a++)
			{
				fwrite(&SaveData.Obstacles[a].Pos,sizeof(iCOORD),1,savefile);

				fwrite(&SaveData.Obstacles[a].Dir,sizeof(int),1,savefile);
				fwrite(&SaveData.Obstacles[a].State,sizeof(bool),1,savefile);
				fwrite(&SaveData.Obstacles[a].Type,sizeof(int),1,savefile);
				fwrite(&SaveData.Obstacles[a].Face,sizeof(int),1,savefile);
			}
			fwrite(&SaveData.Transition,sizeof(char)*64,1,savefile);
			fclose(savefile);

			SetWindowText(ahWnd[ANALYZE],FileName);
			return true;
		}
		else
			return false;
	}
	else
		return false;
	return false;
}

void LoadFile(char* FileName, char* Filter, char* Ex, char* Path=" ")
{
	strcpy(FileName,"");
	OPENFILENAME ofn;
	
	ZeroMemory(&ofn, sizeof(ofn));	// Empty out the file structure
	
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = Window.GetHWND();
	ofn.lpstrFilter = Filter;

	if(Path[0]!=' ')
		ofn.lpstrInitialDir=Path;
	
	ofn.lpstrFile = FileName;
	ofn.hwndOwner = Window.GetHWND();
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = Ex;

	GetOpenFileName(&ofn);
}

bool LoadLevel(char* FileName)	// Loads a file in to the engine itself
{
	if(FileName[0]!='\0')
	{
		FILE *savefile;
		savefile = fopen(FileName,"rb");
		if(savefile)
		{
			fread(&SaveData.Finalized,sizeof(char),1,savefile);

			fread(&SaveData.Version,sizeof(char)*10,1,savefile);
			fread(&SaveData.TileName,sizeof(char)*64,1,savefile);
			fread(&SaveData.ObjeName,sizeof(char)*64,1,savefile);
			fread(&SaveData.PlayName,sizeof(char)*64,1,savefile);

			fread(&SaveData.PlayerStart,sizeof(iCOORD),1,savefile);

			fread(&SaveData.numObstacles,sizeof(int),1,savefile);

			for(int a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
				fread(&SaveData.LevelData[a],sizeof(int),1,savefile);

			for(a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
				fread(&SaveData.LevelColDat[a],sizeof(int),1,savefile);

			fread(&SaveData.NextLevel,sizeof(char)*64,1,savefile);

			for(a=0;a<SaveData.numObstacles;a++)	// Reads in all the obstacles
			{
				fread(&SaveData.Obstacles[a].Pos,sizeof(iCOORD),1,savefile);
				fread(&SaveData.Obstacles[a].Dir,sizeof(int),1,savefile);
				fread(&SaveData.Obstacles[a].State,sizeof(bool),1,savefile);
				fread(&SaveData.Obstacles[a].Type,sizeof(int),1,savefile);
				fread(&SaveData.Obstacles[a].Face,sizeof(int),1,savefile);
			}

			fclose(savefile);

			return true;
		}
		else
		{
			Window.Error("Error finding file",MB_OK,"Could not find %s, please make sure that it exits.",FileName);
			return false;
		}
	}
	return false;
}

BOOL CALLBACK AboutProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);

    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hWnd,NULL,true);
		UpdateWindow(hWnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hWnd,0);
		ahWnd[ABOUT]=NULL;
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_OK:
			EndDialog(hWnd,0);
			ahWnd[ABOUT]=NULL;
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK BMPPreviewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);
	if(msg==WM_PAINT)
	{
		BITMAP bm;
		PAINTSTRUCT ps;

		HDC hdc=BeginPaint(hWnd, &ps);
		HDC dc = CreateCompatibleDC(hdc);
		HBITMAP bBmp;

		char FileName[512];
		GetWindowText(hWnd,FileName,512);

		bBmp = (HBITMAP)LoadImage(Window.GetHINSTANCE(),FileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		HBITMAP hbmOld = (HBITMAP)SelectObject(dc, bBmp);
		GetObject(bBmp, sizeof(BITMAP), &bm);

		BitBlt(hdc,0,0,bm.bmWidth,bm.bmHeight,dc,0,0,SRCCOPY);

		SelectObject(dc, hbmOld);
		DeleteDC(dc);

		EndPaint(hWnd, &ps);
	}
    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hWnd,NULL,true);
		UpdateWindow(hWnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hWnd,0);
		return true;
		break;
	}
	return FALSE;
}

BOOL CALLBACK BMPProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);
    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hWnd,NULL,true);
		UpdateWindow(hWnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hWnd,0);
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BYTES:
		case IDC_KILOBYTES:
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:	// Make sure that "auto" and "group" are off for this to work
				CheckRadioButton(hWnd,IDC_BYTES,IDC_KILOBYTES,LOWORD(wParam));

				char Buff[512];

				char FileName[512];
				GetWindowText(hWnd,FileName,512);

				switch(LOWORD(wParam))
				{
				case IDC_BYTES:
					sprintf(Buff,"%d",(int)FileSize(FileName));
					break;
				case IDC_KILOBYTES:
					sprintf(Buff,"%f",(float)FileSize(FileName)/1024);
					break;
				}
				SetWindowText(GetDlgItem(hWnd,IDC_FILESIZE),Buff);
				UpdateWindow(hWnd);
				return true;
				break;
			}
			break;
		case IDC_OK:
			EndDialog(hWnd,0);
			break;
		case IDC_PREVIEW:
			ahWnd[BMPPREVIEW]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_BMPPREVIEW),hWnd,(DLGPROC)BMPPreviewProc);
			
			char FileName[512];
			GetWindowText(hWnd,FileName,512);
			SetWindowText(ahWnd[BMPPREVIEW],FileName);

			MoveWindow(ahWnd[BMPPREVIEW],0,0,590,590,true);
			ShowWindow(ahWnd[BMPPREVIEW],SW_SHOW);
			UpdateWindow(ahWnd[BMPPREVIEW]);
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK AddProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);
    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hWnd,NULL,true);
		UpdateWindow(hWnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hWnd,0);
		DestroyWindow(ahWnd[ADD]);
		ahWnd[ADD]=NULL;
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_OK:
			int Num=SaveData.numObstacles;
			SaveData.Obstacles[Num].Type=SendMessage(GetDlgItem(hWnd,IDC_TYPE),CB_GETCURSEL,NULL,NULL);

			switch(SaveData.Obstacles[Num].Type)
			{
			case OBS_CONVEYOR:
				SaveData.Obstacles[Num].Dir=DIR_NS;
				SaveData.Obstacles[Num].State=true;
				SaveData.Obstacles[Num].Face=UP;
				break;
			case OBS_PIT:
				SaveData.Obstacles[Num].Dir=0;
				SaveData.Obstacles[Num].State=true;
				SaveData.Obstacles[Num].Face=UP;
				break;
			case OBS_TURN:
				SaveData.Obstacles[Num].Dir=DIR_CC;
				SaveData.Obstacles[Num].State=true;
				SaveData.Obstacles[Num].Face=DOWN;
				break;
			case OBS_BUTTON:
				SaveData.Obstacles[Num].Dir=0;
				SaveData.Obstacles[Num].State=false;
				SaveData.Obstacles[Num].Face=UP;
				break;
			case OBS_MULTI:
				SaveData.Obstacles[Num].Dir=0;
				SaveData.Obstacles[Num].State=false;
				SaveData.Obstacles[Num].Face=UP;
				break;
			case OBS_HAMMER:
				SaveData.Obstacles[Num].State=true;
				SaveData.Obstacles[Num].Face=UP;
				break;
			case OBS_LASER:
				SaveData.Obstacles[Num].Dir=2;
				SaveData.Obstacles[Num].State=true;
				SaveData.Obstacles[Num].Face=DIR_N;
				break;
			case OBS_CRANE:
				SaveData.Obstacles[Num].Dir=0x00000A0A;
				SaveData.Obstacles[Num].State=true;
				SaveData.Obstacles[Num].Face=DIR_N;
				break;
			case OBS_TIMER:
				SaveData.Obstacles[Num].Dir=0;
				SaveData.Obstacles[Num].State=true;
				SaveData.Obstacles[Num].Face=0;
				break;
			}
			SaveData.Obstacles[Num].Pos.X=GetDlgItemInt(hWnd,IDC_X,NULL,NULL);
			SaveData.Obstacles[Num].Pos.Y=GetDlgItemInt(hWnd,IDC_Y,NULL,NULL);
			SaveData.numObstacles++;
			SetDlgItemInt(ahWnd[ANALYZE],IDC_NUMOBSTACLES,SaveData.numObstacles,false);
			LoadObsList();
			EndDialog(hWnd,0);
			DestroyWindow(ahWnd[ADD]);
			ahWnd[ADD]=NULL;
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK ChangeFinalProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);
    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hWnd,NULL,true);
		UpdateWindow(hWnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hWnd,0);
		DestroyWindow(ahWnd[CHANGEFINAL]);
		ahWnd[CHANGEFINAL]=NULL;
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_OK:
			SaveData.Finalized=(char)SendMessage(GetDlgItem(hWnd,IDC_FINALTYPES),CB_GETCURSEL,NULL,NULL);
			
			char Buff[64]="Invalid Finalid Format";

			switch(SaveData.Finalized)
			{
			case 0:
				strcpy(Buff,"Unfinalized");
				break;
			case 1:
				strcpy(Buff,"Finalized");
				break;
			case 2:
				strcpy(Buff,"Tutorial Level");
				break;
			case 3:
				strcpy(Buff,"Story Mode Level");
				break;
			}
			SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_FINALIZED),Buff);

			EndDialog(hWnd,0);
			DestroyWindow(ahWnd[CHANGEFINAL]);
			ahWnd[CHANGEFINAL]=NULL;
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK ObstacleProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);
    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hWnd,NULL,true);
		UpdateWindow(hWnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hWnd,0);
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_STATE:
		case IDC_DIRECTION:
			switch(HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				int ID=GetDlgItemInt(hWnd, IDC_ID, NULL, FALSE);
				switch(SaveData.Obstacles[ID].Type)
				{
				case OBS_CONVEYOR:
					{
						int Choice=SendMessage(GetDlgItem(hWnd,IDC_STATE),CB_GETCURSEL,NULL,NULL);
						int Choice2=SendMessage(GetDlgItem(hWnd,IDC_DIRECTION),CB_GETCURSEL,NULL,NULL);

						switch(LOWORD(wParam))
						{
						case IDC_STATE:
							if(Choice && Choice2<2)
								Choice2+=2;
							else if(!Choice && Choice2>1)
								Choice2-=2;
							break;
						case IDC_DIRECTION:
							Choice=(Choice2<=1) ? 0 : 1;
							break;
						}
						SendMessage(GetDlgItem(hWnd,IDC_STATE),CB_SETCURSEL,Choice,NULL);
						SendMessage(GetDlgItem(hWnd,IDC_DIRECTION),CB_SETCURSEL,Choice2,NULL);
					}
					break;
				default:
					break;
				}	
				break;
			}
			break;
		case IDC_OBSTACLEBOX:
		case IDC_OBSTACLEBOX1:
		case IDC_OBSTACLEBOX2:
		case IDC_OBSTACLEBOX3:
		case IDC_NUMOBS:
			switch(HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				int ID=GetDlgItemInt(hWnd, IDC_ID, NULL, FALSE);
				switch(SaveData.Obstacles[ID].Type)
				{
				case OBS_BUTTON:
					SetDlgItemInt(hWnd,IDC_BUTTONFOLLOW,SendMessage(GetDlgItem(hWnd,IDC_OBSTACLEBOX),CB_GETCURSEL,NULL,NULL),false);
					break;
				case OBS_TIMER:
					SetDlgItemInt(hWnd,IDC_TIMERFOLLOW,SendMessage(GetDlgItem(hWnd,IDC_OBSTACLEBOX),CB_GETCURSEL,NULL,NULL),false);
					break;
				case OBS_MULTI:
					switch(LOWORD(wParam))
					{
					case IDC_OBSTACLEBOX1:
						SetDlgItemInt(hWnd,IDC_MULTIFOLLOW1,SendMessage(GetDlgItem(hWnd,IDC_OBSTACLEBOX1),CB_GETCURSEL,NULL,NULL),false);
						break;
					case IDC_OBSTACLEBOX2:
						SetDlgItemInt(hWnd,IDC_MULTIFOLLOW2,SendMessage(GetDlgItem(hWnd,IDC_OBSTACLEBOX2),CB_GETCURSEL,NULL,NULL),false);
						break;
					case IDC_OBSTACLEBOX3:
						SetDlgItemInt(hWnd,IDC_MULTIFOLLOW3,SendMessage(GetDlgItem(hWnd,IDC_OBSTACLEBOX3),CB_GETCURSEL,NULL,NULL),false);
						break;
					case IDC_NUMOBS:
						if(!SendMessage(GetDlgItem(hWnd,IDC_NUMOBS),CB_GETCURSEL,NULL,NULL))
						{
							SendMessage(GetDlgItem(hWnd,IDC_OBSTACLEBOX3),CB_RESETCONTENT,NULL,NULL);
							SetDlgItemText(hWnd,IDC_MULTIFOLLOW3,"");
							EnableWindow(GetDlgItem(hWnd,IDC_OBSTACLEBOX3),false);
							EnableWindow(GetDlgItem(hWnd,IDC_MULTIFOLLOW3),false);
						}
						else
						{
							EnableWindow(GetDlgItem(hWnd,IDC_OBSTACLEBOX3),true);
							EnableWindow(GetDlgItem(hWnd,IDC_MULTIFOLLOW3),true);

							int Obs=((SaveData.Obstacles[ID].Dir & 0x00FF0000) >> 16);

							SetDlgItemInt(hWnd,IDC_MULTIFOLLOW3,Obs,false);
							LoadObsComboBox(GetDlgItem(hWnd,IDC_OBSTACLEBOX3));
							SendMessage(GetDlgItem(hWnd,IDC_OBSTACLEBOX3),CB_SETCURSEL,Obs,NULL);
						}
						break;
					}
					break;
				default:
					break;
				}	
				break;
			}
			break;
		case IDC_OK:
			{
				int ID=GetDlgItemInt(hWnd, IDC_ID, NULL, FALSE);
				int X=GetDlgItemInt(hWnd, IDC_X, NULL, FALSE);
				int Y=GetDlgItemInt(hWnd, IDC_Y, NULL, FALSE);
				if(X>800)
				{
					Window.Error("Error",MB_OK,"X value is too great, setting to 800.");
					X=800;
				}
				if(Y>600)
				{
					Window.Error("Error",MB_OK,"Y value is too great, setting to 600.");
					Y=600;
				}
				SaveData.Obstacles[ID].Pos.X=X;
				SaveData.Obstacles[ID].Pos.Y=Y;
				switch(SaveData.Obstacles[ID].Type)
				{
				case OBS_CONVEYOR:
					SaveData.Obstacles[ID].State=SendMessage(GetDlgItem(hWnd,IDC_STATE),CB_GETCURSEL,NULL,NULL) ? true : false;

					SaveData.Obstacles[ID].Dir=SendMessage(GetDlgItem(hWnd,IDC_DIRECTION),CB_GETCURSEL,NULL,NULL)%2;
					break;
				case OBS_LASER:
					SaveData.Obstacles[ID].State=SendMessage(GetDlgItem(hWnd,IDC_STATE),CB_GETCURSEL,NULL,NULL) ? true : false;

					SaveData.Obstacles[ID].Face=SendMessage(GetDlgItem(hWnd,IDC_FACE),CB_GETCURSEL,NULL,NULL)+2;

					SaveData.Obstacles[ID].Dir=GetDlgItemInt(hWnd,IDC_DIRECTION,NULL,false);
					if(SaveData.Obstacles[ID].Dir>=DEFAULTILEX)
					{
						SaveData.Obstacles[ID].Dir=DEFAULTILEX-1;
						Window.Error("Error",MB_OK,"Laser length is too long, shortening it to biggest default size: %d",DEFAULTILEX-1);
					}
					break;
				case OBS_TURN:
					SaveData.Obstacles[ID].State=SendMessage(GetDlgItem(hWnd,IDC_STATE),CB_GETCURSEL,NULL,NULL) ? true : false;
					break;
				case OBS_HAMMER:
					SaveData.Obstacles[ID].State=SendMessage(GetDlgItem(hWnd,IDC_STATE),CB_GETCURSEL,NULL,NULL) ? true : false;

					SaveData.Obstacles[ID].Face=SendMessage(GetDlgItem(hWnd,IDC_FACE),CB_GETCURSEL,NULL,NULL);
					break;
				case OBS_BUTTON:
					SaveData.Obstacles[ID].Dir=SendMessage(GetDlgItem(hWnd,IDC_OBSTACLEBOX),CB_GETCURSEL,NULL,NULL);
					break;
				case OBS_PIT:
					SaveData.Obstacles[ID].State=SendMessage(GetDlgItem(hWnd,IDC_STATE),CB_GETCURSEL,NULL,NULL) ? true : false;
					break;
				case OBS_CRANE:
					{
						SaveData.Obstacles[ID].State=SendMessage(GetDlgItem(hWnd,IDC_STATE),CB_GETCURSEL,NULL,NULL) ? true : false;

						int X=GetDlgItemInt(hWnd, IDC_DROPX, NULL, FALSE);
						int Y=GetDlgItemInt(hWnd, IDC_DROPY, NULL, FALSE);
						if(X>=DEFAULTILEX)
						{
							Window.Error("Error",MB_OK,"Drop X is too high, setting to default max size: %d",DEFAULTILEX-1);

							X=DEFAULTILEX-1;
						}
						if(Y>=DEFAULTILEY)
						{
							Window.Error("Error",MB_OK,"Drop Y is too high, setting to default max size: %d",DEFAULTILEY-1);

							Y=DEFAULTILEY-1;
						}
						SaveData.Obstacles[ID].Dir=((X & 0x000000FF) | ((Y & 0x000000FF) << 8));
					}
					break;
				case OBS_MULTI:
					{
						int Obs1,Obs2,Obs3=((SaveData.Obstacles[ID].Dir & 0x00FF0000) >> 16);

						Obs1=GetDlgItemInt(hWnd,IDC_MULTIFOLLOW1,NULL,NULL);
						Obs2=GetDlgItemInt(hWnd,IDC_MULTIFOLLOW2,NULL,NULL);

						SaveData.Obstacles[ID].Dir=0;

						if(IsWindowEnabled(GetDlgItem(hWnd,IDC_MULTIFOLLOW3)))
						{
							Obs3=GetDlgItemInt(hWnd,IDC_MULTIFOLLOW3,NULL,NULL);
							SaveData.Obstacles[ID].Dir=TOGBIT(SaveData.Obstacles[ID].Dir,24);
						}

						SaveData.Obstacles[ID].Dir|=((Obs1 & 0x000000FF) | ((Obs2 & 0x000000FF) << 8) | ((Obs3 & 0x000000FF) << 16));
					}
					break;
				case OBS_TIMER:
					{
						int Offset=GetDlgItemInt(hWnd,IDC_FACE,NULL,NULL);
						if(Offset>65534)
						{
							Window.Error("Error",MB_OK,"Offset is too high, setting to default max size: 65534");

							Offset=65534;
						}
						SaveData.Obstacles[ID].Face=Offset;

						int Delay=GetDlgItemInt(hWnd,IDC_DELAY,NULL,NULL);
						if(Delay>65534)
						{
							Window.Error("Error",MB_OK,"Delay is too high, setting to default max size: 65534");

							Delay=65534;
						}
						int TimerFollow=GetDlgItemInt(hWnd,IDC_TIMERFOLLOW,NULL,NULL);
						SaveData.Obstacles[ID].Dir=(((Delay & 0x0000FFFF) << 8) | (TimerFollow & 0x000000FF));

						// Save the pointed to obstacle
					}
					break;
				default:
					break;
				}
				EndDialog(hWnd,0);
			}
			break;
		case IDC_TIMERFOLLOW:
		case IDC_MULTIFOLLOW1:
		case IDC_MULTIFOLLOW2:
		case IDC_MULTIFOLLOW3:
		case IDC_BUTTONFOLLOW:
			{
				char ID[4];
				GetWindowText(GetDlgItem(hWnd,LOWORD(wParam)),ID,4);

				int IDNumb=atoi(ID);

				CreateObstWindow(IDNumb);
			}
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK AnalyzeProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);
    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hWnd,NULL,true);
		UpdateWindow(hWnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hWnd,0);
		DestroyWindow(hWnd);
		EnableMenuItem(Window.Menus[1],ID_FILE_SAVE,MF_GRAYED);
		EnableMenuItem(Window.Menus[1],ID_FILE_SAVEAS,MF_GRAYED);
		DrawMenuBar(Window.GetHWND());
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_ADD:
			{
				if(ahWnd[ADD]==NULL)
				{
					ahWnd[ADD]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_ADD),hWnd,(DLGPROC)AddProc);

					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Conveyor Belt");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Laser");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Turn-Table");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Hammer");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Button");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Pit");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Crane");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Wall (not used)");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Goal");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Multi-Obs (Hidden)");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_ADDSTRING,NULL,(LPARAM)"Timer (Hidden)");
					SendMessage(GetDlgItem(ahWnd[ADD],IDC_TYPE),CB_SETCURSEL,0,NULL);

					SetDlgItemInt(ahWnd[ADD],IDC_X,0,false);
					SetDlgItemInt(ahWnd[ADD],IDC_Y,0,false);

					SetDlgItemInt(ahWnd[ADD],IDC_ID,SaveData.numObstacles,false);

					ShowWindow(ahWnd[ADD],SW_SHOW);
					UpdateWindow(ahWnd[ADD]);
				}
			}
			break;
		case IDC_CHANGETRANS:
		case IDC_CHANGESPRITE:
		case IDC_CHANGEOBJ:
		case IDC_CHANGETILE:
			{
				char CurrDir[512];
				GetCurrentDirectory(511,CurrDir);
				char FileName[MAX_PATH]=" ";
				char Filt1[]="BMP Image(*.bmp)\0*.bmp\0";
				char Filt2[]="bmp";
				char Path[512];
				GetWindowText(ahWnd[ANALYZE],Path,511);
				for(int a=strlen(Path)-1;a>=0;a--)
				{
					if(Path[a]=='\\')
					{
						Path[a]='\0';
						break;
					}
				}
				if(!a)
					Path[0]=' ';
				LoadFile(FileName,Filt1,Filt2,Path);

				int Lng=strlen(FileName);

				if(FileName[0]!=' ' && tolower(FileName[Lng-1])=='p' && tolower(FileName[Lng-2])=='m' && tolower(FileName[Lng-3])=='b')
				{
					int Start=-1;
					for(int a=Lng-1;a>=0;a--)
					{
						if(FileName[a]=='\\')
						{
							Start=a+1;
							break;
						}
					}
					if(Start!=-1)
					{
						for(a=Start;a<Lng;a++)
							FileName[a-Start]=FileName[a];
						FileName[a-Start]='\0';
					}
					SetCurrentDirectory(CurrDir);
					if(!FileSize(FileName))
						Window.Error("Error",MB_OK,"Could not find %s, please make sure that it is in the current directory that this file is saved in.\nIf this file has not been saved yet, please do so, and then reload the bmp",FileName);
					else
					{
						HWND temp;
						switch(LOWORD(wParam))
						{
						case IDC_CHANGETILE:
							strcpy(SaveData.TileName,FileName);
							temp=GetDlgItem(ahWnd[ANALYZE],IDC_TILEBMP);
							break;
						case IDC_CHANGEOBJ:
							strcpy(SaveData.ObjeName,FileName);
							temp=GetDlgItem(ahWnd[ANALYZE],IDC_OBJECTBMP);
							break;
						case IDC_CHANGESPRITE:
							strcpy(SaveData.PlayName,FileName);
							temp=GetDlgItem(ahWnd[ANALYZE],IDC_SPRITEBMP);
							break;
						case IDC_CHANGETRANS:
							strcpy(SaveData.Transition,FileName);
							temp=GetDlgItem(ahWnd[ANALYZE],IDC_TRANSITIONBMP);
							break;
						}
						SetWindowText(temp,FileName);
						EnableWindow(temp,true);
					}
				}
				SetCurrentDirectory(CurrDir);
			}
			break;
		case IDC_CHANGENEXT:
			{
				char CurrDir[512];
				GetCurrentDirectory(511,CurrDir);
				char FileName[MAX_PATH]=" ";
				char Filt1[]="EFF Level(*.eff)\0*.eff\0";
				char Filt2[]="eff";
				char Path[512];
				GetWindowText(ahWnd[ANALYZE],Path,511);
				for(int a=strlen(Path)-1;a>=0;a--)
				{
					if(Path[a]=='\\')
					{
						Path[a]='\0';
						break;
					}
				}
				if(!a)
					Path[0]=' ';
				LoadFile(FileName,Filt1,Filt2,Path);

				int Lng=strlen(FileName);

				if(FileName[0]!=' ' && tolower(FileName[Lng-1])=='f' && tolower(FileName[Lng-2])=='f' && tolower(FileName[Lng-3])=='e')
				{
					int Start=-1;
					for(int a=Lng-1;a>=0;a--)
					{
						if(FileName[a]=='\\')
						{
							Start=a+1;
							break;
						}
					}
					if(Start!=-1)
					{
						for(a=Start;a<Lng;a++)
							FileName[a-Start]=FileName[a];
						FileName[a-Start]='\0';
					}
					SetCurrentDirectory(CurrDir);
					if(!FileSize(FileName))
						Window.Error("Error",MB_OK,"Could not find %s, please make sure that it is in the current directory that this file is saved in.\nIf this file has not been saved yet, please do so, and then reload the eff level",FileName);
					else
					{
						strcpy(SaveData.NextLevel,FileName);
						SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_NEXTLEVEL),FileName);
						EnableWindow(GetDlgItem(ahWnd[ANALYZE],IDC_NEXTLEVEL),true);
					}
				}
				SetCurrentDirectory(CurrDir);
			}
			break;
		case IDC_CHANGEFINAL:
			{
				if(ahWnd[CHANGEFINAL]==NULL)
				{
					ahWnd[CHANGEFINAL]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_CHANGEFINAL),hWnd,(DLGPROC)ChangeFinalProc);

					SendMessage(GetDlgItem(ahWnd[CHANGEFINAL],IDC_FINALTYPES),CB_ADDSTRING,NULL,(LPARAM)"Unfinalized");
					SendMessage(GetDlgItem(ahWnd[CHANGEFINAL],IDC_FINALTYPES),CB_ADDSTRING,NULL,(LPARAM)"Finalized");
					SendMessage(GetDlgItem(ahWnd[CHANGEFINAL],IDC_FINALTYPES),CB_ADDSTRING,NULL,(LPARAM)"Tutorial Level");
					SendMessage(GetDlgItem(ahWnd[CHANGEFINAL],IDC_FINALTYPES),CB_ADDSTRING,NULL,(LPARAM)"Story Mode Level");
					SendMessage(GetDlgItem(ahWnd[CHANGEFINAL],IDC_FINALTYPES),CB_SETCURSEL,SaveData.Finalized,NULL);

					ShowWindow(ahWnd[CHANGEFINAL],SW_SHOW);
					UpdateWindow(ahWnd[CHANGEFINAL]);
				}
			}
			break;
		case IDC_NEXTLEVEL:
			{
				char Level[64];
				GetWindowText(GetDlgItem(hWnd,IDC_NEXTLEVEL),Level,64);
				EndDialog(ahWnd[ANALYZE],0);
				DestroyWindow(ahWnd[ANALYZE]);
				ahWnd[ANALYZE]=NULL;
				if(LoadLevel(Level))
				{
					OpenLevel(Level);
					EnableMenuItem(Window.Menus[1],ID_FILE_SAVE,MF_ENABLED);
					EnableMenuItem(Window.Menus[1],ID_FILE_SAVEAS,MF_ENABLED);
					DrawMenuBar(Window.GetHWND());
				}
			}
			break;
		case IDC_TRANSITIONBMP:
		case IDC_OBJECTBMP:
		case IDC_SPRITEBMP:
		case IDC_TILEBMP:
			{
				char FileName[512];
				switch(LOWORD(wParam))
				{
				case IDC_OBJECTBMP:
					strcpy(FileName,SaveData.ObjeName);
					break;
				case IDC_SPRITEBMP:
					strcpy(FileName,SaveData.PlayName);
					break;
				case IDC_TILEBMP:
					strcpy(FileName,SaveData.TileName);
					break;
				}
				HBITMAP BMP;
				BMP = (HBITMAP)LoadImage(Window.GetHINSTANCE(),FileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(Window.GetHWND(), &ps);

				HDC hdcMem = CreateCompatibleDC(hdc);
				HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, BMP);
				BITMAP bmp;
				GetObject(BMP, sizeof(bmp), &bmp);

				int X=bmp.bmWidth;
				int Y=bmp.bmHeight;

				SelectObject(hdcMem, hbmOld);
				DeleteDC(hdcMem);

				EndPaint(Window.GetHWND(), &ps);

				ahWnd[BMPS]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_IMAGE),hWnd,(DLGPROC)BMPProc);

				SetWindowText(ahWnd[BMPS],FileName);
				char Buff[32];
				sprintf(Buff,"%d",X);
				SetWindowText(GetDlgItem(ahWnd[BMPS],IDC_WIDTHPIXELS),Buff);
				sprintf(Buff,"%d",Y);
				SetWindowText(GetDlgItem(ahWnd[BMPS],IDC_HEIGHTPIXELS),Buff);
				sprintf(Buff,"%d",(int)FileSize(FileName));
				SetWindowText(GetDlgItem(ahWnd[BMPS],IDC_FILESIZE),Buff);
				sprintf(Buff,"%d",(int)(X/32)*(int)(Y/32));
				SetWindowText(GetDlgItem(ahWnd[BMPS],IDC_NUMTILES),Buff);

				strcpy(Buff,"No Notes.");

				if(ISPOW2(X))
					strcpy(Buff,"WARNING, the width is NOT a power of two, this image will NOT load correctly.");
				if(ISPOW2(Y) && ISPOW2(X))
					strcat(Buff,"\nWARNING, the height is NOT a power of two, this image will NOT load correctly.");
				if(ISPOW2(Y) && !ISPOW2(X))
					strcpy(Buff,"\nWARNING, the height is NOT a power of two, this image will NOT load correctly.");
				SetWindowText(GetDlgItem(ahWnd[BMPS],IDC_NOTES),Buff);

				SendMessage(GetDlgItem(ahWnd[BMPS],IDC_BYTES),BM_SETCHECK,BST_CHECKED,NULL);

				ShowWindow(ahWnd[BMPS],SW_SHOW);
				UpdateWindow(ahWnd[BMPS]);
			}
			break;
		case IDC_UNSORTED:
		case IDC_SORTED:
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:	// Make sure that "auto" and "group" are off for this to work
				CheckRadioButton(ahWnd[ANALYZE],IDC_SORTED,IDC_UNSORTED,LOWORD(wParam));
				LoadObsList();
				UpdateWindow(hWnd);
				return true;
				break;
			}
			break;
		case IDC_DELETE:
			RemObstacle(ConvertID());
			LoadObsList();
			break;
		case IDC_OBSTACLELIST:
			switch(HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				{
					EnableWindow(GetDlgItem(ahWnd[ANALYZE],IDC_OBSTACLEINFO),true);
					EnableWindow(GetDlgItem(ahWnd[ANALYZE],IDC_DELETE),true);

					UpdateWindow(hWnd);
					return true;
				}
				break;
			case LBN_DBLCLK:
				CreateObstWindow(ConvertID());
				break;
			}
			return true;
			break;
		case IDC_OBSTACLEINFO:
			CreateObstWindow(ConvertID());
			break;
		case IDC_X:
			switch(HIWORD(wParam))
			{
			case EN_CHANGE:
				int X=GetDlgItemInt(hWnd,IDC_X,NULL,NULL);
				if(X>800)
				{
					X=800;
					SetDlgItemInt(hWnd,IDC_X,X,false);
				}
				SaveData.PlayerStart.X=X;
				break;
			}
			break;
		case IDC_Y:
			switch(HIWORD(wParam))
			{
			case EN_CHANGE:
				int Y=GetDlgItemInt(hWnd,IDC_Y,NULL,NULL);
				if(Y>575)
				{
					Y=575;
					SetDlgItemInt(hWnd,IDC_Y,Y,false);
				}
				SaveData.PlayerStart.Y=Y;
				break;
			}
			break;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);

	switch(msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		Window.KillWin();
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_FILE_NEW:
			if(ahWnd[ANALYZE]!=NULL)
			{
				if(MessageBox(Window.GetHWND(),"Are you sure that you want to start a new level and erase the current one?","Start New Level",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				{
					EndDialog(ahWnd[ANALYZE],0);
					DestroyWindow(ahWnd[ANALYZE]);
					ahWnd[ANALYZE]=NULL;
				}
				else
					break;
			}

			ahWnd[ANALYZE]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_ANALYZE),Window.GetHWND(),(DLGPROC)AnalyzeProc);

			SetWindowText(ahWnd[ANALYZE],"{ UNTITLED } PLEASE SAVE ME BEFORE TRYING TO LOAD BMPs");

			SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_FINALIZED),"Unfinalized");
			SaveData.Finalized=0;

			SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_VERSION),CURRVERSION);
			strcpy(SaveData.Version,CURRVERSION);

			EnableWindow(GetDlgItem(ahWnd[ANALYZE],IDC_TILEBMP),false);
			SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_TILEBMP),"Please load a file");
			strcpy(SaveData.TileName,"");

			EnableWindow(GetDlgItem(ahWnd[ANALYZE],IDC_OBJECTBMP),false);
			SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_OBJECTBMP),"Please load a file");
			strcpy(SaveData.ObjeName,"");

			EnableWindow(GetDlgItem(ahWnd[ANALYZE],IDC_SPRITEBMP),false);
			SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_SPRITEBMP),"Please load a file");
			strcpy(SaveData.PlayName,"");

			EnableWindow(GetDlgItem(ahWnd[ANALYZE],IDC_TRANSITIONBMP),false);
			SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_TRANSITIONBMP),"( NO TRANSITION PICTURE )");
			strcpy(SaveData.Transition,"");

			SetDlgItemInt(ahWnd[ANALYZE],IDC_NUMOBSTACLES,0,false);
			SaveData.numObstacles=0;

			EnableWindow(GetDlgItem(ahWnd[ANALYZE],IDC_NEXTLEVEL),false);
			SetWindowText(GetDlgItem(ahWnd[ANALYZE],IDC_NEXTLEVEL),"( NO NEXT LEVEL )");
			strcpy(SaveData.NextLevel,"");

			SetDlgItemInt(ahWnd[ANALYZE],IDC_X,32,false);
			SetDlgItemInt(ahWnd[ANALYZE],IDC_Y,32,false);
			SaveData.PlayerStart.X=32;
			SaveData.PlayerStart.Y=32;

			SendMessage(GetDlgItem(ahWnd[ANALYZE],IDC_SORTED),BM_SETCHECK,BST_CHECKED,NULL);

			EnableMenuItem(Window.Menus[1],ID_FILE_SAVEAS,MF_ENABLED);

			ShowWindow(ahWnd[ANALYZE],SW_SHOW);
			UpdateWindow(ahWnd[ANALYZE]);
			break;
		case ID_FILE_OPEN:
			{
				char FileName[MAX_PATH]=" ";
				char Filt1[]="EFFF Levels(*.eff)\0*.eff\0";
				char Filt2[]="eff";
				LoadFile(FileName,Filt1,Filt2);

				int Lng=strlen(FileName);

				if(LoadLevel(FileName) && tolower(FileName[Lng-1])=='f' && tolower(FileName[Lng-2])=='f' && tolower(FileName[Lng-3])=='e')
				{
					OpenLevel(FileName);

					EnableMenuItem(Window.Menus[1],ID_FILE_SAVE,MF_ENABLED);
					EnableMenuItem(Window.Menus[1],ID_FILE_SAVEAS,MF_ENABLED);
					DrawMenuBar(Window.GetHWND());
				}
			}
			break;
		case ID_FILE_SAVE:
			{
				char FileName[512];
				GetWindowText(ahWnd[ANALYZE],FileName,511);
				SaveLevel(FileName);
			}
			break;
		case ID_FILE_SAVEAS:
			{
				char FileName[MAX_PATH]=" ";
				char Filt1[]="EFFF Levels(*.eff)\0*.eff\0";
				char Filt2[]="eff";
				SaveFile(FileName,Filt1,Filt2);

				if(SaveLevel(FileName))
				{
					EnableMenuItem(Window.Menus[1],ID_FILE_SAVE,MF_ENABLED);
				}
			}
			break;
		case ID_FILE_CLOSE:
			Window.KillWin();
			PostQuitMessage(0);
			break;
		case ID_HELP_ABOUT:

			if(ahWnd[ABOUT]==NULL)
			{
				ahWnd[ABOUT]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_ABOUT),hWnd,(DLGPROC)AboutProc);
				ShowWindow(ahWnd[ABOUT],SW_SHOW);
				UpdateWindow(ahWnd[ABOUT]);
			}
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

void InitMenu()
{
	HMENU hMenu,hSubMenu1,hSubMenu2;

	hMenu = CreateMenu();
	hSubMenu1 = CreatePopupMenu();
	hSubMenu2 = CreatePopupMenu();

	AppendMenu(hMenu,MF_STRING | MF_POPUP,(UINT)hSubMenu1, "&File");
	AppendMenu(hMenu,MF_STRING | MF_POPUP,(UINT)hSubMenu2, "&Help");

	AppendMenu(hSubMenu1, MF_STRING, ID_FILE_NEW, "&New");
	AppendMenu(hSubMenu1, MF_STRING, ID_FILE_OPEN, "&Open...");
	AppendMenu(hSubMenu1, MF_STRING | MF_GRAYED, ID_FILE_SAVE, "&Save...");
	AppendMenu(hSubMenu1, MF_STRING | MF_GRAYED, ID_FILE_SAVEAS, "Save &As...");
	AppendMenu(hSubMenu1, MF_STRING, ID_FILE_CLOSE, "&Close");

	AppendMenu(hSubMenu2, MF_STRING, ID_HELP_ABOUT, "&About");

	Window.AddMenu();
	Window.AddMenu();
	Window.AddMenu();

	Window.Menus[0]=hMenu;
	Window.Menus[1]=hSubMenu1;
	Window.Menus[2]=hSubMenu2;

	SetMenu(Window.GetHWND(), Window.Menus[0]);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;
	GetCurrentDirectory(512,GlobalDir);
	InitCommonControls();

	Log.InitLog("EFFFLogFile.txt");

	Log.LoggingIsOn=false;

	Log.WriteToLog("Starting Log");

	Window.InitWindow(&Log);
	Window.InitIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOGO)),LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOGO)));

	if(!Window.CreateWin("EFFF File Viewer/Editor 2.0",640,480,hInstance,SW_SHOWMAXIMIZED))
		return FALSE;

	InitMenu();

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	SetCurrentDirectory(GlobalDir);

	Log.WriteToLog("Ending Log\n");

	return Msg.wParam;
}