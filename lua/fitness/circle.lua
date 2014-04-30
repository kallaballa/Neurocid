require "vector2d" 

maxDistance = math.sqrt(math.pow(600000,2),math.pow(600000,2))
fitness = 0;

ship = neurocid.ship
if ship.fuel == 0 then
  local minDist = 9999999;
  for i, so in pairs(ship.scan.objects) do
    if so.type == 3 then
      minDist = math.min(minDist, so.dist);
    end
  end
  if minDist < 9999999 then
    assert(minDist <= maxDistance);
    fitness = fitness + (0.01 - ((minDist/maxDistance) / 100.0));
  end
else
	center = Vector2D:new(300000,300000);
	radius = 0;
	ship = neurocid.ship
	loc = Vector2D:new(ship.loc.x, ship.loc.y);
	distCenter = Vector2D:Sub(loc, center):magnitude();
	fitness = 1.0 - math.abs((radius/maxDistance) - (distCenter / maxDistance));
end

return fitness;
