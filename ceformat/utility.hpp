/**
@file utility.hpp
@brief Utilities.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#pragma once

#include <ceformat/config.hpp>

namespace ceformat {
namespace utility {

/**
	@addtogroup utility
	@{
*/

/**
	Return lesser of two values.

	@returns The lowest value in the set {@a x, @a y}.
	@tparam T Value type.
	@param x, y Values.
*/
template<
	typename T
>
constexpr T
min_ce(
	T const x,
	T const y
) noexcept {
	return
	x < y
		? x
	: y
	;
}

/** @cond INTERNAL */
namespace {
constexpr std::size_t
digit_count_impl(
	std::size_t const value,
	std::size_t const count
) noexcept {
	return
	0 != value
		? digit_count_impl(
			value / 10,
			count + 1u
		)
	: count
	;
}
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Count number of digits in value.

	@note Input value of @c 0 is counted.

	@returns The number of digits in @a value.
	@param value Value.
*/
constexpr std::size_t
digit_count(
	std::size_t const value
) noexcept {
	return
	digit_count_impl(
		value,
		static_cast<unsigned>(0u == value)
	);
}

/** @} */ // end of doc-group utility

} // namespace utility
} // namespace ceformat
