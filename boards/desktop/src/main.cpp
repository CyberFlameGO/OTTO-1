#include "lib/itc/itc.hpp"
#include "lib/util/with_limits.hpp"

#include <Gamma/Oscillator.h>

#include "app/services/audio.hpp"
#include "app/services/config.hpp"
#include "app/services/controller.hpp"
#include "app/services/graphics.hpp"
#include "app/services/impl/runtime.hpp"
#include "app/services/logic_thread.hpp"

using namespace otto;
using namespace otto::services;

namespace otto::engines {
  struct IDrawable {
    virtual void draw(SkCanvas& ctx) noexcept = 0;
  };

  struct IScreen : IDrawable {};

  /// InputHandler linked to a logic component
  template<typename Logic>
  struct LinkedInputHandler : InputHandler {
    LinkedInputHandler(Logic& l) : logic(l) {}

    auto produce(auto&&... actions) requires requires(Logic& l)
    {
      l.produce(FWD(actions)...);
    }
    {
      return logic.produce(FWD(actions)...);
    }

  protected:
    Logic& logic;
  };

  namespace simple {
    struct State {
      util::StaticallyBounded<float, 11, 880> freq = 340;
    };

    struct Logic final : itc::Producer<State> {
      using Producer::Producer;
    };

    struct Handler final : LinkedInputHandler<Logic> {
      using LinkedInputHandler::LinkedInputHandler;

      void handle(EncoderEvent e) noexcept final
      {
        produce(itc::increment(&State::freq, e.steps));
      }
    };

    struct Audio final : services::Audio::Consumer<State>, core::ServiceAccessor<services::Audio> {
      using Consumer::Consumer;

      void on_state_change(const State& s) noexcept override
      {
        osc.freq(s.freq);
      }
      util::audio_buffer process() noexcept
      {
        auto buf = service<services::Audio>().buffer_pool().allocate();
        std::ranges::generate(buf, osc);
        return buf;
      }
      gam::Sine<> osc;
    };

    struct Screen final : services::Graphics::Consumer<State>, otto::engines::IScreen {
      using Consumer::Consumer;

      void draw(SkCanvas& ctx) noexcept override
      {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(SK_ColorWHITE);
        paint.setStyle(SkPaint::kFill_Style);
        ctx.drawCircle({160, 120}, state().freq.normalize() * 100.f, paint);
      }
    };
  } // namespace simple

  template<typename T>
  concept AnEngine = requires
  {
    typename T::State;
    typename T::Logic;
    typename T::Audio;
    typename T::Screen;
    typename T::Handler;
  }
  &&itc::AState<typename T::State>;

  struct Simple {
    using State = simple::State;
    using Logic = simple::Logic;
    using Audio = simple::Audio;
    using Screen = simple::Screen;
    using Handler = simple::Handler;
  };

  static_assert(AnEngine<Simple>);

}; // namespace otto::engines

int main(int argc, char* argv[])
{
  using namespace services;
  auto app = start_app(ConfigManager::make_default(), //
                       LogicThread::make_default(),   //
                       Controller::make_board(),      //
                       Audio::make_board(),           //
                       Graphics::make_board()         //
  );

  itc::Channel<otto::engines::Simple::State> chan;
  engines::Simple::Logic l(chan);
  engines::Simple::Audio a(chan);
  engines::Simple::Screen s(chan);
  engines::Simple::Handler h(l);

  app.service<Audio>().set_process_callback([&](auto& data) {
    const auto res = a.process();
    std::ranges::copy(util::zip(res, res), data.output.begin());
  });
  app.service<Graphics>().show([&s](SkCanvas& ctx) { s.draw(ctx); });
  app.service<Controller>().set_input_handler(h);

  app.wait_for_stop();
}
