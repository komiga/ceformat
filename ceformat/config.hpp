/**
@file config.hpp
@brief Configuration.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef CEFORMAT_CONFIG_HPP_
#define CEFORMAT_CONFIG_HPP_

#include <cstddef>

namespace ceformat {

/**
	@addtogroup config
	@{
*/

#ifdef DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/**
	Allocator for auxiliary specializations.
	Defaults to @c std::allocator.
*/
#define CEFORMAT_AUX_ALLOCATOR

/**
	String type.
	Defaults to @c aux::basic_string<char>.

	@note ceformat requires this type to satisfy all of the stdlib
	requirements for @c std::string. It is assumed this stores UTF-8-
	encoded data.

	@sa @ref string
*/
#define CEFORMAT_CONFIG_STRING_TYPE

/**
	String stream type.
	Defaults to @c aux::basic_ostringstream<char>.

	@note ceformat requires this type to satisfy all of the stdlib
	requirements for @c std::ostringstream.

	@sa @ref string
*/
#define CEFORMAT_CONFIG_OSTRINGSTREAM_TYPE

#else // -

#ifndef CEFORMAT_AUX_ALLOCATOR
	#define CEFORMAT_AUX_ALLOCATOR \
		std::allocator
#endif

#ifndef CEFORMAT_CONFIG_STRING_TYPE
	#define CEFORMAT_CONFIG_STRING_TYPE \
		aux::basic_string<char>
#endif

#ifndef CEFORMAT_CONFIG_OSTRINGSTREAM_TYPE
	#define CEFORMAT_CONFIG_OSTRINGSTREAM_TYPE \
		aux::basic_ostringstream<char>
#endif

#endif // DOXYGEN_CONSISTS_SOLELY_OF_UNICORNS_AND_CONFETTI

/** @} */ // end of doc-group config

} // namespace ceformat

#endif // CEFORMAT_CONFIG_HPP_
