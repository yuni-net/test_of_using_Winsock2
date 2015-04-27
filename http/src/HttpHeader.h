#ifndef HttpHeader_h_
#define HttpHeader_h_

#include <FantomWaves.h>

/*
** HTTP�w�b�_�[����́E�Ǘ����܂�
** �T�[�o�[����̃��X�|���X�w�b�_�[�Ɍ���܂�
*/
class HttpHeader
{
public:
	// ���̃N���X���Aheader_data�Ɏw�肵��HTTP�w�b�_�[�ŏ��������܂�
	// ������HTTP�w�b�_�[�̉�͂��s���܂�
	// �G���[��������΁Aavarable() �� true ��Ԃ��悤�ɂȂ�܂�
	// header_data�̓��e���k�������ŏI��镶����`���ł����Ă͂����܂���
	// header_data�̓��e��\r\n\r\n�ŏI����Ă���K�v������܂�
	void init(fw::uchar * header_data, fw::uint header_size_);

	// ���̃N���X�����ݎg�����ɂȂ���̂Ȃ̂��ǂ�����Ԃ��܂�
	// �L���Ȃ�true, �����Ȃ�false��Ԃ��܂�
	bool avarable() const;

	// HTTP���X�|���X�̃w�b�_�[�����̃T�C�Y���o�C�g���ŕԂ��܂�
	// �܂�\r\n\r\n�������܂߂��T�C�Y��Ԃ��܂�
	fw::uint header_size() const;

	// �T�[�o�[����̃��X�|���X(3�P�^�̐��l)��2����n�܂���̂ł����true���A
	// �����łȂ����false��Ԃ��܂�
	bool response_is_2XX() const;

	// �T�[�o�[����̃��X�|���X(3�P�^�̐��l)��3����n�܂���̂ł����true���A
	// �����łȂ����false��Ԃ��܂�
	bool response_is_3XX() const;

	// Content-Length �t�B�[���h�����݂����true���A���݂��Ȃ����false��Ԃ��܂�
	bool useful_content_length() const;

	// Content-Length �̒l��Ԃ��܂�
	fw::uint content_length() const;

	// Connection �t�B�[���h�����݂����true���A���݂��Ȃ����false��Ԃ��܂�
	bool useful_connection() const;

	// Connection �̒l��Ԃ��܂�
	bool connection_is_KeepAlive() const;

	// Location �t�B�[���h�����݂����true���A���݂��Ȃ����false��Ԃ��܂�
	bool useful_location() const;

	// Location �̒l��Ԃ��܂�
	const std::string & location() const;

	// ���̊֐����ĂԂƁA�f�[�^���j������܂�
	// avarable() �� false ��Ԃ��悤�ɂȂ�܂�
	void init();






	HttpHeader();

private:
	fw::vuchar buffer;
	mutable std::map<std::string, fw::vstring> table;
	char Version[4];
	char Code[4];
	bool avarable_;
	fw::uint header_size_;


	// ���̊֐��ɓn����������͍s�̗���ł���K�v������
	// �܂�A�Ō��\r\n\0�ƂȂ��Ă���K�v������
	void analyze(const char * str);

	// buffer�̓��e���k�������ŏI��镶����`���ł����Ă͂Ȃ�Ȃ�
	// buffer�̓��e��\r\n\r\n�ŏI����Ă���K�v������
	// analyze()�͓�����buffer�̌�납��2�Ԗڂ̕������k�������ŏ㏑������
	void analyze();
};

#endif