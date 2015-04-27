#ifndef URLSeparater_h_
#define URLSeparater_h_

#include <string>

/*
** URL���A�h���C����uri�ɐ؂蕪���܂�
*/
class URLSeparater
{
public:
	URLSeparater();
	URLSeparater(std::string url);

	void separate(std::string url);

	std::string domain();
	std::string uri();



private:
	std::string domain_;
	std::string uri_;
};

#endif