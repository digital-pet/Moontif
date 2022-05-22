-- hello.lua

local motif = require 'motif'

function readAll(file)
	local f = assert(io.open(file, "rb"))
	local content = f:read("*all")
	f:close()
	return content
end

resources = { '*title: Hello Foone!' }

main = motif.RowColumn {
	motif.ScrolledText {
		editMode = motif.MULTI_LINE_EDIT,
		editable = true,
		rows = 26,
		columns = 80,
		value = readAll("../LICENSE")
	},
	motif.RowColumn {
		motif.PushButton {
                                labelString = "I Accept",
                                activateCallback = function()
                                        print("Button one was pressed")
                                end
                        },
                motif.PushButton {
                                labelString = "Button two",
                                activateCallback = function()
                                        print("Button one was pressed")
                                end
                        }
	}
}

motif.SetLanguageProc(nil, nil, nil)
app, toplevel = motif.Initialize("hello_world_application",resources)

motif.ParseAll(toplevel,main,"main")

motif.Realize(toplevel)

app:MainLoop()
