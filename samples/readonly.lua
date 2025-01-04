local random = require("lua-cryptorandom")

if (not pcall(function() random.bytes = 0 end)) then
    print("Not allowed to change bytes function")
end

if (not pcall(function() random.seed = 0 end)) then
    print("Not allowed to change seed function")
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

