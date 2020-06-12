#ifndef LAPC_H_INC
#define LAPC_H_INC

// wxWidgets GUI ライブラリ //
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/cmdline.h>
#include <wx/listctrl.h>
#include <wx/msgdlg.h>
#include <wx/valnum.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/regex.h>
#include <wx/thread.h>
#include <wx/dirdlg.h>

// c/c++ libs
#include <locale.h> // 日本語用
#include <sys/time.h>
#include <time.h>
#include <fstream>

//
#include "defines.h"
#include "LANIOPanel.h"
#include "LANIOList.h"

class SearchThread;
class LANIOThread;

// イベントマクロ //
wxDECLARE_EVENT(wxEVT_SEARCHTHR_END, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_LANIOTHR_END, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_LANIOTHR_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_LANIODLG_CLOSE, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_LANIODLG_BTN, wxCommandEvent);

wxDEFINE_EVENT(wxEVT_SEARCHTHR_END, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_LANIOTHR_END, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_LANIOTHR_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_LANIODLG_CLOSE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_LANIODLG_BTN, wxCommandEvent);

/////////////////////////////////////////////////////////////
// Class : MyApp
//         アプリケーションオブジェクト
/////////////////////////////////////////////////////////////
class MyApp : public wxApp
{
 public:
	MyApp();
	virtual ~MyApp();
    virtual bool OnInit();
};

/////////////////////////////////////////////////////////////
// Class : MyFrame
//         メインウィンドウ
/////////////////////////////////////////////////////////////

class MyFrame : public wxFrame
{
 public:
    MyFrame(const wxString& title);
	virtual ~MyFrame();
	
 protected:
	SearchThread *pSearch;
	wxCriticalSection CSpSearch; // for thread protection
	LANIOThread *pLANIO[MAX_LANIO];
	wxCriticalSection CSpLANIO[MAX_LANIO]; // for thread protection
	bool fAutoSave;
	wxCriticalSection CSfAutoSave;
	char strPath[MAX_PATH_NAME];
	friend class SearchThread;
	friend class LANIOThread;
    wxDECLARE_EVENT_TABLE();	
 private:
	void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
	void OnButton(wxCommandEvent& event);
	void OnCheck(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);
	void OnSelectDevice(wxListEvent& event);
	void OnDeselectDevice(wxListEvent& event);
	void OnEndSearchThread(wxThreadEvent& event);
	void OnEndLANIOThread(wxThreadEvent& event);
	void OnCloseLANIODialog(wxCommandEvent& event);
	void OnUpdateThread(wxThreadEvent& event);
	void OnDialogBtnEvent(wxCommandEvent& event);
	
	void SearchDevice();
	void AddDevice();
	void ConnectDevice();
	void SaveCSV(int did);
	void ChangeDir();
	
	void UpdateDeviceList();
	void ConnectEnable();
	void ControlEnable(bool enable);
	void UpdateControls();
	void ReadIOLabels();
	void WriteIOLabels();

	LANIOPanel *pLANIOdlg[MAX_LANIO];
	
	wxPanel *btnPanel;
	wxPanel *listPanel;
	wxPanel *savePanel;
	
	wxListCtrl *devList;

	wxTextCtrl *txformPort;
	wxTextCtrl *txformIP;
	wxButton *btnSearch;
	wxButton *btnCnct;
	wxButton *btnAdd;
	wxButton *btnFolder;
	wxButton *btnClose;
	
	wxCheckBox *chkSave;
	
	wxStaticText *PortText;
	wxStaticText *IPText;
	wxStaticText *DevListText;
	wxStaticText *SaveText;
	
	wxStaticBox *saveGrp;
		
	unsigned long portnum;
	
	bool fEnbCnct; // 接続可
	bool fEnbCont; // コントロール可

	char ioLabels[MAX_MODEL][16][MAX_DODI][IO_LBL_MAXLEN+1];
};

enum
{
    // menu items
    Menu_Quit = wxID_EXIT
};

/////////////////////////////////////////////////////////////
// Class : SearchThread
//         検索用スレッド
//         検索に伴う通信中に他処理が遅延しないために用いる
/////////////////////////////////////////////////////////////
class SearchThread : public wxThread
{
 public:
	SearchThread(MyFrame *frame, int mode, int port);
	SearchThread(MyFrame *frame, int mode, const char * ipstr);
	virtual ~SearchThread();
 protected:
	virtual ExitCode Entry();
	MyFrame *hFrame;
 private:
	int portnum;
	int searchmode; // [0]:Search [1]:Add
	char ip[16];
};

/////////////////////////////////////////////////////////////
// Class : LANIOThread
//         各LANIOデバイスの処理を担当するスレッド
//         接続中の1デバイスに付き1つ作成される
/////////////////////////////////////////////////////////////
class LANIOThread : public wxThread
{
 public:
	LANIOThread(MyFrame *frame, LANIOPanel *dlg, int la_id);
	virtual ~LANIOThread();
	bool readDInow;
	bool changeDOnow;
	bool autoRead;
	int autoReadTime; // x 100 msec
	int autoReadCount;
	bool autoChange[MAX_DO];
	int autoChangeTime[MAX_DO]; // x 100 msec
	int autoChangeCount[MAX_DO];
	wxCriticalSection CS; // protect public members
 protected:
	virtual ExitCode Entry();
	MyFrame *hFrame;  // Main window pointer
	LANIOPanel *hDlg; // Dialog pointer
 private:
	int lanio_id; // LANIO ID in the list
	int LAhandle; // device handle
	void RequestUpdate(int reqid);
};

#endif
