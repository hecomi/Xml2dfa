#ifndef INCLUDE_CMD2GRAM_HPP
#define INCLUDE_CMD2GRAM_HPP

#include <string>
#include <vector>

class cmd2gram
{
typedef std::vector<std::vector<std::string>> container_type;

public:
	cmd2gram(const std::string& str, const size_t word_num);
	std::string mk_gram_str() const;
	std::string mk_voca_str() const;
	size_t get_next_num() const;

private:
	bool parse();

	const std::string str_;
	size_t current_num_;
	container_type container_;
};

#endif // INCLUDE_CMD2GRAM_HPP
