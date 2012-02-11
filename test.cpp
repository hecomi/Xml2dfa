#include <iostream>

#include "xml2dfa.hpp"

int main(int argc, char const* argv[])
{
	XML2dfa x2d("input.xml");
	x2d.mk_gram_voca();

	return 0;
}
