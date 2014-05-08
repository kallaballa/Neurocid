require("vector2d")
require("ncutils")

--tprint(nc, 2);

local tick = nc.tick;
local pl = nc.pl;
local shipsA = nc.teamA.ships;
local shipsB = nc.teamB.ships;
local facA = nc.teamA.facilities;
local facB = nc.teamB.facilities;
local shipsA = nc.teamA.ships;
local shipsB = nc.teamB.ships;
local facA = nc.teamA.facilities;
local facB = nc.teamB.facilities;

local axisDir = DirFromRad(pl.rotation);
local sideDirA = axisDir:copy();
sideDirA:rotate(90);
local sideDirB = sideDirA:copy();
sideDirB:rotate(180);

local centerA = Vector2D:new(pl.center.x, pl.center.y);
local centerB = Vector2D:new(pl.center.x, pl.center.y);

centerA = Vector2D:Add(centerA,(Vector2D:Mult(axisDir,(pl.distance/2))));
centerB = Vector2D:Sub(centerB,(Vector2D:Mult(axisDir,(pl.distance/2))));

local spacingA = shipsA[1].layout.radius + pl.spacing;
local spacingB = shipsB[1].layout.radius + pl.spacing;

local lengthA = spacingA * size(shipsA) / 2;
local lengthB = spacingB * size(shipsB) / 2;

local startA = Vector2D:Add(centerA, Vector2D:Mult(sideDirA, fRand(-lengthA, lengthA) * 3));
local startB = Vector2D:Add(centerB, Vector2D:Mult(sideDirB, fRand(-lengthB, lengthB) * 3));

-- place A
for i, s in pairs(shipsA) do
	s.loc = startA:copy();
  s.loc:sub(Vector2D:Mult(sideDirA, (s.layout.radius * 2 + pl.spacing) * i));
  s.loc:add(Vector2D:Mult(axisDir, math.random(-s.layout.radius, s.layout.radius)));
  s.rotation = fRand(-math.pi, math.pi);
end

local transDir = sideDirA:copy();
if math.random() < 0.5 then
	transDir:rotate(180);
end

if size(facA) > 0 then 
	local startFA = startA:copy();
	local range =  facA[1].layout.radius;
	startFA:add(Vector2D:Mult(transDir, math.random(0, range * 8)));
	startFA:add(Vector2D:Mult(axisDir, math.random(range * 5, range * 10)));

	for i, f in pairs(facA) do
		f.loc = startFA:copy();
		f.loc:sub(Vector2D:Mult(transDir, (range * 6 + pl.spacing) * i));
		f.loc:add(Vector2D:Mult(axisDir, math.random(0, range)));
	end
end

-- place B
for i, s in pairs(shipsB) do
  s.loc = startB:copy();
  s.loc:sub(Vector2D:Mult(sideDirB, (s.layout.radius * 2 + pl.spacing) * i));
  s.loc:add(Vector2D:Mult(axisDir, math.random(-s.layout.radius, s.layout.radius)));
  s.rotation = fRand(-math.pi, math.pi);
end

axisDir:rotate(180);
transDir = sideDirB:copy();

if math.random() < 0.5 then
	transDir:rotate(180);
end

if size(facB) > 0 then 
	local startFB = startB:copy();
	local range =  facB[1].layout.radius;
	startFB:add(Vector2D:Mult(transDir, math.random(0, range * 8)));
	startFB:add(Vector2D:Mult(axisDir, math.random(range * 5, range * 10)));

	for i, f in pairs(facB) do
		f.loc = startFB:copy();
  	f.loc:sub(Vector2D:Mult(transDir, (range * 6 + pl.spacing) * i));
  	f.loc:add(Vector2D:Mult(axisDir, math.random(0, range)));
	end
end

return nc 
