#include "HttpHeader.h"

#define content_length_text "Content-Length"
#define connection_text "Connection"
#define location_text "Location"



// ���̃N���X���Aheader_data�Ɏw�肵��HTTP�w�b�_�[�ŏ��������܂�
// ������HTTP�w�b�_�[�̉�͂��s���܂�
// �G���[��������΁Aavarable() �� true ��Ԃ��悤�ɂȂ�܂�
// header_data�̓��e���k�������ŏI��镶����`���ł����Ă͂����܂���
// header_data�̓��e��\r\n\r\n�ŏI����Ă���K�v������܂�
void HttpHeader::init(fw::uchar * header_data, fw::uint header_size_)
{
	buffer.setsize(header_size_);
	memcpy(buffer.head(), header_data, header_size_);
	analyze();
	avarable_ = true;
	this->header_size_ = header_size_;
}

// ���̃N���X�����ݎg�����ɂȂ���̂Ȃ̂��ǂ�����Ԃ��܂�
// �L���Ȃ�true, �����Ȃ�false��Ԃ��܂�
bool HttpHeader::avarable() const
{
	return avarable_;
}

// HTTP���X�|���X�̃w�b�_�[�����̃T�C�Y���o�C�g���ŕԂ��܂�
// �܂�\r\n\r\n�������܂߂��T�C�Y��Ԃ��܂�
fw::uint HttpHeader::header_size() const
{
	return header_size_;
}


bool HttpHeader::response_is_2XX() const { return Code[0] == '2'; }
bool HttpHeader::response_is_3XX() const { return Code[0] == '3'; }

bool HttpHeader::useful_content_length() const { return table[content_length_text].size()>0; }
fw::uint HttpHeader::content_length() const { return atoi(table[content_length_text][0].c_str()); }

bool HttpHeader::useful_connection() const { return table[connection_text].size()>0; }
bool HttpHeader::connection_is_KeepAlive() const { return table[connection_text][0] == "Keep-Alive"; }

bool HttpHeader::useful_location() const { return table[location_text].size()>0; }
const std::string & HttpHeader::location() const { return table[location_text][0]; }


// ���̊֐����ĂԂƁA�f�[�^���j������܂�
// avarable() �� false ��Ԃ��悤�ɂȂ�܂�
void HttpHeader::init()
{
	avarable_ = false;
	table.clear();
}



HttpHeader::HttpHeader()
{
	avarable_ = false;
}


void HttpHeader::analyze(const char * str)
{
	//content-value�̒���,�����镨������ƃ��X�g�Ƌ�ʂł��Ȃ��B�܂��A���̏ꍇ�����������ł��Ă��Ȃ�

	fw::slice lines;
	lines(str, "\r\n");

	for (fw::uint i = lines.size() - 1; i>1; --i)
	{
		const char first = lines[i][0];
		if (first == ' ' || first == '\t')
		{
			lines[i - 1] += lines[i];
			lines.pop();
		}
	}

	char phrase[32];
	sscanf(lines[0].c_str(), "HTTP/%s %s %s", Version, Code, phrase);

	for (fw::uint i = 1; i<lines.size(); ++i)
	{
		fw::uint colon = lines[i].find(':');
		fw::uint beg = lines[i].find_first_not_of(" \t", colon + 1);
		if (beg == std::string::npos) printf("Not Found beg\n\n");

		fw::vstring & ref = table[lines[i].substr(0, colon)];	// �e�[�u��(std::map)�𑝂₵�Ă�

		if (beg == std::string::npos) continue;
		fw::slice param;
		const std::string & params = lines[i].substr(beg);
		param(params, ",");

		for (fw::uint j = 0; j < param.size(); ++j)
		{
			ref.add(param[j].substr(param[j].find_first_not_of(" \t")));
			fw::uint spht = ref.last().find_first_of(" \t");
			ref.last() = ref.last().substr(0, spht);
		}
	}
}

void HttpHeader::analyze()
{
	buffer[buffer.size() - 2] = fw::uchar('\0');

	printf(fw::pointer_cast<const char *>(buffer.head()));

	analyze(fw::pointer_cast<const char *>(buffer.head()));
}

