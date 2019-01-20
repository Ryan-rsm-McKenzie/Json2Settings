#pragma once

#include <vector>  // vector
#include <string>  // string

#include "json.hpp"  // json


class ISetting
{
protected:
	typedef nlohmann::json json;

public:
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


extern std::vector<ISetting*>* settings;
extern std::vector<ISetting*>* consoleSettings;


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


class iSetting : public ISetting
{
public:
	iSetting() = delete;
	iSetting(std::string a_key, bool a_consoleOK, SInt32 a_value);
	virtual ~iSetting();

	virtual void		assign(int a_val) override;
	virtual void		assign(float a_val) override;
	virtual void		dump() override;
	virtual std::string	getValueAsString() const override;

	operator SInt32() const;

protected:
	SInt32 _value;
};


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


template <typename T>
class aSetting :
	public ISetting,
	public std::vector<T>
{
	virtual ~aSetting()
	{}
};


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
		for (auto& it = begin(); it != end(); ++it) {
			_DMESSAGE("\t%s", it->c_str());
		}
	}

	virtual std::string	getValueAsString() const override
	{
		std::string str = _key + ":";
		for (auto& it = begin(); it != end(); ++it) {
			str += "\t" + *it + "\n";
		}
		return str;
	}
};


namespace Json2Settings
{
	class Settings
	{
	public:
		Settings() = delete;

		static bool			loadSettings(bool a_dumpParse = false);
		static ISetting*	set(std::string& a_key, int a_val);
		static void			dump();
		static void			setFileName(const char* a_fileName);

		static void			free();

	private:
		static std::string _fileName;
	};
}
