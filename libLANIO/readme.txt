*******************************
** LANIOライブラリの使用方法 **
*******************************

LANIOライブラリを使用することにより、LANIOをアプリケーションから操作
することができます

・本ソフトウェアの著作権は弊社が保有します。 
・本ソフトウェアの動作保証およびサポートは行っておりません。
・本ソフトウェアによるお客様の損害、逸失利益、第三者のいかなる請求につき
  ましても当社は一切その責任を負いかねます。
・本ソフトウェアは事前に通知せず内容変更、配布中止をすることがあります。

● 対応環境

  LANIOライブラリが使用できる機種は、LA-3R2P、LA-3R2P-P、LA-7P-A、LA-7P-P、
  LA-5P-P、LA-5R、LA-5T2S、LA-5T2S-P、LA-3R3P-P、LA-2R3P-P、
  LA-232/485シリーズです。


● インストールについて

  ライブラリはルート権限で下記のインストールスクリプトを実行することで
  使用可能状態となります。
  [64 bit版 Linux PCの場合] libLANIO_install64.sh
  [32 bit版 Linux PCの場合] libLANIO_install86.sh	
  [Raspberry Pi の場合]     libLANIO_installPi.sh
  
  ライブラリファイルのデフォルトのインストール先は /usr/local/lib となり
  ますが、変更する場合は書き換えてください。


● アンインストールについて

  ルート権限で libLANIO_uninstall.sh を実行します。


●アプリケーションからライブラリを使用する時の手順

  LANIOライブラリを使用するときは、大まかには以下のような手順を踏みます。

  1.ライブラリを初期化する

    該当する関数：LELanioInit

  2.LANに接続されているLANIOを検索する
    （LELanioConnectDirect関数で接続する場合は必要ありません）

    該当する関数：LELanioSearch

  3.発見したLANIOの情報を取得する
    （LELanioConnect、LELanioConnectDirect関数で接続する場合は必要ありません）
    取得する際には、LELanioSearchによって検索された時にライブラリ内部で割り振
    られたLANIO番号が必要となります。

    該当する関数：LELanioGetIpAddress、LELanioGetMacAddress
                  LELanioGetId、LELanioGetModel

  4.指定した条件でLANIOと接続する

    該当する関数：LELanioConnect、LELanioConnectByIdModel
                  LELanioConnectByIpAddress、LELanioConnectByMacAddress
                  LELanioConnectDirect

  5.接続したLANIOのPIOに対して各種処理を実行する
    各種処理の際には、LANIO接続関数が返したLANIOハンドラが必要となります。

    該当する関数：LELanioOutPio、LELanioOutPioAll
                  LELanioInPio、LELanioInPioAll
                  LELanioGetOut、LELanioGetOutAll

  6.LANIOとの接続を終了する
    接続終了には、LANIO接続関数が返したLANIOハンドラが必要となります。

    該当する関数：LELanioClose

  7.ライブラリの終了処理を行う

    該当する関数：LELanioEnd

  その他の関数としては、各種の関数でエラーが発生した場合にそのエラー要因を調べ
  るLELanioGetLastError関数、LANIO検索時にモデルとIDを取得する条件を指定する
  LELanioSetAutoRequestIdModel関数があります。


●各関数のリファレンス

● 初期化、終了、エラー取得関数
LANIOライブラリの初期化、終了、ライブラリ内で発生したエラー情報の取得関数です。

○ LELanioInit

LANIOライブラリを初期化します。
この関数を実行する前に他の関数は実行できません。

用法   : LELanioInit()
引数   : なし
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioInit();
         LANIOライブラリを初期化します。

○ LELanioEnd

LANIOライブラリの使用を終了します。プログラムを終了する時に実行してください。
接続中のLANIOがあれば切断します。
再び使用を再開する場合はLELanioInitを呼び直す必要があります。

用法   : LELanioEnd()
引数   : なし
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioEnd();
         LANIOライブラリの使用を終了します。

○ LELanioGetLastError

直前に発生したエラー番号を取得します。戻り値はエラー番号です。
各関数の実行時の戻り値がエラーだった場合、その原因を調べるのに使用します。
10000以上の場合はOSのエラーに10000を加えたもの、1～の場合はLANIODLLのエラーです。
 1 初期化されていません。LELanioInitを実行してください。
 2 検索した結果、LANIOが発見できませんでした。
 3 検索で発見したLANIOより多い値を指定しました。
 4 指定された条件のLANIOを発見できませんでした。
 5 LANIOと接続していません。LELanioConnectを実行してください。
 6 指定された操作は実行できません。
 7 すでに接続しています。
 8 接続最大数を超過しました。
 9 不正な戻り値が返りました。（設定した数値が異常である可能性があります）

用法   : LELanioGetLastError()
引数   : なし
戻り値 : エラーの情報を返します
例     : int error = LELanioGetLastError();
         変数errorにLANIOの直前のエラー番号を取得します。


● LANIO検索関数
LAN内にあるLANIOを検索する関数群です。

○ LELanioSetAutoRequestIdModel

この関数はLELanioSearchの実行時に、自動的にLANIOのIDとModelを取得する機能につい
ての設定をします。
LELanioSearchより先に実行する必要があります。
LELanioSearch実行時、この機能が有効で、かつ発見したLANIOのシリアルのポート番号
設定がこの関数で指定した番号と一致した場合、自動的にLELanioRequestIdModel関数と
同等の処理を行います。

用法   : LELanioSetAutoRequestIdModel(BOOL enable, int port)
引数   : enable  この機能を使用する場合はTRUE(1)、しない場合はFALSE(0)
         port    IDとModelを自動取得するシリアルのポート番号
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioSetAutoRequestIdModel(TRUE,10500);
         検索時、シリアルのポート番号が10500に設定されているLANIOにIDとModelを
         取得するように設定します。

○ LELanioSearch

LAN内にあるLANIOを検索します。
最初に発見したLANIOより順に内部的に0より番号が割り当てられ、以降の情報取得関数
などにはこの内部的なLANIO番号を引数として指定することになります。
LANIO接続関数で接続中のLANIOがあった場合、この関数を実行すると接続は終了されま
す。
この関数はLANIOシリーズを判別するため、LAN上に存在するシリアルのポート番号設定
値が10003の全XPortデバイスに対し、LANIOのIDとModelを取得するコマンドを送信しま
す。
同一LAN内にLANIO以外のシリアルのポート番号が10003に設定されたXPortデバイスが
あった場合はこの情報取得コマンドを受信するので、誤動作の可能性があります。
情報取得コマンドの無効化および、LANIOと認識するシリアルのポート番号を変更する
にはLELanioSetAutoRequestIdModel関数を使います。

用法   : LELanioSearch(int msec)
引数   : msec    タイムアウト時間（単位 m秒）
戻り値 : 発見したLANIOの数。発見できなかった場合は0。エラー時は-1
例     : int laniocount = LELanioSearch(500);
         LAN内に接続されているLANIOをタイムアウト500msecで検索し、発見数を
         laniocountに代入します。


● LANIO情報取得関数
LELanioSearchで検索したLANIOの各種情報を取得する関数群です。
LELanioSearch後でなければ実行できません。

○ LELanioGetIpAddress

検索したLANIOのIPアドレスを取得します。
IPアドレスは文字形式で、"192.168.0.1"等の形式となります。

用法   : LELanioGetIpAddress(int lanio, char *ipaddress)
引数   : lanio       LANIO番号
         ipaddress   IPアドレスを取得するバッファへのポインタ
                     16バイトの領域が必要です。
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : char ipaddress[16];
         int error = LELanioGetIpAddress(0,ipaddress);
         ipaddressのポインタに、最初に発見したLANIOのIPアドレスを取得します。

○ LELanioGetMacAddress

検索したLANIOのMACアドレスを取得します。
MACアドレスはバイナリ6バイトの形式となります。

用法   : LELanioGetMacAddress(int lanio, unsigned char *macaddress)
引数   : lanio       LANIO番号
         macaddress  MACアドレスを取得するバッファへのポインタ
                     6バイトの領域が必要です。
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : unsigned char macaddress[6];
         int error = LELanioGetMacAddress(0,macaddress);
         macaddressのポインタに、最初に発見したLANIOのMACアドレスを取得します。

○ LELanioGetId

LANIOのID番号を取得します。
LELanioSetAutoRequestIdModelによるID番号とモデル自動取得を行っているか、もしく
はLELanioSearch後にLELanioRequestIdModelによってID番号とモデルを得ているかの
どちらかである必要があります。取得していない場合の値は-1となります。

用法   : LELanioGetId(int lanio, int *id)
引数   : lanio   LANIO番号
         id      LANIOのIDを取得するバッファへのポインタ
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int id;
         int error = LELanioReceiveId(0,&id);
         最初に発見したLANIOのIDをidに取得します。

○ LELanioGetModel

LANIOのモデルを取得します。
LELanioSetAutoRequestIdModelによるID番号とモデル自動取得を行っているか、もしく
はLELanioSearch後にLELanioRequestIdModelによってID番号とモデルを得ているかの
どちらかである必要があります。LA-485シリーズの場合、ID番号は0固定となります。
モデルの数値の意味は次の通りです。
  1 LA-3R2P、LA-3R2P-P
  2 LA-7P、LA-7P-P
  3 LA-5R
  4 LA-5T2S、LA-5T2S-P
  5 LA-5P-P
  6 LA-3R3P-P
  7 LA-232R、LA-232R-P
  8 LA-232T、LA-232T-P
  9 LA-485R、LA-485R-P
 10 LA-485T、LA-485T-P
取得していない場合の値は-1となります。

用法   : LELanioGetModel(int lanio, int *model)
引数   : lanio   LANIO番号
         model   LANIOのモデルを取得するバッファへのポインタ
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int model;
         int error = LELanioReceiveModel(0,&model)
         最初に発見したLANIOのモデルをmodelに取得します。

○ LELanioRequestIdModel

検索したLANIOに対してIDとモデル番号を質問します。

用法   : LELanioRequestIdModel(int lanio, int *id, int *model)
引数   : lanio   LANIO番号
         id      LANIOのIDを取得するバッファへのポインタ
         model   LANIOのモデルを取得するバッファへのポインタ
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int id, model;
         int error = LELanioRequestIdModel(0,&id,&model);
         最初に発見したLANIOのIDとモデル番号をid、modelのポインタに取得します。


● LANIO接続関数
どのLANIOと入出力するかを指定する関数です。戻り値は全てLANIO操作ハンドルとなり
エラー時は-1です。
LANIO操作ハンドルは接続後のPIO操作や切断に必要となります。
LELanioConnectDirectを除いて、LELanioSearchによって発見したLANIOに対してのみ
実行できます。
接続可能の最大数は256です。

○ LELanioConnect

LELanioSearchによって発見された順番を指定して接続します。

用法   : LELanioConnect(int lanio)
引数   : lanio   LANIO番号
戻り値 : エラー時は-1、正常終了時はLANIO操作ハンドルを返します
例     : hLANIO handle = LELanioConnect(0);
         最初に発見したLANIOと接続します。

○ LELanioConnectByIdModel

IDとモデルを指定して接続します。同モデルでIDが同じ複数の機器があった場合は先に
発見した方に接続されます。

用法   : LELanioConnectByIdModel(int id, int model)
引数   : id      LANIOのID番号
         model   LANIOのモデル
戻り値 : エラー時は-1、正常終了時はLANIO操作ハンドルを返します
例     : hLANIO handle = LELanioConnectById(15,1);
         IDが15番のLA-3R2Pと接続します。

○ LELanioConnectByIpAddress

IPアドレスを指定して接続します。

用法   : LELanioConnectByIpAddress(char *ipaddress)
引数   : ipaddress   IPアドレス文字列バッファへのポインタ
戻り値 : エラー時は-1、正常終了時はLANIO操作ハンドルを返します
例     : hLANIO handle = LELanioConnectByIpAddress(ipaddress);
         変数ipaddress文字列内に格納されたIPアドレスのLANIOと接続します。

○ LELanioConnectByMacAddress

MACアドレスを指定して接続します。

用法   : LELanioConnectByMacAddress(unsigned char *macaddress)
引数   : macaddress  MACアドレスバッファへのポインタ
戻り値 : エラー時は-1、正常終了時はLANIO操作ハンドルを返します
例     : hLANIO handle = LELanioConnectByMacAddress(macaddress);
         変数macaddressに格納されたMACアドレスのLANIOと接続します。

○ LELanioConnectDirect

IPアドレスもしくはURLを指定して接続します。
LELanioConnectByIpAddressとの違いはLAN内に存在しないLANIOを対象とすることです。
この関数のみLELanioSearchの必要はありません。

用法   : LELanioConnectDirect(char *address, int msec)
引数   : address IPアドレスまたはurl文字列バッファへのポインタ
         msec    タイムアウト時間（単位 m秒）
戻り値 : エラー時は-1、正常終了時はLANIO操作ハンドルを返します
例     : hLANIO handle = LELanioConnectDirect("dummy.co.jp",5000);
         dummy.co.jpのURLでアクセスできるLANIOとタイムアウト5秒で接続します。


● LANIO切断関数
LANIOとの接続を終了する時に使用します。
LANIO接続関数で戻り値として得たLANIO操作ハンドルで、切断するLANIOを指定します。

○ LELanioClose

LANIOとの接続を切断します。

用法   : LELanioClose(hLANIO handle)
引数   : handle  LANIO操作ハンドル
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioClose(handle);
         LANIO操作ハンドルhandleのLANIOとの接続を終了します。


● 入出力関数
LANIOの入出力ポートを操作する関数です。
LANIO接続関数で戻り値として得たLANIO操作ハンドルで、操作するLANIOを指定します。

○ LELanioOutPio

LANIOのDOを操作します。一点単位の操作に使用します。

用法   : LELanioOutPio(hLANIO handle, int pio, BOOL active)
引数   : handle  LANIO操作ハンドル
         pio     操作するDOポート番号。DO1=0、DO2=1...
         active  ONなら1、OFFなら0
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioOutPio(handle,2,1);
         LANIO操作ハンドルhandleのLANIOのDO3をONにします。

○ LELanioOutPioAll

LANIOのDOを操作します。全てのDOに対し一斉に操作する時に使用します。

用法   : LELanioOutPioAll(hLANIO handle, int piobit)
引数   : handle  LANIO操作ハンドル
         piobit  DOポートの設定。最下位ビットからDO1、DO2...に対応します。
                 ONなら1、OFFなら0となります。
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioOutPioAll(handle,0x05);
         LANIO操作ハンドルhandleのLANIOのDO1、DO3をONに、それ以外をOFFにします。

○ LELanioInPio

LANIOのDIを取得します。一点単位の取得に使用します。

用法   : LELanioInPio(hLANIO handle, int pio, BOOL *active)
引数   : handle  LANIO操作ハンドル
         pio     取得するDIポート番号。DI1=0、DI2=1...に対応します。
         active  ONなら1、OFFなら0が入るバッファへのポインタ
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioInPio(handle,2,&active);
         LANIO操作ハンドルhandleのLANIOのDI3状態をactiveに取得します。

○ LELanioInPioAll

LANIOのDIを取得します。全てのDIに対し一斉に取得する時に使用します。

用法   : LELanioInPioAll(hLANIO handle, int *piobit)
引数   : handle  LANIO操作ハンドル
         piobit  DIポートの内容が入るバッファへのポインタ。
                 最下位ビットからDI1、DI2...
                 ONなら1、OFFなら0となります。
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioInPioAll(handle,&piobit);
         LANIO操作ハンドルhandleのLANIOのDI状態をpiobitに取得します。

○ LELanioGetOut

LANIOの現在のDO状態を取得します。一点単位の取得に使用します。

用法   : LELanioGetOut(hLANIO handle, int pio, BOOL *active)
引数   : handle  LANIO操作ハンドル
         pio     取得するDOポート番号。DO1=0、DO2=1...に対応します。
         active  ONなら1、OFFなら0が入るバッファへのポインタ
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioGetOut(handle,2,&active);
         LANIO操作ハンドルhandleのLANIOの現在のDO3状態をactiveに取得します。

○ LELanioGetOutAll

LANIOの現在のDO状態を取得します。全てのDOに対し一斉に取得する時に使用します。

用法   : LELanioGetOutAll(hLANIO handle, int *piobit)
引数   : handle  LANIO操作ハンドル
         piobit  DOポートの内容が入るバッファへのポインタ。
                 最下位ビットからDO1、DO2...
                 ONなら1、OFFなら0となります。
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioGetOutAll(handle,&piobit);
         LANIO操作ハンドルhandleのLANIOの現在のDO状態をpiobitに取得します。

● 出力端子自動ON/OFF関数
LANIOの入出力ポートの自動ON/OFF動作を指定する関数です。
LA-5R、LA-5T2S(-P)でのみ使用できます。
LANIO接続関数で戻り値として得たLANIO操作ハンドルで、操作するLANIOを指定します。

○ LELanioSetAutoSwitchingActive

LANIOの出力端子自動ON/OFFの開始/停止を行います

用法   : LELanioSetAutoSwitchingActive(hLANIO handle, BOOL active)
引数   : handle  LANIO操作ハンドル
         active  自動ON/OFFを開始する=0以外、停止する=0
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioSetAutoSwitchingActive(handle,1);
         LANIO操作ハンドルhandleのLANIOの出力端子自動ON/OFFを開始します。

○ LELanioGetAutoSwitchingActive

LANIOの出力端子自動ON/OFFの開始/停止設定を取得します

用法   : LELanioGetAutoSwitchingActive(hLANIO handle, BOOL *active)
引数   : handle  LANIO操作ハンドル
         active  自動ON/OFF状態が入るバッファへのポインタ
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioGetAutoSwitchingActive(handle,&active);
         LANIO操作ハンドルhandleのLANIOの出力端子自動ON/OFFをactiveに取得します。

○ LELanioSetAutoSwitchingTime

LANIOの出力端子自動ON/OFFの周期を設定します
周期は100ミリ秒～1400ミリ秒の範囲で指定できます。
2000ミリ秒までは100ミリ秒単位で、それより上は1000ミリ秒単位での指定となります。

用法   : LELanioSetAutoSwitchingTime(hLANIO handle, int msec)
引数   : handle  LANIO操作ハンドル
         msec    周期（単位 m秒）
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioSetAutoSwitchingTime(handle,1);
         LANIO操作ハンドルhandleのLANIOの出力端子自動ON/OFFの周期を1000ミリ秒に
         設定します。

○ LELanioGetAutoSwitchingTime

LANIOの出力端子自動ON/OFFの周期設定を取得します

用法   : LELanioGetAutoSwitchingTime(hLANIO handle, int *msec)
引数   : handle  LANIO操作ハンドル
         msec    周期（単位 m秒）が入るバッファへのポインタ
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioGetAutoSwitchingTime(handle,&msec);
         LANIO操作ハンドルhandleのLANIOの出力端子自動ON/OFFの周期をmsecに
         取得します。

○ LELanioSetAutoSwitchingPio

LANIOの出力端子自動ON/OFFを行うDOを設定します。

用法   : LELanioSetAutoSwitchingPio(hLANIO handle, int autoswdo)
引数   : handle   LANIO操作ハンドル
         autoswdo 最下位ビットより、DO1～DO5で自動ON/OFFを行う端子が1
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioSetAutoSwitchingPio(handle,5);
         LANIO操作ハンドルhandleのLANIOの出力端子自動ON/OFFを行う端子をD0、D2に
         設定します。

○ LELanioGetAutoSwitchingPio

LANIOの出力端子自動ON/OFFを行うDO設定を取得します

用法   : LELanioGetAutoSwitchingPio(hLANIO handle, int *autoswdo)
引数   : handle   LANIO操作ハンドル
         autoswdo 最下位ビットより、DO1～DO5で自動ON/OFFを行う端子が1
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioGetAutoSwitchingPio(handle,&autoswdo);
         LANIO操作ハンドルhandleのLANIOの出力端子自動ON/OFFを行う端子をautoswdoに
         設定します。

● 入出力延長機能設定関数
LANIOの入出力延長機能を指定する関数です。
LA-5P-P、LA-3R3P-P、LA-232/485シリーズのみ使用できます。
LA-232/485シリーズでは定期通知時間設定のみサポートしています。

LANIO接続関数で戻り値として得たLANIO操作ハンドルで、操作するLANIOを指定します。

○ LELanioSetDisconnectionTime

LANIOの無通信状態になった後の切断時間を設定する

用法   : LELanioSetDisconnectionTime(hLANIO handle, int time);
引数   : handle  LANIO操作ハンドル
         time    時間
                0x00:  0ms    0x04: 3sec  0x08:50sec  0x0c:10min
                0x01:300ms    0x05: 5sec  0x09: 1min  0x0d:30min
                0x02:500ms    0x06:10sec  0x0a: 3min  0x0e:50min
                0x03: 1sec    0x07:30sec  0x0b: 5min  0x0f:なし
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioSetDisconnectionTime(handle,6);
         LANIOの無通信状態になった後の切断時間を10secに設定します。

○ LELanioGetDisconnectionTime

LANIOの無通信状態になった後の切断時間を取得する

用法   : LELanioGetDisconnectionTime(hLANIO handle, int *time);
引数   : handle  LANIO操作ハンドル
         time    時間
                0x00:  0ms    0x04: 3sec  0x08:50sec  0x0c:10min
                0x01:300ms    0x05: 5sec  0x09: 1min  0x0d:30min
                0x02:500ms    0x06:10sec  0x0a: 3min  0x0e:50min
                0x03: 1sec    0x07:30sec  0x0b: 5min  0x0f:なし
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioGetDisconnectionTime(handle,&time);
         LANIOの無通信状態になった後の切断時間をtimeに取得します。

○ LELanioSetNoticeTime

LANIOの定期通知時間を設定する

用法   : LELanioSetNoticeTime(hLANIO handle, int time);
引数   : handle  LANIO操作ハンドル
         time    時間
                0x00:通知なし 0x04: 3min     0x08:設定禁止  0x0c:設定禁止
                0x01:10sec    0x05:設定禁止  0x09:設定禁止  0x0d:設定禁止
                0x02:30sec    0x06:設定禁止  0x0a:設定禁止  0x0e:設定禁止
                0x03:60sec    0x07:設定禁止  0x0b:設定禁止  0x0f:設定禁止
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioSetNoticeTime(handle,2);
         LANIOの定期通知時間を30secに設定します。

○ LELanioGetNoticeTime

LANIOの定期通知時間を取得する

用法   : LELanioGetNoticeTime(hLANIO handle, int *time);
引数   : handle  LANIO操作ハンドル
         time    時間
                0x00:通知なし 0x04: 3min    0x08:  ---   0x0c:  ---
                0x01:10sec    0x05:  ---    0x09:  ---   0x0d:  ---
                0x02:30sec    0x06:  ---    0x0a:  ---   0x0e:  ---
                0x03:60sec    0x07:  ---    0x0b:  ---   0x0f:  ---
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioGetNoticeTime(handle,&time);
         LANIOの定期通知時間をtimeに取得します。

● DI検出時間設定
LANIOのDI検出時間を指定する関数です。
LA-5P-P、LA-3R3P-Pでのみ使用できます。

○ LELanioSetDiDetectTime

LANIOのDI検出時間を設定する

用法   : LELanioSetDiDetectTime(hLANIO handle, int time);
引数   : handle  LANIO操作ハンドル
         time    時間(ms単位 4ms～20ms）
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioSetDiDetectTime(handle,10);
         LANIOのチャタリング判定の基準時間を10msecに設定します。

○ LELanioGetDiDetectTime

LANIOのDI検出時間を取得する

用法   : LELanioGetDiDetectTime(hLANIO handle, int *time);
引数   : handle  LANIO操作ハンドル
         time    時間(ms単位 4ms～20ms）
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioGetDiDetectTime(handle,&time);
         LANIOのDI検出時間をtimeに取得します。

● メールアラート拡張設定関数
LANIOのメールアラート拡張設定を指定する関数です。
LA-5P-P、LA-3R3P-Pでのみ使用できます。

○ LELanioSetDiMailEnable

LANIOのDIメールアラート制御を設定します

用法   : LELanioSetDiMailEnable(hLANIO handle, int enable);
引数   : handle  LANIO操作ハンドル
         enable  メールアラートを実行するビットを1で指定
                 最下位ビットからDI1、DI2...
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioSetDiMailEnable(handle,100);
         DI1～DI5をメールアラート有効にする

┌───────┬───────┬───────┬───────┬───────┐
│DI1(1ビット目)│DI2(2ビット目)│DI3(3ビット目)│DI4(4ビット目)│DI5(5ビット目)│
├───────┼───────┼───────┼───────┼───────┤
│有効(1)       │有効(1)       │有効(1)       │有効(1)       │有効(1)       │
└───────┴───────┴───────┴───────┴───────┘


○ LELanioGetDiMailEnable

LANIOのDIメールアラート制御を取得します

用法   : LELanioGetDiMailEnable(hLANIO handle, int *enable);
引数   : handle  LANIO操作ハンドル
         enable  メールアラートを実行するビットを1で指定
                 最下位ビットからDI1、DI2...
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioGetDiMailEnable(handle,&enable);
         LANIOのメールアラートを実行する入力端子をenableに取得します。


○ LELanioSetDiMailLogic

LANIOのDIメールアラート論理を設定します

用法   : LELanioSetDiMailLogic(hLANIO handle, int logic);
引数   : handle  LANIO操作ハンドル
         logic   立ち上がりエッジでメールアラートを実行するビットを1で指定
                 最下位ビットからDI1、DI2...
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioSetDiMailLogic(handle,0x1F);
         DI1～DI5がON(DIのLEDが点灯)のときCP1端子(メールアラート端子)
         をHIGHにする

┌───────┬───────┬───────┬───────┬───────┐
│DI1(1ビット目)│DI2(2ビット目)│DI3(3ビット目)│DI4(4ビット目)│DI5(5ビット目)│
├───────┼───────┼───────┼───────┼───────┤
│ON(1)         │ ON(1)        │ON(1)         │ON(1)         │ON(1)         │
└───────┴───────┴───────┴───────┴───────┘

○ LELanioGetDiMailLogic

LANIOのDIメールアラート論理を取得します

用法   : LELanioGetDiMailLogic(hLANIO handle, int *logic);
引数   : handle  LANIO操作ハンドル
         logic   立ち上がりエッジでメールアラートを実行するビットを1で指定
                 最下位ビットからDI1、DI2...
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioGetDiMailLogic(handle,&logic);
         LANIOのメールアラートを実行する論理をlogicに取得します。

○ LELanioPulseModeOn

LA-5P-P、LA-3R3P-PのPulseModeに入ります

用法   : LELanioPulseModeOn(hLANIO handle);
引数   : handle  LANIO操作ハンドル
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioPulseModeOn(handle);
         PulseModeに入ります

○ LELanioPulseModeOff

LA-5P-P、LA-3R3P-PのPulseModeを終了します

用法   : LELanioPulseModeOff(hLANIO handle);
引数   : handle  LANIO操作ハンドル
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioPulseModeOff(handle);
         PulseModeを終了します

○ LELanioPulseModeGetState

LA-5P-P、LA-3R3P-PのPulseMode状態を取得します

用法   : LELanioPulseModeGetState(hLANIO handle, int *mode);
引数   : handle  LANIO操作ハンドル
         mode    PulseMode状態。モード中なら1、モード中でなければ0
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioPulseModeGetState(handle,&mode);
         PulseMode状態をmodeに取得します

○ LELanioPulseModeGetCount

PulseModeのパルス数を取得します

用法   : LELanioPulseModeGetCount(hLANIO handle, int *value);
引数   : handle  LANIO操作ハンドル
         mode    PulseMode状態。モード中なら1、モード中でなければ0
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioPulseModeGetCount(handle,&value);
         PulseModeのパルス数をvalueに取得します

○ LELanioPulseModeSetConfig

PulseModeの設定をします

用法   : LELanioPulseModeSetConfig(hLANIO handle,
                int pio, BOOL edge, int time, int chatter);
引数   : handle  LANIO操作ハンドル
         pio     操作するDIポート番号。DI1=0、DI2=1...
         edge    0で立ち上がり、1で立ち下がりエッジ
         time    測定時間 0:START-STOP  1:100msec  2:1sec  3:10sec
         chatter チャタリング抑制 0:なし  1～3:設定不可  4～20:4～20msec
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioPulseModeSetConfig(handle,0,1,2,4);
         PulseModeをDI1、立ち下がりエッジ、1sec、チャタリング4msに設定します。

○ LELanioPulseModeGetConfig

PulseModeの設定を取得します

用法   : LELanioPulseModeGetConfig(hLANIO handle,
                int *pio, BOOL *edge, int *time, int *chatter);
引数   : handle  LANIO操作ハンドル
         pio     操作するDIポート番号。DI1=0、DI2=1...
         edge    0で立ち上がり、1で立ち下がりエッジ
         time    測定時間 0:START-STOP  1:100msec  2:1sec  3:10sec
         chatter チャタリング抑制 0:なし  1～3:設定不可  4～20:4～20msec
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioPulseModeGetConfig(handle,&pio,&edge,&time,&chatter);
         PulseModeの設定をpio、edge、time、chatterに取得します。

○ LELanioPulseModeContinuesOn

パルス連続測定を開始します
LELanioPulseModeSetConfigでtimeを0（START-STOP）にしている必要があります

用法   : LELanioPulseModeContinuesOn(hLANIO handle);
引数   : handle  LANIO操作ハンドル
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioPulseModeContinuesOn(handle);
         パルス連続測定を開始します

○ LELanioPulseModeContinuesOff

パルス連続測定を終了します

用法   : LELanioPulseModeContinuesOff(hLANIO handle);
引数   : handle  LANIO操作ハンドル
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioPulseModeContinuesOff(handle);
         パルス連続測定を終了します

○ LELanioPulseModeContinuesGetState

パルス連続測定状態を取得します

用法   : LELanioPulseModeContinuesGetState(hLANIO handle, int *mode);
引数   : handle  LANIO操作ハンドル
         mode    パルス連続測定状態。モード中なら1、モード中でなければ0
戻り値 : エラー時は0、正常終了時は0以外を返します
例     : int error = LELanioPulseModeContinuesGetState(handle,&mode);
         パルス連続測定状態をmodeに取得します


