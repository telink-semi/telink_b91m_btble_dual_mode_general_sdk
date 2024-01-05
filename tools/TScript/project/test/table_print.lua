local rawpairs = pairs


function defaultComp( op1, op2 )
    local type1, type2 = type(op1), type(op2)
    local num1,  num2  = tonumber(op1), tonumber(op2)

    if ( num1 ~= nil) and (num2 ~= nil) then
        return  num1 < num2
    elseif type1 ~= type2 then
        return type1 < type2
    elseif type1 == "string"  then
        return op1 < op2
    elseif type1 == "boolean" then
        return op1
         -- 以上处理: number, string, boolean
    else -- 处理剩下的:  function, table, thread, userdata
        return tostring(op1) < tostring(op2)  -- tostring后比较字符串
    end
end

-- -----------------------------------------
-- 可以按指定顺序遍历的map迭代器
-- @param tbl   要迭代的表
-- @param func  比较函数
-- @example
--      for k,v in pairs(tbl,defaultComp) do print(k,v) end
function pairs(tbl, func)
    if func == nil then
        --return rawpairs(tbl)
		func = defaultComp;
    end

    -- 为tbl创建一个对key排序的数组
    -- 自己实现插入排序，table.sort遇到nil时会失效
    local ary = {}
    local lastUsed = 0
    for key --[[, val--]] in rawpairs(tbl) do
        if (lastUsed == 0) then
            ary[1] = key
        else
            local done = false
            for j=1,lastUsed do  -- 进行插入排序
                if (func(key, ary[j]) == true) then
                    --arrayInsert( ary, key, j)
					table.insert(ary,j,key)
                    done = true
                    break
                end
            end
            if (done == false) then
                ary[lastUsed + 1] = key
            end
        end
        lastUsed = lastUsed + 1
    end

    -- 定义并返回迭代器
    local i = 0
    local iter = function ()
        i = i + 1
        if ary[i] == nil then
            return nil
        else
            return ary[i], tbl[ary[i]]
        end
    end
    return iter
end



function print_r ( t )  
    local print_r_cache={}
    local function sub_print_r(t,indent)
        if (print_r_cache[tostring(t)]) then
            print(indent.."*"..tostring(t))
        else
            print_r_cache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
                        print(indent.."["..pos.."] => "..tostring(val).." {")
                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
                        print(indent..string.rep(" ",string.len(pos)+6).."}")
                    elseif (type(val)=="string") then
                        print(indent.."["..pos..'] => "'..val..'"')
					elseif (type(val)=="function") then
						print(indent.."["..pos..'] => "'..tostring(val)..'"')
					elseif (type(val)=="boolean") then
						print(indent.."["..pos..'] => "'..tostring(val)..'"')
                    else
                        --print(indent.."["..pos.."] => "..tostring(val))
						print(indent.."["..pos.."] => "..string.format("%d(0x%02x)",val,val))
                    end
                end
            else
                print(indent..tostring(t))
            end
        end
    end
    if (type(t)=="table") then
        print(tostring(t).." {")
        sub_print_r(t,"  ")
        print("}")
    else
        sub_print_r(t,"  ")
    end
    print()
end

table.print = print_r;


tbl1 = {
	a = 13,
    b = "36",
    c = "52s",
    d = false,
    e = function() print("e") end
};

tbl2 = {
    a1 = "happy",
    a2 = "new",
    a3 = "year",
}

tbl1.f = tbl2
tbl1.g = tbl2
table.print(tbl1);


