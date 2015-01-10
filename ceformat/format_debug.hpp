/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Format debugging utilities.
*/

#pragma once

#include <ceformat/config.hpp>
#include <ceformat/Format.hpp>

#include <ostream>

/**
	@addtogroup format
	@{
*/

/**
	Debug Element output operator.

	@returns @a stream.
	@param stream Output stream.
	@param e %Element.
*/
std::ostream&
operator<<(
	std::ostream& stream,
	ceformat::Element const& e
) {
	stream
		<< "{idx = " << e.idx
		<< ", beg = " << e.beg
		<< ", end = " << e.end
		<< ", width = " << e.width
		<< ", precision = " << e.precision
		<< ", flags = " << e.flags
		<< ", type = " << ceformat::get_element_type_name(e.type)
	;
	if (ceformat::ElementType::end != e.type) {
		stream << ", blob = \"";
		stream.write(
			e.fmt.string + e.beg,
			static_cast<std::streamsize>(e.end - e.beg)
		);
		stream << "\"}";
	} else {
		stream << '}';
	}
	return stream;
}

/**
	Debug Format output operator.

	@returns @a stream.
	@param stream Output stream.
	@param f %Format.
*/
std::ostream&
operator<<(
	std::ostream& stream,
	ceformat::Format const& f
) {
	stream
		<< "Format {\n"
		<< "  string = \"" << f.string << "\",\n"
		<< "  size = " << f.size << ",\n"
		<< "  element_count = " << f.element_count << ",\n"
		<< "  literal_count = " << f.literal_count << ",\n"
		<< "  elements: {"
	;
	for (
		std::size_t index = 0u;
		(ceformat::ELEMENTS_MAX + 1u) > index;
		++index
	) {
		stream << "\n    " << f.elements[index];
	}
	stream << "\n  }\n}";
	return stream;
}

namespace ceformat {
} // namespace ceformat

/** @} */ // end of doc-group format
