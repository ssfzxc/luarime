local rime = require("libluarime")

print(rime.version())
local id = rime.initial()
print("id:", id)


print("process key")
rime.process_key(id, 115,0)

local context = rime.context(id)
for k, v in pairs(context) do
  print("key: ", k, "   value: ", v)
end

rime.process_key(id, string.byte('='), 0);

context = rime.context(id)
for k, v in pairs(context) do
  print("key: ", k, "   value: ", v)
end

rime.finalize(id)


