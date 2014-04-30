
function size(T)
  local count = 0
  for _ in pairs(T) do count = count + 1 end
  return count
end

function tprint (tbl, indent)
  if not indent then indent = 0 end
  for k, v in pairs(tbl) do
    formatting = string.rep("  ", indent) .. k .. ": "
    if type(v) == "table" then
      print(formatting)
      tprint(v, indent+1)
    elseif type(v) == 'boolean' then
      print(formatting .. tostring(v))
    else
      print(formatting .. v)
    end
  end
end

local euler_constanct = math.exp(1.0);

function sigmoid(x)
  return (1 / (1 + math.pow(euler_constanct,-x)));
end


