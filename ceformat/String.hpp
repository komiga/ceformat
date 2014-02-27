/**
@file String.hpp
@brief String types.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef CEFORMAT_STRING_HPP_
#define CEFORMAT_STRING_HPP_

#include <ceformat/config.hpp>
#include <ceformat/aux.hpp>

namespace ceformat {

/**
	@addtogroup string
	@{
*/

/**
	Turn input into a UTF-8 encoded string literal.

	@param s_ C-string literal.
*/
#define CEFORMAT_STR_LIT(s_) u8 ## s_

/**
	String type.

	@note Contents must be UTF-8.
*/
using String = CEFORMAT_CONFIG_STRING_TYPE;

/**
	Output string stream type.
*/
using OutputStringStream = CEFORMAT_CONFIG_OSTRINGSTREAM_TYPE;

/** @} */ // end of doc-group string

} // namespace ceformat

#endif // CEFORMAT_STRING_HPP_
