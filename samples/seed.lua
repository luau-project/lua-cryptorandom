local random = require("lua-cryptorandom")

local seed, err = random.seed()

if (seed == nil) then
    print("error code: ", err)
else
    print("seed: ", seed)
end
