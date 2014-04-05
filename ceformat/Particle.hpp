/**
@file Particle.hpp
@brief Particle class and utilities.

@author Timothy Howard
@copyright 2013-2014 Timothy Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef CEFORMAT_PARTICLE_HPP_
#define CEFORMAT_PARTICLE_HPP_

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

/** @} */ // end of doc-group particle

} // namespace ceformat

#endif // CEFORMAT_PARTICLE_HPP_
