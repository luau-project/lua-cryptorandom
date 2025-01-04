local random = require("lua-cryptorandom")

local integer, err = random.integer()

if (integer == nil) then
    print("error code: ", err)
else
    print("integer: ", integer)
end
