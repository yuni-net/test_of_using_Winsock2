#include "HttpHeader.h"

HttpHeader::HttpHeader()
	:
	strContentLength("Content-Length"),
	strConnection("Connection"),
	strLocation("Location")
{
	// Nothing
}

char * HttpHeader::bufhead()
{
	return fw::pointer_cast<char *>(buffer.head());
}

fw::uint HttpHeader::bufsize() const
{
	return buffer.size();
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
		printf("\n\nfound header field:%s\n", lines[i].substr(0, colon).c_str());
		printf("target str to find beg:%s\n", lines[i].substr(colon + 1).c_str());
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
	analyze(bufhead());
}

bool HttpHeader::ResponseIs2XX() const { return Code[0] == '2'; }
bool HttpHeader::ResponseIs3XX() const { return Code[0] == '3'; }

bool HttpHeader::UsefulContentLength() const { return table[strContentLength].size()>0; }
fw::uint HttpHeader::ContentLength() const { return atoi(table[strContentLength][0].c_str()); }

bool HttpHeader::UsefulConnection() const { return table[strConnection].size()>0; }
bool HttpHeader::ConnectionIsKeepAlive() const { return table[strConnection][0] == "Keep-Alive"; }

bool HttpHeader::UsefulLocation() const { return table[strLocation].size()>0; }
const std::string & HttpHeader::Location() const { return table[strLocation][0]; }
