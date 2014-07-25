require("vector2d")
require("ncutils")

local fuzz = nc.pl.fuzz;

-- place A
for i, s in pairs(nc.teamA.ships) do
  s.loc.x = s.loc.x + fRand(-s.layout.radius * fuzz, s.layout.radius * fuzz);
  s.loc.y = s.loc.y + fRand(-s.layout.radius * fuzz, s.layout.radius * fuzz);
end

for i, f in pairs(nc.teamA.facilities) do
  f.loc.x = f.loc.x + fRand(-f.layout.radius * fuzz, f.layout.radius * fuzz);
  f.loc.y = f.loc.y + fRand(-f.layout.radius * fuzz, f.layout.radius * fuzz);
end

for i, s in pairs(nc.teamB.ships) do
  s.loc.x = s.loc.x + fRand(-s.layout.radius * fuzz, s.layout.radius * fuzz);
  s.loc.y = s.loc.y + fRand(-s.layout.radius * fuzz, s.layout.radius * fuzz);
end

for i, f in pairs(nc.teamB.facilities) do
  f.loc.x = f.loc.x + fRand(-f.layout.radius * fuzz, f.layout.radius * fuzz);
  f.loc.y = f.loc.y + fRand(-f.layout.radius * fuzz, f.layout.radius * fuzz);
end

return nc 
