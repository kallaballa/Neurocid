require("vector2d")
require("ncutils")

tprint(nc, 2);

local tick = nc.tick;
local sl = nc.sl;
local shipsA = nc.teamA.ships;
local shipsB = nc.teamB.ships;
local facA = nc.teamA.facilities;
local facB = nc.teamB.facilities;
local shipsA = nc.teamA.ships;
local shipsB = nc.teamB.ships;
local facA = nc.teamA.facilities;
local facB = nc.teamB.facilities;

local axisDir = DirFromRad(sl.rotation);
local sideDirA = axisDir:copy();
sideDirA:rotate(90);
local sideDirB = sideDirA:copy();
sideDirB:rotate(180);

local centerA = Vector2D:new(sl.center.x, sl.center.y);
local centerB = Vector2D:new(sl.center.x, sl.center.y);

centerA = Vector2D:Add(centerA,(Vector2D:Mult(axisDir,(sl.distance/2))));
centerB = Vector2D:Sub(centerB,(Vector2D:Mult(axisDir,(sl.distance/2))));

local spacingA = shipsA[1].layout.radius + sl.spacing;
local spacingB = shipsB[1].layout.radius + sl.spacing;

local lengthA = spacingA * (size(shipsA) - 1) / 2;
local lengthB = spacingB * (size(shipsB) - 1) / 2;

local startA = Vector2D:Add(centerA, Vector2D:Mult(sideDirA, lengthA));
local startB = Vector2D:Add(centerB, Vector2D:Mult(sideDirB, lengthB));

for i, s in pairs(shipsA) do
	s.loc = startA:copy();
  s.loc:sub(Vector2D:Mult(sideDirA, (s.layout.radius * 2 + sl.spacing) * i));
  s.loc:add(Vector2D:Mult(axisDir, math.random(-s.layout.radius, s.layout.radius)));
end

local transDir = sideDirA:copy();
if math.random(0,1) < 0.5 then
	transDir:rotate(180);
end

local startFA = startA:copy();
local range =  facA[1].layout.range;
startFA:add(Vector2D:Mult(transDir, math.random(0, range * 8)));
startFA:add(Vector2D:Mult(axisDir, math.random(range * 5, range * 10)));

for i, f in pairs(facA) do
	f.loc = startFA:copy();
	f.loc:sub(Vector2D:Mult(transDir, (range * 6 + sl.spacing) * i));
	f.loc:add(Vector2D:Mult(axisDir, math.random(0, range)));
end

for i, s in pairs(shipsB) do
  s.loc = startB:copy();
  s.loc:sub(Vector2D:Mult(sideDirB, (s.layout.radius * 2 + sl.spacing) * i));
  s.loc:add(Vector2D:Mult(axisDir, math.random(-s.layout.radius, s.layout.radius)));
end

axisDir:rotate(180);
transDir = sideDirB:copy();

if math.random(0,1) < 0.5 then
	transDir:rotate(180);
end

local startFB = startB:copy();
local range =  facB[1].layout.range;
startFB:add(Vector2D:Mult(transDir, math.random(0, range * 8)));
startFB:add(Vector2D:Mult(axisDir, math.random(range * 5, range * 10)));

for i, f in pairs(facB) do
  f.loc = startFB:copy();
  f.loc:sub(Vector2D:Mult(transDir, (range * 6 + sl.spacing) * i));
  f.loc:add(Vector2D:Mult(axisDir, math.random(0, range)));
end

return nc 
