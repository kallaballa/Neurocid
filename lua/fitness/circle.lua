require "vector2d" 

maxDistance = math.sqrt(math.pow(600000,2),math.pow(600000,2))
fitness = 0;

ship = nc.ship

local minDist = 9999999;
for i, so in pairs(ship.scan.objects) do
 	if so.type == 3 then
   	minDist = math.min(minDist, so.dist);
	end
end
if minDist < 9999999 then
 	assert(minDist <= maxDistance);
 	fitness = (0.01 - (minDist/maxDistance) / 100);
  if ship.fuel > 0 then
		local linearVel = Vector2D:new(ship.vel.x, ship.vel.y);
  	print(linearVel:magnitude());
		fitness = fitness * ((1.0 - (linearVel:magnitude()/151) + 1));
	end
end


if ship.dead then
	fitness = fitness / 2
end

return fitness;
