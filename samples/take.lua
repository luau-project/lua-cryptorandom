local random = require("lua-cryptorandom")

local take, err = random.take()

if (take == nil) then
    print("error code: ", err)
else
    print("take: ", take)
end
