#include "filter_menu.h"

#include "game/graphics/gfx.h"

#include "fmt/format.h"
#include "third-party/imgui/imgui.h"
#include "third-party/imgui/imgui_stdlib.h"
#include <cstdlib>   // for rand
#include <ctime>     // for seeding rand

void to_json(json& j, const DebugTextFilter& obj) {
  j = json{{"content", obj.content}, {"type", obj.type}};
}

void from_json(const json& j, DebugTextFilter& obj) {
  j.at("content").get_to(obj.content);
  j.at("type").get_to(obj.type);
}


// Globals
std::vector<std::string> g_saved_filters;
int g_random_value = 0;  // starts at 0

FiltersMenu::FiltersMenu() {
  // Initialize defaults if needed
  g_random_value = 0;  // from the random/reset feature we added
}

void FiltersMenu::draw_window() {
  ImGui::Begin("Filters");

  ImGui::SetNextItemOpen(true);
  if (ImGui::TreeNode("Warp Randomizer Seed")) {
    auto& current_filters = Gfx::g_debug_settings.text_filters;

    // Ensure global matches size of current_filters
    if (g_saved_filters.size() != current_filters.size()) {
      g_saved_filters.resize(current_filters.size());
    }

    // Iterate and display all current debug text filters
    for (size_t i = 0; i < current_filters.size(); i++) {
      std::string label = "random-seed";
      if (current_filters[i].type == DebugTextFilter::Type::NOT_CONTAINS) {
        label = "not-contains?";
      } else if (current_filters[i].type == DebugTextFilter::Type::REGEX) {
        label = "regex?";
      }

      ImGui::Text("%s", label.c_str());
      ImGui::SameLine();

      // InputText will directly update current_filters[i].content
      if (ImGui::InputText(fmt::format("##filter-{}", i).c_str(), &current_filters[i].content)) {
        // Save into global whenever edited
        g_saved_filters[i] = current_filters[i].content;
      }
    }

    // if (ImGui::Button("Add Contains")) {
    //   DebugTextFilter new_filter;
    //   new_filter.type = DebugTextFilter::Type::CONTAINS;
    //   current_filters.push_back(new_filter);
    //   g_saved_filters.push_back("");  // keep globals in sync
    // }
    // ImGui::SameLine();
    // if (ImGui::Button("Add Not-Contains")) {
    //   DebugTextFilter new_filter;
    //   new_filter.type = DebugTextFilter::Type::NOT_CONTAINS;
    //   current_filters.push_back(new_filter);
    //   g_saved_filters.push_back("");  // keep globals in sync
    // }
    // if (ImGui::Button("Clear Filters")) {
    //   current_filters.clear();
    //   g_saved_filters.clear();
    // }

    // if (ImGui::Checkbox("Enable Distance Check", &Gfx::g_debug_settings.text_check_range)) {
    // }
    // if (Gfx::g_debug_settings.text_check_range) {
    //   if (ImGui::SliderFloat("Max Range", &Gfx::g_debug_settings.text_max_range, 0, 250)) {
    //   }
    // }

    // --- NEW RANDOM + RESET BUTTONS ---
    ImGui::Separator();
    if (ImGui::Button("Random")) {
      for (auto& filter : current_filters) {
        if (filter.type == DebugTextFilter::Type::CONTAINS) {
          g_random_value = rand() % 100001;  // 0 - 100000
          filter.content = fmt::format("{}", g_random_value);
          break; // Only apply to the first 'contains' filter
        }
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
      for (auto& filter : current_filters) {
        if (filter.type == DebugTextFilter::Type::CONTAINS) {
          filter.content.clear();
          break; // Only clear the first 'contains' filter
        }
      }
    }
    // ----------------------------------

    ImGui::TreePop();
  }

  ImGui::End();
}