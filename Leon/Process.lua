local DEBUG_DUMP = true

-- Helper functions
local function Dump(o, indent, recurse)
	if indent == nil then
		indent = 1
	end
	if recurse == nil then
		recurse = {}
	else
		if recurse[o] ~= nil then
			return "(recursion halted)"
		end

		local newrecurse = {}
		for i, _ in pairs(recurse) do
			newrecurse[i] = true
		end
		recurse = newrecurse
		recurse[o] = true
	end

	if type(o) == "table" then
		local s = "{\n"
		for k, v in pairs(o) do
			s = `{s}{string.rep("    ", indent)}[{Dump(k, indent + 1, recurse)}] = {Dump(v, indent + 1, recurse)},\n`
		end
		return `{s}{string.rep("    ", indent - 1)}\}`
	elseif type(o) == "string" then
		return `"{tostring(o)}"`
	else
		return tostring(o)
	end
end

local function Identifier(name)
	-- Replace non-alphanumeric characters with underscores
	name = string.gsub(name, "[^%a_]", "_")
	
	-- If name starts with a number, prefix with an underscore
	local fc = string.byte(string.sub(name, 1, 1))
	if fc >= 0x30 and fc <= 0x39 then
		name = "_"..name
	end
	
	return name
end

local function EscapeString(str)
	return str:gsub('[%z\1-\31\128-\255\134\\"]', function(c)
		local num = string.byte(c)
		return `\\{(num // 64) % 8}{(num // 8) % 8}{(num // 1) % 8}`
	end)
end

local function GetFileName(str)
	-- Get text between the last slash and first period after it
	-- Script/Lib/Thread.h becomes Thread

	local last_period = #str
	for i = #str, 1, -1 do
		local c = string.sub(str, i, i)
		if c == "." then
			last_period = i - 1
		elseif c == "/" or c == "\\" then
			return string.sub(str, i + 1, last_period)
		end
	end

	return string.sub(str, 1, last_period)
end

local function GetFunctionName(name)
	-- Remove namespace
	-- Script::Lib::Thread::Require becomes Require

	local last_colon = 1
	for i = #name, 1, -1 do
		local c = string.sub(name, i, i)
		if c == ":" then
			return string.sub(name, i + 1)
		end
	end

	return name
end

local function RemoveTemplate(name)
	for i = 1, #name do
		local c = string.sub(name, i, i)
		if c == "<" then
			return string.sub(name, 1, i - 1)
		end
	end

	return name
end

return {

-- Source process
-- Run for each header file
SourceProcess = function(source, types, enums, classes, functions)
	local name = GetFileName(source)

	local result = ""
	
	result ..= `#include <{source}>\n`
	result ..= "#include \"Script/Lib.h\"\n\n"

	result ..= "#include <string>\n"
	result ..= "#include <iostream>\n\n"
	
	result ..= "namespace PaperPup::Leon\n{\n\n"

	-- Dump debug info
	if DEBUG_DUMP then
		local dumper = `types: {Dump(types)}\nenums: {Dump(enums)}\nclasses: {Dump(classes)}\nfunctions: {Dump(functions)}`
		result ..= `/*\n{dumper}\n*/\n\n`
	end

	-- Create function bridges
	local has_functions = false

	for _, f in pairs(functions) do
		has_functions = true
	
		local fname = GetFunctionName(f.name)
		result ..= `int {fname}_bridge(lua_State *L)\n\{\n`
		result ..= "\t(void)L;\n\n"

		-- Get the arguments to bridge
		-- Also determine if the thread state is to be passed or not
		local args = table.clone(f.arguments)
		
		local has_L_arg = false
		if #args >= 1 then
			local arg_1_type = args[1].type.unqualified_root
			if arg_1_type.name == "lua_State" then
				has_L_arg = true
			end
			
			if has_L_arg then
				table.remove(args, 1)
			end
		end

		-- Generate argument bridges
		local arg_checks = ""
		local arg_gets = ""

		for i, arg in pairs(args) do
			local arg_type = arg.type

			local read_type = arg_type

			-- if read_type.type_type == "lvalue_reference" or read_type.type_type == "rvalue_reference" then
			-- 	read_type = read_type.pointee
			-- end
			read_type = read_type.unqualified_root

			arg_checks ..= `\tluaL_argcheck(L, Script::Lib::Is<{read_type.name}>(*L, {i}), {i}, "'{read_type.name}' expected");\n`
			arg_gets ..= `\tauto {arg.name} = Script::Lib::To<{read_type.name}>(*L, {i});\n`
		end

		-- First, check argument types
		result ..= `{arg_checks}\n`
		
		-- Then check function permission
		local permissions = f.attributes["Permissions"]

		if permissions ~= nil then
			result ..= `\tScript::Lib::CheckPermissions(*L, Script::Context::Permissions::{permissions});\n\n`
		end

		-- Then get arguments
		result ..= `{arg_gets}\n`

		-- Generate function call
		local has_result = f.return_type.name ~= "void"

		if has_result then
			result ..= `\tauto result = {f.name}(`
		else
			result ..= `\t{f.name}(`
		end

		-- Insert L argument if it uses it
		if has_L_arg then
			result ..= "L"
		end
		
		-- Insert bridged arguments
		local printed_arg = has_L_arg
		for _, arg in pairs(args) do
			if printed_arg then
				result ..= ", "
			end
			printed_arg = true

			local arg_type = arg.type

			if arg_type.type_type == "rvalue_reference" then
				result ..= `std::move({arg.name})`
			else
				result ..= arg.name
			end
		end

		-- End function call
		result ..= ");\n\n"

		-- Bridge result
		if f.attributes.Yield then
			result ..= "\treturn lua_yield(L, 0);\n"
		else
			if has_result then
				result ..= "\tScript::Lib::Push(*L, result);\n"
				result ..= "\treturn 1;\n"
			else
				result ..= "\treturn 0;\n"
			end
		end

		result ..= "}\n\n"
	end

	if has_functions then
		-- Create registration list
		result ..= `static luaL_Reg {name}_lib[] = \{\n`
		for _, f in pairs(functions) do
			local fname = GetFunctionName(f.name)
			result ..= `\t\{ "{fname}", {fname}_bridge \},\n`
		end
		result ..= "\t{ nullptr, nullptr }\n};\n\n"
	end

	-- Generate class registration function
	result ..= `void {name}_register(lua_State *L)\n`
	result ..= "{\n"
	result ..= "\t(void)L;\n"

	if has_functions then
		result ..= `\tluaL_register(L, "{name}", {name}_lib);\n`
	end

	result ..= "}\n"
	
	result ..= "\n}\n"

	return result
end;

-- Glue process
-- Run once for all header files
GlueProcess = function(sources)
	local result = ""

	result ..= "#include \"lua.h\"\n"
	result ..= "#include \"lualib.h\"\n\n"

	result ..= "namespace PaperPup::Leon\n{\n\n"

	-- Import library register functions
	for _, v in pairs(sources) do
		result ..= `extern void {GetFileName(v.source)}_register(lua_State *L);\n`
	end

	-- Generate main library register function
	result ..= "\nvoid Register(lua_State *L)\n{\n"
	result ..= "\t(void)L;\n"

	-- Register each library
	for _, v in pairs(sources) do
        result ..= `\t{GetFileName(v.source)}_register(L);\n`
	end
	
	result ..= "}\n"

	result ..= "\n}\n"

	return result
end;

};
