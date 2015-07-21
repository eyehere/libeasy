#!/opt/soft/base_env/openresty/luajit/bin/luajit-2.0.0-beta10
-- test.lua
-- -- the first program in every language
package.path="/opt/soft/base_env/openresty/lualib/?.lua"
package.cpath="/opt/soft/base_env/openresty/lualib/?.so"

require("easy_lua")

-----------------------------------------------------------------------------
--------------------server_conhash------------------------------------------
-----------------------------------------------------------------------------
local load_policy_file     = "/opt/soft/lua/load_server_conhash_data.lua"
local server_conhash = easy_lua.server_conhash_create(load_policy_file);
local ip = easy_lua.server_conhash_find(server_conhash, 127, "/videos/tv/xxx.ts");
if ip then 
    print(ip);
end
easy_lua.server_conhash_destroy(server_conhash);
