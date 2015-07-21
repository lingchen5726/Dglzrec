// dglzrecDlg.cpp : implementation file
//

#include "stdafx.h"
#include "windowsx.h"
#include <time.h>
#include "dglzrec.h"
#include "dglzrecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/*我们需要启动和终止线程，因此应增添一个全局变量来控制当前线程状态
1表示活动状态，0表示将终止线程。
*/
int threadFlag;
int paused = 0;
CString	m_Path;
DGLZ_T g_dglz;
DGLZ_T* g_p_dglz = &g_dglz;
char g_err_msg[GMSG_MAX] = {0};
char g_rest_msg[GMSG_MAX] = {0};
char g_player_msg[DGLZ_PLAYERS][GMSG_MAX] = {0};
extern CDglzrecApp theApp;
extern CDglzrecDlg dlg;
CFile fdparse;
static char stdbuf[BUF_SIZE*DGLZ_MAX_CARDS] = {0};
static CStatic* p_pk[PK_MAX] = {0};

/*
static DGLZ_VEDIO_T vedio_supported[2] = {
	{
		0,
		DGLZ_1280x1024_L_T,
		DGLZ_1280x1024_L_B,
		DGLZ_1280x1024_WIDTH,
		DGLZ_1280x1024_HEIGHT,
		DGLZ_1280x1024_A_X,
		DGLZ_1280x1024_B_X,
		DGLZ_1280x1024_C_X,
		DGLZ_1280x1024_A_Y,
		DGLZ_1280x1024_B_Y,
		DGLZ_1280x1024_C_Y,
		DGLZ_1280x1024_D_Y
	}, {
		1,
		DGLZ_1024x768_L_T,
		DGLZ_1024x768_L_B,
		DGLZ_1024x768_WIDTH,
		DGLZ_1024x768_HEIGHT,
		DGLZ_1024x768_A_X,
		DGLZ_1024x768_B_X,
		DGLZ_1024x768_C_X,
		DGLZ_1024x768_A_Y,
		DGLZ_1024x768_B_Y,
		DGLZ_1024x768_C_Y,
		DGLZ_1024x768_D_Y
	}
};
*/

UINT Dgzs(LPVOID param)
{
	while(1==threadFlag){
		switch (g_p_dglz->state) {
			case DGLZ_GAME_STARTED:
				DglzHandParse();
				//::MessageBox((HWND)param, "Dglz Started.", "Thread", MB_OK);
				break;
			case DGLZ_ROUND_PLAYING:
				DglzRPlayingHandle();
				break;
			default:
				memset(g_err_msg, 0x0, GMSG_MAX);
				sprintf(g_err_msg, "State machine error, state is %d", g_p_dglz->state);
				AfxMessageBox(g_err_msg);
				break;
		}
		Sleep(DGLZ_ROUND_CAP_DELAY);
	}
	g_p_dglz->state = DGLZ_GAME_STARTED;
//	::MessageBox((HWND)param, "Dglz Terminated.", "Thread", MB_OK);
	return 0;
}

void CreateParseFile()
{
	UINT32 i=0;
	CString name;
	CString tname;
	CFile fd;
	CFile tfd;
	LONG lData = 0;
	LONG tlOffset = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER); 
	char pbuf[BUF_SIZE] = {0};

	name.Format("E:\\game\\dglz_rec\\temp\\match.dat", i);
	if (fd.Open(name, CFile::modeCreate|CFile::modeWrite)){
		for(i=0;i<DGLZ_MAX_CARDS;i++){
			//open file
			tname.Format("E:\\game\\dglz_rec\\std_54\\%d.bmp", i+1);
			if (tfd.Open(tname, CFile::modeRead)){
				//fseek to sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
				tfd.Seek(tlOffset, CFile::begin);
				//Read DWORD size=btm.bmWidthBytes*btm.bmHeight;
				memset(pbuf, 0x0, BUF_SIZE);
				tfd.Read(pbuf, BUF_SIZE);
				tfd.Close();
				fd.Seek(i*BUF_SIZE, CFile::begin);
				fd.Write( pbuf, BUF_SIZE);
			}else {
				return ;
			}
		}
		fd.Close();
	}else{
		return ;
	}
}

BOOL LoadMyRes(UINT ResourceName, LPCSTR ResourceType, LPVOID* ppdata)
{
	BOOL   bResult   =   FALSE;  
	HGLOBAL hGlobal   =   NULL;  
	HRSRC hSource   =   NULL;  
	*ppdata = NULL;  

	hSource   =   FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(ResourceName), ResourceType);  
   	if (hSource==NULL) return(FALSE);  
   	hGlobal = LoadResource(AfxGetResourceHandle(), hSource);  
	if (hGlobal==NULL) return(FALSE);  
   	*ppdata = LockResource(hGlobal);
	return TRUE;
}

void LoadParseFile()
{
	LPVOID pbuf = NULL;
	
	if(TRUE==LoadMyRes(IDR_DAT1, _T("dat"), (LPVOID*)(&pbuf))){
		memset(g_err_msg, 0x0, GMSG_MAX);
		memcpy((void*)stdbuf, (void*)pbuf, (size_t)BUF_SIZE*DGLZ_MAX_CARDS);
//		sprintf(g_err_msg, "%2x, %2x, %2x, %2x, %2x, %2x, %2x, %2x ", stdbuf[0], stdbuf[1], stdbuf[2], stdbuf[3], stdbuf[4], stdbuf[5], stdbuf[6], stdbuf[7]);
//		AfxMessageBox(g_err_msg);
	}
}

UINT32 DglzRectCardParse(LPCRECT pRect, UINT32* p_card)
{
	CDC dc;
	HDC hDc;
	CBitmap bm;

	*p_card = 0;
	HWND hMain = ::FindWindow(NULL, "大怪路子");
	if (hMain){
		RECT r;
		::GetWindowRect(hMain, &r);
		g_p_dglz->vset.win_left = r.left;
		g_p_dglz->vset.win_top = r.top;
		hDc = ::GetDC(hMain);
		dc.Attach(hDc);
	}else {
		return 1;
	}
	int Width=pRect->right - pRect->left;
	int Height=pRect->bottom - pRect->top;
	bm.CreateCompatibleBitmap(&dc,Width,Height);
	CDC tdc;
	tdc.CreateCompatibleDC(&dc);
	CBitmap*pOld=tdc.SelectObject(&bm);
	tdc.BitBlt(0,0,Width,Height,&dc,pRect->left,pRect->top,SRCCOPY);
	tdc.SelectObject(pOld);
	BITMAP btm;
	bm.GetBitmap(&btm);
	DWORD size=btm.bmWidthBytes*btm.bmHeight;
	LPSTR lpData=(LPSTR)GlobalAllocPtr(GPTR,size);
	BITMAPINFOHEADER bih;
	bih.biBitCount=btm.bmBitsPixel;
	bih.biClrImportant=0;
	bih.biClrUsed=0;
	bih.biCompression=0;
	bih.biHeight=btm.bmHeight;
	bih.biPlanes=1;
	bih.biSize=sizeof(BITMAPINFOHEADER);
	bih.biSizeImage=size;
	bih.biWidth=btm.bmWidth;
	bih.biXPelsPerMeter=0;
	bih.biYPelsPerMeter=0;
	GetDIBits(dc,bm,0,bih.biHeight,lpData,(BITMAPINFO*)&bih,DIB_RGB_COLORS);
	//Compare with the stdbuf
	for(int i=0;i<DGLZ_MAX_CARDS;i++){
		if(0==memcmp((const void *)&stdbuf[i*BUF_SIZE], (const void *)lpData, BUF_SIZE)){
			*p_card = i+1;
			if(NULL!=lpData) GlobalFreePtr(lpData);
			if(NULL!=hDc) ReleaseDC(hMain, dc.Detach());
			return 0;
		}
	}
	if(NULL!=lpData) GlobalFreePtr(lpData);\
	if(NULL!=hDc) ReleaseDC(hMain, dc.Detach());
	return 1;

}
	
void SaveBmpbyRectName(LPCRECT pRect, CString* p_fullname)
{
	CDC dc;
//	dc.CreateDC("DISPLAY",NULL,NULL,NULL);
	CBitmap bm;
	HDC hDc;

	HWND hMain = ::FindWindow(NULL, "大怪路子");
	if (hMain){
		RECT r;
		::GetWindowRect(hMain, &r);
		g_p_dglz->vset.win_left = r.left;
		g_p_dglz->vset.win_top = r.top;
		hDc = ::GetDC(hMain);
		dc.Attach(hDc);
	}else {
//		::MessageBox(NULL, "Dglz Not found", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	int Width=pRect->right - pRect->left;
	int Height=pRect->bottom - pRect->top;
	//生成画布
	bm.CreateCompatibleBitmap(&dc,Width,Height);
	CDC tdc;
	tdc.CreateCompatibleDC(&dc);
	CBitmap*pOld=tdc.SelectObject(&bm);
	tdc.BitBlt(0,0,Width,Height,&dc,pRect->left,pRect->top,SRCCOPY);
	tdc.SelectObject(pOld);
	BITMAP btm;
	bm.GetBitmap(&btm);
	DWORD size=btm.bmWidthBytes*btm.bmHeight;
	LPSTR lpData=(LPSTR)GlobalAllocPtr(GPTR,size);
	BITMAPINFOHEADER bih;
	bih.biBitCount=btm.bmBitsPixel;
	bih.biClrImportant=0;
	bih.biClrUsed=0;
	bih.biCompression=0;
	bih.biHeight=btm.bmHeight;
	bih.biPlanes=1;
	bih.biSize=sizeof(BITMAPINFOHEADER);
	bih.biSizeImage=size;
	bih.biWidth=btm.bmWidth;
	bih.biXPelsPerMeter=0;
	bih.biYPelsPerMeter=0;
	GetDIBits(dc,bm,0,bih.biHeight,lpData,(BITMAPINFO*)&bih,DIB_RGB_COLORS);
	static int filecount=0;
	BITMAPFILEHEADER bfh;
	bfh.bfReserved1=bfh.bfReserved2=0;
	bfh.bfType=((WORD)('M'<< 8)|'B');
	bfh.bfSize=54+size;
	bfh.bfOffBits=54;
	CFile bf;
	if(bf.Open(*p_fullname,CFile::modeCreate|CFile::modeWrite)){
		bf.WriteHuge(&bfh,sizeof(BITMAPFILEHEADER));
		bf.WriteHuge(&bih,sizeof(BITMAPINFOHEADER));
		bf.WriteHuge(lpData,size);
		bf.Close();
 	}
	GlobalFreePtr(lpData);
	if(NULL!=hDc) {
		ReleaseDC(hMain, dc.Detach());
	}
	//	UpdateData(FALSE);
}

void myLClick(DWORD dx, DWORD dy)
{
	SetCursorPos(dx, dy);
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN, dx, dy, NULL, NULL);
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP, dx, dy, NULL, NULL);
}

void myRClick(DWORD dx, DWORD dy)
{
	SetCursorPos(dx, dy);
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTDOWN, dx, dy, NULL, NULL);
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTUP, dx, dy, NULL, NULL);
}

void myDoubleClick(DWORD dx, DWORD dy)
{
	SetCursorPos(dx, dy);
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN, dx, dy, NULL, NULL);
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP, dx, dy, NULL, NULL);
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN, dx, dy, NULL, NULL);
	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP, dx, dy, NULL, NULL);
}

/*0 is the same, 1 isn't the same*/
UINT32 FileCompare(CString* p_name1, CString* p_name2)
{
	CFile fd1;
	CFile fd2;	
	DWORD fd1_len;
	DWORD fd2_len;
	void far* p1;
	void far* p2;
	int rtn = 0;
	
	if(fd1.Open(*p_name1,CFile::modeRead)
		&&fd2.Open(*p_name2,CFile::modeRead)
	){
         	fd1_len = fd1.GetLength();
         	fd2_len = fd2.GetLength();
		if(fd1_len!=fd2_len){
			fd1.Close();
			fd2.Close();
			return 1;
		}else{	
			if((NULL!=(p1 = GlobalAlloc(0, fd1_len)))
				&&(NULL!=(p2 = GlobalAlloc(0, fd2_len)))
				){
				fd1.ReadHuge(p1, fd1_len);
				fd2.ReadHuge(p2, fd2_len);
				rtn = memcmp(p1, p2, fd1_len);
				GlobalFree(p1);
				GlobalFree(p2);
				fd1.Close();
				fd2.Close();
				if(0==rtn){
					return 0;
				}else {
					return 1;
				}
			}
			fd1.Close();
			fd2.Close();
			return 1;
		}
	}
	return 1;
}

//侦测目前屏幕分辨率
void GetCurrentVideoSettings(DEVMODE *pdevmode)
{
	HWND hwndDesktop=GetDesktopWindow();
	HDC hdc=GetDC(hwndDesktop);
	pdevmode -> dmSize =sizeof(DEVMODE);
	pdevmode -> dmBitsPerPel=GetDeviceCaps(hdc,BITSPIXEL);
	pdevmode -> dmPelsWidth=GetSystemMetrics(SM_CXSCREEN);
	pdevmode -> dmPelsHeight=GetSystemMetrics(SM_CYSCREEN);
	pdevmode -> dmFields=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	if (NULL!=hdc) ReleaseDC(hwndDesktop, hdc);
	/*
		memset(g_err_msg, 0x0, GMSG_MAX);
		sprintf(g_err_msg, "Cur vedio is width:%d height:%d bits:%d", pdevmode -> dmPelsWidth, pdevmode -> dmPelsHeight, pdevmode -> dmBitsPerPel);
		AfxMessageBox(g_err_msg);	
	*/
}

//Now mode just have 0 or 1, 0 autoset, 2 to get global saved old and recover
//return 0 means ok, others for fall
int  DglzVedioSet(DEVMODE *pdevmode, UINT32 mode)
{
	pdevmode->dmSize = sizeof(DEVMODE);
	pdevmode->dmFields = DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL;

	if(1==mode){
		pdevmode->dmPelsWidth = g_p_dglz->old_vedio.width;
		pdevmode->dmPelsHeight = g_p_dglz->old_vedio.height;
		pdevmode->dmBitsPerPel = g_p_dglz->old_vedio.bits;
	 	if (DISP_CHANGE_SUCCESSFUL!=ChangeDisplaySettings(pdevmode, 0)){
			return -1;
		}
		g_p_dglz->old_vedio.changed = 1;
	}else if(0==mode){
		if((pdevmode->dmPelsWidth>1280)&&(pdevmode->dmPelsHeight>=1024)){
			pdevmode->dmPelsWidth = 1280;
			pdevmode->dmPelsHeight = 1024;
			pdevmode->dmBitsPerPel = 32;
		 	if (DISP_CHANGE_SUCCESSFUL!=ChangeDisplaySettings(pdevmode, 0)){
				return -1;
			}
			g_p_dglz->vset.mode= 0;
		}else if((pdevmode->dmPelsWidth>1024)&&(pdevmode->dmPelsHeight>=768)){
			pdevmode->dmPelsWidth = 1024;
			pdevmode->dmPelsHeight = 768;
			pdevmode->dmBitsPerPel = 32;
		 	if (DISP_CHANGE_SUCCESSFUL!=ChangeDisplaySettings(pdevmode, 0)){
				return -1;
			}
			g_p_dglz->vset.mode= 1;
		}else{
			return -1;
		}
	}else{
		return -1;
	}

	Sleep(5000);
	return 0;
}

int DglzVedioInit(void)
{
	DEVMODE dev = {0};
	
/*	GetCurrentVideoSettings(&dev);
	g_p_dglz->old_vedio.width = dev.dmPelsWidth;
	g_p_dglz->old_vedio.height= dev.dmPelsHeight;
	g_p_dglz->old_vedio.bits= dev.dmBitsPerPel;		
	if((1280==dev.dmPelsWidth)&&(1024==dev.dmPelsHeight)&&(32==dev.dmBitsPerPel)){
		g_p_dglz->vset.mode= 0;
	}else if((1024==dev.dmPelsWidth)&&(768==dev.dmPelsHeight)&&(32==dev.dmBitsPerPel)){
		g_p_dglz->vset.mode = 1;
	}else {
		//Will auto change the vedio settings to the best one we supported
		if(0!=DglzVedioSet(&dev, 0)){
			return -1;	
		}
	}
*/	
	g_p_dglz->vset.dglz_l_t = DGlZ_OFF_L_T;
	g_p_dglz->vset.dglz_l_b = DGlZ_OFF_L_B;
	g_p_dglz->vset.dglz_w = DGLZ_OFF_WIDTH;
	g_p_dglz->vset.dglz_h = DGLZ_OFF_HEIGHT;
	g_p_dglz->vset.dglz_a_x= DGlZ_OFF_A_X;
	g_p_dglz->vset.dglz_b_x= DGlZ_OFF_B_X;
	g_p_dglz->vset.dglz_c_x= DGlZ_OFF_C_X;
	g_p_dglz->vset.dglz_a_y= DGlZ_OFF_A_Y;
	g_p_dglz->vset.dglz_b_y= DGlZ_OFF_B_Y;
	g_p_dglz->vset.dglz_c_y= DGlZ_OFF_C_Y;
	g_p_dglz->vset.dglz_d_y= DGlZ_OFF_D_Y;
	
	return 0;

}

//可以考虑一开始就把我自己的牌先减掉，这样可以不用考虑后来
//手牌的位置了，而且不需要看我自己出过什么牌
void  DglzGameInit(void)
{
	int i=0;
	int j=0;
	
//	m_Path = _T("D:\\husband\\Study\\VC_study\\dglz_record\\mcontest\\cards\\");	
	m_Path = _T("E:\\game\\dglz_rec\\");	
	memset(g_p_dglz, 0x0, sizeof(DGLZ_T));
	if (0!=DglzVedioInit()){
		memset(g_err_msg, 0x0, GMSG_MAX);
		sprintf(g_err_msg, "Now just support 1280x1024x32 and 1024x768x32 vedio settings, init failed.");
		AfxMessageBox(g_err_msg);	
		exit(1);
	}
	for(i=0;i<DGLZ_MAX_CARDS;i++) {
		g_p_dglz->all_cards[i] = DGLZ_CARDS_SUIT;
	}
	memset(&g_p_dglz->players[0], 0x0, sizeof(PLAYER_T)*DGLZ_PLAYERS);
	for(i=0;i<DGLZ_PLAYERS;i++){
		g_p_dglz->players[i].id = i;
		g_p_dglz->players[i].cursor = 0;
		g_p_dglz->players[i].ava = 1;
	}
	memset(g_p_dglz->mycards, 0x0, DGLZ_HAND_CARDS);
	memset(g_p_dglz->othercards, 0x0, DGLZ_MAX_CARDS);
	memset(g_err_msg, 0x0, GMSG_MAX);
	//Load match.dat to stdbuf
	LoadParseFile();
}

void DglzHandSave(void)
{
	CRect cards_rect;
	UINT32 i=0;
	CString name;

	m_Path = _T("E:\\game\\dglz_rec\\");
	memset(g_p_dglz, 0x0, sizeof(DGLZ_T));
	DglzVedioInit();

	for(i=0;i<DGLZ_HAND_CARDS;i++){
		name.Format("temp\\hand%d.bmp",i);
		name=m_Path+name;
		cards_rect.left = g_p_dglz->vset.dglz_l_t+i*(g_p_dglz->vset.dglz_w);
		cards_rect.right = g_p_dglz->vset.dglz_l_t+(i+1)*(g_p_dglz->vset.dglz_w);
		cards_rect.top = g_p_dglz->vset.dglz_l_b;
		cards_rect.bottom = g_p_dglz->vset.dglz_l_b+g_p_dglz->vset.dglz_h;
		SaveBmpbyRectName(&cards_rect, &name);
	}
}

//dglz_single_card_parse
UINT32 DglzCardParse(CString* pic_name, UINT32* p_card)
{
	CString std_name;
	UINT8 i=0;

	*p_card = 0;
	/*wan parse*/
	for(i=1;i<=DGLZ_MAX_CARDS;i++){
		std_name.Format("std_54\\%d.bmp", i);
		std_name=m_Path+std_name;
		if(0==FileCompare(pic_name, &std_name)){
			*p_card = i;
			return 0;
		}
	}
	return 1;
}

#define MHXX_ROLE_XSTART 386
#define MHXX_ROLE_YSTART 123
#define MHXX_ROLE_WIDTH 250
#define MHXX_BAG_XSTART 346
#define MHXX_BAG_YSTART 715
#define MHXX_SLEEP_TIME 15000

void MHXX_Mining_ChangeRole(unsigned int role)
{
    DWORD X = MHXX_ROLE_XSTART + MHXX_ROLE_WIDTH * role;
    DWORD Y = MHXX_ROLE_YSTART;

	MyLClick(X, Y);
	Sleep(20000);
}

void MHXX_Mining_ArrangeBag(void)
{
	MyLClick(MHXX_BAG_XSTART, MHXX_BAG_YSTART);
	Sleep(20000);
}

//www.91wan.com/mhxx
void MHXX_Mining()
{
	int i=0;
	for (i=0;i<ROLE_MAX;i++) {
	    //Change to role i
        MHXX_Mining_ChangeRole(i);
		//ArrangeBag
        MHXX_Mining_ArrangeBag();
	}
    sleep(MHXX_SLEEP_TIME);
}

void DglzHandParse(void)
{
	UINT8 i=0;
	CRect cards_rect;

	DglzGameInit();
	for(i=0;i<DGLZ_HAND_CARDS;i++){
		cards_rect.left = g_p_dglz->vset.dglz_l_t+i*(g_p_dglz->vset.dglz_w);
		cards_rect.right = g_p_dglz->vset.dglz_l_t+(i+1)*(g_p_dglz->vset.dglz_w);
		cards_rect.top = g_p_dglz->vset.dglz_l_b;
		cards_rect.bottom = g_p_dglz->vset.dglz_l_b+g_p_dglz->vset.dglz_h;
		DglzRectCardParse(&cards_rect, &DGLZGHS[i]);
	}
	for(i=0;i<DGLZ_HAND_CARDS;i++){
		if(DGLZGHS[i]>=1){
			g_p_dglz->all_cards[DGLZGHS[i]-1]--;
		}
	}
	//++ for test
	#ifdef mydebug
	memset(g_err_msg, 0x0, GMSG_MAX);
	sprintf(g_err_msg, "My hs is %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d", DGLZGHS[0], DGLZGHS[1], DGLZGHS[2], DGLZGHS[3], DGLZGHS[4], DGLZGHS[5], DGLZGHS[6], DGLZGHS[7], DGLZGHS[8], DGLZGHS[9], DGLZGHS[10], DGLZGHS[11], DGLZGHS[12], DGLZGHS[13], DGLZGHS[14], DGLZGHS[15], DGLZGHS[16], DGLZGHS[17], DGLZGHS[18], DGLZGHS[19], DGLZGHS[20], DGLZGHS[21], DGLZGHS[22], DGLZGHS[23], DGLZGHS[24], DGLZGHS[25], DGLZGHS[26]);
	AfxMessageBox(g_err_msg);
	#endif
	//End of test
	g_p_dglz->state = DGLZ_ROUND_PLAYING;

}

/*Lance ++ for dglz at 2007.06.13
void CDglzrecDlg::OnHelperDglzCap(void)
{
	TILE_T act = 0;

 	m_Path = _T("D:\\husband\\Study\\VC_study\\dglz_record\\mcontest\\cards\\");	
	DglzVedioInit();
	DglzHandSave();
}
*/

//Judge whether 1 card in player id's rect
void DglzGetPosbyId(UINT32 id, UINT32* x, UINT32* y)
{
	switch(id){
		case 0:
			*x = g_p_dglz->vset.dglz_b_x;
			*y = g_p_dglz->vset.dglz_d_y;
			break;
		case 1:
			*x = g_p_dglz->vset.dglz_c_x;
			*y = g_p_dglz->vset.dglz_c_y;
			break;
		case 2:
			*x = g_p_dglz->vset.dglz_c_x;
			*y = g_p_dglz->vset.dglz_b_y;
			break;
		case 3:
			*x = g_p_dglz->vset.dglz_b_x;
			*y = g_p_dglz->vset.dglz_a_y;
			break;
		case 4:
			*x = g_p_dglz->vset.dglz_a_x;
			*y = g_p_dglz->vset.dglz_b_y;
			break;
		case 5:
			*x = g_p_dglz->vset.dglz_a_x;
			*y = g_p_dglz->vset.dglz_c_y;
			break;
		default:
			break;
	}
}

void DglzPlayerSave(UINT32 id)
{
	UINT32 i=0;
	CString name;
	CRect rect;
	UINT32 left = 0;
	UINT32 top = 0;

	memset(&rect, 0x0, sizeof(CRect));
	DglzGetPosbyId(id, &left, &top);
	for(i=0;i<DGLZ_MINROUND;i++){
		name.Format("temp\\id%d_%d.bmp", id, i);
		name=m_Path+name;
		rect.left = left+i*g_p_dglz->vset.dglz_w;
		rect.right = left+(i+1)*g_p_dglz->vset.dglz_w;
		rect.top = top;
		rect.bottom = top+g_p_dglz->vset.dglz_h;
		SaveBmpbyRectName(&rect, &name);
	}
}

//To judge whether new cards should be record, will work together with the matched judgement
//To be called when sth found but not sure whether the last
void Dglz_judge_refreshed(UINT32 id, UINT32 matched)
{
	UINT32 cur = DGLZ_PLAYERCUR(id);
	UINT32 i = 0;
	UINT32 j = 0;
	UINT32 samed = 0;

	if(cur>0){
		//至少是第二次进来，统计有多少个与上一次分析的一样
		for(i=0;i<DGLZ_MINROUND;i++){
			for(j=0;j<DGLZ_MINROUND;j++){
				if((0!=(DGLZ_PLAYER(id, cur, i)))&&((DGLZ_PLAYER(id, cur, i))==(DGLZ_PLAYER(id, cur-1, j)))){
					samed++;
				}
			}
		}
		if (1!=DGLZ_PLAYERAVA(id)){
			if(samed>0){
				DGLZ_PLAYERAVA(id) = 0;
				for(i=0;i<DGLZ_MINROUND;i++){
					DGLZ_PLAYER(id, cur, i) = 0;
				}
			}else{
				DGLZ_PLAYERAVA(id) = 1;
			}
		}else{
			//如果上一次说可以拍了，如果有两张以上重复，则认为是遮挡原因
			//20080913通过测试发现，只要有一张一样，多半就是遮挡问题
			//if(samed>1){
			if(samed>0){
				DGLZ_PLAYERAVA(id) = 0;
				for(i=0;i<DGLZ_MINROUND;i++){
					DGLZ_PLAYER(id, cur, i) = 0;
				}
			}else{
				DGLZ_PLAYERAVA(id) = 1;
			}
		}
		
	}
}

UINT8 Dglz_last_samed(void)
{
	UINT8 ret_val = 0;
	UINT8 id = 0;
	UINT8 i = 0;
	UINT8 j = 0;
	UINT32 cur = 0;
	UINT8 samed = 0;

	for(id=0;id<DGLZ_PLAYERS;id++){
		samed = 0;
		cur = DGLZ_PLAYERCUR(id);
		if(cur>0) {
			for(i=0;i<DGLZ_MINROUND;i++){
				for(j=0;j<DGLZ_MINROUND;j++){
					if((0!=(DGLZ_PLAYER(id, cur, i)))&&((DGLZ_PLAYER(id, cur, i))==(DGLZ_PLAYER(id, cur-1, j)))){
						samed++;
					}
				}
			}
			if (samed>0) ret_val++;
		}	
	}

	return ret_val;
}

void DglzPlayerRoundDetect(UINT32 id, UINT32* pmatch)
{
	int i=0;
	CString name;
	CRect rect;
	UINT32 left = 0;
	UINT32 top = 0;
	UINT32 cur = DGLZ_PLAYERCUR(id);

	if(cur>=DGLZ_HAND_CARDS) return ;
	*pmatch = 0;
	memset(&rect, 0x0, sizeof(CRect));
	DglzGetPosbyId(id, &left, &top);
	switch (id) {
		case 0:
		case 3:
		case 4:
		case 5:
			for(i=0;i<DGLZ_MINROUND;i++){
				rect.left = left+i*g_p_dglz->vset.dglz_w;
				rect.right = left+(i+1)*g_p_dglz->vset.dglz_w;
				rect.top = top;
				rect.bottom = top+g_p_dglz->vset.dglz_h;
				if(0==DglzRectCardParse(&rect, &DGLZ_PLAYER(id, cur, i))){
					(*pmatch)++;
				}else{
					return ;
				}
			}
			break;
		case 1:
		case 2:
			for(i=DGLZ_MINROUND-1;i>=0;i--){
				rect.left = left+i*g_p_dglz->vset.dglz_w;
				rect.right = left+(i+1)*g_p_dglz->vset.dglz_w;
				rect.top = top;
				rect.bottom = top+g_p_dglz->vset.dglz_h;
				if(0==DglzRectCardParse(&rect, &DGLZ_PLAYER(id, cur, i))){
					(*pmatch)++;
				}else{
					return ;
				}
			}
			break;
		default:
			break;
	}

}

//To parse player id's rect cards_rect, and judge 1, 2, 3, 5 four branch
//如果从效率角度出发，应该先只拍一张，对有无新货进行判断
int DglzPlayerRoundParse(UINT32 id)
{
	//maybe some fixed offset
	CString name1;
	CString to_del;
	UINT8 i=0;
	UINT8 rtn = 0;
	UINT32 cur = DGLZ_PLAYERCUR(id);
	UINT32 matched = 0;
	UINT32 tmp = 0;
	UINT8 d_rtn = 0;

	for(i=0;i<DGLZ_MINROUND;i++){
		to_del.Format("temp\\id%d_%d.bmp", id, i);
		to_del=m_Path+to_del;
		d_rtn = DeleteFile(to_del);
	}
	DglzPlayerRoundDetect(id, &matched);
	if(0==matched){
		DGLZ_PLAYERAVA(id) = 1;
	}else {
		Dglz_judge_refreshed(id, matched);
		if(1==DGLZ_PLAYERAVA(id)){
			if(0!=id){
				//My cards was -- at handparse.
				for(i=0;i<DGLZ_MINROUND;i++){
					tmp = DGLZ_PLAYER(id, cur, i);
					if(tmp>=1) {
						g_p_dglz->all_cards[tmp-1]--;
					}
				}
			}
			//++ for test
			#ifdef mydebug
			memset(g_err_msg, 0x0, GMSG_MAX);
			sprintf(g_err_msg, "ID %d is: %2d %2d %2d %2d %2d", id, DGLZ_PLAYER(id, cur, 0), DGLZ_PLAYER(id, cur, 1), DGLZ_PLAYER(id, cur, 2), DGLZ_PLAYER(id, cur, 3), DGLZ_PLAYER(id, cur, 4));
			AfxMessageBox(g_err_msg);
			#endif
			//End of test
			DGLZ_PLAYERCUR(id)++;
			DGLZ_PLAYERAVA(id) = 0;
		}
	}
	
	return rtn;
}

void DglzRPlayingHandle(void)
{
	UINT32 id=0;

	if(1!=paused){	
		for(id=0;id<DGLZ_PLAYERS;id++){
			DglzPlayerRoundParse(id);
			Sleep(DGLZ_PLAYER_CAP_DELAY);
		}
		dlg.ShowAll();
	}
}
//End of dglz

//return offset: how many char was copied, if return 0, sth error
UINT32 show_num(UINT32 card, char* show)
{
	UINT32 num = ((card-1)/4)+1;

	switch(num){
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			_itoa(num, show, 10);
			return 1;
		case 10:	
			_itoa(num, show, 10);
			return 2;
		case 11:
			*show = 'J';
			return 1;
		case 12:
			*show = 'Q';
			return 1;
		case 13:
			*show = 'K';
			return 1;
		case 1:
			*show = 'A';
			return 1;
		case 14:	
			if(card==54){
				*show = 'B';
			}else if(card==53){
				*show = 'S';
			}
			return 1;
		default:
//			_itoa(card, show, 10);
			return 0;
	}
}

//from 0 to cur, format the msg buffer passed
void Dglz_show_history(UINT32 id)
{
	UINT32 cur = DGLZ_PLAYERCUR(id);
	UINT32 i=0;
	UINT32 j=0;
	UINT32 k=0;
	UINT32 tmp = 0;
	UINT32 offset = 0;
	UINT32 passed = 0;
	char * pmsg = &g_player_msg[id][0];

	memset(pmsg, 0x0, GMSG_MAX);
	for(i=0;i<cur;i++){
		for(k=0;k<DGLZ_MINROUND;k++){
			tmp = DGLZ_PLAYER(id, i, k);
			offset = show_num(tmp, pmsg+j);
			if(0!=offset){
				passed++;
			}
			j+=offset;
			if(2==offset){
				pmsg[j] = ' ';
				j++;
			}else if(1==offset){
				pmsg[j] = ' ';
				pmsg[j+1] = ' ';
				j+=2;
			}
		}
		pmsg[j] = '\r';
		pmsg[j+1] = '\n';
		j+=2;
	}
	strcpy(pmsg+j, "The rest is ");
	j+=12;
	if(passed<=DGLZ_HAND_CARDS){
		_itoa(DGLZ_HAND_CARDS-passed, pmsg+j, 10);
		j+=2;
		pmsg[j] = '\n';
	}
//	AfxMessageBox(msg);
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDglzrecDlg dialog

CDglzrecDlg::CDglzrecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDglzrecDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDglzrecDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDglzrecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDglzrecDlg)
	DDX_Control(pDX, IDC_PIC29, m_pic29);
	DDX_Control(pDX, IDC_PIC28, m_pic28);
	DDX_Control(pDX, IDC_PIC27, m_pic27);
	DDX_Control(pDX, IDC_PIC26, m_pic26);
	DDX_Control(pDX, IDC_PIC25, m_pic25);
	DDX_Control(pDX, IDC_PIC24, m_pic24);
	DDX_Control(pDX, IDC_PIC23, m_pic23);
	DDX_Control(pDX, IDC_PIC22, m_pic22);
	DDX_Control(pDX, IDC_PIC21, m_pic21);
	DDX_Control(pDX, IDC_PIC20, m_pic20);
	DDX_Control(pDX, IDC_PIC19, m_pic19);
	DDX_Control(pDX, IDC_PIC18, m_pic18);
	DDX_Control(pDX, IDC_PIC17, m_pic17);
	DDX_Control(pDX, IDC_PIC16, m_pic16);
	DDX_Control(pDX, IDC_PIC15, m_pic15);
	DDX_Control(pDX, IDC_PIC14, m_pic14);
	DDX_Control(pDX, IDC_PIC13, m_pic13);
	DDX_Control(pDX, IDC_PIC12, m_pic12);
	DDX_Control(pDX, IDC_PIC11, m_pic11);
	DDX_Control(pDX, IDC_PIC10, m_pic10);
	DDX_Control(pDX, IDC_PIC9, m_pic9);
	DDX_Control(pDX, IDC_PIC8, m_pic8);
	DDX_Control(pDX, IDC_PIC7, m_pic7);
	DDX_Control(pDX, IDC_PIC6, m_pic6);
	DDX_Control(pDX, IDC_PIC5, m_pic5);
	DDX_Control(pDX, IDC_PIC4, m_pic4);
	DDX_Control(pDX, IDC_PIC3, m_pic3);
	DDX_Control(pDX, IDC_PIC2, m_pic2);
	DDX_Control(pDX, IDC_PIC1, m_pic1);
	DDX_Control(pDX, IDC_PIC0, m_pic0);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDglzrecDlg, CDialog)
	//{{AFX_MSG_MAP(CDglzrecDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_RESTART, OnRestart)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_ESCAPE, OnEscape)
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
	ON_BN_CLICKED(IDC_PK, OnPk)
	ON_BN_CLICKED(IDC_CAP, OnCap)
	ON_BN_CLICKED(IDC_CreateParseFile, OnCreateParseFile)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_CAPJG, OnCapjg)
	ON_BN_CLICKED(IDC_CAPJS, OnCapjs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDglzrecDlg message handlers

void CDglzrecDlg::pic_locate()
{
	p_pk[0] = &m_pic0;
	p_pk[1] = &m_pic1;
	p_pk[2] = &m_pic2;
	p_pk[3] = &m_pic3;
	p_pk[4] = &m_pic4;
	p_pk[5] = &m_pic5;
	p_pk[6] = &m_pic6;
	p_pk[7] = &m_pic7;
	p_pk[8] = &m_pic8;
	p_pk[9] = &m_pic9;
	p_pk[10] = &m_pic10;
	p_pk[11] = &m_pic11;
	p_pk[12] = &m_pic12;
	p_pk[13] = &m_pic13;
	p_pk[14] = &m_pic14;
	p_pk[15] = &m_pic15;
	p_pk[16] = &m_pic16;
	p_pk[17] = &m_pic17;
	p_pk[18] = &m_pic18;
	p_pk[19] = &m_pic19;
	p_pk[20] = &m_pic20;
	p_pk[21] = &m_pic21;
	p_pk[22] = &m_pic22;
	p_pk[23] = &m_pic23;
	p_pk[24] = &m_pic24;
	p_pk[25] = &m_pic25;
	p_pk[26] = &m_pic26;
	p_pk[27] = &m_pic27;
	p_pk[28] = &m_pic28;
	p_pk[29] = &m_pic29;

}

BOOL CDglzrecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	g_p_dglz->state = DGLZ_GAME_STARTED;
	pic_locate();
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDglzrecDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CDglzrecDlg::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDglzrecDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDglzrecDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDglzrecDlg::ShowAll()
{
	UINT32 i=0;

	for(i=0;i<DGLZ_PLAYERS;i++){
		Dglz_show_history(i);
	}
	GetDlgItem(IDC_EDIT0)->SetWindowText(&g_player_msg[0][0]);   
	GetDlgItem(IDC_EDIT1)->SetWindowText(&g_player_msg[1][0]);   
	GetDlgItem(IDC_EDIT2)->SetWindowText(&g_player_msg[2][0]);   
	GetDlgItem(IDC_EDIT3)->SetWindowText(&g_player_msg[3][0]);   
	GetDlgItem(IDC_EDIT4)->SetWindowText(&g_player_msg[4][0]);   
	GetDlgItem(IDC_EDIT5)->SetWindowText(&g_player_msg[5][0]);   
	DglzRestShow();	
	GetDlgItem(IDC_EDITREST)->SetWindowText(g_rest_msg);   

}

void CDglzrecDlg::OnOK() 
{
	// TODO: Add extra validation here

	CDialog::OnOK();
}

void CDglzrecDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	threadFlag = 0;
/*
	DEVMODE dev = {0};

	if(1==g_p_dglz->old_vedio.changed){
		GetCurrentVideoSettings(&dev);
		DglzVedioSet(&dev, 1);
	}
*/	
	CDialog::OnCancel();
}

void CDglzrecDlg::OnRestart() 
{
	// TODO: Add your control notification handler code here
	if(1==threadFlag){
		g_p_dglz->state = DGLZ_GAME_STARTED;
		RestClear();
	}else {
		threadFlag = 1;
		HWND hWnd = GetSafeHwnd();
		AfxBeginThread(Dgzs, hWnd, THREAD_PRIORITY_NORMAL);
		this->SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
}

void DglzEscape(void)
{
	HWND hMain = ::FindWindow(NULL, "大怪路子");
	if (hMain){
		POINT pos;
		pos.x = 0;
		pos.y = 0;
		ClientToScreen(hMain, &pos);
		g_p_dglz->vset.client_left = pos.x;
		g_p_dglz->vset.client_top = pos.y;
	}else {
//		::MessageBox(NULL, "Dglz not found", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	DWORD dx = g_p_dglz->vset.client_left;
	DWORD dy = g_p_dglz->vset.client_top;
	myLClick(dx+8, dy+500);
	Sleep(300);
	myDoubleClick(dx+DGLZ_ESC_X, dy+DGLZ_ESC_Y);
	Sleep(200);
	myDoubleClick(dx+DGLZ_ESC_X, dy+DGLZ_ESC_Y);
}

/*Now use to show the cards remained
*/
void DglzRestShow(void)
{
	UINT32 bj = g_p_dglz->all_cards[53];
	UINT32 lj = g_p_dglz->all_cards[52];
//	UINT32 a, 2, 3, 4, 5, 6, 7, 8, 9, 10, j, q, k
	UINT32 a[13] = {0};
	UINT32 i=0;

	for(i=0;i<13;i++){
		a[i] = g_p_dglz->all_cards[4*i]+g_p_dglz->all_cards[4*i+1]+g_p_dglz->all_cards[4*i+2]+g_p_dglz->all_cards[4*i+3];
	}
	memset(g_rest_msg, 0x0, GMSG_MAX);
	sprintf(g_rest_msg, "大:%d \r\n小:%d \r\n2   :%d \r\n3   :%d \r\n4   :%d \r\n5   :%d \r\n\r\nA   :%d \r\nK   :%d \r\nQ   :%d \r\nJ   :%d \r\n\r\n10  :%d \r\n9   :%d \r\n8   :%d \r\n7   :%d \r\n6   :%d \r\n", 	bj, lj, a[1], a[2], a[3], a[4], a[0], a[12], a[11], a[10], a[9], a[8], a[7], a[6], a[5]);
	//AfxMessageBox(g_rest_msg);
	//End of test

}

void CDglzrecDlg::OnStart() 
{
	// TODO: Add your control notification handler code here
	if(0==threadFlag){
		threadFlag = 1;
		HWND hWnd = GetSafeHwnd();
		AfxBeginThread(Dgzs, hWnd, THREAD_PRIORITY_NORMAL);
		this->SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
}

void CDglzrecDlg::OnStop() 
{
	// TODO: Add your control notification handler code here
	if(1==threadFlag){
		threadFlag = 0;
		//20080913: Cancel topMost onStop
		this->SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
}

void CDglzrecDlg::OnEscape() 
{
	// TODO: Add your control notification handler code here
	DglzEscape();
}

void CDglzrecDlg::OnPause() 
{
	// TODO: Add your control notification handler code here
	CWnd* pWnd= this->GetDlgItem(IDC_PAUSE);  
	
	if(0==paused) {
		paused = 1;
		pWnd->SetWindowText(_T("继续记牌(&P)"));
	} else if(1==paused){
		paused = 0;
		pWnd->SetWindowText(_T("暂停记牌(&P)"));
	}
	
}

//To show Card in a picture component
void CDglzrecDlg::ShowCard(UINT32 card, UINT32 loc)
{
	CBitmap cb;

	UINT32 pic_id = IDB_BITMAP1+card-1;

 	cb.LoadBitmap(pic_id);
	p_pk[loc]->SetBitmap(cb);
}

void CDglzrecDlg::RestClear()
{
	UINT32 i=0;
/*
	for(i=0;i<PK_MAX;i++){
		g_p_dglz->all_cards[i] = 1;
	}
	for(i=0;i<PK_MAX;i++){
		if (g_p_dglz->all_cards[i]>0){
			ShowCard(i+1, i);
		}
	}
	Sleep(1000);
*/	
	
	for(i=0;i<PK_MAX;i++){
		p_pk[i]->SetBitmap(0);
	}
}

void CDglzrecDlg::OnPk() 
{
	// TODO: Add your control notification handler code here
	CBitmap cb;
	unsigned int cards [DGLZ_MAX_CARDS] = {0};
	UINT32 i = 0;
	UINT32 j = 0;
	
	for(j=0;j<DGLZ_MAX_CARDS;j++){
		cards[j] = g_p_dglz->all_cards[j];
	}
	RestClear();
	for(i=0, j=0;(i<PK_MAX)&&(j<DGLZ_MAX_CARDS);){
		if ((cards[j]>0)&&(cards[j]<=DGLZ_CARDS_SUIT)){
			ShowCard(j+1, i);
			cards[j]--;
			i++;
		}else{
			j++;
		}
	}
}

void CDglzrecDlg::OnCap() 
{
	// TODO: Add your control notification handler code here
	RECT rect;
	CString name;
	rect.left = DGlZ_OFF_L_T;
	rect.top = DGlZ_OFF_L_B;
	rect.right = DGlZ_OFF_L_T+DGLZ_OFF_WIDTH*DGLZ_HAND_CARDS;
	rect.bottom = DGlZ_OFF_L_B+DGLZ_OFF_HEIGHT;

	srand((unsigned)time(NULL));
	
	name.Format("E:\\game\\dglz_rec\\pic\\%d.bmp", rand());
	SaveBmpbyRectName(&rect, &name);
//	DglzHandSave();
/*
	for(i=0;i<DGLZ_HAND_CARDS;i++){
		name.Format("temp\\hand%d.bmp",i);
		name=m_Path+name;
		cards_rect.left = g_p_dglz->vset.dglz_l_t+i*(g_p_dglz->vset.dglz_w);
		cards_rect.right = g_p_dglz->vset.dglz_l_t+(i+1)*(g_p_dglz->vset.dglz_w);
		cards_rect.top = g_p_dglz->vset.dglz_l_b;
		cards_rect.bottom = g_p_dglz->vset.dglz_l_b+g_p_dglz->vset.dglz_h;
		SaveBmpbyRectName(&cards_rect, &name);
	}
*/
}

void CDglzrecDlg::OnClearpk() 
{
	// TODO: Add your control notification handler code here
	RestClear();
}

void CDglzrecDlg::OnCreateParseFile() 
{
	// TODO: Add your control notification handler code here
	//CreateParseFile();
	LoadParseFile();
}



void CDglzrecDlg::OnTest() 
{
	// TODO: Add your control notification handler code here
	RECT rect;
	CString name;
	rect.left = 0;
	rect.top = 0;
	rect.right = 851;
	rect.bottom = 624;

	srand((unsigned)time(NULL));
	
	name.Format("E:\\game\\dglz_rec\\pic\\%d.bmp", rand());
	SaveBmpbyRectName(&rect, &name);
	
}

void CDglzrecDlg::OnCapjg() 
{
	// TODO: Add your control notification handler code here
	CRect cards_rect;
	CString name;

	name.Format("temp\\jg.bmp");
	name=m_Path+name;
	cards_rect.left = DGlZ_JG_X;
	cards_rect.right = DGlZ_JG_X+DGlZ_JG_W;
	cards_rect.top = DGlZ_JG_Y;
	cards_rect.bottom = DGlZ_JG_Y+DGlZ_JG_H;
	SaveBmpbyRectName(&cards_rect, &name);
}

void CDglzrecDlg::OnCapjs() 
{
	// TODO: Add your control notification handler code here
	CRect cards_rect;
	CString name;

	name.Format("temp\\js.bmp");
	name=m_Path+name;
	cards_rect.left = DGLZ_JS_X;
	cards_rect.right = DGLZ_JS_X+DGLZ_JS_W;
	cards_rect.top = DGLZ_JS_Y;
	cards_rect.bottom = DGLZ_JS_Y+DGLZ_JS_H;
	SaveBmpbyRectName(&cards_rect, &name);
	
}
