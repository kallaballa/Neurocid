require "vector2d" 

maxDistance = math.sqrt(math.pow(600000,2),math.pow(600000,2))
fitness = 0;

ship = nc.ship

local linearVel = Vector2D:new(ship.history.avgVel.x, ship.history.avgVel.y);
print(linearVel:magnitude());
fitness = ((1.0 - (linearVel:magnitude()/151) ));

return fitness;
