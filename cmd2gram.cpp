#include <iostream>
#include <string>
#include <sstream>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/format.hpp>

#include "cmd2gram.hpp"
#include "str2voca.hpp"

namespace qi  = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace sw  = qi::standard_wide;

/* ------------------------------------------------------------------------- */
//  Constructor
/* ------------------------------------------------------------------------- */
cmd2gram::cmd2gram(const std::string& str, const size_t current_num)
: str_(str), current_num_(current_num)
{
	parse();
}

/* ------------------------------------------------------------------------- */
//  Parse
//    e.g. : "(a|b|c)d(e|f)g" -->  { {a,b,c}, d, {e,f}, g}
/* ------------------------------------------------------------------------- */
bool cmd2gram::parse()
{
	using sw::char_;
	using qi::_1;
	using qi::lit;

	std::string::const_iterator
		first = str_.begin(),
		last  = str_.end();

	qi::rule<
		std::string::const_iterator,
		std::string(),
		sw::space_type
	> word, words, sentence;

	std::vector<std::string> buf;

	word     = +( char_ - '(' - '|' - ')' );
	words    = -lit('(')
	            >> ( word [phx::push_back(phx::ref(buf), _1)] % '|' )
	            >> -lit("|)") [phx::push_back(phx::ref(buf), "")]
	            >> -lit(')');
	sentence = *(
	              words
	                [phx::push_back(phx::ref(container_), phx::ref(buf))]
	                [phx::clear(phx::ref(buf))]
	            )
	            >> qi::eol;

	bool r = qi::phrase_parse(first, last, sentence, sw::space);

	// check if parse finished
	if (!r || first != last) {
		return false;
	}

	return true;
}

/* ------------------------------------------------------------------------- */
//  grammar ファイル形式を出力
//  e.g.
//  	S		: NS_B WORD0 WORD1 WORD2 NS_E
//  	WORD0	: WORD0_0
//		WORD1	: WORD1_0
//		WORD1	: NOISE		# (hoge|)の場合用
//		WORD2	: WORD2_0
//		WORD2	: WORD2_1
/* ------------------------------------------------------------------------- */
std::string cmd2gram::mk_gram_str() const
{
	// 最終結果出力用
	std::stringstream ss;
	// 細分化したところ
	std::vector<std::string> words;

	// S を出力
	size_t n = current_num_;
	ss << "S\t: NS_B ";
	for (const auto& sentence : container_) {
		ss << boost::format("WORD%1% ") % n;

		size_t m = 0;
		for (const auto& x : sentence) {
			std::stringstream ws;
			if (x == "") {
				ws << boost::format("WORD%1%\t: NOISE\n") % n;
			} else {
				ws << boost::format("WORD%1%\t: WORD%1%_%2%\n") % n % m;
			}
			words.push_back( ws.str() );
			++m;
		}

		++n;
	}
	ss << "NS_E\n";

	// WORD* を出力
	for (const auto& x : words) {
		ss << x;
	}

	return ss.str();
}

/* ------------------------------------------------------------------------- */
//  voca ファイル形式を出力
/* ------------------------------------------------------------------------- */
std::string cmd2gram::mk_voca_str() const
{
	std::stringstream ss;

	size_t n = current_num_;
	for (const auto& sentence : container_) {
		size_t m = 0;
		for (const auto& x : sentence) {
			if ( x == "" ) { ++m; continue; }
			ss << boost::format("%% WORD%1%_%2%\n") % n % m;
			ss << boost::format("%1%\t%2%\n") % x % str2voca(x);
			++m;
		}
		++n;
	}

	return ss.str();
}

/* ------------------------------------------------------------------------- */
//  get the next number of WORD
//  	e.g. cmd2gram("(hoge|fuga)(piyo|)", 10)
//  	     --> added 4 words (hoge, fuga, piyo, and blank)
//  	     --> get_words_num() == 14
/* ------------------------------------------------------------------------- */
size_t cmd2gram::get_next_num() const
{
	return current_num_ + container_.size();
}
