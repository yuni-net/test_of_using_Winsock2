#ifndef http_h_
#define http_h_

#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib")

#include <string>
#include <FantomWaves.h>
#include "HttpHeader.h"

/*
** v3.0.0
** HTTP�ʐM���s���A�f�[�^���擾���܂�
** GET���N�G�X�g�݂̂ŁA�f�[�^���擾�������_�Őڑ���؂�܂�
** Transfer-Encoding: chunked �ɂ͌��ݖ��Ή�
*/
class Http
{
public:
	/* @brief �T�[�o�[�ƒʐM���s���A�f�[�^����M���܂�
	** @param
	**  url:�T�[�o�[��IP�A�h���X��h���C�������łȂ��AURL�S�̂��w�肵�Ă�������
	** @return ����:true, ���s:false
	*/
	bool load(const std::string url);

	/* @brief "�v���L�V"�T�[�o�[�ƒʐM���s���A�f�[�^����M���܂�
	** @param
	**  url:�T�[�o�[��IP�A�h���X��h���C�������łȂ��AURL�S�̂��w�肵�Ă�������
	**  proxy_name:�v���L�V�T�[�o�[�̃h���C����IP�A�h���X���w�肵�܂�(ex."192.168.1.1")
	**  user_pass64:"���[�U�[��:�p�X���[�h"�̕������64�G���R�[�h������������w�肵�܂�
	** @return ����:true, ���s:false
	*/
	bool load(
		const std::string url,
		const std::string proxy_name,
		const std::string user_pass64);

	// ��M�����f�[�^�𕶎���Ƃ��Ď擾���܂�
	// �����Ƀk���������ǉ����ꂽ��ԂƂȂ��Ă��܂�
	const char * content_as_text() const;

	// ��M�����f�[�^�̃o�C�g�����擾���܂�
	// �����ɒǉ����ꂽ�k�������̕��̓J�E���g����܂���
	fw::uint content_byte() const;

	// �T�[�o�[����Ԃ��ꂽHTTP�w�b�_�[���Ǘ�����N���X�̃C���X�^���X���擾���܂�
	const HttpHeader & get_header() const;

	// �����G���[���N�����Ă��Ȃ��Ƃ�true, ���炩�̃G���[�����ɋN���Ă���Ƃ�false��Ԃ��܂�
	bool no_error() const;

	// �G���[�̎�ނ���肷��ۂɎg�p���܂� **********************
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

	// �G���[�̎�ނ𕶎���Ƃ��Ď擾���܂�
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

	ErrorMode error_mode;	// �G���[�����������ꍇ�͂����ɃG���[���[�h���L�^����܂�

	// �ʐM�ɂ��擾�����f�[�^�������Ɋi�[����܂�
	// �����ɂ̓k���������ǉ�����܂�
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




	// send����recv���܂�
	void just_load();

	// �T�[�o�[�Ƃ̐ڑ���؂�܂�
	void disconnect();




	// winsock2�����������܂�
	bool init_winsock();

	// �h���C��������IP�A�h���X���擾���܂�
	bool get_IP();

	// �\�P�b�g���쐬���܂�
	bool create_socket();

	// ���ۂɃT�[�o�[�ɃA�N�Z�X���܂�
	bool connect_to_server();

	// send���܂�
	bool request();

//	bool ReceiveAll();

	// recv���܂�
	void receive();


	// ��M�o�b�t�@����f�[�^�����o�������𑱍s���ׂ����ǂ������f���܂�
	// ���s���ׂ��łȂ����f�[�^���o���͑S�Ċ��������Ȃ�true���A
	// ���s���ׂ����f�[�^���o���͂܂��������Ă��Ȃ��Ȃ�false��Ԃ��܂�
	bool judge_finish_or_not(fw::uint total, fw::uint received_byte, fw::uint prepared_size);

	// ��M�����f�[�^���L�^����o�b�t�@�[�̃T�C�Y���A��M�����f�[�^�̃T�C�Y�Ƃ҂����蓯���ɂ��܂�
	// �o�b�t�@�[�͂�����x�]�T�������č쐬����Ă��邩��ł�
	void adjust_content(fw::uint size);

	// ��M�f�[�^����w�b�_�[������T���܂�
	// �w�b�_�[�����̑S�Ă���M�ς݂Ȃ�w�b�_�[�Ǘ��N���X�Ƀf�[�^��n���܂�
	// �����Ă��̏�ŉ�͂��s���܂�
	void look_for_header();




	// Winsock2�̃N���[���i�b�v���s���܂�
	static void cleanup(void);




};


#endif