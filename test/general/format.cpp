
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

	all{"%% %d %u %#x %#o %f %s"},
	flags{"%+-2d %u %#x %#o %f %b %#08p %#p"},
	align{"[%-4d] [%4u] [%-#6x] [%#4o] [%012f] [%-10b] [%#016p]"},
	max{"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"},
	obj{"%s"},
	empty{"empty"},
	null{""}
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
		<< "start:\n\n"
		<< cf::print<all>(-3, 42u, 0x12abcdef, 0777, 3.14f, "string") << '\n'
		<< cf::print<flags>(42, 42u, 42u, 42, 42.0f, true, ep, nullptr) << '\n'
		<< cf::print<align>(-42, 42u, 42, 42u, -42.0f, false, ep) << '\n'
		<< cf::print<max>() << '\n'
		<< cf::print<obj>(obj.elements[0u]) << '\n'
		<< cf::print<obj>(concrete) << '\n'
		<< cf::print<obj>(Tracked{}) << '\n'
		<< cf::print<empty>() << '\n'
	;
	cf::write<all>(std::cout, -3, 42u, 0x12abcdef, 0777, 3.14f, "string");
	std::cout << '\n';
	std::cout.flush();
}
