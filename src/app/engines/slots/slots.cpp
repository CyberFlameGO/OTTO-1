#include "slots.hpp"

namespace otto::engines::slots {

  void Logic::on_state_change(const SoundSlotsState& state) noexcept
  {
    tl::optional old = idx.old_val();
    if (idx.check_changed(state.active_idx)) {
      // Serialize current
      if (old) data_.json_objects[*old] = util::serialize(_ctx);
      // Deserialize new
      util::deserialize_from(data_.json_objects[state.active_idx], _ctx);
    }
  }

  void Logic::set_managed(util::DynSerializable&& ctx)
  {
    _ctx = std::move(ctx);
  }
} // namespace otto::engines::slots
