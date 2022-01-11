-- hello.lua

local motif = require 'motif'

function readAll(file)
	local f = assert(io.open(file, "rb"))
	local content = f:read("*all")
	f:close()
	return content
end

resources = { '*title: Hello World!' }

main = motif.MainWindow {
	motif.ScrolledText {
		editMode = motif.MULTI_LINE_EDIT,
		editable = true,
		rows = 26,
		columns = 80,
		value = readAll("hello.lua")
	}
}

motif.SetLanguageProc(nil, nil, nil)
app, toplevel = motif.Initialize("hello_world_application",resources)

motif.Realize(toplevel, main)


app:MainLoop()
