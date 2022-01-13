local motif = require 'motif'

button = motif.PushButton {
	labelString = "Push here to say hello",
	activateCallback = function ()
		print("Hello yourself!")
	end
}

motif.SetLanguageProc(nil, nil, nil)
app, toplevel = motif.Initialize("XLua")

motif.ParseAll(toplevel,button,"dialog")

motif.Realize(toplevel)
app:MainLoop()
