/**
@file Format.hpp
@brief Format and Element classes.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef CEFORMAT_FORMAT_HPP_
#define CEFORMAT_FORMAT_HPP_

#include <ceformat/config.hpp>
#include <ceformat/utility.hpp>
#include <ceformat/element_defs.hpp>

#include <stdexcept>
#include <iomanip>
#include <ostream>
#include <sstream>

// TODO: Floating-point precision

namespace ceformat {

// Forward declarations
struct Particle;
class Format;
class Element;

/**
	@addtogroup format
	@{
*/

/** @cond INTERNAL */
enum class ParticleKind : unsigned {
	invalid = 0u,
	type,
	flag,
	numeral
};

struct Particle final {
	ParticleKind const kind;
	char const value;
	union {
		ElementType const type;
		unsigned const flag;
	};

	enum ctor_kind_sentinel {
		invalid = 0,
		numeral
	};

	constexpr
	Particle(
		enum ctor_kind_sentinel const sentinel
	) noexcept
		: kind(
			ctor_kind_sentinel::numeral == sentinel
				? ParticleKind::numeral
			: ParticleKind::invalid
		)
		, value('\0')
		, flag(0u)
	{}

	constexpr
	Particle(
		char const value,
		ElementType const type
	) noexcept
		: kind(ParticleKind::type)
		, value(value)
		, type(type)
	{}

	constexpr
	Particle(
		char const value,
		ElementFlags const flag
	) noexcept
		: kind(ParticleKind::flag)
		, value(value)
		, flag(static_cast<unsigned>(flag))
	{}
};

namespace {
constexpr Particle const
s_particle_numeral{Particle::numeral},
s_particles[]{
	// types
	{ELEMENT_CHAR, ElementType::esc},
	{'d', ElementType::dec},
	{'u', ElementType::uns},
	{'x', ElementType::hex},
	{'o', ElementType::oct},
	{'f', ElementType::flt},
	{'b', ElementType::boo},
	{'p', ElementType::ptr},
	{'s', ElementType::str},

	// flags
	{'#', ElementFlags::show_base},
	{'+', ElementFlags::show_sign},
	// s_particle_numeral catches this
	//{'0', ElementFlags::zero_padded},
	{'-', ElementFlags::left_align},
	{Particle::invalid}
};
} // anonymous namespace

constexpr Particle const&
particle_classify(
	char const value,
	std::size_t const index = 0u
) noexcept {
	return
	// Numeral could be width or ElementFlags::zero_padded
	('0' <= value && '9' >= value)
		? s_particle_numeral

	// End or match
	: ParticleKind::invalid == s_particles[index].kind
	|| value == s_particles[index].value
		? s_particles[index]

	// Continue
	: particle_classify(value, index + 1u)
	;
}
/** @endcond */ // INTERNAL

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
	/** Ending position. */
	std::size_t const end;

private:
	enum invalid_t {
		invalid
	};

	bool valid_;

	constexpr
	Element(
		Format const& fmt,
		std::size_t const index,
		enum invalid_t const
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
		bool const in_width,
		unsigned const c_flags
	) const noexcept;

	constexpr unsigned
	cons_flags(
		std::size_t const pos,
		bool const zero_padded,
		bool const in_width,
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
			? Element{*this, index, Element::invalid}

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

#endif // CEFORMAT_FORMAT_HPP_
