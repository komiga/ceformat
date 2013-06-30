/**
@file element_defs.hpp
@brief Element definitions and utilities.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef CEFORMAT_ELEMENT_DEFS_HPP_
#define CEFORMAT_ELEMENT_DEFS_HPP_

#include <ceformat/config.hpp>

#include <utility>

namespace ceformat {

// Forward declarations
enum class ElementType : unsigned;
enum class ElementFlags : unsigned;
// get_element_type_name()
// element_flag_count()

/**
	@addtogroup format
	@{
*/

enum : std::size_t {
	/** Maximum number of elements. */
	ELEMENTS_MAX = 16u,
	/** Last element index. */
	ELEMENT_LAST = ELEMENTS_MAX - 1,
	/** Number of element flags. */
	ELEMENT_FLAG_COUNT = 4u
};

enum : char {
	/** Element definition character. */
	ELEMENT_CHAR = '%'
};

/**
	Element type.
*/
enum class ElementType : unsigned {
	end = 0u,	/**< Terminator or invalid. */
	esc,		/**< Escaped @c ELEMENT_CHAR. */
	dec,		/**< Integral with decimal base. */
	uns,		/**< Unsigned integral with decimal base. */
	hex,		/**< Integral with hexadecimal base. */
	oct,		/**< Integral with octal base. */
	flt,		/**< Floating-point. */
	boo,		/**< Boolean (boolalpha). */
	ptr,		/**< Pointer. */
	str			/**< String or object. */
};

/**
	Element flags.
*/
enum class ElementFlags : unsigned {
	show_base	= 1 << 0, /**< Show base. */
	show_sign	= 1 << 1, /**< Always show sign. */
	zero_padded	= 1 << 2, /**< Zero-pad instead of space-pad. */
	left_align	= 1 << 3, /**< Align left. */

	/** No flags. */
	none = 0x00,

	/** All flags. */
	all
		= show_base
		| show_sign
		| zero_padded
		| left_align
	,
	/**
		Permitted flags for ElementType values.
		@{
	*/
	permitted_end = none,
	permitted_esc = none,
	permitted_dec = all & ~show_base,
	permitted_uns = all & ~show_base,
	permitted_hex = all & ~show_sign,
	permitted_oct = all & ~show_sign,
	permitted_flt = all & ~show_base,
	permitted_boo = left_align,
	permitted_ptr = all & ~show_sign,
	permitted_str = left_align
	/** @} */
};

/** @cond INTERNAL */
namespace {
static constexpr unsigned const
s_flags_permitted[]{
	static_cast<unsigned>(ElementFlags::permitted_end),
	static_cast<unsigned>(ElementFlags::permitted_esc),
	static_cast<unsigned>(ElementFlags::permitted_dec),
	static_cast<unsigned>(ElementFlags::permitted_uns),
	static_cast<unsigned>(ElementFlags::permitted_hex),
	static_cast<unsigned>(ElementFlags::permitted_oct),
	static_cast<unsigned>(ElementFlags::permitted_flt),
	static_cast<unsigned>(ElementFlags::permitted_boo),
	static_cast<unsigned>(ElementFlags::permitted_ptr),
	static_cast<unsigned>(ElementFlags::permitted_str)
};

static constexpr char const
s_type_values[]{
	'\0',
	ELEMENT_CHAR,
	'd',
	'u',
	'x',
	'o',
	'f',
	'b',
	'p',
	's'
},
s_type_name_invalid[] = "INVALID",
* const s_type_names[]{
	"end",
	"esc",
	"dec",
	"uns",
	"hex",
	"oct",
	"flt",
	"boo",
	"ptr",
	"str"
};
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Get element type name.
	@returns Name of element type.
	@param type %Element type.
*/
constexpr char const*
get_element_type_name(
	ElementType const type
) noexcept {
	return
	std::extent<decltype(s_type_names)>::value > static_cast<unsigned>(type)
		? s_type_names[static_cast<unsigned>(type)]

	// invalid type
	: s_type_name_invalid
	;
}

/** @cond INTERNAL */
namespace {

constexpr unsigned
element_flag_count_impl(
	unsigned const flags,
	unsigned const count,
	unsigned const index
) noexcept {
	return
	// test flag and continue
	index < ELEMENT_FLAG_COUNT
		? element_flag_count_impl(
			flags,
			count + (0 != (flags & (1 << index))),
			index + 1u
		)

	// tested all flags
	: count
	;
}

} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Count number of element flags enabled.
	@returns Number of element flags enabled in @a flags.
	@param flags Flags.
*/
constexpr unsigned
element_flag_count(
	unsigned const flags
) noexcept {
	return
	element_flag_count_impl(flags, 0u, 0u);
}

/** @} */ // end of doc-group format

} // namespace ceformat

#endif // CEFORMAT_ELEMENT_DEFS_HPP_
