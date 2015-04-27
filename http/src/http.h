#ifndef http_h_
#define http_h_

#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib")

#include <string>
#include <FantomWaves.h>
#include "HttpHeader.h"

/*
** v3.0.0
** HTTP通信を行い、データを取得します
** GETリクエストのみで、データを取得した時点で接続を切ります
** Transfer-Encoding: chunked には現在未対応
*/
class Http
{
public:
	/* @brief サーバーと通信を行い、データを受信します
	** @param
	**  url:サーバーのIPアドレスやドメインだけでなく、URL全体を指定してください
	** @return 成功:true, 失敗:false
	*/
	bool load(const std::string url);

	/* @brief "プロキシ"サーバーと通信を行い、データを受信します
	** @param
	**  url:サーバーのIPアドレスやドメインだけでなく、URL全体を指定してください
	**  proxy_name:プロキシサーバーのドメインかIPアドレスを指定します(ex."192.168.1.1")
	**  user_pass64:"ユーザー名:パスワード"の文字列を64エンコードした文字列を指定します
	** @return 成功:true, 失敗:false
	*/
	bool load(
		const std::string url,
		const std::string proxy_name,
		const std::string user_pass64);

	// 受信したデータを文字列として取得します
	// 末尾にヌル文字が追加された状態となっています
	const char * content_as_text() const;

	// 受信したデータのバイト数を取得します
	// 末尾に追加されたヌル文字の分はカウントされません
	fw::uint content_byte() const;

	// サーバーから返されたHTTPヘッダーを管理するクラスのインスタンスを取得します
	const HttpHeader & get_header() const;

	// 何もエラーが起こっていないときtrue, 何らかのエラーが既に起きているときfalseを返します
	bool no_error() const;

	// エラーの種類を特定する際に使用します **********************
	bool failed_to_match_version() const;
	bool failed_to_init_winsock() const;
	bool failed_to_create_socket() const;
	bool failed_to_find_IP() const;
	bool failed_to_connect() const;
	bool failed_to_send() const;
	bool failed_to_receive() const;
	bool unsupported_HTTPS() const;
	bool server_unsupported() const;
	bool failed_to_find_URI() const;
	bool server_disconnected() const;
	bool socket_error() const;
	// ********************************************************

	// エラーの種類を文字列として取得します
	std::string error_text() const;



	Http();
	~Http();

private:
	enum ErrorMode
	{
		NoErrorMode,
		FailedToMatchVersionMode,
		FailedToInitWinsockMode,
		FailedToCreateSocketMode,
		FailedToFindIPMode,
		FailedToConnectMode,
		FailedToSendMode,
		FailedToReceiveMode,
		UnsupportedHTTPSMode,
		ServerUnsupportedMode,
		FailedToFindURIMode,
		ServerDisconnectedMode,
		SocketErrorMode
	};

	ErrorMode error_mode;	// エラーが発生した場合はここにエラーモードが記録されます

	// 通信により取得したデータがここに格納されます
	// 末尾にはヌル文字が追加されます
	fw::vuchar content;

	std::string domain;
	std::string uri;
	SOCKET socket;
	SOCKADDR_IN hostinfo;
	bool socketflag;
	bool connecting;
//	fw::uint ContentSize;
	short port;
	std::string additionalHeader;
	std::string host;
	HttpHeader header;




	// sendしてrecvします
	void just_load();

	// サーバーとの接続を切ります
	void disconnect();




	// winsock2を初期化します
	bool init_winsock();

	// ドメイン名からIPアドレスを取得します
	bool get_IP();

	// ソケットを作成します
	bool create_socket();

	// 実際にサーバーにアクセスします
	bool connect_to_server();

	// sendします
	bool request();

//	bool ReceiveAll();

	// recvします
	void receive();


	// 受信バッファからデータを取り出す処理を続行すべきかどうか判断します
	// 続行すべきでない＝データ取り出しは全て完了したならtrueを、
	// 続行すべき＝データ取り出しはまだ完了していないならfalseを返します
	bool judge_finish_or_not(fw::uint total, fw::uint received_byte, fw::uint prepared_size);

	// 受信したデータを記録するバッファーのサイズを、受信したデータのサイズとぴったり同じにします
	// バッファーはある程度余裕を持って作成されているからです
	void adjust_content(fw::uint size);

	// 受信データからヘッダー部分を探します
	// ヘッダー部分の全てが受信済みならヘッダー管理クラスにデータを渡します
	// そしてその場で解析が行われます
	void look_for_header();




	// Winsock2のクリーンナップを行います
	static void cleanup(void);




};


#endif