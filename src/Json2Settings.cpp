#include "Json2Settings.h"

#include <fstream>  // ifstream
#include <exception>  // exception
#include <string>  // string

#include "json.hpp"  // json


ISetting::ISetting(std::string a_key, bool a_consoleOK) :
	_key(a_key)
{
	if (!settings) {
		settings = new std::vector<ISetting*>;
	}
	settings->push_back(this);
	if (a_consoleOK) {
		if (!consoleSettings) {
			consoleSettings = new std::vector<ISetting*>;
		}
		consoleSettings->push_back(this);
	}
}


ISetting::~ISetting()
{}


void ISetting::assign(bool a_val)
{}


void ISetting::assign(int a_val)
{}


void ISetting::assign(float a_val)
{}


void ISetting::assign(const char* a_val)
{}


void ISetting::assign(std::string a_val)
{}


void ISetting::assign(json& a_val)
{}


const std::string& ISetting::key() const
{
	return _key;
}


bSetting::bSetting(std::string a_key, bool a_consoleOK, bool a_value) :
	ISetting(a_key, a_consoleOK),
	_value(a_value)
{}


bSetting::~bSetting()
{}


void bSetting::assign(bool a_val)
{
	_value = a_val;
}


void bSetting::assign(int a_val)
{
	_value = a_val ? true : false;
}


void bSetting::assign(float a_val)
{
	_value = (int)a_val ? true : false;
}


void bSetting::dump()
{
	_DMESSAGE("%s: %s", _key.c_str(), _value ? "True" : "False");
}


std::string bSetting::getValueAsString() const
{
	return _value ? "True" : "False";
}


bSetting::operator bool() const
{
	return _value;
}


iSetting::iSetting(std::string a_key, bool a_consoleOK, SInt32 a_value) :
	ISetting(a_key, a_consoleOK),
	_value(a_value)
{}


iSetting:: ~iSetting()
{}


void iSetting::assign(int a_val)
{
	_value = a_val;
}


void iSetting::assign(float a_val)
{
	_value = (int)a_val;
}


void iSetting::dump()
{
	_DMESSAGE("%s: %i", _key.c_str(), _value);
}


std::string iSetting::getValueAsString() const
{
	return std::to_string(_value);
}


iSetting::operator SInt32() const
{
	return _value;
}


fSetting::fSetting(std::string a_key, bool a_consoleOK, float a_value) :
	ISetting(a_key, a_consoleOK),
	_value(a_value)
{}


fSetting::~fSetting()
{}


void fSetting::assign(int a_val)
{
	_value = (float)a_val;
}


void fSetting::assign(float a_val)
{
	_value = a_val;
}


void fSetting::dump()
{
	_DMESSAGE("%s: %f", _key.c_str(), _value);
}


std::string fSetting::getValueAsString() const
{
	return std::to_string(_value);
}


fSetting::operator float() const
{
	return _value;
}


sSetting::sSetting(std::string a_key, bool a_consoleOK, std::string a_value) :
	ISetting(a_key, a_consoleOK),
	std::string(a_value)
{}


sSetting::~sSetting()
{}


void sSetting::assign(std::string a_val)
{
	std::string::operator=(a_val);
}


void sSetting::assign(const char* a_val)
{
	std::string::operator=(a_val);
}


void sSetting::dump()
{
	_DMESSAGE("%s: %s", _key.c_str(), c_str());
}


std::string sSetting::getValueAsString() const
{
	return data();
}


namespace Json2Settings
{
	bool Settings::loadSettings(bool a_dumpParse)
	{
		using nlohmann::json;

		std::ifstream istream(_fileName.c_str());
		if (!istream.is_open()) {
			_ERROR("[ERROR] Failed to open .json file!\n");
		}
		json j;
		try {
			istream >> j;
			if (a_dumpParse) {
				_DMESSAGE("[DEBUG] PARSE DUMP\n%s\n", j.dump(4).c_str());
			}
			json::iterator it;
			for (auto& setting : *settings) {
				it = j.find(setting->key());

				if (it == j.end()) {
					_ERROR("[ERROR] Failed to find (%s) within .json!\n", setting->key().c_str());
					continue;
				}

				switch (it->type()) {
				case json::value_t::array:
					{
						json jArr = it.value();
						setting->assign(jArr);
					}
					break;
				case json::value_t::string:
					{
						std::string str = it.value();
						setting->assign(str);
					}
					break;
				case json::value_t::boolean:
					{
						bool b = it.value();
						setting->assign(b);
					}
					break;
				case json::value_t::number_integer:
				case json::value_t::number_unsigned:
					{
						int num = it.value();
						setting->assign(num);
					}
					break;
				case json::value_t::number_float:
					{
						float num = it.value();
						setting->assign(num);
					}
					break;
				default:
					_DMESSAGE("[ERROR] Parsed value is of invalid type (%s)!\n", j.type_name());
				}
			}
		} catch (std::exception& e) {
			_ERROR("[ERROR] Failed to parse .json file!\n");
			_ERROR(e.what());
			istream.close();
			return false;
		}

		istream.close();
		return true;
	}


	ISetting* Settings::set(std::string& a_key, int a_val)
	{
		for (auto& setting : *consoleSettings) {
			if (setting->key() == a_key) {
				setting->assign(a_val);
				return setting;
			}
		}
		return 0;
	}


	void Settings::dump()
	{
		_DMESSAGE("=== SETTINGS DUMP BEGIN ===");
		for (auto& setting : *settings) {
			setting->dump();
		}
		_DMESSAGE("=== SETTINGS DUMP END ===");
	}


	void Settings::setFileName(const char* a_fileName)
	{
		_fileName = a_fileName;
	}


	void Settings::free()
	{
		delete settings;
		delete consoleSettings;
	}


	std::string Settings::_fileName = "";
}


std::vector<ISetting*>* settings = 0;
std::vector<ISetting*>* consoleSettings = 0;
