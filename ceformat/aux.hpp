/**
@file aux.hpp
@brief Auxiliary stdlib specializations.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef CEFORMAT_AUX_HPP_
#define CEFORMAT_AUX_HPP_

#include <ceformat/config.hpp>

#include <string>
#include <sstream>

namespace ceformat {
namespace aux {

/**
	@addtogroup aux
	@{
*/

/**
	@c std::basic_string<CharT, Traits>.
*/
template<
	typename CharT,
	class Traits = std::char_traits<CharT>
>
using basic_string
= std::basic_string<
	CharT, Traits,
	CEFORMAT_AUX_ALLOCATOR<CharT>
>;

/**
	@c std::basic_stringstream<CharT, Traits>.
*/
template<
	typename CharT,
	class Traits = std::char_traits<CharT>
>
using basic_ostringstream
= std::basic_ostringstream<
	CharT, Traits,
	CEFORMAT_AUX_ALLOCATOR<CharT>
>;

/** @} */ // end of doc-group aux

} // namespace aux
} // namespace ceformat

#endif // CEFORMAT_AUX_HPP_
