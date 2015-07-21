// dglzrecDlg.h : header file
//

#if !defined(AFX_DGLZRECDLG_H__731C907D_FCF0_4CC6_9408_A4F1084B08C0__INCLUDED_)
#define AFX_DGLZRECDLG_H__731C907D_FCF0_4CC6_9408_A4F1084B08C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DGLZ_ROUND_CAP_DELAY	40
#define DGLZ_PLAYER_CAP_DELAY	10

//For dglz of shanghai games center
#define DGLZ_HAND_CARDS				27
#define DGLZ_MAX_CARDS					54
#define DGLZ_CARDS_SUIT				3	
#define DGLZ_PLAYERS					6
#define DGLZ_MINROUND					5		
//++ for 1280x767x32 resolution
#define DGLZ_1280x1024_L_T				97
#define DGLZ_1280x1024_L_B				627
#define DGLZ_1280x1024_WIDTH			12
#define DGLZ_1280x1024_HEIGHT			28
#define DGLZ_1280x1024_A_X				89
#define DGLZ_1280x1024_B_X				229
#define DGLZ_1280x1024_C_X				361
#define DGLZ_1280x1024_A_Y				267
#define DGLZ_1280x1024_B_Y				317
#define DGLZ_1280x1024_C_Y				427
#define DGLZ_1280x1024_D_Y				477
//++ for 1024x768x32 resolution
#define DGLZ_1024x768_L_T				97
#define DGLZ_1024x768_L_B				499
#define DGLZ_1024x768_WIDTH			12
#define DGLZ_1024x768_HEIGHT			28
#define DGLZ_1024x768_A_X				89
#define DGLZ_1024x768_B_X				229
#define DGLZ_1024x768_C_X				361
#define DGLZ_1024x768_A_Y				139
#define DGLZ_1024x768_B_Y				189
#define DGLZ_1024x768_C_Y				299
#define DGLZ_1024x768_D_Y				349
//++ for offset from dglz_window
//101, 444 -3, -39
/*20081222 old bak
#define DGlZ_OFF_L_T					98
#define DGlZ_OFF_L_B					405
#define DGLZ_OFF_WIDTH					12
#define DGLZ_OFF_HEIGHT				28
#define DGlZ_OFF_A_X					90 
#define DGlZ_OFF_B_X					230
#define DGlZ_OFF_C_X					362
#define DGlZ_OFF_A_Y					45
#define DGlZ_OFF_B_Y					95
#define DGlZ_OFF_C_Y					205
#define DGlZ_OFF_D_Y					255

#define DGLZ_ESC_X						282
#define DGLZ_ESC_Y						488

#define DGlZ_JG_X						66
#define DGlZ_JG_Y						63
#define DGlZ_JG_W						47
#define DGlZ_JG_H						11

//188,123  15,11
#define DGLZ_JS_X						188
#define DGLZ_JS_Y						123
#define DGLZ_JS_W						15
#define DGLZ_JS_H						11
*/

//20081222
#define DGlZ_OFF_L_T					97
#define DGlZ_OFF_L_B					402
#define DGLZ_OFF_WIDTH					12
#define DGLZ_OFF_HEIGHT				28
#define DGlZ_OFF_A_X					89 
#define DGlZ_OFF_B_X					229
#define DGlZ_OFF_C_X					361
#define DGlZ_OFF_A_Y					42
#define DGlZ_OFF_B_Y					92
#define DGlZ_OFF_C_Y					202
#define DGlZ_OFF_D_Y					252

#define DGLZ_ESC_X						282
#define DGLZ_ESC_Y						488

#define DGlZ_JG_X						66
#define DGlZ_JG_Y						63
#define DGlZ_JG_W						47
#define DGlZ_JG_H						11

//188,123  15,11
#define DGLZ_JS_X						188
#define DGLZ_JS_Y						123
#define DGLZ_JS_W						15
#define DGLZ_JS_H						11


//End of 1024x768x32
#define DGLZGHS 							(g_p_dglz->mycards)
#define DGLZ_PLAYERCUR(id)				(g_p_dglz->players[(id)].cursor)
#define DGLZ_PLAYERAVA(id)				(g_p_dglz->players[(id)].ava)
#define DGLZ_PLAYER(id, cur, i)			(g_p_dglz->players[(id)].cards[(cur)][(i)])
#define GMSG_MAX						800
#define PK_MAX							30
//1344 = 12x28x4
#define BUF_SIZE							1344				
//End of dglz

#ifndef mydebug
//#define mydebug
#endif
//End of dglz

typedef unsigned char UINT8;
typedef unsigned int UINT32;
typedef UINT8 TILE_T;

enum {
	DGLZ_GAME_STARTED = 0,
	DGLZ_ROUND_PLAYING,
};
typedef unsigned char DGLZ_STATE_T;


//Frome 1-54 is the spade A to Big joker
typedef struct player {
	unsigned int id;												//From 0 to 5
	unsigned int cards[DGLZ_HAND_CARDS][DGLZ_MINROUND];   		//The hand show of the player
	unsigned int cursor;											//The current cursor of the player's cards have shown
	unsigned int ava;										//if ==1, means record available, record->0, detect null and set 1
}PLAYER_T;

typedef struct dglz_vedio{
	UINT32 mode;			//now 0: 1280x1024x32, 1:1024x768x32
	UINT32 win_left;
	UINT32 win_top;
	UINT32 client_left;
	UINT32 client_top;
	UINT32 dglz_l_t;
	UINT32 dglz_l_b;
	UINT32 dglz_w;
	UINT32 dglz_h;
	UINT32 dglz_a_x;
	UINT32 dglz_b_x;
	UINT32 dglz_c_x;
	UINT32 dglz_a_y;
	UINT32 dglz_b_y;
	UINT32 dglz_c_y;
	UINT32 dglz_d_y;
}DGLZ_VEDIO_T;

typedef struct old_vedio{
	UINT32 changed;			//to indicate whether resolution was auto changed
	UINT32 width;
	UINT32 height;
	UINT32 bits;
}OLD_VEDIO_T;

typedef struct dglz {
	DGLZ_STATE_T state;
	DGLZ_VEDIO_T vset;
	OLD_VEDIO_T old_vedio;
	unsigned int all_cards [DGLZ_MAX_CARDS];						//All cards remained
	PLAYER_T players[DGLZ_PLAYERS];								//6 palyer's information
	unsigned int mycards[DGLZ_HAND_CARDS];						//for use to record my cards in hand
	unsigned int othercards[DGLZ_MAX_CARDS];						//To show other cards except mine
}DGLZ_T;

void SaveBmpbyRectName(LPCRECT pRect, CString* p_fullname);
unsigned int FileCompare(CString* p_name1, CString* p_name2);
void DglzHandParse(void);
void DglzRPlayingHandle(void);
void GetCurrentVideoSettings(DEVMODE *pdevmode);
int  DglzVedioSet(DEVMODE *pdevmode, UINT32 mode);
void DglzRestShow(void);

/////////////////////////////////////////////////////////////////////////////
// CDglzrecDlg dialog

class CDglzrecDlg : public CDialog
{
// Construction
public:
	CDglzrecDlg(CWnd* pParent = NULL);	// standard constructor
	void ShowAll();

// Dialog Data
	//{{AFX_DATA(CDglzrecDlg)
	enum { IDD = IDD_DGLZREC_DIALOG };
	CStatic	m_pic29;
	CStatic	m_pic28;
	CStatic	m_pic27;
	CStatic	m_pic26;
	CStatic	m_pic25;
	CStatic	m_pic24;
	CStatic	m_pic23;
	CStatic	m_pic22;
	CStatic	m_pic21;
	CStatic	m_pic20;
	CStatic	m_pic19;
	CStatic	m_pic18;
	CStatic	m_pic17;
	CStatic	m_pic16;
	CStatic	m_pic15;
	CStatic	m_pic14;
	CStatic	m_pic13;
	CStatic	m_pic12;
	CStatic	m_pic11;
	CStatic	m_pic10;
	CStatic	m_pic9;
	CStatic	m_pic8;
	CStatic	m_pic7;
	CStatic	m_pic6;
	CStatic	m_pic5;
	CStatic	m_pic4;
	CStatic	m_pic3;
	CStatic	m_pic2;
	CStatic	m_pic1;
	CStatic	m_pic0;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDglzrecDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDglzrecDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRestart();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnEscape();
	afx_msg void OnPause();
	afx_msg void OnPk();
	afx_msg void pic_locate();
	afx_msg void ShowCard(UINT32 card, UINT32 loc);
	afx_msg void OnCap();
	afx_msg void RestClear();
	afx_msg void OnClearpk();
	afx_msg void OnCreateParseFile();
	afx_msg void OnTest();
	afx_msg void OnCapjg();
	afx_msg void OnCapjs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//CDglzrecDlg* pdlg;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DGLZRECDLG_H__731C907D_FCF0_4CC6_9408_A4F1084B08C0__INCLUDED_)
