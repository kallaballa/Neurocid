require "vector2d" 

maxDistance = math.sqrt(math.pow(600000,2),math.pow(600000,2))
fitness = 0;

ship = nc.ship

local center = Vector2D:new(300000,300000);
local range = 0;  
local loc = Vector2D:new(ship.loc.x, ship.loc.y);
local dist = Vector2D:Sub(center,loc):magnitude();
fitness = 1.0 - math.abs((range - dist) / maxDistance);

return fitness;
