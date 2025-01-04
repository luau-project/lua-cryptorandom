local random = require("lua-cryptorandom")

-- all the fields of this library are read-only

if (not pcall(function() random.bytes = 0 end)) then
    print("Not allowed to change bytes function")
end

if (not pcall(function() random.take = 0 end)) then
    print("Not allowed to change take function")
end

if (not pcall(function() random.integer = 0 end)) then
    print("Not allowed to change integer function")
end

if (not pcall(function() random.number = 0 end)) then
    print("Not allowed to change number function")
end

if (not pcall(function() random.anyfield = 0 end)) then
    print("Not allowed to change any arbitrary field")
end

