#include <editor/ed_pch.h>

#include <editor/view/ed_profiler.h>

/*
ed_profiler_view_t *ed_profiler_view_create(eg_context_t *context) {
  ed_profiler_view_t *profiler = (ed_profiler_view_t *)eg_heap_alloc(sizeof(ed_profiler_view_t), 1, 0);

  profiler->base.context = context;
  profiler->base.is_dirty = 0;
  profiler->base.is_open = 1;
  profiler->base.is_docked = 0;

  return profiler;
}
void ed_profiler_view_refresh(ed_profiler_view_t *profiler) {
  // TODO
}
void ed_profiler_view_draw(ed_profiler_view_t *profiler) {
  static float history = 10.0F;
  static int32_t interval = 120;
  static float target_30_fps = 1.0F / 30.0F;
  static float target_60_fps = 1.0F / 60.0F;
  static float target_120_fps = 1.0F / 120.0F;

  ImGui::SetNextItemWidth(200.0F);
  ImGui::SliderFloat("History", &history, 1, 30, "%.1F s");
  ImGui::SameLine();
  ImGui::SetNextItemWidth(200.0F);
  ImGui::InputInt("Interval", &interval);

  static ImPlotFlags plot_flags =
    ImPlotFlags_NoMouseText |
    ImPlotFlags_NoTitle |
    ImPlotFlags_NoMenus |
    ImPlotFlags_NoBoxSelect;

  static ImPlotAxisFlags x_axis_flags =
    ImPlotAxisFlags_NoMenus |
    ImPlotAxisFlags_AutoFit;

  static ImPlotAxisFlags y_axis_flags =
    ImPlotAxisFlags_NoMenus;

  ImPlot::PushStyleColor(ImPlotCol_PlotBg, IM_COL32(15, 15, 15, 255));
  ImPlot::PushStyleColor(ImPlotCol_FrameBg, ED_SHALLOW_GRAY_COLOR);
  ImPlot::PushStyleColor(ImPlotCol_LegendBg, ED_SHALLOW_GRAY_COLOR);

  ImVec2 window_size = ImGui::GetWindowSize();

  if (ImPlot::BeginPlot("##Profiler", ImVec2(window_size.x, window_size.y), plot_flags)) {

    ImPlot::SetupAxes(0, 0, x_axis_flags, y_axis_flags);
    ImPlot::SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Outside);

    float time = eg_context_time(profiler->base.context);

    ImPlot::SetupAxisLimits(ImAxis_X1, time - history, time, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, -0.005F, 1.0F / 30.0F + 0.01F);

    ImPlot::SetNextLineStyle(ImVec4(0.5F, 0.0F, 0.0F, 1.0F));
    ImPlot::PlotInfLines("##30Fps", &target_30_fps, 1, ImPlotInfLinesFlags_Horizontal);

    ImPlot::SetNextLineStyle(ImVec4(0.5F, 0.5F, 0.0F, 1.0F));
    ImPlot::PlotInfLines("##60Fps", &target_60_fps, 1, ImPlotInfLinesFlags_Horizontal);

    ImPlot::SetNextLineStyle(ImVec4(0.0F, 0.5F, 0.0F, 1.0F));
    ImPlot::PlotInfLines("##120Fps", &target_120_fps, 1, ImPlotInfLinesFlags_Horizontal);

    // ImPlot::SetNextLineStyle(ImVec4(1.0F, 1.0F, 1.0F, 1.0F), 1.0f);
    //  ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5F);

    uint64_t sample_index = 0;
    uint64_t sample_count = EG_PROFILER_SAMPLE_LANE_COUNT;

    while (sample_index < sample_count) {

      if (g_profiler_cpu_sample_names[sample_index]) {

        // TODO
        // ImPlot::PlotShaded(g_profiler_cpu_sample_names[sample_index], &g_profiler_cpu_samples[sample_index][0].time, &g_profiler_cpu_samples[sample_index][0].value, PROFILER_SAMPLE_COUNT, -INFINITY, 0, (int32_t)g_profiler_frame_index, sizeof(cpu_frame_sample_t));
        ImPlot::PlotLine(g_profiler_cpu_sample_names[sample_index], &g_profiler_cpu_samples[sample_index][0].time, &g_profiler_cpu_samples[sample_index][0].delta, EG_PROFILER_SAMPLE_COUNT, 0, (int32_t)g_profiler_frame_index, sizeof(eg_cpu_frame_sample_t));
      }

      sample_index++;
    }

    ImPlot::EndPlot();
  }

  ImPlot::PopStyleColor(3);
}
void ed_profiler_view_destroy(ed_profiler_view_t *profiler) {
  eg_heap_free(profiler);
}
*/
