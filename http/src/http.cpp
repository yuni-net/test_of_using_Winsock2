#include "http.h"
#include "HttpHeader.h"

http::http()
{
	mode = NoErrorMode;
	socketflag = false;
	connecting = false;
}

http & http::connect()
{
	if (url.find("https://") != std::string::npos)
	{
		mode = UnsupportedHTTPSMode;
		return *this;
	}

	if (InitWinsock() == false) return *this;
	if (CreateSocket() == false) return *this;

	{
		std::string WithoutScheme;
		if (url.find("http://") == std::string::npos) WithoutScheme = url;
		else WithoutScheme = url.substr(7);

		fw::uint slash = WithoutScheme.find('/');
		domain = WithoutScheme.substr(0, slash);
		uri = WithoutScheme.substr(slash);

		// debug
		printf("domain:%s\nuri:%s\n", domain.c_str(), uri.c_str());
	}

	if (GetIP()) ConnectToServer();

	return *this;
}

http & http::get()
{
	if (NoError())
	{
	//	if(HeadRequest() )
	//	{
	//		if(ReceiveHead() )
	//		{
				content.setsize(110000);
				if (GetRequest())
				{
					ReceiveAll();
				}
	//		}
	//	}
	}

	return *this;
}

http & http::disconnect()
{
	if (connecting) shutdown(socket, SD_BOTH);
	connecting = false;
	return *this;
}

bool http::autoget()
{
	return connect().get().disconnect().NoError();
}


bool http::NoError() const { return mode == NoErrorMode; }
bool http::FailedToMatchVersion() const { return mode == FailedToMatchVersionMode; }
bool http::FailedToInitWinsock() const { return mode == FailedToInitWinsockMode; }
bool http::FailedToCreateSocket() const { return mode == FailedToCreateSocketMode; }
bool http::FailedToFindIP() const { return mode == FailedToFindIPMode; }
bool http::FailedToConnect() const { return mode == FailedToConnectMode; }
bool http::FailedToSend() const { return mode == FailedToSendMode; }
bool http::FailedToReceive() const { return mode == FailedToReceiveMode; }
bool http::UnsupportedHTTPS() const { return mode == UnsupportedHTTPSMode; }
bool http::ServerUnsupported() const { return mode == ServerUnsupportedMode; }
bool http::FailedToFindURI() const { return mode == FailedToFindURIMode; }
bool http::ServerDisconnected() const { return mode == ServerDisconnectedMode; }
bool http::SocketError() const { return mode == SocketErrorMode; }

const char * http::errortext_p() const
{
#define macro(name) if( name () ) return #name;
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
std::string http::errortext() const
{
	return std::string(errortext_p());
}

void http::output_to_file(const char * name) const
{
	fw::bfile file(name);
	file.open_to_write();
	file.write(content.head(), content.size());
}
void http::output_to_file(const std::string & name) const
{
	output_to_file(name.c_str());
}

const char * http::text() const
{
	// There is a problem.
	if (content.last() == '\0') return fw::pointer_cast<const char *>(content.head());
	fw::vuchar another(content);
	another.add('\0');
	return fw::pointer_cast<const char *>(another.head());
}
const char * http::text()
{
	// debug
	printf("size:%d\n", content.size());

	if (content.last() != '\0')
	{
		content.add('\0');
	}

	return fw::pointer_cast<const char *>(content.head());
}



bool http::InitWinsock()
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
			mode = FailedToMatchVersionMode;
			return false;
		}
	}
	else
	{
		mode = FailedToInitWinsockMode;
		return false;
	}

	did = true;
	atexit(cleanup);	// プログラムが正常終了した際にcleanup関数を呼び出すよう設定

	return true;
}

bool http::GetIP()
{
	hostinfo;
	hostinfo.sin_family = AF_INET;
	hostinfo.sin_port = htons(short(80));
	ULONG & ref = hostinfo.sin_addr.S_un.S_addr;
	ref = inet_addr(domain.c_str());
	if (ref == INADDR_NONE)
	{
		HOSTENT * hostent = gethostbyname(domain.c_str());
		if (hostent == NULL)
		{
			mode = FailedToFindIPMode;
			return false;
		}
		ref = *fw::pointer_cast<fw::uint *>(hostent->h_addr_list[0]);
	}

	return true;
}

bool http::CreateSocket()
{
	if (socketflag) return true;

	socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (socket == INVALID_SOCKET)
	{
		mode = FailedToCreateSocketMode;
		return false;
	}
	socketflag = true;

	return true;
}

bool http::ConnectToServer()
{
	connecting = false;
	if (::connect(socket, fw::pointer_cast<sockaddr *>(&hostinfo), sizeof(hostinfo)) == SOCKET_ERROR)
	{
		mode = FailedToConnectMode;
		return false;
	}
	connecting = true;
	return true;
}

bool http::request(const char * mode)
{
	std::string request
		=
		fw::cnct() <<
		mode << " " << uri << " HTTP/1.1" << "\r\n" <<
		"Host: " << domain << "\r\n" <<
		"Content-Length: 0" << "\r\n" <<
		"Connection: Keep-Alive" << "\r\n" <<=
		"\r\n";

	if (::send(socket, request.c_str(), request.length(), 0) == SOCKET_ERROR)
	{
		this->mode = FailedToSendMode;
		return false;
	}

	return true;
}

bool http::HeadRequest()
{
	return this->request("HEAD");
}

bool http::ReceiveHead()
{
	
	HttpHeader header;
	header.buffer.setsize(1024);
	fw::uint HeadSize = receive(header.buffer, true);
	if (NoError() == false)
	{
		printf("ReceiveHead\n");
		return false;
	}
	header.buffer.setsize(HeadSize);
	header.analyze();

	if (header.ResponseIs2XX() == false && header.ResponseIs3XX() == false)
	{
		mode = ServerUnsupportedMode;
		return false;
	}

	if (header.ResponseIs3XX())
	{
		if (header.UsefulLocation())
		{
			uri = header.Location();
			if (HeadRequest())
			{
				return ReceiveHead();
			}

			return false;
		}
		else
		{
			mode = FailedToFindURIMode;
			return false;
		}
	}

	if (header.UsefulConnection())
	{
		if (header.ConnectionIsKeepAlive() == false) ConnectToServer();
	}
	else
	{
		ConnectToServer();
	}

	if (header.UsefulContentLength())
	{
		ContentSize = header.ContentLength();
		content.setsize(HeadSize + 256 + ContentSize);
	}
	else
	{
		content.setsize(1024);
	}

	return true;
}

bool http::GetRequest()
{
	return this->request("GET");
}

bool http::ReceiveAll()
{
	struct for_StatusIs2XX
	{
		bool operator() (const fw::vuchar & content)
		{
			struct for_findstr
			{
				fw::uint operator() (const char * str)
				{
					//	for(uint i=0;i<strlen(str)-1;++i)
					for (fw::uint i = 0; i + 1<strlen(str); ++i)
					{
						if (str[i] == '\r' && str[i + 1] == '\n') return i;
					}
					return 0;
				}
			}findstr;

			fw::vector<char> firstline;
			const char * strp = fw::pointer_cast<const char *>(content.head());
			fw::uint flsize = findstr(strp);
			firstline.setsize(flsize + 1);
			memcpy(firstline.head(), strp, flsize);
			firstline.last() = '\0';
			char version[9], status[4], message[33];
			sscanf(firstline.head(), "%s %s %s", version, status, message);

			return status[0] == '2';
		}
	}StatusIs2XX;

	fw::uint done = receive(content, false);

	if (NoError() == false)
	{
		return false;
	}

	if (StatusIs2XX(content) == false)
	{
		mode = ServerUnsupportedMode;
		return false;
	}

	if (done > content.size())
	{
		printf("over capacity\n");	// debug
		content.addsize(content.size());

		if (GetRequest()) return ReceiveAll();
		else return false;
	}

	content.setsize(done);

	return NoError();
}

fw::uint http::receive(fw::vuchar & buffer, bool HeadFlag)
{
	fw::uint HeadSize = 0;
	fw::uint total = 0;
	while (true)
	{
		int nBytesRecv = recv(socket, fw::pointer_cast<char *>(buffer.head()), buffer.size(), 0);
		total += nBytesRecv;
		if (nBytesRecv == SOCKET_ERROR)
		{
			mode = SocketErrorMode;
			return 0;
		}
		if (nBytesRecv == 0)
		{
			mode = ServerDisconnectedMode;
			return 0;
		}

		if (HeadFlag)
		{
			fw::uchar * last4 = buffer.address(total - 4);
			if
				(
				last4[0] == '\r' &&
				last4[1] == '\n' &&
				last4[2] == '\r' &&
				last4[3] == '\n'
				)
			{
				break;
			}
		}
		else
		{
			if (HeadSize == 0)
			{
				content[total] = '\0';
				const char * endpoint = strstr(fw::pointer_cast<const char *>(content.head()), "\r\n\r\n");
				if (endpoint != NULL)
				{
					endpoint += strlen("\r\n\r\n");
					HeadSize = endpoint - fw::pointer_cast<const char *>(content.head());
				}
			}

			if (HeadSize != 0)
			{
				if ((total - HeadSize) >= ContentSize) break;
			}
		}
	}


	return total;
}


http::~http()
{
	disconnect();
	if (socketflag) closesocket(socket);
}

void http::cleanup(void)
{
	WSACleanup();
}
