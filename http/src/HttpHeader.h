#ifndef HttpHeader_h_
#define HttpHeader_h_

#include <FantomWaves.h>

/*
** HTTPヘッダーを解析・管理します
** サーバーからのレスポンスヘッダーに限ります
*/
class HttpHeader
{
public:
	// このクラスを、header_dataに指定したHTTPヘッダーで初期化します
	// 内部でHTTPヘッダーの解析が行われます
	// エラーが無ければ、avarable() は true を返すようになります
	// header_dataの内容がヌル文字で終わる文字列形式であってはいけません
	// header_dataの内容は\r\n\r\nで終わっている必要があります
	void init(fw::uchar * header_data, fw::uint header_size_);

	// このクラスが現在使い物になるものなのかどうかを返します
	// 有効ならtrue, 無効ならfalseを返します
	bool avarable() const;

	// HTTPレスポンスのヘッダー部分のサイズをバイト数で返します
	// つまり\r\n\r\n部分も含めたサイズを返します
	fw::uint header_size() const;

	// サーバーからのレスポンス(3ケタの数値)が2から始まるものであればtrueを、
	// そうでなければfalseを返します
	bool response_is_2XX() const;

	// サーバーからのレスポンス(3ケタの数値)が3から始まるものであればtrueを、
	// そうでなければfalseを返します
	bool response_is_3XX() const;

	// Content-Length フィールドが存在すればtrueを、存在しなければfalseを返します
	bool useful_content_length() const;

	// Content-Length の値を返します
	fw::uint content_length() const;

	// Connection フィールドが存在すればtrueを、存在しなければfalseを返します
	bool useful_connection() const;

	// Connection の値を返します
	bool connection_is_KeepAlive() const;

	// Location フィールドが存在すればtrueを、存在しなければfalseを返します
	bool useful_location() const;

	// Location の値を返します
	const std::string & location() const;

	// この関数を呼ぶと、データが破棄されます
	// avarable() は false を返すようになります
	void init();






	HttpHeader();

private:
	fw::vuchar buffer;
	mutable std::map<std::string, fw::vstring> table;
	char Version[4];
	char Code[4];
	bool avarable_;
	fw::uint header_size_;


	// この関数に渡す文字列情報は行の羅列である必要がある
	// つまり、最後は\r\n\0となっている必要がある
	void analyze(const char * str);

	// bufferの内容がヌル文字で終わる文字列形式であってはならない
	// bufferの内容は\r\n\r\nで終わっている必要がある
	// analyze()は内部でbufferの後ろから2番目の文字をヌル文字で上書きする
	void analyze();
};

#endif