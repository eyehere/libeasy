local data_file = "/opt/soft/data/group_server_data.lua";
local group_server          = {};
local group_count           = 0;

-----------------------------函数定义区---------------------------
-- group_server
function group(p)
    if p.id then
        group_server[group_count + 1] = p;
        group_count = group_count + 1;
    end
end

--载入数据文件
function load_data()
    dofile(data_file);
end

-- 获取group的数量
function get_group_count()
    return group_count;
end

-- 获取第i个group的id
function get_group_id(i)
    return group_server[i].id;
end

-- 获取第i个group下的host的数量
function get_server_count_by_seq(i)
    local count=0;
    local group = group_server[i];
    local count = #group;
   return count; 
end

-- 获取第i个group下的第n个host的ip和hash_ip
function get_server_by_seq(i, n)
    local group = group_server[i];
    local host = group[n];
    local ip = host.ip;
    local hash_ip = host.hash_ip;
    return ip, hash_ip;
end
