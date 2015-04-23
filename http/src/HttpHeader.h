#ifndef HttpHeader_h_
#define HttpHeader_h_

#include <FantomWaves.h>

class HttpHeader
{
public:
	fw::vuchar buffer;

	HttpHeader();

	char * bufhead();
	fw::uint bufsize() const;

	// この関数に渡す文字列情報は行の羅列である必要がある
	// つまり、最後は\r\n\0となっている必要がある
	void analyze(const char * str);

	// bufferの内容がヌル文字で終わる文字列形式であってはならない
	// bufferの内容は\r\n\r\nで終わっている必要がある
	// analyze()は内部でbufferの後ろから2番目の文字をヌル文字で上書きする
	void analyze();

	bool ResponseIs2XX() const;
	bool ResponseIs3XX() const;

	bool UsefulContentLength() const;
	fw::uint ContentLength() const;

	bool UsefulConnection() const;
	bool ConnectionIsKeepAlive() const;

	bool UsefulLocation() const;
	const std::string & Location() const;

private:
	mutable std::map<std::string, fw::vstring> table;
	char Version[4];
	char Code[4];

	const std::string strContentLength;
	const std::string strConnection;
	const std::string strLocation;
};

#endif