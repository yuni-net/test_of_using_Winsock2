#include "URLSeparater.h"
#include <FantomWaves.h>

URLSeparater::URLSeparater()
{
	// Nothing
}

URLSeparater::URLSeparater(std::string url)
{
	separate(url);
}

void URLSeparater::separate(std::string url)
{
	std::string WithoutScheme;
	if (url.find("http://") == std::string::npos)
	{
		WithoutScheme = url;
	}
	else
	{
		WithoutScheme = url.substr(strlen("http://"));
	}

	fw::uint slash = WithoutScheme.find('/');
	domain_ = WithoutScheme.substr(0, slash);
	uri_ = WithoutScheme.substr(slash);
}

std::string URLSeparater::domain()
{
	return domain_;
}
std::string URLSeparater::uri()
{
	return uri_;
}
