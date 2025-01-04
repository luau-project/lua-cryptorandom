local random = require("lua-cryptorandom")

local number, err = random.number()

if (number == nil) then
    print("error code: ", err)
else
    print("number: ", number)
end
