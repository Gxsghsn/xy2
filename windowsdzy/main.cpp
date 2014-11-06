
#include <windows.h>
#include <tchar.h>
#include  <time.h> 
#include <WinDef.h>
#include "resource.h"

#pragma comment(lib,"winmm.lib")		
#pragma  comment(lib,"Msimg32.lib")		

#define WINDOW_WIDTH	815							
#define WINDOW_HEIGHT	600							
#define WINDOW_TITLE		L"������"	
#define PARTICLE_NUMBER	50							


struct CHARACTER
{
	int		NowHp;		//��ǰ����ֵ
	int		MaxHp;			//�������ֵ
	int		NowMp;		//��ǰħ��ֵ
	int		MaxMp;		//���ħ��ֵ
	int		Level;			//��ɫ�ȼ�
	int		Strength;		//��ɫ����ֵ
	int		Intelligence; //��ɫ����ֵ
	int		Agility;			//��ɫ����ֵ
	int x;
	int y;
};

struct SNOW
{
	int x; //ѩ���� X���� 
	int y; //ѩ����Y����
	BOOL exist;  //ѩ���Ƿ����
};


//����һ������ö����
enum ActionTypes
{
	ACTION_TYPE_NORMAL = 0,		//��ͨ����
	ACTION_TYPE_CRITICAL = 1,		//����һ��
	ACTION_TYPE_MAGIC = 2,		//ħ������
	ACTION_TYPE_MISS = 3,			//�������	
	ACTION_TYPE_RECOVER = 4,	//�ͷŻָ�ħ�� 
};

HDC				g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;      //ȫ���豸���������ȫ���ڴ�DC���
DWORD		g_tPre = 0, g_tNow = 0, g_mv;					//����l������������¼ʱ��,g_tPre��¼��һ�λ�ͼ��ʱ�䣬g_tNow��¼�˴�׼����ͼ��ʱ��
RECT				g_rect;				//����һ��RECT�ṹ�壬���ڴ����ڲ��������������
int					g_iFrameNum, g_iTxtNum;  //֡������������
wchar_t			text[8][100];  //�洢��������ֵ��ַ�������
BOOL			g_bCanAttack, g_bGameOver;   //������Ϸ״̬��ʶ����ɫ�Ƿ���Թ�������Ϸ�Ƿ����
SNOW			SnowFlowers[PARTICLE_NUMBER];   //ѩ����������
int					g_SnowNum = 0; //����g_SnowNum����ѩ���ļ���
CHARACTER	Hero, Boss, Boss_wy;  //������ɫ�ṹ��ʵ�����ֱ��ʾӢ�ۺ�BOSS
ActionTypes	HeroActionType, BossActionType;  //������������ö��ʵ�����ֱ��ʾӢ�ۺ�BOSS�Ķ�������

HBITMAP		g_hBackGround, g_hGameOver, g_hVictory, g_hSnow;  //��������Ϸ��������Ϸʤ����λͼ���
HBITMAP		g_hMonsterBitmap, g_hHeroBitmap, g_hRecoverSkill, g_hWyBitMap;  //BOSS��ɫͼ��Ӣ�۽�ɫͼ���ָ�����ͼ��λͼ���
HBITMAP		g_hSkillButton1, g_hSkillButton2, g_hSkillButton3, g_hSkillButton4;  //4�����ܰ�ť��λͼ���
HBITMAP		g_hHeroSkill1, g_hHeroSkill2, g_hHeroSkill3;  //3��Ӣ�ۼ���Ч����λͼ���
HBITMAP		g_hBossSkill1, g_hBossSkill2, g_hBossSkill3;  //3��BOSS����Ч����λͼ���
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


LRESULT CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);//���ڹ��̺���
BOOL						Game_Init(HWND hwnd);			//�ڴ˺����н�����Դ�ĳ�ʼ��
VOID							Game_Fight(HWND hwnd);		//�ڴ˺����н��л�ͼ�������д
BOOL						Game_ShutDown(HWND hwnd);	//�ڴ˺����н�����Դ������
VOID							Die_Check(int NowHp, bool isHero);   //������麯��
VOID							Message_Insert(wchar_t* str);  //������Ϣ������
VOID							HeroAction_Logic();  //Ӣ�۶����߼��жϺ���
VOID							HeroAction_Paint();  //Ӣ�۶�����ͼ����
VOID							BossAction_Logic(int); //���ﶯ���߼��жϺ���
VOID							BossAction_Paint(int); //���ﶯ����ͼ����
VOID							Snow_Paint();
VOID							Game_Paint(HWND hwnd);
VOID							ReStart();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wndClass = { 0 };							//��WINDCLASSEX������һ��������
	wndClass.cbSize = sizeof(WNDCLASSEX);			//���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//���ô��ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;					//����ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra = 0;								//������ĸ����ڴ棬ȡ0�Ϳ�����
	wndClass.cbWndExtra = 0;							//���ڵĸ����ڴ棬��Ȼȡ0������
	wndClass.hInstance = hInstance;						//ָ���������ڹ��̵ĳ����ʵ�������
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //���ؼ����Զ���icoͼ��
	wndClass.hCursor = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);    //ָ��������Ĺ������
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  //ΪhbrBackground��Աָ��һ����ɫ��ˢ���	
	wndClass.lpszMenuName = NULL;						//��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = L"ForTheDreamOfGameDevelop";		//��һ���Կ���ֹ���ַ�����ָ������������֡�

	if (!RegisterClassEx(&wndClass))				//����괰�ں���Ҫ�Դ��������ע�ᣬ�������ܴ��������͵Ĵ���
		return -1;

	HWND hwnd = CreateWindow(L"ForTheDreamOfGameDevelop", WINDOW_TITLE,				//ϲ���ּ��Ĵ������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�250,80����
	ShowWindow(hwnd, nShowCmd);    //����ShowWindow��������ʾ����
	UpdateWindow(hwnd);						//�Դ��ڽ��и��£��������������·���Ҫװ��һ��

	//��Ϸ��Դ�ĳ�ʼ��������ʼ��ʧ�ܣ�����һ����Ϣ�򣬲�����FALSE
	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"��Դ��ʼ��ʧ��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����
		return FALSE;
	}
	PlaySound(L"GameMedia\\�λ�����ԭ��-ս��1-ɭ��.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //ѭ�����ű������� 

	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(123));
	SetMenu(hwnd, hMenu);

	MSG msg = { 0 };				//���岢��ʼ��msg
	while (msg.message != WM_QUIT)		//ʹ��whileѭ���������Ϣ����WM_QUIT��Ϣ���ͼ���ѭ��
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //�鿴Ӧ�ó�����Ϣ���У�����Ϣʱ�������е���Ϣ�ɷ���ȥ��
		{
			TranslateMessage(&msg);		//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);			//�ַ�һ����Ϣ�����ڳ���
		}
		else
		{
			g_tNow = GetTickCount();   //��ȡ��ǰϵͳʱ��
			if (g_tNow - g_tPre >= 60){
				if (nowscen == 1)
					Game_Fight(hwnd);
				else{
					Game_Paint(hwnd);
				}
			}
		}
	}

	UnregisterClass(L"ForTheDreamOfGameDevelop", wndClass.hInstance);  //����׼��������ע��������
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)						//switch��俪ʼ
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
	case WM_TIMER:						//��ʱ����Ϣ
		//Game_Fight(hwnd);                //����Game_Paint�����������д��ڻ�ͼ;
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
	case WM_KEYDOWN:					//������Ϣ
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

	case WM_LBUTTONDOWN:			//��������Ϣ
		if (!g_bCanAttack)
		{
			int x = LOWORD(lParam);		//X����
			int y = HIWORD(lParam);		//Y����

			//�����޸ĵ���ĸ�������
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
	case WM_DESTROY:					//���Ǵ���������Ϣ
		Game_ShutDown(hwnd);			//�����Զ������Դ������Game_ShutDown���������˳�ǰ����Դ����
		PostQuitMessage(0);			//��ϵͳ�����и��߳�����ֹ����������ӦWM_DESTROY��Ϣ
		break;									//������switch���

	default:										//������case�����������ϣ���ִ�и�default���
		return DefWindowProc(hwnd, message, wParam, lParam);		//����ȱʡ�Ĵ��ڹ���
	}

	return 0;									//�����˳�
}



BOOL Game_Init(HWND hwnd)
{
	srand((unsigned)time(NULL));      //��ϵͳʱ���ʼ��������� 
	

	HBITMAP bmp;

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);  //����һ����hdc���ݵ��ڴ�DC
	g_bufdc = CreateCompatibleDC(g_hdc);//�ٴ���һ����hdc���ݵĻ���DC
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //��һ���ʹ��ڼ��ݵĿյ�λͼ����

	SelectObject(g_mdc, bmp);//����λͼ����ŵ�mdc��

	wchar_t   filename[20];
	//��Ҷվ��
	for (int i = 0; i<4; i++)
	{
		memset(filename, 0, sizeof(filename));  //filename�ĳ�ʼ��
		swprintf_s(filename, L"wy\\%d.bmp", i);  //����swprintf_s����������װ������Ӧ��ͼƬ�ļ�����
		g_hWyStand[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	//��Ҷ�ƶ�
	for (int i = 0; i < 8; i++){
		memset(filename, 0, sizeof(filename));
		swprintf_s(filename, L"wy\\wyyd\\wyyda%d.bmp", i);
		g_hWyMove[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	//ʱ��
	for (int i = 0; i < 9; i++){
		memset(filename, 0, sizeof(filename));
		swprintf_s(filename, L"time\\5\\%d.bmp", i + 1);
		g_hTime[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	//���缼��
	/*g_hMagic*/
	for (int i = 0; i < 54; i++){
		memset(filename, 0, sizeof(filename));
		swprintf_s(filename, L"magic\\%d.bmp", i + 1);
		g_hMagic[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}

	//С����
	//g_hXjn
	for (int i = 0; i < 34; i++){
		memset(filename, 0, sizeof(filename));
		swprintf_s(filename, L"xjn\\%d.bmp", i + 1);
		g_hXjn[i] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	//����һϵ����Ϸ��Դͼ��λͼ�����
	g_hGameOver = (HBITMAP)LoadImage(NULL, L"GameMedia\\gameover.bmp", IMAGE_BITMAP, 1086, 396, LR_LOADFROMFILE);  //��Ϸ����λͼ
	g_hVictory = (HBITMAP)LoadImage(NULL, L"GameMedia\\victory.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);  //��Ϸʤ��λͼ
	g_hBackGround = (HBITMAP)LoadImage(NULL, L"bg_al.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);  //����λͼ
	g_hMonsterBitmap = (HBITMAP)LoadImage(NULL, L"nw9.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);  //�����ɫλͼ
	g_hHeroBitmap = (HBITMAP)LoadImage(NULL, L"nr.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);  //Ӣ�۽�ɫλͼ
	g_hHeroSkill1 = (HBITMAP)LoadImage(NULL, L"GameMedia\\heroslash.bmp", IMAGE_BITMAP, 364, 140, LR_LOADFROMFILE);  //Ӣ��1����λͼ
	g_hHeroSkill2 = (HBITMAP)LoadImage(NULL, L"GameMedia\\heromagic.bmp", IMAGE_BITMAP, 374, 288, LR_LOADFROMFILE);  //Ӣ��2����λͼ
	g_hHeroSkill3 = (HBITMAP)LoadImage(NULL, L"GameMedia\\herocritical.bmp", IMAGE_BITMAP, 574, 306, LR_LOADFROMFILE);  //Ӣ��3����λͼ
	g_hSkillButton1 = (HBITMAP)LoadImage(NULL, L"jnl.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	//g_hSkillButton1 = (HBITMAP)LoadImage(NULL,L"GameMedia\\skillbutton1.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);  //����1ͼ��λͼ
	//g_hSkillButton2= (HBITMAP)LoadImage(NULL,L"GameMedia\\skillbutton2.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);   //����2ͼ��λͼ
	//g_hSkillButton3= (HBITMAP)LoadImage(NULL,L"GameMedia\\skillbutton3.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);   //����3ͼ��λͼ
	g_hcw = (HBITMAP)LoadImage(NULL, L"cw_tx.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);   //����4ͼ��λͼ
	g_hrw = (HBITMAP)LoadImage(NULL, L"rw_tx.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hBossSkill1 = (HBITMAP)LoadImage(NULL, L"GameMedia\\monsterslash.bmp", IMAGE_BITMAP, 234, 188, LR_LOADFROMFILE);   //����1����λͼ
	g_hBossSkill2 = (HBITMAP)LoadImage(NULL, L"GameMedia\\monstermagic.bmp", IMAGE_BITMAP, 387, 254, LR_LOADFROMFILE);  //����2����λͼ
	g_hBossSkill3 = (HBITMAP)LoadImage(NULL, L"GameMedia\\monstercritical.bmp", IMAGE_BITMAP, 574, 306, LR_LOADFROMFILE); //����3����λͼ
	g_hSnow = (HBITMAP)LoadImage(NULL, L"GameMedia\\snow.bmp", IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);   //ѩ��λͼ
	g_hRecoverSkill = (HBITMAP)LoadImage(NULL, L"GameMedia\\recover.bmp", IMAGE_BITMAP, 150, 150, LR_LOADFROMFILE);    //�ָ�����λͼ
	g_hSprite[0] = (HBITMAP)LoadImage(NULL, L"nr_mv.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hSprite[1] = (HBITMAP)LoadImage(NULL, L"nr_st1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hBackGround_mv = (HBITMAP)LoadImage(NULL, L"bg1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hBackGround_mvzd = (HBITMAP)LoadImage(NULL, L"zd8.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hxbk = (HBITMAP)LoadImage(NULL, L"xbk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	g_hxbk2 = (HBITMAP)LoadImage(NULL, L"xbk3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	for (int k = 0; k < 19; k++){
		memset(filename, 0, sizeof(filename));  //filename�ĳ�ʼ��
		swprintf_s(filename, L"wy\\fight\\%d.bmp", k);  //����swprintf_s����������װ������Ӧ��ͼƬ�ļ�����
		g_hWySkill[k] = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
	//g_hWySkill = (HBITMAP)LoadImage(NULL, L"wy\\xbk2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	GetClientRect(hwnd, &g_rect);		//ȡ���ڲ���������Ĵ�С

	Hero.NowHp = Hero.MaxHp = 500;	//�趨�������ֵ�ĵ�ǰֵ������
	Hero.Level = 6;					//�趨��ҽ�ɫ�ȼ�
	Hero.NowMp = Hero.MaxMp = 60;    //���ħ��ֵ�ĵ�ǰֵ������
	Hero.Strength = 10;			//�趨Ӣ�۵�����ֵ
	Hero.Agility = 20;				//�趨Ӣ�۵�����ֵ   
	Hero.Intelligence = 10;		//�趨Ӣ�۵�����ֵ
	Hero.x = 580;
	Hero.y = 269;

	Boss.NowHp = Boss.MaxHp = 500;	//�趨BOSS����ֵ�ĵ�ǰֵ������
	Boss.Level = 10;						//�趨BOSS�ĵȼ�
	Boss.Strength = 10;				//�趨BOSS������ֵ
	Boss.Agility = 10;                   //�趨BOSS������ֵ
	Boss.Intelligence = 10;			//�趨BOSS������ֵ
	Boss.x = 125;
	Boss.y = 100;

	Boss_wy.NowHp = Boss_wy.MaxHp = 500;	//�趨BOSS����ֵ�ĵ�ǰֵ������
	Boss_wy.Level = 10;						//�趨BOSS�ĵȼ�
	Boss_wy.Strength = 10;				//�趨BOSS������ֵ
	Boss_wy.Agility = 10;                   //�趨BOSS������ֵ
	Boss_wy.Intelligence = 10;			//�趨BOSS������ֵ
	Boss_wy.x = 140;
	Boss_wy.y = 140;

	g_iTxtNum = 0;		//��ʼ����ʾ��Ϣ��Ŀ

	//HFONT hFont;
	hFont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("΢���ź�"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_mdc, TRANSPARENT);    //����������ʾ����͸��

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
		Game_Fight(hwnd);  //����һ����ϷMain����
	else
		Game_Paint(hwnd);
	return TRUE;
}
//ս��
VOID Game_Fight(HWND hwnd)
{
	wchar_t str[100];
	//����

	SelectObject(g_bufdc, g_hBackGround);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	//ʱ��
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

	/*hFont = CreateFont(100, 50, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("΢���ź�"));
	SelectObject(g_mdc, hFont);
	SetTextColor(g_mdc, RGB(255, 0, 0));
	TextOut(g_mdc, 350, 100, L"01", 2);*/


	/*hFont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("΢���ź�"));
	SelectObject(g_mdc, hFont);*/
	//�±߿�
	SelectObject(g_bufdc, g_hxbk);
	TransparentBlt(g_mdc, 0, WINDOW_HEIGHT - 88 - 76, 819, 110, g_bufdc, 7, 550, 812, 110, RGB(255, 255, 255));

	//����ͷ��
	SelectObject(g_bufdc, g_hcw);
	BitBlt(g_mdc, 0, 0, 104, 35, g_bufdc, 0, 1, SRCCOPY);
	//����Ѫ��
	for (int i = 0; i < 22; i++)
		BitBlt(g_mdc, 35 + i * 3, 0, 3, 35, g_bufdc, 104, 1, SRCCOPY);
	//����ͷ��
	SelectObject(g_bufdc, g_hrw);
	BitBlt(g_mdc, WINDOW_WIDTH - 145, 0, 128, 43, g_bufdc, 496, 0, SRCCOPY);
	//����Ѫ��
	for (int i = 0; i < (int)((double)Hero.NowHp / Hero.MaxHp * 22); i++)
		BitBlt(g_mdc, WINDOW_WIDTH - 145 + 47 + i * 4, 0, 4, 43, g_bufdc, 623, 0, SRCCOPY);

	if (!g_bGameOver)//�����Ϸδ����
	{
		Snow_Paint(); //���÷�װ��ѩ����������Ч���ĺ�����ģ����ѩ����
	}

	//��ʾ��ս��Ϣ
	SetTextColor(g_mdc, RGB(255, 255, 255));
	for (int i = 0; i<g_iTxtNum; i++)
		TextOut(g_mdc, 20, 410 + i * 18, text[i], wcslen(text[i]));

	//Ů�
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
	//��Ҷ
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


	//�������ͼ�Լ��������ֵ��ħ��ֵ����ʾ
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

		////Ѫ
		//SelectObject(g_mdc, CreateSolidBrush(RGB(255, 0, 0)));
		//Rectangle(g_mdc, Hero.x, Hero.y, Hero.x + (Hero.NowHp / Hero.MaxHp) * 100, Hero.y + 5);
		////��
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
			//TransparentBlt(g_mdc,0,0,800,600,g_bufdc,0,0,800,600,RGB(0,0,0));//͸��ɫΪRGB(0,0,0)
			nowscen = 0;
		}
	}

	else if (!g_bCanAttack)		//���ϼ��ܰ�ť
	{
		//��������
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
	//��Ϸû��������Ϊ��Ҳ��ɹ���״̬���㿪ʼ�������Ļ�ͼ
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
		//��Ҷ����
		if (g_iFrameNum >= 91 && g_iFrameNum <= 119)
		{
			BossAction_Paint(1);
		}

		if (g_iFrameNum == 120)			//�غϽ���
		{
			timeorder = 0;
			g_bCanAttack = false;
			g_iFrameNum = 0;

			//ÿ�غϵ�ħ����Ȼ�ָ�
			if (!g_bGameOver)
			{
				int MpRecover = 2 * (rand() % Hero.Intelligence) + 6;
				Hero.NowMp += MpRecover;

				if (Hero.NowMp >= Hero.MaxMp)
				{
					Hero.NowMp = Hero.MaxMp;
				}

				swprintf_s(str, L"�غϽ������Զ��ָ��ˡ�%d����ħ��ֵ", MpRecover);
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
	g_tPre = GetTickCount();     //��¼�˴λ�ͼʱ��
}
//��ս��
VOID Game_Paint(HWND hwnd){
	//����
	SelectObject(g_bufdc, g_hBackGround_mv);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, g_iX - WINDOW_WIDTH / 2, g_iY - WINDOW_HEIGHT / 2, SRCCOPY);

	

	//��ɫ���ƶ���վ����
	if (g_tNow - g_mv < 100){
		SelectObject(g_bufdc, g_hSprite[0]);
	}
	else{
		SelectObject(g_bufdc, g_hSprite[1]);
	}
	TransparentBlt(g_mdc, WINDOW_WIDTH / 2 - 46, WINDOW_HEIGHT / 2 - 99, 85, 113, g_bufdc, g_iNum_mv * 85, g_iDirection*113.5, 85, 113, RGB(255, 255, 255));


	//���ǲ�
	SelectObject(g_bufdc, g_hBackGround_mvzd);
	TransparentBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, g_iX - WINDOW_WIDTH / 2, g_iY - WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT, RGB(255, 255, 255));

	//�±߿�
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

	g_tPre = GetTickCount();     //��¼�˴λ�ͼʱ��
	g_iNum_mv++;
	if (g_iNum_mv == 4)
		g_iNum_mv = 0;
	if (rand() % 100 == 1) {
		ReStart();
	}
}

void Message_Insert(wchar_t* str)
{
	//���δ��8����Ϣ��ֱ���¿�һ����Ϣ
	if (g_iTxtNum < 8)
	{
		swprintf_s(text[g_iTxtNum], str);
		g_iTxtNum++;
	}
	//�������8����Ϣ��ֻ�м����������һ����Ϣ
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

	if (NowHp <= 0)//����ֵС�ڵ���0��
	{
		
		if (isHero)  //�����Ӣ�۵�����ֵС���㣬��ʾ��Ϸʧ��
		{
			PlaySound(L"GameMedia\\failure.wav", NULL, SND_FILENAME | SND_ASYNC); //����ʧ������ 
			swprintf_s(str, L": ( ʤ���˱��ҳ��£���������������......");  //���ʧ����ʾ��
			Message_Insert(str);  //���뵽������Ϣ��
			g_bGameOver = true;
		}
		else
		{
			if (Boss_wy.NowHp <= 0 && Boss.NowHp <= 0){
				PlaySound(L"GameMedia\\victory.wav", NULL, SND_FILENAME | SND_ASYNC); //����ʤ������ 
				swprintf_s(str, L"���꣬��Ӯ�ˣ��������Ӱ�~~~~~��������");  //���ʤ����ʾ��
				nowscen = 0;
				Message_Insert(str);  //���뵽������Ϣ��
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
			swprintf_s(str, L"���ͽ��Ѵ���..�Թ����ճ��ˡ�%d�����˺�", damage);
		}
		else       
		{
			damage = 3 * (rand() % Hero.Agility) + Hero.Level*Hero.Strength + 20;
			Boss.NowHp -= (int)damage;
			Boss_wy.NowHp -= (int)damage;
			swprintf_s(str, L"���...�Թ����ճ��ˡ�%d�����˺�", damage);
		}

		Message_Insert(str);
		break;

	case ACTION_TYPE_MAGIC:  //�ͷ��һ𽣷�
		if (Hero.NowMp >= 30)
		{
			damage = 5 * (2 * (rand() % Hero.Agility) + Hero.Level*Hero.Intelligence);
			Boss.NowHp -= (int)damage;
			Boss_wy.NowHp -= (int)damage;
			Hero.NowMp -= 30;
			swprintf_s(str, L"...�Թ����ճ��ˡ�%d�����˺�", damage);
		}
		else
		{
			HeroActionType = ACTION_TYPE_MISS;
			swprintf_s(str, L"ħ��ֵ����30�㣬ʩ��ʧ��");
		}
		Message_Insert(str);
		break;

	case ACTION_TYPE_RECOVER:  //ʹ��������

		if (Hero.NowMp >= 40)
		{
			Hero.NowMp -= 40;
			int HpRecover = 5 * (5 * (rand() % Hero.Intelligence) + 40);
			Hero.NowHp += HpRecover;
			if (Hero.NowHp >= Hero.MaxHp)
			{
				Hero.NowHp = Hero.MaxHp;
			}
			swprintf_s(str, L"���ʹ�������������ָ��ˡ�%d��������ֵ���о��ö��ˡ�", HpRecover);
		}
		else
		{
			HeroActionType = ACTION_TYPE_MISS;
			swprintf_s(str, L"ħ��ֵ����40�㣬ʩ��ʧ��");
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
			TransparentBlt(g_mdc, 0, 0, 492 / 2 + 100, 593 / 2, g_bufdc, 0, 0, 492, 593, RGB(255, 255, 255));//TransparentBlt������͸����ɫ��ΪRGB(0,0,0)
		}
		break;

	case ACTION_TYPE_CRITICAL:
		//SelectObject(g_bufdc,g_hHeroSkill3);
		//TransparentBlt(g_mdc,20,60,574,306,g_bufdc,0,0,574,306,RGB(0,0,0));//TransparentBlt������͸����ɫ��ΪRGB(0,0,0)
		if (g_iFrameNum - 4 <= 54){
			SelectObject(g_bufdc, g_hMagic[g_iFrameNum - 4]);
			TransparentBlt(g_mdc, 0, 0, WINDOW_WIDTH / 640 * WINDOW_WIDTH, WINDOW_WIDTH / 640 * WINDOW_HEIGHT, g_bufdc, 0, 0, 640, 479, RGB(255, 255, 255));//TransparentBlt������͸����ɫ��ΪRGB(0,0,0)

		}
		break;

	case ACTION_TYPE_MAGIC:
		SelectObject(g_bufdc, g_hHeroSkill2);
		TransparentBlt(g_mdc, 50, 100, 374, 288, g_bufdc, 0, 0, 374, 288, RGB(0, 0, 0));
		//if (g_iFrameNum - 4 <= 54){
		//	SelectObject(g_bufdc, g_hMagic[g_iFrameNum-4]);
		//	TransparentBlt(g_mdc, 0, 0, WINDOW_WIDTH / 640 *WINDOW_WIDTH, WINDOW_WIDTH / 640 *WINDOW_HEIGHT, g_bufdc, 0, 0, 640, 479, RGB(255, 255, 255));//TransparentBlt������͸����ɫ��ΪRGB(0,0,0)
		//
		//}
		break;

	case ACTION_TYPE_RECOVER:   //�ָ���������
		SelectObject(g_bufdc, g_hRecoverSkill);
		TransparentBlt(g_mdc, 560, 170, 150, 150, g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));//TransparentBlt������͸����ɫ��ΪRGB(0,0,0)
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
		case 0:						//�ͷ���ͨ��������ڤ���
			BossActionType = ACTION_TYPE_NORMAL;
			break;
		case 1:						//�ͷ�����һ��
			BossActionType = ACTION_TYPE_CRITICAL;
			break;
		case 2:						//ʹ����Ѫ��
			BossActionType = ACTION_TYPE_MAGIC;
			break;
		}
	}
	else								//����ֵС��1/2ʱ
	{
		switch (rand() % 3)
		{
		case 0:						//�ͷ���Ѫ��
			BossActionType = ACTION_TYPE_MAGIC;
			break;
		case 1:						//�ͷ�����һ��
			BossActionType = ACTION_TYPE_CRITICAL;
			break;
		case 2:						//ʹ��÷��˹ķ�ظ�
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

			swprintf_s(str, L"С��Ҷ�� %d�� ���˺�", damage);
			Message_Insert(str);

			Die_Check(Hero.NowHp, true);
		}
		return;
	}

	switch (BossActionType)
	{
	case ACTION_TYPE_NORMAL:
		SelectObject(g_bufdc, g_hBossSkill1);
		TransparentBlt(g_mdc, 500, 150, 234, 188, g_bufdc, 0, 0, 234, 188, RGB(0, 0, 0));//TransparentBlt������͸����ɫ��ΪRGB(0,0,0)
		//��30������ʱ����������˺��̶Ȳ�������ʾ��Ϣ
		if (g_iFrameNum == 30)
		{
			damage = rand() % Boss.Agility + Boss.Level*Boss.Strength;
			Hero.NowHp -= (int)damage;

			swprintf_s(str, L"Ů��ͷ���ڤ���...������ճɡ� %d�� ���˺�", damage);
			Message_Insert(str);

			Die_Check(Hero.NowHp, true);
		}
		break;

	case ACTION_TYPE_MAGIC:
		SelectObject(g_bufdc, g_hBossSkill2);
		TransparentBlt(g_mdc, 450, 150, 387, 254, g_bufdc, 0, 0, 387, 254, RGB(0, 0, 0));
		//��30������ʱ����������˺��̶Ȳ�������ʾ��Ϣ
		if (g_iFrameNum == 30)
		{
			damage = 2 * (2 * (rand() % Boss.Agility) + Boss.Strength*Boss.Intelligence);
			Hero.NowHp -= damage;
			recover = (int)((float)damage*0.2f);
			Boss.NowHp += recover;
			swprintf_s(str, L"Ů��ͷ���Ѫ��...������ճɡ� %d �����˺�,����ָ���%d��������ֵ", damage, recover);
			Message_Insert(str);

			Die_Check(Hero.NowHp, true);
		}
		break;

	case ACTION_TYPE_CRITICAL:
		SelectObject(g_bufdc, g_hBossSkill3);
		TransparentBlt(g_mdc, 280, 100, 574, 306, g_bufdc, 0, 0, 574, 306, RGB(0, 0, 0));
		//��30������ʱ����������˺��̶Ȳ�������ʾ��Ϣ
		if (g_iFrameNum == 30)
		{
			damage = 2 * (rand() % Boss.Agility + Boss.Level*Boss.Strength);
			Hero.NowHp -= (int)damage;

			swprintf_s(str, L"Ů�...������ճɡ�%d�����˺�.", damage);
			Message_Insert(str);

			Die_Check(Hero.NowHp, true);
		}
		break;

	case ACTION_TYPE_RECOVER:
		SelectObject(g_bufdc, g_hRecoverSkill);
		TransparentBlt(g_mdc, 150, 150, 150, 150, g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));
		//��30������ʱ����ظ�����ֵ��������ʾ��Ϣ
		if (g_iFrameNum == 30)
		{
			recover = 2 * Boss.Intelligence*Boss.Intelligence;
			Boss.NowHp += recover;
			swprintf_s(str, L"Ů�...�ָ��ˡ�%d��������ֵ", recover);
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
