
#include <windows.h>
#include <tchar.h>
#include  <time.h> 
#include <WinDef.h>
#include "resource.h"

#pragma comment(lib,"winmm.lib")		
#pragma  comment(lib,"Msimg32.lib")		

#define WINDOW_WIDTH	815							
#define WINDOW_HEIGHT	600							
#define WINDOW_TITLE		L"大话西游"	
#define PARTICLE_NUMBER	50							


struct CHARACTER
{
	int		NowHp;		//当前生命值
	int		MaxHp;			//最大生命值
	int		NowMp;		//当前魔法值
	int		MaxMp;		//最大魔法值
	int		Level;			//角色等级
	int		Strength;		//角色力量值
	int		Intelligence; //角色智力值
	int		Agility;			//角色敏捷值
	int x;
	int y;
};

struct SNOW
{
	int x; //雪花的 X坐标 
	int y; //雪花的Y坐标
	BOOL exist;  //雪花是否存在
};


//定义一个动作枚举体
enum ActionTypes
{
	ACTION_TYPE_NORMAL = 0,		//普通攻击
	ACTION_TYPE_CRITICAL = 1,		//致命一击
	ACTION_TYPE_MAGIC = 2,		//魔法攻击
	ACTION_TYPE_MISS = 3,			//攻击落空	
	ACTION_TYPE_RECOVER = 4,	//释放恢复魔法 
};

HDC				g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;      //全局设备环境句柄与全局内存DC句柄
DWORD		g_tPre = 0, g_tNow = 0, g_mv;					//声明l两个函数来记录时间,g_tPre记录上一次绘图的时间，g_tNow记录此次准备绘图的时间
RECT				g_rect;				//定义一个RECT结构体，用于储存内部窗口区域的坐标
int					g_iFrameNum, g_iTxtNum;  //帧数和文字数量
wchar_t			text[8][100];  //存储着输出文字的字符串数组
BOOL			g_bCanAttack, g_bGameOver;   //两个游戏状态标识，角色是否可以攻击，游戏是否结束
SNOW			SnowFlowers[PARTICLE_NUMBER];   //雪花粒子数组
int					g_SnowNum = 0; //定义g_SnowNum用于雪花的计数
CHARACTER	Hero, Boss, Boss_wy;  //两个角色结构体实例，分别表示英雄和BOSS
ActionTypes	HeroActionType, BossActionType;  //两个动作类型枚举实例，分别表示英雄和BOSS的动作类型

HBITMAP		g_hBackGround, g_hGameOver, g_hVictory, g_hSnow;  //背景，游戏结束，游戏胜利的位图句柄
HBITMAP		g_hMonsterBitmap, g_hHeroBitmap, g_hRecoverSkill, g_hWyBitMap;  //BOSS角色图，英雄角色图，恢复技能图的位图句柄
HBITMAP		g_hSkillButton1, g_hSkillButton2, g_hSkillButton3, g_hSkillButton4;  //4个技能按钮的位图句柄
HBITMAP		g_hHeroSkill1, g_hHeroSkill2, g_hHeroSkill3;  //3个英雄技能效果的位图句柄
HBITMAP		g_hBossSkill1, g_hBossSkill2, g_hBossSkill3;  //3个BOSS技能效果的位图句柄
HBITMAP		g_hWySkill[19], g_hWyStand[4], g_hWyMove[8];
HBITMAP g_hcw, g_hrw;
HBITMAP g_hSprite[2] = { NULL }, g_hBackGround_mv;
HBITMAP g_hBackGround_mvzd, g_hxbk, g_hxbk2;
HBITMAP g_hTime[10];
HBITMAP g_hMagic[54], g_hXjn[34];
int picorder = 0, g_iNum = 0;
int nowscen = 0;
int g_iNum_mv = 0, g_iX = 0, g_iY = 0, g_iDirection = 0;
int HaveBoss = 1;
//POINT pt_canavailable[] = { 2732, 2921, 3394, 3338, 3864, 3525, 5340, 2719, 5252, 2660, 4918, 2509, 4667, 2596, 4084, 2269, 2732, 2921 };
POINT pt_canavailable[] = { 598, 1093, 2734, 2633, 3743, 2021, 2305, 938, 1896, 1089, 1257, 727 };
POINT pt_cannotavailable1[] = { 1357, 1749, 1869, 2037, 2793, 1645, 2285, 1345 };
POINT pt_cannotavailable2[] = { 2563, 1987, 2569, 2020, 2633, 2014, 2620, 1982 };
POINT pt_cannotavailable3[] = { 857, 1057, 1241, 1397, 1585, 1233, 1117, 989 };
HRGN canavailable;
HRGN cannotavailable1;
HRGN cannotavailable2;
HRGN cannotavailable3;
HFONT hFont;
int timeflag = 0, timeorder = 0;
int hdtflag = 0;
HMENU hMenu;


LRESULT CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);//窗口过程函数
BOOL						Game_Init(HWND hwnd);			//在此函数中进行资源的初始化
VOID							Game_Fight(HWND hwnd);		//在此函数中进行绘图代码的书写
BOOL						Game_ShutDown(HWND hwnd);	//在此函数中进行资源的清理
VOID							Die_Check(int NowHp, bool isHero);   //死亡检查函数
VOID							Message_Insert(wchar_t* str);  //文字消息处理函数
VOID							HeroAction_Logic();  //英雄动作逻辑判断函数
VOID							HeroAction_Paint();  //英雄动作绘图函数
VOID							BossAction_Logic(int); //怪物动作逻辑判断函数
VOID							BossAction_Paint(int); //怪物动作绘图函数
VOID							Snow_Paint();
VOID							Game_Paint(HWND hwnd);
VOID							ReStart();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wndClass = { 0 };							//用WINDCLASSEX定义了一个窗口类
	wndClass.cbSize = sizeof(WNDCLASSEX);			//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;					//设置指向窗口过程函数的指针
	wndClass.cbClsExtra = 0;								//窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra = 0;							//窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);    //指定窗口类的光标句柄。
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  //为hbrBackground成员指定一个白色画刷句柄	
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";		//用一个以空终止的字符串，指定窗口类的名字。

	if (!RegisterClassEx(&wndClass))				//设计完窗口后，需要对窗口类进行注册，这样才能创建该类型的窗口
		return -1;

	HWND hwnd = CreateWindow(L"ForTheDreamOfGameDevelop", WINDOW_TITLE,				//喜闻乐见的创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//调整窗口显示时的位置，使窗口左上角位于（250,80）处
	ShowWindow(hwnd, nShowCmd);    //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);						//对窗口进行更新，就像我们买了新房子要装修一样

	//游戏资源的初始化，若初始化失败，弹出一个消息框，并返回FALSE
	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"资源初始化失败", L"消息窗口", 0); //使用MessageBox函数，创建一个消息窗口
		return FALSE;
	}
	PlaySound(L"GameMedia\\梦幻西游原声-战斗1-森林.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //循环播放背景音乐 

	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(123));
	SetMenu(hwnd, hMenu);

	MSG msg = { 0 };				//定义并初始化msg
	while (msg.message != WM_QUIT)		//使用while循环，如果消息不是WM_QUIT消息，就继续循环
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage(&msg);		//将虚拟键消息转换为字符消息
			DispatchMessage(&msg);			//分发一个消息给窗口程序。
		}
		else
		{
			g_tNow = GetTickCount();   //获取当前系统时间
			if (g_tNow - g_tPre >= 60){
				if (nowscen == 1)
					Game_Fight(hwnd);
				else{
					Game_Paint(hwnd);
				}
			}
		}
	}

	UnregisterClass(L"ForTheDreamOfGameDevelop", wndClass.hInstance);  //程序准备结束，注销窗口类
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)						//switch语句开始
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case ID_40001:
			HaveBoss = 0;
			break;
		case ID_40002:
			HaveBoss = 1;
			break;
		}
		break;
	case WM_TIMER:						//定时器消息
		//Game_Fight(hwnd);                //调用Game_Paint（）函数进行窗口绘图;
		//g_iX + 217;
		picorder++;
		if (picorder == 4) picorder = 0;
		timeflag++;
		/*timeorder++;
		if (timeorder == 8)
		timeorder = 0;*/
		if (timeflag == 5){
			timeflag = 0;
			timeorder++;
		}
		//g_iNum++;
		//if (g_iNum == 7){
		//g_iNum = 0;
		//skillflag = 0;
		//}
		break;
	case WM_KEYDOWN:					//按键消息
		switch (wParam){
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		case VK_UP:
			if (PtInRegion(canavailable, g_iX, g_iY - 10) && !PtInRegion(cannotavailable1, g_iX, g_iY - 10) && !PtInRegion(cannotavailable2, g_iX, g_iY - 10) && !PtInRegion(cannotavailable3, g_iX, g_iY - 10))
				g_iY -= 10;
			g_iDirection = 3;
			if (g_iY < 0)
				g_iY = 0;
			break;
		case VK_RIGHT:
			if (PtInRegion(canavailable, g_iX + 10, g_iY) && !PtInRegion(cannotavailable1, g_iX + 10, g_iY - 10) && !PtInRegion(cannotavailable2, g_iX + 10, g_iY) && !PtInRegion(cannotavailable3, g_iX + 10, g_iY))
				g_iX += 10;
			g_iDirection = 2;
			/*if (g_iX > WINDOW_WIDTH - 75)
			g_iX = WINDOW_WIDTH - 75;*/
			break;
		case VK_DOWN:
			if (PtInRegion(canavailable, g_iX, g_iY + 10) && !PtInRegion(cannotavailable1, g_iX, g_iY + 10) && !PtInRegion(cannotavailable2, g_iX, g_iY + 10) && !PtInRegion(cannotavailable3, g_iX, g_iY + 10))
				g_iY += 10;
			g_iDirection = 0;
			/*if (g_iY > WINDOW_HEIGHT - 135)
			g_iY = WINDOW_HEIGHT - 135;*/
			break;
		case VK_LEFT:
			if (PtInRegion(canavailable, g_iX - 10, g_iY) && !PtInRegion(cannotavailable1, g_iX - 10, g_iY) && !PtInRegion(cannotavailable2, g_iX - 10, g_iY) && !PtInRegion(cannotavailable3, g_iX - 10, g_iY))
				g_iX -= 10;
			g_iDirection = 1;
			if (g_iX < 0)
				g_iX = 0;
			break;
		}
		g_mv = GetTickCount();
		break;

	case WM_LBUTTONDOWN:			//鼠标左键消息
		if (!g_bCanAttack)
		{
			int x = LOWORD(lParam);		//X坐标
			int y = HIWORD(lParam);		//Y坐标

			//在这修改点击哪个技能栏
			if (x >= 530 && x <= 570 && y >= 420 && y <= 470)
			{
				g_bCanAttack = true;
				HeroActionType = ACTION_TYPE_MAGIC;
			}
			if (x >= 0 && x <= 300 && y >= 0 && y <= 300)
			{
				g_bCanAttack = true;
				HeroActionType = ACTION_TYPE_NORMAL;
			}
			if (x >= 700 && x <= 760 && y >= 200 && y <= 226)
			{
				g_bCanAttack = true;
				HeroActionType = ACTION_TYPE_MAGIC;
			}
			if (x >= 650 && x <= 700 && y >= 420 && y <= 470)
			{
				g_bCanAttack = true;
				HeroActionType = ACTION_TYPE_RECOVER;
			}
		}
		break;
	case WM_DESTROY:					//若是窗口销毁消息
		Game_ShutDown(hwnd);			//调用自定义的资源清理函数Game_ShutDown（）进行退出前的资源清理
		PostQuitMessage(0);			//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
		break;									//跳出该switch语句

	default:										//若上述case条件都不符合，则执行该default语句
		return DefWindowProc(hwnd, message, wParam, lParam);		//调用缺省的窗口过程
	}

	return 0;									//正常退出
}



BOOL Game_Init(HWND hwnd)
{
	srand((unsigned)time(NULL));      //用系统时间初始化随机种子 
	

	HBITMAP bmp;

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);  //创建一个和hdc兼容的内存DC
	g_bufdc = CreateCompatibleDC(g_hdc);//再创建一个和hdc兼容的缓冲DC
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //建一个和窗口兼容的空的位图对象

	SelectObject(g_mdc, bmp);//将空位图对象放到mdc中

	wchar_t   filename[20];
	//五叶站立
	for (int i = 0; i<4; i++)
	{
		memset(filename, 0, sizeof(filename));  //filename的初始化
		swprintf_s(filename, L"wy\\%d.bmp", i);  //调用swprintf_s函数，“组装”出对应的图片文件名称
		g_hWyStand[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	//五叶移动
	for (int i = 0; i < 8; i++){
		memset(filename, 0, sizeof(filename));
		swprintf_s(filename, L"wy\\wyyd\\wyyda%d.bmp", i);
		g_hWyMove[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	//时间
	for (int i = 0; i < 9; i++){
		memset(filename, 0, sizeof(filename));
		swprintf_s(filename, L"time\\5\\%d.bmp", i + 1);
		g_hTime[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	//闪电技能
	/*g_hMagic*/
	for (int i = 0; i < 54; i++){
		memset(filename, 0, sizeof(filename));
		swprintf_s(filename, L"magic\\%d.bmp", i + 1);
		g_hMagic[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//小技能
	//g_hXjn
	for (int i = 0; i < 34; i++){
		memset(filename, 0, sizeof(filename));
		swprintf_s(filename, L"xjn\\%d.bmp", i + 1);
		g_hXjn[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	//载入一系列游戏资源图到位图句柄中
	g_hGameOver = (HBITMAP)LoadImage(NULL, L"GameMedia\\gameover.bmp", IMAGE_BITMAP, 1086, 396, LR_LOADFROMFILE);  //游戏结束位图
	g_hVictory = (HBITMAP)LoadImage(NULL, L"GameMedia\\victory.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);  //游戏胜利位图
	g_hBackGround = (HBITMAP)LoadImage(NULL, L"bg_al.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);  //背景位图
	g_hMonsterBitmap = (HBITMAP)LoadImage(NULL, L"nw9.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);  //怪物角色位图
	g_hHeroBitmap = (HBITMAP)LoadImage(NULL, L"nr.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);  //英雄角色位图
	g_hHeroSkill1 = (HBITMAP)LoadImage(NULL, L"GameMedia\\heroslash.bmp", IMAGE_BITMAP, 364, 140, LR_LOADFROMFILE);  //英雄1技能位图
	g_hHeroSkill2 = (HBITMAP)LoadImage(NULL, L"GameMedia\\heromagic.bmp", IMAGE_BITMAP, 374, 288, LR_LOADFROMFILE);  //英雄2技能位图
	g_hHeroSkill3 = (HBITMAP)LoadImage(NULL, L"GameMedia\\herocritical.bmp", IMAGE_BITMAP, 574, 306, LR_LOADFROMFILE);  //英雄3技能位图
	g_hSkillButton1 = (HBITMAP)LoadImage(NULL, L"jnl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	//g_hSkillButton1 = (HBITMAP)LoadImage(NULL,L"GameMedia\\skillbutton1.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);  //技能1图标位图
	//g_hSkillButton2= (HBITMAP)LoadImage(NULL,L"GameMedia\\skillbutton2.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);   //技能2图标位图
	//g_hSkillButton3= (HBITMAP)LoadImage(NULL,L"GameMedia\\skillbutton3.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);   //技能3图标位图
	g_hcw = (HBITMAP)LoadImage(NULL, L"cw_tx.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);   //技能4图标位图
	g_hrw = (HBITMAP)LoadImage(NULL, L"rw_tx.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hBossSkill1 = (HBITMAP)LoadImage(NULL, L"GameMedia\\monsterslash.bmp", IMAGE_BITMAP, 234, 188, LR_LOADFROMFILE);   //怪物1技能位图
	g_hBossSkill2 = (HBITMAP)LoadImage(NULL, L"GameMedia\\monstermagic.bmp", IMAGE_BITMAP, 387, 254, LR_LOADFROMFILE);  //怪物2技能位图
	g_hBossSkill3 = (HBITMAP)LoadImage(NULL, L"GameMedia\\monstercritical.bmp", IMAGE_BITMAP, 574, 306, LR_LOADFROMFILE); //怪物3技能位图
	g_hSnow = (HBITMAP)LoadImage(NULL, L"GameMedia\\snow.bmp", IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);   //雪花位图
	g_hRecoverSkill = (HBITMAP)LoadImage(NULL, L"GameMedia\\recover.bmp", IMAGE_BITMAP, 150, 150, LR_LOADFROMFILE);    //恢复技能位图
	g_hSprite[0] = (HBITMAP)LoadImage(NULL, L"nr_mv.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hSprite[1] = (HBITMAP)LoadImage(NULL, L"nr_st1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hBackGround_mv = (HBITMAP)LoadImage(NULL, L"bg1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hBackGround_mvzd = (HBITMAP)LoadImage(NULL, L"zd8.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hxbk = (HBITMAP)LoadImage(NULL, L"xbk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hxbk2 = (HBITMAP)LoadImage(NULL, L"xbk3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	for (int k = 0; k < 19; k++){
		memset(filename, 0, sizeof(filename));  //filename的初始化
		swprintf_s(filename, L"wy\\fight\\%d.bmp", k);  //调用swprintf_s函数，“组装”出对应的图片文件名称
		g_hWySkill[k] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	//g_hWySkill = (HBITMAP)LoadImage(NULL, L"wy\\xbk2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	GetClientRect(hwnd, &g_rect);		//取得内部窗口区域的大小

	Hero.NowHp = Hero.MaxHp = 500;	//设定玩家生命值的当前值及上限
	Hero.Level = 6;					//设定玩家角色等级
	Hero.NowMp = Hero.MaxMp = 60;    //玩家魔法值的当前值及上限
	Hero.Strength = 10;			//设定英雄的力量值
	Hero.Agility = 20;				//设定英雄的敏捷值   
	Hero.Intelligence = 10;		//设定英雄的智力值
	Hero.x = 580;
	Hero.y = 269;

	Boss.NowHp = Boss.MaxHp = 500;	//设定BOSS生命值的当前值及上限
	Boss.Level = 10;						//设定BOSS的等级
	Boss.Strength = 10;				//设定BOSS的力量值
	Boss.Agility = 10;                   //设定BOSS的敏捷值
	Boss.Intelligence = 10;			//设定BOSS的智力值
	Boss.x = 125;
	Boss.y = 100;

	Boss_wy.NowHp = Boss_wy.MaxHp = 500;	//设定BOSS生命值的当前值及上限
	Boss_wy.Level = 10;						//设定BOSS的等级
	Boss_wy.Strength = 10;				//设定BOSS的力量值
	Boss_wy.Agility = 10;                   //设定BOSS的敏捷值
	Boss_wy.Intelligence = 10;			//设定BOSS的智力值
	Boss_wy.x = 140;
	Boss_wy.y = 140;

	g_iTxtNum = 0;		//初始化显示消息数目

	//HFONT hFont;
	hFont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_mdc, TRANSPARENT);    //设置文字显示背景透明

	//g_iX = pt_canavailable[0].x;
	//g_iY = pt_canavailable[0].y;
	g_iX = 1587;
	g_iY = 950;
	g_iDirection = 3;
	g_iNum_mv = 0;


	canavailable = CreatePolygonRgn(pt_canavailable, 6, WINDING);
	cannotavailable1 = CreatePolygonRgn(pt_cannotavailable1, 4, WINDING);
	cannotavailable2 = CreatePolygonRgn(pt_cannotavailable2, 4, WINDING);
	cannotavailable3 = CreatePolygonRgn(pt_cannotavailable3, 4, WINDING);

	SetTimer(hwnd, 1, 200, NULL);
	if (nowscen == 1)
		Game_Fight(hwnd);  //调用一次游戏Main函数
	else
		Game_Paint(hwnd);
	return TRUE;
}
//战斗
VOID Game_Fight(HWND hwnd)
{
	wchar_t str[100];
	//背景

	SelectObject(g_bufdc, g_hBackGround);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	//时间
	if (!g_bCanAttack){
		if (8 - timeorder == 0){
			g_bCanAttack = true;
			HeroActionType = ACTION_TYPE_NORMAL;
			//break;
		}
		SelectObject(g_bufdc, g_hTime[8 - timeorder]);
		//BitBlt(g_mdc, 0, 0, 819, 656, g_bufdc, 0, 0, SRCCOPY);
		//TransparentBlt(g_mdc, 0, WINDOW_HEIGHT - 68 - 78, 819, 110, g_bufdc, 7, 550, 812, 110, RGB(255, 255, 255));
		TransparentBlt(g_mdc, WINDOW_WIDTH / 2 - 50, 50, 134, 60, g_bufdc, 353, 54, 134, 60, RGB(255, 255, 255));
	}

	/*hFont = CreateFont(100, 50, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
	SelectObject(g_mdc, hFont);
	SetTextColor(g_mdc, RGB(255, 0, 0));
	TextOut(g_mdc, 350, 100, L"01", 2);*/


	/*hFont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
	SelectObject(g_mdc, hFont);*/
	//下边框
	SelectObject(g_bufdc, g_hxbk);
	TransparentBlt(g_mdc, 0, WINDOW_HEIGHT - 88 - 76, 819, 110, g_bufdc, 7, 550, 812, 110, RGB(255, 255, 255));

	//宠物头像
	SelectObject(g_bufdc, g_hcw);
	BitBlt(g_mdc, 0, 0, 104, 35, g_bufdc, 0, 1, SRCCOPY);
	//宠物血条
	for (int i = 0; i < 22; i++)
		BitBlt(g_mdc, 35 + i * 3, 0, 3, 35, g_bufdc, 104, 1, SRCCOPY);
	//人物头像
	SelectObject(g_bufdc, g_hrw);
	BitBlt(g_mdc, WINDOW_WIDTH - 145, 0, 128, 43, g_bufdc, 496, 0, SRCCOPY);
	//人物血条
	for (int i = 0; i < (int)((double)Hero.NowHp / Hero.MaxHp * 22); i++)
		BitBlt(g_mdc, WINDOW_WIDTH - 145 + 47 + i * 4, 0, 4, 43, g_bufdc, 623, 0, SRCCOPY);

	if (!g_bGameOver)//如果游戏未结束
	{
		Snow_Paint(); //调用封装了雪花飞舞粒子效果的函数，模拟下雪场景
	}

	//显示对战消息
	SetTextColor(g_mdc, RGB(255, 255, 255));
	for (int i = 0; i<g_iTxtNum; i++)
		TextOut(g_mdc, 20, 410 + i * 18, text[i], wcslen(text[i]));

	//女娲
	if (Boss.NowHp>0)
	{
		SelectObject(g_bufdc, g_hMonsterBitmap);
		TransparentBlt(g_mdc, Boss.x, Boss.y, 210, 350, g_bufdc, picorder * 226, 0, 210, 350, RGB(255, 255, 255));
		swprintf_s(str,L"%d / %d",Boss.NowHp,Boss.MaxHp);
		SetTextColor(g_mdc,RGB(255,10,10));
		TextOut(g_mdc,Boss.x+50,Boss.y+120,str,wcslen(str));//
		SelectObject(g_mdc, CreateSolidBrush(RGB(255, 0, 0)));
		Rectangle(g_mdc, Boss.x + 100, Boss.y, Boss.x + 100 + (Boss.NowHp / Boss.MaxHp) * 100, Boss.y + 5);
	}
	//五叶
	if (Boss_wy.NowHp>0 && !(g_iFrameNum >= 91 && g_iFrameNum <= 119))
	{
		//for (int i = 1; i <= 10; i++){
		SelectObject(g_bufdc, g_hWyStand[picorder]);
		TransparentBlt(g_mdc, Boss_wy.x, Boss_wy.y, 99, 113, g_bufdc, 0, 0, 99, 113, RGB(255, 255, 255));
		//}
		//g_hWyStand
		swprintf_s(str, L"%d / %d", Boss_wy.NowHp, Boss_wy.MaxHp);
		SetTextColor(g_mdc, RGB(255, 10, 10));
		TextOut(g_mdc, Boss_wy.x + 50, Boss_wy.y + 120, str, wcslen(str));//
		SelectObject(g_mdc, CreateSolidBrush(RGB(255, 0, 0)));
		Rectangle(g_mdc, Boss_wy.x, Boss_wy.y, Boss_wy.x + (Boss_wy.NowHp / Boss_wy.MaxHp) * 100, Boss_wy.y + 5);
	}


	//贴上玩家图以及玩家生命值，魔法值的显示
	if (Hero.NowHp>0)
	{
		SelectObject(g_bufdc, g_hHeroBitmap);
		TransparentBlt(g_mdc, Hero.x, Hero.y, 113, 150, g_bufdc, picorder * 113, 0, 113, 150, RGB(255, 255, 255));

		swprintf_s(str,L"%d / %d",Hero.NowHp,Hero.MaxHp);
		SetTextColor(g_mdc,RGB(255,10,10));
		TextOut(g_mdc,Hero.x+20,Hero.y+100,str, wcslen(str));

		swprintf_s(str,L"%d / %d",Hero.NowMp,Hero.MaxMp);
		SetTextColor(g_mdc,RGB(10,10,255));
		TextOut(g_mdc,Hero.x+20,Hero.y+120,str, wcslen(str));

		////血
		//SelectObject(g_mdc, CreateSolidBrush(RGB(255, 0, 0)));
		//Rectangle(g_mdc, Hero.x, Hero.y, Hero.x + (Hero.NowHp / Hero.MaxHp) * 100, Hero.y + 5);
		////蓝
		//SelectObject(g_mdc, CreateSolidBrush(RGB(0, 0, 255)));
		////Rectangle(g_mdc, Boss.x, Boss.y, Boss.x + (Boss.NowHp / Boss.MaxHp) * 100, Boss.y + 5);
		//Rectangle(g_mdc, Hero.x, Hero.y + 5, Hero.x + (Hero.NowHp / Hero.MaxHp) * 100, Hero.y + 10);
	}

	if (g_bGameOver)
	{
		if (Hero.NowHp <= 0)
		{
			SelectObject(g_bufdc, g_hGameOver);
			BitBlt(g_mdc, 120, 50, 543, 396, g_bufdc, 543, 0, SRCAND);
			BitBlt(g_mdc, 120, 50, 543, 396, g_bufdc, 0, 0, SRCPAINT);
		}

		else
		{
			//SelectObject(g_bufdc,g_hVictory);
			//TransparentBlt(g_mdc,0,0,800,600,g_bufdc,0,0,800,600,RGB(0,0,0));//透明色为RGB(0,0,0)
			nowscen = 0;
		}
	}

	else if (!g_bCanAttack)		//贴上技能按钮
	{
		//贴技能栏
		SelectObject(g_bufdc, g_hSkillButton1);
		//BitBlt(g_mdc,530,0,100,300,g_bufdc,0,0,SRCCOPY);
		TransparentBlt(g_mdc, 700, 200, 61, 202, g_bufdc, 0, 0, 61, 202, RGB(255, 255, 255));
		/*SelectObject(g_bufdc,g_hSkillButton2);
		BitBlt(g_mdc,590,420,50,50,g_bufdc,0,0,SRCCOPY);
		SelectObject(g_bufdc,g_hSkillButton3);
		BitBlt(g_mdc,650,420,50,50,g_bufdc,0,0,SRCCOPY);
		SelectObject(g_bufdc,g_hSkillButton4);
		BitBlt(g_mdc,710,420,50,50,g_bufdc,0,0,SRCCOPY);*/
	}
	//游戏没结束，且为玩家不可攻击状态，便开始接下来的绘图
	else
	{
		g_iFrameNum++;

		if (g_iFrameNum >= 5 && g_iFrameNum <= 65)
		{

			if (g_iFrameNum == 5)
			{
				HeroAction_Logic();
				Die_Check(Boss.NowHp, false);
				Die_Check(Boss_wy.NowHp, false);
			}
			HeroAction_Paint();
		}

		if (g_iFrameNum == 75)
		{
			BossAction_Logic(0);
		}
		//*/
		if (g_iFrameNum >= 86 && g_iFrameNum <= 90)
		{
			BossAction_Paint(0);
		}
		//五叶攻击
		if (g_iFrameNum >= 91 && g_iFrameNum <= 119)
		{
			BossAction_Paint(1);
		}

		if (g_iFrameNum == 120)			//回合结束
		{
			timeorder = 0;
			g_bCanAttack = false;
			g_iFrameNum = 0;

			//每回合的魔法自然恢复
			if (!g_bGameOver)
			{
				int MpRecover = 2 * (rand() % Hero.Intelligence) + 6;
				Hero.NowMp += MpRecover;

				if (Hero.NowMp >= Hero.MaxMp)
				{
					Hero.NowMp = Hero.MaxMp;
				}

				swprintf_s(str, L"回合结束，自动恢复了【%d】点魔法值", MpRecover);
				Message_Insert(str);
			}
		}
	}

	if (hdtflag == 0){
		for (int i = 0; i < WINDOW_WIDTH / 5; i++){
			for (int j = 0; j < 5; j++)
				BitBlt(g_hdc, i + j*WINDOW_WIDTH / 5, 0, 1, WINDOW_HEIGHT, g_mdc, i + j*WINDOW_WIDTH / 5, 0, SRCCOPY);
			Sleep(20);
		}
		hdtflag = 1;
	}
	else{
		BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
	}
	//BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
	g_tPre = GetTickCount();     //记录此次绘图时间
}
//非战斗
VOID Game_Paint(HWND hwnd){
	//背景
	SelectObject(g_bufdc, g_hBackGround_mv);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, g_iX - WINDOW_WIDTH / 2, g_iY - WINDOW_HEIGHT / 2, SRCCOPY);

	

	//角色（移动和站立）
	if (g_tNow - g_mv < 100){
		SelectObject(g_bufdc, g_hSprite[0]);
	}
	else{
		SelectObject(g_bufdc, g_hSprite[1]);
	}
	TransparentBlt(g_mdc, WINDOW_WIDTH / 2 - 46, WINDOW_HEIGHT / 2 - 99, 85, 113, g_bufdc, g_iNum_mv * 85, g_iDirection*113.5, 85, 113, RGB(255, 255, 255));


	//覆盖层
	SelectObject(g_bufdc, g_hBackGround_mvzd);
	TransparentBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, g_iX - WINDOW_WIDTH / 2, g_iY - WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT, RGB(255, 255, 255));

	//下边框
	SelectObject(g_bufdc, g_hxbk2);
	TransparentBlt(g_mdc, 0, WINDOW_HEIGHT - 88 - 76, 819, 110, g_bufdc, 7, 550, 812, 110, RGB(255, 255, 255));


	if (hdtflag == 1){
		for (int i = 0; i < WINDOW_WIDTH / 5; i++){
			for (int j = 0; j < 5; j++)
				BitBlt(g_hdc, i + j*WINDOW_WIDTH / 5, 0, 1, WINDOW_HEIGHT, g_mdc, i + j*WINDOW_WIDTH / 5, 0, SRCCOPY);
			Sleep(20);
		}
		hdtflag = 0;
	}
	else{
		BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
	}


	//BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount();     //记录此次绘图时间
	g_iNum_mv++;
	if (g_iNum_mv == 4)
		g_iNum_mv = 0;
	if (rand() % 100 == 1) {
		ReStart();
	}
}

void Message_Insert(wchar_t* str)
{
	//如果未满8行消息，直接新开一条消息
	if (g_iTxtNum < 8)
	{
		swprintf_s(text[g_iTxtNum], str);
		g_iTxtNum++;
	}
	//如果满了8行消息，只有挤走最上面的一条消息
	else
	{
		for (int i = 0; i<g_iTxtNum; i++)
			swprintf_s(text[i], text[i + 1]);
		swprintf_s(text[7], str);
	}
}


void Die_Check(int NowHp, bool isHero)
{
	wchar_t str[100];

	if (NowHp <= 0)//生命值小于等于0了
	{
		
		if (isHero)  //如果是英雄的生命值小于零，表示游戏失败
		{
			PlaySound(L"GameMedia\\failure.wav", NULL, SND_FILENAME | SND_ASYNC); //播放失败音乐 
			swprintf_s(str, L": ( 胜败乃兵家常事，大侠请重新来过......");  //输出失败提示语
			Message_Insert(str);  //插入到文字消息中
			g_bGameOver = true;
		}
		else
		{
			if (Boss_wy.NowHp <= 0 && Boss.NowHp <= 0){
				PlaySound(L"GameMedia\\victory.wav", NULL, SND_FILENAME | SND_ASYNC); //播放胜利音乐 
				swprintf_s(str, L"少年，你赢了，有两下子啊~~~~~！！！！");  //输出胜利提示语
				nowscen = 0;
				Message_Insert(str);  //插入到文字消息中
				g_bGameOver = true;
			}
		}
	}
}

VOID HeroAction_Logic()
{
	int damage = 0;
	wchar_t str[100];

	switch (HeroActionType)
	{
	case ACTION_TYPE_NORMAL:

		if (1 == rand() % 4)                 
		{
			HeroActionType = ACTION_TYPE_CRITICAL;
			damage = (int)(4.5f*(float)(3 * (rand() % Hero.Agility) + Hero.Level*Hero.Strength + 20));
			Boss.NowHp -= (int)damage;
			Boss_wy.NowHp -= (int)damage;
			swprintf_s(str, L"恩赐解脱触发..对怪物照成了【%d】点伤害", damage);
		}
		else       
		{
			damage = 3 * (rand() % Hero.Agility) + Hero.Level*Hero.Strength + 20;
			Boss.NowHp -= (int)damage;
			Boss_wy.NowHp -= (int)damage;
			swprintf_s(str, L"玩家...对怪物照成了【%d】点伤害", damage);
		}

		Message_Insert(str);
		break;

	case ACTION_TYPE_MAGIC:  //释放烈火剑法
		if (Hero.NowMp >= 30)
		{
			damage = 5 * (2 * (rand() % Hero.Agility) + Hero.Level*Hero.Intelligence);
			Boss.NowHp -= (int)damage;
			Boss_wy.NowHp -= (int)damage;
			Hero.NowMp -= 30;
			swprintf_s(str, L"...对怪物照成了【%d】点伤害", damage);
		}
		else
		{
			HeroActionType = ACTION_TYPE_MISS;
			swprintf_s(str, L"魔法值不足30点，施法失败");
		}
		Message_Insert(str);
		break;

	case ACTION_TYPE_RECOVER:  //使用气疗术

		if (Hero.NowMp >= 40)
		{
			Hero.NowMp -= 40;
			int HpRecover = 5 * (5 * (rand() % Hero.Intelligence) + 40);
			Hero.NowHp += HpRecover;
			if (Hero.NowHp >= Hero.MaxHp)
			{
				Hero.NowHp = Hero.MaxHp;
			}
			swprintf_s(str, L"玩家使用了气疗术，恢复了【%d】点生命值，感觉好多了。", HpRecover);
		}
		else
		{
			HeroActionType = ACTION_TYPE_MISS;
			swprintf_s(str, L"魔法值不足40点，施法失败");
		}
		Message_Insert(str);
		break;
	}

}


VOID HeroAction_Paint()
{
	switch (HeroActionType)
	{
	case ACTION_TYPE_NORMAL:
		if (g_iFrameNum >= 15 && g_iFrameNum <= 49){
			SelectObject(g_bufdc, g_hXjn[g_iFrameNum - 15]);
			TransparentBlt(g_mdc, 0, 0, 492 / 2 + 100, 593 / 2, g_bufdc, 0, 0, 492, 593, RGB(255, 255, 255));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		}
		break;

	case ACTION_TYPE_CRITICAL:
		//SelectObject(g_bufdc,g_hHeroSkill3);
		//TransparentBlt(g_mdc,20,60,574,306,g_bufdc,0,0,574,306,RGB(0,0,0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		if (g_iFrameNum - 4 <= 54){
			SelectObject(g_bufdc, g_hMagic[g_iFrameNum - 4]);
			TransparentBlt(g_mdc, 0, 0, WINDOW_WIDTH / 640 * WINDOW_WIDTH, WINDOW_WIDTH / 640 * WINDOW_HEIGHT, g_bufdc, 0, 0, 640, 479, RGB(255, 255, 255));//TransparentBlt函数的透明颜色设为RGB(0,0,0)

		}
		break;

	case ACTION_TYPE_MAGIC:
		SelectObject(g_bufdc, g_hHeroSkill2);
		TransparentBlt(g_mdc, 50, 100, 374, 288, g_bufdc, 0, 0, 374, 288, RGB(0, 0, 0));
		//if (g_iFrameNum - 4 <= 54){
		//	SelectObject(g_bufdc, g_hMagic[g_iFrameNum-4]);
		//	TransparentBlt(g_mdc, 0, 0, WINDOW_WIDTH / 640 *WINDOW_WIDTH, WINDOW_WIDTH / 640 *WINDOW_HEIGHT, g_bufdc, 0, 0, 640, 479, RGB(255, 255, 255));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		//
		//}
		break;

	case ACTION_TYPE_RECOVER:   //恢复，气疗术
		SelectObject(g_bufdc, g_hRecoverSkill);
		TransparentBlt(g_mdc, 560, 170, 150, 150, g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		break;
	}
}

VOID BossAction_Logic(int bossnum)
{
	srand((unsigned)time(NULL));
	if (Boss.NowHp > (Boss.MaxHp / 2))
	{
		switch (rand() % 3)
		{
		case 0:						//释放普通攻击“幽冥鬼火”
			BossActionType = ACTION_TYPE_NORMAL;
			break;
		case 1:						//释放致命一击
			BossActionType = ACTION_TYPE_CRITICAL;
			break;
		case 2:						//使用嗜血咒
			BossActionType = ACTION_TYPE_MAGIC;
			break;
		}
	}
	else								//生命值小于1/2时
	{
		switch (rand() % 3)
		{
		case 0:						//释放嗜血咒
			BossActionType = ACTION_TYPE_MAGIC;
			break;
		case 1:						//释放致命一击
			BossActionType = ACTION_TYPE_CRITICAL;
			break;
		case 2:						//使用梅肯斯姆回复
			BossActionType = ACTION_TYPE_RECOVER;
			break;
		}
	}
}

VOID BossAction_Paint(int bossnum)
{
	int damage = 0, recover = 0;
	wchar_t str[100];

	if (bossnum == 1){

		if (g_iFrameNum >= 91 && g_iFrameNum < 99){
			SelectObject(g_bufdc, g_hWyMove[g_iFrameNum - 91]);
			TransparentBlt(g_mdc, Boss_wy.x + (Hero.x - Boss_wy.x - 50) / 8 * (g_iFrameNum - 91), Boss_wy.y + (Hero.y - Boss_wy.y - 50) / 8 * (g_iFrameNum - 91), 97, 120, g_bufdc, 0, 0, 97, 120, RGB(255, 255, 255));
		}
		//sdaf
		else if (g_iFrameNum >= 99 && g_iFrameNum < 117){
			SelectObject(g_bufdc, g_hWySkill[g_iFrameNum - 99]);
			TransparentBlt(g_mdc, Hero.x - 50, Hero.y - 50, 157, 158, g_bufdc, 0, 0, 157, 158, RGB(255, 255, 255));

			damage = 10;
			Hero.NowHp -= (int)damage;

			swprintf_s(str, L"小五叶【 %d】 点伤害", damage);
			Message_Insert(str);

			Die_Check(Hero.NowHp, true);
		}
		return;
	}

	switch (BossActionType)
	{
	case ACTION_TYPE_NORMAL:
		SelectObject(g_bufdc, g_hBossSkill1);
		TransparentBlt(g_mdc, 500, 150, 234, 188, g_bufdc, 0, 0, 234, 188, RGB(0, 0, 0));//TransparentBlt函数的透明颜色设为RGB(0,0,0)
		//第30个画面时计算玩家受伤害程度并加入显示消息
		if (g_iFrameNum == 30)
		{
			damage = rand() % Boss.Agility + Boss.Level*Boss.Strength;
			Hero.NowHp -= (int)damage;

			swprintf_s(str, L"女娲释放幽冥鬼火...对玩家照成【 %d】 点伤害", damage);
			Message_Insert(str);

			Die_Check(Hero.NowHp, true);
		}
		break;

	case ACTION_TYPE_MAGIC:
		SelectObject(g_bufdc, g_hBossSkill2);
		TransparentBlt(g_mdc, 450, 150, 387, 254, g_bufdc, 0, 0, 387, 254, RGB(0, 0, 0));
		//第30个画面时计算玩家受伤害程度并加入显示消息
		if (g_iFrameNum == 30)
		{
			damage = 2 * (2 * (rand() % Boss.Agility) + Boss.Strength*Boss.Intelligence);
			Hero.NowHp -= damage;
			recover = (int)((float)damage*0.2f);
			Boss.NowHp += recover;
			swprintf_s(str, L"女娲释放嗜血咒...对玩家照成【 %d 】点伤害,自身恢复【%d】点生命值", damage, recover);
			Message_Insert(str);

			Die_Check(Hero.NowHp, true);
		}
		break;

	case ACTION_TYPE_CRITICAL:
		SelectObject(g_bufdc, g_hBossSkill3);
		TransparentBlt(g_mdc, 280, 100, 574, 306, g_bufdc, 0, 0, 574, 306, RGB(0, 0, 0));
		//第30个画面时计算玩家受伤害程度并加入显示消息
		if (g_iFrameNum == 30)
		{
			damage = 2 * (rand() % Boss.Agility + Boss.Level*Boss.Strength);
			Hero.NowHp -= (int)damage;

			swprintf_s(str, L"女娲...对玩家照成【%d】点伤害.", damage);
			Message_Insert(str);

			Die_Check(Hero.NowHp, true);
		}
		break;

	case ACTION_TYPE_RECOVER:
		SelectObject(g_bufdc, g_hRecoverSkill);
		TransparentBlt(g_mdc, 150, 150, 150, 150, g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));
		//第30个画面时怪物回复生命值并加入显示消息
		if (g_iFrameNum == 30)
		{
			recover = 2 * Boss.Intelligence*Boss.Intelligence;
			Boss.NowHp += recover;
			swprintf_s(str, L"女娲...恢复了【%d】点生命值", recover);
			Message_Insert(str);
		}
		break;
	}
}

VOID Snow_Paint()
{
	if (g_SnowNum< PARTICLE_NUMBER)
	{
		SnowFlowers[g_SnowNum].x = rand() % g_rect.right;
		SnowFlowers[g_SnowNum].y = 0;
		SnowFlowers[g_SnowNum].exist = true;
		g_SnowNum++;
	}

	for (int i = 0; i<PARTICLE_NUMBER; i++)
	{
		if (SnowFlowers[i].exist)
		{
			SelectObject(g_bufdc, g_hSnow);
			TransparentBlt(g_mdc, SnowFlowers[i].x, SnowFlowers[i].y, 20, 20, g_bufdc, 0, 0, 20, 20, RGB(0, 0, 0));


			if (rand() % 2 == 0)
				SnowFlowers[i].x += rand() % 6;
			else
				SnowFlowers[i].x -= rand() % 6;

			SnowFlowers[i].y += 10;

			if (SnowFlowers[i].y > g_rect.bottom)
			{
				SnowFlowers[i].x = rand() % g_rect.right;
				SnowFlowers[i].y = 0;
			}
		}

	}
}


VOID ReStart()
{
	if (!HaveBoss) return;
	Boss.NowHp = Boss.MaxHp;
	Boss_wy.NowHp = Boss_wy.MaxHp;
	g_bGameOver = false;
	g_bCanAttack = false;
	nowscen = 1;
}

BOOL Game_ShutDown(HWND hwnd)
{
	DeleteObject(g_hBackGround);
	DeleteObject(g_hBackGround);
	DeleteObject(g_hGameOver);
	DeleteObject(g_hVictory);
	DeleteObject(g_hSnow);
	DeleteObject(g_hMonsterBitmap);
	DeleteObject(g_hHeroBitmap);
	DeleteObject(g_hRecoverSkill);
	DeleteObject(g_hSkillButton1);
	DeleteObject(g_hSkillButton2);
	DeleteObject(g_hSkillButton3);
	DeleteObject(g_hSkillButton4);
	DeleteObject(g_hHeroSkill1);
	DeleteObject(g_hHeroSkill2);
	DeleteObject(g_hHeroSkill3);
	DeleteObject(g_hBossSkill1);
	DeleteObject(g_hBossSkill2);
	DeleteObject(g_hBossSkill3);

	DeleteObject(g_hBackGround_mv);
	for (int i = 0; i<2; i++)
	{
		DeleteObject(g_hSprite[i]);
	}
	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd, g_hdc);
	return TRUE;
}
