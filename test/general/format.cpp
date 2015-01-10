
#include <ceformat/Format.hpp>
#include <ceformat/format_debug.hpp>
#include <ceformat/print.hpp>

#include <iostream>

static constexpr ceformat::Format const
	//bad_length{"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"},
	//bad_flag_1{"%-%"},
	//bad_flag_2{"%+x"},
	//bad_flag_3{"%+o"},
	//bad_flag_4{"%#s"},
	//bad_width{"%1%"},

	all{"%% %d %u %#x %#o %f %s %c"},
	flags{"%+-2d %u %#x %#o %f %b %#08p %#p"},
	align{"[%-4d] [%4u] [%-#6x] [%#4o] [%07.2f] [%-10b] [%#016p]"},
	max{"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"},
	obj{"%s"},
	empty{"empty"},
	null{""},
	floats{"%f/%#f %e/%#e %g/%#g %g %g %010f %010.4f %f %.4f"}
;

#define SNOTE(n__) std::cout << "Tracked(" n__ ")\n";

struct Tracked final {
	Tracked() { SNOTE(""); }
	Tracked(Tracked&&) { SNOTE("&&"); }
	Tracked(Tracked const&) { SNOTE("&"); }
	~Tracked() { SNOTE("~"); }
};

std::ostream&
operator<<(
	std::ostream& stream,
	Tracked const&
) {
	return stream << "<< Tracked";
}

namespace cf = ceformat;

char const
strlit_solid[] = "strlit_solid";

char const* const
strlit_solid_unbound = "strlit_solid_unbound";

#define CEFORMAT_TEST_IO(f_)												\
	<< cf::f_<all>(-3, 42u, 0x12abcdef, 0777, 3.14f, "string", 'A') << '\n'	\
	<< cf::f_<flags>(42, 42u, 42u, 42, 42.0f, true, ep, nullptr) << '\n'	\
	<< cf::f_<align>(-42, 42u, 42, 42u, -42.0f, false, ep) << '\n'			\
	<< cf::f_<max>() << '\n'												\
	<< cf::f_<obj>("strlit") << '\n'										\
	<< cf::f_<obj>(strlit_solid) << '\n'									\
	<< cf::f_<obj>(strlit_solid_unbound) << '\n'							\
	<< cf::f_<obj>(obj.elements[0u]) << '\n'								\
	<< cf::f_<obj>(concrete) << '\n'										\
	<< cf::f_<obj>(Tracked{}) << '\n'										\
	<< cf::f_<empty>() << '\n'												\
	<< "null: " << cf::f_<null>() << '\n'

signed
main() {
	std::cout
		<< all << '\n'
		<< flags << '\n'
		<< align << '\n'
		<< max << '\n'
		<< obj << '\n'
		<< empty << '\n'
		<< null << '\n'
	;

	cf::Element const* const ep = &obj.elements[0u];
	Tracked concrete;
	std::cout
		<< "\nwith print:\n\n"
		CEFORMAT_TEST_IO(print)

		<< "\nwith sentinel:\n\n"
		CEFORMAT_TEST_IO(write_sentinel)
	;

	std::cout << "\nwith write:\n\n";
	cf::write<all>(std::cout, -3, 42u, 0x12abcdef, 0777, 3.14f, "string", 'A');
	std::cout << '\n';
	cf::write<floats>(
		std::cout,
		// normal/alternative
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		// generic
		3.14f, 3.00014e06f,
		// width & precision
		3.14f, 3.14f, 3.14f, 3.14f
	);
	std::cout << '\n';
	std::cout.flush();
}
