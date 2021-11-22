map = Map("ibm_conf")

section = map:section(NamedSection, "general", "conf", "IBM program section")

flag = section:option(Flag, "enable", "Enable", "Enable program")

orgId = section:option(Value,"orgId","orgId")

typeId = section:option(Value,"typeId","typeId")

deviceId = section:option( Value, "deviceId", "deviceId")

authToken = section:option(Value,"authToken","authToken")

return map
