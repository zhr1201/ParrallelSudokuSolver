#include "string-utils.h"

namespace sudoku {

void trim(std::string &str) {
	size_t endpos = str.find_last_not_of(" \t");
	size_t startpos = str.find_first_not_of(" \t");
	if (std::string::npos != endpos) {
		str = str.substr( 0, endpos+1 );
		str = str.substr( startpos );
	}
	else {
		str="";
		return;
	}
	startpos = str.find_first_not_of(" \t");
	if ( std::string::npos != startpos ) {
		str = str.substr( startpos );
	}
}

std::vector<std::string> split(const std::string &s, const std::string &delim) {
	std::vector<std::string> elems;
	int last = 0, next = 0; 
	while((next = s.find(delim, last)) != std::string::npos) { 
		elems.push_back(s.substr(last, next-last)); 
		last = next + 1; 
	}
	elems.push_back(s.substr(last));
	return elems;
}

std::vector<std::string> split_by_white(const std::string &s) {
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while(!ss.eof()) {
		ss >> item;
		elems.push_back(item);
	}
	return elems;
}

std::string join(const std::vector<std::string> str_vec, const std::string &delim) {
	std::string result;
	int vec_size = str_vec.size();
	if(vec_size == 0) {
		return "";
	}else if(vec_size == 1) {
		return str_vec[0];
	}
	for(int i=0; i<str_vec.size() - 1; ++i) {
		result += str_vec[i] + delim;
	}
	result += str_vec.back();
	return result;
}

std::string& replace_str(std::string& str, const std::string& to_replaced, const std::string& newchars) {
	for (std::string::size_type pos(0); pos != std::string::npos; pos += newchars.length()) {
		pos = str.find(to_replaced, pos);
		if (pos != std::string::npos)
			str.replace(pos, to_replaced.length(), newchars);
		else
			break;
	}
	return str;
}
}
