#include <gtest/gtest.h>
#include <filesystem>

class Environment : public ::testing::Environment {
 public:
  ~Environment() override {}

  void SetUp() override {}

  void TearDown() override {
    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path())) {
      if (entry.is_regular_file()) {
        const auto& path = entry.path();
        if (path.extension() == std::string(".db")) {
          std::filesystem::remove(path);
        }
      }
    }
  }
};

int main(int argc, char **argv) {
  ::testing::AddGlobalTestEnvironment(new Environment());
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}