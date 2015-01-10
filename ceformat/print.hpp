/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Format printing.
*/

#pragma once

#include <ceformat/config.hpp>
#include <ceformat/String.hpp>
#include <ceformat/element_defs.hpp>
#include <ceformat/Format.hpp>
#include <ceformat/detail/type.hpp>

#include <type_traits>
#include <functional>
#include <iostream>

namespace ceformat {

/**
	@addtogroup print
	@{
*/

/** @cond INTERNAL */
namespace {

using ios = std::ios_base;

static constexpr ios::fmtflags const
flag_none
	= static_cast<ios::fmtflags>(0u),
type_flag_table[]{
	flag_none,	// end
	flag_none,	// esc
	flag_none,	// chr
	ios::dec,		// dec
	ios::dec,		// uns
	ios::hex,		// hex
	ios::oct,		// oct
	flag_none,		// flt; handled based on format symbol
	ios::boolalpha,	// boo
	ios::hex,		// ptr
	flag_none	// str
},
stream_flag_mask
	= ios::showpos
	| ios::showbase
	| ios::showpoint
	| ios::basefield
	| ios::floatfield
	| ios::adjustfield
	| ios::boolalpha
;

static_assert(
	static_cast<unsigned>(ElementType::NUM) ==
	std::extent<decltype(type_flag_table)>::value,
	"type_flag_table needs to be updated to match ElementType"
);

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
		std::streamsize precision;
		char fill;
	} const orig{
		stream.flags(),
		stream.width(),
		stream.precision(),
		stream.fill()
	};

	ios::fmtflags stream_flags
		= type_flag_table[static_cast<unsigned>(element.type)]

		// element flags
		| (element.has_flag(ElementFlags::alternative)
			? ios::showbase | ios::showpoint
			: flag_none
		)
		| (element.has_flag(ElementFlags::show_sign)
			? ios::showpos
			: flag_none
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

	switch ((element.fmt.string + element.end)[-1]) {
	case 'f': stream_flags |= ios::fixed; break;
	case 'e': stream_flags |= ios::scientific; break;
	case 'g':
		if (!element.has_flag(ElementFlags::alternative)) {
			stream_flags &= ~ios::showpoint;
		}
		break;
	}

	if (0u != element.width) {
		stream.width(static_cast<std::streamsize>(element.width));
	}
	if (-1 != element.precision) {
		stream.precision(static_cast<std::streamsize>(element.precision));
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
	if (-1 != element.precision) {
		stream.precision(orig.precision);
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

/** @cond INTERNAL */
//namespace {
struct FormatSentinel final {
	using lambda_type = std::function<void(std::ostream&)>;
	lambda_type lambda;
};
//} // anonymous namespace

inline std::ostream&
operator<<(
	std::ostream& stream,
	FormatSentinel const& sentinel
) {
	sentinel.lambda(stream);
	return stream;
}
/** @endcond */

/**
	Construct iostream-formattable object.

	@warning The sentinel holds a reference of the parameter pack
	to avoid copies, which can reference temporaries. Because of
	this, ensure the return value is written to a stream in a single
	"statement" -- i.e., before the ending semicolon for this call.

	@returns Object writeable to an @c std::ostream.
	@tparam format %Format.
	@tparam ...ArgP Argument pack.
	@param args Arguments.
*/
template<
	Format const& format,
	typename... ArgP
>
inline FormatSentinel
write_sentinel(
	ArgP&&... args
) {
	// FIXME: Haha, neat. Defect in Clang 3.4 construes [&args...] as
	// the GNU array range extension if the lambda is surrounded by {}
	return FormatSentinel{FormatSentinel::lambda_type(
		[&args...](
			std::ostream& stream
		) {
			ceformat::write<format>(
				stream,
				args...
			);
		}
	)};
}

/** @} */ // end of doc-group print

} // namespace ceformat
