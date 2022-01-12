local motif = require "motif"

gui = motif.Form {
	fractionBase = 3,
	motif.Frame {
		topAttachment = motif.ATTACH_FORM,
		rightAttachment = motif.ATTACH_FORM,
		bottomAttachment = motif.ATTACH_POSITION,
		leftAttachment = motif.ATTACH_FORM,

		bottomPosition = 1,

		motif.LabelGadget {
			labelString = "Motif Buttons in Lua",
			childType = motif.FRAME_TITLE_CHILD
		},
		motif.RowColumn {
			motif.PushButton {
				labelString = "Button one",
				activateCallback = function()
					print("Button one was pressed")
				end
			},
			motif.PushButton {
				labelString = "Button two",
				activateCallback = function()
					print("Button two was pressed")
				end
			},
		}
	},
	motif.TabStack {
		topAttachment = motif.ATTACH_POSITION,
		rightAttachment = motif.ATTACH_FORM,
		bottomAttachment = motif.ATTACH_FORM,
		leftAttachment = motif.ATTACH_FORM,

		topPosition = 2,

		tabSide = motif.TABS_ON_BOTTOM,
		tabStyle = motif.TABS_ROUNDED,

		tabCornerPercent = 100,
		tabMarginHeight = 10,
		tabMarginWidth = 10,

		products = motif.LabelGadget {
				labelString = "frame title",
		},
		groups = motif.Frame {
			motif.LabelGadget {
				labelString = "frame title",
				childType = motif.FRAME_TITLE_CHILD,
			},
			rc = motif.RowColumn {
				motif.PushButton {
					labelString = "pushme",
					activateCallback = function (w)
						print(tf:GetString())
						tfield = w:Parent().t
						print(tfield:GetString())
					end
				},
				t = motif.TextField {
					activateCallback = function (w)
						print(w:GetString())
					end,
					valueChangedCallback = function (w)
						print(w:GetString())
					end,
					focusCallback = function (w)
						print("Button down " ..
						    w:GetString())
					end
				}
			}
		},
	}

}

tf = gui[2].groups.rc.t

motif.SetLanguageProc(nil, nil, nil)
app, toplevel = motif.Initialize("LuaMotif")

motif.ParseAll(toplevel,gui,"Button Demo")

motif.Realize(toplevel)

app:MainLoop()
