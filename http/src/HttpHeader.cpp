#include "HttpHeader.h"

#define content_length_text "Content-Length"
#define connection_text "Connection"
#define location_text "Location"



// このクラスを、header_dataに指定したHTTPヘッダーで初期化します
// 内部でHTTPヘッダーの解析が行われます
// エラーが無ければ、avarable() は true を返すようになります
// header_dataの内容がヌル文字で終わる文字列形式であってはいけません
// header_dataの内容は\r\n\r\nで終わっている必要があります
void HttpHeader::init(fw::uchar * header_data, fw::uint header_size_)
{
	buffer.setsize(header_size_);
	memcpy(buffer.head(), header_data, header_size_);
	analyze();
	avarable_ = true;
	this->header_size_ = header_size_;
}

// このクラスが現在使い物になるものなのかどうかを返します
// 有効ならtrue, 無効ならfalseを返します
bool HttpHeader::avarable() const
{
	return avarable_;
}

// HTTPレスポンスのヘッダー部分のサイズをバイト数で返します
// つまり\r\n\r\n部分も含めたサイズを返します
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


// この関数を呼ぶと、データが破棄されます
// avarable() は false を返すようになります
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
	//content-valueの中に,が入る物があるとリストと区別できない。また、その場合正しく分割できていない

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

		fw::vstring & ref = table[lines[i].substr(0, colon)];	// テーブル(std::map)を増やしてる

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

