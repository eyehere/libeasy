#!/opt/soft/base_env/openresty/luajit/bin/luajit-2.0.0-beta10

-- test.lua
package.path="/opt/soft/base_env/openresty/lualib/?.lua"
package.cpath="/opt/soft/base_env/openresty/lualib/?.so"
local utils = require("easy_lua")

md5 = utils.gen_md5("http://88888888.com/3725464546/videos/tv/20121015/10ca3ce7456df8d404fed24c6a24e15e.f4v")
print("md5:" .. md5 .. ", size:" .. #md5)
