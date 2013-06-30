
-- Test

dofile("../premake_common.lua")

solution("test")
	configurations {"debug", "release"}
	platforms {"x32", "x64"}

-- Include all groups

include "general"

if _ACTION == "clean" then
	local prjs = solution().projects
	for _, prj in ipairs(prjs) do
		os.rmdir(prj.basedir.."/out")
	end
end
