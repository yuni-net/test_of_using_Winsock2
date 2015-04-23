#include <stdio.h>
#include <stdlib.h>
#include "src\http.h"

int main()
{
	http me;
//	me.url = "http://www25.atpages.jp/monoqloreator/index.html";
//	me.url = "http://dixq.net/forum/index.php";
//	me.url = "localhost/index.html";
//	me.url = "http://www.nicovideo.jp/";
	me.url = "http://nlab.itmedia.co.jp/nl/articles/1504/10/news022.html";

	std::string proxy_name("192.168.120.20");
//	std::string proxy_name("nsmain");
	std::string userPass64("bmhzMzAwNzA6YjE5OTQxMDA1");

	if (me.connect_proxy(proxy_name, userPass64).get().disconnect().NoError())
	{
		printf("succeeded!\nresult:\n");
		if (me.content.size() > 0)
		{
			me.output_to_file("gotpage.txt");
			printf("%s\n", me.text());
		}
		else printf("none\n");
	}
	else printf("%s\n", me.errortext_p());

	return 0;
}