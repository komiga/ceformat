/**
@file print.hpp
@brief Format printing.

@author Tim Howard
@copyright 2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef CEFORMAT_PRINT_HPP_
#define CEFORMAT_PRINT_HPP_

#include <ceformat/config.hpp>
#include <ceformat/String.hpp>
#include <ceformat/Format.hpp>
#include <ceformat/detail/type.hpp>

namespace ceformat {

/**
	@addtogroup print
	@{
*/

/** @cond INTERNAL */
namespace {

using ios = std::ios_base;

static constexpr ios::fmtflags const
type_flag_table[]{
	static_cast<ios::fmtflags>(0u),	// end
	static_cast<ios::fmtflags>(0u),	// esc
	ios::dec,		// dec
	ios::dec,		// uns
	ios::hex,		// hex
	ios::oct,		// oct
	ios::fixed,		// flt
	ios::boolalpha,	// boo
	ios::hex,		// ptr
	static_cast<ios::fmtflags>(0u)	// str
},
stream_flag_mask
	= ios::showpos
	| ios::showbase
	| ios::showpoint
	| ios::basefield
	| ios::floatfield
	| ios::adjustfield
	| ios::fixed
	| ios::boolalpha
;

template<
	Format const& format,
	typename Arg
>
inline void
write_element(
	std::ostream& stream,
	Element const& element,
	Arg&& arg
) {
	struct {
		ios::fmtflags flags;
		std::streamsize width;
		char fill;
	} const orig{
		stream.flags(),
		stream.width(),
		stream.fill()
	};

	ios::fmtflags stream_flags
		= ios::showpoint
		| type_flag_table[static_cast<unsigned>(element.type)]

		// element flags
		| (element.has_flag(ElementFlags::show_base)
			? ios::showbase
			: static_cast<ios::fmtflags>(0u)
		)
		| (element.has_flag(ElementFlags::show_sign)
			? ios::showpos
			: static_cast<ios::fmtflags>(0u)
		)
		| (element.has_flag(ElementFlags::left_align)
			? ios::left
			: ios::internal
		)
	;
	stream.fill(
		element.has_flag(ElementFlags::zero_padded)
			? '0'
			: ' '
	);

	if (0u != element.width) {
		stream.width(static_cast<std::streamsize>(element.width));
	}

	stream.setf(
		stream_flags,
		stream_flag_mask
	);
	stream <<
		static_cast<typename detail::type_to_element<Arg>::cast>(
			std::forward<Arg>(arg)
		)
	;

	// Reset flags and width
	stream.fill(orig.fill);
	stream.flags(orig.flags);
	if (0u != element.width) {
		stream.width(orig.width);
	}
}

template<
	Format const& format
>
inline void
write_impl(
	std::ostream& stream,
	std::size_t const last_pos,
	std::size_t const element_index
) {
	Element const& element = format.elements[element_index];
	stream.write(
		format.string + last_pos,
		static_cast<std::streamsize>(element.beg - last_pos)
		+ (ElementType::esc == element.type)
	);
	if (ElementType::end != element.type) {
		write_impl<format>(
			stream,
			element.end,
			element_index + 1u
		);
	}
}

template<
	Format const& format,
	typename ArgF,
	typename... ArgP
>
inline void
write_impl(
	std::ostream& stream,
	std::size_t const last_pos,
	std::size_t const element_index,
	ArgF&& front,
	ArgP&&... args
) {
	Element const& element = format.elements[element_index];
	stream.write(
		format.string + last_pos,
		static_cast<std::streamsize>(element.beg - last_pos)
		+ (ElementType::esc == element.type)
	);
	if (ElementType::esc == element.type) {
		write_impl<format>(
			stream,
			element.end,
			element_index + 1u,
			std::forward<ArgF>(front),
			std::forward<ArgP>(args)...
		);
	} else {
		write_element<format>(
			stream,
			element,
			std::forward<ArgF>(front)
		);
		write_impl<format>(
			stream,
			element.end,
			element_index + 1u,
			std::forward<ArgP>(args)...
		);
	}
}
} // anonymous namespace
/** @endcond */ // INTERNAL

/**
	Write format to stream.

	@tparam format %Format.
	@tparam ...ArgP Argument pack.
	@param stream Stream to write to.
	@param args Arguments.
*/
template<
	Format const& format,
	typename... ArgP
>
void
write(
	std::ostream& stream,
	ArgP&&... args
) {
	static_assert(
		sizeof...(ArgP) == format.literal_count,
		"arguments do not match format"
	);
	static_assert(
		detail::type_check<format, ArgP...>(),
		"type of argument does not match element in format"
	);

	write_impl<format>(
		stream,
		0u,
		0u,
		std::forward<ArgP>(args)...
	);
}

/**
	Write format to string.

	@returns Formatted string.
	@tparam format %Format.
	@tparam ...ArgP Argument pack.
	@param args Arguments.
*/
template<
	Format const& format,
	typename... ArgP
>
inline String
print(
	ArgP&&... args
) {
	OutputStringStream stream;
	// Ensure stream does not flush after every output operation
	stream.flags(stream.flags() & ~ios::unitbuf);
	write<format>(
		stream,
		std::forward<ArgP>(args)...
	);
	return stream.str();
}

/** @} */ // end of doc-group print

} // namespace ceformat

#endif // CEFORMAT_PRINT_HPP_
