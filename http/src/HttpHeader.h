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

	// ���̊֐��ɓn����������͍s�̗���ł���K�v������
	// �܂�A�Ō��\r\n\0�ƂȂ��Ă���K�v������
	void analyze(const char * str);

	// buffer�̓��e���k�������ŏI��镶����`���ł����Ă͂Ȃ�Ȃ�
	// buffer�̓��e��\r\n\r\n�ŏI����Ă���K�v������
	// analyze()�͓�����buffer�̌�납��2�Ԗڂ̕������k�������ŏ㏑������
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