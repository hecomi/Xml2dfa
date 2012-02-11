#ifndef INCLUDE_INTERPRETER_HPP
#define INCLUDE_INTERPRETER_HPP

#include <string>
#include <vector>

class XML2dfa
{
public:
	XML2dfa(const std::string& file_name);
	bool load(const std::string& file_name);
	void mk_gram_voca();
	void conv();

private:
	std::vector<std::string> command_str_vec_;
};

#endif // INCLUDE_INTERPRETER_HPP
