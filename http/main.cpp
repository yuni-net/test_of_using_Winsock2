#include <stdio.h>
#include <stdlib.h>
#include "src\http.h"

int main()
{
	http me;
//	me.url = "http://www25.atpages.jp/monoqloreator/index.html";
	me.url = "http://dixq.net/forum/index.php";
//	me.url = "localhost/index.html";

	if (me.autoget())
	{
		printf("succeeded!\nresult:\n");
		if (me.content.size() > 0)
		{
			me.output_to_file("gotpage");
			printf("%s\n", me.text());
		}
		else printf("none\n");
	}
	else printf("%s\n", me.errortext_p());

	return 0;
}