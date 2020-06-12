//////////////////////////////////////////////////////////////////////
// defines.h
//  各種定数とウィンドウID/イベントIDなどの定義を行う
//////////////////////////////////////////////////////////////////////

#ifndef DEFINES_H_INC
#define DEFINES_H_INC

// 言語設定 //
#define JP // Commenting out this line for English //

// メインウィンドウサイズ //
#define MAINWIN_WID 680
#define MAINWIN_HGT 620

// デバイスリストサイズ //
#define DEVLIST_WID 460
#define DEVLIST_HGT 330

// サブウィンドウサイズ //
#define LADLG_WID 600
#define LADLG_HGT 400

// デバイスリストの項目数 //
#define DEV_NCOL 4

// 最大デバイス数 (ライブラリの制限=64、変更禁止) //
#define MAX_LANIO 64

// ファイル名文字数 //
#define MAX_PATH_NAME 192
#define MAX_FILE_NAME 256

// LANIOモデル数 (基本的に変更禁止) //
#define MAX_MODEL 11

// DO及びDIの最大数 (基本的に変更禁止) //
#define MAX_DODI 7
#define MAX_DO 5

////////////////// IDs //////////////////
#define ID_BTN_SEARCH 101
#define ID_BTN_IPADD 102
#define ID_BTN_CNCT 103
#define ID_BTN_FOLDER 104
#define ID_BTN_CLOSE 105

#define ID_BTN_DLGIOLBL 110
#define ID_BTN_DLGIOLBL_END (ID_BTN_DLGIOLBL + MAX_DODI)
// note : ID 110-117 is reserved
#define ID_BTN_DOCTRL 120
#define ID_BTN_DOCTRL_END (ID_BTN_DOCTRL + MAX_DO)
// note : ID 120-125 is reserved
#define ID_BTN_DICTRL 130

#define ID_FORM_PORT 201
#define ID_FORM_IP 202

#define ID_FORM_DLGIO 210
#define ID_FORM_DLGIO_END (ID_FORM_DLGIO + MAX_DODI)
// note : ID 210-217 is reserved

#define ID_FORM_AUTODO 220
#define ID_FORM_AUTODO_END (ID_FORM_AUTODO_END + MAX_DO)
// note : ID 220-225 is reserved
#define ID_FORM_AUTODI 230

#define ID_CHK_DO 500
#define ID_CHK_DO_END (ID_CHK_DO + MAX_DO)
// note : ID 500-505 is reserved
#define ID_CHK_DI 510
#define ID_CHK_SAVE 520

#define ID_DEVLIST 2001

#define ID_LANIO_DLG 3000
// note : ID 3000-3064 is reserved
#define ID_LANIO_DLG_END (ID_LANIO_DLG + MAX_LANIO)

///////////// スレッド、サブウィンドウ関連 /////////////

// Chラベル文字数上限 //
#define IO_LBL_MAXLEN 16

#define SEARCHTHR_MODE_SER 0
#define SEARCHTHR_MODE_ADD 1

#define THREAD_ERR_SEARCH (-1001)
#define THREAD_ERR_ADD (-1002)

#define THREAD_REQ_SHOW 1
#define THREAD_REQ_BTNACTIVE 2
#define THREAD_REQ_UPDATE_DI 3
#define THREAD_REQ_UPDATE_DO 4
#define THREAD_REQ_SAVE_CSV  5
#define THREAD_REQ_BTNACTIVE_DLG 6

#define PANEL_ERR_NOMODEL (-1)

#endif
