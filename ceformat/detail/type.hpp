/**
@file detail/type.hpp
@brief Type checking and T-ElementType conversion.

@author Tim Howard
@copyright 2013-2014 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

#ifndef CEFORMAT_DETAIL_TYPE_HPP_
#define CEFORMAT_DETAIL_TYPE_HPP_

#include <ceformat/config.hpp>
#include <ceformat/String.hpp>
#include <ceformat/Format.hpp>

#include <type_traits>
#include <ostream>

namespace ceformat {
namespace detail {

template<
	typename T
>
using rm_const_t = typename std::remove_const<T>::type;

template<
	typename T
>
using rm_ref_t = typename std::remove_reference<T>::type;

template<
	typename T
>
using rm_cref_t = rm_const_t<typename std::remove_reference<T>::type>;

// NB: cast is rvalue reference to avoid calling _any_ ctors
// when using ouput argument in expression.
template<
	typename T,
	class = void
>
struct type_to_element {
	using cast = T&&;
	static constexpr bool valid = false;

	static constexpr bool
	type_matches(
		ElementType const
	) noexcept {
		return false;
	}
};

template<
	typename T
>
constexpr bool
tte_boolean() noexcept {
	return
	std::is_same<
		bool,
		rm_cref_t<T>
	>::value
	;
}

template<
	typename T
>
constexpr bool
tte_integral() noexcept {
	return
	!tte_boolean<T>() &&
	std::is_integral<
		rm_ref_t<T>
	>::value
	;
}

template<
	typename T
>
constexpr bool
tte_floating_point() noexcept {
	return
	std::is_floating_point<
		rm_ref_t<T>
	>::value
	;
}

template<
	typename T
>
constexpr bool
tte_string_charwise() noexcept {
	return
	// bounded (e.g., literal or bounded variable)
	(
		std::is_array<rm_ref_t<T>>::value &&
		1u == std::rank<rm_ref_t<T>>::value &&
		std::is_same<
			char,
			rm_const_t<
				typename std::remove_extent<
					rm_ref_t<T>
				>::type
			>
		>::value
	) ||
	// unbounded non-const (e.g., from a system function)
	std::is_same<
		char*,
		rm_cref_t<T>
	>::value ||
	// unbounded literal (e.g., variable)
	std::is_same<
		char const*,
		rm_cref_t<T>
	>::value
	;
}

template<
	typename T
>
constexpr bool
tte_pointer() noexcept {
	return
	!tte_string_charwise<T>()

	&& (
		std::is_pointer<
			rm_ref_t<T>
		>::value ||
		std::is_same<
			std::nullptr_t,
			rm_ref_t<T>
		>::value
	)
	;
}

// NB: SFINAE pissery due to stdlib not defining ostream operator<<
// for std::nullptr_t

template<
	typename T,
	typename = void
>
struct tte_object_sfinae {
	static constexpr bool
	value = false;
};

template<
	typename T
>
struct tte_object_sfinae<
	T,
	typename std::enable_if<
		!tte_pointer<T>() &&
		std::is_same<
			std::ostream&,
			decltype(std::declval<std::ostream&>() << std::declval<T&>())
		>::value
	>::type
> {
	static constexpr bool
	value = true;
};

template<
	typename T
>
constexpr bool
tte_string() noexcept {
	return
	!tte_integral<T>() &&
	!tte_boolean<T>() &&
	!tte_floating_point<T>() &&
	!tte_pointer<T>()

	&& (
		std::is_same<
			String,
			rm_cref_t<T>
		>::value ||

		tte_string_charwise<T>() ||
		tte_object_sfinae<T>::value
	)
	;
}

// integral

template<
	typename T
>
struct type_to_element<
	T,
	typename std::enable_if<
		tte_integral<T>()
	>::type
> {
	using cast = T&&;
	static constexpr bool valid = true;

	static constexpr bool
	type_matches(
		ElementType const type
	) noexcept {
		return false
		|| ElementType::dec == type
		|| ElementType::hex == type
		|| ElementType::oct == type
			? true

		: ElementType::uns == type
			? std::is_unsigned<
				rm_ref_t<T>
			>::value

		: false
		;
	}
};

// floating-point

template<
	typename T
>
struct type_to_element<
	T,
	typename std::enable_if<
		tte_floating_point<T>()
	>::type
> {
	using cast = T&&;
	static constexpr bool valid = true;

	static constexpr bool
	type_matches(
		ElementType const type
	) noexcept {
		return ElementType::flt == type;
	}
};

// boolean

template<
	typename T
>
struct type_to_element<
	T,
	typename std::enable_if<
		tte_boolean<T>()
	>::type
> {
	using cast = T&&;
	static constexpr bool valid = true;

	static constexpr bool
	type_matches(
		ElementType const type
	) noexcept {
		return ElementType::boo == type;
	}
};

// pointer

template<
	typename T
>
struct type_to_element<
	T,
	typename std::enable_if<
		tte_pointer<T>()
	>::type
> {
	using cast = void const*;
	static constexpr bool valid = true;

	static constexpr bool
	type_matches(
		ElementType const type
	) noexcept {
		return ElementType::ptr == type;
	}
};

// string/object

template<
	typename T
>
struct type_to_element<
	T,
	typename std::enable_if<
		tte_string<T>()
	>::type
> {
	using cast = T&&;
	static constexpr bool valid = true;

	static constexpr bool
	type_matches(
		ElementType const type
	) noexcept {
		return ElementType::str == type;
	}
};

namespace {

template<
	Format const&,
	typename...
>
struct type_check_impl;

template<
	Format const& format
>
struct type_check_impl<format> {
	static constexpr bool
	g(
		std::size_t const
	) noexcept {
		return true;
	}
};

template<
	Format const& format, 
	typename I,
	typename... P
>
struct type_check_impl<format, I, P...> {
	static constexpr bool
	g(
		std::size_t const index
	) noexcept {
		return false
		|| !type_to_element<I>::valid
		|| !type_to_element<I>::type_matches(format.elements[index].type)
			? throw std::logic_error("type of argument does not match element")

		// continue
		: type_check_impl<format, P...>::g(
			format.next_literal_index(index)
		)
		;
	}
};

} // anonymous namespace

template<
	Format const& format,
	typename... ArgP
>
constexpr bool
type_check() noexcept {
	return type_check_impl<format, ArgP...>::g(
		ElementType::esc == format.elements[0u].type
			? format.next_literal_index(0u)
		: 0u
	);
}

} // namespace detail
} // namespace ceformat

#endif // CEFORMAT_DETAIL_TYPE_HPP_
