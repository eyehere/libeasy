#!/opt/soft/base_env/openresty/luajit/bin/luajit-2.0.0-beta10

package.path="/opt/soft/base_env/openresty/lualib/?.lua"
package.cpath="/opt/soft/base_env/openresty/lualib/?.so"
local utils = require("easy_lua")

-- time token
print("====================================");
print("当前时间：" .. os.time());
token = utils.gen_time_token_des3("12345678")
print("时间token：" .. token)
data = utils.decrypt_time_token_des3(token, "12345678")
print("解密后：" .. data)


-- 普通
print("====================================");
data = "1377766173)(*&^html@#$%a"
key = "!z@#$@sedq2s*#(^"
print("待加密内容：" .. data)
mi = utils.encrypt_des3(data, key)
print("加密后：" .. mi)

dec = utils.decrypt_des3(mi, key);
if not dec then
    print("nil");
end
print("解密后: " .. dec);
