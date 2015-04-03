
local S, G, R = precore.helpers()

precore.make_config_scoped("ceformat.env", {
	once = true,
}, {
{global = function()
	precore.define_group("CEFORMAT", os.getcwd())
end}})

precore.make_config("ceformat.strict", nil, {
{project = function()
	configuration {}
		flags {
			"FatalWarnings"
		}

	configuration {"linux"}
		buildoptions {
			"-pedantic-errors",
			"-Wextra",

			"-Wuninitialized",
			"-Winit-self",

			"-Wmissing-field-initializers",
			"-Wredundant-decls",

			"-Wfloat-equal",
			"-Wold-style-cast",

			"-Wnon-virtual-dtor",
			"-Woverloaded-virtual",

			"-Wunused",
			"-Wundef",
		}
end}})

precore.make_config("ceformat.dep", nil, {
{project = function()
	configuration {}
		includedirs {
			S"${CEFORMAT_ROOT}/",
		}
end}})

precore.apply_global({
	"precore.env-common",
	"ceformat.env",
})
