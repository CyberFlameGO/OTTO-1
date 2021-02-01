#pragma once

#include <memory_resource>
#include <unordered_map>

#include "lib/util/crtp.hpp"
#include "lib/util/serialization.hpp"
#include "lib/util/string_ref.hpp"
#include "lib/util/visitor.hpp"

#include "lib/core/service.hpp"
#include "lib/toml.hpp"

#include "app/services/runtime.hpp"

namespace otto {
  template<typename Conf>
  struct ConfHandle;

  /// CRTP Base class for Config objects.
  template<typename Derived>
  struct Config {
    using Handle = ConfHandle<Derived>;
    /// The name as it will be shown in the config file
    ///
    /// Can be overridden in `Derived` by defining a similar constant.
    static constexpr util::string_ref name = util::qualified_name_of<Derived>;

    [[nodiscard]] util::string_ref get_name() const noexcept
    {
      return Derived::name;
    }
  };

  template<typename T>
  concept AConfig = std::is_base_of_v<Config<T>, T>&& util::ASerializable<T>;

  namespace services {

    struct ConfigManager : core::Service<ConfigManager> {
      ConfigManager() = default;
      /// Initialize the configurations from a toml object
      ConfigManager(toml::value config_data);
      /// Initialize the configurations from a toml file
      ConfigManager(const std::filesystem::path& config_path);

      ~ConfigManager();

      void write_to_file();

      /// Construct a config type, and initialize it from the config data
      ///
      /// Also adds the constructed data back to the stored toml
      template<AConfig Conf>
      Conf make_conf() noexcept;

      /// Attempts to read the default config file `./ottoconf.toml`.
      ///
      /// If this file is not found, simply runs the default constructor
      static core::ServiceHandle<ConfigManager> make_default()
      {
        std::filesystem::path config_path = "./ottoconf.toml";
        if (std::filesystem::is_regular_file(config_path)) {
          return make(config_path);
        }
        LOGI("Config file {} not found", config_path.c_str());
        return make();
      }

      [[nodiscard]] toml::value into_toml() const;

    private:
      template<AConfig Conf>
      static constexpr const char* key_of() noexcept;

      std::filesystem::path file_path = "";
      [[no_unique_address]] core::ServiceAccessor<Runtime> runtime;
      toml::value config_data_ = toml::table();
    };
  } // namespace services


  template<AConfig Conf>
  struct ConfHandle<Conf> {
    ConfHandle() noexcept
      : ConfHandle([&] {
          if (confman.is_active()) return confman->make_conf<Conf>();
          return Conf{};
        }())
    {}
    ConfHandle(Conf c) noexcept : conf(std::move(c)) {}

    const Conf* operator->() const noexcept
    {
      return &conf;
    }

    const Conf& operator*() const noexcept
    {
      return conf;
    }

  private:
    [[no_unique_address]] core::UnsafeServiceAccessor<services::ConfigManager> confman;
    Conf conf;
  };

} // namespace otto

namespace toml {}

// Implementations:
namespace otto::services {

  inline ConfigManager::ConfigManager(toml::value config_data) : config_data_(std::move(config_data)) {}

  inline ConfigManager::ConfigManager(const std::filesystem::path& config_path)
  try : ConfigManager(toml::parse(config_path)) {
    file_path = config_path;
  } catch (std::runtime_error& e) {
    LOGE("Error reading config file:");
    LOGE("{}", e.what());
  }

  inline void ConfigManager::write_to_file()
  {
    if (file_path.empty()) return;
    LOGI("Writing config to {}", file_path.c_str());
    std::ofstream file;
    file.open(file_path);
    file << into_toml();
    file.close();
  }

  inline ConfigManager::~ConfigManager()
  {
    if (!file_path.empty()) write_to_file();
  }

  inline toml::value ConfigManager::into_toml() const
  {
    return config_data_;
  }

  template<AConfig Conf>
  Conf ConfigManager::make_conf() noexcept
  {
    Conf res;
    auto& data = config_data_[res.get_name().c_str()];
    try {
      if (data.type() != toml::value_t::empty) util::deserialize_from(data, res);
    } catch (const std::out_of_range& e) {
      LOGI("Error in config {}, using default: ", res.get_name());
      LOGI("{}", e.what());
    } catch (const std::exception& e) {
      LOGW("Error in config {}, using default: ", res.get_name());
      LOGW("{}", e.what());
    }
    try {
      util::serialize_into(data, res);
    } catch (const std::exception& e) {
      LOGW("Error serializing config, overwriting");
      data = util::serialize(res);
    }
    return res;
  }

  template<AConfig Conf>
  constexpr const char* ConfigManager::key_of() noexcept
  {
    return util::qualified_name_of<Conf>.c_str();
  }

} // namespace otto::services
