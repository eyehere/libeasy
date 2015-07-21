#!/opt/soft/base_env/openresty/luajit/bin/luajit-2.0.0-beta10
-- test.lua
-- -- the first program in every language
package.path="/opt/soft/base_env/openresty/lualib/?.lua"
package.cpath="/opt/soft/base_env/openresty/lualib/?.so"

require("easy_lua")
-----------------------------------------------------------------------------
------------------------------------ipzone tes-------------------------------
-----------------------------------------------------------------------------
ip_zones = easy_lua.ip_zones_load("/opt/soft/data/ip_zone.data", 0)

zone = easy_lua.ip_zones_find(ip_zones, "202.108.14.240")
print(zone)

easy_lua.ip_zones_destroy(ip_zones)
