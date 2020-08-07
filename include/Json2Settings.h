#pragma once

#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>


namespace Json2Settings
{
	using json = nlohmann::json;

	using boolean_t = typename json::boolean_t;
	using integer_t = typename json::number_integer_t;
	using unsigned_t = typename json::number_unsigned_t;
	using float_t = typename json::number_float_t;
	using string_t = typename json::string_t;
	using char_t = typename string_t::value_type;


	static_assert(std::is_same_v<std::string, string_t>);


	class ISetting;


	inline std::vector<ISetting*>& get_settings() noexcept
	{
		static std::vector<ISetting*> settings;
		return settings;
	}


	// interface
	class ISetting
	{
	public:
		ISetting() = delete;
		inline ISetting(string_t a_key) :
			_key(std::move(a_key))
		{
			auto& settings = get_settings();
			settings.push_back(this);
		}

		virtual ~ISetting() = 0
		{
			auto& settings = get_settings();
			auto it = std::find(settings.begin(), settings.end(), this);
			if (it != settings.end()) {
				settings.erase(it);
			}
		}

		inline void assign(boolean_t a_val) { assign_impl(a_val); }
		inline void assign(integer_t a_val) { assign_impl(a_val); }
		inline void assign(unsigned_t a_val) { assign_impl(a_val); }
		inline void assign(float_t a_val) { assign_impl(a_val); }
		inline void assign(const char_t* a_val) { assign_impl(string_t(a_val)); }
		inline void assign(const string_t& a_val) { assign_impl(string_t(a_val)); }
		inline void assign(string_t&& a_val) { assign_impl(std::move(a_val)); }
		inline void assign(const json& a_val) { assign_impl(a_val); }
		inline void assign(json&& a_val) { assign_impl(a_val); }

		inline ISetting& operator=(boolean_t a_val)
		{
			assign(a_val);
			return *this;
		}

		inline ISetting& operator=(integer_t a_val)
		{
			assign(a_val);
			return *this;
		}

		inline ISetting& operator=(unsigned_t a_val)
		{
			assign(a_val);
			return *this;
		}

		inline ISetting& operator=(float_t a_val)
		{
			assign(a_val);
			return *this;
		}

		inline ISetting& operator=(const char_t* a_val)
		{
			assign(a_val);
			return *this;
		}

		inline ISetting& operator=(const string_t& a_val)
		{
			assign(a_val);
			return *this;
		}

		inline ISetting& operator=(string_t&& a_val)
		{
			assign(std::move(a_val));
			return *this;
		}

		inline ISetting& operator=(const json& a_val)
		{
			assign(a_val);
			return *this;
		}

		inline ISetting& operator=(json&& a_val)
		{
			assign(std::move(a_val));
			return *this;
		}

		[[nodiscard]] inline string_t dump() const { return dump_impl(); }
		[[nodiscard]] inline string_t to_string() const { return to_string_impl(); }
		[[nodiscard]] constexpr const string_t& key() const noexcept { return _key; }

	protected:
		virtual void assign_impl([[maybe_unused]] boolean_t a_val) {}
		virtual void assign_impl([[maybe_unused]] integer_t a_val) {}
		virtual void assign_impl([[maybe_unused]] unsigned_t a_val) {}
		virtual void assign_impl([[maybe_unused]] float_t a_val) {}
		virtual void assign_impl([[maybe_unused]] string_t&& a_val) {}
		virtual void assign_impl([[maybe_unused]] const json& a_val) {}

		[[nodiscard]] virtual string_t dump_impl() const
		{
			string_t dmp = key();
			dmp += ": ";
			dmp += to_string();
			return dmp;
		}

		[[nodiscard]] virtual string_t to_string_impl() const { return "<undefined>"; }

	private:
		string_t _key;
	};


	inline std::string to_string(const ISetting& a_value)
	{
		return a_value.to_string();
	}


	// boolean
	class bSetting : public ISetting
	{
	public:
		using value_type = boolean_t;

		bSetting() = delete;
		inline bSetting(string_t a_key, value_type a_value) :
			ISetting(std::move(a_key)),
			_value(std::move(a_value))
		{}

		virtual ~bSetting() = default;

		[[nodiscard]] constexpr value_type& operator*() noexcept { return _value; }
		[[nodiscard]] constexpr const value_type& operator*() const noexcept { return _value; }

		[[nodiscard]] constexpr value_type* operator->() noexcept { return std::addressof(_value); }
		[[nodiscard]] constexpr const value_type* operator->() const noexcept { return std::addressof(_value); }

	protected:
		virtual void assign_impl(boolean_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(integer_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(unsigned_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(float_t a_val) override { _value = static_cast<value_type>(a_val); }

		[[nodiscard]] virtual string_t to_string_impl() const override { return _value ? "True" : "False"; }

	private:
		value_type _value;
	};


	// integer
	class iSetting : public ISetting
	{
	public:
		using value_type = integer_t;

		iSetting() = delete;
		inline iSetting(string_t a_key, value_type a_value) :
			ISetting(std::move(a_key)),
			_value(std::move(a_value))
		{}

		virtual ~iSetting() = default;

		[[nodiscard]] constexpr value_type& operator*() noexcept { return _value; }
		[[nodiscard]] constexpr const value_type& operator*() const noexcept { return _value; }

		[[nodiscard]] constexpr value_type* operator->() noexcept { return std::addressof(_value); }
		[[nodiscard]] constexpr const value_type* operator->() const noexcept { return std::addressof(_value); }

	protected:
		virtual void assign_impl(boolean_t a_val) override { _value = a_val ? 1 : 0; }
		virtual void assign_impl(integer_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(unsigned_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(float_t a_val) override { _value = static_cast<value_type>(a_val); }

		[[nodiscard]] virtual string_t to_string_impl() const override
		{
			using std::to_string;
			return to_string(_value);
		}

	private:
		value_type _value;
	};


	// unsigned integer
	class uSetting : public ISetting
	{
	public:
		using value_type = unsigned_t;

		uSetting() = delete;
		inline uSetting(string_t a_key, value_type a_value) :
			ISetting(std::move(a_key)),
			_value(std::move(a_value))
		{}

		virtual ~uSetting() = default;

		[[nodiscard]] constexpr value_type& operator*() noexcept { return _value; }
		[[nodiscard]] constexpr const value_type& operator*() const noexcept { return _value; }

		[[nodiscard]] constexpr value_type* operator->() noexcept { return std::addressof(_value); }
		[[nodiscard]] constexpr const value_type* operator->() const noexcept { return std::addressof(_value); }

	protected:
		virtual void assign_impl(boolean_t a_val) override { _value = a_val ? 1 : 0; }
		virtual void assign_impl(integer_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(unsigned_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(float_t a_val) override { _value = static_cast<value_type>(a_val); }

		[[nodiscard]] virtual string_t to_string_impl() const override
		{
			using std::to_string;
			return to_string(_value);
		}

	private:
		value_type _value;
	};


	// float
	class fSetting : public ISetting
	{
	public:
		using value_type = float_t;

		fSetting() = delete;
		inline fSetting(string_t a_key, value_type a_value) :
			ISetting(std::move(a_key)),
			_value(std::move(a_value))
		{}

		virtual ~fSetting() = default;

		[[nodiscard]] constexpr value_type& operator*() noexcept { return _value; }
		[[nodiscard]] constexpr const value_type& operator*() const noexcept { return _value; }

		[[nodiscard]] constexpr value_type* operator->() noexcept { return std::addressof(_value); }
		[[nodiscard]] constexpr const value_type* operator->() const noexcept { return std::addressof(_value); }

	protected:
		virtual void assign_impl(boolean_t a_val) override { _value = a_val ? 1.0 : 0.0; }
		virtual void assign_impl(integer_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(unsigned_t a_val) override { _value = static_cast<value_type>(a_val); }
		virtual void assign_impl(float_t a_val) override { _value = static_cast<value_type>(a_val); }

		[[nodiscard]] virtual string_t to_string_impl() const override
		{
			using std::to_string;
			return to_string(_value);
		}

	private:
		value_type _value;
	};


	// string
	class sSetting : public ISetting
	{
	public:
		using value_type = string_t;

		sSetting() = delete;
		inline sSetting(string_t a_key, value_type a_value) :
			ISetting(std::move(a_key)),
			_value(std::move(a_value))
		{}

		virtual ~sSetting() = default;

		[[nodiscard]] constexpr value_type& operator*() noexcept { return _value; }
		[[nodiscard]] constexpr const value_type& operator*() const noexcept { return _value; }

		[[nodiscard]] constexpr value_type* operator->() noexcept { return std::addressof(_value); }
		[[nodiscard]] constexpr const value_type* operator->() const noexcept { return std::addressof(_value); }

	protected:
		virtual void assign_impl(string_t&& a_val) override { _value = std::move(a_val); }

		virtual string_t to_string_impl() const override { return _value; }

	private:
		value_type _value;
	};


	namespace Impl
	{
		template <class T, class = void>
		struct is_get_defined :
			std::false_type
		{};

		template <class T>
		struct is_get_defined<T, std::void_t<decltype(&json::get<T>)>> :
			std::true_type
		{};

		template <class T>
		inline constexpr bool is_get_defined_v = is_get_defined<T>::value;
	}


	template <class, class = void>
	class aSetting;


	// array
	template <class T>
	class aSetting<T, std::enable_if_t<Impl::is_get_defined_v<T>>> : public ISetting
	{
	public:
		using value_type = T;
		using container_type = std::vector<value_type>;

		aSetting() = delete;

		inline aSetting(string_t a_key) :
			ISetting(std::move(a_key)),
			_container()
		{}

		inline aSetting(string_t a_key, std::initializer_list<value_type> a_init) :
			ISetting(std::move(a_key)),
			_container(std::move(a_init))
		{}

		virtual ~aSetting() = default;

		[[nodiscard]] constexpr container_type& operator*() noexcept { return _container; }
		[[nodiscard]] constexpr const container_type& operator*() const noexcept { return _container; }

		[[nodiscard]] constexpr container_type* operator->() noexcept { return std::addressof(_container); }
		[[nodiscard]] constexpr const container_type* operator->() const noexcept { return std::addressof(_container); }

	protected:
		virtual void assign_impl(const json& a_val) override
		{
			_container.clear();
			for (auto& val : a_val) {
				_container.push_back(val.get<value_type>());
			}
		}

	private:
		container_type _container;
	};


	template <class, class = void>
	class oSetting;


	// object
	template <class T>
	class oSetting<T, std::enable_if_t<Impl::is_get_defined_v<T>>> : public ISetting
	{
	public:
		using value_type = T;

		oSetting() = delete;

		template <std::enable_if_t<std::is_default_constructible_v<value_type>, int> = 0>
		inline oSetting(string_t a_key) :
			ISetting(std::move(a_key)),
			_value{}
		{}

		template <class... Args, std::enable_if_t<std::is_constructible_v<value_type, Args...>, int> = 0>
		inline explicit oSetting(string_t a_key, [[maybe_unused]] std::in_place_t, Args&&... a_args) :
			ISetting(std::move(a_key)),
			_value(std::forward<Args>(a_args)...)
		{}

		virtual ~oSetting() = default;

		[[nodiscard]] constexpr value_type& operator*() noexcept { return _value; }
		[[nodiscard]] constexpr const value_type& operator*() const noexcept { return _value; }

		[[nodiscard]] constexpr value_type* operator->() noexcept { return std::addressof(_value); }
		[[nodiscard]] constexpr const value_type* operator->() const noexcept { return std::addressof(_value); }

	protected:
		virtual void assign_impl(const json& a_val) override { a_val.get_to(_value); }

	private:
		value_type _value;
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
					setting->assign(*it);
					break;
				case value_t::string:
					setting->assign(it->get<string_t>());
					break;
				case value_t::boolean:
					setting->assign(it->get<boolean_t>());
					break;
				case value_t::number_integer:
					setting->assign(it->get<integer_t>());
					break;
				case value_t::number_unsigned:
					setting->assign(it->get<unsigned_t>());
					break;
				case value_t::number_float:
					setting->assign(it->get<float_t>());
					break;
				case value_t::object:
					setting->assign(*it);
					break;
				default:
					result.first += "Parsed value is of invalid type(";
					result.first += j.type_name();
					result.first += ")!\n";
				}
			}
		} catch (const std::exception& e) {
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
		const auto& settings = get_settings();
		if (!settings.empty()) {
			dmp += '\n';
			for (auto& setting : settings) {
				dmp += setting->dump();
			}
		}
		return dmp;
	}
}
