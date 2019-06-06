#pragma once

#include <cstdint>  // int32_t
#include <exception>  // exception
#include <fstream>  // ifstream
#include <string>  // string
#include <vector>  // vector

#include "json.hpp"  // json


class ISetting
{
public:
	using json = nlohmann::json;


	ISetting() = delete;
	ISetting(std::string a_key, bool a_consoleOK);
	virtual ~ISetting();

	virtual void		assign(bool a_val);
	virtual void		assign(int a_val);
	virtual void		assign(float a_val);
	virtual void		assign(const char* a_val);
	virtual void		assign(std::string a_val);
	virtual void		assign(json& a_val);
	virtual void		dump() = 0;
	virtual std::string	getValueAsString() const = 0;
	const std::string&	key() const;

protected:
	std::string	_key;
};


inline ISetting::~ISetting()
{}


inline void ISetting::assign(bool a_val)
{}


inline void ISetting::assign(int a_val)
{}


inline void ISetting::assign(float a_val)
{}


inline void ISetting::assign(const char* a_val)
{}


inline void ISetting::assign(std::string a_val)
{}


inline void ISetting::assign(json& a_val)
{}


inline const std::string& ISetting::key() const
{
	return _key;
}


class bSetting : public ISetting
{
public:
	bSetting() = delete;
	bSetting(std::string a_key, bool a_consoleOK, bool a_value);
	virtual ~bSetting();

	virtual void		assign(bool a_val) override;
	virtual void		assign(int a_val) override;
	virtual void		assign(float a_val) override;
	virtual void		dump() override;
	virtual std::string	getValueAsString() const override;

	operator bool() const;

protected:
	bool _value;
};


inline bSetting::bSetting(std::string a_key, bool a_consoleOK, bool a_value) :
	ISetting(a_key, a_consoleOK),
	_value(a_value)
{}


inline bSetting::~bSetting()
{}


inline void bSetting::assign(bool a_val)
{
	_value = a_val;
}


inline void bSetting::assign(int a_val)
{
	_value = a_val ? true : false;
}


inline void bSetting::assign(float a_val)
{
	_value = (int)a_val ? true : false;
}


inline void bSetting::dump()
{
	_DMESSAGE("%s: %s", _key.c_str(), _value ? "True" : "False");
}


inline std::string bSetting::getValueAsString() const
{
	return _value ? "True" : "False";
}


inline bSetting::operator bool() const
{
	return _value;
}


class iSetting : public ISetting
{
public:
	iSetting() = delete;
	iSetting(std::string a_key, bool a_consoleOK, std::int32_t a_value);
	virtual ~iSetting();

	virtual void		assign(int a_val) override;
	virtual void		assign(float a_val) override;
	virtual void		dump() override;
	virtual std::string	getValueAsString() const override;

	operator std::int32_t() const;

protected:
	std::int32_t _value;
};


inline iSetting::iSetting(std::string a_key, bool a_consoleOK, std::int32_t a_value) :
	ISetting(a_key, a_consoleOK),
	_value(a_value)
{}


inline iSetting:: ~iSetting()
{}


inline void iSetting::assign(int a_val)
{
	_value = a_val;
}


inline void iSetting::assign(float a_val)
{
	_value = (int)a_val;
}


inline void iSetting::dump()
{
	_DMESSAGE("%s: %i", _key.c_str(), _value);
}


inline std::string iSetting::getValueAsString() const
{
	return std::to_string(_value);
}


inline iSetting::operator std::int32_t() const
{
	return _value;
}


class fSetting : public ISetting
{
public:
	fSetting() = delete;
	fSetting(std::string a_key, bool a_consoleOK, float a_value);
	virtual ~fSetting();

	virtual void		assign(int a_val) override;
	virtual void		assign(float a_val) override;
	virtual void		dump() override;
	virtual std::string	getValueAsString() const override;

	operator float() const;

protected:
	float _value;
};


inline fSetting::fSetting(std::string a_key, bool a_consoleOK, float a_value) :
	ISetting(a_key, a_consoleOK),
	_value(a_value)
{}


inline fSetting::~fSetting()
{}


inline void fSetting::assign(int a_val)
{
	_value = (float)a_val;
}


inline void fSetting::assign(float a_val)
{
	_value = a_val;
}


inline void fSetting::dump()
{
	_DMESSAGE("%s: %f", _key.c_str(), _value);
}


inline std::string fSetting::getValueAsString() const
{
	return std::to_string(_value);
}


inline fSetting::operator float() const
{
	return _value;
}


class sSetting :
	public ISetting,
	public std::string
{
public:
	sSetting() = delete;
	sSetting(std::string a_key, bool a_consoleOK, std::string a_value);
	virtual ~sSetting();

	virtual void		assign(std::string a_val) override;
	virtual void		assign(const char* a_val) override;
	virtual void		dump() override;
	virtual std::string	getValueAsString() const override;
};


inline sSetting::sSetting(std::string a_key, bool a_consoleOK, std::string a_value) :
	ISetting(a_key, a_consoleOK),
	std::string(a_value)
{}


inline sSetting::~sSetting()
{}


inline void sSetting::assign(std::string a_val)
{
	std::string::operator=(a_val);
}


inline void sSetting::assign(const char* a_val)
{
	std::string::operator=(a_val);
}


inline void sSetting::dump()
{
	_DMESSAGE("%s: %s", _key.c_str(), c_str());
}


inline std::string sSetting::getValueAsString() const
{
	return data();
}


template <class> class aSetting;


template <>
class aSetting<std::string> :
	public ISetting,
	public std::vector<std::string>
{
private:
	using json = nlohmann::json;

public:
	aSetting() = delete;
	aSetting(std::string a_key, bool a_consoleOK, std::initializer_list<std::string> a_list = {}) :
		ISetting(a_key, a_consoleOK),
		std::vector<std::string>(a_list)
	{}

	virtual ~aSetting()
	{}

	virtual void assign(json& a_val) override
	{
		clear();
		for (auto& val : a_val) {
			emplace_back(val.get<std::string>());
		}
	}

	virtual void dump() override
	{
		_DMESSAGE("%s:", _key.c_str());
		for (auto it = begin(); it != end(); ++it) {
			_DMESSAGE("\t%s", it->c_str());
		}
	}

	virtual std::string	getValueAsString() const override
	{
		std::string str = _key + ":";
		for (auto it = begin(); it != end(); ++it) {
			str += "\t" + *it + "\n";
		}
		return str;
	}
};


namespace Json2Settings
{
	class Settings
	{
		friend class ISetting;
	public:
		Settings() = delete;

		static bool			loadSettings(bool a_dumpParse = false);
		static ISetting*	set(std::string& a_key, int a_val);
		static void			dump();
		static void			setFileName(const char* a_fileName);

	protected:
		static std::vector<ISetting*>&						settings();
		static std::unordered_map<std::string, ISetting*>&	consoleSettings();

	private:
		inline static std::string _fileName = "";
	};


	inline bool Settings::loadSettings(bool a_dumpParse)
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
			for (auto& setting : settings()) {
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


	inline ISetting* Settings::set(std::string& a_key, int a_val)
	{
		auto it = consoleSettings().find(a_key);
		if (it != consoleSettings().end()) {
			it->second->assign(a_val);
			return it->second;
		} else {
			return 0;
		}
	}


	inline void Settings::dump()
	{
		_DMESSAGE("=== SETTINGS DUMP BEGIN ===");
		for (auto& setting : settings()) {
			setting->dump();
		}
		_DMESSAGE("=== SETTINGS DUMP END ===");
	}


	inline void Settings::setFileName(const char* a_fileName)
	{
		_fileName = a_fileName;
	}


	inline std::vector<ISetting*>& Settings::settings()
	{
		static std::vector<ISetting*> settings;
		return settings;
	}


	inline std::unordered_map<std::string, ISetting*>& Settings::consoleSettings()
	{
		static std::unordered_map<std::string, ISetting*> consoleSettings;
		return consoleSettings;
	}
}


inline ISetting::ISetting(std::string a_key, bool a_consoleOK) :
	_key(a_key)
{
	using Json2Settings::Settings;

	Settings::settings().push_back(this);
	if (a_consoleOK) {
		Settings::consoleSettings().insert(std::make_pair(_key, this));
	}
}
