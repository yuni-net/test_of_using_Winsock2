#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <FantomWaves.h>
#include "src\http.h"

#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

int main()
{
	Http me;
//	me.url = "http://www25.atpages.jp/monoqloreator/index.html";
//	me.url = "http://dixq.net/forum/index.php";
//	std::string url = "localhost/index.html";
//	me.url = "http://www.nicovideo.jp/";
	std::string url = "http://nlab.itmedia.co.jp/nl/articles/1504/10/news022.html";

	std::string proxy_name("192.168.120.20");
//	std::string proxy_name("nsmain");
	std::string userPass64("bmhzMzAwNzA6YjE5OTQxMDA1");

	if (me.load(url))
	{
		printf("\nsucceeded!");

		std::string file_path("gotpage.txt");
		std::ofstream ofs(file_path.c_str(), std::ios::trunc);
		ofs << me.content_as_text();

		ShellExecute(
			NULL,           // �e�E�B���h�E�̃n���h��
			"edit",         // ����
			file_path.c_str(),	// ����Ώۂ̃t�@�C��
			NULL,			// ����̃p�����[�^
			NULL,			// ����̃f�B���N�g��
			SW_SHOW         // �\�����
			);
	}
	else printf("%s\n", me.error_text());

	return 0;
}