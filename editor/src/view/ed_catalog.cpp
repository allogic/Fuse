#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_dockspace.h>

#include <editor/view/ed_catalog.h>

static void ed_catalog_view_draw_swapchain(ed_catalog_view_t *catalog, uint64_t asset_index);
static void ed_catalog_view_draw_renderer(ed_catalog_view_t *catalog, uint64_t asset_index);
static void ed_catalog_view_draw_pipeline(ed_catalog_view_t *catalog, uint64_t asset_index);
static void ed_catalog_view_draw_model(ed_catalog_view_t *catalog, uint64_t asset_index);
static void ed_catalog_view_draw_scene(ed_catalog_view_t *catalog, uint64_t asset_index);

static void ed_catalog_view_draw_asset_buttons(ed_catalog_view_t *catalog);

ed_catalog_view_t *ed_catalog_view_create(eg_context_t *context, asset_type_t asset_type) {
  ed_catalog_view_t *catalog = (ed_catalog_view_t *)heap_alloc(sizeof(ed_catalog_view_t), 1, 0);

  catalog->base.context = context;
  catalog->base.is_dirty = 0;
  catalog->base.is_open = 1;
  catalog->base.is_docked = 0;

  catalog->selected_asset = -1;
  catalog->asset_type = asset_type;

  switch (catalog->asset_type) {
    case ASSET_TYPE_SWAPCHAIN: {

      catalog->assets = database_load_swapchain_assets();

      break;
    }
    case ASSET_TYPE_RENDERER: {

      catalog->assets = database_load_renderer_assets();

      break;
    }
    case ASSET_TYPE_PIPELINE: {

      catalog->assets = database_load_pipeline_assets();

      break;
    }
    case ASSET_TYPE_MODEL: {

      catalog->assets = database_load_model_assets();

      break;
    }
    case ASSET_TYPE_SCENE: {

      catalog->assets = database_load_scene_assets();

      break;
    }
  }

  return catalog;
}
void ed_catalog_view_refresh(ed_catalog_view_t *catalog) {
  switch (catalog->asset_type) {
    case ASSET_TYPE_SWAPCHAIN: {

      database_destroy_swapchain_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_RENDERER: {

      database_destroy_renderer_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_PIPELINE: {

      database_destroy_pipeline_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_MODEL: {

      database_destroy_model_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_SCENE: {

      database_destroy_scene_assets(&catalog->assets);

      break;
    }
  }

  switch (catalog->asset_type) {
    case ASSET_TYPE_SWAPCHAIN: {

      catalog->assets = database_load_swapchain_assets();

      break;
    }
    case ASSET_TYPE_RENDERER: {

      catalog->assets = database_load_renderer_assets();

      break;
    }
    case ASSET_TYPE_PIPELINE: {

      catalog->assets = database_load_pipeline_assets();

      break;
    }
    case ASSET_TYPE_MODEL: {

      catalog->assets = database_load_model_assets();

      break;
    }
    case ASSET_TYPE_SCENE: {

      catalog->assets = database_load_scene_assets();

      break;
    }
  }
}
void ed_catalog_view_draw(ed_catalog_view_t *catalog) {
  static ImGuiTableFlags table_flags =
    ImGuiTableFlags_BordersInner |
    ImGuiTableFlags_RowBg |
    ImGuiTableFlags_Resizable |
    ImGuiTableFlags_SizingStretchProp;

  if (ImGui::BeginTable("##AssetTable", 2, table_flags)) {

    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableHeadersRow();

    uint64_t asset_index = 0;
    uint64_t asset_count = vector_count(&catalog->assets);

    while (asset_index < asset_count) {

      switch (catalog->asset_type) {
        case ASSET_TYPE_SWAPCHAIN: {

          ed_catalog_view_draw_swapchain(catalog, asset_index);

          break;
        }
        case ASSET_TYPE_RENDERER: {

          ed_catalog_view_draw_renderer(catalog, asset_index);

          break;
        }
        case ASSET_TYPE_PIPELINE: {

          ed_catalog_view_draw_pipeline(catalog, asset_index);

          break;
        }
        case ASSET_TYPE_MODEL: {

          ed_catalog_view_draw_model(catalog, asset_index);

          break;
        }
        case ASSET_TYPE_SCENE: {

          ed_catalog_view_draw_scene(catalog, asset_index);

          break;
        }
      }

      asset_index++;
    }

    ImGui::EndTable();
  }
}
void ed_catalog_view_destroy(ed_catalog_view_t *catalog) {
  switch (catalog->asset_type) {
    case ASSET_TYPE_SWAPCHAIN: {

      database_destroy_swapchain_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_RENDERER: {

      database_destroy_renderer_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_PIPELINE: {

      database_destroy_pipeline_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_MODEL: {

      database_destroy_model_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_SCENE: {

      database_destroy_scene_assets(&catalog->assets);

      break;
    }
  }

  heap_free(catalog);
}

static void ed_catalog_view_draw_swapchain(ed_catalog_view_t *catalog, uint64_t asset_index) {
  swapchain_asset_t *swapchain_asset = (swapchain_asset_t *)vector_at(&catalog->assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", swapchain_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(swapchain_asset->name, asset_index == catalog->selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    catalog->selected_asset = asset_index;
  }
}
static void ed_catalog_view_draw_renderer(ed_catalog_view_t *catalog, uint64_t asset_index) {
  renderer_asset_t *renderer_asset = (renderer_asset_t *)vector_at(&catalog->assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", renderer_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(renderer_asset->name, asset_index == catalog->selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    catalog->selected_asset = asset_index;
  }
}
static void ed_catalog_view_draw_pipeline(ed_catalog_view_t *catalog, uint64_t asset_index) {
  pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&catalog->assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", pipeline_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(pipeline_asset->name, asset_index == catalog->selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    catalog->selected_asset = asset_index;
  }
}
static void ed_catalog_view_draw_model(ed_catalog_view_t *catalog, uint64_t asset_index) {
  model_asset_t *model_asset = (model_asset_t *)vector_at(&catalog->assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", model_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(model_asset->name, asset_index == catalog->selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    catalog->selected_asset = asset_index;
  }
}
static void ed_catalog_view_draw_scene(ed_catalog_view_t *catalog, uint64_t asset_index) {
  scene_asset_t *scene_asset = (scene_asset_t *)vector_at(&catalog->assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", scene_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(scene_asset->name, asset_index == catalog->selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    catalog->selected_asset = asset_index;
  }
}

static void ed_catalog_view_draw_asset_buttons(ed_catalog_view_t *catalog) {
  ImGui::SameLine(ImGui::GetWindowSize().x - 20.0F);

  ImGui::PushStyleColor(ImGuiCol_Button, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ED_LIGHT_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ED_ACTIVE_GREY_COLOR);

  ImGui::PushFont(g_editor_material_symbols);

  if (ImGui::Button(ICON_MS_DELETE, ImVec2(20.0F, 0.0F))) {
    // TODO
  }

  ImGui::SameLine(ImGui::GetWindowSize().x - 40.0F);

  if (ImGui::Button(ICON_MS_MERGE, ImVec2(20.0F, 0.0F))) {
    // TODO
  }

  ImGui::PopFont();
  ImGui::PopStyleColor(3);
}
