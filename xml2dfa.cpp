#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "xml2dfa.hpp"
#include "cmd2gram.hpp"

/* ------------------------------------------------------------------------- */
//  Constructor
/* ------------------------------------------------------------------------- */
XML2dfa::XML2dfa(const std::string& file_name)
{
	if ( !load(file_name) ) {
		std::cerr << boost::format("Error@XML2dfa::load() : %1%\n") % file_name;
	}
}

/* ------------------------------------------------------------------------- */
//  以下の形式のXMLを読み込んで command を command_str_vec_ に格納
//  	<iRemocon>
//  		<word command="(hoge|fuga)piyo" num="1" />
//  		...
//  	</iRemocon>
/* ------------------------------------------------------------------------- */
bool XML2dfa::load(const std::string& file_name)
{
	using namespace boost::property_tree;

	command_str_vec_.clear();

	ptree root;
	read_xml(file_name, root);

	for (const auto& pt : root.get_child("iRemocon")) {
		auto word = pt.second.get_optional<std::string>("<xmlattr>.word");
		if (!word) {
			return false;
		}
		command_str_vec_.push_back(word.get());
	}

	return true;
}

/* ------------------------------------------------------------------------- */
//
/* ------------------------------------------------------------------------- */
void XML2dfa::mk_gram_voca()
{
	std::string gram, voca;

	int word_cnt = 0;
	for (const auto& x : command_str_vec_) {
		cmd2gram c2g(x, word_cnt);

		gram += c2g.mk_gram_str();
		voca += c2g.mk_voca_str();

		word_cnt = c2g.get_next_num();
	}

	std::cout << "gram:\n" << gram << std::endl;
	std::cout << "voca:\n" << voca << std::endl;
}

/* ------------------------------------------------------------------------- */
//
/* ------------------------------------------------------------------------- */
void XML2dfa::conv()
{

}
