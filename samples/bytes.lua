local random = require("lua-cryptorandom")

-- number of bytes to generate
local n = 10

local bytes, err = random.bytes(n)

if (bytes == nil) then
    print("error code: ", err)
else
    assert(n == #bytes, "Unexpected number of bytes")

    -- print each byte
    for i, b in ipairs(bytes) do
        print(i, ("0x%02X"):format(b))
    end
end
