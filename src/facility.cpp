#include "facility.hpp"
#include "ship.hpp"

namespace neurocid {

void Facility::move(BattleFieldLayout& bfl) {
}

void Facility::resetGameState() {
  dead_ = false;
  explode_ = false;
  crashed_ = false;
}

void Facility::resetScore() {
  damage_ = 0;
  crash_ = 0;
  crashDamage_ = 0;
  recharged_ = 0;
}

void Facility::damage() {
  damage_++;
  if (damage_ >= layout_.maxDamage_) {
    death();
  }
}

void Facility::death() {
  damage_ = layout_.maxDamage_;
  dead_ = true;
  explode_ = true;
}

void Facility::crash() {
  crash_++;
  crashDamage_++;
  crashed_ = true;

  if (crashDamage_ >= layout_.crashesPerDamage_) {
    crashDamage_ = 0;
    damage();
  }
}

bool Facility::recharge(Ship& s) {
//  if (recharged_ < layout_.maxRecharge_) {
    Coord amount = (s.layout_.maxFuel_ - s.fuel_);
    amount += (s.layout_.maxAmmo_ - s.ammo_) * 10;
    CHECK(amount >= 0);
    s.recharged_ += amount;
    s.fuel_ = s.layout_.maxFuel_;
    s.ammo_ = s.layout_.maxAmmo_;
    recharged_ += amount;
    return true;
//  }
//  return false;
}
}
