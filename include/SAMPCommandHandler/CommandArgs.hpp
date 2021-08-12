#pragma once

#include SAMPCOMMANDHANDLER_PCH

#include <SAMPCommandHandler/Helper/CaseInsensitiveCompare.hpp>

namespace samp_cmd
{

constexpr size_t MaxCommandArgs = 256;

template <typename T>
struct Opt
{
	using Type = T;

	Opt(T& ref_)
		: ref(ref_)
	{
	}

	Opt(T& ref_, T alternative_)
		: ref(ref_), alternative(alternative_)
	{
	}

	T& ref;
	std::optional<T> alternative = std::nullopt;
};

namespace detail
{

template <typename T, typename = void>
struct IsOptArgT : std::false_type {};
template<class T>
struct IsOptArgT< Opt<T> > : std::true_type {};

template<typename T>
constexpr bool IsOptArg = IsOptArgT< T >::value;

}

/// <summary>
/// 	A class used to efficiently parse and access command arguments.
/// </summary>
/// <remarks>
/// 	This class does not allocate any dynamic memory to hold the arguments buffer.
/// 	It's scope has to be limited to the lifetime of the originally provided buffer.
/// </remarks>
struct CommandArgs
	: public std::array<std::string_view, MaxCommandArgs>
{
	using Super = std::array<std::string_view, MaxCommandArgs>;

	CommandArgs() = default;

	/// <summary>
	/// 	Constructs `CommandArgs` object from given arguments string_view.
	/// 	without parsing it.
	/// </summary>
	/// <param name="params_">Arguments string_view</param>
	/// <returns>Constructed CommandArgs object</returns>
	explicit CommandArgs(std::string_view params_)
		: params(params_)
	{
	}


	/// <summary>
	/// 	Constructs `CommandArgs` object from given arguments string_view.
	/// 	and parses till the end of the buffer.
	/// </summary>
	/// <param name="params_">Arguments string_view</param>
	/// <returns>Constructed CommandArgs object</returns>
	static CommandArgs parse(std::string_view params_);


	/// <summary>Tries to parse next argument.</summary>
	/// <returns>
	/// 	<c>true</c> if successfully parsed, otherwise <c>false</c>.
	/// </returns>
	bool parseNext();


	/// <summary>Returns the tail - rest of the arguments string.</summary>
	/// <returns>Rest as a string_view</returns>
	std::string_view rest() const;
	

	/// <summary>
	/// 	Returns the tail - rest of the arguments string
	/// 	without spaces at the beginning
	/// </summary>
	/// <returns>Tail as a string_view</returns>
	std::string_view tail() const;


	/// <summary>Tries to scan up to `num_` parameters.</summary>
	/// <param name="num_">Requested number of parameters (relative to numParsed() before the call)</param>
	/// <returns>Number of successfully parsed parameters.</returns>
	size_t tryParse(size_t num_);


	/// <summary>Parses arguments till the end of the string.</summary>
	/// <returns>Number of parsed parameters</returns>
	size_t parseAll();


	/// <returns>Number of parsed parameters</returns>
	size_t numParsed() const { return _numParsed; }


	/// <summary>
	/// 	Gets an optional value of the argument of index `idx_`,
	/// 	or `std::nullopt` if argument doesn't exist or couldn't be converted to requested type.
	/// </summary>
	/// <tparam name="T">Requested value type</tparam>
	/// <param name="idx_">Argument index</param>
	/// <returns>Optional argument value of specified type</returns>
	template <typename T = std::string_view>
	std::optional<T> get(size_t idx_) const = delete;


	/// <summary>
	/// 	Gets an optional int value of the argument of index `idx_`,
	/// 	or `std::nullopt` if argument doesn't exist or couldn't be converted to integer.
	/// </summary>
	/// <param name="idx_">Argument index</param>
	/// <returns>Optional argument int value</returns>
	template <>
	std::optional<int> get<int>(size_t idx_) const
	{
		if (idx_ >= this->numParsed())
			return std::nullopt;

		return string_algo::to<int>( (*this)[idx_] );
	}


	/// <summary>
	/// 	Gets an optional float value of the argument of index `idx_`,
	/// 	or `std::nullopt` if argument doesn't exist or couldn't be converted to float.
	/// </summary>
	/// <param name="idx_">Argument index</param>
	/// <returns>Optional argument float value</returns>
	template <>
	std::optional<float> get<float>(size_t idx_) const
	{
		if (idx_ >= this->numParsed())
			return std::nullopt;
		
		return string_algo::to<float>( (*this)[idx_] );
	}


	/// <summary>
	/// 	Gets an optional string value of the argument of index `idx_`,
	/// 	or `std::nullopt` if argument doesn't exist.
	/// </summary>
	/// <param name="idx_">Argument index</param>
	/// <returns>Optional argument string value</returns>
	template <>
	std::optional<std::string> get<std::string>(size_t idx_) const
	{
		if (idx_ >= this->numParsed())
			return std::nullopt;

		return std::string((*this)[idx_]);
	}


	/// <summary>
	/// 	Gets an optional string_view value of the argument of index `idx_`,
	/// 	or `std::nullopt` if argument doesn't exist.
	/// </summary>
	/// <param name="idx_">Argument index</param>
	/// <returns>Optional argument string_view value</returns>
	template <>
	std::optional<std::string_view> get<std::string_view>(size_t idx_) const
	{
		if (idx_ >= this->numParsed())
			return std::nullopt;

		return (*this)[idx_];
	}


	/// <summary>
	/// 	Gets the value of the argument of index `idx_`,
	/// 	or alternative value (default value of type T if not specified)
	/// 	if argument doesn't exist or couldn't be converted to requested type.
	/// </summary>
	/// <tparam name="T">Requested value type</tparam>
	/// <param name="idx_">Argument index</param>
	/// <returns>Argument value of specified type or an alternative</returns>
	template <typename T = std::string_view>
	T req(size_t idx_, T alternative_ = T{}) const
	{
		return this->get(idx_).value_or(alternative_);
	}

	/// <returns>The entire arguments buffer as a std::string_view</returns>
	explicit operator std::string_view() 	const { return params; }


	/// <returns>The entire arguments buffer as a std::string</returns>
	explicit operator std::string() 		const { return std::string(params); }

	template <typename... TArgs>
	bool operator()(TArgs&&... args_)
	{
		constexpr size_t NumArgs = sizeof...(args_);
		constexpr size_t NumReqArgs = (static_cast<int>(!detail::IsOptArg<TArgs>) + ...);

		size_t parsed = this->tryParse(NumArgs);
		if (parsed < NumReqArgs)
			return false;
		
		size_t argNum = this->numParsed() - parsed;
		auto handleArg = [&](auto & arg) -> bool {
			using ArgType = std::remove_reference_t< decltype(arg) >;
			if constexpr (!detail::IsOptArg<ArgType>)
			{
				auto val = this->get<ArgType>(argNum);
				if (!val.has_value())
					return false;
				
				arg = val.value();
				++argNum;
				return true;
			}
			else
			{
				auto val = this->get< typename ArgType::Type >(argNum);
				if (!val.has_value())
				{
					if (arg.alternative.has_value())
						arg.ref = arg.alternative.value();
				}
				else {
					arg.ref = val.value();
				}

				++argNum;
				return true;
			}
		};

		return ((handleArg(args_)) && ...);
	}

	// Public variables:

	/// Holds the entire arguments buffer.
	std::string_view params;
private:

	// Private variables:

	size_t _numParsed 		= 0;
	size_t _lastParsedPos 	= 0;
};

}

