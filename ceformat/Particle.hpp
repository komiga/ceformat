/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Particle class and utilities.
*/

#pragma once

#include <ceformat/config.hpp>
#include <ceformat/element_defs.hpp>

namespace ceformat {

// Forward declarations
struct Particle;

/**
	@addtogroup particle
	@{
*/

enum class ParticleKind : unsigned {
	invalid = 0u,
	type,
	flag,
	numeral,
	precision,
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
		numeral,
		precision,
	};

	constexpr
	Particle(
		enum ctor_kind_sentinel const sentinel
	) noexcept
		: kind(
			ctor_kind_sentinel::numeral == sentinel
				? ParticleKind::numeral
			: ctor_kind_sentinel::precision == sentinel
				? ParticleKind::precision
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
s_particle_precision{Particle::precision},
s_particles[]{
	// types
	{ELEMENT_CHAR, ElementType::esc},
	{'c', ElementType::chr},
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
particle_classify_find(
	char const value,
	std::size_t const index
) noexcept {
	return
	// End or match
	ParticleKind::invalid == s_particles[index].kind
	|| value == s_particles[index].value
		? s_particles[index]

	// Continue
	: particle_classify_find(value, index + 1u)
	;
}

constexpr Particle const&
particle_classify(
	char const value,
	std::size_t const index = 0u
) noexcept {
	return
	// Numeral could be width or ElementFlags::zero_padded
	('0' <= value && value <= '9')
		? s_particle_numeral

	// Precision specifier
	: '.' == value
		? s_particle_precision

	: particle_classify_find(value, index)
	;
}

/** @} */ // end of doc-group particle

} // namespace ceformat
