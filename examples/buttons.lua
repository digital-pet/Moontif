require "motif"

gui = Form {
	fractionBase = 3,
	Frame {
		topAttachment = ATTACH_FORM,
		rightAttachment = ATTACH_FORM,
		bottomAttachment = ATTACH_POSITION,
		leftAttachment = ATTACH_FORM,

		bottomPosition = 1,

		LabelGadget {
			labelString = "Motif Buttons in Lua",
			childType = FRAME_TITLE_CHILD
		},
		RowColumn {
			PushButton {
				labelString = "Button one",
				activateCallback = function()
					print("Button one was pressed")
				end
			},
			PushButton {
				labelString = "Button two",
				activateCallback = function()
					print("Button two was pressed")
				end
			},
		}
	},
	TabStack {
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

		products = LabelGadget {
				labelString = "frame title",
		},
		groups = Frame {
			LabelGadget {
				labelString = "frame title",
				childType = FRAME_TITLE_CHILD,
			},
			rc = RowColumn {
				PushButton {
					labelString = "pushme",
					activateCallback = function (w)
						print(tf:GetString())
						tfield = w:Parent().t
						print(tfield:GetString())
					end
				},
				t = TextField {
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

SetLanguageProc(nil, nil, nil)
toplevel, app = Initialize("LuaMotif")

Realize(toplevel, gui)

MainLoop(app)
