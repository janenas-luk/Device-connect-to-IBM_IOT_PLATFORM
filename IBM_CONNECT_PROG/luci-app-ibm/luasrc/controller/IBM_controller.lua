module("luci.controller.IBM_controller", package.seeall)

function index()
	entry({"admin", "services", "IBM"}, cbi("IBM_model"), _("IBM_program"),105)
end
