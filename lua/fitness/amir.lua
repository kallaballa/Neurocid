require "vector2d" 
require "ncutils"

-- recursivly print the ship table
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


-- the following global variables are required by the c++ code and will be read to build a performance descriptor.
offensiveRatio = -1;
defensiveRatio = -1;
friendlyRatioInv = -1;
aimRatio = 0.0;

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

if shots == 0 or ratedProjectiles == 0 then
  fitness = 0;
else
  if not layout.disableProjectilesFitness then
		aimRatio = (1.0 - (totalDiff / ratedProjectiles));
		assert(aimRatio >= 0.0);
		assert(aimRatio <= 1.0);
	end

	friendlyRatioInv = 2.0 - ((ship.friendlyFire / shots) / 10);
	local damageRatioInv = 2.0 - (ship.damage / layout.maxDamage);
	local fDamageRatioInv = 2.0 - (nc.facility.damage / nc.facility.layout.maxDamage)
  offensiveRatio = (ship.offensiveHits / shots) / 10; -- FIXME: we need the blast layout
  defensiveRatio = (ship.defensiveHits / shots) / 10;
	
  assert(aimRatio >= 0.0);
  assert(aimRatio <= 1.0);
  assert(damageRatioInv >= 0.0);
  assert(damageRatioInv <= 2.0);
  assert(friendlyRatioInv >= 0.0);
  assert(friendlyRatioInv <= 2.0);
  assert(offensiveRatio >= 0.0);
  assert(offensiveRatio <= 1.0);
  assert(defensiveRatio >= 0.0);
  assert(defensiveRatio <= 1.0);

  local aim = (aimRatio / (ship.failedShots + 1));
  local taken = damageRatioInv * friendlyRatioInv * fDamageRatioInv;
  local offense = ship.offensiveHits;
  local defense = ship.defensiveHits;
  local score = (offense + defense) * taken;

  fitness = aim + score;

  if ship.dead then
      fitness = fitness / 2.0;
	end

--  if nc.winner then
--      fitness = math.pow(fitness,0.9);
--  end

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
