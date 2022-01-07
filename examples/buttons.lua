local motif = require "motif"

gui = motif.Form {
	fractionBase = 3,
	motif.Frame {
		topAttachment = ATTACH_FORM,
		rightAttachment = ATTACH_FORM,
		bottomAttachment = ATTACH_POSITION,
		leftAttachment = ATTACH_FORM,

		bottomPosition = 1,

		motif.LabelGadget {
			labelString = "Motif Buttons in Lua",
			childType = FRAME_TITLE_CHILD
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
		topAttachment = ATTACH_POSITION,
		rightAttachment = ATTACH_FORM,
		bottomAttachment = ATTACH_FORM,
		leftAttachment = ATTACH_FORM,

		topPosition = 2,

		tabSide = TABS_ON_BOTTOM,
		tabStyle = TABS_ROUNDED,

		tabCornerPercent = 100,
		tabMarginHeight = 64,
		tabMarginWidth = 64,

		products = motif.LabelGadget {
				labelString = "frame title",
		},
		groups = motif.Frame {
			motif.LabelGadget {
				labelString = "frame title",
				childType = FRAME_TITLE_CHILD,
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

motif.Realize(toplevel, gui)

app:MainLoop()
