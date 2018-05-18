#include "flying.h"
#include "comps.h"
#include "ecs.hpp"
#include "vec_math.h"
#include "imgui.h"

const float HALF_PI = (float)(M_PI / 2.0);

float
angle_to_mouse(const WVec& mouse, const Transform& t)
{

  auto local_to = inversed(t, mouse - t.position);
  return -t.direction * atan2f(-local_to.x, local_to.y);
}

float
drag(float angle, float vel)
{
  if (angle < 0) {
    return drag(-angle, vel);
  }
  float fac = fmax(1, exp(vel / 700));
  if (angle < HALF_PI) {
    return fac * expf(-powf(angle - HALF_PI, 2.0f) * 1.4f);
  }
  return fac * expf(powf(angle - HALF_PI, 2.0f) * 0.1f);
}

float
lift(float angle, float stall)
{
  assert(angle <= M_PI);
  if (angle < 0) {
    return -lift(-angle, stall);
  }
  if (angle < stall) {
    return sin(angle * HALF_PI / stall);
  } else if (angle < HALF_PI) {
    return cos((angle - stall) * HALF_PI / (HALF_PI - stall));
  }
  return -0.3 * lift((float)M_PI - angle, stall);
}

void
flying(CanFly,
       const Flying& fc,
       Movement& mc,
       const Transform& pc,
       const Input& ic)
{}

void
hover(const Transform& pc, Movement& mc, const Input& ic)
{
  mc.next_accel.y = copysignf(1, ic.mouse.y - pc.position.y) * 80.0f;
  if (abs(ic.mouse.y - pc.position.y) > 10) {
    mc.next_accel -= mc.velocity * 2.0f;
  } else {
    mc.next_accel.x -= mc.velocity.x * 4.0f;
  }
  mc.next_accel.y -= 5.0;
}

void
dummy_flying(const Transform& pc, Movement& mc, const Input& ic)
{
  mc.next_accel.y -= 60.;

  WVec air_dir = w_normalize(mc.velocity);
  float vel = w_magnitude(mc.velocity);

  float vel_squ = w_dot(mc.velocity, mc.velocity);
  auto glide_dir = w_rotated({ 0, 1 }, pc.rotation * pc.direction);
  auto angle = w_angle_to_vec(mc.velocity, glide_dir);
  ImGui::Text("%f", angle);

  mc.next_accel -= air_dir * vel_squ * drag(angle, vel) * 0.0067f;
  mc.next_accel += w_tangent(air_dir) * vel_squ * lift(angle, 0.26) * 0.007f;
  mc.next_accel += 120.0f * glide_dir;
  //
  // rotations
  mc.change_angle = angle_to_mouse(ic.mouse, pc);

  ImGui::Text("%f, %f", glide_dir.x, glide_dir.y);
}
