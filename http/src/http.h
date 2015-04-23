#ifndef http_h_
#define http_h_

#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib")

#include <string>
#include <FantomWaves.h>

class http
{
public:
	std::string url;
	fw::vuchar content;


	http & connect();
	http & connect_proxy(const std::string & proxy_name, const std::string & userPass64);

	http & get();
	http & disconnect();

	bool autoget();

//	const char * text() const;
	const char * text();

	bool NoError() const;
	bool FailedToMatchVersion() const;
	bool FailedToInitWinsock() const;
	bool FailedToCreateSocket() const;
	bool FailedToFindIP() const;
	bool FailedToConnect() const;
	bool FailedToSend() const;
	bool FailedToReceive() const;
	bool UnsupportedHTTPS() const;
	bool ServerUnsupported() const;
	bool FailedToFindURI() const;
	bool ServerDisconnected() const;
	bool SocketError() const;

	const char * errortext_p() const;
	std::string errortext() const;

	void output_to_file(const char * name) const;
	void output_to_file(const std::string & name) const;






	http();
	~http();

	static void cleanup(void);
private:
	enum Mode
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
	Mode mode;

	std::string domain;
	std::string uri;
	SOCKET socket;
	SOCKADDR_IN hostinfo;
	bool socketflag;
	bool connecting;
	fw::uint ContentSize;
	short port;
	std::string additionalHeader;
	std::string host;

	bool InitWinsock();
	bool GetIP();
	bool CreateSocket();
	bool ConnectToServer();
	bool request(const char * mode);
	bool HeadRequest();
	bool ReceiveHead();
	bool GetRequest();
	bool ReceiveAll();

	fw::uint receive(fw::vuchar & buffer, bool HeadFlag);


};


#endif