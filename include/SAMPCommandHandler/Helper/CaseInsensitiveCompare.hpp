#pragma once

#include SAMPCOMMANDHANDLER_PCH

namespace samp_cmd
{

struct IgnoreCaseLess
{
	// case-independent (ci) compare_less binary function
	struct CompareIgnoreCase
	{
		bool operator() (unsigned char c1_, unsigned char c2_) const
		{
			return tolower(c1_) < tolower(c2_); 
		}
	};
	bool operator() (std::string_view const& s1_, std::string_view const& s2_) const
	{
		return std::lexicographical_compare(s1_.begin(), s1_.end(), s2_.begin(), s2_.end(), CompareIgnoreCase{});
	}
	bool operator() (std::string const& s1_, std::string const& s2_) const
	{
		return std::lexicographical_compare(s1_.begin(), s1_.end(), s2_.begin(), s2_.end(), CompareIgnoreCase{});
	}
	bool operator() (std::string const& s1_, std::string_view const& s2_) const
	{
		return std::lexicographical_compare(s1_.begin(), s1_.end(), s2_.begin(), s2_.end(), CompareIgnoreCase{});
	}
	bool operator() (std::string_view const& s1_, std::string const& s2_) const
	{
		return std::lexicographical_compare(s1_.begin(), s1_.end(), s2_.begin(), s2_.end(), CompareIgnoreCase{});
	}

	// IMPORTANT:
	using is_transparent = int;
};

}