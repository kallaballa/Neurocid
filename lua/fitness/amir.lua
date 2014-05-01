require "vector2d" 
require "ncutils"

-- tprint(nc.ship, 2)

local width = nc.bfl.width;
local height = nc.bfl.height;
local totalDiff = 0.0;
local ratedProjectiles = 0;
local maxDistance = math.sqrt(math.pow(width,2),math.pow(height,2))
local ship = nc.ship;
local layout = nc.ship.layout;
local projectiles = nc.ship.projectiles;
local shots = size(projectiles);
local fitness = 0;

if not layout.disableProjectileFitness then
	for i, p in pairs(projectiles) do 
		if size(p.scan.objects) == 0 then
		  ratedProjectiles = ratedProjectiles + 1;	
		  totalDiff = totalDiff + 1;
	  else
			assert(size(p.scan.objects) == 2);
			assert(p.scan.objects[1].type == 1);
			assert(p.scan.objects[2].type == 0);
	
			for j, so in pairs(p.scan.objects) do
				if so.type == 1 then
			  local loc = Vector2D:new(so.loc.x, so.loc.y);
				local ploc = Vector2D:new(p.loc.x, p.loc.y);
				local pstartLoc = Vector2D:new(p.startLoc.x, p.startLoc.y)
				local perfect = Vector2D:Normalize(Vector2D:Sub(loc, pstartLoc));
				local candidate = Vector2D:Normalize(Vector2D:Sub(ploc, pstartLoc));
				local vdiff = candidate:copy();
				vdiff:rotateBy(perfect);
				local angDistPerfect = math.abs(RadFromDir(vdiff)) / math.pi;
	
	      assert(angDistPerfect >= 0.0);
	      assert(angDistPerfect <= 1.0);
	
				local distance = so.dist;
				if distance > maxDistance then
					distance = maxDistance;
				end
	
				local distPerfect = (distance/maxDistance);
				local diffPerfect = distPerfect * angDistPerfect
				assert(diffPerfect >= 0.0);
	      assert(diffPerfect <= 1.0);
	
				local startDist = Vector2D:Sub(loc, pstartLoc):magnitude();
	
				if startDist < p.layout.maxTravel * 2 then
					diffPerfect = diffPerfect / 2
				end

				totalDiff = totalDiff + diffPerfect;
      	ratedProjectiles = ratedProjectiles + 1;
			end
			end
  	end
	end
end

if shots == 0 then
  fitness = 0;
else
  assert(ratedProjectiles > 0 or layout.disableProjectilesFitness);
  local aimRatio = 0.0
  if not layout.disableProjectilesFitness then
		aimRatio = (1.0 - (totalDiff / ratedProjectiles));
		assert(aimRatio >= 0.0);
		assert(aimRatio <= 1.0);
	end

	local failRatio = ship.failedShots / shots;
	local hitRatio = ship.hits / shots;
	local rechargeRatio = ship.recharged/layout.maxFuel;
	local friendlyRatioInv = 1.0 - ship.friendlyFire / shots;
	local damageRatioInv = 1.0 / ((ship.damage / layout.maxDamage) + 1);

  assert(hitRatio >= 0.0);
  assert(hitRatio <= 1.0);
  assert(damageRatioInv >= 0.5);
  assert(damageRatioInv <= 1.0);
  assert(friendlyRatioInv >= 0.0);
  assert(friendlyRatioInv <= 1.0);

	local aim = (aimRatio / (failRatio + 1));
  local dealt = damageRatioInv * friendlyRatioInv;
  local inflicted = (ship.hits * 3) + (ship.kills * 6);
	local score = inflicted * dealt

  if ship.captured > 0 then
    inflicted = inflicted * 2;
  end

  fitness = aim + inflicted;

  if ship.dead then
      fitness = fitness / 2.0;
	end
end

if fitness == 0 and ship.fuel == 0 then
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
end

return fitness;
