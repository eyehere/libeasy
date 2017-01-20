#!/opt/soft/base_env/openresty/luajit/bin/luajit-2.0.0-beta10

-- test.lua
package.path="/opt/soft/base_env/openresty/lualib/?.lua"
package.cpath="/opt/soft/base_env/openresty/lualib/?.so"
local utils = require("easy_lua")

local lock = utils.process_lock_create("/var/test.lck", 0);
if not lock then
    print("create lock failed")
    return;
end

print("init")
utils.process_lock_init(lock);

print("try lock")
local rt, err = utils.process_try_lock(lock);
if not rt then
    print(err);
end

print("unlock");
utils.process_unlock(lock);

print("lock")
local rt = utils.process_lock(lock)
if not rt then
    print("lock failed");
end

print("release");
utils.process_lock_release(lock);
