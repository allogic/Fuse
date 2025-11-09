#include <editor/ed_pch.h>
#include <editor/ed_profiler.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_main.h>

uint8_t g_profiler_is_open = 1;
uint8_t g_profiler_is_docked = 0;

static uint64_t selected_target_fps = 2;

void profiler_create(context_t *context) {
}
void profiler_refresh(context_t *context) {
}
void profiler_draw(context_t *context) {
  if (dockspace_begin_child("Profiler", &g_profiler_is_open, &g_profiler_is_docked)) {

    static float history = 4.0F;
    static float target_30_fps = 1.0F / 30.0F;
    static float target_60_fps = 1.0F / 60.0F;
    static float target_120_fps = 1.0F / 120.0F;

    ImGui::SetNextItemWidth(200.0F);
    ImGui::SliderFloat("History", &history, 1, 30, "%.1F s");

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
    ImPlot::PushStyleColor(ImPlotCol_FrameBg, EDITOR_DOCKING_BACKGROUND_COLOR);
    ImPlot::PushStyleColor(ImPlotCol_LegendBg, EDITOR_DOCKING_BACKGROUND_COLOR);

    if (ImPlot::BeginPlot("##Profiler", ImVec2(-1.0F, 300.0F), plot_flags)) {

      ImPlot::SetupAxes(0, 0, x_axis_flags, y_axis_flags);
      ImPlot::SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Outside);

      ImPlot::SetupAxisLimits(ImAxis_X1, context->time - history, context->time, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 1.0F / 30.0F + 0.01);

      ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5F);

      ImPlot::SetNextLineStyle(ImVec4(0.5F, 0.0F, 0.0F, 1.0F));
      ImPlot::PlotInfLines("##30Fps", &target_30_fps, 1, ImPlotInfLinesFlags_Horizontal);

      ImPlot::SetNextLineStyle(ImVec4(0.5F, 0.5F, 0.0F, 1.0F));
      ImPlot::PlotInfLines("##60Fps", &target_60_fps, 1, ImPlotInfLinesFlags_Horizontal);

      ImPlot::SetNextLineStyle(ImVec4(0.0F, 0.5F, 0.0F, 1.0F));
      ImPlot::PlotInfLines("##120Fps", &target_120_fps, 1, ImPlotInfLinesFlags_Horizontal);

      uint64_t sample_index = 0;
      uint64_t sample_count = PROFILER_SAMPLE_LANE_COUNT;

      while (sample_index < sample_count) {

        if (g_profiler_cpu_sample_names[sample_index]) {

          // TODO
          // ImPlot::PlotShaded(g_profiler_cpu_sample_names[sample_index], &g_profiler_cpu_samples[sample_index][0].time, &g_profiler_cpu_samples[sample_index][0].value, PROFILER_SAMPLE_COUNT, -INFINITY, 0, (int32_t)g_profiler_frame_index, sizeof(cpu_frame_sample_t));
          ImPlot::PlotLine(g_profiler_cpu_sample_names[sample_index], &g_profiler_cpu_samples[sample_index][0].time, &g_profiler_cpu_samples[sample_index][0].delta, PROFILER_SAMPLE_COUNT, 0, (int32_t)g_profiler_frame_index, sizeof(cpu_frame_sample_t));
        }

        sample_index++;
      }

      ImPlot::EndPlot();
    }

    ImPlot::PopStyleColor(3);

    dockspace_end_child(g_profiler_is_docked);
  }
}
void profiler_destroy(context_t *context) {
}
