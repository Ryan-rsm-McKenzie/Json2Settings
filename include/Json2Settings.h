#pragma once

#include <cstdarg>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <CppCoreCheck\Warnings.h>
#pragma warning(disable: ALL_CPPCORECHECK_WARNINGS)
#include "json.hpp"
#pragma warning(default: ALL_CPPCORECHECK_WARNINGS)


namespace Json2Settings
{
	using json = nlohmann::json;

	using boolean_t = json::boolean_t;
	using integer_t = json::number_integer_t;
	using unsigned_t = json::number_unsigned_t;
	using float_t = json::number_float_t;
	using string_t = json::string_t;
	using char_t = string_t::value_type;

	static_assert(std::is_same<std::string, string_t>::value);


	namespace Impl
	{
		class VArgFormatter
		{
		public:
			VArgFormatter(const char_t* a_format, ...) :
				_buf()
			{
				std::va_list args;
				va_start(args, a_format);
				do_format(a_format, args);
				va_end(args);
			}

			VArgFormatter(const char_t* a_format, std::va_list a_args) :
				_buf()
			{
				do_format(a_format, a_args);
			}

			inline void operator()(const char_t* a_format, ...)
			{
				std::va_list args;
				va_start(args, a_format);
				do_format(a_format, args);
				va_end(args);
			}

			inline void operator()(const char_t* a_format, std::va_list a_args)
			{
				do_format(a_format, a_args);
			}

			[[nodiscard]] inline string_t str() const
			{
				return c_str();
			}

			[[nodiscard]] inline const char_t* c_str() const
			{
				return _buf.data();
			}

		private:
			inline void do_format(const char_t* a_format, std::va_list a_args)
			{
				std::va_list argsCopy;
				va_copy(argsCopy, a_args);

				_buf.resize(static_cast<std::size_t>(std::vsnprintf(0, 0, a_format, a_args)) + 1);
				std::vsnprintf(_buf.data(), _buf.size(), a_format, argsCopy);

				va_end(argsCopy);
			}

			std::vector<char_t> _buf;
		};


		[[nodiscard]] inline string_t format(const char* a_format, ...)
		{
			std::va_list args;
			va_start(args, a_format);
			string_t str(VArgFormatter(a_format, args).str());
			va_end(args);
			return str;
		}
	}


	class ISetting;


	inline std::vector<ISetting*>& get_settings()
	{
		static std::vector<ISetting*> settings;
		return settings;
	}


	class ISetting
	{
	public:
		ISetting() = delete;
		ISetting(string_t a_key) : _key(std::move(a_key)) { get_settings().push_back(this); }
		virtual ~ISetting() { auto& set = get_settings(); auto it = std::find(set.begin(), set.end(), this); if (it != set.end()) set.erase(it); }

		inline void assign(boolean_t a_val) { assign_impl(a_val); }
		inline void assign(integer_t a_val) { assign_impl(a_val); }
		inline void assign(unsigned_t a_val) { assign_impl(a_val); }
		inline void assign(float_t a_val) { assign_impl(a_val); }
		inline void assign(const char_t* a_val) { assign_impl(string_t(a_val)); }
		inline void assign(const string_t& a_val) { assign_impl(string_t(a_val)); }
		inline void assign(string_t&& a_val) { assign_impl(std::move(a_val)); }
		inline void assign(const json& a_val) { assign_impl(a_val); }
		inline void assign(json&& a_val) { assign_impl(a_val); }

		inline ISetting& operator=(boolean_t a_val) { assign(a_val); return *this; }
		inline ISetting& operator=(integer_t a_val) { assign(a_val); return *this; }
		inline ISetting& operator=(unsigned_t a_val) { assign(a_val); return *this; }
		inline ISetting& operator=(float_t a_val) { assign(a_val); return *this; }
		inline ISetting& operator=(const char_t* a_val) { assign(a_val); return *this; }
		inline ISetting& operator=(const string_t& a_val) { assign(a_val); return *this; }
		inline ISetting& operator=(string_t&& a_val) { assign(std::move(a_val)); return *this; }
		inline ISetting& operator=(const json& a_val) { assign(a_val); return *this; }
		inline ISetting& operator=(json&& a_val) { assign(std::move(a_val)); return *this; }

		[[nodiscard]] inline string_t dump() const { return dump_impl(); }
		[[nodiscard]] inline string_t to_string() const { return to_string_impl(); }
		[[nodiscard]] inline const string_t& key() const { return _key; }

	protected:
		virtual void assign_impl([[maybe_unused]] boolean_t a_val) {}
		virtual void assign_impl([[maybe_unused]] integer_t a_val) {}
		virtual void assign_impl([[maybe_unused]] unsigned_t a_val) {}
		virtual void assign_impl([[maybe_unused]] float_t a_val) {}
		virtual void assign_impl([[maybe_unused]] string_t&& a_val) {}
		virtual void assign_impl([[maybe_unused]] const json& a_val) {}

		[[nodiscard]] virtual string_t dump_impl() const { return Impl::format("%s: %s", key().c_str(), to_string().c_str()); }
		[[nodiscard]] virtual string_t to_string_impl() const = 0;

	private:
		string_t _key;
	};


	class bSetting : public ISetting
	{
	public:
		using value_type = boolean_t;

		bSetting() = delete;
		bSetting(string_t a_key, value_type a_value) : ISetting(std::move(a_key)), _value(std::move(a_value)) {}
		virtual ~bSetting() = default;

		[[nodiscard]] inline value_type& operator*() { return _value; }
		[[nodiscard]] inline const value_type& operator*() const { return _value; }

	protected:
		virtual void assign_impl(boolean_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(integer_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(unsigned_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(float_t a_val) override { _value = static_cast<value_type>(a_val); }

		[[nodiscard]] virtual string_t to_string_impl() const override { return _value ? "True" : "False"; }

	private:
		value_type _value;
	};


	class iSetting : public ISetting
	{
	public:
		using value_type = integer_t;

		iSetting() = delete;
		iSetting(string_t a_key, value_type a_value) : ISetting(std::move(a_key)), _value(std::move(a_value)) {}
		virtual ~iSetting() = default;

		[[nodiscard]] inline value_type& operator*() { return _value; }
		[[nodiscard]] inline const value_type& operator*() const { return _value; }

	protected:
		virtual void assign_impl(boolean_t a_val) override { _value = a_val ? 1 : 0; }
		virtual void assign_impl(integer_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(unsigned_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(float_t a_val) override { _value = static_cast<value_type>(a_val); }

		[[nodiscard]] virtual string_t to_string_impl() const override { return std::to_string(_value); }

	private:
		value_type _value;
	};


	class uSetting : public ISetting
	{
	public:
		using value_type = unsigned_t;

		uSetting() = delete;
		uSetting(string_t a_key, value_type a_value) : ISetting(std::move(a_key)), _value(std::move(a_value)) {}
		virtual ~uSetting() = default;

		[[nodiscard]] inline value_type& operator*() { return _value; }
		[[nodiscard]] inline const value_type& operator*() const { return _value; }

	protected:
		virtual void assign_impl(boolean_t a_val) override { _value = a_val ? 1 : 0; }
		virtual void assign_impl(integer_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(unsigned_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(float_t a_val) override { _value = static_cast<value_type>(a_val); }

		[[nodiscard]] virtual string_t to_string_impl() const override { return std::to_string(_value); }

	private:
		value_type _value;
	};


	class fSetting : public ISetting
	{
	public:
		using value_type = float_t;

		fSetting() = delete;
		fSetting(string_t a_key, value_type a_value) : ISetting(std::move(a_key)), _value(std::move(a_value)) {}
		virtual ~fSetting() = default;

		[[nodiscard]] inline value_type& operator*() { return _value; }
		[[nodiscard]] inline const value_type& operator*() const { return _value; }

	protected:
		virtual void assign_impl(boolean_t a_val) override { _value = a_val ? 1.0 : 0.0; }
		virtual void assign_impl(integer_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(unsigned_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(float_t a_val) override { _value = static_cast<value_type>(a_val); }

		[[nodiscard]] virtual string_t to_string_impl() const override { return std::to_string(_value); }

	private:
		value_type _value;
	};


	class sSetting : public ISetting
	{
	public:
		using value_type = string_t;

		sSetting() = delete;
		sSetting(string_t a_key, value_type a_value) : ISetting(std::move(a_key)), _value(std::move(a_value)) {}
		virtual ~sSetting() = default;

		[[nodiscard]] inline value_type& operator*() { return _value; }
		[[nodiscard]] inline const value_type& operator*() const { return _value; }

		[[nodiscard]] inline value_type* operator->() { return std::addressof(_value); }
		[[nodiscard]] inline const value_type* operator->() const { return std::addressof(_value); }

	protected:
		virtual void assign_impl(string_t&& a_val) override { _value = std::move(a_val); }

		virtual string_t to_string_impl() const override { return _value; }

	private:
		value_type _value;
	};


	template <class> class aSetting;


	template <>
	class aSetting<string_t> : public ISetting
	{
	public:
		using value_type = string_t;
		using container_type = std::vector<value_type>;


		aSetting() = delete;

		aSetting(string_t a_key, std::initializer_list<value_type> a_init = {}) :
			ISetting(std::move(a_key)),
			_container(std::move(a_init))
		{}

		virtual ~aSetting() = default;

		[[nodiscard]] container_type& operator*()
		{
			return _container;
		}

		[[nodiscard]] const container_type& operator*() const
		{
			return _container;
		}

		[[nodiscard]] container_type* operator->()
		{
			return std::addressof(_container);
		}

		[[nodiscard]] const container_type* operator->() const
		{
			return std::addressof(_container);
		}

	protected:
		virtual void assign_impl(const json& a_val) override
		{
			_container.clear();
			for (auto& val : a_val) {
				_container.push_back(val.get<value_type>());
			}
		}

		[[nodiscard]] virtual string_t dump_impl() const override
		{
			string_t dmp(key());
			dmp += ':';
			for (auto& it : _container) {
				dmp += "\n\t";
				dmp += it;
			}
			return dmp;
		}

		[[nodiscard]] virtual string_t to_string_impl() const override
		{
			string_t str;
			bool skip = true;
			for (auto& it : _container) {
				if (!skip) {
					str += '\n';
				}
				str += it;
				skip = false;
			}
			return str;
		}

	private:
		container_type _container;
	};


	inline std::pair<string_t, bool> load_settings(std::filesystem::path a_fileName, bool a_dumpParse = false, bool a_suppressWarnings = false)
	{
		using value_t = json::value_t;

		std::pair<string_t, bool> result("", false);

		std::basic_ifstream<char_t> inFile(a_fileName);
		if (!inFile.is_open()) {
			result.first += "Failed to open .json file!\n";
			result.second = false;
			return result;
		}

		json j;
		try {
			inFile >> j;
			if (a_dumpParse) {
				result.first += '\n';
				result.first += j.dump(4);
			}

			json::iterator it;
			for (auto& setting : get_settings()) {
				it = j.find(setting->key());

				if (it == j.end()) {
					if (!a_suppressWarnings) {
						result.first += "Failed to find (";
						result.first += setting->key();
						result.first += ") within .json!\n";
					}
					continue;
				}

				switch (it->type()) {
				case value_t::array:
					setting->assign(it.value());
					break;
				case value_t::string:
					setting->assign(it.value().get<string_t>());
					break;
				case value_t::boolean:
					setting->assign(it.value().get<boolean_t>());
					break;
				case value_t::number_integer:
					setting->assign(it.value().get<integer_t>());
					break;
				case value_t::number_unsigned:
					setting->assign(it.value().get<unsigned_t>());
					break;
				case value_t::number_float:
					setting->assign(it.value().get<float_t>());
					break;
				default:
					result.first += "Parsed value is of invalid type(";
					result.first += j.type_name();
					result.first += ")!\n";
				}
			}
		} catch (std::exception& e) {
			result.first += "Failed to parse .json file!\n";
			result.first += e.what();
			result.first += '\n';
			result.second = false;
			return result;
		}

		result.second = true;
		return result;
	}


	[[nodiscard]] inline string_t dump_settings()
	{
		string_t dmp;
		bool skip = true;
		for (auto& setting : get_settings()) {
			if (!skip) {
				dmp += '\n';
			}
			dmp += setting->dump();
			skip = false;
		}
		return dmp;
	}
}


namespace std
{
	inline std::string to_string(const Json2Settings::ISetting& a_value)
	{
		return a_value.to_string();
	}
}
