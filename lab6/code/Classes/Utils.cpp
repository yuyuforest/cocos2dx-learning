#include "Utils.h"
#include "json\writer.h"
#include <sstream>

std::string serializeRapidjsonDocument(const rapidjson::Document &doc) {
  rapidjson::StringBuffer buffer;
  buffer.Clear();
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  return std::string(buffer.GetString());
}

std::string intToStr(int i) {
	std::stringstream ss;
	ss << i;
	std::string str;
	ss >> str;
	return str;
}