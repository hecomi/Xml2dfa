#include <iostream>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/regex.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <unicode/translit.h>
#include <mecab.h>

#include "str2voca.hpp"

namespace adaptors = boost::adaptors;
using adaptors::transformed;

namespace qi = boost::spirit::qi;
using qi::standard_wide::char_;
using qi::_1;
using qi::_val;

/* ------------------------------------------------------------------------- */
// 文字列 --> カタカナ
/* ------------------------------------------------------------------------- */
struct str2kana
{
	typedef std::string result_type;

	result_type operator() (const result_type& str) const {
		std::vector<result_type> v;
		result_type::const_iterator
			first = str.begin(),
				  last  = str.end();
		qi::parse(first, last, +(char_-',')%',', v);

		return v[8];
	}

};

/* ------------------------------------------------------------------------- */
// カタカナ --> ローマ字
/* ------------------------------------------------------------------------- */
struct kana2yomi
{
	typedef std::string result_type;

	result_type operator() (const result_type& str) const {
		UnicodeString input = str.c_str();

		// 「ン」をマーキング
		input.findAndReplace("ン", "[ン]");

		// カタカナ --> Latin 変換
		UErrorCode error = U_ZERO_ERROR;
		boost::shared_ptr<Transliterator> t(
			Transliterator::createInstance("Katakana-Latin", UTRANS_FORWARD, error)
		);
		t->transliterate(input);

		// 伸ばす音の表記変更 + マーキングしたンをNにする + 「つ」を「q」にする
		std::map<UnicodeString, UnicodeString> long_map = {
			{"\u0101","a:"},
			{"\u0113","i:"},
			{"\u012B","u:"},
			{"\u014D","e:"},
			{"\u014D","o:"},
			{"[n]", "N"},
			{"~", "q"}
		};
		for (const auto& x : long_map) {
			input.findAndReplace(x.first, x.second);
		}

		// 変換結果取得
		size_t length = input.length();
		char* result = new char[length + 1];
		input.extract(0, length, result, "utf8");

		return result;
	}
};

/* ------------------------------------------------------------------------- */
// ローマ字 --> voca 形式
/* ------------------------------------------------------------------------- */
struct yomi2voca
{
	typedef std::string result_type;

	result_type operator() (const result_type& str) const {
		std::string result(str);

		std::map<std::string, std::string> regex_map =
			{
				{"[aiueoNq]:?", "$0 "},
				{"[^aiueoNq]{1,2}", "$0 "},
				{"[^a-zN:@]", ""},
				{"\\s+", " "},
			};

		for (const auto& x : regex_map) {
			boost::regex r(x.first);
			result = boost::regex_replace(result, r, x.second, boost::format_all);
		}

		return result;
	}
};

/* ------------------------------------------------------------------------- */
// 文字列を MeCab で分解して vector に格納
/* ------------------------------------------------------------------------- */
std::vector<std::string> str2morpheme(const std::string& str)
{
	// MeCab による形態素解析
	boost::shared_ptr<MeCab::Tagger> tagger(MeCab::createTagger(""));
	const MeCab::Node* node = tagger->parseToNode(str.c_str());

	// 結果をコンテナに突っ込む
	std::vector<std::string> features;
	for (node = node->next; node->next; node = node->next) {
		features.push_back(node->feature);
	}

	return features;
}

/* ------------------------------------------------------------------------- */
// 文字列を voca 形式に変換
/* ------------------------------------------------------------------------- */
std::string str2voca(const std::string& str)
{
	auto features = str2morpheme(str);

	// 発音箇所だけ取り出す
	std::string result;
	for (
		const std::string& x
		: features | transformed(str2kana())
		           | transformed(kana2yomi())
		           | transformed(yomi2voca())
	) {
		result += x;
	}

	return result != "" ? result : "sp";
}

/* ------------------------------------------------------------------------- */
// 文字列の品詞を判断
/* ------------------------------------------------------------------------- */
bool check_part_of_speech(const std::string& str, const std::string& part_of_speech)
{
	auto features = str2morpheme(str);

	for (const auto& x : features) {
		std::string::const_iterator
			first = x.begin(),
			last  = x.end();
		std::vector<std::string> v;
		qi::parse(first, last, +(char_-',')%',', v);
		if (v[1] == part_of_speech) {
			return true;
		}
	}

	return false;
}
