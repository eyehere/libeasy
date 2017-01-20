#!/opt/soft/base_env/openresty/luajit/bin/luajit-2.0.0-beta10

-- test.lua
package.path="/opt/soft/base_env/openresty/lualib/?.lua"
package.cpath="/opt/soft/base_env/openresty/lualib/?.so"
local utils = require("easy_lua")

i = 0;
local uuid_list = {};
while i < 1000 do
    uuid = utils.gen_uuid();
    for j, k in ipairs(uuid_list) do
        if uuid == k then
            print("重复的uuid:" .. uuid)
        end
    end
    print("uuid:" .. uuid .. ", size:" .. #uuid)
    table.insert(uuid_list, uuid);
    i = i + 1;
end
