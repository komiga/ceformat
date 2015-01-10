/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Format and Element classes.
*/

#pragma once

#include <ceformat/config.hpp>
#include <ceformat/utility.hpp>
#include <ceformat/element_defs.hpp>
#include <ceformat/Particle.hpp>

#include <stdexcept>
#include <iomanip>
#include <ostream>
#include <sstream>

namespace ceformat {

// Forward declarations
class Format;
class Element;

/**
	@addtogroup format
	@{
*/

/**
	%Format element.
*/
class Element final {
public:
	friend class Format;

	/** %Format. */
	Format const& fmt;
	/** %Element index. */
	std::size_t const idx;
	/** Beginning position. */
	std::size_t const beg;
	/** Type. */
	ElementType const type;
	/** Flags. */
	unsigned const flags;
	/** Width. */
	std::size_t const width;
	/** Floating-point precision. */
	signed const precision;
	/** Ending position. */
	std::size_t const end;

private:
	bool valid_;

	enum class ctor_invalid {};

	enum class NumeralSegment : unsigned {
		none = 0u,
		width,
		precision,
	};

	constexpr
	Element(
		Format const& fmt,
		std::size_t const index,
		ctor_invalid const
	) noexcept;

	constexpr
	Element(
		Format const& fmt,
		std::size_t const index,
		std::size_t const pos
	) noexcept;

	constexpr std::size_t
	cons_beg(
		std::size_t const pos
	) const noexcept;

	constexpr unsigned
	cons_flags_inner(
		char const value,
		Particle const& particle,
		std::size_t const next,
		bool const zero_padded,
		NumeralSegment const segment,
		bool const precision_prelude,
		unsigned const c_flags
	) const noexcept;

	constexpr unsigned
	cons_flags(
		std::size_t const pos,
		bool const zero_padded,
		NumeralSegment const segment,
		bool const precision_prelude,
		unsigned const c_flags
	) const noexcept;

	constexpr ElementType
	cons_type_inner(
		Particle const& particle,
		std::size_t const next
	) const noexcept;

	constexpr ElementType
	cons_type(
		std::size_t const pos
	) const noexcept;

	constexpr std::size_t
	cons_end() const noexcept;

	constexpr std::size_t
	cons_width_inner(
		char const value,
		Particle const& particle,
		std::size_t const next,
		bool const in_width,
		unsigned const width_accum
	) const noexcept;

	constexpr std::size_t
	cons_width(
		std::size_t pos,
		bool const in_width,
		unsigned const width_accum
	) const noexcept;

	constexpr signed
	cons_precision_inner(
		char const value,
		Particle const& particle,
		std::size_t const next,
		bool const in_precision,
		signed const precision_accum
	) const noexcept;

	constexpr signed
	cons_precision(
		std::size_t pos,
		bool const in_precision,
		signed const precision_accum
	) const noexcept;

	constexpr bool
	flag_check() const noexcept;

	constexpr bool
	valid_check() const noexcept;

public:
	/**
		Check if element has a flag.

		@returns @c true if the element has @a flag enabled.
		@param f Flag to test.
	*/
	constexpr bool
	has_flag(
		ElementFlags const f
	) const noexcept {
		return
		static_cast<unsigned>(f) & this->flags;
	}
};

/**
	%Format.
*/
class Format final {
public:
	/** %Format string. */
	char const* const string;
	/** Size of format string. */
	std::size_t const size;
	/** Elements. */
	Element const elements[ELEMENTS_MAX + 1u];
	/** Number of elements. */
	std::size_t const element_count;
	/** Number of literal elements. */
	std::size_t const literal_count;

private:
	constexpr Element
	e(
		std::size_t const index
	) const noexcept {
		return
		// Construct all elements past ElementType::end as invalid
		0u < index && ElementType::end == this->elements[index - 1].type
			? Element{*this, index, Element::ctor_invalid{}}

		// Fill next slot
		: Element{
			*this,
			index,
			(0u == index) ? 0u : elements[index - 1].end
		}
		;
	}

	constexpr std::size_t
	cons_size(
		std::size_t const c_size = 0u
	) const noexcept {
		return false ? 0u
		: 0u != c_size
			? c_size - static_cast<unsigned>('\0' == this->string[c_size - 1])
		: c_size
		;
	}

	constexpr std::size_t
	cons_count(
		std::size_t const index = 0u
	) const noexcept {
		return false ? 0u
		: ELEMENTS_MAX + 1u == index
			? throw std::logic_error("element array overrun")

		// No more elements to consider
		: ElementType::end == this->elements[index].type
			? index

		// Continue
		: cons_count(index + 1u)
		;
	}

	constexpr std::size_t
	cons_count_literal(
		std::size_t const count = 0u,
		std::size_t const index = 0u
	) const noexcept {
		return false ? 0u
		: ELEMENTS_MAX + 1u == index
			? throw std::logic_error("element array overrun")

		// No more elements to consider
		: ElementType::end == this->elements[index].type
			? count

		// Continue
		: cons_count_literal(
			count
			// esc is not a literal type
			+ static_cast<std::size_t>(
				ElementType::esc != this->elements[index].type
			),
			index + 1u
		)
		;
	}

public:
	/**
		Construct with C-string.

		@tparam N Size of C-string; inferred from @a string.
		@param string C-string.
	*/
	template<
		std::size_t N
	>
	constexpr explicit
	Format(
		char const (&string)[N]
	) noexcept
		: string(string)
		, size(cons_size(N))
		, elements{
			e(0u ),e(1u ),
			e(2u ),e(3u ),
			e(4u ),e(5u ),
			e(6u ),e(7u ),
			e(8u ),e(9u ),
			e(10u),e(11u),
			e(12u),e(13u),
			e(14u),e(15u),
			e(16u)
		}
		, element_count(cons_count())
		, literal_count(cons_count_literal())
	{}

	/**
		Beginning element iterator for format.

		@returns Beginning element iterator.
	*/
	constexpr Element const*
	begin() const noexcept { return elements; }

	/**
		Ending element iterator for format.

		@returns Ending element iterator.
	*/
	constexpr Element const*
	end() const noexcept { return elements + ELEMENTS_MAX; }

	/**
		Get the next literal element index.

		@remarks @c ElementType::esc and @c ElementType::end are
		non-literal types.

		@returns
		- Index of next literal element index, or
		- @a index if @c ElementType::end was encountered.
		@param index Current index (incremented).
	*/
	constexpr std::size_t
	next_literal_index(
		std::size_t const index
	) const noexcept {
		return false ? 0u
		// Escapes are non-literal
		: ElementType::esc == this->elements[index + 1u].type
			? next_literal_index(index + 1u)

		// Terminate at end
		: ElementType::end == this->elements[index + 1u].type
			? index

		// All others are literal
		: index + 1u
		;
	}
}; // class Format

#include <ceformat/impl/Element.inl>

/** @} */ // end of doc-group format

} // namespace ceformat
