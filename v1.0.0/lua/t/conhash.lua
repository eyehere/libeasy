#!/opt/soft/base_env/openresty/luajit/bin/luajit-2.0.0-beta10
-- test.lua
-- -- the first program in every language
package.path="/opt/soft/base_env/openresty/lualib/?.lua"
package.cpath="/opt/soft/base_env/openresty/lualib/?.so"

require("easy_lua")
-----------------------------------------------------------------------------
----------------------------conhash test------------------------------------
-----------------------------------------------------------------------------
conhash = easy_lua.conhash_init()

ips = {"10.10.10.10", "20.20.20.20", "30.30.30.30", "40.40.40.40", "50.50.50.50",
		"60.60.60.60", "70.70.70.70", "80.80.80.80", "90.90.90.90", "100.100.100.100"}

print("add node example...")
for  i, v in pairs(ips) do
	print((i - 1) .. " " .. v)
	rt = easy_lua.conhash_add_node(conhash, i - 1, v, 500)
	if rt==-1 then
		print("add " .. v .. "failed")
	end
end


print("del one node...")

rt = easy_lua.conhash_del_node(conhash, "10.10.10.10", string.len("10.10.10.10"))
if rt==-1 then
	print("del 10.10.10.10 failed")
end

print("look up")
ip = "200.200.200.200"
ip = easy_lua.conhash_lookup(conhash, ip, string.len(ip))

if ip then
	print("node: " .. ip)
else
	print("not find");
end

easy_lua.conhash_fini()
