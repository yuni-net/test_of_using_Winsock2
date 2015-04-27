#include "Http.h"
#include "HttpHeader.h"
#include "URLSeparater.h"

// プロキシサーバーと通信する際、hostは取得したいURLのドメインなのか？？？


/* @brief サーバーと通信を行い、データを受信します
** @param
**  url:サーバーのIPアドレスやドメインだけでなく、URL全体を指定してください
** @return 成功:true, 失敗:false
*/
bool Http::load(const std::string url)
{
	error_mode = NoErrorMode;
	content.zerosize();
	header.init();

	if (init_winsock() == false)
	{
		return false;
	}
	if (create_socket() == false)
	{
		return false;
	}

	port = 80;
	additionalHeader = "";

	URLSeparater rater(url);
	this->domain = rater.domain();
	this->uri = rater.uri();
	this->host = domain;

	if (get_IP() == false)
	{
		return false;
	}

	if (connect_to_server() == false)
	{
		return false;
	}

	just_load();
	disconnect();

	return true;
}

/* @brief "プロキシ"サーバーと通信を行い、データを受信します
** @param
**  url:サーバーのIPアドレスやドメインだけでなく、URL全体を指定してください
**  proxy_name:プロキシサーバーのドメインかIPアドレスを指定します(ex."192.168.1.1")
**  user_pass64:"ユーザー名:パスワード"の文字列を64エンコードした文字列を指定します
** @return 成功:true, 失敗:false
*/
bool Http::load(
	const std::string url,
	const std::string proxy_name,
	const std::string user_pass64)
{
	error_mode = NoErrorMode;
	content.zerosize();
	header.init();

	if (init_winsock() == false)
	{
		return false;
	}
	if (create_socket() == false)
	{
		return false;
	}

	port = 8080;
	additionalHeader = "Proxy-Authorization: Basic " + user_pass64 + "\r\n";

	URLSeparater rater(url);
	this->domain = proxy_name;
	this->uri = url;
	this->host = domain;

	if (get_IP() == false)
	{
		return false;
	}

	if (connect_to_server() == false)
	{
		return false;
	}

	just_load();
	disconnect();

	return true;
}

// 受信したデータを文字列として取得します
// 末尾にヌル文字が追加された状態となっています
const char * Http::content_as_text() const
{
	const fw::uchar * address = content.address(header.header_size());
	return fw::pointer_cast<const char *>(address);
}

// 受信したデータのバイト数を取得します
// 末尾に追加されたヌル文字の分はカウントされません
fw::uint Http::content_byte() const
{
	return content.size() - 1;
}


// サーバーから返されたHTTPヘッダーを管理するクラスのインスタンスを取得します
const HttpHeader & Http::get_header() const
{
	return header;
}

// 何もエラーが起こっていないときtrue, 何らかのエラーが既に起きているときfalseを返します
bool Http::no_error() const { return error_mode == NoErrorMode; }

bool Http::failed_to_match_version() const { return error_mode == FailedToMatchVersionMode; }
bool Http::failed_to_init_winsock() const { return error_mode == FailedToInitWinsockMode; }
bool Http::failed_to_create_socket() const { return error_mode == FailedToCreateSocketMode; }
bool Http::failed_to_find_IP() const { return error_mode == FailedToFindIPMode; }
bool Http::failed_to_connect() const { return error_mode == FailedToConnectMode; }
bool Http::failed_to_send() const { return error_mode == FailedToSendMode; }
bool Http::failed_to_receive() const { return error_mode == FailedToReceiveMode; }
bool Http::unsupported_HTTPS() const { return error_mode == UnsupportedHTTPSMode; }
bool Http::server_unsupported() const { return error_mode == ServerUnsupportedMode; }
bool Http::failed_to_find_URI() const { return error_mode == FailedToFindURIMode; }
bool Http::server_disconnected() const { return error_mode == ServerDisconnectedMode; }
bool Http::socket_error() const { return error_mode == SocketErrorMode; }


// エラーの種類を文字列として取得します
std::string Http::error_text() const
{
#define macro(name) if( error_mode == name##Mode ) return #name;
	macro(FailedToMatchVersion)
		macro(FailedToInitWinsock)
		macro(FailedToCreateSocket)
		macro(FailedToFindIP)
		macro(FailedToConnect)
		macro(FailedToSend)
		macro(FailedToReceive)
		macro(UnsupportedHTTPS)
		macro(ServerUnsupported)
		macro(FailedToFindURI)
		macro(ServerDisconnected)
		macro(SocketError)
#undef macro
		return "NoError";
}




Http::Http()
{
	error_mode = NoErrorMode;
	socketflag = false;
	connecting = false;
}

Http::~Http()
{
	disconnect();
	if (socketflag) closesocket(socket);
}


void Http::just_load()
{
	request();
	
	receive();

	if (no_error() == false)
	{
		return;
	}

	if (header.response_is_2XX() == false)
	{
		error_mode = ServerUnsupportedMode;
		return;
	}
}

void Http::disconnect()
{
	if (connecting) shutdown(socket, SD_BOTH);
	connecting = false;
}












bool Http::init_winsock()
{
	/*
	** Winsockの初期化は、このアプリケーション上で一度だけ実行される
	*/

	static bool did = false;
	if (did == true)
	{
		return true;
	}

	const char lobyte = 2;
	const char hibyte = 0;
	WSADATA wsd;
	bool wsdres = WSAStartup(MAKEWORD(lobyte, hibyte), &wsd) == 0;

	if (wsdres)
	{
		bool match = LOBYTE(wsd.wVersion) == lobyte && HIBYTE(wsd.wVersion) == hibyte;
		if (match == false)
		{
			error_mode = FailedToMatchVersionMode;
			return false;
		}
	}
	else
	{
		error_mode = FailedToInitWinsockMode;
		return false;
	}

	did = true;
	atexit(cleanup);	// プログラムが正常終了した際にcleanup関数を呼び出すよう設定

	return true;
}

bool Http::get_IP()
{
	hostinfo.sin_family = AF_INET;
	hostinfo.sin_port = htons(short(port));
	ULONG & ref = hostinfo.sin_addr.S_un.S_addr;
	ref = inet_addr(domain.c_str());
	if (ref == INADDR_NONE)
	{
		HOSTENT * hostent = gethostbyname(domain.c_str());
		if (hostent == NULL)
		{
			error_mode = FailedToFindIPMode;
			return false;
		}
		ref = *fw::pointer_cast<fw::uint *>(hostent->h_addr_list[0]);
	}

	return true;
}

bool Http::create_socket()
{
	if (socketflag) return true;

	socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (socket == INVALID_SOCKET)
	{
		error_mode = FailedToCreateSocketMode;
		return false;
	}
	socketflag = true;

	return true;
}

bool Http::connect_to_server()
{
	connecting = false;
	if (::connect(socket, fw::pointer_cast<sockaddr *>(&hostinfo), sizeof(hostinfo)) == SOCKET_ERROR)
	{
		error_mode = FailedToConnectMode;
		return false;
	}
	connecting = true;
	return true;
}

bool Http::request()
{
	std::string request
		=
		fw::cnct() <<
		"GET " << uri << " HTTP/1.1" << "\r\n" <<
		"Host: " << host << "\r\n" <<
		"Content-Length: 0" << "\r\n" <<
		"Connection: close" << "\r\n" <<
		additionalHeader <<=
		"\r\n";

	printf("\n");
	printf(request.c_str());
	printf("\n");

	if (::send(socket, request.c_str(), request.length(), 0) == SOCKET_ERROR)
	{
		this->error_mode = FailedToSendMode;
		return false;
	}

	return true;
}


void Http::receive()
{
#if 0
	テザリングの場合ヘッダの直後にヌル文字が入り、
	その後 0D 0A が続く
#endif

	const int onetime_size = 65536 * 2;
	fw::uint total = 0;

	while (true)
	{
		content.addsize(onetime_size);

		int received_byte = recv(socket, fw::pointer_cast<char *>(content.address(total)), onetime_size - 1, 0);
		total += received_byte;
		content[total] = '\0';

		if (received_byte == SOCKET_ERROR)
		{
			error_mode = SocketErrorMode;
			return;
		}
		if (received_byte == 0)
		{
			error_mode = ServerDisconnectedMode;
			return;
		}

		if (header.avarable())
		{
			if (judge_finish_or_not(total, received_byte, onetime_size-1))
			{
				adjust_content(total);
				return;
			}
		}
		else
		{
			look_for_header();
		}		
	}
}


bool Http::judge_finish_or_not(fw::uint total, fw::uint received_byte, fw::uint prepared_size)
{
	if (header.useful_content_length())
	{
		if (total >= header.header_size() + header.content_length())
		{
			printf("\nfinished recv cause reached to Content-Length\n");
			return true;
		}
	}
	else
	{
		if (received_byte < prepared_size)
		{
			// 用意したバッファよりも格納されたデータの方が少なかった
			// つまりこれ以上受信すべきデータは無い
			printf("\nfinished recv cause recv buffer is empty\n");
			return true;
		}
	}

	return false;
}

void Http::adjust_content(fw::uint size)
{
	content.setsize(size);
}

void Http::look_for_header()
{
	const char * endpoint = strstr(fw::pointer_cast<const char *>(content.head()), "\r\n\r\n");
	
	if (endpoint == NULL)
	{
		return;
	}

	endpoint += strlen("\r\n\r\n");
	fw::uint header_size = endpoint - fw::pointer_cast<const char *>(content.head());

	header.init(content.head(), header_size);
}



void Http::cleanup(void)
{
	WSACleanup();
}


