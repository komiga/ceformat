/**
@file

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

// class Element implementation

constexpr
Element::Element(
	Format const& fmt,
	std::size_t const index,
	enum invalid_t const
) noexcept
	: fmt(fmt)
	, idx(index)
	, beg(fmt.size)
	, type(ElementType::end)
	, flags(0u)
	, width(0u)
	, end(fmt.size)
	, valid_(false)
{}

constexpr
Element::Element(
	Format const& fmt,
	std::size_t const index,
	std::size_t const pos
) noexcept
	: fmt(fmt)
	, idx(index)
	, beg(cons_beg(pos))
	, type(cons_type(this->beg + 1u))
	, flags(cons_flags(this->beg + 1u, false, false, 0u))
	, width(cons_width(this->beg + 1u, false, 0u))
	, end(cons_end())
	, valid_(valid_check())
{}

// beg

constexpr std::size_t
Element::cons_beg(
	std::size_t const pos
) const noexcept {
	return false ? 0u
	// No element found; this is an ElementType::end
	: pos >= this->fmt.size
		? this->fmt.size

	// Found element
	: ELEMENT_CHAR == this->fmt.string[pos]
		? pos

	// Next
	: cons_beg(pos + 1u)
	;
}

// type

constexpr ElementType
Element::cons_type_inner(
	Particle const& particle,
	std::size_t const next
) const noexcept {
	return false ? ElementType::end
	: ParticleKind::invalid == particle.kind
		? throw std::logic_error("format string overflow; malformed element")

	// Take type
	: ParticleKind::type == particle.kind
		? particle.type

	// Continue otherwise
	: cons_type(next)
	;
}

constexpr ElementType
Element::cons_type(
	std::size_t const pos
) const noexcept {
	return false ? ElementType::end
	// If no ELEMENT_CHAR was found in cons_beg()
	: this->beg == this->fmt.size
		? ElementType::end

	: pos >= this->fmt.size
		? throw std::logic_error("format string overflow; malformed element")

	// Process next particle
	: cons_type_inner(
		particle_classify(this->fmt.string[pos]),
		pos + 1u
	)
	;
}

// flags

constexpr unsigned
Element::cons_flags_inner(
	char const value,
	Particle const& particle,
	std::size_t const next,
	bool const zero_padded,
	bool const in_width,
	unsigned const c_flags
) const noexcept {
	return false ? 0u
	: ParticleKind::invalid == particle.kind
		? throw std::logic_error("format string overflow; malformed element")

	// Numeral can be leading zero or element width
	: ParticleKind::numeral == particle.kind
		// Multiple leading zeros is invalid
		? !in_width && zero_padded && '0' == value
			? throw std::logic_error("only one leading zero is permitted")

		// Continue
		: cons_flags(
			next,
			zero_padded || (!in_width && '0' == value),
			in_width || '0' != value,
			c_flags | (
				!in_width && '0' == value
					? static_cast<unsigned>(ElementFlags::zero_padded)
				: 0u
			)
		)

	: ParticleKind::flag == particle.kind
		// Flag after width is invalid
		? in_width
			? throw std::logic_error(
				"flags must occur before width"
			)

		// Include flag and continue
		: cons_flags(
			next,
			zero_padded,
			in_width /* false */,
			c_flags | particle.flag
		)

	// Terminate at ParticleKind::type
	: c_flags
	;
}

constexpr unsigned
Element::cons_flags(
	std::size_t const pos,
	bool const zero_padded,
	bool const in_width,
	unsigned const c_flags
) const noexcept {
	return false ? 0u
	// No flags
	: ElementType::end == this->type
		? 0u

	: pos >= this->fmt.size
		? throw std::logic_error("format string overflow; malformed element")

	// Process next particle
	: cons_flags_inner(
		this->fmt.string[pos],
		particle_classify(this->fmt.string[pos]),
		pos + 1u,
		zero_padded,
		in_width,
		c_flags
	)
	;
}

// width

constexpr std::size_t
Element::cons_width_inner(
	char const value,
	Particle const& particle,
	std::size_t const next,
	bool const in_width,
	unsigned const width_accum
) const noexcept {
	return false ? 0u
	: ParticleKind::invalid == particle.kind
		? throw std::logic_error("format string overflow; malformed element")

	// Include digit and continue
	: ParticleKind::numeral == particle.kind
	&& (in_width || '0' != value)
		? cons_width(
			next,
			true,
			(width_accum * 10u)
			+ static_cast<unsigned>(value - '0')
		)

	// Terminate at ParticleKind::type
	: ParticleKind::type == particle.kind
		? width_accum

	// Continue (ParticleKind::Clag)
	: cons_width(next, in_width, width_accum)
	;
}

constexpr std::size_t
Element::cons_width(
	std::size_t pos,
	bool const in_width,
	unsigned const width_accum
) const noexcept {
	return false ? 0u
	// No flags
	: ElementType::end == this->type
		? 0u

	: pos >= this->fmt.size
		? throw std::logic_error("format string overflow; malformed element")

	// Process next particle
	: cons_width_inner(
		this->fmt.string[pos],
		particle_classify(this->fmt.string[pos]),
		pos + 1u,
		in_width,
		width_accum
	)
	;
}

// end

constexpr std::size_t
Element::cons_end() const noexcept {
	return
	// Force to end of format string
	ElementType::end == this->type
		? this->fmt.size

	// Calculate actual range of element
	: this->beg
		+ 1u // ELEMENT_CHAR
		+ element_flag_count(this->flags)
		+ (0u < this->width
			? utility::digit_count(this->width)
		: 0u)
		+ 1u // Type character
	;
}

// valid_

constexpr bool
Element::flag_check() const noexcept {
	return
	0u == (this->flags & ~s_flags_permitted[static_cast<unsigned>(this->type)]);
}

constexpr bool
Element::valid_check() const noexcept {
	return false ? false
	: ELEMENTS_MAX == this->idx && ElementType::end != this->type
		? throw std::logic_error("number of elements exceeds maximum")

	: fmt.size < this->end
		? throw std::logic_error("internal: element size improperly calculated")

	: !flag_check()
		? throw std::logic_error("element flag(s) not valid with type")

	: ElementType::esc == this->type && 0u < this->width
		? throw std::logic_error("element width not permitted with escape")

	: true
	;
}
